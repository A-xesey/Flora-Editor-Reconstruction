#include "stdafx.h"
#include "FETest.h"
#include <Spore\Editors\BakeManager.h>

FETest::FETest()
{
}


FETest::~FETest()
{
}
void FETest::ParseLine(const ArgScript::Line& line)
{
	auto option_bake = line.GetOption("bake", 0);

	/*auto option_editor = line.GetOption("e", 2);
	int editorUI_part1 = 0;
	int editorUI_part2 = 0;
	editorUI_part1 = mpFormatParser->ParseInt(option_editor[0]);
	editorUI_part2 = mpFormatParser->ParseInt(option_editor[1]);*/

	/*ResourceKey a;
	a.groupID = GroupIDs::FloraModels;
	a.typeID = TypeIDs::flr;
	a.instanceID = mpFormatParser->ParseUInt(option_bake[0]);*/

	auto option_delete = line.GetOption("delete", 0);
	auto option_water = line.GetOption("water", 0);

	//Simulator::cPlanet* planet = Simulator::GetActivePlanet();
	Simulator::cPlanetRecord* planetRecord = Simulator::GetActivePlanetRecord();

	if (option_bake || option_delete || option_water)// || option_editor)
	{
		/*if (option_editor)
		{
			if (Editor.IsActive()) {
				switch (editorUI_part1)
				{
				case 0x60:
					Editor.mpEditorUI->field_60 = editorUI_part2;
					break;
				case 0x74:
					Editor.mpEditorUI->field_74 = editorUI_part2;
					break;
				case 0x7C:
					Editor.mpEditorUI->field_7C = editorUI_part2;
					break;
				case 0x88:
					Editor.mpEditorUI->field_88 = editorUI_part2;
					break;
				case 0x8C:
					Editor.mpEditorUI->field_8C = editorUI_part2;
					break;
				case 0x90:
					Editor.mpEditorUI->field_90 = editorUI_part2;
					break;
				case 0x94:
					Editor.mpEditorUI->field_94 = editorUI_part2;
					break;
				case 0x98:
					Editor.mpEditorUI->field_98 = editorUI_part2;
					break;
				case 0x9C:
					Editor.mpEditorUI->field_9C = editorUI_part2;
					break;
				case 0xA0:
					Editor.mpEditorUI->mbEditorModelForceSaveover = editorUI_part2;
					break;
				case 0xA1:
					Editor.mpEditorUI->field_A1 = editorUI_part2;
					break;
				case 0xA2:
					Editor.mpEditorUI->field_A2 = editorUI_part2;
					break;
				case 0xA4:
					Editor.mpEditorUI->field_A4 = editorUI_part2;
					break;
				case 0xA8:
					Editor.mpEditorUI->field_A8 = editorUI_part2;
					break;
				case 0xAC:
					Editor.mpEditorUI->field_AC = editorUI_part2;
					break;
				case 0xB0:
					Editor.mpEditorUI->field_B4 = editorUI_part2;
					break;
				case 0xB8:
					Editor.mpEditorUI->field_B8 = editorUI_part2;
					break;
				case 0xBC:
					Editor.mpEditorUI->field_BC = editorUI_part2;
					break;
				case 0xC0:
					Editor.mpEditorUI->field_C0 = editorUI_part2;
					break;
				case 0xC4:
					Editor.mpEditorUI->field_C4 = editorUI_part2;
					break;
				case 0xC8:
					Editor.mpEditorUI->field_C8 = editorUI_part2;
					break;
				case 0xCC:
					Editor.mpEditorUI->field_CC = editorUI_part2;
					break;
				case 0xD0:
					Editor.mpEditorUI->field_D0 = editorUI_part2;
					break;
				case 0xD1:
					Editor.mpEditorUI->field_D1 = editorUI_part2;
					break;
				case 0xD4:
					Editor.mpEditorUI->field_D4 = editorUI_part2;
					break;
				case 0xD8:
					Editor.mpEditorUI->field_D8 = editorUI_part2;
					break;
				case 0xDC:
					Editor.mpEditorUI->field_DC = editorUI_part2;
					break;
				case 0xE0:
					Editor.mpEditorUI->field_E0 = editorUI_part2;
					break;
				case 0xE4:
					Editor.mpEditorUI->field_E4 = editorUI_part2;
					break;
				case 0xE5:
					Editor.mpEditorUI->field_E5 = editorUI_part2;
					break;
				case 0xE8:
					Editor.mpEditorUI->field_E8 = editorUI_part2;
					break;
				case 0xEC:
					Editor.mpEditorUI->field_EC = editorUI_part2;
					break;
				case 0xF0:
					Editor.mpEditorUI->field_F0 = editorUI_part2;
					break;
				case 0xF4:
					Editor.mpEditorUI->field_F4 = editorUI_part2;
					break;
				case 0xF8:
					Editor.mpEditorUI->field_F8 = editorUI_part2;
					break;
				case 0xFC:
					Editor.mpEditorUI->field_FC = editorUI_part2;
					break;
				case 0x100:
					Editor.mpEditorUI->field_100 = editorUI_part2;
					break;
				case 0x101:
					Editor.mpEditorUI->field_101 = editorUI_part2;
					break;
				case 0x102:
					Editor.mpEditorUI->field_102 = editorUI_part2;
					break;
				case 0x103:
					Editor.mpEditorUI->field_103 = editorUI_part2;
					break;
				case 0x104:
					Editor.mpEditorUI->field_104 = editorUI_part2;
					break;
				case 0x105:
					Editor.mpEditorUI->field_105 = editorUI_part2;
					break;
				case 0x106:
					Editor.mpEditorUI->field_106 = editorUI_part2;
					break;
				case 0x107:
					Editor.mpEditorUI->field_107 = editorUI_part2;
					break;
				case 0x108:
					Editor.mpEditorUI->field_108 = editorUI_part2;
					break;
				case 0x10C:
					Editor.mpEditorUI->field_10C = editorUI_part2;
					break;
				case 0x110:
					Editor.mpEditorUI->field_110 = editorUI_part2;
					break;
				case 0x128:
					Editor.mpEditorUI->field_128 = editorUI_part2;
					break;
				default:
					App::ConsolePrintF("smh wrong... meow");
					break;
				}
			}
		}
		else
			App::ConsolePrintF("Error: enter the editor before using this option.");*/

		if (option_bake) ///Plant models loading
		{
			//BakeManager.Bake(a, Editors::BakeParameters::BakeParameters(1));
			//if (planet && planetRecord)
			if (planetRecord != 0 && planetRecord->mPlantSpecies.size() > 0)
			{
				eastl::vector<ResourceKey> PlantSpecies = planetRecord->mPlantSpecies;
				if (PlantSpecies.size() > 0)
				{
					for (int i = PlantSpecies.size(); i > 0; --i)
					{
						BakeManager.Bake(PlantSpecies[i - 1], Editors::BakeParameters::BakeParameters(1));
					}
					/*for (ResourceKey plant : PlantSpecies)
					{
						BakeManager.Bake(plant, Editors::BakeParameters::BakeParameters(1));
					}*/
				}
				else
					App::ConsolePrintF("There are no plants on this planet.");
			}
			else
				App::ConsolePrintF("Error: enter the planet before using this option.");
		}

		if (option_water)
		{
			//if (planet != 0 && planetRecord != 0)
			//if (PlanetModel.mpTerrain != 0)
			if (PlanetModel.Get())
			{
				Terrain::DisposeRefractionRTTs();
				Terrain::CreateRefractionRTTs();
			}
			else
				App::ConsolePrintF("Error: enter the planet before using this option.");
		}

		if (option_delete) //delete flora
		{
			if (Editor.IsActive() && Editor.mEditorName == id("FloraEditorSetup")) {
					if (Editor.mSaveExtension == id("flr"))
					{
						Editor.mSaveExtension = id("crt");
						App::ConsolePrintF("DeleteFlora activated: now you can to delete the flora.");
					}
					else
					{
						Editor.mSaveExtension = id("flr");
						App::ConsolePrintF("DeleteFlora deactivated: now you can to save the flora.");
					}
			}
			else
				App::ConsolePrintF("Error: enter the flora editor before using this option.");
		}
	}
	else App::ConsolePrintF("You can only write -bake or -delete option.");
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
