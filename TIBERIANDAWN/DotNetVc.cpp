#include "DotNetVc.h"
#include "ClrApi.h"
#include "QuickSaveScript.h"

// #include <msclr/marshal.h>
// #include <vcclr.h>

using namespace System;
using namespace Runtime::InteropServices;
using namespace System::Windows::Forms;
using namespace Threading;
using namespace Collections::Generic;
using namespace Collections::Concurrent;

void DotNetVc::MainInit(const int argc, char* argv[])
{
	// Shutdown();
	//
	// auto strs = gcnew array<String^>(argc);
	//
	// for (int i = 0; i < argc; ++i)
	// 	strs[i] = gcnew String(argv[i]);
	//
	// ClrApi::Cs = gcnew ClrApi(strs, gcnew QuickSaveScript());
}

void DotNetVc::MainLoop()
{
	// ClrApi::Cs->MainLoop();
}

void DotNetVc::ProcessKeystrokes()
{
	// ClrApi::Cs->ProcessKeystrokes();
}

void DotNetVc::Shutdown()
{
	// if (ClrApi::Cs != nullptr)
	// 	ClrApi::Cs->Shutdown();
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
