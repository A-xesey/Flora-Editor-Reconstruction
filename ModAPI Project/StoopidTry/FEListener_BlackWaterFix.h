#pragma once

#include <Spore\BasicIncludes.h>

#define FEListener_BlackWaterFixPtr intrusive_ptr<FEListener_BlackWaterFix>

class FEListener_BlackWaterFix 
	: public App::IMessageListener
	, public DefaultRefCounted
{
public:
	static const uint32_t TYPE = id("FEListener_BlackWaterFix");

	FEListener_BlackWaterFix();
	~FEListener_BlackWaterFix();

	int AddRef() override;
	int Release() override;
	
	// This is the function you have to implement, called when a message you registered to is sent.
	bool HandleMessage(uint32_t messageID, void* message) override;
};
