#include "stdafx.h"
#include "FEBlackWaterFixMsg.h"

using namespace Simulator;
using namespace Terrain;

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
		if (GetRefractionBuffersRenderTargetID() != nullptr &&
			GetRefractionBlur1RenderTargetID() != nullptr &&
			GetRefractionBlur2RenderTargetID() != nullptr)
			if (GetCurrentContext() != SpaceContext::Planet ||
				(GameModeManager.GetActiveModeID() < 0x1654C00 || GameModeManager.GetActiveModeID() > 0x1654C10))
			{
				DisposeRefractionRTTs();
				CreateRefractionRTTs();
				return true;
			}
	}
	// Return true if the message has been handled. Other listeners will receive the message regardless of the return value.
	return false;
}
