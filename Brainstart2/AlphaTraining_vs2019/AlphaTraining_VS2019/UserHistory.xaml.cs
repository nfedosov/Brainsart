using System;
using System.Collections.Generic;
using System.IO;
using System.Diagnostics;
using System.Windows;

namespace AlphaTraining
{
    /// <summary>
    /// Interaction logic for UserHistory.xaml
    /// </summary>
    public partial class UserHistory : Window
    {
        UserCard _userCard;
        List<string> _sessionsList = new List<string>();
        List<PlotPost> _postplots = new List<PlotPost>() {
            new PlotPost(0),
            new PlotPost(1),
        };


        public UserHistory()
        {
            InitializeComponent();
        }



        public void LoadPostPlots()
        {      
            lbPost.ItemsSource = _postplots;
            lbPost.Items.Refresh();
        }


        



        internal void SetUserCard(UserCard userCard)
        {
            _userCard = userCard;
            lblUserName.Content = _userCard.Name;
            lblUserSurname.Content = _userCard.Surname;
            lblUserGender.Content = _userCard.Gender;
            lblUserAge.Content = _userCard.Age;
            lblUserId.Content = _userCard.Id;

            // Загрузить список сеансов
            List<string> records_list = new List<string>();

            foreach (var sessionDate in Directory.EnumerateDirectories(String.Format(@".\Data\users\{0}\", _userCard.Id)))
            {
                _sessionsList.Add(Path.GetFileName(sessionDate).Replace('_', ' '));
                foreach (var fif_file in Directory.EnumerateFiles(sessionDate))
                {
                    if (fif_file.Contains(".fif"))
                    {
                        records_list.Add(Path.GetFullPath(fif_file));
                    }
                }    

            }



            ProcessStartInfo startInfo = new ProcessStartInfo();
            startInfo.FileName = SystemVariables.Instance.PythonPath;
            startInfo.FileName = "C:/Program Files/Spyder/Python/python.exe";
            startInfo.Arguments = @"./Data/scripts/postAnalysisSession.py "
            + String.Join(" ", records_list);

            startInfo.UseShellExecute = false;
            startInfo.CreateNoWindow = true;
            var process = Process.Start(startInfo);
            if (null != process)
            {
                process.WaitForExit();
            }

           
            lbSessionsList.ItemsSource = _sessionsList;
        }

        private void lbSessionsList_SelectionChanged(object sender, System.Windows.Controls.SelectionChangedEventArgs e)
        {

        }
    }




}
