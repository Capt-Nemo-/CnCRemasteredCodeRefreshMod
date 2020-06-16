// using System.Collections.Concurrent;
// using System.Collections.Generic;
// using System.Threading;
// using System.Windows.Forms;
// using JetBrains.Annotations;
//
// namespace CncDotNet
// {
//     public class CncMain
//     {
//         // private ConcurrentQueue<Keys> KeyQueue { get; } = new ConcurrentQueue<Keys>();
//
//         // private bool IsShutdown { get; set; }
//
//         // public INativeApiRoot Native { get; }
//
//         // private List<MissionScriptBase> MissionScripts { get; } = new List<MissionScriptBase>();
//
//         // private KeyListener Keys { get; set; }
//
//         // [PublicAPI]
//         // public string[] ModuleCommandLine { get; }
//
//         // private bool KeyboardHookActive { get; set; }
//         //
//         // private bool MainLoopActiveFlag { get; set; }
//
//         // [PublicAPI]
//         // public CncMain(string[] cmdLine, INativeApiRoot root)
//         // {
//         //     // ModuleCommandLine = cmdLine;
//         //     // Native = root;
//         //
//         //     // RestartScripts();
//         //
//         //     // new Thread(KeyboardHookWatcher) {IsBackground = true, Name = "Managed keyboard hook watcher"}.Start();
//         // }
//
//         // private void KeyboardHookWatcher()
//         // {
//         //     while (!IsShutdown)
//         //     {
//         //         MainLoopActiveFlag = false;
//         //         Thread.Sleep(150);
//         //
//         //         if (!MainLoopActiveFlag && KeyboardHookActive)
//         //         {
//         //             //"long" time elapsed till last main loop and hook is active, we are probably in the main menu, lets disable keyboard hook
//         //             //this is needed because active hook introduces significant and annoying lag on key press when in main menu, don't know why ¯\_(ツ)_/¯
//         //
//         //             UnhookKeyboard();
//         //         }
//         //     }
//         // }
//
//         // private void RestartScripts()
//         // {
//         //     foreach (MissionScriptBase script in MissionScripts)
//         //         script.OnStopped();
//         //
//         //     MissionScripts.Add(new QuickSaveScript());
//         //
//         //     foreach (MissionScriptBase script in MissionScripts)
//         //     {
//         //         script.Cnc = this;
//         //         script.OnStarted();
//         //     }
//         // }
//
//         // [PublicAPI]
//         // public void MainLoop()
//         // {
//         //     if (IsShutdown)
//         //         return;
//         //
//         //     MainLoopActiveFlag = true;
//         //
//         //     if (!KeyboardHookActive)
//         //         HookKeyboard();
//         // }
//
//         // private void HookKeyboard()
//         // {
//         //     if (IsShutdown)
//         //         return;
//         //
//         //     UnhookKeyboard();
//         //
//         //     Keys = new KeyListener();
//         //     Keys.LowLevelKeyPress += (_, e) => KeyQueue.Enqueue(e.KeyData);
//         //
//         //     Keys.Start();
//         //
//         //     KeyboardHookActive = true;
//         // }
//         //
//         // private void UnhookKeyboard()
//         // {
//         //     Keys?.Dispose();
//         //     Keys = null;
//         //
//         //     ClearKeyQueue();
//         //
//         //     KeyboardHookActive = false;
//         // }
//         //
//         // private void ClearKeyQueue()
//         // {
//         //     while (KeyQueue.TryDequeue(out _))
//         //     {
//         //     }
//         // }
//
//         // [PublicAPI]
//         // public void ProcessKeystrokes()
//         // {
//         //     if (IsShutdown)
//         //         return;
//         //
//         //     while (KeyQueue.TryDequeue(out Keys key))
//         //     {
//         //         foreach (MissionScriptBase script in MissionScripts)
//         //             script.OnKeyInput(key);
//         //     }
//         // }
//
//         [PublicAPI]
//         public void Shutdown()
//         {
//             if (IsShutdown)
//                 return;
//
//             IsShutdown = true;
//
//             UnhookKeyboard();
//
//             foreach (MissionScriptBase script in MissionScripts)
//                 script.OnStopped();
//         }
//     }
// }