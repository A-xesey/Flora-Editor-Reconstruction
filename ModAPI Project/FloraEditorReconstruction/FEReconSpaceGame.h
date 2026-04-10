#pragma once
#include <Spore\BasicIncludes.h>
#include "FESpaceToolCreate.h"
#include "FESpaceToolEdit.h"

using namespace Simulator;

class FEReconSpaceGame
{
public:

	static void Initialize() {
		ToolManager.AddStrategy(new FESpaceToolCreate(), id("FEReconPlantStrategyCreate"));
		ToolManager.AddStrategy(new FESpaceToolEdit(), id("FEReconPlantStrategyEdit"));
	}

	static void Dispose() {
	}

	static void AttachDetours();
};

////Game features
///Create/Edit plant tools
member_detour(HandleMessage, cSimulatorSpaceGame, bool(uint32_t, int))
{
	bool detoured(uint32_t messageID, int shit)
	{
		bool func = original_function(this, messageID, shit);
		if (!func)
		{
			if (messageID != 0x44ebe3f)
			{
				if (0x37aac1d < messageID)
				{
					//if creature was not created then return tool back
					if (messageID == 0x3b092aa || messageID == 0x3c5dbc7)
					{
						if (showLog) ConsolePrintF("cSimulatorSpaceGame_HandleMessage: Creature tool");
						cPlayerInventoryPtr inventory = SimulatorSpaceGame.GetPlayerInventory();
						if (inventory != nullptr)
						{
							size_t cargoSlots = inventory->GetAvailableCargoSlotsCount();
							if (showLog) ConsolePrintF("cSimulatorSpaceGame_HandleMessage: cargoSlots is %i", cargoSlots);
							if (cargoSlots == 0)
							{
								//SP::cSPSimulatorSpaceGame::ClearActiveTool
								CALL(Address(ModAPI::ChooseAddress(0xfc83d0, 0xff3d30)), void, Args(cPlayerInventory*), Args(inventory.get()));
								//SPGDeactivateActives::DeactivateActives(inventory.get());
								cSpaceToolDataPtr pTool;
								if (messageID == 0x3b092aa)
									ToolManager.LoadTool({ id("CreatureEdit"),0,0 }, pTool);
								if (messageID == 0x3c5dbc7)
								{
									ToolManager.LoadTool({ id("CreatureCreate"),0,0 }, pTool);
									pTool->mCurrentAmmoCount = 1;// -= 9;
								}
								if (pTool != nullptr)
								{
									if (showLog) ConsolePrintF("cSimulatorSpaceGame_HandleMessage: pTool exists");
									pTool->ShowEventLog(pTool.get(), id("FullInventory"));
									inventory->AddItem(pTool.get());
								}
								else if  (showLog) ConsolePrintF("cSimulatorSpaceGame_HandleMessage: pTool does not exists");
							}
						}
					}
					return false;
				}
				//if plant was created then return plant as cargo item
				if (*(char*)(shit + 0x44) == '\0')
				{
					if (showLog) ConsolePrintF("cSimulatorSpaceGame_HandleMessage: Plant tool");
					ResourceKey creation = *(ResourceKey*)(shit + 0x18);
					if (creation.instanceID != 0 && creation.groupID == GroupIDs::FloraModels && creation.typeID == TypeIDs::flr)
					{
						if (showLog) ConsolePrintF("cSimulatorSpaceGame_HandleMessage: Plant Key exists");
						cPlantCargoInfoPtr plantCargo = PlantSpeciesManager.CreatePlantItem(creation);
						cPlayerInventoryPtr inventory = SimulatorSpaceGame.GetPlayerInventory();
						if (plantCargo != nullptr && inventory != nullptr)
						{
							if (showLog) ConsolePrintF("cSimulatorSpaceGame_HandleMessage: plant cargo was created");
							plantCargo->mItemCount = 5;
							inventory->AddItem(plantCargo.get());
						} else if (showLog) ConsolePrintF("cSimulatorSpaceGame_HandleMessage: plant item or player inventory does not exists");
					}
				}
			}
			return false;
		}
		return func;
	}
};


void FEReconSpaceGame::AttachDetours()
{
	//Game features
	HandleMessage::attach(Address(ModAPI::ChooseAddress(0x00fdfd50, 0x0100b730)));
}