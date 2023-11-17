using System;
using System.IO;
using System.Windows;
using System.Text.Json;

namespace AlphaTraining
{
    /// <summary>
    /// Interaction logic for NewUserDialog.xaml
    /// </summary>
    public partial class NewUserDialog : Window
    {
        UserCard userCard = new UserCard();

        string[] genders = { "Мужской", "Женский" };

        public NewUserDialog()
        {
            InitializeComponent();
            cbUserGender.ItemsSource = genders;
            cbUserGender.SelectedIndex = 0;
            tbUserName.Focus();
        }

        private void btnDone_Click(object sender, RoutedEventArgs e)
        {
            // Проверить, заполнена ли форма
            if(tbUserName.Text == string.Empty)
            {
                MessageBox.Show("Введите имя пользователя!");
                return;
            }

            if (tbUserSurname.Text == string.Empty)
            {
                MessageBox.Show("Введите фамилию пользователя!");
                return;
            }

            if (tbUserAge.Text == string.Empty)
            {
                MessageBox.Show("Введите возраст пользователя!");
                return;
            }

            try
            {
                userCard.Age = Convert.ToInt32(tbUserAge.Text);
            }
            catch (FormatException)
            {
                MessageBox.Show("Введите корректный возраст пользователя!");
                return;
            }

            userCard.Gender = genders[cbUserGender.SelectedIndex];


            // Все хорошо. Сохраняем имя, создаем папку пользователя 
            Directory.CreateDirectory(Path.Combine(@".\Data\users", userCard.GenerateId()));

            // Создаем файл с карточкой пользователя
            userCard.Serialize();

            this.DialogResult = true;

            // и закрываем за собой окно
            Close();
        }

        private void btnCancel_Click(object sender, RoutedEventArgs e)
        {
            this.DialogResult = false;
            Close();
        }

        public UserCard GetUrerCard()
        {
            return userCard;
        }

        private void tbUserSurname_TextChanged(object sender, System.Windows.Controls.TextChangedEventArgs e)
        {
            userCard.Surname = tbUserSurname.Text;
            lblUserId.Content = userCard.GenerateId();
        }

        private void tbUserName_TextChanged(object sender, System.Windows.Controls.TextChangedEventArgs e)
        {
            userCard.Name = tbUserName.Text;
            lblUserId.Content = userCard.GenerateId();
        }
    }
}
