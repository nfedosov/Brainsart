import sys
import numpy as np
import pickle
import mne
import matplotlib
matplotlib.use('TkAgg')

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
    
    ### ASSUME THAT MOVE AND REST DURATIONS ARE EQUAL
    
    
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

def plot_raw_data_process(eeg_data):
    
    modified_EEG = eeg_data
   
    modified_EEG.notch_filter([50.0,100.0,150.0,200.0])
    modified_EEG.filter(2.0,30.0)
            
    modified_EEG.plot(block=True)

def plot_baseline(filename):
    raw, exp_settings = load_eeg_data_from_file(filename)
    plot_raw_data_process(raw)
    

if __name__ == '__main__':    
    if(len(sys.argv) > 1):
        plot_baseline(sys.argv[1])
        