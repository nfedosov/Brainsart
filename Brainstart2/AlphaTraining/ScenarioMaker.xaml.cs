using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;

namespace AlphaTraining
{
    /// <summary>
    /// Interaction logic for ScenarioMaker.xaml
    /// </summary>
    public partial class ScenarioMaker : Window
    {
        public ScenarioMaker()
        {
            InitializeComponent();
        }

        private void btnCancel_Click(object sender, RoutedEventArgs e)
        {
            this.Close();
        }

        private void btnDone_Click(object sender, RoutedEventArgs e)
        {
            // Проверить форму
            if(tbScenarioName.Text == String.Empty)
            {
                MessageBox.Show("Введите имя сценария!");
                return;
            }

            if(lbScript.Items.Count == 0)
            {
                MessageBox.Show("Сценарий должен содержать хотя бы один элемент!!");
                return;
            }

            // Сохранить сценарий в файл

            this.Close();
        }
    }
}
