import pyttsx3
import os

if __name__ == '__main__':
    engine = pyttsx3.init()

    fullPath = os.path.join(os.getcwd(), "bar.wav")

    engine.save_to_file('Закройте глаза', 'instr.mp3')
    engine.runAndWait()