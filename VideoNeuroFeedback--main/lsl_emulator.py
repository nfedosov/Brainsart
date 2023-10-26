import time
from pylsl import StreamInfo, StreamOutlet
import random

def emulate_lsl_stream():
    # Создаем новый поток (info) с именем 'SpeedStream', типом 'PlaybackSpeed' и одним каналом
    info = StreamInfo('SpeedStream', 'PlaybackSpeed', 1, 0, 'float32', 'myuniquesourceid23443')

    # Создаем outlet (выход) для передачи данных
    outlet = StreamOutlet(info)

    print("LSL stream 'SpeedStream' started. Sending data...")

    while True:
        # Генерируем случайную скорость воспроизведения в диапазоне от 0.5 до 2.0
        playback_speed = random.choice([10, 10, 900])

        # Отправляем значение скорости через LSL
        outlet.push_sample([playback_speed])

        # Пауза 5 секунд перед отправкой следующего значения
        time.sleep(2)

if __name__ == '__main__':
    emulate_lsl_stream()

