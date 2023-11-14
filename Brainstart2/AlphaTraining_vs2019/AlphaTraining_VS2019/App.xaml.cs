using System;
using System.Windows;

namespace AlphaTraining
{
    /// <summary>
    /// Interaction logic for App.xaml
    /// </summary>
    public partial class App : Application
    {
        private void Application_Startup(object sender, StartupEventArgs e)
        {
            // Проверить, установлен ли Python:
            if(SystemVariables.Instance.PythonPath == String.Empty)
            {
                MessageBox.Show("Не найден интерпретатор Python!");
                return;
            }

            MainWindow wnd = new MainWindow();

            // Сначала показать окно выбора режима работы
            OptionSelectorWindow optionSelectorWindow = new OptionSelectorWindow();
            optionSelectorWindow.ShowDialog();

            // Если выбран режим записи нового пользователя, сохранить введенное имя пользователя
            switch(optionSelectorWindow.GetApplicationMode())
            {
                case ApplicationMode.NewUser:
                                        
                    wnd.SetUserName(optionSelectorWindow.GetUserName());

                    if (e.Args.Length == 1)
                    {
                        int stepFromCmd = Convert.ToInt32(e.Args[0]);
                        if (stepFromCmd > 0)
                        {
                            wnd.JumpToStep(stepFromCmd - 1);
                        }
                    }

                    wnd.Show();

                    break;
            }            
        }
    }
}
