import sys
import matplotlib.pyplot as plt
import numpy as np
import pickle
import mne
import scipy.signal as sn
import scipy.stats as st





from PyQt5.QtWidgets import QApplication, QMainWindow, QWidget, \
        QVBoxLayout, QPushButton, QLabel, QFileDialog, QDialog,\
        QLineEdit, QComboBox, QHBoxLayout, QCheckBox, QTableWidget,\
        QHeaderView, QButtonGroup, QRadioButton, QStackedLayout
        
from PyQt5.QtCore import Qt
        
from PyQt5.QtGui import QImage, QPixmap
        
        
from pyqtgraph import PlotWidget, mkPen




from utils_EEG_analysis import WhiteKalman, TopoFilterCavas



class CFIRFit(QWidget):

    def __init__(self, eeg_data, exp_info, spatial_filter_dict):
        
        super().__init__()
        self.init_ui()
        
        self.eeg_data = eeg_data
        
        self.srate = self.eeg_data.info['sfreq']
        
        self.exp_info = exp_info
        
        self.spatial_filter_dict = spatial_filter_dict
        self.ica = self.spatial_filter_dict['ica']
        self.spat_filter = self.spatial_filter_dict['filter_coef']
        self.num_component = self.spatial_filter_dict['num_component']
        
        self.duration = exp_info['blocks']['Open']['duration']
        
        
        self.f_high = 2.0
        self.b_high,self.a_high = sn.butter(1,2.0, btype = 'high',fs = self.srate)
    
        self.f_low = 40.0
        self.b_low,self.a_low = sn.butter(1,40.0, btype = 'low',fs = self.srate)
    
        
    
        self.f_50 = 50.0
        self.b_50,self.a_50 = sn.butter(2,[48.0,52.0], btype = 'bandstop',fs = self.srate)
        
        self.f_100 = 100.0
        self.b_100,self.a_100 = sn.butter(1,[97.0,103.0], btype = 'bandstop',fs = self.srate)
        
        self.f_150 = 150.0
        self.b_150,self.a_150 = sn.butter(1,[146.0,154.0], btype = 'bandstop',fs = self.srate)
        
        self.f_200 = 200.0
        self.b_200,self.a_200 = sn.butter(1,[195.0,205.0], btype = 'bandstop',fs = self.srate)
        
        self.central_freq = None
        
        self.alpha_range = [9.0,12.0]
        self.beta_range = [18.0,24.0]
        self.range = self.alpha_range
        
        
        self.default_relatio = 10000
        self.relatio = self.default_relatio
        

    def init_ui(self):
        self.setWindowTitle('White Kalman Filter')
        self.setGeometry(200, 200, 400, 200)

        self.layout = QVBoxLayout()
        #label = QLabel('This is a pop-out widget!')
        
        
        self.plot_raw_psd_button = QPushButton('Plot source raw psd')
        self.plot_raw_psd_button.clicked.connect(self.plot_raw_psd)
        self.layout.addWidget(self.plot_raw_psd_button)
       
        
        
        
        central_freq_label = QLabel('set central frequency manually')
        self.layout.addWidget(central_freq_label)
      
        self.set_central_freq = QLineEdit()
        self.layout.addWidget(self.set_central_freq)
        self.set_central_freq.textChanged.connect(self.change_f0)

        


        self.band_choice = QComboBox(self)
        self.band_choice.setGeometry(10, 50, 150, 30)

        # Add choices to the drop-down list
        choices = ['alpha','beta']
        self.band_choice.addItems(choices)

        # Connect a slot (function) to handle the choice selection
        self.band_choice.currentIndexChanged.connect(self.handle_band_selection)
        self.layout.addWidget(self.band_choice)
        

        self.automatic_f0_button = QPushButton('Automatically find central freq')
        self.automatic_f0_button.clicked.connect(self.auto_calc_f0)
        self.layout.addWidget(self.automatic_f0_button)
        
        
        
        relatio_label = QLabel('Set the relative value of observation noise to process noise')
        self.layout.addWidget(relatio_label)
        self.set_relatio = QLineEdit()
        self.layout.addWidget(self.set_relatio)
        self.set_relatio.textChanged.connect(self.relatio_change)   
     
        self.setLayout(self.layout)
        
        self.fit_kf_button = QPushButton('Fit kalman filter')
        self.fit_kf_button.clicked.connect(self.fit_kalman_filter)
        self.layout.addWidget(self.fit_kf_button)
        
        
        
                
    def relatio_change(self,text):

        # This slot is called whenever the text in the QLineEdit changes
        self.relatio = float(text)
        
        self.q = 0.01
        self.r = 0.01*self.relatio
        
        

    def fit_kalman_filter(self):
        
        self.filtered_eeg = self.eeg_data.copy()
        self.gt_eeg = self.eeg_data.copy()
        
        self.gt_eeg = self.spat_filter @self.gt_eeg[:,:][0]
        
        
        self.filtered_eeg= self.spat_filter @self.filtered_eeg[:,:][0]
        self.kalman = WhiteKalman(self.central_freq, self.A, self.srate, self.q, self.r)

        
        self.cmplx_filtered_eeg = self.kalman.apply(self.filtered_eeg)
        
        envelope = np.abs(self.cmplx_filtered_eeg)
        
        arr = np.quantile(envelope,[0.05,0.095])
        
        
        self.q0 = arr[0]
        self.q1 = arr[1]
        
        print(self.q0)
        
    
    def plot_corr_lat(self,gt, filtered, time_w = 0.2):
        
        halfW = int(self.srate*time_w)
        
        env_latency = np.zeros(halfW)
        phase_latency = np.zeros(halfW)
        
        for i in range(halfW):
            shifted_gt = np.roll(gt,i)
            env_latency[i] = st.pearsonr(np.abs(shifted_gt), np.abs(self.cmplx_filtered_eeg))[0]
            phase_latency[i] = st.pearsonr(np.angle(shifted_gt), np.angle(self.cmplx_filtered_eeg))[0]
            
        plt.figure()
        plt.plot(env_latency)
        plt.figure()
        plt.plot(phase_latency)
        plt.show()

    def handle_band_selection(self):
        selected_choice = self.band_choice.currentText()
        
        
        if selected_choice == 'alpha':
            self.range = self.alpha_range
        
        if selected_choice == 'beta':
            self.range = self.beta_range
        
            


        
    def auto_calc_f0(self):
        
        ics = self.ica.get_sources(self.eeg_data)
        ic= ics.pick(self.num_component)
        
        
        events = mne.events_from_annotations(self.eeg_data)

        ic_cond1 = mne.Epochs(ic,events[0], tmin = 0, tmax = self.duration, event_id = events[1][self.exp_info['blocks']['Cond1']['message']], baseline = None)
        ic_cond2 = mne.Epochs(ic,events[0], tmin = 0, tmax = self.duration, event_id=events[1][self.exp_info['blocks']['Cond2']['message']],baseline = None)
        
        
        
        psd_cond1 = ic_cond1.compute_psd(fmin = self.range[0], picks = [0],fmax = self.range[1]).get_data(return_freqs = True)
      
        psd_cond2 = ic_cond2.compute_psd(fmin = self.range[0],picks = [0], fmax = self.range[1]).get_data(return_freqs = True)
      
        
        rel_psd = np.mean(psd_cond1[0])/np.mean(psd_cond2[0])
        
        self.central_freq= psd_cond1[1][np.argmax(np.mean(psd_cond1[0],axis = 0))]
      
        self.set_central_freq.setText(str(self.central_freq))
        
        
        
        
    def plot_raw_psd(self):
        
        #if num events == 2
        
        ics = self.ica.get_sources(self.eeg_data)
        ic= ics.pick(self.num_component)
        
        

        self.ica.plot_sources(self.eeg_data, picks = self.num_component)  #CHECK THERE are
        
        #if ... there are binary events ??? if not, just plot spectrs
        events = mne.events_from_annotations(self.eeg_data)

        ic_cond1 = mne.Epochs(ic,events[0], tmin = 0, tmax = self.duration, event_id = events[1][self.exp_info['blocks']['Cond1']['message']], baseline = None)
        ic_cond2 = mne.Epochs(ic,events[0], tmin = 0, tmax = self.duration, event_id=events[1][self.exp_info['blocks']['Cond2']['message']],baseline = None)
        
        
        #rel_alphas =np.zeros(n_channels)
        #central_freq_alphas= np.zeros(n_channels) #CHECK only for automatic identfication
        
        #plt.figure()
        fig,ax = plt.subplots()
        
    

        ic_cond1.plot_psd(ax = ax, picks = [0],color = 'red', spatial_colors = False,fmin = 2.0,fmax = 30)
        ic_cond2.plot_psd(ax = ax, picks = [0],color = 'blue', spatial_colors = False,fmin  = 2.0,fmax = 30)
        plt.show()
       
    def change_f0(self,text):

        # This slot is called whenever the text in the QLineEdit changes
        self.central_freq = float(text)

      

