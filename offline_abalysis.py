


















import mne
import numpy as np
from matplotlib import pyplot as plt
from datetime import datetime
import os
import scipy.signal as sn
import pickle
from kalman_float_int import float_int_kalman, grid_optimizer, ideal_envelope, int_kalman






plt.close('all')

np.random.seed(0)




pathdir = 'C:/Users/Fedosov/Documents/projects/adapt_zerolat2/results/baseline_experiment_03-10_20-58-05/'





file = open(pathdir+'data.pickle', "rb")
container =  pickle.load(file)
file.close()

exp_settings = container['exp_settings']
srate = exp_settings['srate']
data = container['eeg']
stims = container['stim']
channel_names = exp_settings['channel_names']
for i in range(9):
    channel_names[i] = channel_names[i].upper()
print(channel_names)
n_channels = len(channel_names)







info = mne.create_info(ch_names=channel_names, sfreq = srate, ch_types = 'eeg',)
raw =  mne.io.RawArray(data.T, info)

print(data.shape)
print(info)

### ASSUME THAT MOVE AND REST DURATIONS ARE EQUAL
duration = exp_settings['blocks']['Move']['duration']
move_id = exp_settings['blocks']['Move']['id']
rest_id = exp_settings['blocks']['Rest']['id']
prepare_id = exp_settings['blocks']['Prepare']['id']
ready_id = exp_settings['blocks']['Ready']['id']



start_times = (np.where(np.isin(stims[1:]-stims[:-1], [move_id -prepare_id,rest_id-prepare_id]))[0]+1)/srate


description = []
for st in start_times:
    if stims[int(round((st*srate)))] == move_id:
        description.append('Move')

    if stims[int(round((st*srate)))] == rest_id:
        description.append('Rest')

  
print(description, start_times, duration)




annotations = mne.Annotations(start_times, duration, description)
raw.set_annotations(annotations)


montage = mne.channels.make_standard_montage('standard_1005')
raw.set_montage(montage)



raw.plot(scalings = dict(eeg=1e0))
plt.show()
raw_copy = raw.copy()

#HERE MANUALLY MARK BAD SEGMENTS

#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#



bad_segments = []

bad_segments.append(np.arange(5*srate))
for annot in raw._annotations:
    if annot['description'] == 'BAD_':
        #print('HERE')
        bad_segments = bad_segments + np.arange(int(round(annot['onset']*srate)),int(round((annot['onset']+annot['duration'])*srate))).tolist()


last_idx = int(round(raw[:][1][-1]*srate))

bad_segments.append(np.arange(last_idx-5*srate,last_idx+1))
bad_segments =np.concatenate(bad_segments)
good_idx = np.setdiff1d(np.arange(last_idx+1),bad_segments)



raw.notch_filter([50.0,100.0,150.0,200.0])
raw.filter(2.0,30.0)

ica = mne.preprocessing.ICA()
ica.fit(raw, start = int(5*srate), stop = int(last_idx-5*srate))

ics = ica.get_sources(raw)
ica.plot_sources(raw)

events = mne.events_from_annotations(raw)

ics_move = mne.Epochs(ics,events[0], tmin = 0, tmax = duration, event_id = events[1]['Move'], baseline = None)
ics_rest = mne.Epochs(ics,events[0], tmin = 0, tmax = duration, event_id = events[1]['Rest'],baseline = None)

rel_alphas =np.zeros(n_channels)
rel_betas = np.zeros(n_channels)
central_freq_alphas= np.zeros(n_channels)
central_freq_betas= np.zeros(n_channels)

