#pragma once

using namespace System;
using namespace System::Windows::Forms;
using namespace Runtime::InteropServices;

delegate IntPtr KeyboardProcedure(int nCode, IntPtr wParam, IntPtr lParam);

ref class KeyListener
{
private:

	static const int
		WhKeyboardLowLevel = 13,
		WmKeydown = 0x100,
		WmKeyup = 0x0101,
		WmSyskeydown = 0x104,
		WmSyskeyup = 0x0105;

	[DllImport("user32.dll")]
	static IntPtr SetWindowsHookEx(
		int idHook,
		KeyboardProcedure^ lpfn,
		IntPtr hMod,
		UInt32 dwThreadId);

	[DllImport("user32.dll")]
	static bool UnhookWindowsHookEx(IntPtr hhk);

	[DllImport("user32.dll")]
	static IntPtr CallNextHookEx(
		IntPtr hhk,
		int nCode,
		IntPtr wParam,
		IntPtr lParam);

	[DllImport("kernel32.dll")]
	static IntPtr GetModuleHandle(String^ lpModuleName);

	static IntPtr KeyListener::SetHook(KeyboardProcedure^ proc)
	{
		const auto currentProcess = Diagnostics::Process::GetCurrentProcess();

		if (currentProcess->MainModule == nullptr)
			throw gcnew Exception("Cannot setup key hook.");

		return SetWindowsHookEx(WhKeyboardLowLevel, proc, GetModuleHandle(currentProcess->MainModule->ModuleName), 0);
	}

	IntPtr KeyListener::HookCallback(int nCode, IntPtr wParam, IntPtr lParam)
	{
		if (nCode >= 0)
		{
			const auto wParamInt = wParam.ToInt32();

			switch (wParamInt)
			{
			case WmSyskeydown:
			case WmKeydown:
				{
					const auto key = static_cast<Keys>(Marshal::ReadInt32(lParam));

					switch (key)
					{
					case Keys::LControlKey:
					case Keys::RControlKey:
					case Keys::Control:
					case Keys::ControlKey:
						_modifier = Keys::Control;
						_modifierInvokeKey = key;
						break;
					case Keys::Shift:
					case Keys::ShiftKey:
					case Keys::LShiftKey:
					case Keys::RShiftKey:
						_modifier = Keys::Shift;
						_modifierInvokeKey = key;
						break;
					case Keys::LMenu:
					case Keys::RMenu:
					case Keys::Alt:
						_modifier = Keys::Alt;
						_modifierInvokeKey = key;
						break;
					}

					_prevKey = key;
					// OnLowLevelKeyDown(key);
					break;
				}
			case WmSyskeyup:
			case WmKeyup:
				{
					auto key = static_cast<Keys>(Marshal::ReadInt32(lParam));
					auto prevKey = _prevKey, modifier = _modifier, keyOriginal = key;

					if (modifier != Keys::None)
					{
						if (key == _modifierInvokeKey)
						{
							_modifier = Keys::None;
							_modifierInvokeKey = Keys::None;
						}

						key = key | modifier;
					}

					// OnLowLevelKeyUp(key);

					if (prevKey != Keys::None)
					{
						if (prevKey == keyOriginal)
							OnLowLevelKeyPress(key);

						_prevKey = Keys::None;
					}

					break;
				}
			}
		}

		return CallNextHookEx(_keyboardHookId, nCode, wParam, lParam);
	}

	IntPtr _keyboardHookId = IntPtr::Zero;
	KeyboardProcedure^ _keyboardProcedure; //musi zustat referencovane

	Keys _modifier = Keys::None, _modifierInvokeKey = Keys::None, _prevKey = Keys::None;

	property bool IsDisposed;

	void OnLowLevelKeyPress(Keys e)
	{
		if (LowLevelKeyPress != nullptr)
			LowLevelKeyPress(this, gcnew KeyEventArgs(e));
	}

public:

	property bool Active;

	KeyListener()
	{
		_keyboardProcedure = gcnew KeyboardProcedure(this, &KeyListener::HookCallback);
	}

	KeyEventHandler^ LowLevelKeyPress;

	void Start()
	{
		if (Active)
			return;

		if (IsDisposed)
			throw gcnew ObjectDisposedException("");

		_keyboardHookId = SetHook(_keyboardProcedure);
		Active = true;
	}

	void Destroy()
	{
		if (IsDisposed)
			return;

		IsDisposed = true;
		Active = false;

		if (_keyboardHookId != IntPtr::Zero)
			UnhookWindowsHookEx(_keyboardHookId);
	}
};