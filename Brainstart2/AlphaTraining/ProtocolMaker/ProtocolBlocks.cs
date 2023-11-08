using System;
using System.Collections.Generic;


namespace AlphaTraining
{
    public class ProtocolBlocks
    {
        static readonly ProtocolBlocks Instance = new ProtocolBlocks();
        List<ProtocolBlock> blocks = new List<ProtocolBlock>();
        private int _freeConditionId = 0;

        private ProtocolBlocks() { }

        public static ProtocolBlocks GetInstance() 
        { 
            return Instance; 
        }

        public void AddProtocolBlock(string name, double duration)
        {
            blocks.Add(WaitProtocolBlock.CreateProtocolBlock(name, ++_freeConditionId, duration));
        }

        public void AddProtocolBlock(string name, double duration, string message)
        {
            blocks.Add(ConditionalProtocolBlock.CreateProtocolBlock(name, ++_freeConditionId, duration, message));
        }

        public List<ProtocolBlock> GetBlocks()
        {
            return blocks;
        }
    }
}
