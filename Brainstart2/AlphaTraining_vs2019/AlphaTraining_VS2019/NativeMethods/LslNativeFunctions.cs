using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;


namespace AlphaTraining.NativeMethods
{
    public class LslNativeFunctions
    {
        [DllImport("LslHelperLib.dll"/*, CallingConvention = CallingConvention.Cdecl*/)]
        public static extern bool GetFirstLslStream(
            uint BufferLength,
            [MarshalAs(UnmanagedType.LPStr)]
            StringBuilder LslStreamName);

        [DllImport("LslHelperLib.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool GetNextLslStream(
            uint BufferLength,
            [MarshalAs(UnmanagedType.LPStr)]
            StringBuilder LslStreamName);
    }
}
