using AlphaTraining.Pipeline;
using System;
using System.Collections.Generic;
using System.Diagnostics;
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
        UserCard _userCard;
        string _sessionDate = string.Empty;
        List<PipelineItem> _steps;
        List<String> _scenarios = new List<String>();
        List<PlotView> _plots = new List<PlotView>();
        List<String> _lslStreams = new List<string>();

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

            UpdateTab();

        }

        #region MainLogic

        public bool JumpToStep(int step)
        {
            bool jumpToNextStep = false;

            // Проверить валидность заполенных полей формы, прежде чем переходить к следующему шагу
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
                UpdateTab();

            }

            if (_step < _steps.Count)
            {
                lblStepName.Content = _steps[_step].Name;
            }
            else
            {
                Close();
            }

            return jumpToNextStep;
        }

        private void lbStepsProgress_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            _step = lbStepsProgress.SelectedIndex;
            lblStepName.Content = _steps[_step].Name;
            tabMain.SelectedIndex = _step;
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

        private void UpdateTab()
        {
            switch (tabMain.SelectedIndex)
            {
                case 0:
                    PrepareScenarios();
                    break;

                case 3:
                    UbpdateSessionStartTab();
                    break;
            }
        }

        public void SetUserCard(UserCard userCard)
        {
            _userCard = userCard;
            _sessionDate = DateTime.Now.ToString("dd_MMMM_yyyy");

            Title = "Alpha Training. " + _userCard.Id;
        }

        public string GetUserName()
        {
            return _userCard.Id;
        }

        public string GetSessionDate()
        {
            return _sessionDate;
        }

        #endregion

        #region ProtocolsTab

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

        public string GetSelectedProtocolName()
        {
            if ((lbScenarios.Items.Count > 0) && (lbScenarios.SelectedItem != null))
            {
                return Path.GetFullPath(String.Format(@"./Data/scenarios/{0}.json", _scenarios[lbScenarios.SelectedIndex]));
            }

            return String.Empty;
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

        #endregion

        #region CalibrationTab


        #endregion

        #region FiltersSetupTab

        public void LoadPlots()
        { 
            int fileCount = Directory.GetFiles(Path.GetFullPath(string.Format(".\\Data\\temp\\")), "*Timeseries*", SearchOption.TopDirectoryOnly).Length;
            for (int i = 0; i < fileCount; i++)
            {
                _plots.Add(new PlotView(i));
            }

            lbPlots.ItemsSource = _plots;

            cbTemporalFilterType.Items.Add("CFIR");

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
            for (int i = 0; i < _plots.Count; i++)
            {
                if (_plots[i].IsSelected)
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

        public int GetTemporalFilterType()
        {
            if (cbTemporalFilterType.Text == "CFIR")
            {
                return 1;
            }

            if (cbTemporalFilterType.Text == "Фильтр Калмана")
            {
                return 0;
            }
            return 0;

        }

        public string GetCentralFrequencyValue()
        {
            if (cbAutoCentralFreq.IsChecked.Value)
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
                gbAutoCentralFreq.Visibility = Visibility.Visible;
                gbExactCentralFreq.Visibility = Visibility.Hidden;
            }
        }

        private void cbAutoCentralFreq_Checked(object sender, RoutedEventArgs e)
        {
            if ((gbAutoCentralFreq != null) && (gbExactCentralFreq != null))
            {
                gbAutoCentralFreq.Visibility = Visibility.Hidden;
                gbExactCentralFreq.Visibility = Visibility.Visible;
            }
        }

        private void btnPlotBaseline_Click(object sender, RoutedEventArgs e)
        {
            // Запустить Python-скрипт, который запустит запись согласно меткам в протоколе
            ProcessStartInfo startInfo = new ProcessStartInfo();
            startInfo.FileName = SystemVariables.Instance.PythonPath;
            startInfo.Arguments = @"./Data/scripts/PlotBaseline.py " + _steps[_step - 1].GetArguments();
            startInfo.UseShellExecute = false;
            startInfo.CreateNoWindow = true;

            var process = Process.Start(startInfo);
            if (null != process)
            {
                process.WaitForExit();
            }
        }

        private void lbPlots_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {

        }

        #endregion

        #region SessionStartTab

        private void UbpdateSessionStartTab()
        {
            UpdateLslStreamsList();
        }

        private void UpdateLslStreamsList()
        {
            _lslStreams.Clear();
            cbLslStreams.Items.Clear();

            int nLslStreamMaxName = 256;
            StringBuilder sbLslStreamName = new StringBuilder(nLslStreamMaxName);

            bool result = NativeMethods.LslNativeFunctions.GetFirstLslStream((uint)nLslStreamMaxName, sbLslStreamName);
            while(result)
            {
                _lslStreams.Add(sbLslStreamName.ToString());
                cbLslStreams.Items.Add(sbLslStreamName.ToString());

                result = NativeMethods.LslNativeFunctions.GetNextLslStream((uint)nLslStreamMaxName, sbLslStreamName);
            }

            for(int i = 0; i < _lslStreams.Count; i++)
            {
                if(_lslStreams[i].Contains("Data"))
                {
                    cbLslStreams.SelectedIndex = i;
                    break;
                }
            }
        }

        internal Visualization GetSelectedGameName()
        {
            if (rbPenguin.IsChecked.Value)
            {
                return Visualization.Penguin;
            }
            else if (rbFontain.IsChecked.Value)
            {
                return Visualization.Fontain;
            }

            return Visualization.None;
        }

        public string GetLslStreamName()
        {
            if( -1 != cbLslStreams.SelectedIndex)
            {
                return _lslStreams[cbLslStreams.SelectedIndex];
            }

            return string.Empty;
        }

        #endregion

    }
}
