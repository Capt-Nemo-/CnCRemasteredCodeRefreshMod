#include "DotNetApiVc.h"
// #include <vcclr.h>

using namespace System;

void DotNetApiVc::MainInit(int argc, char *argv[])
{
	// Windows::Forms::MessageBox::Show(argc.ToString());
	
	auto strs = gcnew array<String^>(argc);
	
	for (int i = 0; i < argc; ++i)		
		strs[i] = gcnew String(argv[i]);
	
	DotNetApi::DotNetApiCs::MainInit(strs);	
}
