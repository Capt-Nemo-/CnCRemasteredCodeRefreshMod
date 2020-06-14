#pragma once
// #include "INFANTRY.H"
// #include "LOGIC.H"

class DotNetApiVc
{
public:
	static void MainInit(int argc, char *argv[]);

	static void MainLoop();
	static void ProcessKeystrokes();
	static void Shutdown();
	
	// static void DebugLine(const char* str);
	// static void DebugLine(int str);

	// static void AiStart(const LogicClass& logic);
	// static void AiEnd(const LogicClass& logic);
	// static void OnActivated(const InfantryClass* obj);
	// static void OnDeactivated(const InfantryClass* obj);
	// static void OnKeyInput(int inputKN, int inputVK, bool preview);
	// static void DebugBox(const char* str);
	// static void DebugBox(int str);
};