class WhiteKalmanFit(QWidget):

    def __init__(self, eeg_data, exp_info, spatial_filter_dict):
        
        super().__init__()
        self.init_ui()
        
        
        
        self.eeg_data = eeg_data
        
        self.srate = self.eeg_data.info['sfreq']
        
        self.exp_info = exp_info
        
        
        self.cond_N = 0
        if 'Cond1' in self.exp_info['blocks']:
            self.cond_N = 1
            self.duration1 = exp_info['blocks']['Cond1']['duration']
            if 'Cond2' in self.exp_info['blocks']:
                self.cond_N = 2
                self.duration2 = exp_info['blocks']['Cond2']['duration']
      
        
        
        self.spatial_filter_dict = spatial_filter_dict
        self.ica = self.spatial_filter_dict['ica']
        self.spat_filter = self.spatial_filter_dict['filter_coef']
        self.num_component = self.spatial_filter_dict['num_component']
        
        #self.duration = exp_info['blocks']['Open']['duration']
        
        '''
        self.a_low = None
        self.b_low = None
        
        self.a_high = None
        self.b_high = None
        
        self.a_50 = None
        self.b_50 = None
        self.a_100 = None
        self.b_100 = None
        self.a_150 = None
        self.b_150 = None
        self.a_200 = None
        self.b_200 = None'''
        
        self.f_high = 2.0
        self.b_high,self.a_high = sn.butter(1,2.0, btype = 'high',fs = self.srate)
    
        self.f_low = 40.0
        self.b_low,self.a_low = sn.butter(1,40.0, btype = 'low',fs = self.srate)
    
        '''
    
        self.f_50 = 50.0
        self.b_50,self.a_50 = sn.butter(2,[48.0,52.0], btype = 'bandstop',fs = self.srate)
        
        self.f_100 = 100.0
        self.b_100,self.a_100 = sn.butter(1,[97.0,103.0], btype = 'bandstop',fs = self.srate)
        
        self.f_150 = 150.0
        self.b_150,self.a_150 = sn.butter(1,[146.0,154.0], btype = 'bandstop',fs = self.srate)
        
        self.f_200 = 200.0
        self.b_200,self.a_200 = sn.butter(1,[195.0,205.0], btype = 'bandstop',fs = self.srate)
        
        '''
        
        
        self.central_freq = None
        
        self.alpha_range = [9.0,12.0]
        self.beta_range = [18.0,24.0]
        self.range = self.alpha_range
        
        
        self.default_relatio = 1000
        self.A = 0.995
        self.relatio = self.default_relatio
        
        
        self.q = 1.0*self.relatio
        self.r = 1.0
        
        
        
        
        
        
        

    def init_ui(self):
        self.setWindowTitle('White Kalman Filter')
        self.setGeometry(200, 200, 400, 200)

        self.layout = QVBoxLayout()
        #label = QLabel('This is a pop-out widget!')
        
        
        self.plot_raw_psd_button = QPushButton('Plot source raw psd')
        self.plot_raw_psd_button.clicked.connect(self.plot_raw_psd)
        self.layout.addWidget(self.plot_raw_psd_button)
       
        
        
        
        central_freq_label = QLabel('set central frequency manually')
        self.layout.addWidget(central_freq_label)
      
        self.set_central_freq = QLineEdit()
        self.layout.addWidget(self.set_central_freq)
        self.set_central_freq.textChanged.connect(self.change_f0)

        


        self.band_choice = QComboBox(self)
        self.band_choice.setGeometry(10, 50, 150, 30)

        # Add choices to the drop-down list
        choices = ['alpha','beta']
        self.band_choice.addItems(choices)

        # Connect a slot (function) to handle the choice selection
        self.band_choice.currentIndexChanged.connect(self.handle_band_selection)
        self.layout.addWidget(self.band_choice)
        


        
        
        self.automatic_f0_button = QPushButton('Automatically find central freq')
        self.automatic_f0_button.clicked.connect(self.auto_calc_f0)
        self.layout.addWidget(self.automatic_f0_button)
        
        
        
        relatio_label = QLabel('Set the relative value of observation noise to process noise')
        self.layout.addWidget(relatio_label)
        self.set_relatio = QLineEdit()
        self.layout.addWidget(self.set_relatio)
        self.set_relatio.textChanged.connect(self.relatio_change)   
     
        self.setLayout(self.layout)
        
        self.fit_kf_button = QPushButton('Fit kalman filter')
        self.fit_kf_button.clicked.connect(self.fit_kalman_filter)
        self.layout.addWidget(self.fit_kf_button)
        
        
        
    def relatio_change(self,text):

        # This slot is called whenever the text in the QLineEdit changes
        self.relatio = float(text)
        
        self.q = 1.0*self.relatio
        self.r = 1.0
        
        

    def fit_kalman_filter(self):
        
        self.filtered_eeg = self.eeg_data.copy()
        self.gt_eeg = self.eeg_data.copy()
        
        self.gt_eeg = self.spat_filter @self.gt_eeg[:,:][0]
        
        
        self.filtered_eeg= self.spat_filter @self.filtered_eeg[:,:][0]
        
        ####
        
        b, a = sn.butter(3,[self.central_freq-2.0,self.central_freq+2.0],
                         btype = 'bandpass',fs = self.srate)
        self.gt_eeg = sn.filtfilt(b,a,self.gt_eeg)
        
        b50, a50 = sn.butter(3,[48.0,52.0],btype = 'bandstop',
                         fs = self.srate)
        self.gt_eeg = sn.filtfilt(b50,a50,self.gt_eeg)
        
        
        self.cmplx_gt_eeg = sn.hilbert(self.gt_eeg)
       
        
        self.kalman = WhiteKalman(self.central_freq, self.A, self.srate, self.q, self.r)
        
        
        self.cmplx_filtered_eeg = self.kalman.apply(self.filtered_eeg)
        
        
        self.cmplx_filtered_eeg = self.kalman.apply(self.filtered_eeg)
        
        envelope = np.abs(self.cmplx_filtered_eeg)
        
        arr = np.quantile(envelope,[0.05,0.095])
        
        
        self.q0 = arr[0]
        self.q1 = arr[1]
        
        print(self.q0)
        
        
    
    def plot_corr_lat(self,gt, filtered, time_w = 0.2):
        
        halfW = int(self.srate*time_w)
        
        env_latency = np.zeros(halfW)
        phase_latency = np.zeros(halfW)
        
        for i in range(halfW):
            shifted_gt = np.roll(gt,i)
            env_latency[i] = st.pearsonr(np.abs(shifted_gt), np.abs(self.cmplx_filtered_eeg))[0]
            phase_latency[i] = st.pearsonr(np.angle(shifted_gt), np.angle(self.cmplx_filtered_eeg))[0]
            
        plt.figure()
        plt.plot(env_latency)
        plt.figure()
        plt.plot(phase_latency)
        plt.show()

    def handle_band_selection(self):
        selected_choice = self.band_choice.currentText()
        
        
        if selected_choice == 'alpha':
            self.range = self.alpha_range
        
        if selected_choice == 'beta':
            self.range = self.beta_range
        
            


        
    def auto_calc_f0(self):
        
        ics = self.ica.get_sources(self.eeg_data)
        ic= ics.pick(self.num_component)
        
        
        events = mne.events_from_annotations(self.eeg_data)

        #ic_cond1 = mne.Epochs(ic,events[0], tmin = 0, tmax = self.duration1, event_id = events[1][self.exp_info['blocks']['Cond1']['message']], baseline = None)
        #ic_cond2 = mne.Epochs(ic,events[0], tmin = 0, tmax = self.duration2, event_id=events[1][self.exp_info['blocks']['Cond2']['message']],baseline = None)
        
        raw_cond1 = mne.Epochs(self.eeg_data,events[0], tmin = 0, tmax = self.duration1-5.0, event_id=events[1][self.exp_info['blocks']['Cond1']['message']], baseline = None)
        raw_cond2 = mne.Epochs(self.eeg_data,events[0], tmin = 5.0, tmax = self.duration2, event_id=events[1][self.exp_info['blocks']['Cond2']['message']],baseline = None)
        
        
        ic_cond1=self.spat_filter[None,:]@raw_cond1.get_data()
        ic_cond2=self.spat_filter[None,:]@raw_cond2.get_data()
        
        freqs,pxx1 = sn.welch(ic_cond1,axis = 2,fs = self.srate,nperseg = np.min([int(self.srate*10),int(self.duration1*self.srate)]))
        pxx1 = pxx1.mean(axis = 0)
        
        freqs,pxx2 = sn.welch(ic_cond2,axis = 2,fs = self.srate,nperseg = np.min([int(self.srate*10),int(self.duration1*self.srate)]))
        pxx2 = pxx2.mean(axis = 0)
        
        self.central_freq = 10.5
        
        #psd_cond1 = ic_cond1.compute_psd(fmin = self.range[0], picks = [0],fmax = self.range[1]).get_data(return_freqs = True)
      
        #psd_cond2 = ic_cond2.compute_psd(fmin = self.range[0],picks = [0], fmax = self.range[1]).get_data(return_freqs = True)
      
        
        #self.central_freq= freqs[np.argmax(np.mean(psd_cond1[0],axis = 0))]
      
        
          
        self.set_central_freq.setText(str(self.central_freq))
        
        
        
        
    def plot_raw_psd(self):
        
        #if num events == 2
        
        ics = self.ica.get_sources(self.eeg_data)
        ic= ics.pick(self.num_component)
        
        

        self.ica.plot_sources(self.eeg_data, picks = self.num_component)  #CHECK THERE are
        
        #if ... there are binary events ??? if not, just plot spectrs
        events = mne.events_from_annotations(self.eeg_data)

        ic_cond1 = mne.Epochs(ic,events[0], tmin = 0, tmax = self.duration1, event_id = events[1][self.exp_info['blocks']['Cond1']['message']], baseline = None)
        ic_cond2 = mne.Epochs(ic,events[0], tmin = 0, tmax = self.duration2, event_id=events[1][self.exp_info['blocks']['Cond2']['message']],baseline = None)
        
        
        
        #rel_alphas =np.zeros(n_channels)
        #central_freq_alphas= np.zeros(n_channels) #CHECK only for automatic identfication
        
        #plt.figure()
        fig,ax = plt.subplots()
        
    

        ic_cond1.plot_psd(ax = ax, picks = [0],color = 'red', spatial_colors = False,fmin = 2.0,fmax = 30)
        ic_cond2.plot_psd(ax = ax, picks = [0],color = 'blue', spatial_colors = False,fmin  = 2.0,fmax = 30)

        plt.show()
       
    def change_f0(self,text):

        # This slot is called whenever the text in the QLineEdit changes
        self.central_freq = float(text)



