using System.IO;
using System.Windows;

namespace AlphaTraining
{
    /// <summary>
    /// Interaction logic for NewUserDialog.xaml
    /// </summary>
    public partial class NewUserDialog : Window
    {
        string _userName = string.Empty;
        public NewUserDialog()
        {
            InitializeComponent();

            tbUserName.Focus();
        }

        private void btnDone_Click(object sender, RoutedEventArgs e)
        {
            // Проверить, есть ли пользователи с данным именем

            // Перечислить каталоги в папке с пользователями
            foreach (var filename in Directory.GetDirectories(@".\Data\users"))
            {
                if (tbUserName.Text == Path.GetFileName(filename))
                {
                    MessageBox.Show("Пользователь с указанным именем уже существует!", "Ошибка", MessageBoxButton.OK, MessageBoxImage.Error);
                    return;
                }
            }

            // Все хорошо. Сохраняем имя, создаем папку пользователя и закрываем за собой окно
            Directory.CreateDirectory(Path.Combine(@".\Data\users", tbUserName.Text));

            _userName = tbUserName.Text;
            this.DialogResult = true;
            Close();
        }

        private void btnCancel_Click(object sender, RoutedEventArgs e)
        {
            this.DialogResult = false;
            Close();
        }

        public string GetUrerName()
        {
            return _userName;
        }
    }
}
