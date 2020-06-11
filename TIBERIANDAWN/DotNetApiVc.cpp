#include "DotNetApiVc.h"
// #include <msclr/marshal.h>
#include "FUNCTION.H"
#include "MSGLIST.H"

// #include "KEYBOARD.H"
// #include <vcclr.h>

using namespace System;
using namespace CncDotNet;
using namespace Runtime::InteropServices;

ref class CsApi : INativeApiRoot
{
public:

	virtual void SaveGame(String^ filename, String^ description)
	{
		const pin_ptr<String^> fnamePtr = &filename, descPtr = &description;

		if (!Save_Game(reinterpret_cast<char*>(fnamePtr), reinterpret_cast<char*>(descPtr)))
			throw gcnew Exception(gcnew String("Save game procedure failed."));
	}

	virtual void LoadGame(String^ filename)
	{
		const pin_ptr<String^> fnamePtr = &filename;

		if (!Load_Game(reinterpret_cast<char*>(fnamePtr)))
			throw gcnew Exception(gcnew String("Load game procedure failed."));
	}

	virtual void ShowQuickMessage(String^ text, CnCTextColor color, int timeoutMs)
	{
		const auto ticks = timeoutMs == Threading::Timeout::Infinite
			                   ? -1
			                   : (timeoutMs / (1000 / DotNetApiVc::Global_Ticks_Per_Sec));

		auto intPtr = Marshal::StringToHGlobalAnsi(text);
		const auto ptr = static_cast<char*>(static_cast<void*>(intPtr));

		Messages.Add_Message(ptr, static_cast<int>(color), TPF_6PT_GRAD | TPF_USE_GRAD_PAL | TPF_FULLSHADOW, ticks, 0,
		                     0);

		Marshal::FreeHGlobal(intPtr);
	}

	static CnC^ Cs = nullptr;
};

void DotNetApiVc::MainInit(const int argc, char* argv[])
{
	auto strs = gcnew array<String^>(argc);

	for (int i = 0; i < argc; ++i)
		strs[i] = gcnew String(argv[i]);

	const auto root = gcnew CsApi();
	CsApi::Cs = CnC::MainInit(strs, root);
}

void DotNetApiVc::MainLoopStart()
{
}

void DotNetApiVc::MainLoopEnd()
{
}

// void DotNetApiVc::AiStart(const LogicClass& logic)
// {
// }
//
// void DotNetApiVc::AiEnd(const LogicClass& logic)
// {
// }
//
// void DotNetApiVc::OnActivated(const InfantryClass* obj)
// {
// }
//
// void DotNetApiVc::OnDeactivated(const InfantryClass* obj)
// {
// }

void DotNetApiVc::OnKeyInput(const int inputKN, const int inputVK, const bool preview)
{
	CsApi::Cs->ProcessKeyInput(inputKN, inputVK, preview);
}

void DotNetApiVc::DebugBox(const char* str)
{
	IO::File::AppendAllText("DotNetDbg.txt", DateTime::Now.ToString() + " " + gcnew String(str) + Environment::NewLine);
}

void DotNetApiVc::DebugBox(int str)
{
	IO::File::AppendAllText("DotNetDbg.txt", DateTime::Now.ToString() + " " + str.ToString() + Environment::NewLine);
}
