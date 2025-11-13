#include "stdafx.h"
#include "FESpaceTools.h"
#include <Spore\Simulator\cSpaceToolData.h>
#include <Spore\Simulator\cGamePlant.h>

using namespace Simulator;

FESpaceTools::FESpaceTools()
{
}


FESpaceTools::~FESpaceTools()
{
}

bool FESpaceTools::Update(cSpaceToolData* pTool, bool showErrors, const char16_t** ppFailText = nullptr)
{
	if (pTool->GetItemInstanceID() == id("FE_PlantEdit"))
	{
		auto GamePlant = GetData<cGamePlant>();
		for (auto gameplant : GamePlant)
		{
			if (auto plantModel = object_cast<cSpatialObject>(gameplant))
			{
				if (pTool->mpToolTarget == plantModel)
				{
					App::ConsolePrintF("meow");
					return true;
				}
			}
		}
	}
	return false;
}

bool FESpaceTools::OnHit(cSpaceToolData* pTool, const Vector3& position, SpaceToolHit hitType, int)	//for Edit
{
	return false;
}

bool FESpaceTools::WhileFiring(cSpaceToolData* pTool, const Vector3& aimPoint, int)	//for New
{
	if (pTool->GetItemInstanceID() == id("FE_PlantCreate"))
	{
		App::ConsolePrintF("mreow");
		return true;
	}
	return false;
}

// For internal use, do not modify.
int FESpaceTools::AddRef()
{
	return DefaultRefCounted::AddRef();
}

// For internal use, do not modify.
int FESpaceTools::Release()
{
	return DefaultRefCounted::Release();
}

// You can extend this function to return any other types your class implements.
void* FESpaceTools::Cast(uint32_t type) const
{
	CLASS_CAST(Object);
	CLASS_CAST(Simulator::cToolStrategy);
	CLASS_CAST(FESpaceTools);
	return nullptr;
}
