#pragma once
#include <Spore\BasicIncludes.h>

#define FEReconSporepediaPtr intrusive_ptr<FEReconSporepedia>

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
		if (this->mIsViewableLarge)
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

void FEReconSporepedia::AttachDetours()
{
	//Sporepedia/Pollinator
	Sporepedia_IsEditable::attach(GetAddress(cSPAssetDataOTDB, IsEditable));
	ObjectTemplateDB_GetSaveAreaID::attach(Address(ModAPI::ChooseAddress(0x0055bfa0, 0x00563340)));
}
