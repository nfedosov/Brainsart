using System;
using System.Collections.Generic;
using System.IO;
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
        List<PlotPost> _postplots = new List<PlotPost>();


        public UserHistory()
        {
            InitializeComponent();
            LoadPostPlots();
        }



        public void LoadPostPlots()
        {
            _postplots.Add(new PlotPost());
            

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
            foreach(var sessionDate in Directory.EnumerateDirectories(String.Format(@".\Data\users\{0}\", _userCard.Id)))
            {
                _sessionsList.Add(Path.GetFileName(sessionDate).Replace('_', ' '));
            }

            lbSessionsList.ItemsSource = _sessionsList;
        }
    }




}
