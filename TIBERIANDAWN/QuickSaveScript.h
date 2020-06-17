#pragma once

#include "ClrApi.h"

using namespace System;
using namespace System::Windows::Forms;

private ref class QuickSaveScript sealed : MissionScriptBase
{
	String^ GetFilename()
	{
		return "QUICKSAVE_TD_" + CnC->CurrentScenarioName;
	}

public:
	void OnKeyInput(Keys key) override
	{
		switch (key)
		{
		case Keys::F5:
			CnC->SaveGame(GetFilename(), "Quick save slot");
			CnC->ShowQuickMessage("Quick save successful...", 1000);
			break;
		case Keys::F9:
			try
			{
				CnC->LoadGame(GetFilename());
				CnC->ShowQuickMessage("Quick load successful...", 1000);
			}
			catch (IO::FileNotFoundException^)
			{
				CnC->ShowQuickMessage("Error: save file not found...", 2000);
			}
			catch (Exception^)
			{
				CnC->ShowQuickMessage("Error: quick load failed...", 2000);
			}

			break;
		}
	}
};