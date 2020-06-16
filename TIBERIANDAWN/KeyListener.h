#pragma once

using namespace System;
using namespace System::Windows::Forms;

delegate IntPtr KeyboardProcedure(int nCode, IntPtr wParam, IntPtr lParam);

ref class KeyListener
{
private:

	static IntPtr SetHook(KeyboardProcedure^ proc);

	IntPtr HookCallback(int nCode, IntPtr wParam, IntPtr lParam);

	IntPtr _keyboardHookId = IntPtr::Zero;
	KeyboardProcedure^ _keyboardProcedure; //musi zustat referencovane

	Keys _modifier = Keys::None, _modifierInvokeKey = Keys::None, _prevKey = Keys::None;

	bool _isActive;
	property bool IsDisposed;
	void OnLowLevelKeyPress(Keys e);
public:

	property bool Active
	{
		bool get();
	private:
		void set(bool);
	}

	KeyListener();

	event KeyEventHandler^ LowLevelKeyPress;

	void Start();
	void Destroy();
};
