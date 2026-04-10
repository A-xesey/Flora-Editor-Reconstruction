#include "stdafx.h"
#include "FESpaceToolCreate.h"

extern bool showLog;

using namespace Simulator;
using namespace App;

FESpaceToolCreate::FESpaceToolCreate()
{
}


FESpaceToolCreate::~FESpaceToolCreate()
{
}

bool FESpaceToolCreate::WhileFiring(cSpaceToolData* pTool, const Vector3& aimPoint, int pSpaceToolStrategy)	//for New
{
	bool WhileFiring = cToolStrategy::WhileFiring(pTool, aimPoint, pSpaceToolStrategy);
	if (showLog) ConsolePrintF("==== FESpaceToolCreate_WhileFiring ====\noriginal function: %i", WhileFiring);
	if (WhileFiring)
	{
		cPlayerInventoryPtr inventory = SimulatorSpaceGame.GetPlayerInventory();
		size_t availableCargoSlots = inventory->GetAvailableCargoSlotsCount();
		if (showLog) ConsolePrintF("availableCargoSlots: %i", availableCargoSlots);
		if (availableCargoSlots != 0)
		{
			//SPGDeactivateActives::DeactivateActives(inventory.get());
			//SP::cSPSimulatorSpaceGame::ClearActiveTool
			CALL(Address(ModAPI::ChooseAddress(0xfc83d0, 0xff3d30)), void, Args(cPlayerInventory*), Args(inventory.get()));
			EditorRequestPtr editorRequest = new Editors::EditorRequest();
			editorRequest->editorID = id("FloraEditorUFO");
			editorRequest->sporepediaCanSwitch = false;
			editorRequest->allowSporepedia = false;
			editorRequest->hasCreateNewButton = false;
			editorRequest->hasSaveButton = false;
			editorRequest->hasExitButton = false;
			editorRequest->field_3C = true;
			editorRequest->field_3D = true;
			editorRequest->field_64 = true;
			SimGameModeManager.SubmitEditorRequest(editorRequest.get());
			if (showLog) ConsolePrintF("editor request was submited");
		}
		else
			pTool->ShowEventLog(pTool, id("FullInventory"));
	}
	return WhileFiring;
}

// For internal use, do not modify.
int FESpaceToolCreate::AddRef()
{
	return DefaultRefCounted::AddRef();
}

// For internal use, do not modify.
int FESpaceToolCreate::Release()
{
	return DefaultRefCounted::Release();
}

// You can extend this function to return any other types your class implements.
void* FESpaceToolCreate::Cast(uint32_t type) const
{
	CLASS_CAST(Object);
	CLASS_CAST(cToolStrategy);
	CLASS_CAST(FESpaceToolCreate);
	return nullptr;
}
