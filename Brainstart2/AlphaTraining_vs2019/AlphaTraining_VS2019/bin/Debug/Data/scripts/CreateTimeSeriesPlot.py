import sys
import matplotlib.pyplot as plt
import numpy as np
import scipy.signal as sn
import pickle
import mne
from mne.channels import make_standard_montage
from matplotlib.figure import Figure
import difflib
try:
    from mne.viz import plot_topomap
except ImportError:
    pass

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
    if 'Cond0' in exp_settings['blocks']:
        cond_N = 1
        if 'Cond1' in exp_settings['blocks']:
            cond_N = 2
        
    if cond_N > 0:
        duration1 = exp_settings['blocks']['Cond0']['duration']
        cond0_id = int(exp_settings['blocks']['Cond0']['id'])
        
    if cond_N >1:
        duration2= exp_settings['blocks']['Cond1']['duration']
        cond1_id = int(exp_settings['blocks']['Cond1']['id'])
    
    start_times = np.zeros(0, dtype = 'float')
    
    if cond_N > 0:
        where_idx = (stims == cond0_id).astype('int')
        where_idx = np.concatenate([np.zeros(1),where_idx])
        
        deriv_idx = where_idx[1:]-where_idx[:-1]
        
        start_times = np.concatenate([start_times,np.where(deriv_idx > 0)[0]/srate])
    
           
        num_cond0 = len(start_times) 
        
        if cond_N < 2:
            description = []
            for st in start_times:
                if stims[int(round((st*srate)))] == cond0_id:
                    description.append(exp_settings['blocks']['Cond0']['message'])
        
            annotations = mne.Annotations(start_times, duration1, description)
            raw.set_annotations(annotations)
        
        
        
    if cond_N > 1:
        where_idx = (stims == cond1_id).astype('int')
        where_idx = np.concatenate([np.zeros(1),where_idx])
        
        deriv_idx = where_idx[1:]-where_idx[:-1]
        
        start_times = np.concatenate([start_times,np.where(deriv_idx > 0)[0]/srate])
            
        description = []
        for st in start_times:
            if stims[int(round((st*srate)))] == cond1_id:
                description.append(exp_settings['blocks']['Cond1']['message'])
            if stims[int(round((st*srate)))] == cond0_id:
                description.append(exp_settings['blocks']['Cond0']['message'])
                
        num_cond1 = len(description)
        durations = np.concatenate([np.ones(num_cond0)*duration1,np.ones(num_cond1)*duration2])
        
        annotations = mne.Annotations(start_times, duration2, description)
        raw.set_annotations(annotations)
        
    
    return (raw, exp_settings)

def compute_ICA_spatial_filter(eeg_data, low_freq = 2, high_freq = 40):

    modified_eeg = eeg_data.copy()
    
    modified_eeg.notch_filter([50.0, 100.0, 150.0, 200.0])
    modified_eeg.filter(low_freq, high_freq)
    
    
    last_idx = (modified_eeg[0,:][0]).shape[1]
    ica = mne.preprocessing.ICA(random_state = 0)

    srate = eeg_data.info['sfreq']
    ica.fit(modified_eeg, start = int(3*srate), stop = int(last_idx-3*srate))
        
    pca_M = ica.pca_components_

    ica_M = ica.unmixing_matrix_

    unmixing_M = ica_M @ pca_M
           
    return unmixing_M

