
import datetime
import json
import os
import pickle
import sys
import pylsl
import numpy as np

from lsl_inlet import LSLInlet


def RunPreparedProtocol(protocol_file, result_file):
    # десериализовать список блоков
    data = open(protocol_file,)
    data = json.load(data) 

    # сформировать словирь блоков из созданной последовательности блоков        
    blocks = dict()
    for i in range(len(data)):
        if data[i]["name"] not in blocks:
            blocks[data[i]["name"]] = {'duration': data[i]["duration"], 'id': data[i]["code"], 'message': data[i]["message"]}
    
    # список меток?
    seq2 = list()
    for i in range(len(data)):
        seq2.append(data[i]["name"])
        
    
    streams = pylsl.resolve_streams()

    # подготовить настройки приемника lsl-канала
    exp_settings = {
                'exp_name': 'Baseline',
                'lsl_stream_name': streams[0].name(),
                'blocks': blocks,
                'sequence': seq2,
    
                #максимальная буферизация принятых данных через lsl
                'max_buflen': 5,  # in seconds!!!!
    
                #максимальное число принятых семплов в чанке, после которых появляется возможность
                #считать данные
                'max_chunklen': 1,  # in number of samples!!!!
    
    
                ##какой канал использовать
                #'channels_subset': 'P3-A1',
                #'bands': [[9.0, 13.0],[18.0,25.0]],
                #'results_path': results_path
                }
    
    inlet = LSLInlet(exp_settings)
    inlet.srate = inlet.get_frequency()
    xml_info = inlet.info_as_xml()
    channel_names = inlet.get_channels_labels()
    print(channel_names)
    exp_settings['channel_names'] = channel_names
    
    n_channels = len(channel_names)
    
    
    srate = int(round(inlet.srate)) #Hz
    exp_settings['srate'] = srate
    
    #max buffer length
    total_samples = 0
    
    for block_name in exp_settings['sequence']:
        current_block = exp_settings['blocks'][block_name]
        total_samples += round(srate * current_block['duration'])
    
    data = np.zeros((total_samples+round(exp_settings['max_buflen']*srate),n_channels))
    print(data.shape)
    stims =  np.zeros((total_samples+round(exp_settings['max_buflen']*srate),1)).astype(int)
    
    
    n_samples_received = 0
    n_samples_received_in_block = 0
    
        
    block_idx = 0
    block_name = exp_settings['sequence'][0]
    current_block = exp_settings['blocks'][block_name]

    # посчитать количество отсчетов с учтом частоты
    n_samples = srate * current_block['duration']
    block_id = current_block['id']      
        
    
    while (1):

        # как только получли нужно количество отсчетов, переходим к следующему шагу
        if n_samples_received_in_block >= n_samples:
            dif =  n_samples_received_in_block - n_samples
            
            
            block_idx += 1
            
            
            if block_idx >= len(exp_settings['sequence']):
                #save_and_finish()
                inlet.disconnect()
    
                break
    
            block_name = exp_settings['sequence'][block_idx]
            current_block = exp_settings['blocks'][block_name]
            n_samples = srate * current_block['duration']
            n_samples_received_in_block = dif
            block_id = current_block['id']
            if dif>0:
                stims[n_samples_received-dif:n_samples_received] = block_id
        
    
        # получить очередной чанк из lsl-канала
        chunk, t_stamp = inlet.get_next_chunk()
        # print(f"{chunk=}")
        if chunk is not None:
            n_samples_in_chunk = len(chunk)

            # сохранить полученные отсчеты в буфер
            data[n_samples_received:n_samples_received + n_samples_in_chunk, :] = chunk

            # пометить полученные отсчеты идентификатором текущего блока
            stims[n_samples_received:n_samples_received + n_samples_in_chunk] = block_id

            # просуммировать количество полученных отсчетов в рамках блока
            n_samples_received_in_block += n_samples_in_chunk

            # просуммировать общее количество полученных отсчетов
            n_samples_received += n_samples_in_chunk
    
          
    # отсекаем только нужное количество отсчетов
    data = data[:total_samples]
    stims = stims[:total_samples,0]
    
    
    #os.makedirs(results_path)
    
    file = open(result_file, "wb")
    pickle.dump({'eeg': data, 'stim': stims,
            'xml_info': xml_info, 'exp_settings': exp_settings}, file = file)
    file.close()
    
    return

if __name__ == '__main__':
    # проверяем входные параметры
    if(len(sys.argv) > 2):
        RunPreparedProtocol(sys.argv[1], sys.argv[2])
