#pragma once
#include <Spore\BasicIncludes.h>
#include "FESpaceToolCreate.h"
#include "FESpaceToolEdit.h"

#define FEReconSpaceGamePtr intrusive_ptr<FEReconSpaceGame>

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
					if (messageID == 0x3b092aa || messageID == 0x3c5dbc7)
					{
						cPlayerInventoryPtr inventory = SimulatorSpaceGame.GetPlayerInventory();
						if (inventory != nullptr)
						{
							size_t cargoSlots = inventory->GetAvailableCargoSlotsCount();
							if (cargoSlots == 0)
							{
								if (inventory->mpActiveCargoItem != nullptr)
								{
									inventory->mpActiveCargoItem->mbIsActive = false;
									inventory->mpActiveCargoItem = nullptr;
								}
								if (inventory->mpActiveTool != nullptr) {
									MessageManager.MessageSend(0x61dae65, inventory->mpActiveTool.get());
									ToolManager.DeactivateTool(inventory->mpActiveTool.get());
									inventory->mpActiveTool->mbIsActive = false;
									inventory->mpActiveTool = nullptr;

								}
								cSpaceToolDataPtr pTool;
								if (messageID == 0x3b092aa)
									ToolManager.LoadTool({ id("CreatureEdit"),0,0 }, pTool);
								if (messageID == 0x3c5dbc7)
								{
									ToolManager.LoadTool({ id("CreatureCreate"),0,0 }, pTool);
									pTool->mCurrentAmmoCount -= 9;
								}
								if (pTool != nullptr)
								{
									pTool->ShowEventLog(pTool.get(), id("FullInventory"));
									inventory->AddItem(pTool.get());
								}
							}
						}
					}
					return false;
				}
				if (*(char*)(shit + 0x44) == '\0')
				{
					ResourceKey creation = *(ResourceKey*)(shit + 0x18);
					if (creation.instanceID != 0 && creation.groupID == GroupIDs::FloraModels && creation.typeID == TypeIDs::flr)
					{
						cPlantCargoInfoPtr plantCargo = PlantSpeciesManager.CreatePlantItem(creation);
						cPlayerInventoryPtr inventory = SimulatorSpaceGame.GetPlayerInventory();
						if (plantCargo != nullptr && inventory != nullptr)
						{
							plantCargo->mItemCount = 5;
							inventory->AddItem(plantCargo.get());
						}
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