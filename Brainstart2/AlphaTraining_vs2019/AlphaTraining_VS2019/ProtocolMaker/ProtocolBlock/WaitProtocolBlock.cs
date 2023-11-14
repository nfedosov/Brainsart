using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace AlphaTraining
{
    internal class WaitProtocolBlock : ProtocolBlock
    {
        static int freeWaitId = 0;

        public static WaitProtocolBlock CreateProtocolBlock(string name, int blockId, double duration)
        {
            return new WaitProtocolBlock(name, blockId, freeWaitId++, duration);
        }

        public WaitProtocolBlock(string name, int blockId, int waitId, double duration)
            : base(blockId, String.Format("Wait{0}", waitId), "+", duration, blockId, name)
        {

        }
    }
}
