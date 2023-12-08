
# -*- coding: utf-8 -*-
"""
Created on Mon Mar 13 20:56:58 2023

@author: Fedosov
"""
import mne
import matplotlib.pyplot as plt
import numpy as np
import sys

import seaborn as sns
import pandas as pd
import scipy.signal as sn




class BaseKalman:
    def __init__(self, H, Phi, Q, R):
        
        self.x = np.zeros((Phi.shape[0],1))
        self.P = np.eye(Phi.shape[0])
        
        self.Q = Q
        self.R = R
        self.H = H
        self.Phi = Phi
        
        
        
    def step(self, y):
        self.x_ = self.Phi@self.x
        
        self.P_ = self.Phi@self.P@self.Phi.T+self.Q
        
        self.res = y-self.H@self.x_
        
        self.S = self.H@self.P_@self.H.T+self.R
        
        self.K = self.P_@self.H.T/self.S
        
        self.x = self.x_+self.K@self.res
        
        self.P = (np.eye(self.P.shape[0])-self.K@self.H)@self.P_
        #print(self.P[0,0])
        
        
    
        
        
        
class WhiteKalman(BaseKalman):
    def __init__(self, freq0, A, srate, q, r):
        
        self.freq0 = freq0
        self.A = A
        H = np.array([[1.0,0.0]])
        Phi = A*np.array([[np.cos(2.0*np.pi*freq0/srate),-np.sin(2.0*np.pi*freq0/srate)],[np.sin(2.0*np.pi*freq0/srate),np.cos(2.0*np.pi*freq0/srate)]])
        
        self.Q = np.eye(2)*q
        self.R = r
        
        
        
        super().__init__(H, Phi,self.Q, self.R)
        
        
   
    def apply(self, y):
        
        Len = y.shape[0]
        filtered_real =np.zeros(Len)
        filtered_imag = np.zeros(Len)
        envelope =np.zeros(Len)
        phase = np.zeros(Len)
        complex_signal = np.zeros(Len, dtype = 'complex')
        
        for i in range(Len):
            self.step(y[i])
            #filtered_real[i] = self.x[0]
            #filtered_imag[i] = self.x[1]
            #envelope[i] = np.sqrt(self.x[0]**2+self.x[1]**2)
            #phase[i] = np.angle(self.x[0]+self.x[1]*1j)
            complex_signal[i] = self.x[0]+self.x[1]*1j
            
        return complex_signal#,filtered_real, filtered_imag, envelope, phase
       
    
    

       
       
       
       
class CFIR_firbased:
    def __init__(self, band,numtaps,srate):
        self.band = band
        self.srate = srate
      
        
        self.numtaps = numtaps
        
        self.b = sn.firwin(self.numtaps, cutoff = self.band, window='hamming', pass_zero=False, fs=self.srate)  
        self.b=self.b/np.linalg.norm(self.b)
        
        self.b_cfir = sn.hilbert(self.b)
        
        
        
    #def step(self):
    #    pass
        
    def apply(self,y):
        filtered = sn.lfilter(self.b_cfir, 1.0,y)
        
        return filtered
        





#список аргументов

