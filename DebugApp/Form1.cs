using System;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Drawing.Imaging;
using System.IO;
using System.Runtime.InteropServices;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace DebugApp
{
    public partial class Form1 : Form
    {
        //[return: MarshalAs(UnmanagedType.)]

        [DllImport("TiberianDawn.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern uint CNC_Version(uint versionIn);
        
        [DllImport("TiberianDawn.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern void CNC_Init(string command_line, IntPtr event_callback);
        
        [DllImport("TiberianDawn.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern bool CNC_Save_Load(bool save, string file_path_and_name, string game_type);
        
        [DllImport("TiberianDawn.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern void CNC_Set_Home_Cell(int x, int y, ulong player_id);
        
        [DllImport("TiberianDawn.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern bool CNC_Advance_Instance(ulong player_id);

        [DllImport("TiberianDawn.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern bool CNC_Get_Palette(byte[,] palette_in);

        [DllImport("TiberianDawn.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern bool CNC_Get_Visible_Page(byte[] buffer_in, out int width, out int height);
        
        private Bitmap Bmp { get; set; }

        public Form1()
        {
            InitializeComponent();
        }

        protected override async void OnLoad(EventArgs e)
        {
            const ulong playerId = 59057270;

            string savePath = Path.GetDirectoryName(Application.ExecutablePath) + @"\\cnc_temporary_save_file.sav";

            CNC_Init(@"-CD""C:\Steam\steamapps\common\CnCRemastered\DATA\CNCDATA\TIBERIAN_DAWN\CD1""", IntPtr.Zero);

            if (!CNC_Save_Load(false, savePath, "GAME_NORMAL"))
                throw new Exception("Load game failed.");

            CNC_Set_Home_Cell(2, 54, playerId);

            var screenBuffer = new byte[50 * 1024 * 1024];
            var palette = new byte[256, 3];
            
            while (true)
            {
                if (!CNC_Advance_Instance(playerId))
                    throw new Exception("Advance instance failed.");

                //advance second frame to get smoother game sim
                if (!CNC_Advance_Instance(playerId))
                    throw new Exception("Advance instance failed.");
                
                if (!CNC_Get_Palette(palette))
                    throw new Exception("Get palette failed.");
            
                if (!CNC_Get_Visible_Page(screenBuffer, out int width, out int height))
                    throw new Exception("Advance instance failed.");
                
                Bmp ??= new Bitmap(width, height, PixelFormat.Format8bppIndexed);
                
                ColorPalette ncp = Bmp.Palette;

                for (int i = 0; i < 256; i++)
                    ncp.Entries[i] = Color.FromArgb(255, palette[i, 0], palette[i, 1], palette[i, 2]);

                Bmp.Palette = ncp;

                BitmapData bmpData = Bmp.LockBits(new Rectangle(0, 0, Bmp.Width, Bmp.Height), ImageLockMode.WriteOnly, Bmp.PixelFormat);

                IntPtr ptr = bmpData.Scan0;
                int bytes = bmpData.Stride * Bmp.Height;
                
                Marshal.Copy(screenBuffer, 0, ptr, bytes);
                Bmp.UnlockBits(bmpData);

                Refresh();
                
                await Task.Delay(16); //~60 fps refresh
            }
        }
        
        protected override void OnPaintBackground(PaintEventArgs e)
        {
            Graphics g = e.Graphics;
            g.InterpolationMode = InterpolationMode.NearestNeighbor;
            g.CompositingMode = CompositingMode.SourceCopy;
            g.CompositingQuality = CompositingQuality.HighSpeed;
            g.PixelOffsetMode = PixelOffsetMode.HighSpeed;
            g.SmoothingMode = SmoothingMode.HighSpeed;
            
            if (Bmp != null)
                g.DrawImage(Bmp, g.VisibleClipBounds);
        }
    }
}