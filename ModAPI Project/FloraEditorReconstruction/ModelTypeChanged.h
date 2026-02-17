#pragma once

#include <Spore\BasicIncludes.h>

#define ModelTypeChangedPtr intrusive_ptr<ModelTypeChanged>

class ModelTypeChanged 
	: public App::IMessageListener
	, public DefaultRefCounted
{
public:
	static const uint32_t TYPE = id("ModelTypeChanged");

	ModelTypeChanged();
	~ModelTypeChanged();

	int AddRef() override;
	int Release() override;
	
	// This is the function you have to implement, called when a message you registered to is sent.
	bool HandleMessage(uint32_t messageID, void* message) override;
};