class SpatialWindow(QWidget):
    def __init__(self, eeg_data, exp_info):
        super().__init__()
        self.eeg_data = eeg_data
        
        self.exp_info = exp_info
        
        self.cond_N = 0
        if 'Cond1' in self.exp_info['blocks']:
            self.cond_N = 1
            self.duration1 = exp_info['blocks']['Cond1']['duration']
            if 'Cond2' in self.exp_info['blocks']:
                self.cond_N = 2
                self.duration2 = exp_info['blocks']['Cond2']['duration']
      
        
        #self.parameters_changed = True
        
        
        
        self.filter_type = 0 # 0 -ICA, 1 - CSP
        
        self.spat_filter = None
        
        self.low_freq = 2.0
        self.high_freq = 40.0
        
        self.srate = self.eeg_data.info['sfreq']
        self.n_channels = len(self.eeg_data.info['ch_names'])
        
        self.channel_names = self.eeg_data.info['ch_names']
        
        self.component_num = None
        
        self.ica = None
        
        
        self.init_ui()
        
        
        

    def init_ui(self):
        self.setWindowTitle('Spatial Filter')
        
        self.setFixedSize(1450, 800)
        #self.setGeometry(200, 200, 400, 200)



        self.layout = QHBoxLayout()
        
        self.menu_layout = QVBoxLayout()
        self.layout.addLayout(self.menu_layout,stretch = 1)
        #label = QLabel('This is a pop-out widget!')
        
        
        #self.plot_psd_button = QPushButton('Plot raw psd')
        #self.plot_psd_button.clicked.connect(self.plot_raw_psd)
        
        
        
        low_freq_label = QLabel('Low frequency filter for spatial analysis')
        self.menu_layout.addWidget(low_freq_label)
        self.low_freq_input = QLineEdit(str(self.low_freq))
        self.menu_layout.addWidget(self.low_freq_input)
        self.low_freq_input.textChanged.connect(self.low_freq_changed)

        
        high_freq_label = QLabel('High frequency filter for spatial analysis')
        self.menu_layout.addWidget(high_freq_label)
        self.high_freq_input = QLineEdit(str(self.high_freq))
        self.menu_layout.addWidget(self.high_freq_input)
        self.high_freq_input.textChanged.connect(self.high_freq_changed)

        self.setLayout(self.layout)
        
        
        self.time_mode = True
        
        self.topography_mode = True
        
        
        
        self.filter_choice = QComboBox(self)
        self.filter_choice.setGeometry(10, 50, 150, 30)

        # Add choices to the drop-down list
        choices = ['ICA', 'CSP']
        self.filter_choice.addItems(choices)

        # Connect a slot (function) to handle the choice selection
        self.filter_choice.currentIndexChanged.connect(self.handle_filter_selection)


        self.perform_button = QPushButton('Fit spatial filter')
        self.perform_button.clicked.connect(self.compute_spatial_filter)
        
        self.menu_layout.addWidget(self.filter_choice)
        self.menu_layout.addWidget(self.perform_button)
        
        
        
        self.plot_source_psd = QPushButton('Plot psd of sources')
        self.plot_source_psd.clicked.connect(self.compute_spatial_filter)
        
        
        #spat_num_label = QLabel('Choose the spatial component number')
        #add automatic detection tools
        #self.layout.addWidget(spat_num_label)
        #self.enter_component_num = QLineEdit()
        #self.menu_layout.addWidget(self.enter_component_num)
        #self.enter_component_num.textChanged.connect(self.component_num_changed)
        
        
        self.save_and_exit_button = QPushButton('Accept configuration')
        self.save_and_exit_button.clicked.connect(self.save_and_exit)
        self.menu_layout.addStretch(1)
        self.menu_layout.addWidget(self.save_and_exit_button)
        
        
        self.time_series = self.eeg_data[:,:][0]
        self.columns = ['index','topography', 'time series']
        self.qTable = QTableWidget(self)
        
        self.layout.addWidget(self.qTable,stretch = 5)
        
        #self.layout.
        
        
        self.qTable.setColumnCount(len(self.columns))
        self.qTable.setRowCount(self.time_series.shape[0])
        self.qTable.setHorizontalHeaderLabels(self.columns)
        
        
        self.qTable.horizontalHeader().sectionClicked.connect(self.handle_header_click)
        #self.qTable.horizontalHeader().setSectionResizeMode(QHeaderView.Stretch)

        self.N_rows = len(self.channel_names)
        self.topographies = np.eye(self.n_channels)
        self.filters = np.eye(self.n_channels)
        
        ####################
        
        #self.checkboxes = []
        #self.topographies_items = []
        self.plot_items = []
        self.psd_plot_items = []
        self.topographies_items = []
        
        self.stacked_lays = []
        
        
        self.psd_lines1 = []
        self.psd_lines2 = []
        self.time_lines = []
        #self.scores = []
        #_previous_plot_link = None
        
        
        
        
        
        self.qGroupButton=QButtonGroup()
        
        # Заполнить таблицу
        for ind in range(self.qTable.rowCount()):
            
            # первая колонка - RadioButton
            radioButton = QRadioButton()
            self.qGroupButton.addButton(radioButton,ind)
            self.qTable.setCellWidget(ind, 0, radioButton)
            #========================================================

            # Вторая колонка - топография
            
            topo_filter = TopoFilterCavas(self, self.channel_names, self.topographies[:, ind], self.filters[:, ind])
            self.topographies_items.append(topo_filter)
            self.qTable.setCellWidget(ind, 1, topo_filter)
            #========================================================

            #Третья колонка - график ==============================
            plot_widget = PlotWidget(enableMenu=False)
            plot_widget.setMouseEnabled(y=False)            
            
            self.qTable.setRowHeight(ind,200)
            self.plot_items.append(plot_widget)
            
            self.psd_lines1.append(None)
            self.psd_lines2.append(None)
            self.time_lines.append(None)
            
            
            panel_widget = QWidget()
            stackedLayout = QStackedLayout(self)
            panel_widget.setLayout(stackedLayout)
            stackedLayout.addWidget(plot_widget)
            
            self.stacked_lays.append(stackedLayout)            
            
            self.qTable.setCellWidget(ind, 2, panel_widget)
            #========================================================

            plot_widget = PlotWidget(enableMenu=False)
            plot_widget.setMouseEnabled(y=False)
            
            self.psd_plot_items.append(plot_widget)
            
            stackedLayout.addWidget(plot_widget)

        self.set_time_mode(self.time_mode)
        self.time_mode = False
        
        self.qTable.horizontalHeader().setStretchLastSection(True)
        self.qTable.resizeColumnsToContents()
               
       
        self.qGroupButton.idClicked.connect(self.change_component_num)
        
    def save_and_exit(self):
        self.close()
        
        
    def handle_header_click(self, index):
        if index == 2:
            self.set_time_mode(self.time_mode)
            self.time_mode = not self.time_mode
        
       
    
    def set_topography_mode(self, flag = False):
        pass
    
        
    def set_time_mode(self, flag = False):
        
        if flag == True:
            print(self.filters)

            self.time_series = self.filters@ self.eeg_data[:,:][0]
            
            for ind in range(self.qTable.rowCount()):
                
                x = np.arange(self.time_series.shape[1]) / self.srate
                
                if self.time_lines[ind]:
                    self.time_lines[ind].clear()
                    
                self.time_lines[ind] = self.plot_items[ind].plot(x=x, y=self.time_series[ind,:])
               
                self.stacked_lays[ind].setCurrentIndex(0)
               
        else:
            if self.cond_N == 2:  
                events = mne.events_from_annotations(self.eeg_data)
                
               
                raw_cond1 = mne.Epochs(self.eeg_data,events[0], tmin = 0, tmax = self.duration1-5.0, event_id=events[1][self.exp_info['blocks']['Cond1']['message']], baseline = None)
                raw_cond2 = mne.Epochs(self.eeg_data,events[0], tmin = 5.0, tmax = self.duration2, event_id=events[1][self.exp_info['blocks']['Cond2']['message']],baseline = None)
                               
                
                ic_cond1=self.filters@raw_cond1.get_data()
                ic_cond2=self.filters@raw_cond2.get_data()
                
                freqs,pxx1 = sn.welch(ic_cond1,axis = 2,fs = self.srate,nperseg = np.min([int(self.srate*10),int(self.duration1*self.srate)]))
                pxx1 = pxx1.mean(axis = 0)
                
                freqs,pxx2 = sn.welch(ic_cond2,axis = 2,fs = self.srate,nperseg = np.min([int(self.srate*10),int(self.duration1*self.srate)]))
                pxx2 = pxx2.mean(axis = 0)
                
                ind_to_plot = np.where((freqs<self.high_freq)*(freqs>self.low_freq))[0]
                
                for i in range(self.N_rows):
                    if self.psd_lines1[i]:
                        self.psd_lines1[i].clear()
                        self.psd_lines2[i].clear()
                    self.psd_lines1[i]=self.psd_plot_items[i].plot(freqs[ind_to_plot],np.log10(pxx1[i,ind_to_plot]),pen = mkPen('r', width=3, style=Qt.DashLine) )
                    self.psd_lines2[i]=self.psd_plot_items[i].plot(freqs[ind_to_plot],np.log10(pxx2[i,ind_to_plot]),pen = mkPen('g', width=3, style=Qt.DashLine))
                                        
                    self.stacked_lays[i].setCurrentIndex(1)
    
                #plt.show()
        
         
    def change_component_num(self,idx):
        self.component_num = idx
        
        pca_M = self.ica.pca_components_

        ica_M = self.ica.unmixing_matrix_

        unmixing_M = ica_M@pca_M
        
        
    
        self.spat_filter = unmixing_M[self.component_num,:]
        



    def high_freq_changed(self, text):
        # This slot is called whenever the text in the QLineEdit changes
        self.high_freq = float(text)
        
    def low_freq_changed(self, text):
        # This slot is called whenever the text in the QLineEdit changes
        self.low_freq = float(text)

     
    #def plot_raw_psd(self):
    #    self.eeg_data.plot_psd()
        
        
    def handle_filter_selection(self):
        
        selected_choice = self.filter_choice.currentText()

        if selected_choice == 'ICA':
            self.filter_type = 0
        if selected_choice == 'CSP':
            self.filter_type = 1
            
            
    def compute_spatial_filter(self):
        modified_eeg = self.eeg_data.copy()
        
        modified_eeg.notch_filter([50.0,100.0,150.0,200.0])
        modified_eeg.filter(self.low_freq,self.high_freq)
        
        #??? JUMP
        if self.filter_type == 0:
            last_idx = (modified_eeg[0,:][0]).shape[1]
            self.ica = mne.preprocessing.ICA()
            self.ica.fit(modified_eeg, start = int(5*self.srate), stop = int(last_idx-5*self.srate))
            
            
            
            pca_M = self.ica.pca_components_

            ica_M = self.ica.unmixing_matrix_

            unmixing_M = ica_M@pca_M
            
            self.filters = unmixing_M
            self.topographies = np.linalg.pinv(unmixing_M)
            #self.plot_spatial_filter()
            
        self.time_mode = True
        self.set_time_mode(self.time_mode)
        self.time_mode = False
        self.upd_topo()
        
        
    def upd_topo(self):
            
        for i in range(self.qTable.rowCount()):
            #????
            self.topographies_items[i].update_data(self.topographies[:,i], self.filters[i,:])