fig, axs = plt.subplots(3, 3)
for i in range(n_channels):

    ics_move.plot_psd(ax = axs[i//3, i%3], picks = [i,], color = 'red', spatial_colors = False,fmin = 2.0,fmax = 30)
    ics_rest.plot_psd(ax = axs[i//3, i%3], picks = [i,], color = 'blue', spatial_colors = False,fmin  = 2.0,fmax = 30)
    axs[i//3, i%3].set_title(str(i))
    
    psd_alpha_rest = ics_rest.compute_psd(fmin = 9.0, fmax = 13.0,picks = [i,]).get_data(return_freqs = True)
    psd_beta_rest = ics_rest.compute_psd(fmin = 16.0, fmax = 26.0,picks = [i,]).get_data(return_freqs = True)
    
    psd_alpha_move = ics_move.compute_psd(fmin = 9.0, fmax = 13.0,picks = [i,]).get_data(return_freqs = True)
    psd_beta_move = ics_move.compute_psd(fmin = 16.0, fmax = 26.0,picks = [i,]).get_data(return_freqs = True)
    
    rel_alphas[i] = np.mean(psd_alpha_rest[0])/np.mean(psd_alpha_move[0])
    rel_betas[i] = np.mean(psd_beta_rest[0])/np.mean(psd_beta_move[0])
    
    central_freq_alphas[i]= psd_alpha_rest[1][np.argmax(np.mean(psd_alpha_rest[0],axis = 0))]
    central_freq_betas[i] = psd_beta_rest[1][np.argmax(np.mean(psd_beta_rest[0],axis = 0))]
    
    
    

    


ica.plot_components()


    
plt.show()
#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#


alpha_idx = np.argmax(rel_alphas)
print('index of SMR component is ', alpha_idx)

#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#

central_alpha = 10.5#central_freq_alphas[alpha_idx]
central_beta = central_freq_betas[alpha_idx]

print('central alpha freq is ', central_alpha)
print('central beta freq is ', central_beta)




#????? можно сделать тоже вводимыми значениями
bands = [[central_alpha - 2.0, central_alpha + 2.0],[central_beta-3.0,central_beta+3.0]]




pca_M = ica.pca_components_

ica_M = ica.unmixing_matrix_

unmixing_M = ica_M@pca_M

ica_filter = unmixing_M[alpha_idx,:]

rhythm = ics[alpha_idx,:][0][0,:]






f,pxx = sn.welch(ics_rest.get_data()[:,alpha_idx,:],axis = 1, fs = srate, nperseg = srate)
pxx = np.mean(pxx,axis = 0)
plt.figure()
plt.plot(f[:40],np.log10(pxx[:40]))


raw_realt = raw_copy[:][0]


# q_s, r_s - ????
freq0 = (bands[0][1]+bands[0][0])/2
A = 0.99

b50,a50=sn.butter(1,[40.0,60.0], btype = 'bandstop',fs = srate)
#b100,a100 = sn.butter(1,[96.0,104.0], btype = 'bandstop',fs = srate)
#b150,a150 = sn.butter(1,[144.0,156.0], btype = 'bandstop',fs = srate)
#b200,a200 = sn.butter(1,[192.0,208.0], btype = 'bandstop',fs = srate)
b,a = sn.butter(1,[2.0,50.0], btype = 'bandpass', fs =srate) # или 2.0





'''
w, h = sn.freqz(b,a,fs = srate)
plt.figure()
plt.plot(np.abs(h))
plt.figure()

phi = np.angle(h)
t = np.arange(len(phi)) / float(srate) * 1000  # time in ms
latency = -phi / (2 * np.pi * w) * 1000  # latency in ms
plt.plot(w,latency)

'''



# plt.figure()
# w, gd = sn.group_delay((b, a),fs = srate)

# plt.title('Digital filter group delay')
# plt.plot(w, gd*1000/srate)
# plt.ylabel('Group delay [samples]')
# plt.xlabel('Frequency [rad/sample]')
# plt.show()


#w, gd = sn.group_delay((b50, a50),fs = srate)

#plt.title('Digital filter group delay')
#plt.plot(w, gd*1000/srate)
#plt.ylabel('Group delay [samples]')
#plt.xlabel('Frequency [rad/sample]')
#plt.show()




realt  = ica_filter@raw_realt


#realt = sn.lfilter(b50,a50,realt)
#realt = sn.lfilter(b100,a100,realt)
#realt = sn.lfilter(b150,a150,realt)
#realt = sn.lfilter(b200,a200,realt)
#realt = sn.lfilter(b,a,realt)

b = (b*0x1FFF).astype('int16')
a = (a*0x1FFF).astype('int16')

b50 = (b50*0x1FFF).astype('int16')
a50 = (a50*0x1FFF).astype('int16')


x_buf = np.zeros(3)
y_buf = np.zeros(3)

x_buf50 = np.zeros(3)
y_buf50 = np.zeros(3)




filtered = np.zeros(realt.shape[0])




counter = 0
M = 3
for i in range(realt.shape[0]):
    x_buf[counter] = realt[i]
    
    x_buf50[counter] = (b[0]*x_buf[counter%M]+b[1]*x_buf[(counter-1)%M]+b[2]*x_buf[(counter-2)%M]-a[1]*y_buf[(counter-1)%M]-a[2]*y_buf[(counter-2)%M])/0x1FFF
    y_buf[counter] =x_buf50[counter]
    
    
    filtered[i] = (b50[0]*x_buf50[counter%M]+b50[1]*x_buf50[(counter-1)%M]+b50[2]*x_buf50[(counter-2)%M]-a50[1]*y_buf50[(counter-1)%M]-a50[2]*y_buf50[(counter-2)%M])/0x1FFF
    y_buf50[counter] =filtered[i]
    

    
    
    
    counter += 1
    counter %= M
    
    
realt = filtered
    




#corr_mem, lat_mem = grid_optimizer(realt, good_idx, freq0, A, srate, log_num = 12, q_bounds = [2,-9],r_bounds = [2,-9])
    


#q =0.001
#r = 1.0
#q = 1e-09
#r = 4.641588833612782e-06

q = 0.003981071705534978   #0.1
r = 18.478497974222908


intORfloat = 0
if (intORfloat):
    
    
    kf = float_int_kalman(freq0, A, srate, q, r)
    
    filtered, envelope = kf.apply(realt)
    
    
    ff, psd = sn.welch(realt, fs=srate, nperseg=srate*4)
    plt.figure()
    plt.plot(ff,np.log10(psd))
    
    ff, psd = sn.welch(filtered, fs=srate, nperseg=srate*4)
    plt.plot(ff,np.log10(psd))
    
    
    
    
    
    
    
    
    
    envelope = envelope[good_idx]
    
    gt_envelope = ideal_envelope(freq0, srate, realt)[good_idx]
    
    
        
    
    cc_cum = np.zeros(99)
    for i,bias in enumerate(range(1,100)):
    
    
        gt_envelope_trim = gt_envelope[:-bias]
    
        gt_envelope_trim -= np.mean(gt_envelope_trim)
     
        
        
        envelope_trim = envelope[bias:].copy()
       
    
        envelope_trim -= np.mean(envelope_trim)
      
    
        cc_cum[i] = np.sum(envelope_trim*gt_envelope_trim)/(np.linalg.norm(envelope_trim)*np.linalg.norm(gt_envelope_trim))
        #beta_cc_cum[i] = np.sum(beta_envelope*gt_beta_envelope)/(np.linalg.norm(beta_envelope)*np.linalg.norm(gt_beta_envelope))
    
    
    plt.figure()
    plt.plot(np.arange(1,100)*1000/srate,cc_cum)
    plt.xlabel('latency, ms')
    plt.ylabel('corr')
    plt.plot([20,20],[0.55,0.9])
    
    
    
    plt.figure()
    time = np.arange(good_idx.shape[0])*1000.0/srate
    plt.plot(time,realt[good_idx],alpha = 0.5)
    plt.plot(time,filtered[good_idx])
    plt.plot(time,envelope)
    
    plt.xabel('time,ms')
    plt.legend('')
    
    
    
    
    
    
    
    
    #kf.q_s = 0.01
    #kf.r_s = 10.0
    
    # meas, KF, n_iter: int = 50, tol: float = 1e-3
    
    #alpha_rhythm += np.sin(np.arange(alpha_rhythm.shape[0])*(freq_alpha*2*np.pi)/srate)*0.1
    
    
    
    
    
    
    
    
    
    #consider bad segments
    
    # plt.figure()
    # plt.hist(alpha_envelope_kf[good_idx],bins = 40, range = [np.min(alpha_envelope_kf[good_idx]),np.max(alpha_envelope_kf[good_idx])])
    
    # hq = 0.97
    # lq = 0.1
    
    # high_quantile_alpha_kf = np.quantile(alpha_envelope_kf[good_idx],hq)
    # low_quantile_alpha_kf = np.quantile(alpha_envelope_kf[good_idx],lq)
    # plt.figure()
    # plt.plot(realt, alpha = 0.5)
    # plt.plot(np.real(kf_states_alpha))
    # plt.plot(alpha_envelope_kf)
    # length = len(alpha_envelope_kf)
    # plt.plot(np.arange(length),[high_quantile_alpha_kf]*length)
    # plt.plot(np.arange(length),[low_quantile_alpha_kf]*length)
    
    
    
    
    
    
    
    
    
    
    
    
    #file = open(pathdir + 'model.pickle', "wb")
    #pickle.dump({'hqa_kf': high_quantile_alpha_kf,'lqa_kf': low_quantile_alpha_kf,
    #             'hqb_kf': high_quantile_beta_kf,'lqb_kf': low_quantile_beta_kf,
    #             'hqa_cfir': high_quantile_alpha_cfir,'lqa_cfir': low_quantile_alpha_cfir,
    #             'hqb_cfir': high_quantile_beta_cfir,'lqb_cfir': low_quantile_beta_cfir,
    #            'kf': kf,'cfir_alpha': cfir_alpha,'cfir_beta':cfir_beta, 'ica_filter': ica_filter,
    #           'b': b,'a': a, 'b50': b50,'a50': a50, 'b100':b100,'a100':a100,'b150':b150,'a150':a150,'b200':b200,'a200':a200,
    #            'smoother_alpha_cfir':smoother_alpha_cfir,'smoother_beta_cfir':smoother_beta_cfir}, file = file)
    #file.close()
    #
    
    plt.show()








else:
    
    realt[:500] = np.random.randn(500)/10.0*np.max(np.abs(realt[500:]))
    
    realt = ((realt/np.max(np.abs(realt)))*0x7F).astype('int16')
    
    kf = int_kalman(freq0, A, srate, q, r)
    
    

    mask0 = np.uint32(0x0000FFFF)
    mask1 = np.uint32(0xFFFF0000)

    r0 = np.bitwise_and(kf.R, mask0)
    r1 = np.right_shift(np.bitwise_and(kf.R, mask1),16)
    
    q0 = np.bitwise_and(kf.Q[0,0], mask0)
    q1 = np.right_shift(np.bitwise_and(kf.Q[0,0], mask1),16)
    
    #??????????????? signed or unsigned ????????????????????????????

    print(bin(r1))
    print(bin(r0))


    thr = np.uint16(3000)#0xFF)
    
    combined_array = np.concatenate((b, a, b50, a50, kf.Phi.flatten(),[q1],[q0],[r1],[r0],[thr]))


    
    # Write the array to a text file with comma+space delimiters
    with open('C:/Users/Fedosov/Documents/projects/mks/FilterTest/Win32/Debug/kalman.txt', 'w') as file:
        file.write(', '.join(str(x) for x in combined_array))
    
        
    
    filtered, envelope = kf.apply(realt)
    
    
    ff, psd = sn.welch(realt, fs=srate, nperseg=srate*4)
    plt.figure()
    plt.plot(ff,np.log10(psd))
    
    ff, psd = sn.welch(filtered, fs=srate, nperseg=srate*4)
    plt.plot(ff,np.log10(psd))
    
    
    plt.xlabel('freq, Hz')
    plt.ylabel('psd, uV^2/Hz')
    
    
    
    
    
    
    
    
    
    
    
    
    
    envelope = envelope[good_idx]
    
    gt_envelope = ideal_envelope(freq0, srate, realt)[good_idx]
    
    
        
    
    cc_cum = np.zeros(99)
    for i,bias in enumerate(range(1,100)):
    
    
        gt_envelope_trim = gt_envelope[:-bias]
    
        gt_envelope_trim -= np.mean(gt_envelope_trim)
     
        
        
        envelope_trim = envelope[bias:].copy()
       
    
        envelope_trim -= np.mean(envelope_trim)
      
    
        cc_cum[i] = np.sum(envelope_trim*gt_envelope_trim)/(np.linalg.norm(envelope_trim)*np.linalg.norm(gt_envelope_trim))
        #beta_cc_cum[i] = np.sum(beta_envelope*gt_beta_envelope)/(np.linalg.norm(beta_envelope)*np.linalg.norm(gt_beta_envelope))
    
    
    plt.figure()
    plt.plot(np.arange(1,100)*1000/srate,cc_cum)
    plt.xlabel('latency, ms')
    plt.ylabel('corr')
    plt.plot([20,20],[0.55,0.9])
    
    
    biir,aiir = sn.butter(1,[freq0-2.0,freq0+2.0],btype ='bandpass',fs = srate)
    super_gt = sn.lfilter(biir,aiir,realt)
    

    w, h = sn.freqz(b,a,fs = srate)
    plt.figure()
    plt.plot(w,np.abs(h))
    plt.figure()

    phi = np.angle(h)
    t = np.arange(len(phi)) / float(srate) * 1000  # time in ms
    latency = -phi / (2 * np.pi * w) * 1000  # latency in ms
    plt.plot(w,latency)
    plt.xlabel('freq,Hz')
    plt.ylabel('latency, ms')
    
    
    plt.figure()
    time = np.arange(good_idx.shape[0])*1000.0/srate
    plt.plot(time,realt[good_idx]*10,alpha = 0.5)
    plt.plot(time,filtered[good_idx])
    plt.plot(time,envelope)
    plt.plot(time,super_gt[good_idx]*10,alpha = 0.5)
    
    plt.xlabel('time,ms')
    plt.legend(['high-pass filtered raw','kalman filtered','envelope','iir-1-order, 9-13 Hz'])
    
    
    
    
    
    #kf.q_s = 0.01
    #kf.r_s = 10.0
    
    # meas, KF, n_iter: int = 50, tol: float = 1e-3
    
    #alpha_rhythm += np.sin(np.arange(alpha_rhythm.shape[0])*(freq_alpha*2*np.pi)/srate)*0.1
    
    
    
    
    
    
    
    
    
    #consider bad segments
    
    # plt.figure()
    # plt.hist(alpha_envelope_kf[good_idx],bins = 40, range = [np.min(alpha_envelope_kf[good_idx]),np.max(alpha_envelope_kf[good_idx])])
    
    # hq = 0.97
    # lq = 0.1
    
    # high_quantile_alpha_kf = np.quantile(alpha_envelope_kf[good_idx],hq)
    # low_quantile_alpha_kf = np.quantile(alpha_envelope_kf[good_idx],lq)
    # plt.figure()
    # plt.plot(realt, alpha = 0.5)
    # plt.plot(np.real(kf_states_alpha))
    # plt.plot(alpha_envelope_kf)
    # length = len(alpha_envelope_kf)
    # plt.plot(np.arange(length),[high_quantile_alpha_kf]*length)
    # plt.plot(np.arange(length),[low_quantile_alpha_kf]*length)
    
    
    
    
    
    
    
    
    
    
    
    
    #file = open(pathdir + 'model.pickle', "wb")
    #pickle.dump({'hqa_kf': high_quantile_alpha_kf,'lqa_kf': low_quantile_alpha_kf,
    #             'hqb_kf': high_quantile_beta_kf,'lqb_kf': low_quantile_beta_kf,
    #             'hqa_cfir': high_quantile_alpha_cfir,'lqa_cfir': low_quantile_alpha_cfir,
    #             'hqb_cfir': high_quantile_beta_cfir,'lqb_cfir': low_quantile_beta_cfir,
    #            'kf': kf,'cfir_alpha': cfir_alpha,'cfir_beta':cfir_beta, 'ica_filter': ica_filter,
    #           'b': b,'a': a, 'b50': b50,'a50': a50, 'b100':b100,'a100':a100,'b150':b150,'a150':a150,'b200':b200,'a200':a200,
    #            'smoother_alpha_cfir':smoother_alpha_cfir,'smoother_beta_cfir':smoother_beta_cfir}, file = file)
    #file.close()
    #
    
    plt.show()



