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

//bool FloraRandom::ValidPlant(const ResourceKey& key, ModelTypes plantType, const eastl::vector<ResourceKey>& passSamePlants, bool gameMode)
//{
//	Simulator::cSpeciesProfile* profile = SpeciesManager.GetSpeciesProfile(key);
//	if (profile != nullptr && profile->mModelType == plantType)
//	{
//		if (gameMode)
//		{
//			for (const ResourceKey& plant : passSamePlants)
//			{
//				if (key.instanceID == plant.instanceID)
//					return false;
//			}
//		}
//		return true;
//	}
//
//	return false;
//}

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
	if (allFlora.size() != 0)
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

			ResourceKey plantType{ 0, TypeIDs::flr, GroupIDs::FloraModels };
			for (const ResourceKey& key : allFlora)
			{
				plantType.instanceID = key.instanceID;
				Simulator::cSpeciesProfile* profile = SpeciesManager.GetSpeciesProfile(plantType);
				if (profile != nullptr)
				{
					if (profile->mModelType == kPlantSmall && smallSpecies)
						plants.push_back(plantType);
					else if (profile->mModelType == kPlantMedium && mediumSpecies)
						plants.push_back(plantType);
					else if (profile->mModelType == kPlantLarge && largeSpecies)
						plants.push_back(plantType);
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
