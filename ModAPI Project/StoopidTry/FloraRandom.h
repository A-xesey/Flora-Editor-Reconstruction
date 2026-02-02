#pragma once

#include <Spore\BasicIncludes.h>

#define FloraRandomPtr intrusive_ptr<FloraRandom>

class FloraRandom 
	//: public Object
	: public DefaultRefCounted
{
public:
	//static const uint32_t TYPE = id("FloraRandom");
	
	FloraRandom();
	~FloraRandom();

	virtual void CreateFloraList();
	virtual void ClearFloraList();
	virtual bool ValidPlant(const ResourceKey& key, const eastl::vector<ResourceKey>& passSamePlants);
	virtual uint32_t GetRandomFloraName(bool smallSpecies, bool mediumSpecies, bool largeSpecies, bool isGameMode);
	int AddRef() override;
	int Release() override;
	//void* Cast(uint32_t type) const override;
};
