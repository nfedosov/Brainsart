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
        public BaselineRecording()
        {
            InitializeComponent();
        }

       private void UpdateInstruction(object obj)
        {
            ProtocolBlock protocolBlock = obj as ProtocolBlock;

            Dispatcher.Invoke((Action)delegate () 
            {
                SystemSounds.Asterisk.Play();
                tbScreen.Content = protocolBlock.message;
            });
        }

        private void FinalizeRecording(object obj)
        {
            ProtocolBlock protocolBlock = obj as ProtocolBlock;

            Dispatcher.Invoke((Action)delegate ()
            {
                tbScreen.Content = "Запись baseline закончена!";
                SystemSounds.Asterisk.Play();
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

        public async void PlayScenario(List<ProtocolBlock> blocks)
        {
            Play(blocks);
        }
    }
}
