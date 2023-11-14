using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace AlphaTraining
{
    internal class ConditionalProtocolBlock : ProtocolBlock
    {
        static int freeConditionalId = 0;

        public static ConditionalProtocolBlock CreateProtocolBlock(string name, int blockId, double duration, string message)
        {
            return new ConditionalProtocolBlock(name, blockId, freeConditionalId++, duration, message);
        }

        public ConditionalProtocolBlock(string name, int blockId, int waitId, double duration, string message)
            : base(blockId, String.Format("Cond{0}", waitId), message, duration, blockId, name)
        {
            
        }
    }
}
