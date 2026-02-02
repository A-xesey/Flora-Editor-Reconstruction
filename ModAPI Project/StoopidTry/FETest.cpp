#include "stdafx.h"
#include "FETest.h"
#include <Spore\Simulator\SubSystem\cSpeciesRelationshipManager.h>

using namespace Simulator;

FETest::FETest()
{
}


FETest::~FETest()
{
}

void FETest::ParseLine(const ArgScript::Line& line)
{
}

const char* FETest::GetDescription(ArgScript::DescriptionMode mode) const
{
	if (mode == ArgScript::DescriptionMode::Basic) {
		return "This cheat does something.";
	}
	else {
		return "FETest: Elaborate description of what this cheat does.";
	}
}
