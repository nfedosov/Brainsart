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
    /// Interaction logic for ProtocolBlockCreator.xaml
    /// </summary>
    public partial class ProtocolBlockCreator : Window
    {
        BlockType _blockType;

        public ProtocolBlockCreator()
        {
            InitializeComponent();

            tbBlockName.Focus();
        }

        public void SetBlockType(BlockType blockType)
        {
            _blockType = blockType;

            switch (_blockType) 
            { 
                case BlockType.Wait:
                    gbMessage.Visibility = Visibility.Hidden;
                    tbBlockName.Text = "Pause";
                    this.Title = "Новый блок ожидания";
                    break;

                case BlockType.Condition:
                    gbMessage.Visibility = Visibility.Visible;
                    tbBlockName.Text = String.Empty;
                    this.Title = "Новый блок условия";
                    break;
            }
        }

        private void btnOk_Click(object sender, RoutedEventArgs e)
        {
            string blockName = string.Empty;
            double duration = 0;
            string message = string.Empty;

            // Проверить корректность формы
            if (tbBlockName.Text == String.Empty)
            {
                MessageBox.Show("Введите имя блока!");
                return;
            }

            blockName = tbBlockName.Text;

            if (tbDuration.Text == String.Empty)
            {
                MessageBox.Show("Введите длительность блока в секундах!");
                return;
            }

            duration = Convert.ToDouble(tbDuration.Text);

            switch (_blockType)
            {
                case BlockType.Condition:
                    if (tbMessage.Text == String.Empty)
                    {
                        MessageBox.Show("Введите сообщение!");
                        return;
                    }

                    message = tbMessage.Text;
                    break;
            }
            // Сформировать новый блок протокола

            switch (_blockType)
            {
                case BlockType.Wait:
                    ProtocolBlocks.GetInstance().AddProtocolBlock(blockName, duration);
                    break;

                case BlockType.Condition:
                    ProtocolBlocks.GetInstance().AddProtocolBlock(blockName, duration, message);
                    break;
            }

            this.Close();
        }

        private void btnCancel_Click(object sender, RoutedEventArgs e)
        {
            this.Close();
        }
    }
}
