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
        private readonly ImageAttributes _postProcess;

        private const ulong PlayerId = 0;

        // ReSharper disable InconsistentNaming

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

        [DllImport("TiberianDawn.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern void CNC_Handle_Input(InputRequestEnum input_event, byte special_key_flags, ulong player_id, int x1, int y1, int x2, int y2);

        private enum InputRequestEnum
        {
            INPUT_REQUEST_NONE = 0,
            INPUT_REQUEST_MOUSE_MOVE = 1,
            INPUT_REQUEST_MOUSE_LEFT_CLICK = 2,
            INPUT_REQUEST_MOUSE_RIGHT_DOWN = 3,
            INPUT_REQUEST_MOUSE_RIGHT_CLICK = 4,
            INPUT_REQUEST_MOUSE_AREA = 5,
            INPUT_REQUEST_MOUSE_AREA_ADDITIVE = 6,
            INPUT_REQUEST_SELL_AT_POSITION = 7,
            INPUT_REQUEST_SELECT_AT_POSITION = 8,
            INPUT_REQUEST_COMMAND_AT_POSITION = 9,
            INPUT_REQUEST_SPECIAL_KEYS = 10
        };

        // ReSharper restore InconsistentNaming

        private Rectangle SourceRect { get; set; }

        private Rectangle DestRect { get; set; }

        private Bitmap Bmp { get; set; }

        public Form1()
        {
            InitializeComponent();

            // ReSharper disable once VirtualMemberCallInConstructor
            MinimumSize = new Size(100, 100);
            // ReSharper disable once VirtualMemberCallInConstructor
            DoubleBuffered = true;

            const float brightness = 0.05f, contrast = 3.0f, gamma = 1.1f;

            float[][] colorMatrix =
            {
                new[] {contrast, 0, 0, 0, 0}, // scale red
                new[] {0, contrast, 0, 0, 0}, // scale green
                new[] {0, 0, contrast, 0, 0}, // scale blue
                new[] {0, 0, 0, 1.0f, 0}, // don't scale alpha
                new[] {brightness, brightness, brightness, 0, 1}
            };

            _postProcess = new ImageAttributes();
            _postProcess.ClearColorMatrix();
            _postProcess.SetColorMatrix(new ColorMatrix(colorMatrix), ColorMatrixFlag.Default, ColorAdjustType.Bitmap);
            _postProcess.SetGamma(gamma, ColorAdjustType.Bitmap);
        }

        protected override async void OnLoad(EventArgs e)
        {
            if (CNC_Version(123) != 256)
                throw new Exception("Unknown engine version.");

            string savePath = Path.GetDirectoryName(Application.ExecutablePath) + @"\\gdi_save.sav";

            CNC_Init(@"-CD""C:\Steam\steamapps\common\CnCRemastered\DATA\CNCDATA\TIBERIAN_DAWN\CD1""", IntPtr.Zero);
            CNC_Init(@"-CD""C:\Steam\steamapps\common\CnCRemastered\DATA\CNCDATA\TIBERIAN_DAWN\CD1""", IntPtr.Zero);

            if (!CNC_Save_Load(false, savePath, "GAME_NORMAL"))
                throw new Exception("Load game failed.");

            CNC_Set_Home_Cell(2, 54, PlayerId);

            var screenBuffer = new byte[50 * 1024 * 1024]; //Bill Gates says 640K ought to be enough, I say 50 megs :P
            var palette = new byte[256, 3];
            
            while (true)
            {
                if (!CNC_Advance_Instance(PlayerId))
                    throw new Exception("Advance instance failed.");

                if (!CNC_Get_Palette(palette))
                    throw new Exception("Get palette failed.");

                if (!CNC_Get_Visible_Page(screenBuffer, out int width, out int height))
                    throw new Exception("Get visible page failed.");

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

                await Task.Delay(33); //~30 fps refresh
            }
        }

        private Point? MouseDownPoint { get; set; }

        private Point CurrentMousePos { get; set; }

        protected override void OnMouseDown(MouseEventArgs e)
        {
            if (Bmp == null)
                return;

            MouseDownPoint = e.Location;
        }

        protected override void OnMouseUp(MouseEventArgs e)
        {
            if (Bmp == null)
                return;

            Point location = e.Location;
            Point? md = MouseDownPoint;

            if (md.HasValue)
            {
                const int clickTolerance = 5;

                if (Math.Abs(location.X - md.Value.X) < clickTolerance && Math.Abs(location.Y - md.Value.Y) < clickTolerance)
                {
                    OnMouseEvent(e, false);
                }
                else
                {
                    Point? origin = MapClientPointToRenderArea(md.Value), end = MapClientPointToRenderArea(e.Location);

                    if (origin.HasValue && end.HasValue)
                    {
                        var mouseArea = new Rectangle(
                            Math.Min(origin.Value.X, end.Value.X),
                            Math.Min(origin.Value.Y, end.Value.Y),
                            Math.Abs(origin.Value.X - end.Value.X),
                            Math.Abs(origin.Value.Y - end.Value.Y));

                        CNC_Handle_Input(InputRequestEnum.INPUT_REQUEST_MOUSE_AREA, 0, PlayerId, mouseArea.Left, mouseArea.Top, mouseArea.Right, mouseArea.Bottom);
                    }
                }
            }

            MouseDownPoint = null;
        }

        protected override void OnMouseMove(MouseEventArgs e)
        {
            if (Bmp == null)
                return;

            OnMouseEvent(e, true);
            CurrentMousePos = e.Location;

            if (e.Button == MouseButtons.Left)
                Refresh();
        }

        private void OnMouseEvent(MouseEventArgs e, bool mouseMove)
        {
            Point? loc = MapClientPointToRenderArea(e.Location);

            if (loc.HasValue)
            {
                if (mouseMove)
                {
                    CNC_Handle_Input(InputRequestEnum.INPUT_REQUEST_MOUSE_MOVE, 0, PlayerId, loc.Value.X, loc.Value.Y, 0, 0);
                }
                else
                {
                    switch (e.Button)
                    {
                        case MouseButtons.Left:
                            CNC_Handle_Input(InputRequestEnum.INPUT_REQUEST_MOUSE_LEFT_CLICK, 0, PlayerId, loc.Value.X, loc.Value.Y, 0, 0);
                            break;
                        case MouseButtons.Right:
                            CNC_Handle_Input(InputRequestEnum.INPUT_REQUEST_MOUSE_RIGHT_DOWN, 0, PlayerId, loc.Value.X, loc.Value.Y, 0, 0);
                            CNC_Handle_Input(InputRequestEnum.INPUT_REQUEST_MOUSE_RIGHT_CLICK, 0, PlayerId, loc.Value.X, loc.Value.Y, 0, 0);
                            break;
                        default:
                            return;
                    }
                }
            }
        }

        private Point? MapClientPointToRenderArea(Point point)
        {
            Rectangle destRect = DestRect, srcRect = SourceRect;

            float relPosX = (point.X - destRect.Left) / (float) destRect.Width, relPosY = (point.Y - destRect.Top) / (float) destRect.Height;

            int mappedPosX = (int) (srcRect.Left + (srcRect.Width * relPosX)), mappedPosY = (int) (srcRect.Top + (srcRect.Height * relPosY));

            if (srcRect.Contains(mappedPosX, mappedPosY))
                return new Point(mappedPosX, mappedPosY);

            return null;
        }

        protected override void OnPaintBackground(PaintEventArgs e)
        {
            Graphics g = e.Graphics;

            g.InterpolationMode = InterpolationMode.NearestNeighbor;
            g.CompositingMode = CompositingMode.SourceCopy;
            g.CompositingQuality = CompositingQuality.HighSpeed;
            g.PixelOffsetMode = PixelOffsetMode.HighSpeed;
            g.SmoothingMode = SmoothingMode.HighSpeed;

            g.Clear(Color.Black);

            if (Bmp != null)
            {
                var srcRect = new Rectangle(0, 0, Bmp.Width, Bmp.Height);

                float ar = Math.Min(g.VisibleClipBounds.Width / srcRect.Width, g.VisibleClipBounds.Height / srcRect.Height);
                int destWidth = (int) (srcRect.Width * ar), destHeight = (int) (srcRect.Height * ar);

                var destRect = new Rectangle(
                    (int) ((g.VisibleClipBounds.Width / 2f) - (destWidth / 2f)),
                    (int) ((g.VisibleClipBounds.Height / 2f) - (destHeight / 2f)),
                    destWidth,
                    destHeight);

                SourceRect = srcRect;
                DestRect = destRect;

                g.DrawImage(
                    Bmp,
                    destRect,
                    srcRect.X,
                    srcRect.Y,
                    srcRect.Width,
                    srcRect.Height,
                    GraphicsUnit.Pixel,
                    _postProcess);

                Point? md = MouseDownPoint;

                if (md.HasValue)
                {
                    Point currentPoint = CurrentMousePos;

                    var selectArea = new Rectangle(
                        Math.Min(md.Value.X, currentPoint.X),
                        Math.Min(md.Value.Y, currentPoint.Y),
                        Math.Abs(md.Value.X - currentPoint.X),
                        Math.Abs(md.Value.Y - currentPoint.Y));

                    g.DrawRectangle(Pens.White, selectArea);
                }
            }
        }
    }
}