class RawVisWin(QWidget):
    def __init__(self, eeg_data):
        self.eeg_data = eeg_data
        self.parameters_changed = True
        
        super().__init__()
        self.init_ui()
        
        

    def init_ui(self):
        self.setWindowTitle('Pop-Out Widget')
        self.setGeometry(200, 200, 400, 200)

        self.layout = QVBoxLayout()
        #label = QLabel('This is a pop-out widget!')
        
        
        
        low_freq_label = QLabel('Low frequency filter for vizualization')
        self.layout.addWidget(low_freq_label)
        self.low_freq_input = QLineEdit()

        # TODO: добавить значения по умолчанию
        self.layout.addWidget(self.low_freq_input)

        high_freq_label = QLabel('High frequency filter for vizualization')
        self.layout.addWidget(high_freq_label)
        self.high_freq_input = QLineEdit()       
        
        # TODO: добавить значения по умолчанию
        self.layout.addWidget(self.high_freq_input)
        
        self.setLayout(self.layout)
        
        
        self.plot_button = QPushButton('Plot EEG data')
        self.plot_button.clicked.connect(self.plot_raw_data_process)
        self.layout.addWidget(self.plot_button)

        
        
    
    def plot_raw_data_process(self):
        if self.parameters_changed:
            self.modified_EEG = self.eeg_data.copy()
            
            if 1:
                self.modified_EEG.notch_filter([50.0,100.0,150.0,200.0])
            self.modified_EEG.filter(2.0,30.0)
            
        self.modified_EEG.plot()
        

