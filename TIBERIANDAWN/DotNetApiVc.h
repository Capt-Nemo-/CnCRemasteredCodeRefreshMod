#pragma once
#include "infantry.h"
#include "logic.h"

//..\bin\$(Configuration)\

class DotNetApiVc
{
public:
	static void MainInit(int argc, char *argv[]);

	static void MainLoopStart();
	static void MainLoopEnd();
	
	static void AiStart(const LogicClass& logic);
	static void AiEnd(const LogicClass& logic);
	static void OnActivated(InfantryClass* obj);
	static void OnDeactivated(InfantryClass* obj);
	static void OnKeyInput(const int inputKN, const int inputVK, bool preview);
// private:
// 	static void SaveGameHandler(System::String^ filename, System::String^ description);
};