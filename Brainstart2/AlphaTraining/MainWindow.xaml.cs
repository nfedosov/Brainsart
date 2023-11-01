using AlphaTraining.Pipeline;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Security.AccessControl;
using System.Text;
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
        List<PipelineItem> _steps = new List<PipelineItem>() {
            new PipelineItem("Подготовка эксперимента"),
            new BaselineRecorder("Запись baseline"),
            new FilterConfigurator("Настройка"),
            new ExperimentPipeline("Тренинг"),
        };

        

        private int _step = 0;
        public MainWindow()
        {
            InitializeComponent();

            lbStepsProgress.ItemsSource = _steps;
            lbStepsProgress.SelectedIndex = _step;
            lblStepName.Content = _steps[_step].Name;

            PrepareScenarios();
        }

        public void JumpToStep(int step)
        {
            if ((step > 0) && (step < _steps.Count))
            {

                switch (step)
                {                    
                    case 1:
                        this.IsEnabled = false;
                        _steps[step-1].Run("");

                        this.IsEnabled = true;
                        break;

                    default:
                        

                        if (_step == (_steps.Count - 1))
                        {
                            btnNextStep.Content = "Поехали!";
                        }
                        else
                        {
                            btnNextStep.Content = "Далее";
                        }

                        this.IsEnabled = false;
                        _steps[step-1].Run(_steps[step - 2].GetArguments());

                        this.IsEnabled = true;

                        break;
                }

                _step = step;
                lbStepsProgress.SelectedIndex = _step;
                lblStepName.Content = _steps[_step].Name;
            }
            else
            {
                // выполнить последний шаг и уйти в закат
                _steps[_steps.Count - 1].Run(_steps[_steps.Count - 2].GetArguments());
                
            }
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

            foreach (var filename in Directory.GetFiles(@"./Data/scenarios"))
            {
                lbScenarios.Items.Add(Path.GetFileNameWithoutExtension(filename));
            }
        }

        private void NextStep_Click(object sender, RoutedEventArgs e)
        {
            if(_step <= (_steps.Count))
            {
                // Проверить корректность текущей форсы
                if(CurrentTabIsValid())
                {
                    JumpToStep(_step + 1);
                }
            }
        }

        private bool CurrentTabIsValid()
        {
            switch (_step)
            {
                case 0:
                    // Сценарий доолжен быть выбран
                    if ((lbScenarios.Items.Count > 0) && (lbScenarios.SelectedItem != null))
                    {
                        return true;
                    }
                    else
                    {
                        MessageBox.Show("Не выбран ни один сценарий!");
                    }
                    break;

                default:
                    return true;
            }

            return false;
        }

        private void btnNewScenario_Click(object sender, RoutedEventArgs e)
        {
            ScenarioMaker scenarioMaker = new ScenarioMaker();
            scenarioMaker.ShowDialog();
        }

        private void btnDeleteScenario_Click(object sender, RoutedEventArgs e)
        {
            // Вывести окно "Вы уверены, что хотите удалить сценарий?"

            // Удалить файл
        }
    }
}
