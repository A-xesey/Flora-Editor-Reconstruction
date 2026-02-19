#pragma once
#include <Spore\BasicIncludes.h>
#include <Spore\Editors\BakeManager.h>
#include "FEBlackWaterFixMsg.h"
#include "FloraRandom.h"

using namespace App;
using namespace Simulator;

FEBlackWaterFixMsgPtr messageBakeSprites;
FloraRandomPtr floraRandom;

class FEReconBakeManager
{
public:
	static void Initialize() {
		floraRandom = new FloraRandom();
		messageBakeSprites = new FEBlackWaterFixMsg();
		MessageManager.AddListener(messageBakeSprites.get(), id("BakeManager_BakeSprites"));
	}

	static void Dispose() {
		floraRandom = nullptr;
		MessageManager.RemoveListener(messageBakeSprites.get(), id("BakeManager_BakeSprites"));
		messageBakeSprites = nullptr;
	}

	static void AttachDetours();
};

///fix 3 sec freeze during sprites baking
static_detour(BakeManager_SpriteTextureMIPs, void(int, int, int, int))
{
	void detoured(int param_1, int textureQuality, int textureDXT, int param_4)
	{
		original_function(param_1, textureQuality, 0, param_4);
		return;
	}
};

///detour it for message
static_detour(BakeManager_BakeSprites, void(PropertyList*, Graphics::IModelWorld*, uint32_t))
{
	void detoured(PropertyList * templateProp, Graphics::IModelWorld * creationModel, uint32_t message)
	{
		return original_function(templateProp, creationModel, id("BakeManager_BakeSprites"));
	}
};

