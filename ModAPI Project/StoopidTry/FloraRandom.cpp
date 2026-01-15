#include "stdafx.h"
#include "FloraRandom.h"

eastl::vector<ResourceKey> allFlora;

FloraRandom::FloraRandom()
{
}


FloraRandom::~FloraRandom()
{
}

void FloraRandom::CreateFloraList()
{
	Resource::StandardFileFilter filter;
	filter.groupID = GroupIDs::FloraModels;
	filter.typeID = 0x2D5C9AF;	//summary

	ResourceManager.GetResourceKeyList(allFlora, &filter);
}

void FloraRandom::ClearFloraList()
{
	allFlora.clear();
}

bool FloraRandom::ValidPlant(const ResourceKey& key, const eastl::vector<ResourceKey>& passSamePlants)
{
	for (const ResourceKey& plant : passSamePlants)
	{
		if (key.instanceID == plant.instanceID)
			return false;
	}
	return true;
}

uint32_t FloraRandom::GetRandomFloraName(bool smallSpecies, bool mediumSpecies, bool largeSpecies)
{
	if (allFlora.mpBegin != allFlora.mpEnd)
	{
		int rando;
		if (smallSpecies && mediumSpecies && largeSpecies)
		{
			rando = rand(allFlora.size() - 1);
			return allFlora[rando].instanceID;
		}
		else
		{
			eastl::vector<ResourceKey> plants;

			for (const ResourceKey& key : allFlora)
			{
				Simulator::cSpeciesProfile* profile = SpeciesManager.GetSpeciesProfile(key);
				if (profile != nullptr)
				{
					if (profile->mFruits.mpBegin != profile->mFruits.mpEnd)
					{
						if (profile->mModelType == kPlantSmall && smallSpecies)
							plants.push_back(key);
						else if (profile->mModelType == kPlantMedium && mediumSpecies)
							plants.push_back(key);
					}
					if (profile->mModelType == kPlantLarge && largeSpecies)
						plants.push_back(key);
				}
			}
			rando = rand(plants.size() - 1);
			return plants[rando].instanceID;
		}
	}
	return 0;
}

// For internal use, do not modify.
int FloraRandom::AddRef()
{
	return DefaultRefCounted::AddRef();
}

// For internal use, do not modify.
int FloraRandom::Release()
{
	return DefaultRefCounted::Release();
}

// You can extend this function to return any other types your class implements.
//void* FloraRandom::Cast(uint32_t type) const
//{
//	CLASS_CAST(Object);
//	CLASS_CAST(FloraRandom);
//	return nullptr;
//}
