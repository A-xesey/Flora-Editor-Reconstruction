#include "stdafx.h"
#include "FETest.h"

using namespace Editors;
using namespace App;

FETest::FETest()
{
}

FETest::~FETest()
{
}

bool showLog = false;

void FETest::ParseLine(const ArgScript::Line& line)
{
	showLog = !showLog;
	if (showLog) ConsolePrintF("FERLog: Turned on. Now in console will sending functions information");
	else ConsolePrintF("FERLog: Turned Off");
}

const char* FETest::GetDescription(ArgScript::DescriptionMode mode) const
{
	if (mode == ArgScript::DescriptionMode::Basic) {
		return "FERLog used for logging Flora Editor Reconstruction functions";
	}
	else {
		return "FERLog used for logging Flora Editor Reconstruction functions. Used it if you get a bug and send it to mod author";
	}
}
