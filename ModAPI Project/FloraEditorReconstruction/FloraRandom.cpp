#include "stdafx.h"
#include "FloraRandom.h"

eastl::vector<ResourceKey> allFlora;
eastl::vector<ResourceKey> allValidFlora;

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

	//creations can't be show up in the sporepedia if it don't have the summary file, so we so used it
	filter.typeID = 0x2D5C9AF;	//summary

	ResourceManager.GetResourceKeyList(allFlora, &filter);

	//a big check if creation has gaprop
	for (const ResourceKey& key : allFlora)
	{
		ResourceKey newKey(key.instanceID, TypeIDs::pollen_metadata, key.groupID);
		ResourceObjectPtr keyObject;
		ResourceManager.GetResource(newKey, &keyObject);
		if (keyObject != nullptr)
		{
			bool hasGAProp = false;
			cAssetMetadataPtr metadata = object_cast<Pollinator::cAssetMetadata>(keyObject);
			if (metadata != nullptr)
			{
				if (metadata->mTags.mpBegin != metadata->mTags.mpEnd)
				{
					for (auto tag : metadata->mTags)
					{
						if (tag == u"gaprop")
						{
							hasGAProp = true;
							break;
						}
					}
				}
				if (!hasGAProp)
					allValidFlora.push_back(key);
			}
		}
	}
}

//tbh i'm not sure if we need it...
void FloraRandom::ClearFloraList()
{
	allFlora.clear();
	allValidFlora.clear();
}

//for game stages, to avoid ecosystem collaps
bool FloraRandom::ValidPlant(const ResourceKey& key, const eastl::vector<ResourceKey>& passSamePlants)
{
	for (const ResourceKey& plant : passSamePlants)
	{
		if (key.instanceID == plant.instanceID)
			return false;
	}
	return true;
}

uint32_t FloraRandom::GetRandomFloraName(bool smallSpecies, bool mediumSpecies, bool largeSpecies, bool isGameMode)
{
	eastl::vector<ResourceKey> randomFloraList;
	if (isGameMode)
		randomFloraList = allValidFlora;
	else
		randomFloraList = allFlora;

	if (randomFloraList.mpBegin != randomFloraList.mpEnd)
	{
		int rando;
		if (smallSpecies && mediumSpecies && largeSpecies)
		{
			rando = rand(randomFloraList.size() - 1);
			return randomFloraList[rando].instanceID;
		}
		else
		{
			eastl::vector<ResourceKey> plants;

			for (const ResourceKey& key : randomFloraList)
			{
				Simulator::cSpeciesProfile* profile = SpeciesManager.GetSpeciesProfile(key);
				if (profile != nullptr)
				{
					if (isGameMode)
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
					else
					{
						if (profile->mModelType == kPlantSmall && smallSpecies)
							plants.push_back(key);
						else if (profile->mModelType == kPlantMedium && mediumSpecies)
							plants.push_back(key);
						else if (profile->mModelType == kPlantLarge && largeSpecies)
							plants.push_back(key);
					}
				}
				profile = nullptr;
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
void* FloraRandom::Cast(uint32_t type) const
{
	CLASS_CAST(Object);
	CLASS_CAST(FloraRandom);
	return nullptr;
}
