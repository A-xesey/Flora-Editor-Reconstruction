#pragma once

#include <Spore\BasicIncludes.h>
#include <Spore\Simulator\cDefaultBeamTool.h>

using namespace Simulator;

class FESpaceToolEdit 
	: public cDefaultBeamTool
{
public:
	static const uint32_t TYPE = id("FESpaceToolEdit");
	
	FESpaceToolEdit();
	~FESpaceToolEdit();

	virtual bool WhileAiming(cSpaceToolData* pTool, const Vector3& aimPoint, bool showErrors);
	virtual bool OnHit(cSpaceToolData* pTool, const Vector3& position, SpaceToolHit hitType, int) override;

	int AddRef() override;
	int Release() override;
	void* Cast(uint32_t type) const override;
};
