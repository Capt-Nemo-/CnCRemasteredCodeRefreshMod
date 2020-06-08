#include "DotNetApiVc.h"
#include "KEYBOARD.H"
// #include <vcclr.h>

using namespace System;
using namespace DotNetApi;

void DotNetApiVc::MainInit(int argc, char* argv[])
{
	// Windows::Forms::MessageBox::Show(argc.ToString());

	auto strs = gcnew array<String^>(argc);

	for (int i = 0; i < argc; ++i)
		strs[i] = gcnew String(argv[i]);

	DotNetApiCs::MainInit(strs);
	
	// DotNetApiCs::SaveGameHandler = gcnew Func<String^, String^>(DotNetApiVc::SaveGameHandler);
}

void DotNetApiVc::SaveGameHandler(String^ filename, String^ description)
{
	pin_ptr<String^> fnamePtr = &filename, descPtr = &description;
		
}

void DotNetApiVc::MainLoopStart()
{
}

void DotNetApiVc::MainLoopEnd()
{
}

void DotNetApiVc::AiStart(const LogicClass& logic)
{
}

void DotNetApiVc::AiEnd(const LogicClass& logic)
{
}

void DotNetApiVc::OnActivated(InfantryClass* obj)
{
}

void DotNetApiVc::OnDeactivated(InfantryClass* obj)
{
}

void DotNetApiVc::OnKeyInput(const int inputKN, const int inputVK, bool preview)
{
	DotNetApiCs::OnKeyInput(inputKN, inputVK, preview);
}
