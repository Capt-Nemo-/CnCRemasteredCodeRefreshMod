using System;
using System.Runtime.InteropServices;
using System.Windows.Forms;

namespace DebugApp
{
    public partial class Form1 : Form
    {
        //[return: MarshalAs(UnmanagedType.)]

        [DllImport("TiberianDawn.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int CNC_Version(uint versionIn);
        
        public Form1()
        {
            InitializeComponent();
        }

        protected override void OnLoad(EventArgs e)
        {
            int ver = CNC_Version(45);
        }
    }
}