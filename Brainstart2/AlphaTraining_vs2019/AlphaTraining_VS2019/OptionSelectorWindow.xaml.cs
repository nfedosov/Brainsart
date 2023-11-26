using System.Collections.Generic;
using System.IO;
using System.Windows;
using System.Windows.Documents;

namespace AlphaTraining
{


    /// <summary>
    /// Interaction logic for OptionSelectorWindow.xaml
    /// </summary>
    public partial class OptionSelectorWindow : Window
    {
        UserCard _userCard = null;
        List<string> _usersList = new List<string>();

        AlphaTrainingAction _selectedAction = AlphaTrainingAction.Close;

        public OptionSelectorWindow()
        {
            InitializeComponent();

            // Перечислить существующих пользователей
            foreach (var filename in Directory.GetDirectories(@".\Data\users"))
            {
                _usersList.Add(Path.GetFileName(filename));
            }

            _usersList.Add("<Новый пользователь>");

            lbUsersList.ItemsSource = _usersList;
        }

        private void CreateNewUser()
        {
            _selectedAction = AlphaTrainingAction.NewUser;
            Close();
        }

        private void LoadUserCard()
        {
            string userId = _usersList[lbUsersList.SelectedIndex];
            _userCard = UserCard.Load(userId);

            Close();
        }

        private void btnAnalysis_Click(object sender, RoutedEventArgs e)
        {
            ChooseOption();
        }

        public AlphaTrainingAction GetSelectedAction()
        {
            return _selectedAction;
        }

        public UserCard GetUserCard()
        {
            return _userCard;
        }

        private void ChooseOption()
        {
            if (lbUsersList.SelectedItem != null)
            {
                if (lbUsersList.SelectedIndex == (_usersList.Count - 1))
                {
                    // Добавить нового пользователя
                    CreateNewUser();
                }
                else
                {
                    // Загрузить карточку выбранного пользователя
                    NewSession();
                }
            }
        }

        private void NewSession()
        {
            LoadUserCard();
            _selectedAction = AlphaTrainingAction.MainWindow;

            this.Hide();
            //Close();
        }

        private void btnHistory_Click(object sender, RoutedEventArgs e)
        {
            LoadUserCard();
            _selectedAction = AlphaTrainingAction.Results;
            Close();
        }

        private void lbUsersList_SelectionChanged(object sender, System.Windows.Controls.SelectionChangedEventArgs e)
        {
            if ((lbUsersList.SelectedIndex != -1))
            {
                // Если выбран существующий пользователь, показать кнопки просмотра истории сеансов
                if (lbUsersList.SelectedIndex == (lbUsersList.Items.Count - 1))
                {
                    spExisingUser.Visibility = Visibility.Hidden;
                    spNewUser.Visibility = Visibility.Visible;
                }
                else
                {
                    spExisingUser.Visibility = Visibility.Visible;
                    spNewUser.Visibility = Visibility.Hidden;
                }
            }
        }
    }
}
