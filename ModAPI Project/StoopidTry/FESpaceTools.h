#pragma once

#include <Spore\BasicIncludes.h>
#include <Spore\Simulator\cToolStrategy.h>

#define FESpaceToolsPtr intrusive_ptr<FESpaceTools>

using namespace Simulator;

class FESpaceTools
	: public cToolStrategy
{
public:
	static const uint32_t TYPE = id("FESpaceTools");
	
	FESpaceTools();
	~FESpaceTools();

	virtual bool Update(cSpaceToolData* pTool, bool showErrors, const char16_t** ppFailText = nullptr) override;
	virtual bool OnHit(cSpaceToolData* pTool, const Vector3& position, SpaceToolHit hitType, int) override;
	virtual bool WhileFiring(cSpaceToolData* pTool, const Vector3& aimPoint, int) override;

	int AddRef() override;
	int Release() override;
	void* Cast(uint32_t type) const override;
};
