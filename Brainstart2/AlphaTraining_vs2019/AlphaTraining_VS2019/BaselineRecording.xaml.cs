using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Media;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;

namespace AlphaTraining
{
    /// <summary>
    /// Interaction logic for BaselineRecording.xaml
    /// </summary>
    public partial class BaselineRecording : Window
    {
        List<ProtocolBlock> _blocks;

        public BaselineRecording()
        {
            InitializeComponent();

        }

       private void UpdateInstruction(object obj)
        {
            ProtocolBlock protocolBlock = obj as ProtocolBlock;

            Dispatcher.Invoke((Action)delegate ()
            {
                Beep();
                tbScreen.Content = protocolBlock.message;
            });
        }

        private static void Beep()
        {
            MediaPlayer snd = new MediaPlayer();
            snd.Open(new Uri(".//Beep.wav", UriKind.Relative));
            snd.Volume = 0.5;
            snd.Play();
        }

        private void FinalizeRecording(object obj)
        {
            ProtocolBlock protocolBlock = obj as ProtocolBlock;

            Dispatcher.Invoke((Action)delegate ()
            {
                tbScreen.Content = "Калибровка закончена!";
                Beep();
                WindowStyle = WindowStyle.SingleBorderWindow;
            });
        }

        private async Task Play(List<ProtocolBlock> blocks)
        {
            // Code to remove close box from window
            WindowStyle = WindowStyle.ToolWindow;
           
            int cumulativeDelay = 0;
            Timer timer;

            // "Проиграть" сценарий
            foreach (var protocolBlock in blocks)
            {
                tbScreen.Content = protocolBlock.message;

                timer = new Timer(new TimerCallback(UpdateInstruction), protocolBlock, cumulativeDelay, 0);
                cumulativeDelay += (int)(protocolBlock.duration * 1000);
            }

            timer = new Timer(new TimerCallback(FinalizeRecording), null, cumulativeDelay, 0);
        }

        public void SetScenario(List<ProtocolBlock> blocks)
        {
            _blocks = blocks;
            
        }

        private async void Window_Loaded(object sender, RoutedEventArgs e)
        {
            await Play(_blocks);
        }
    }
}
