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
	static void OnActivated(const InfantryClass* obj);
	static void OnDeactivated(const InfantryClass* obj);
	static void OnKeyInput(int inputKN, int inputVK, bool preview);
};