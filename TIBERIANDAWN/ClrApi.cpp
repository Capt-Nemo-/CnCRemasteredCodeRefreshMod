#include "ClrApi.h"

void ClrApi::OnLowLevelKeyPress(Object^ sender, KeyEventArgs^ args)
{
	KeyQueue->Enqueue(args->KeyData);
}

void ClrApi::HookKeyboard()
{
	if (IsShutdown)
		return;

	UnhookKeyboard();

	Keys = gcnew KeyListener();
	Keys->LowLevelKeyPress = gcnew KeyEventHandler(this, &ClrApi::OnLowLevelKeyPress);

	Keys->Start();

	KeyboardHookActive = true;
}

void ClrApi::UnhookKeyboard()
{
	if (Keys != nullptr)
		Keys->Destroy();

	Keys = nullptr;

	ClearKeyQueue();

	KeyboardHookActive = false;
}

void ClrApi::KeyboardHookWatcher()
{
	while (!IsShutdown)
	{
		MainLoopActiveFlag = false;
		Thread::Sleep(150);

		if (!MainLoopActiveFlag && KeyboardHookActive)
		{
			//"long" time elapsed till last main loop and hook is active, we are probably in the main menu, lets disable keyboard hook
			//this is needed because active hook introduces significant and annoying lag on key press when in main menu, don't know why ¯\_(ツ)_/¯

			UnhookKeyboard();
		}
	}
}

void ClrApi::RestartScripts()
{
	for each (MissionScriptBase^ script in MissionScripts)
	{
		script->CnC = this;

		script->OnStopped();
		script->OnStarted();
	}
}

ClrApi::ClrApi(array<String^>^ cmdLine, ... array<MissionScriptBase^>^ scripts)
{
	MissionScripts = gcnew List<MissionScriptBase^>(scripts);
	KeyQueue = gcnew ConcurrentQueue<::Keys>();

	ModuleCommandLine = cmdLine;

	RestartScripts();

	auto wThread = gcnew Thread(gcnew ThreadStart(this, &ClrApi::KeyboardHookWatcher));

	wThread->IsBackground = true;
	wThread->Name = "Managed keyboard hook watcher";

	wThread->Start();
}

void ClrApi::MainLoop()
{
	if (IsShutdown)
		return;

	MainLoopActiveFlag = true;

	if (!KeyboardHookActive)
		HookKeyboard();
}

void ClrApi::ProcessKeystrokes()
{
	if (IsShutdown)
		return;

	::Keys key;
	while (KeyQueue->TryDequeue(key))
	{
		for each (MissionScriptBase^ script in MissionScripts)
			script->OnKeyInput(key);
	}
}

void ClrApi::Shutdown()
{
	if (IsShutdown)
		return;

	IsShutdown = true;

	UnhookKeyboard();

	for each (MissionScriptBase^ script in MissionScripts)
		script->OnStopped();
}

void ClrApi::ClearKeyQueue()
{
	::Keys key;
	while (KeyQueue->TryDequeue(key))
	{
	}
}

void ClrApi::DeleteSave(String^ filename)
{
	auto fnamePtr = Marshal::StringToHGlobalAnsi(filename);

	try
	{
		const char* fnameChar = static_cast<char*>(fnamePtr.ToPointer());

		RawFileClass file(fnameChar);
		file.Delete();
	}
	finally
	{
		Marshal::FreeHGlobal(fnamePtr);
	}
}

void ClrApi::SaveGame(String^ filename, String^ description)
{
	auto fnamePtr = Marshal::StringToHGlobalAnsi(filename), descPtr = Marshal::StringToHGlobalAnsi(description);

	try
	{
		if (!Save_Game(static_cast<char*>(fnamePtr.ToPointer()), static_cast<char*>(descPtr.ToPointer())))
			throw gcnew Exception(gcnew String("Save game procedure failed."));
	}
	finally
	{
		Marshal::FreeHGlobal(fnamePtr);
		Marshal::FreeHGlobal(descPtr);
	}
}

void ClrApi::LoadGame(String^ filename)
{
	auto fnamePtr = Marshal::StringToHGlobalAnsi(filename);

	try
	{
		const char* fnameChar = static_cast<char*>(fnamePtr.ToPointer());

		RawFileClass file(fnameChar);

		if (file.Is_Available(false))
		{
			if (!Load_Game(fnameChar))
				throw gcnew Exception(gcnew String("Load game procedure failed."));
		}
		else
		{
			throw gcnew IO::FileNotFoundException("Quick load file not found.");
		}
	}
	finally
	{
		Marshal::FreeHGlobal(fnamePtr);
	}
}

void ClrApi::ShowQuickMessage(String^ text, int timeoutMs)
{
	auto intPtr = Marshal::StringToHGlobalAnsi(text);

	try
	{
		const auto ptr = static_cast<char*>(intPtr.ToPointer());

		On_Message(ptr, timeoutMs / 1000.0, -1);
	}
	finally
	{
		Marshal::FreeHGlobal(intPtr);
	}
}
