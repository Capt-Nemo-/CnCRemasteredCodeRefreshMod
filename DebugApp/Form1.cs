using System;
using System.Runtime.InteropServices;
using System.Windows.Forms;

namespace DebugApp
{
    public partial class Form1 : Form
    {
        //[return: MarshalAs(UnmanagedType.)]

        [DllImport("TiberianDawn.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern uint CNC_Version(uint versionIn);
        
        [DllImport("TiberianDawn.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern void CNC_Init_2();
        
        public Form1()
        {
            InitializeComponent();
        }

        protected override void OnLoad(EventArgs e)
        {
            // uint ver = CNC_Version(45);

            CNC_Init_2();
        }
    }
}