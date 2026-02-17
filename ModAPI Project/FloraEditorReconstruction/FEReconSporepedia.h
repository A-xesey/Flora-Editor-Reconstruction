#pragma once
#include <Spore\BasicIncludes.h>
#include <Spore\Sporepedia\cSPUILargeAssetView.h>

using namespace Sporepedia;

class FEReconSporepedia
{
public:

	static void Initialize() {
	}

	static void Dispose() {
	}

	static void AttachDetours();
};


////Sporepedia
///Makes all editable (except locking creations)
virtual_detour(Sporepedia_IsEditable, cSPAssetDataOTDB, IAssetData, bool())
{
	bool detoured()
	{
		if (this->mIsViewableLarge && this->mIsPlayable)
			return this->mIsEditable = true;
		else
			return this->mIsEditable = false;
	}
};

///Delete creations
static_detour(ObjectTemplateDB_GetSaveAreaID, Resource::SaveAreaID(GroupIDs::Names))
{
	Resource::SaveAreaID detoured(GroupIDs::Names groupID)
	{
		if (groupID == GroupIDs::CreatureModels)
			return Resource::SaveAreaID::Creatures;

		else if (groupID == GroupIDs::BuildingModels)
			return Resource::SaveAreaID::Buildings;

		else if (groupID == GroupIDs::VehicleModels)
			return Resource::SaveAreaID::Vehicles;

		else if (groupID == GroupIDs::UFOModels)
			return Resource::SaveAreaID::UFOs;

		else if (groupID == GroupIDs::Adventures)
			return Resource::SaveAreaID::Adventures;

		else if (groupID == GroupIDs::CellModels)
			return Resource::SaveAreaID::Cells;

		else if (groupID == GroupIDs::CityMusic)
			return Resource::SaveAreaID::CityMusic;

		else if (groupID == GroupIDs::FloraModels)
			return Resource::SaveAreaID::Plants;

		else
			return static_cast<Resource::SaveAreaID>(0);
	}
};

member_detour(cSPUILargeAssetView_LoadLayout, Sporepedia::cSPUILargeAssetView, void())
{
	void detoured()
	{
		original_function(this);

		if (this->field_AB)
		{
			if (this->mWinStatsContainer != nullptr && this->mAssetData != nullptr)
			{
				if (this->mVerbIcons != nullptr)
				{
					ResourceKey key = this->mAssetData->GetKey();
					if (key.typeID == TypeIDs::flr)
					{
						Editors::cCreatureDataResource* creatureData;
						bool isLoaded = Editor.LoadCreatureData(&key, &creatureData);
						if (isLoaded && creatureData != nullptr)
						{
							//add creature abilities verbtrays support in sporepedia large view
							float unk = this->mAssetData->func3Ch();
							Editor.ComputeCreatureVerbIcons(creatureData, this->mVerbIcons.get(), -1, unk);

							//add headers for each plant types
							if (this->mVerbIcons->mVerbTrays.size() > 0) {
								for (const cSPEditorVerbIconTrayPtr& verbTray : this->mVerbIcons->mVerbTrays) {
									IWindowPtr floraHeader = verbTray->mLayout->FindWindowByID(0x811C9DC5);
									if (floraHeader != nullptr && verbTray->mWinText != nullptr && verbTray->mRollover != nullptr)
									{
										ResourceKey iconKey{ 0, TypeIDs::png, id("VerbIcons") };
										LocalizedString localHeader;
										//LocalizedString localDesc;
										Simulator::cSpeciesProfile* profile = SpeciesManager.GetSpeciesProfile(creatureData->GetResourceKey());
										App::Property* propHeader = nullptr;
										App::Property* propDesc = nullptr;
										if (profile != nullptr)
										{
											if (profile->mModelType == kPlantLarge)
											{
												localHeader.SetText(id("sporepedia"), 0x0002000F);
												//localDesc.SetText(id("fe_02"), 0x000000A1);
												verbTray->mVerbTrayProperties->GetProperty(id("verbTrayNameLarge"), propHeader);
												verbTray->mVerbTrayProperties->GetProperty(id("verbTrayDescriptionLarge"), propDesc);
												iconKey.instanceID = id("flora_large");
											}
											else if (profile->mModelType == kPlantMedium)
											{
												localHeader.SetText(id("sporepedia"), 0x00020010);
												//localDesc.SetText(id("fe_02"), 0x000000A2);
												verbTray->mVerbTrayProperties->GetProperty(id("verbTrayNameMedium"), propHeader);
												verbTray->mVerbTrayProperties->GetProperty(id("verbTrayDescriptionMedium"), propDesc);
												iconKey.instanceID = id("flora_medium");
											}
											else if (profile->mModelType == kPlantSmall)
											{
												localHeader.SetText(id("sporepedia"), 0x00020011);
												//localDesc.SetText(id("fe_02"), 0x000000A3);
												verbTray->mVerbTrayProperties->GetProperty(id("verbTrayNameSmall"), propHeader);
												verbTray->mVerbTrayProperties->GetProperty(id("verbTrayDescriptionSmall"), propDesc);
												iconKey.instanceID = id("flora_small");
											}
										}
										/*IWindowPtr mWinRolloverText = verbTray->mRollover->mLayout.FindWindowByID(0x0331CC0E);
										IWindowPtr mWinRolloverDesc = verbTray->mRollover->mLayout.FindWindowByID(0x0331CC0F);*/
										if (iconKey.instanceID != 0 && localHeader.GetText() != nullptr
											&& propHeader != nullptr && propDesc != nullptr /*&& localDesc.GetText() != nullptr
											&& mWinRolloverText != nullptr && mWinRolloverDesc != nullptr*/)
										{
											verbTray->mVerbTrayProperties->SetProperty(0x04ACEDF4, propHeader);	//verbTrayName
											verbTray->mVerbTrayProperties->SetProperty(0x04ACEDF9, propDesc);	//verbTrayDescription
											UTFWin::Image::SetBackgroundByKey(floraHeader.get(), iconKey);
											verbTray->mWinText->SetCaption(localHeader.GetText());
											/*mWinRolloverText->SetCaption(localHeader.GetText());
											mWinRolloverDesc->SetCaption(localDesc.GetText());*/
										}
										profile = nullptr;
									}
								}
							}
						}
						creatureData != nullptr;
					}
				}
			}
		}
	}
};

void FEReconSporepedia::AttachDetours()
{
	//Sporepedia/Pollinator
	Sporepedia_IsEditable::attach(GetAddress(cSPAssetDataOTDB, IsEditable));
	ObjectTemplateDB_GetSaveAreaID::attach(Address(ModAPI::ChooseAddress(0x0055bfa0, 0x00563340)));
	cSPUILargeAssetView_LoadLayout::attach(Address(ModAPI::ChooseAddress(0x6631c0, 0x66dac0)));
}
