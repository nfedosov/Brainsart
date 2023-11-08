using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace AlphaTraining
{
    internal class ProtocolBlockDescriptor
    {
        private string name;


        private string id;

        private string message;

        private double duration;

        public ProtocolBlockDescriptor(ProtocolBlock protocolBlock)
        {
            name = protocolBlock.name;
            id = protocolBlock.id;
            message = protocolBlock.message;
            duration = protocolBlock.duration;
        }

        public string Description
        {
            get
            {
                return String.Format("[{0} c] {1}", duration, message);
            }
        }
    }
}
