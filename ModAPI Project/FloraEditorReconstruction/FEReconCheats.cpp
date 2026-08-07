#include "stdafx.h"
#include "FEReconCheats.h"

using namespace App;

FEReconCheats::FEReconCheats() {}
FEReconCheats::~FEReconCheats() {}

bool showLog = false;

void FEReconCheats::ParseLine(const ArgScript::Line& line)
{
	if (line.HasFlag("log")) {
		showLog = !showLog;
		if (showLog) ConsolePrintF("FERecon log: turned on. Now in console will sending information.");
		else ConsolePrintF("FERecon log: turned off");
	}
	else if (line.HasFlag("hotkeys"))
		ConsolePrintF("In flora editor:\n\tCtrl + Q: show type chooser\n\tShift + F: swap building manipulator to vehicle and back");
	else
		ConsolePrintF("You can only use -log and -hotkeys flags.");
}

const char* FEReconCheats::GetDescription(ArgScript::DescriptionMode mode) const
{
	return "flags:\n-hotkeys: show hotkeys that can be used\n-log: to see how almost all mod's code works";
}
