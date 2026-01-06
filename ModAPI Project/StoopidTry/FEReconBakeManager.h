#pragma once
#include <Spore\BasicIncludes.h>
#include <Spore\Editors\BakeManager.h>
#include "FEBlackWaterFixMsg.h""
#include "FloraRandom.h"

#define FEReconBakeManagerPtr intrusive_ptr<FEReconBakeManager>

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

///fix 3 sec freeze
//bool SpriteTextureMIPsDirectlyChange()
//{
//	App::PropertyList* propList = App::GetAppProperties();
//	App::Property* prop = 0;
//
//	AppProperties.GetProperty(id("FERecon_DirectlyChange"), prop);
//	bool DirectlyChange;
//	prop->GetBool(propList, id("FERecon_DirectlyChange"), DirectlyChange);
//
//	return DirectlyChange;
//};
//
//int SpriteTextureMIPsProperties(bool Parameters, bool variable)
//{
//	App::PropertyList* propList = App::GetAppProperties();
//	App::Property* prop = 0;
//	int SpriteTextureMIPsProperties;
//
//	if (!Parameters) //textureQuality
//	{
//		if (!variable) //NMap
//		{
//			AppProperties.GetProperty(id("FERecon_textureQualityNormalMap"), prop);
//			prop->GetInt32(propList, id("FERecon_textureQualityNormalMap"), SpriteTextureMIPsProperties);
//		}
//		else //DMap
//		{
//			AppProperties.GetProperty(id("FERecon_textureQualityDiffuseMap"), prop);
//			prop->GetInt32(propList, id("FERecon_textureQualityDiffuseMap"), SpriteTextureMIPsProperties);
//		}
//	}
//	else //textureDXT
//	{
//		if (!variable)  //NMap
//		{
//			AppProperties.GetProperty(id("FERecon_textureDXTNormalMap"), prop);
//			prop->GetInt32(propList, id("FERecon_textureDXTNormalMap"), SpriteTextureMIPsProperties);
//		}
//		else  //DMap
//		{
//			AppProperties.GetProperty(id("FERecon_textureDXTDiffuseMap"), prop);
//			prop->GetInt32(propList, id("FERecon_textureDXTDiffuseMap"), SpriteTextureMIPsProperties);
//		}
//	}
//
//	if (!Parameters && SpriteTextureMIPsProperties != -1 && SpriteTextureMIPsProperties != 4 && SpriteTextureMIPsProperties != 5
//		&& SpriteTextureMIPsProperties != 6)
//	{
//		if (!variable)
//		{
//			App::ConsolePrintF("The textureQualityNormalMap value has been reset to avoid crashing the game.\nYou can only use -1, 4, 5 or 6.");
//			SpriteTextureMIPsProperties = 4;
//		}
//		else
//		{
//			App::ConsolePrintF("The textureQualityDiffuseMap value has been reset to avoid crashing the game.\nYou can only use -1, 4, 5 or 6.");
//			SpriteTextureMIPsProperties = 6;
//		}
//	}
//	else if (Parameters && SpriteTextureMIPsProperties > 90)
//	{
//		App::ConsolePrintF("The textureDXT value has been reset to avoid totally freezing the game.");
//		SpriteTextureMIPsProperties = 50;
//	}
//	return SpriteTextureMIPsProperties;
//};

