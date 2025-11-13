#include "stdafx.h"
#include "FEBlackWaterFixMsg.h"

FEBlackWaterFixMsg::FEBlackWaterFixMsg()
{
}


FEBlackWaterFixMsg::~FEBlackWaterFixMsg()
{
}

// For internal use, do not modify.
int FEBlackWaterFixMsg::AddRef()
{
	return DefaultRefCounted::AddRef();
}

// For internal use, do not modify.
int FEBlackWaterFixMsg::Release()
{
	return DefaultRefCounted::Release();
}


// The method that receives the message. The first thing you should do is checking what ID sent this message...
bool FEBlackWaterFixMsg::HandleMessage(uint32_t messageID, void* message)
{
	if (messageID == id("BakeManager_BakeSprites"))
	{
		Graphics::RenderTargetID* refractBuffers = Terrain::GetRefractionBuffersRenderTargetID();
		Graphics::RenderTargetID* refractBlur1 = Terrain::GetRefractionBlur1RenderTargetID();
		Graphics::RenderTargetID* refractBlur2 = Terrain::GetRefractionBlur2RenderTargetID();
		if (refractBuffers != 0 && refractBlur1 != 0 && refractBlur2 != 0
			&& ((PlanetModel.mpTerrain == 0 && PlanetModel.mpTerrain2 == 0)
				|| (PlanetModel.mpTerrain != 0 && PlanetModel.mpTerrain2 != 0
					&& GameModeManager.GetActiveModeID() == GameModeIDs::kEditorMode)))
		{
			Terrain::DisposeRefractionRTTs();
			Terrain::CreateRefractionRTTs();
			return true;
		}
	}
	// Return true if the message has been handled. Other listeners will receive the message regardless of the return value.
	return false;
}