def post_analysis(raw_names):
    
    
    Num_sessions = len(raw_names)
    

    
    
    #[ 'C:/Users/Fedosov/Documents/projects/Brainstart/records/probe.fif']
    
    mean_alpha = list()
    max_alpha = list()
    count_alpha = list()
    
    
    sess_list = list()
    
    
    for i in range(Num_sessions):
    
        raw =mne.io.Raw(raw_names[i])
        
        data = raw[:,:][0]
        
        print(data.shape)
        
        
        # пока костыль. Сделать нормальную замену
        config_name = raw_names[i].replace('probe_','config_').replace('.fif','.txt')
        
        config = np.loadtxt(config_name, delimiter = ', ')
        
        print(config)
        
        Nch = config[0]
        filter_type = config[1]
        to_prefilter = config[2]
        central_freq = config[3]
        
        q0 = config[4]
        q1 = config[5]
        
        if filter_type == 0:
            q = config[6]
            r = config[7]
            
            srate = config[8]
            
            spat_filter = config[9:9+int(Nch)]
            
            
            kalman = WhiteKalman(central_freq, 0.995, srate, q, r)

            
        
            
        if filter_type == 1:
            window_sz = config[6]
            
            srate = config[7]
            
            spat_filter = config[9:9+int(Nch)]
            
            
            cfir = CFIR_firbased([central_freq-1.0,central_freq+1.0],window_sz,srate)

        
   
        
        
        if to_prefilter:
            
        
            b_low, a_low = sn.butter(2,50.0,'low',fs = srate)
            
            b50, a50 = sn.butter(4,[48.0,52.0],'bandstop',fs = srate)
            
            b_high, a_high = sn.butter(2,4.0,'high',fs = srate)
            
            b100, a100 = sn.butter(2,[98.0,102.0],'bandstop',fs = srate)
            b150, a150 = sn.butter(2,[148.0,152.0],'bandstop',fs = srate)
            b200, a200 = sn.butter(2,[198.0,202.0],'bandstop',fs = srate)
            
            filtered_eeg = spat_filter@data
             
            filtered_eeg = sn.lfilter(b_low, a_low, filtered_eeg)
            filtered_eeg = sn.lfilter(b_high, a_high, filtered_eeg)
            filtered_eeg = sn.lfilter(b50, a50, filtered_eeg)
            filtered_eeg = sn.lfilter(b100, a100, filtered_eeg)
            filtered_eeg = sn.lfilter(b150, a150, filtered_eeg)
            filtered_eeg = sn.lfilter(b200, a200, filtered_eeg)
            
            
        else:
            filtered_eeg = spat_filter@data
        
        
             
            
        

  
        if filter_type == 0:
            cmplx_filtered_eeg = kalman.apply(filtered_eeg)
            
        if filter_type == 1:
            cmplx_filtered_eeg = cfir.apply(filtered_eeg)
            
            
            

        envelope = np.abs(cmplx_filtered_eeg)
        
        
        sess_list.append(i)
        
        
        mean_alpha.append(np.mean(envelope))
        max_alpha.append(np.max(envelope))
        
        exceeds = (envelope>q1)
        diff = exceeds[1:].astype(int)-exceeds[:-1].astype(int)
        
        count_alpha.append(np.sum(np.isclose(diff,-1)))
    

    
    cat = np.concatenate([np.array(sess_list)[None,:],np.array(mean_alpha)[None,:]]).T
    
    df =  pd.DataFrame(data=cat, columns=['Номер сессии','Средняя мощность'])
    
    
    #sns.set(rc={'figure.figsize':(18,6)})
    sns.lmplot(x = 'Номер сессии',y='Средняя мощность', data=df, fit_reg=True, ci=95, n_boot=1000,aspect=2)
    
    
    plt.savefig(f'C:\\Users\\Fedosov\\Documents\\projects\\brainstart_final\\Brainstart2\\AlphaTraining_vs2019\\AlphaTraining_VS2019\\bin\\Debug\\Data\\Post_analysis_0.png', dpi=150)

    
    
    
    
    cat = np.concatenate([np.array(mean_alpha)[None,:],np.array(sess_list)[None,:]]).T
    
    df =  pd.DataFrame(data=cat, columns=['Номер сессии','Количество всплесков в минуту'])
    
    
    #sns.set(rc={'figure.figsize':(18,6)})
    sns.lmplot(x = 'Номер сессии',y='Количество всплесков в минуту', data=df, fit_reg=True, ci=95, n_boot=1000,aspect=2, line_kws={'color': 'red'})
    
    
    plt.savefig(f'C:\\Users\\Fedosov\\Documents\\projects\\brainstart_final\\Brainstart2\\AlphaTraining_vs2019\\AlphaTraining_VS2019\\bin\\Debug\\Data\\Post_analysis_1.png', dpi=150)
           
    
    
    


if __name__ == '__main__':
    # проверяем входные параметры
    print(len(sys.argv))
    #if(len(sys.argv) > 2):
    #    print("call RunPreparedProtocol")
    post_analysis(sys.argv[1:len(sys.argv)])
    #else:
    #    print("Invalid number of parameters")

    
    








