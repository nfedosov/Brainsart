# -*- coding: utf-8 -*-
"""
Created on Thu Nov 30 12:08:53 2023

@author: Fedosov
"""

import scipy.signal as sn


class CFIR_firbased:
    def __init__(self, band,lensec,srate,window = 'hamming'):
        self.band = band
        self.srate = srate
        self.window = window
        self.lensec = lensec
        
        self.numtaps = self.lensec()
        
        self.b = sn.firwin(self.numtaps, cutoff = self.band, window=self.window, pass_zero=False, fs=self.srate)  
        self.b_cfir = sn.hilbert(self.b)
        
        
        
    def step(self):
        pass
        
    def apply(self,y):
        filtered = sn.lfilter(self.b_cfir, 1.0,y)
        
        return filtered






def fit_cfir_filter(eeg_data, component_num, config_file_path):

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
    win_len = 1 * relatio
  
    cfir = CFIR(central_freq, A, srate, q, r)

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