#include "DotNetApiVc.h"
// #include <msclr/marshal.h>
#include "FUNCTION.H"

// #include "KEYBOARD.H"
// #include <vcclr.h>

using namespace System;
using namespace CncDotNet;
using namespace Runtime::InteropServices;

extern void On_Message(const char* message, float timeout_seconds, __int64 message_id);

ref class CsApi : INativeApiRoot
{
public:

	property String^ CurrentScenarioName { virtual String^ get() { return gcnew String(ScenarioName); } }

	virtual void DeleteSave(String^ filename)
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

	virtual void SaveGame(String^ filename, String^ description)
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

	virtual void LoadGame(String^ filename)
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

	virtual void ShowQuickMessage(String^ text, int timeoutMs)
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

	static CncMain^ Cs = nullptr;
};

void DotNetApiVc::MainInit(const int argc, char* argv[])
{
	Shutdown();

	auto strs = gcnew array<String^>(argc);

	for (int i = 0; i < argc; ++i)
		strs[i] = gcnew String(argv[i]);

	const auto root = gcnew CsApi();

	CsApi::Cs = gcnew CncMain(strs, root);
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