static_detour(BakeManager_SpriteTextureMIPs, void(int, int, int, int))
{
	void detoured(int param_1, int textureQuality, int textureDXT, int param_4)
	{
		//if (SpriteTextureMIPsDirectlyChange() == true)
		//{
		//	if (textureQuality == 4) //NormalMap
		//	{
		//		textureQuality = SpriteTextureMIPsProperties(0, 0);
		//		textureDXT = SpriteTextureMIPsProperties(1, 0);
		//	}
		//	if (textureQuality == 6) //DiffuseMap
		//	{
		//		textureQuality = SpriteTextureMIPsProperties(0, 1);
		//		textureDXT = SpriteTextureMIPsProperties(1, 1);
		//	}
		//}
		//original_function(param_1, textureQuality, textureDXT, param_4);
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

///Baking
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
				//bool IsBaked = BakeManager.IsBaked(plantKey, false); need to update BakeManager to Release version
				//if (GetCurrentContext() == SpaceContext::Planet && !IsBaked)
				PropertyListPtr PlantItemProperty;
				PropManager.GetPropertyList(plantItem->GetItemID().instanceID, plantItem->GetItemID().groupID, PlantItemProperty);
				if (PlantItemProperty != nullptr)
				{
					BakeManager.BakeModel(plantKey, Editors::BakeParameters::BakeParameters(0, 4, 0x609b763));

					/*PropertyListPtr PlantItemProperty;
					PropManager.GetPropertyList(plantItem->GetItemID().instanceID, plantItem->GetItemID().groupID, PlantItemProperty);*/

					//if (PlantItemProperty != nullptr)
					{
						float mBaseRadius;
						float mCanopyRadius;
						float mHeight;

						Property::GetFloat(PlantItemProperty.get(), 0x0254CF89, mBaseRadius);
						Property::GetFloat(PlantItemProperty.get(), 0x0254CF8F, mCanopyRadius);
						Property::GetFloat(PlantItemProperty.get(), 0x0254CF97, mHeight);

						//if (!IsBaked)
						{
							float PseudoFloraImposterScale = (mBaseRadius + mCanopyRadius + mHeight) / 3;

							ResourceKey SetPlantModelLOD0(plantItem->GetItemID().instanceID, TypeIDs::gmdl, 0x40667100);
							ResourceKey SetPlantModelLOD1(plantItem->GetItemID().instanceID, TypeIDs::gmdl, 0x40666201);
							ResourceKey SetPlantModelLOD2(plantItem->GetItemID().instanceID, TypeIDs::gmdl, 0x40666202);
							ResourceKey SetPlantModelLOD3(plantItem->GetItemID().instanceID, TypeIDs::gmdl, 0x40666203);
							ResourceKey SetPlantSpriteTop(plantItem->GetItemID().instanceID, TypeIDs::raster, 0x40662800);
							ResourceKey SetPlantSpriteSide(plantItem->GetItemID().instanceID, TypeIDs::raster, 0x40662801);

							PlantItemProperty->SetProperty(0x00F9EFBB, &Property().SetValueKey(SetPlantModelLOD0));
							PlantItemProperty->SetProperty(0x00F9EFBC, &Property().SetValueKey(SetPlantModelLOD1));
							PlantItemProperty->SetProperty(0x00F9EFBD, &Property().SetValueKey(SetPlantModelLOD2));
							PlantItemProperty->SetProperty(0x00F9EFBE, &Property().SetValueKey(SetPlantModelLOD3));
							PlantItemProperty->SetProperty(0xD124FCE7, &Property().SetValueKey(SetPlantSpriteTop));
							PlantItemProperty->SetProperty(0xD124FCE4, &Property().SetValueKey(SetPlantSpriteSide));
							PlantItemProperty->SetProperty(0x04057881, &Property().SetValueFloat(PseudoFloraImposterScale));

							plantItem->mFloraImposterScale = PseudoFloraImposterScale;
						}
						/*else
						{
							float mFloraImposterScale;
							Property::GetFloat(PlantItemProperty.get(), 0x04057881, mFloraImposterScale);
							plantItem->mFloraImposterScale = mFloraImposterScale;
						}*/

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
		else
		{
			ResourceKey newPlant(0, TypeIDs::flr, GroupIDs::FloraModels);
			cPlanetRecordPtr planetRecord = GetActivePlanetRecord();
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
							newPlant.instanceID = floraRandom->GetRandomFloraName(smallPlants.size() != tScore, mediumPlants.size() != tScore, largePlants.size() != tScore);
							profile = SpeciesManager.GetSpeciesProfile(newPlant);
							if (profile != nullptr)
							{
								if (gameMode)
								{
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
	BakeManager_BakeSprites::attach(Address(ModAPI::ChooseAddress(0x00430890, 0x00431340)));
	BakeManager_SpriteTextureMIPs::attach(Address(ModAPI::ChooseAddress(0x004306c0, 0x00431170)));
	cPlantSpeciesManager_CreatePlantItem::attach(GetAddress(Simulator::cPlantSpeciesManager, CreatePlantItem));
}
