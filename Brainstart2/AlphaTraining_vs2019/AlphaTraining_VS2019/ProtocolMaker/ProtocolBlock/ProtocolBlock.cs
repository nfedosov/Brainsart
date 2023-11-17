using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace AlphaTraining
{
    public enum ProtocolBlockType
    { 
        None,

        Wait,

        Condition,
    }


    public class ProtocolBlock
    {
        public string name
        {
            get; set;
        }


        public string id
        {
            get; set;
        }

        public string message
        {
            get; set;
        }

        public double duration
        {
            get; set;
        }

        public int code
        {
            get; set;
        }

        public string label
        {
            get; set;
        }

        public ProtocolBlockType BlockType
        {
            get; set;
        }

    public ProtocolBlock()
        {

        }

        public ProtocolBlock(int id_, string name_, string message_, double duration_, int code_, string label_)
        {
            id = Convert.ToString(id_);
            name = name_;
            message = message_;
            duration = duration_;
            code = code_;
            label = label_;
        }

        
    }
}