member_detour(cPlantSpeciesManager_CreatePlantItem, cPlantSpeciesManager, cPlantCargoInfo* (const ResourceKey&))
{
	cPlantCargoInfo* detoured(const ResourceKey & speciesID)
	{
		cPlantCargoInfo* plantItem = original_function(this, speciesID);
		if (plantItem != nullptr)
		{
			ResourceKey plantKey = plantItem->GetItemID();
			if (plantKey.instanceID != 0 && plantKey.groupID != 0 && plantKey.typeID != 0)
			{
				//plant baking
				bool IsBakingOrBaked = BakeManager.IsBakingOrBaked(plantKey);
				//this functions works outside of planets, so i decided to add this check
				if (GetCurrentContext() == SpaceContext::Planet && !IsBakingOrBaked)
				{
					BakeManager.BakeModel(plantKey, Editors::BakeParameters::BakeParameters(0, 4, 0x609b763));

					PropertyListPtr PlantItemProperty;
					PropManager.GetPropertyList(plantKey.instanceID, plantKey.groupID, PlantItemProperty);

					//since I didn't find are functions that update plant models, we need to update it during the baking
					if (PlantItemProperty != nullptr)
					{
						float mBaseRadius;
						float mCanopyRadius;
						float mHeight;

						Property::GetFloat(PlantItemProperty.get(), 0x0254CF89, mBaseRadius);
						Property::GetFloat(PlantItemProperty.get(), 0x0254CF8F, mCanopyRadius);
						Property::GetFloat(PlantItemProperty.get(), 0x0254CF97, mHeight);

						//always passes the check, so commented out
						/*bool IsBaked = BakeManager.IsBaked(plantKey, false);
						set models to show plant in game
						if (!IsBaked)*/
						{
							//floraimposter initializes only after baking, so we need a temporary solution to the problem
							float FloraImposterScaleTemp = (mBaseRadius + mCanopyRadius + mHeight) / 3;

							//ConsolePrintF("FloraImposterScaleTemp 1 %f", FloraImposterScaleTemp);
							//Simulator::cSpeciesProfile* profile = SpeciesManager.GetSpeciesProfile(plantKey);
							//Vector3 BBoxMax = -profile->mBoundingBox.upper;
							//Vector3 BBoxMin = profile->mBoundingBox.lower;
							//if (BBoxMin.x <= BBoxMax.x) BBoxMin.x = BBoxMax.x;
							//if (BBoxMin.y <= BBoxMax.y) BBoxMin.y = BBoxMax.y;
							//BBoxMin.z *= 0.5f;
							////BBoxMax.z *= 2.0f;
							////if (BBoxMin.z <= BBoxMax.z) BBoxMin.z = BBoxMax.z;
							//if (BBoxMin.z < BBoxMax.y) BBoxMin.z = BBoxMax.y;
							//float result = (BBoxMin.x + BBoxMin.y + BBoxMin.z)/6;
							//FloraImposterScaleTemp = result;
							//ConsolePrintF("FloraImposterScaleTemp 2 %f", FloraImposterScaleTemp);

							ResourceKey PlantModelLOD0(plantKey.instanceID, TypeIDs::gmdl, 0x40667100);
							ResourceKey PlantModelLOD1(plantKey.instanceID, TypeIDs::gmdl, 0x40666201);
							ResourceKey PlantModelLOD2(plantKey.instanceID, TypeIDs::gmdl, 0x40666202);
							ResourceKey PlantModelLOD3(plantKey.instanceID, TypeIDs::gmdl, 0x40666203);
							ResourceKey PlantSpriteTop(plantKey.instanceID, TypeIDs::raster, 0x40662800);
							ResourceKey PlantSpriteSide(plantKey.instanceID, TypeIDs::raster, 0x40662801);

							PlantItemProperty->SetProperty(0x00F9EFBB, &Property().SetValueKey(PlantModelLOD0));
							PlantItemProperty->SetProperty(0x00F9EFBC, &Property().SetValueKey(PlantModelLOD1));
							PlantItemProperty->SetProperty(0x00F9EFBD, &Property().SetValueKey(PlantModelLOD2));
							PlantItemProperty->SetProperty(0x00F9EFBE, &Property().SetValueKey(PlantModelLOD3));
							PlantItemProperty->SetProperty(0xD124FCE7, &Property().SetValueKey(PlantSpriteTop));
							PlantItemProperty->SetProperty(0xD124FCE4, &Property().SetValueKey(PlantSpriteSide));
							PlantItemProperty->SetProperty(0x04057881, &Property().SetValueFloat(FloraImposterScaleTemp));

							plantItem->mFloraImposterScale = FloraImposterScaleTemp;
						}
						//the code never passes it, so it's commented out
						/*else
						{
							ConsolePrintF("Is baked! :D");
							float mFloraImposterScale;
							Property::GetFloat(PlantItemProperty.get(), 0x04057881, mFloraImposterScale);
							plantItem->mFloraImposterScale = mFloraImposterScale;
						}*/

						//finally, to show the plant in the game
						plantItem->mAlphaModel = plantKey.instanceID;
						plantItem->mModelIDLOD0 = plantKey.instanceID;
						plantItem->mModelIDLOD1 = plantKey.instanceID;
						plantItem->mModelIDLOD2 = plantKey.instanceID;
						plantItem->mModelIDLOD3 = plantKey.instanceID;
						plantItem->mBaseRadius = mBaseRadius;
						plantItem->mCanopyRadius = mCanopyRadius;
						plantItem->mHeight = mHeight;
					}
				}
			}
		}
		//replace the missing in the game files plant to another
		else
		{
			ResourceKey newPlant(0, TypeIDs::flr, GroupIDs::FloraModels);
			cPlanetRecordPtr planetRecord = GetActivePlanetRecord();
			//this functions works outside of planets, so i decided to add this check
			if (planetRecord != nullptr && GetCurrentContext() == SpaceContext::Planet)
			{
				if (planetRecord->mPlantSpecies.mpBegin != planetRecord->mPlantSpecies.mpEnd)
				{
					int OriginalSize = planetRecord->mPlantSpecies.size();
					eastl::vector<ResourceKey> smallPlants;
					eastl::vector<ResourceKey> mediumPlants;
					eastl::vector<ResourceKey> largePlants;
					cSpeciesProfile* profile = nullptr;
					for (const ResourceKey& plant : planetRecord->mPlantSpecies)	//pass missing plants
					{
						if (ResourceManager.GetResource(plant))
						{
							profile = SpeciesManager.GetSpeciesProfile(plant);
							if (profile != nullptr)
							{
								if (profile->mModelType == kPlantSmall)
									smallPlants.push_back(plant);
								else if (profile->mModelType == kPlantMedium)
									mediumPlants.push_back(plant);
								else if (profile->mModelType == kPlantLarge)
									largePlants.push_back(plant);
							}
						}
					}
					if (smallPlants.size() + mediumPlants.size() + largePlants.size() != OriginalSize)	//replace missing plants to another
					{
						floraRandom->CreateFloraList();
						int tScore = 3;
						bool gameMode = false;
						if (GameModeManager.GetActiveModeID() != GameModeIDs::kScenarioMode)	//check if gameMode is game Stage
						{
							tScore = TerraformingManager.GetTScore(planetRecord.get());
							gameMode = true;
						}
						do
						{
							newPlant.instanceID = floraRandom->GetRandomFloraName(smallPlants.size() != tScore, mediumPlants.size() != tScore, largePlants.size() != tScore, gameMode);
							profile = SpeciesManager.GetSpeciesProfile(newPlant);
							if (profile != nullptr)
							{
								if (gameMode)
								{
									//to avoid duplicates in game stages, otherwise it will break planet ecosystem
									if (!floraRandom->ValidPlant(newPlant, smallPlants)) continue;
									else if (!floraRandom->ValidPlant(newPlant, mediumPlants)) continue;
									else if (!floraRandom->ValidPlant(newPlant, largePlants)) continue;
								}
								if (profile->mModelType == kPlantSmall)
									smallPlants.push_back(newPlant);
								else if (profile->mModelType == kPlantMedium)
									mediumPlants.push_back(newPlant);
								else if (profile->mModelType == kPlantLarge)
									largePlants.push_back(newPlant);
							}
						} while (smallPlants.size() + mediumPlants.size() + largePlants.size() != OriginalSize);
						floraRandom->ClearFloraList();
					}
					profile = nullptr;
					planetRecord->mPlantSpecies.clear();
					planetRecord->mPlantSpecies.insert(planetRecord->mPlantSpecies.mpBegin, smallPlants.mpBegin, smallPlants.mpEnd);
					planetRecord->mPlantSpecies.insert(planetRecord->mPlantSpecies.mpBegin, mediumPlants.mpBegin, mediumPlants.mpEnd);
					planetRecord->mPlantSpecies.insert(planetRecord->mPlantSpecies.mpBegin, largePlants.mpBegin, largePlants.mpEnd);
				}
			}
			newPlant.instanceID = id("FE_bumpy1_Sp");	//to avoid game crashing
			plantItem = original_function(this, newPlant);
		}
		return plantItem;
	}
};


void FEReconBakeManager::AttachDetours()
{
	//Baking system
	BakeManager_BakeSprites::attach(Address(ModAPI::ChooseAddress(0x430890, 0x431340)));
	BakeManager_SpriteTextureMIPs::attach(Address(ModAPI::ChooseAddress(0x4306c0, 0x431170)));
	cPlantSpeciesManager_CreatePlantItem::attach(GetAddress(Simulator::cPlantSpeciesManager, CreatePlantItem));
}
