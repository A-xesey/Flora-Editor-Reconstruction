#pragma once

#include <Spore\BasicIncludes.h>
#include <Spore\Simulator\cToolStrategy.h>

#define FESpaceToolCreatePtr intrusive_ptr<FESpaceToolCreate>

using namespace Simulator;

class FESpaceToolCreate
	: public cToolStrategy
{
public:
	static const uint32_t TYPE = id("FESpaceToolCreate");
	
	FESpaceToolCreate();
	~FESpaceToolCreate();

	virtual bool WhileFiring(cSpaceToolData* pTool, const Vector3& aimPoint, int) override;

	int AddRef() override;
	int Release() override;
	void* Cast(uint32_t type) const override;
};
