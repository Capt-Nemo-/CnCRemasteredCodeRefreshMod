#pragma once

#include "FUNCTION.H"
#include "KeyListener.h"

using namespace System;
using namespace Runtime::InteropServices;
using namespace System::Windows::Forms;
using namespace Threading;
using namespace Collections::Generic;
using namespace Collections::Concurrent;

extern void On_Message(const char* message, float timeout_seconds, __int64 message_id);

ref class ClrApi;

public ref class MissionScriptBase abstract
{
public:
	property ClrApi^ CnC;

	virtual void OnKeyInput(Keys key)
	{
	}

	virtual void OnStarted()
	{
	}

	virtual void OnStopped()
	{
	}
};

public ref class ClrApi
{
private:

	array<String^>^ _cmdLine;

	property List<MissionScriptBase^>^ MissionScripts;

	property bool KeyboardHookActive;

	property bool MainLoopActiveFlag;

	property bool IsShutdown;

	property KeyListener^ Keys;

	property ConcurrentQueue<::Keys>^ KeyQueue;

	void OnLowLevelKeyPress(Object^ sender, KeyEventArgs^ args);

	void HookKeyboard();

	void UnhookKeyboard();

	void KeyboardHookWatcher();

	void RestartScripts();

internal:

	static ClrApi^ Cs;

	explicit ClrApi(array<String^>^ cmdLine, ... array<MissionScriptBase^>^ scripts);

	void MainLoop();

	void ProcessKeystrokes();

	void Shutdown();

	void ClearKeyQueue();

public:

	property array<String^>^ ModuleCommandLine
	{
		array<String^>^ get() { return _cmdLine; }
	private:
		void set(array<String^>^ value) { _cmdLine = value; }
	}

	property String^ CurrentScenarioName { String^ get() { return gcnew String(ScenarioName); } }

	void DeleteSave(String^ filename);

	void SaveGame(String^ filename, String^ description);

	void LoadGame(String^ filename);

	void ShowQuickMessage(String^ text, int timeoutMs);
};