class EEGAnalysisApp(QMainWindow):
    
    def __init__(self, sourcefilename = "", savefilename = ""):
        super().__init__()
        self.init_ui(sourcefilename, savefilename)


    def init_ui(self, filename = "", savefilename = ""):
        
        self.setWindowTitle('EEG Analysis App')
        #self.setGeometry(100, 100, 800, 600)
        self.setFixedSize(800,500)

        self.central_widget = QWidget(self)
        self.setCentralWidget(self.central_widget)

        self.layout = QVBoxLayout()

###
#        self.load_button = QPushButton('Load EEG Data')
#        self.load_button.clicked.connect(self.load_eeg_data)
#        self.layout.addWidget(self.load_button)
###

        
        self.badseg_button = QPushButton('Mark bad segments')
        self.layout.addWidget(self.badseg_button)
        self.badseg_button.clicked.connect(self.show_raw_vis_win)


        self.spatialF_button = QPushButton('Perform spatial filtration')
        self.spatialF_button.clicked.connect(self.show_spatial_filter_win)
        self.layout.addWidget(self.spatialF_button)

        

        self.glob_layout = QHBoxLayout()
        self.glob_layout.addLayout(self.layout)
        self.central_widget.setLayout(self.glob_layout)
        
        
        pixmap = QPixmap('./Data/Scripts/brain_AI.png')
        self.im_label = QLabel()
        self.im_label.setPixmap(pixmap)
        #self.image = QImage()
        #self.image.load('brain_AI.png')
        self.glob_layout.addWidget(self.im_label)
        
        
        self.filter_choice = QComboBox(self)
        #self.filter_choice.setGeometry(10, 50, 150, 30)

        # Add choices to the drop-down list
        choices = ['white kf', 'cfir', 'pink kf']
        self.filter_choice.addItems(choices)

        # Connect a slot (function) to handle the choice selection
        self.filter_choice.currentIndexChanged.connect(self.handle_filter_selection)
        self.layout.addWidget(self.filter_choice)
        
        
        self.fit_temporal_filter = QPushButton('Fit temporal filter')
        self.fit_temporal_filter.clicked.connect(self.call_temp_filt_win)
        self.layout.addWidget(self.fit_temporal_filter)
        
        
        self.layout.addStretch(1)
        
        self.status_label = QLabel('Status: No EEG data loaded')
        self.layout.addWidget(self.status_label)
        
        self.status_label.setWordWrap(True)
        
        
        self.save_config_button = QPushButton('Save configuration')
        self.save_config_button.clicked.connect(self.save_config)
        
        
        self.layout.addWidget(self.save_config_button)

        if filename != "":
            self.eeg_file_path = filename
            self.load_eeg_data_from_file()

        if savefilename == "":
            self.file_path = "config.txt"
        else:
            self.file_path = savefilename
        
    def save_config(self): 
            
        options = QFileDialog.Options()
        options |= QFileDialog.ReadOnly  # To make sure the file is only saved
        file_dialog = QFileDialog()
        # file_path, _ = file_dialog.getSaveFileName(self, "Save Data", "", "Text Files (*.txt);;All Files (*)", options=options)

        # Check if a valid file path is provided
        if self.file_path:

            combined_array = np.concatenate((np.array([self.kalman_win.central_freq]),np.array([self.kalman_win.q0]),\
                                             np.array([self.kalman_win.q1]),self.spatial_filter_win.spat_filter))#,np.array([777])))
            print(combined_array)
            print(combined_array.shape)
    
    
            # Write the array to a text file with comma+space delimiters
            with open(self.file_path, 'w') as file:
                # here dump, then, in another script - load and change the phase                
                file.write(', '.join(str(x) for x in combined_array))
                
                
         

        
    def call_temp_filt_win(self):
        spatial_filter_dict = dict()
        spatial_filter_dict['filter_coef'] = self.spatial_filter_win.spat_filter
        spatial_filter_dict['ica'] = self.spatial_filter_win.ica        
        spatial_filter_dict['num_component'] = self.spatial_filter_win.component_num
        
        
        self.kalman_win = WhiteKalmanFit(self.raw, self.exp_settings, spatial_filter_dict)
        self.kalman_win.show()
        


    def handle_filter_selection(self):
        selected_choice = self.filter_choice.currentText()

        if selected_choice == 'white kf':
            self.filter_type = 0
        if selected_choice == 'cfir':
            self.filter_type = 1
        if selected_choice == 'pink kf':
            self.status_label.setText('NO PINK')
            
        
    def load_eeg_data_from_file(self):
        # Load EEG data from the selected file
        # Update status label
        file = open(self.eeg_file_path, "rb")
        container =  pickle.load(file)
        file.close()
        
        self.exp_settings = container['exp_settings']
        srate = self.exp_settings['srate']
        data = container['eeg']
        stims = container['stim']
        
        channel_names = self.exp_settings['channel_names']
        
              
    
        for i in range(len(channel_names)):
            channel_names[i] = channel_names[i].upper()
            print(channel_names)
            n_channels = len(channel_names)

        info = mne.create_info(ch_names=channel_names, sfreq = srate, ch_types = 'eeg',)
        raw =  mne.io.RawArray(data.T, info)
        

        print(data.shape)
        print(info)
        
        
        # condition1 , condition2, prepare, ready
        
        ### ASSUME THAT MOVE AND REST DURATIONS ARE EQUAL
        
        
        self.cond_N = 0
        if 'Cond1' in self.exp_settings['blocks']:
            self.cond_N = 1
            if 'Cond2' in self.exp_settings['blocks']:
                self.cond_N = 2
            
        
        
        
        if self.cond_N > 0:
            duration1 = self.exp_settings['blocks']['Cond1']['duration']
            cond1_id = int(self.exp_settings['blocks']['Cond1']['id'])
            
        if self.cond_N >1:
            duration2= self.exp_settings['blocks']['Cond2']['duration']
            cond2_id = int(self.exp_settings['blocks']['Cond2']['id'])
            
        #close_id = self.exp_settings['blocks']['Close']['id']
        #prepare_id = self.exp_settings['blocks']['Prepare']['id']
        #ready_id = self.exp_settings['blocks']['Ready']['id']
        
        start_times = np.zeros(0, dtype = 'float')
        
        if self.cond_N > 0:
            where_idx = (stims == cond1_id).astype('int')
            where_idx = np.concatenate([np.zeros(1),where_idx])
            
            deriv_idx = where_idx[1:]-where_idx[:-1]
            
            start_times = np.concatenate([start_times,np.where(deriv_idx > 0)[0]/srate])
        
    
        
            #description = []
            #for st in start_times:
            #    if stims[int(round((st*srate)))] == cond1_id:
            #        description.append(self.exp_settings['blocks']['Cond1']['message'])
            
            num_cond1 = len(start_times) 
            
            if self.cond_N < 2:
                description = []
                for st in start_times:
                    if stims[int(round((st*srate)))] == cond1_id:
                        description.append(self.exp_settings['blocks']['Cond1']['message'])
                
                print(description, start_times, duration1)
            
                annotations = mne.Annotations(start_times, duration1, description)
                raw.set_annotations(annotations)
            
            
            
        if self.cond_N > 1:
            where_idx = (stims == cond2_id).astype('int')
            where_idx = np.concatenate([np.zeros(1),where_idx])
            
            deriv_idx = where_idx[1:]-where_idx[:-1]
            
            start_times = np.concatenate([start_times,np.where(deriv_idx > 0)[0]/srate])
        
            #start_times = np.zeros(0, dtype = 'float')
            
        
            description = []
            for st in start_times:
                if stims[int(round((st*srate)))] == cond2_id:
                    description.append(self.exp_settings['blocks']['Cond2']['message'])
                if stims[int(round((st*srate)))] == cond1_id:
                    description.append(self.exp_settings['blocks']['Cond1']['message'])
            
                
            print(description, start_times,duration1, duration2)
        
        
            num_cond2 = len(description)
            durations = np.concatenate([np.ones(num_cond1)*duration1,np.ones(num_cond2)*duration2])
            
            annotations = mne.Annotations(start_times, duration2, description)
            raw.set_annotations(annotations)
        
        self.raw = raw
        
        #montage = mne.channels.make_standard_montage('standard_1005')
        #raw.set_montage(montage)
        
        self.status_label.setText(f'Status: EEG data loaded from {self.eeg_file_path}')

    def load_eeg_data(self):
        # Add code to open a file dialog and load EEG data here
        file_dialog = QFileDialog(self)
        self.eeg_file_path, _ = file_dialog.getOpenFileName(self, 'Open EEG Data', '')#, #'EEG Data Files (*.eeg)')

        if self.eeg_file_path:
            self.load_eeg_data_from_file()
        

        

  
   
    def show_spatial_filter_win(self):
        # Add code to perform Independent Component Analysis (ICA) on the loaded EEG data here
        # Update status label
        #self.status_label.setText('Status: filter_not_fitted')
        self.spatial_filter_win = SpatialWindow(self.raw, self.exp_settings)
        self.spatial_filter_win.show()
        
    
       
    
    def show_raw_vis_win(self):
            self.raw_vis_win = RawVisWin(self.raw)
            self.raw_vis_win.show()  
        
    

def main():
    
    plt.close('all')
    np.random.seed(0)
    
    
    app = QApplication(sys.argv)

    if(len(sys.argv) > 2):
        window = EEGAnalysisApp(sys.argv[1], sys.argv[2])
    else:
        window = EEGAnalysisApp()

    window.show()
    sys.exit(app.exec_())

    

if __name__ == '__main__':
    main()








