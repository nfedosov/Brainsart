import sys
import matplotlib.pyplot as plt
import numpy as np
import scipy.signal as sn
import pickle
import mne

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

def compute_ICA_spatial_filter(eeg_data, low_freq = 2, high_freq = 40):

    modified_eeg = eeg_data.copy()
    
    modified_eeg.notch_filter([50.0, 100.0, 150.0, 200.0])
    modified_eeg.filter(low_freq, high_freq)
    
    
    last_idx = (modified_eeg[0,:][0]).shape[1]
    ica = mne.preprocessing.ICA()

    srate = eeg_data.info['sfreq']
    ica.fit(modified_eeg, start = int(5*srate), stop = int(last_idx-5*srate))
        
    pca_M = ica.pca_components_

    ica_M = ica.unmixing_matrix_

    unmixing_M = ica_M @ pca_M
           
    return unmixing_M

def create_spectrum_plots(eeg_data, exp_info, filters, low_freq = 2, high_freq = 40):
    events = mne.events_from_annotations(eeg_data)
        
    duration1 = exp_info['blocks']['Cond1']['duration']
    duration2 = exp_info['blocks']['Cond2']['duration']
    srate = eeg_data.info['sfreq']
    N_rows = len(eeg_data.info['ch_names'])

    raw_cond1 = mne.Epochs(eeg_data,events[0], tmin = 0, tmax = duration1-5.0, event_id=events[1][exp_info['blocks']['Cond1']['message']], baseline = None)
    raw_cond2 = mne.Epochs(eeg_data,events[0], tmin = 5.0, tmax = duration2, event_id=events[1][exp_info['blocks']['Cond2']['message']],baseline = None)
                   
    
    ic_cond1=filters@raw_cond1.get_data()
    ic_cond2=filters@raw_cond2.get_data()
    
    freqs,pxx1 = sn.welch(ic_cond1,axis = 2,fs = srate, nperseg = np.min([int(srate*10),int(duration1*srate)]))
    pxx1 = pxx1.mean(axis = 0)
    
    freqs,pxx2 = sn.welch(ic_cond2,axis = 2,fs = srate,nperseg = np.min([int(srate*10),int(duration1*srate)]))
    pxx2 = pxx2.mean(axis = 0)
    
    ind_to_plot = np.where((freqs < high_freq) * (freqs > low_freq))[0]
    
    my_dpi=96
    for i in range(N_rows):
        # собственно отрисовка графиков
        plt.clf()
        plt.plot(freqs[ind_to_plot], np.log10(pxx1[i,ind_to_plot]), color='r')
        plt.plot(freqs[ind_to_plot], np.log10(pxx2[i,ind_to_plot]), color='g')
        plt.savefig(f'Data\\temp\\Spectrum_{i}.png', dpi=my_dpi)
                            

def create_time_series_plots(eeg_data, filters):
    srate = eeg_data.info['sfreq']
    time_series = filters @ eeg_data[:,:][0]
    my_dpi=96

    for i in range(time_series.shape[0]):        
        x = np.arange(time_series.shape[1]) / srate       
        
        plt.clf()
        plt.figure(figsize=(1000/my_dpi, 200/my_dpi), dpi=my_dpi)
        plt.plot(x, time_series[i,:], linewidth=1 )
        plt.savefig(f'Data\\temp\\Timeseries_{i}.png', dpi=my_dpi)

def plot_data(filename, fit_filter = False):


    eeg_data, exp_settings = load_eeg_data_from_file(filename)   

    n_channels = len(eeg_data.info['ch_names'])

    if(fit_filter):
        filters = compute_ICA_spatial_filter(eeg_data)
    else:
        filters = np.eye(n_channels)

    create_time_series_plots(eeg_data,  filters)

    create_spectrum_plots(eeg_data, exp_settings, filters)

if __name__ == '__main__':    
    if(len(sys.argv) > 1):
        plot_data(sys.argv[1], True)
