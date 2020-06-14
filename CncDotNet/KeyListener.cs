using System;
using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Windows.Forms;
using JetBrains.Annotations;

namespace CncDotNet
{
    public sealed class KeyListener : IDisposable
    {
        private const int
            WhKeyboardLowLevel = 13,
            WmKeydown = 0x100,
            WmKeyup = 0x0101,
            WmSyskeydown = 0x104,
            WmSyskeyup = 0x0105;

        private IntPtr _keyboardHookId = IntPtr.Zero;

        // ReSharper disable once PrivateFieldCanBeConvertedToLocalVariable
        private readonly KeyboardProcedure _keyboardProcedure; //musi zustat referencovane

        private Keys _modifier = Keys.None, _modifierInvokeKey = Keys.None, _prevKey = Keys.None;

        private bool IsDisposed { get; set; }

        [UsedImplicitly]
        public bool Active { get; private set; }
        
        public KeyListener()
        {
            _keyboardProcedure = HookCallback;
        }

        public void Start()
        {
            if (Active)
                return;

            if (IsDisposed)
                throw new ObjectDisposedException("");

            _keyboardHookId = SetHook(_keyboardProcedure);
            Active = true;
        }

        #region Events

        /// <summary>
        /// Doba behu handleru nesmi presahnout hodnotu zadanou v systemu (HKEY_CURRENT_USER\ControlPanel\Desktop\LowLevelHooksTimeout).
        /// </summary>
        public event KeyEventHandler LowLevelKeyDown;

        private void OnLowLevelKeyDown(Keys e) => LowLevelKeyDown?.Invoke(this, new KeyEventArgs(e));

        /// <summary>
        /// Doba behu handleru nesmi presahnout hodnotu zadanou v systemu (HKEY_CURRENT_USER\ControlPanel\Desktop\LowLevelHooksTimeout).
        /// </summary>
        public event KeyEventHandler LowLevelKeyUp;

        private void OnLowLevelKeyUp(Keys e) => LowLevelKeyUp?.Invoke(this, new KeyEventArgs(e));

        /// <summary>
        /// Doba behu handleru nesmi presahnout hodnotu zadanou v systemu (HKEY_CURRENT_USER\ControlPanel\Desktop\LowLevelHooksTimeout).
        /// </summary>
        public event KeyEventHandler LowLevelKeyPress;

        private void OnLowLevelKeyPress(Keys e) => LowLevelKeyPress?.Invoke(this, new KeyEventArgs(e));

        #endregion

        #region DLL Imports

        [DllImport("user32.dll", CharSet = CharSet.Auto, SetLastError = true)]
        private static extern IntPtr SetWindowsHookEx(
            int idHook,
            KeyboardProcedure lpfn,
            IntPtr hMod,
            uint dwThreadId);

        [DllImport("user32.dll", CharSet = CharSet.Auto, SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        private static extern bool UnhookWindowsHookEx(IntPtr hhk);

        [DllImport("user32.dll", CharSet = CharSet.Auto, SetLastError = true)]
        private static extern IntPtr CallNextHookEx(
            IntPtr hhk,
            int nCode,
            IntPtr wParam,
            IntPtr lParam);

        [DllImport("kernel32.dll", CharSet = CharSet.Auto, SetLastError = true)]
        private static extern IntPtr GetModuleHandle(string lpModuleName);

        #endregion
        
        private static IntPtr SetHook(KeyboardProcedure proc)
        {
            using var currentProcess = Process.GetCurrentProcess();
            using ProcessModule currentModule = currentProcess.MainModule;

            if (currentModule == null)
                throw new Exception("Cannot setup key hook.");

            return SetWindowsHookEx(WhKeyboardLowLevel, proc, GetModuleHandle(currentModule.ModuleName), 0);
        }

        private delegate IntPtr KeyboardProcedure(int nCode, IntPtr wParam, IntPtr lParam);

        public static Keys? ReadKeyDown(ref Message msg, Keys keyData)
        {
            switch (msg.Msg)
            {
                case WmKeydown:
                case WmSyskeydown:
                    return keyData;
            }

            return null;
        }

        private IntPtr HookCallback(int nCode, IntPtr wParam, IntPtr lParam)
        {
            if (nCode >= 0)
            {
                int wParamInt = wParam.ToInt32();

                switch (wParamInt)
                {
                    case WmSyskeydown:
                    case WmKeydown:
                    {
                        var key = (Keys) Marshal.ReadInt32(lParam);

                        switch (key)
                        {
                            case Keys.LControlKey:
                            case Keys.RControlKey:
                            case Keys.Control:
                            case Keys.ControlKey:
                                _modifier = Keys.Control;
                                _modifierInvokeKey = key;
                                break;
                            case Keys.Shift:
                            case Keys.ShiftKey:
                            case Keys.LShiftKey:
                            case Keys.RShiftKey:
                                _modifier = Keys.Shift;
                                _modifierInvokeKey = key;
                                break;
                            case Keys.LMenu:
                            case Keys.RMenu:
                            case Keys.Alt:
                                _modifier = Keys.Alt;
                                _modifierInvokeKey = key;
                                break;
                        }

                        _prevKey = key;
                        OnLowLevelKeyDown(key);
                        break;
                    }
                    case WmSyskeyup:
                    case WmKeyup:
                    {
                        var key = (Keys) Marshal.ReadInt32(lParam);
                        Keys prevKey = _prevKey, modifier = _modifier, keyOriginal = key;

                        if (modifier != Keys.None)
                        {
                            if (key == _modifierInvokeKey)
                            {
                                _modifier = Keys.None;
                                _modifierInvokeKey = Keys.None;
                            }

                            key |= modifier;
                        }

                        OnLowLevelKeyUp(key);

                        if (prevKey != Keys.None)
                        {
                            if (prevKey == keyOriginal)
                                OnLowLevelKeyPress(key);

                            _prevKey = Keys.None;
                        }

                        break;
                    }
                }
            }

            return CallNextHookEx(_keyboardHookId, nCode, wParam, lParam);
        }

        public void Dispose()
        {
            if (IsDisposed)
                return;

            IsDisposed = true;
            Active = false;

            if (_keyboardHookId != IntPtr.Zero)
                UnhookWindowsHookEx(_keyboardHookId);
        }
    }
}