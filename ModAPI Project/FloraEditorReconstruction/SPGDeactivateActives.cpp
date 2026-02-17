#include "stdafx.h"
#include "SPGDeactivateActives.h"

//SPGDeactivateActives::SPGDeactivateActives()
//{
//}
//
//
//SPGDeactivateActives::~SPGDeactivateActives()
//{
//}

void SPGDeactivateActives::DeactivateActives(cPlayerInventory* inventory)
{
	if (inventory != nullptr)
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
	}
}

// For internal use, do not modify.
//int SPGDeactivateActives::AddRef()
//{
//	return DefaultRefCounted::AddRef();
//}
//
//// For internal use, do not modify.
//int SPGDeactivateActives::Release()
//{
//	return DefaultRefCounted::Release();
//}