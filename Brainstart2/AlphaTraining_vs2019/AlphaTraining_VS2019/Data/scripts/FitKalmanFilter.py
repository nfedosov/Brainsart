import numpy as np
import mne
import pickle
from utils_EEG_analysis import WhiteKalman
import sys

def load_eeg_data_from_file(filename):
    file = open(filename, "rb")
    container =  pickle.load(file)
    file.close()

    exp_settings = container['exp_settings']
    srate = exp_settings['srate']
    data = container['eeg']
    stims = container['stim']
    
    channel_names = exp_settings['channel_names']
    
    for i in range(len(channel_names)):
        channel_names[i] = channel_names[i].upper()
        print(channel_names)
        n_channels = len(channel_names)

    info = mne.create_info(ch_names=channel_names, sfreq = srate, ch_types = 'eeg',)
    raw =  mne.io.RawArray(data.T, info)
    
       
    # condition1 , condition2, prepare, ready    
    
    cond_N = 0
    if 'Cond1' in exp_settings['blocks']:
        cond_N = 1
        if 'Cond2' in exp_settings['blocks']:
            cond_N = 2
        
    if cond_N > 0:
        duration1 = exp_settings['blocks']['Cond1']['duration']
        cond1_id = int(exp_settings['blocks']['Cond1']['id'])
        
    if cond_N >1:
        duration2= exp_settings['blocks']['Cond2']['duration']
        cond2_id = int(exp_settings['blocks']['Cond2']['id'])
    
    start_times = np.zeros(0, dtype = 'float')
    
    if cond_N > 0:
        where_idx = (stims == cond1_id).astype('int')
        where_idx = np.concatenate([np.zeros(1),where_idx])
        
        deriv_idx = where_idx[1:]-where_idx[:-1]
        
        start_times = np.concatenate([start_times,np.where(deriv_idx > 0)[0]/srate])
    
           
        num_cond1 = len(start_times) 
        
        if cond_N < 2:
            description = []
            for st in start_times:
                if stims[int(round((st*srate)))] == cond1_id:
                    description.append(exp_settings['blocks']['Cond1']['message'])
        
            annotations = mne.Annotations(start_times, duration1, description)
            raw.set_annotations(annotations)
        
        
        
    if cond_N > 1:
        where_idx = (stims == cond2_id).astype('int')
        where_idx = np.concatenate([np.zeros(1),where_idx])
        
        deriv_idx = where_idx[1:]-where_idx[:-1]
        
        start_times = np.concatenate([start_times,np.where(deriv_idx > 0)[0]/srate])
            
        description = []
        for st in start_times:
            if stims[int(round((st*srate)))] == cond2_id:
                description.append(exp_settings['blocks']['Cond2']['message'])
            if stims[int(round((st*srate)))] == cond1_id:
                description.append(exp_settings['blocks']['Cond1']['message'])
                
        num_cond2 = len(description)
        durations = np.concatenate([np.ones(num_cond1)*duration1,np.ones(num_cond2)*duration2])
        
        annotations = mne.Annotations(start_times, duration2, description)
        raw.set_annotations(annotations)
    
    return (raw, exp_settings)


def auto_calc_f0():
    return 10.5

def compute_ICA_spatial_filter(eeg_data, low_freq = 2, high_freq = 40):

    modified_eeg = eeg_data.copy()
    
    modified_eeg.notch_filter([50.0, 100.0, 150.0, 200.0])
    modified_eeg.filter(low_freq, high_freq)
    
    
    last_idx = (modified_eeg[0,:][0]).shape[1]
    ica = mne.preprocessing.ICA()

    srate = eeg_data.info['sfreq']
    ica.fit(modified_eeg, start = int(5*srate), stop = int(last_idx-5*srate))
           
    return ica

def fit_kalman_filter(eeg_data, component_num, config_file_path):

    srate = eeg_data.info['sfreq']

    # Проинициализировать ICA фильтр
    ica = compute_ICA_spatial_filter(eeg_data)

    pca_M = ica.pca_components_
    ica_M = ica.unmixing_matrix_
    unmixing_M = ica_M@pca_M

    spat_filter = unmixing_M[component_num,:]


    filtered_eeg = eeg_data.copy()
    gt_eeg = eeg_data.copy()

    gt_eeg = spat_filter @ gt_eeg[:,:][0]
    filtered_eeg = spat_filter @ filtered_eeg[:,:][0]

    central_freq = auto_calc_f0()

    default_relatio = 1000
    relatio = default_relatio
    A = 0.995
    q = 1.0 * relatio
    r = 1.0
    kalman = WhiteKalman(central_freq, A, srate, q, r)

    cmplx_filtered_eeg = kalman.apply(filtered_eeg)

    envelope = np.abs(cmplx_filtered_eeg)
    arr = np.quantile(envelope,[0.05, 0.095])

    q0 = arr[0]
    q1 = arr[1]

    combined_array = np.concatenate((np.array([central_freq]),np.array([q0]), np.array([q1]), spat_filter))
    
    # Write the array to a text file with comma+space delimiters
    with open(config_file_path, 'w') as file:
        # here dump, then, in another script - load and change the phase                
        file.write(', '.join(str(x) for x in combined_array))

def fit_kalman_filter_for_file(file_name, component_num, config_file_name):
    eeg_data, exp_settings = load_eeg_data_from_file(sys.argv[1], True)
    fit_kalman_filter(eeg_data)

if __name__ == '__main__':    
    if(len(sys.argv) > 3):
        eeg_data, exp_settings = load_eeg_data_from_file(sys.argv[1])
        component_num = int(sys.argv[2])
        config_file_name = sys.argv[3]
        fit_kalman_filter(eeg_data, component_num, config_file_name)