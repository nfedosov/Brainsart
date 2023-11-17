using System;
using System.Collections.Generic;
using System.IO;
using System.Text.Json;
using System.Windows;

namespace AlphaTraining
{
    internal class Protocol
    {
        static readonly Protocol Instance = new Protocol();
        List<ProtocolBlock> blocks = new List<ProtocolBlock>();

        private Protocol() { }

        public static Protocol GetInstance()
        {
            return Instance;
        }

        public void Add(ProtocolBlock protocolBlock)
        {
            blocks.Add(protocolBlock);
        }

        public List<ProtocolBlock> GetBlocks()
        {
            return blocks;
        }

        internal void Remove(int selectedBlockIndex)
        {
            if(selectedBlockIndex < blocks.Count)
            {
                blocks.RemoveAt(selectedBlockIndex);
            }
        }

        internal void MoveUp(int selectedBlockIndex)
        {
            if ((selectedBlockIndex < blocks.Count)
                &&
                (selectedBlockIndex > 0))
            {
                var selectedBlock = blocks[selectedBlockIndex];

                blocks.RemoveAt(selectedBlockIndex);
                blocks.Insert(selectedBlockIndex - 1, selectedBlock);
            }
        }

        internal void MoveDown(int selectedBlockIndex)
        {
            if (selectedBlockIndex < blocks.Count - 1)
            {
                var selectedBlock = blocks[selectedBlockIndex];

                blocks.RemoveAt(selectedBlockIndex);
                blocks.Insert(selectedBlockIndex + 1, selectedBlock);
            }
        }

        public bool IsValid()
        {
            // Протокол долже содержать как минимум ДВА условия

            // Тут можно использовать LINQ
            int nConditionBlocksCount = 0;
            foreach (var protocolBlock in blocks)
            {
                if (protocolBlock.BlockType == ProtocolBlockType.Condition)
                {
                    nConditionBlocksCount++;
                }
            }

            if(nConditionBlocksCount < 2)
            {

                // TODO: сделать по нормальному(
                MessageBox.Show("Протокол должен содержать как минимум 2 условия!");
                return false;
            }

            return true;
        }

        internal void Serialize(string fileName)
        {
            using (StreamWriter sw = new StreamWriter(File.OpenWrite(fileName)))
            {
                var options = new JsonSerializerOptions
                {
                    WriteIndented = true
                };

                sw.Write(JsonSerializer.Serialize(blocks, options));
            }
        }
    }
}
