using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
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
    /// Interaction logic for EndOfSession.xaml
    /// </summary>
    public partial class EndOfSessionDialog : Window
    {
        AlphaTrainingAction _nextAction = AlphaTrainingAction.OptionSelection;

        public EndOfSessionDialog()
        {
            InitializeComponent();
        }

        private void btnExit_Click(object sender, RoutedEventArgs e)
        {
            _nextAction = AlphaTrainingAction.Close;
            Close();
        }

        private void btnMainMenu_Click(object sender, RoutedEventArgs e)
        {
            _nextAction = AlphaTrainingAction.OptionSelection;
            Close();
        }

        private void btnResults_Click(object sender, RoutedEventArgs e)
        {
            _nextAction = AlphaTrainingAction.Results;
            Close();
        }

        public AlphaTrainingAction GetAction()
        {
            return _nextAction;
        }
    }
}
