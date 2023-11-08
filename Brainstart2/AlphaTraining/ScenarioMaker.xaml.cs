using System;
using System.Windows;

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

            lbBlocks.ItemsSource = ProtocolBlocks.GetInstance().GetBlocks();
            lbScript.ItemsSource = Protocol.GetInstance().GetBlocks();
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
            Protocol.GetInstance().Serialize(@"./Data/scenarios/" + tbScenarioName.Text + ".json");

            this.Close();
        }

        private void btnAddCondition_Click(object sender, RoutedEventArgs e)
        {
            // Показать форму создания блока
            var creatorForm = new ProtocolBlockCreator();
            creatorForm.SetBlockType(BlockType.Condition);
            creatorForm.ShowDialog();

            lbBlocks.Items.Refresh();
        }

        private void btnAddWait_Click(object sender, RoutedEventArgs e)
        {
            // Показать форму создания блока
            var creatorForm = new ProtocolBlockCreator();
            creatorForm.SetBlockType(BlockType.Wait);
            creatorForm.ShowDialog();
            
            lbBlocks.Items.Refresh();
        }

        private void btnAddBlock_Click(object sender, RoutedEventArgs e)
        {
            if(lbBlocks.SelectedItem != null)
            {
                int selectedBlockIndex = lbBlocks.SelectedIndex;

                // Добавить новый блок в конец
                Protocol.GetInstance().Add(ProtocolBlocks.GetInstance().GetBlocks()[selectedBlockIndex]);

                lbScript.Items.Refresh();

                return;
            }

            MessageBox.Show("Выберите блок, который необходимо добавить в протокол!");
        }

        private void btnRemoveBlock_Click(object sender, RoutedEventArgs e)
        {
            if (lbScript.SelectedItem != null)
            {
                int selectedBlockIndex = lbScript.SelectedIndex;

                Protocol.GetInstance().Remove(selectedBlockIndex);

                lbScript.Items.Refresh();

                return;
            }

            MessageBox.Show("Выберите блок, который необходимо добавить в протокол!");
        }

        private void btnMoveUp_Click(object sender, RoutedEventArgs e)
        {
            if (lbScript.SelectedItem != null)
            {
                int selectedBlockIndex = lbScript.SelectedIndex;

                Protocol.GetInstance().MoveUp(selectedBlockIndex);

                lbScript.Items.Refresh();

                return;
            }

            MessageBox.Show("Выберите блок, который необходимо переместить!");
        }

        private void btnMoveDown_Click(object sender, RoutedEventArgs e)
        {
            if (lbScript.SelectedItem != null)
            {
                int selectedBlockIndex = lbScript.SelectedIndex;

                Protocol.GetInstance().MoveDown(selectedBlockIndex);

                lbScript.Items.Refresh();

                return;
            }

            MessageBox.Show("Выберите блок, который необходимо переместить!");
        }
    }
}
