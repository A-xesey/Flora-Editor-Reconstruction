#include "stdafx.h"
#include "FloraRandom.h"

extern bool showLog;

using namespace App;

RandomNumberGenerator randomGen;

vector<ResourceKey> allFlora;
vector<ResourceKey> allValidFlora;

bool CreateFloraList()
{
	DatabasePtr data = Resource::Paths::GetSaveArea(Resource::SaveAreaID::Cache);
	if (data != nullptr) {
		Resource::StandardFileFilter filter;
		filter.groupID = GroupIDs::FloraModels;

		//creations can't be show up in the sporepedia if it don't have the summary file, so we will use this type
		filter.typeID = 0x2D5C9AF;	//summary
		data->GetKeyList(allFlora, &filter);
		
		//has creation a gaprop?
		for (const ResourceKey& key : allFlora) {
			cAssetMetadataPtr metadata;
			if (Pollinator::GetMetadata(key.instanceID, key.groupID, metadata)) {
				bool hasGAProp = false;
				if (!metadata->mTags.empty()) {
					auto gaprop = find(metadata->mTags.begin(), metadata->mTags.end(), u"gaprop");
					if (gaprop != metadata->mTags.end())
						hasGAProp = true;
					gaprop = nullptr;
				}
				if (!hasGAProp) allValidFlora.push_back(key);
			}
		}
		if (showLog) ConsolePrintF("FloraRandom_CreateFloraList: allFlora (is empty?) %i\n size is %i\n allValidFlora size %i", allFlora.empty(), allFlora.size(), allValidFlora.size());
		return true;
	}
	return false;
}

void ClearFloraList()
{
	allFlora.clear(); allFlora.resize(0);
	allValidFlora.clear(); allValidFlora.resize(0);
}

//for game stages, to avoid ecosystem collaps
bool ValidPlant(const ResourceKey& key, const vector<ResourceKey>& plantList)
{
	bool res = true;
	if (find(plantList.begin(), plantList.end(), key) != plantList.end()) res = false;
	/*for (const ResourceKey& plant : plantList)
	{
		if (key.instanceID == plant.instanceID) {
			res = false;
			break;
		}
	}*/
	if (showLog) ConsolePrintF("FloraRandom_ValidPlant: result is %i", res);
	return res;
}

uint32_t GetRandomFloraName(bool smallSpecies, bool mediumSpecies, bool largeSpecies, bool isGameMode)
{
	vector<ResourceKey> randomFloraList;
	if (isGameMode) randomFloraList = allValidFlora;
	else randomFloraList = allFlora;

	if (showLog) ConsolePrintF("==== FloraRandom_GetRandomFloraName ====\nrandomFloraList (is empty?): %i", randomFloraList.empty());
	if (!randomFloraList.empty()) {
		int rando;
		if (smallSpecies && mediumSpecies && largeSpecies) {
			if (showLog) ConsolePrintF("full randomization");
			rando = randomGen.RandomInt(randomFloraList.size());
			return randomFloraList[rando].instanceID;
		}
		else {
			if (showLog) ConsolePrintF("randomization by plant type");
			vector<ResourceKey> plants;

			for (const ResourceKey& key : randomFloraList) {
				Simulator::cSpeciesProfile* profile = SpeciesManager.GetSpeciesProfile(key);
				if (profile != nullptr) {
					if (isGameMode)
					{
						if (!profile->mFruits.empty())
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
			rando = randomGen.RandomInt(plants.size());
			return plants[rando].instanceID;
		}
	}
	return 0;
}
