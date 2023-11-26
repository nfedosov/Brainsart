using System;
using System.Windows;

namespace AlphaTraining
{
    public enum AlphaTrainingAction
    {
        // Главное окно выбора опции
        OptionSelection,

        // Окно заполнния данных нового пользователя
        NewUser,

        // Окно нового сеанса
        MainWindow,

        // Окно просмотра результатов
        Results,

        // Выход из приложения
        Close,
    }

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
                        
            

            AlphaTrainingAction currentAction = AlphaTrainingAction.OptionSelection;
            UserCard userCard = null;
            
            do
            {
                OptionSelectorWindow optionSelectorWindow = new OptionSelectorWindow();
                NewUserDialog newUserDialog = new NewUserDialog();
                MainWindow wnd = new MainWindow();

                switch (currentAction)
                {
                    case AlphaTrainingAction.OptionSelection:
                        // Сначала показать окно выбора режима работы
                        
                        optionSelectorWindow.ShowDialog();

                        // Потом определить, что выбрал пользователь
                        currentAction = optionSelectorWindow.GetSelectedAction();

                        // получить карточку выбранного пользователя, если есть
                        userCard = optionSelectorWindow.GetUserCard();
                        break;

                    case AlphaTrainingAction.NewUser:

                        // Показать окно добавления нового пользователя
                        newUserDialog.ShowDialog();
                        if (true == newUserDialog.DialogResult)
                        {
                            // В случае завершения ввода данных пользователя принудительно переходим к новому сеансу пользователя
                            userCard = newUserDialog.GetUrerCard();
                            currentAction = AlphaTrainingAction.MainWindow;
                        }
                        else
                        {
                            // В противном случае переходим в окно выбора опции
                            currentAction = AlphaTrainingAction.OptionSelection;
                        }

                        break;

                    case AlphaTrainingAction.MainWindow:
                        
                        wnd.SetUserCard(userCard);
                        wnd.ShowDialog();

                        // По закрытию окна узнать дальнейшие действия пользователя
                        // Показать завершающее диалоговое окно 
                        EndOfSessionDialog endOfSessionDialog = new EndOfSessionDialog();
                        endOfSessionDialog.ShowDialog();

                        currentAction = endOfSessionDialog.GetAction();
                        break;

                    case AlphaTrainingAction.Results:
                        UserHistory userHistory = new UserHistory();
                        userHistory.SetUserCard(userCard);
                        userHistory.ShowDialog();

                        // По завершению просмотра результатов всегда возвращаемся к основному окну
                        currentAction = AlphaTrainingAction.OptionSelection;
                        break;

                    case AlphaTrainingAction.Close:
                        break;
                }

            } while (currentAction != AlphaTrainingAction.Close);

            Shutdown();
        }
    }
}
