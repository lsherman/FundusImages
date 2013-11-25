using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Runtime.InteropServices;

namespace FundusWeb.Models
{
    // Image Segmentation Service
    public class FundusSegment
    {
        [DllImport("FundusSegment.dll", EntryPoint = "startup", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int startup(String directory);

        [DllImport("FundusSegment.dll", EntryPoint = "segmentImage", CallingConvention = CallingConvention.Cdecl)]
        public static extern int segmentImage(byte[] data, IntPtr length, ref IntPtr result, ref IntPtr bytes);

        [DllImport("FundusSegment.dll", EntryPoint = "freeImage", CallingConvention = CallingConvention.Cdecl)]
        public static extern void freeImage(IntPtr data);

        [DllImport("FundusSegment.dll", EntryPoint = "shutdown", CallingConvention = CallingConvention.Cdecl)]
        public static extern void shutdown();
    }
}