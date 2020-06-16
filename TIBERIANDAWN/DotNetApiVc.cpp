#include "DotNetApiVc.h"
#include "FUNCTION.H"
#include "KeyListener.h"

// #include <msclr/marshal.h>
// #include <vcclr.h>

using namespace System;
using namespace Runtime::InteropServices;
using namespace System::Windows::Forms;
using namespace Threading;
using namespace Collections::Generic;
using namespace Collections::Concurrent;

extern void On_Message(const char* message, float timeout_seconds, __int64 message_id);

ref class MissionScriptBase abstract
{
public:
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

ref class CsApi
{
private:

	property List<MissionScriptBase^>^ MissionScripts;

	property bool KeyboardHookActive;

	property bool MainLoopActiveFlag;

	property bool IsShutdown;

	property KeyListener^ Keys;

	property ConcurrentQueue<::Keys>^ KeyQueue;

	void KeyboardHookWatcher()
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

	void RestartScripts()
	{
		for each (MissionScriptBase^ script in MissionScripts)
		{
			script->OnStopped();
			script->OnStarted();
		}
	}

internal:
	property array<String^>^ ModuleCommandLine
	{
		array<String^>^ get();
	private:
		void set(array<String^>^);
	}

public:
	CsApi(array<String^>^ cmdLine, ... array<MissionScriptBase^>^ scripts)
	{
		MissionScripts = gcnew List<MissionScriptBase^>(scripts);
		KeyQueue = gcnew ConcurrentQueue<::Keys>();

		ModuleCommandLine = cmdLine;

		RestartScripts();

		auto wThread = gcnew Thread(gcnew ThreadStart(this, &CsApi::KeyboardHookWatcher));

		wThread->IsBackground = true;
		wThread->Name = "Managed keyboard hook watcher";

		wThread->Start();
	}

	void MainLoop()
	{
		if (IsShutdown)
			return;

		MainLoopActiveFlag = true;

		if (!KeyboardHookActive)
			HookKeyboard();
	}

	void OnLowLevelKeyPress(Object^ sender, KeyEventArgs^ args)
	{
		KeyQueue->Enqueue(args->KeyData);		
	}

	void HookKeyboard()
	{
		if (IsShutdown)
			return;

		UnhookKeyboard();

		Keys = gcnew KeyListener();
		Keys->LowLevelKeyPress += gcnew KeyEventHandler(this, &OnLowLevelKeyPress);

		Keys->Start();

		KeyboardHookActive = true;
	}

	void UnhookKeyboard()
	{
		if (Keys != nullptr)
			Keys->Destroy();

		Keys = nullptr;

		ClearKeyQueue();

		KeyboardHookActive = false;
	}

	void ProcessKeystrokes()
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

	void Shutdown()
	{
		if (IsShutdown)
			return;

		IsShutdown = true;

		UnhookKeyboard();

		for each (MissionScriptBase^ script in MissionScripts)
				script->OnStopped();
	}

	void ClearKeyQueue()
	{
		::Keys key;
		while (KeyQueue->TryDequeue(key))
		{
		}
	}

	static property String^ CurrentScenarioName { String^ get() { return gcnew String(ScenarioName); } }

	static void DeleteSave(String^ filename)
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

	static void SaveGame(String^ filename, String^ description)
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

	static void LoadGame(String^ filename)
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

	static void ShowQuickMessage(String^ text, int timeoutMs)
	{
		auto intPtr = Marshal::StringToHGlobalAnsi(text);

		try
		{
			const auto ptr = static_cast<char*>(intPtr.ToPointer());

			On_Message(ptr, timeoutMs / 1000.0, -1);

			// Messages.Add_Message(ptr, static_cast<int>(color), TPF_6PT_GRAD | TPF_USE_GRAD_PAL | TPF_FULLSHADOW, 1, 0, 0);				
		}
		finally
		{
			Marshal::FreeHGlobal(intPtr);
		}
	}

	static CsApi^ Cs;
};

ref class QuickSaveScript : MissionScriptBase
{
private:

	static String^ GetFilename()
	{
		return "QUICKSAVE_TD_" + CsApi::Cs->CurrentScenarioName;
	}

public:
	void OnKeyInput(Keys key) override
	{
		switch (key)
		{
		case Keys::F5:
			CsApi::Cs->SaveGame(GetFilename(), "Quick save slot");
			CsApi::Cs->ShowQuickMessage("Quick save successful...", 1000);
			break;
		case Keys::F9:
			try
			{
				CsApi::Cs->LoadGame(GetFilename());
				CsApi::Cs->ShowQuickMessage("Quick load successful...", 1000);
			}
			catch (IO::FileNotFoundException^)
			{
				CsApi::Cs->ShowQuickMessage("Error: save file not found...", 2000);
			}
			catch (Exception^ ex)
			{
				CsApi::Cs->ShowQuickMessage("Error: quick load failed...", 2000);
			}

			break;
		}
	}
};

void DotNetApiVc::MainInit(const int argc, char* argv[])
{
	Shutdown();

	auto strs = gcnew array<String^>(argc);

	for (int i = 0; i < argc; ++i)
		strs[i] = gcnew String(argv[i]);

	// const auto root = gcnew CsApi();

	CsApi::Cs = gcnew CsApi(strs, gcnew QuickSaveScript());
}

void DotNetApiVc::MainLoop()
{
	CsApi::Cs->MainLoop();
}

void DotNetApiVc::ProcessKeystrokes()
{
	CsApi::Cs->ProcessKeystrokes();
}

void DotNetApiVc::Shutdown()
{
	if (CsApi::Cs != nullptr)
		CsApi::Cs->Shutdown();
}

// void DotNetApiVc::DebugLine(const char* str)
// {
// 	IO::File::AppendAllText("DotNetDbg.txt", DateTime::Now.ToString() + " " + gcnew String(str) + Environment::NewLine);
// }
//
// void DotNetApiVc::DebugLine(int str)
// {
// 	IO::File::AppendAllText("DotNetDbg.txt", DateTime::Now.ToString() + " " + str.ToString() + Environment::NewLine);
// }
