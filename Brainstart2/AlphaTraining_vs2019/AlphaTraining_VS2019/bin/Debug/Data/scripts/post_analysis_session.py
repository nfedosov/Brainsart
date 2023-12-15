# -*- coding: utf-8 -*-
"""
Created on Thu Nov 16 11:49:16 2023

@author: Fedosov
"""

import mne

import numpy as np
import scipy.signal as sn



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


#список аргументов

def post_analysis(argv):
    
    
    raw_names =[]#[ 'C:/Users/Fedosov/Documents/projects/Brainstart/records/probe.fif']
    config_names = []#['config_30.11.2023_17.20.txt']
    
    
    for i in range(len(raw_names)):
    
        raw =mne.io.Raw(raw_names[i])
        
        data = raw[:,:][0]
        
        
        config = np.loadtxt(config_names[i])
        
    # some processing copy, loading some coeffiecents
    
    
    # on the output- envelope
    
    
    
    #num_spindles = 
    Num_sessions = 5
    
    
    #envelope = np.random.rand(nTs)
    
    
    #alpha_spindles = 
    
    #mean_alpha_power = np.mean(alpha_spindles)
    
    sess_list = [1,2,3,4,5]
    
    mean_alpha = [34.1,45.2,54.5,48.1,60.8]

    
    
    
    
    str_mean_alpha = "Mean alpha power: 207.9 uV^2"
    str_num_spindles = "Number of spindle: 54"
    
    
    
    #plt.figure()
    #plt.bar(sess_list,mean_alpha)
    #plt.xlabel('number of session')
    #plt.ylabel('number of alpha spindles')
    
    
    
    cat = np.concatenate([np.array(sess_list)[None,:],np.array(mean_alpha)[None,:]]).T
    
    df =  pd.DataFrame(data=cat, columns=['Номер сессии','Средняя мощность'])
    
    
    #sns.set(rc={'figure.figsize':(18,6)})
    sns.lmplot(x = 'Номер сессии',y='Средняя мощность', data=df, fit_reg=True, ci=95, n_boot=1000,aspect=2)
    
    
    plt.savefig(f'..\\temp\\Post_analysis_0.png', dpi=150)
           
    
    
    mean_alpha = [6.5,4.7,5.9,6.2,6.0]

    
    
    
    cat = np.concatenate([np.array(mean_alpha)[None,:],np.array(sess_list)[None,:]]).T
    
    df =  pd.DataFrame(data=cat, columns=['Номер сессии','Количество всплесков в минуту'])
    
    
    #sns.set(rc={'figure.figsize':(18,6)})
    sns.lmplot(x = 'Номер сессии',y='Количество всплесков в минуту', data=df, fit_reg=True, ci=95, n_boot=1000,aspect=2, line_kws={'color': 'red'})
    
    
    plt.savefig(f'..\\temp\\Post_analysis_1.png', dpi=150)
           
    
    
    
    
    
    
    
    for i in range(len(argv)):
        pass
   
    


if __name__ == '__main__':
    # проверяем входные параметры
    print("function enter")
    #if(len(sys.argv) > 2):
    #    print("call RunPreparedProtocol")
    post_analysis(sys.argv)
    #else:
    #    print("Invalid number of parameters")

    
    








