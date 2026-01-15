#include "stdafx.h"
#include "FESpaceToolEdit.h"
#include "SPGDeactivateActives.h"

FESpaceToolEdit::FESpaceToolEdit()
{
}


FESpaceToolEdit::~FESpaceToolEdit()
{
}

bool FESpaceToolEdit::WhileAiming(cSpaceToolData* pTool, const Vector3& aimPoint, bool showErrors)
{
	bool WhileAiming = cToolStrategy::WhileAiming(pTool, aimPoint, showErrors);
	
	eastl::vector<cSpatialObjectPtr> spatialTarget;
	bool targetFound = GameViewManager.IntersectSphere(aimPoint, pTool->mDamageRadius*10, spatialTarget);
	if (targetFound)
	{
		for (auto target : spatialTarget)
		{
			if (object_cast<cGamePlant>(target) == nullptr)
				return false;
		}
	}

	return WhileAiming;
}

bool FESpaceToolEdit::OnHit(cSpaceToolData* pTool, const Vector3& position, SpaceToolHit hitType, int pSpaceToolStrategy)	//for Edit
{
	bool OnHit = cDefaultBeamTool::OnHit(pTool, position, kHitCombatant, pSpaceToolStrategy);
	if (OnHit)
	{
		auto beam = pTool->mpBeam;
		if (beam != nullptr)
		{
			cGameDataPtr gameData = beam->mpTargetedCombatant;
			if (gameData != nullptr)
			{
				ResourceKey plant;
				if (object_cast<cGamePlant>(gameData) != nullptr)
					plant = object_cast<cGamePlant>(gameData)->mSpecies;

				else if (object_cast<cObstacle>(gameData) != nullptr &&
					(object_cast<cObstacle>(gameData)->mPlantType == cObstacle::kLargeSpecies
						|| object_cast<cObstacle>(gameData)->mPlantType == cObstacle::kMediumSpecies
						|| object_cast<cObstacle>(gameData)->mPlantType == cObstacle::kSmallSpecies))
					plant = object_cast<cObstacle>(gameData)->mSpeciesKey;
				else
					return true;

				cPlayerInventoryPtr inventory = SimulatorSpaceGame.GetPlayerInventory();
				size_t availableCargoSlots = inventory->GetAvailableCargoSlotsCount();
				if (availableCargoSlots != 0)
				{
					beam->mbStopBeam = true;
					SPGDeactivateActives::DeactivateActives(inventory.get());
					EditorRequestPtr editorRequest = new Editors::EditorRequest();
					editorRequest->editorID = id("FloraEditorSetupUFO");
					if (plant.instanceID != 0)
					{
						editorRequest->creationKey.instanceID = plant.instanceID;
						editorRequest->creationKey.typeID = plant.typeID;
						editorRequest->creationKey.groupID = plant.groupID;
					}
					//auto a = &editorRequest->field_94;
					editorRequest->sporepediaCanSwitch = false;
					editorRequest->allowSporepedia = false;
					editorRequest->hasCreateNewButton = false;
					editorRequest->hasSaveButton = false;
					editorRequest->hasExitButton = false;
					editorRequest->field_3C = true;
					editorRequest->field_3D = true;
					editorRequest->field_64 = true;
					SimGameModeManager.SubmitEditorRequest(editorRequest.get());
				}
				else
					pTool->ShowEventLog(pTool, id("FullInventory"));
			}
		}
	}
	return true;
	//uint32_t messageID;
	//auto vehicle = object_cast<cVehicle>(gameData);
	//if (vehicle == nullptr) {
	//	auto building = object_cast<cBuilding>(gameData);
	//	if (building == nullptr) {
	//		auto gameUFO = object_cast<cGameDataUFO>(gameData);
	//		if (gameUFO == nullptr) {
	//			auto planetaryArtifact = object_cast<cPlanetaryArtifact>(gameData);
	//			if (planetaryArtifact == nullptr) {
	//				auto ornament = object_cast<cOrnament>(gameData);
	//				if (ornament == nullptr) goto LAB_0105a9ce;
	//				messageID = 0xf46093da;
	//			}
	//			else {
	//				messageID = 0xf46092d3;
	//			}
	//			MessageManager.MessageSend(messageID, 0);
	//		}
	//		else {
	//			App::ConsolePrintF("0x%x", gameUFO->GetCastID());
	//			//flag = FUN_010533e0(gameUFO);
	//		}
	//	}
	//	else {
	//		App::ConsolePrintF("0x%x", building->GetCastID());
	//		//flag = FUN_01053250(building;
	//	}
	//}
	//else {
	//	App::ConsolePrintF("0x%x", vehicle->GetCastID());
	//	//flag = FUN_01053330(vehicle);
	//}
	/*LAB_0105a9ce:
		return true;*/
	//eastl::vector<cSpatialObjectPtr> combatantsTarget;
	//bool targetFound = GameViewManager.IntersectSphere(position, pTool->mDamageRadius, combatantsTarget, true);
	//if (targetFound)
	//{
	//	for (auto targetObject : combatantsTarget)
	//	{
	//		cSpatialObjectPtr plantObject;
	//		ResourceKey plant;
	//		if (object_cast<cGamePlant>(targetObject) != nullptr)
	//		{
	//			plantObject = object_cast<cGamePlant>(targetObject);
	//			plant = ((cGamePlant*)plantObject.get())->mSpecies;
	//		}
	//		else
	//		{
	//			pTool->AddAmmo(1);
	//			//pTool->ShowEventLog();
	//			return false;
	//		}
	//		cPlayerInventoryPtr inventory = SimulatorSpaceGame.GetPlayerInventory();
	//		size_t availableCargoSlots = inventory->GetAvailableCargoSlotsCount();
	//		if (availableCargoSlots != 0)
	//		{
	//			SimulatorPlayerUFO.field_18 = false;
	//			if (inventory != nullptr)
	//			{
	//				pTool->mbIsActive = false;
	//				ToolManager.DeactivateTool(pTool);
	//			}
	//			EditorRequestPtr editorRequest = new Editors::EditorRequest();
	//			editorRequest->editorID = id("FloraEditorSetupUFO");
	//			if (plant.instanceID != 0)
	//			{
	//				editorRequest->creationKey.instanceID = plant.instanceID;
	//				editorRequest->creationKey.typeID = plant.typeID;
	//				editorRequest->creationKey.groupID = plant.groupID;
	//			}
	//			//auto a = &editorRequest->field_94;
	//			editorRequest->sporepediaCanSwitch = false;
	//			editorRequest->allowSporepedia = false;
	//			editorRequest->hasCreateNewButton = false;
	//			editorRequest->hasSaveButton = false;
	//			editorRequest->hasExitButton = false;
	//			editorRequest->field_3C = true;
	//			editorRequest->field_3D = true;
	//			editorRequest->field_64 = true;
	//			SimGameModeManager.SubmitEditorRequest(editorRequest.get());
	//			return true;
	//		}
	//		else
	//		{
	//			pTool->AddAmmo(1);
	//			//pTool->ShowEventLog();
	//			return false;
	//		}
	//	}
	//}
}

// For internal use, do not modify.
int FESpaceToolEdit::AddRef()
{
	return DefaultRefCounted::AddRef();
}

// For internal use, do not modify.
int FESpaceToolEdit::Release()
{
	return DefaultRefCounted::Release();
}

// You can extend this function to return any other types your class implements.
void* FESpaceToolEdit::Cast(uint32_t type) const
{
	CLASS_CAST(Object);
	CLASS_CAST(cDefaultBeamTool);
	CLASS_CAST(FESpaceToolEdit);
	return nullptr;
}