def create_spectrum_plots(eeg_data, exp_info, filters, low_freq = 2, high_freq = 40):
    events = mne.events_from_annotations(eeg_data)
        
    print(exp_info['blocks'])
    duration1 = exp_info['blocks']['Cond0']['duration']
    duration2 = exp_info['blocks']['Cond1']['duration']
    srate = eeg_data.info['sfreq']
    N_channels = len(eeg_data.info['ch_names'])

    raw_cond0 = mne.Epochs(eeg_data,events[0], tmin = 0, tmax = duration1, event_id=events[1][exp_info['blocks']['Cond0']['message']], baseline = None)
    raw_cond1 = mne.Epochs(eeg_data,events[0], tmin = 0, tmax = duration2, event_id=events[1][exp_info['blocks']['Cond1']['message']],baseline = None)
                   
    
    ic_cond0=filters@raw_cond0.get_data()
    ic_cond1=filters@raw_cond1.get_data()
    
    freqs,pxx1 = sn.welch(ic_cond0,axis = 2,fs = srate, nperseg = np.min([int(srate*10),int(duration1*srate)]))
    pxx1 = pxx1.mean(axis = 0)
    
    freqs,pxx2 = sn.welch(ic_cond1,axis = 2,fs = srate,nperseg = np.min([int(srate*10),int(duration1*srate)]))
    pxx2 = pxx2.mean(axis = 0)
    
    ind_to_plot = np.where((freqs < high_freq) * (freqs > low_freq))[0]
    
    my_dpi=96
    for i in range(N_channels):
        # собственно отрисовка графиков
        plt.clf()

        # Пока костыль! разобраться с кодировкой и брать описание условия из параметров протокола
        plt.plot(freqs[ind_to_plot], np.log10(pxx1[i,ind_to_plot]), color='r', label="Глаза открыты")
        plt.plot(freqs[ind_to_plot], np.log10(pxx2[i,ind_to_plot]), color='g', label="Глаза закрыты")
        plt.legend()
        plt.savefig(f'C:\\Users\\Fedosov\\Documents\\projects\\brainstart_final\\Brainstart2\\AlphaTraining_vs2019\\AlphaTraining_VS2019\\bin\\Debug\\Data\\temp\\Spectrum_{i}.png', dpi=my_dpi)
                            

def create_time_series_plots(eeg_data, filters):
    srate = eeg_data.info['sfreq']
    time_series = filters @ eeg_data[:,:][0]
    my_dpi=96

    for i in range(time_series.shape[0]):        
        x = np.arange(time_series.shape[1]) / srate       
        
        plt.clf()
        plt.figure(figsize=(1000/my_dpi, 200/my_dpi), dpi=my_dpi)
        plt.plot(x, time_series[i,:], linewidth=1 )
        plt.savefig(f'C:\\Users\\Fedosov\\Documents\\projects\\brainstart_final\\Brainstart2\\AlphaTraining_vs2019\\AlphaTraining_VS2019\\bin\\Debug\\Data\\temp\\Timeseries_{i}.png', dpi=my_dpi)

def create_topomap_plot(exp_settings, data, plot_type, index):
   
    montage = make_standard_montage("standard_1005")
    all_pos = montage.get_positions()['ch_pos']
    
    channel_names = exp_settings['channel_names']

    pos = np.zeros((len(channel_names),3))
    
    #names = ['O1','O2','P3','P4']
    
    mne_names = montage.ch_names
    
    names = list()
    for i in range(len(channel_names)):
        name = channel_names[i].replace('A1','').replace('A2','')
        
        print(name,difflib.get_close_matches(name, mne_names,cutoff=0.4))
        mne_name = difflib.get_close_matches(name, mne_names,cutoff =0.4)[0]
        
        pos[i] = all_pos[mne_name]
        
        names.append(mne_name)
        
    pos = pos[:,:2]

    data = np.array(data)
    
    show_names = []
    mask = np.array([name.upper() in show_names for name in names]) if names else None
    v_min, v_max = None, None
    if (data == data[0]).all():
        data[0] += 0.1
        data[1] -= 0.1
        v_min, v_max = -1, 1

    fig = Figure(figsize=(5, 4), dpi=96)
    axes = fig.add_subplot(111)
    
    a, b = plot_topomap(data, pos, axes=axes, show=False, contours=0, names=names,
                        mask=mask,
                        mask_params=dict(marker='o',
                                         markerfacecolor='w',
                                         markeredgecolor='w',
                                         linewidth=0,
                                         markersize=3))
    

    fig.savefig(f'C:\\Users\\Fedosov\\Documents\\projects\\brainstart_final\\Brainstart2\\AlphaTraining_vs2019\\AlphaTraining_VS2019\\bin\\Debug\\Data\\temp\\{plot_type}_{index}.png')    

def plot_data(filename, fit_filter = False):

    eeg_data, exp_settings = load_eeg_data_from_file(filename)

    n_channels = len(eeg_data.info['ch_names'])

    if(fit_filter):
        filters = compute_ICA_spatial_filter(eeg_data)
    else:
        filters = np.eye(n_channels)

    for ind in range(0, n_channels):
        create_topomap_plot(exp_settings, np.linalg.pinv(filters)[:,ind], 'filter', ind)

    create_time_series_plots(eeg_data,  filters)

    create_spectrum_plots(eeg_data, exp_settings, filters)
    

if __name__ == '__main__':  
    if(len(sys.argv) > 1):        
        plot_data(sys.argv[1], True)
        pass
    print(len(sys.argv))