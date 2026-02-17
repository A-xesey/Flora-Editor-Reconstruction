#include "stdafx.h"
#include "FETest.h"

using namespace Terrain;
using namespace Graphics;

FETest::FETest()
{
}

FETest::~FETest()
{
}

void FETest::ParseLine(const ArgScript::Line& line)
{
	App::ConsolePrintF("%i", (PlanetModel.mpTerrain != 0));
	App::ConsolePrintF("%i", (PlanetModel.mpTerrain2 != 0));
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
