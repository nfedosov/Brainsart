using AlphaTraining.Pipeline;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Security.AccessControl;
using System.Text;
using System.Text.Json;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;

namespace AlphaTraining
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        string _userName = string.Empty;
        List<PipelineItem> _steps;
        List<String> _scenarios = new List<String>();
        List<PlotView> _plots = new List<PlotView>();

        private int _step = 0;
        public MainWindow()
        {
            InitializeComponent();

            _steps = new List<PipelineItem>() {
                new ExperimentPreparation(this, "Подготовка к калибровке"),
                new BaselineRecorder(this, "Калибровка"),
                new FilterConfigurator(this, "Настройка"),
                new ExperimentSettingsPipeline(this, "Тренинг"),
                //new TerminalStep(this, "Старт"),
            };

            lbStepsProgress.ItemsSource = _steps;
            lbStepsProgress.SelectedIndex = _step;
            lblStepName.Content = _steps[_step].Name;

            PrepareScenarios();
        }

        internal Visualization GetSelectedGameName()
        {
            if(rbPenguin.IsChecked.Value)
            {
                return Visualization.Penguin;
            }
            else if (rbFontain.IsChecked.Value)
            {
                return Visualization.Fontain;
            }

            return Visualization.None;
        }

        private string EncloseWithQuotes(string filename)
        {
            if (filename.StartsWith("\""))
            {
                return filename;
            }

            return "\"" + filename + "\"";

        }

        public bool JumpToStep(int step)
        {
            bool jumpToNextStep = false;

            if (false == _steps[_step].CanMoveForward())
            {
                return false;
            }

            if (_step < _steps.Count)
            {
                if (_step == 0)
                {
                    jumpToNextStep = _steps[_step].Run("");
                }
                else
                {                   
                   if (step == (_steps.Count - 1))
                   {
                       btnNextStep.Content = "Старт!";
                   }
                   else
                   {
                       btnNextStep.Content = "Далее";
                   }

                   jumpToNextStep = _steps[_step].Run(_steps[_step - 1].GetArguments());
                }

                _step = step;
                lbStepsProgress.SelectedIndex = _step;
                lbStepsProgress.Items.Refresh();

                
            }

            if (_step < _steps.Count)
            {
                lblStepName.Content = _steps[_step].Name;
            }
            else
            {
                // уйти в закат
                this.Close();
            }

            return jumpToNextStep;
        }

        private void lbStepsProgress_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            _step = lbStepsProgress.SelectedIndex;
            lblStepName.Content = _steps[_step].Name;
            tabMain.SelectedIndex = _step;
        }

        private void PrepareScenarios()
        {
            lbScenarios.Items.Clear();
            _scenarios.Clear();

            if (Directory.Exists(@"./Data/scenarios"))
            {
                foreach (var filename in Directory.GetFiles(@"./Data/scenarios"))
                {
                    _scenarios.Add(Path.GetFileNameWithoutExtension(filename));
                    lbScenarios.Items.Add(Path.GetFileNameWithoutExtension(filename));
                }
            }
        }

        private void NextStep_Click(object sender, RoutedEventArgs e)
        {
            if (_step <= (_steps.Count))
            {
                bool jumpToNext = JumpToStep(_step + 1);

                while (jumpToNext)
                {
                    jumpToNext = JumpToStep(_step + 1);
                }
            }
        }

        private void btnNewScenario_Click(object sender, RoutedEventArgs e)
        {
            ScenarioMaker scenarioMaker = new ScenarioMaker();
            scenarioMaker.ShowDialog();

            PrepareScenarios();
        }

        private void btnDeleteScenario_Click(object sender, RoutedEventArgs e)
        {
            // Вывести окно "Вы уверены, что хотите удалить сценарий?"

            // Удалить файл
        }

        //================================================================================
        // Открытые функции
        //================================================================================

        public void SetUserName(string userName)
        {
            _userName = userName;

            Title = "Alpha Training. " + userName;
        }

        public string GetUserName()
        {
            return _userName;
        }

        public string GetSelectedProtocolName()
        {
            if ((lbScenarios.Items.Count > 0) && (lbScenarios.SelectedItem != null))
            {
                return Path.GetFullPath(String.Format(@"./Data/scenarios/{0}.json", _scenarios[lbScenarios.SelectedIndex]));
            }

            return String.Empty;
        }

        public void DisplayProtocolBlock(string text, double duration)
        {
            tabMain.SelectedIndex = 1;

            tbProtocolScreen.Text = text;

            Thread.Sleep(Convert.ToInt32(duration * 1000));
        }

        private void lbScenarios_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if ((lbScenarios.Items.Count > 0) && (lbScenarios.SelectedItem != null))
            {
                var scenarioFileName = GetSelectedProtocolName();


                using (StreamReader sr = new StreamReader(File.OpenRead(scenarioFileName)))
                {
                    List<ProtocolBlock> blocks = JsonSerializer.Deserialize<List<ProtocolBlock>>(sr.ReadToEnd());
                    List<ProtocolBlockDescriptor> blocksDescriptors = new List<ProtocolBlockDescriptor>();
                    if (null != blocks)
                    {
                        foreach (var block in blocks)
                        {
                            blocksDescriptors.Add(new ProtocolBlockDescriptor(block));
                        }



                        lbProtocolDetails.ItemsSource = blocksDescriptors;
                        lbProtocolDetails.Items.Refresh();
                    }
                }
            }
        }

        private void btnEditScenario_Click(object sender, RoutedEventArgs e)
        {

        }
               

        public void LoadPlots()
        {
            for (int i = 0; i < 4; i++)
            {
                _plots.Add(new PlotView(i));
            }

            lbPlots.ItemsSource = _plots;
        }

        private void UpdatePlotsView()
        {
            foreach (var plot in _plots)
            {
                plot.SwitchView();
            }

            lbPlots.Items.Refresh();
        }

        private void rbTimeSeries_Checked(object sender, RoutedEventArgs e)
        {
            if (_plots.Count != 0)
            {
                rbSpectrum.IsChecked = false;

                UpdatePlotsView();
            }
        }

        private void rbSpectrum_Checked(object sender, RoutedEventArgs e)
        {
            if (_plots.Count != 0)
            {
                rbTimeSeries.IsChecked = false;

                UpdatePlotsView();
            }
        }

        public int GetSelectedPlot()
        {
            for(int i = 0; i < _plots.Count; i++)
            {
                if(_plots[i].IsSelected)
                {
                    return i;
                }
            }

            return -1;
        }

        public string GetSpatialFilerLowerFreq()
        {
            return tbHighPassFilter.Text.Replace(',', '.');
        }

        public string GetSpatialFilerHighFreq()
        {
            return tbLowPassFilter.Text.Replace(',', '.');
        }

        public string GetSpatialFilterType()
        {
            return cbSpatialFilterType.Text;
        }

        public string GetTemporalFilterType()
        {
            return cbTemporalFilterType.Text;
        }

        public string GetCentralFrequencyValue()
        {
            if(cbAutoCentralFreq.IsChecked.Value)
            {
                return "auto_" + cbRythmType.SelectedValue.ToString();
            }
            else
            {
                return tbCentralFreqVal.Text;
            }
        }

        private void cbAutoCentralFreq_Unchecked(object sender, RoutedEventArgs e)
        {
            if ((gbAutoCentralFreq != null) && (gbExactCentralFreq != null))
            {
                gbAutoCentralFreq.Visibility = Visibility.Hidden;
                gbExactCentralFreq.Visibility = Visibility.Visible;
            }
        }

        private void cbAutoCentralFreq_Checked(object sender, RoutedEventArgs e)
        {
            if ((gbAutoCentralFreq != null) && (gbExactCentralFreq != null))
            {
                gbAutoCentralFreq.Visibility = Visibility.Visible;
                gbExactCentralFreq.Visibility = Visibility.Hidden;
            }
        }
    }
}
