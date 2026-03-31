#pragma once
#include <Spore\BasicIncludes.h>

using namespace App;
using namespace Editors;


class FEReconEditorsFruits
{
public:

	static void Initialize() {
		//adds fruit counter
		PropertyListPtr VerbTrayCollectionProp;
		PropManager.GetPropertyList(id("Ferecon_VerbTrayCollection"), id("verbtrays"), VerbTrayCollectionProp);
		if (VerbTrayCollectionProp != nullptr) {
			VerbTrayCollectionProp->RemoveProperty(0x04AA3838);
			ResourceKey header = { id("flora_header"), 0, id("EditorHeaderTray_FE") };
			ResourceKey ferecon_fruit = { id("ferecon_fruit"), 0, 0 };
			vector<ResourceKey>* array = new vector<ResourceKey>();
			array->push_back(header);
			array->push_back(ferecon_fruit);
			VerbTrayCollectionProp->SetProperty(0x04AA3838, &Property().SetArrayKey((*array).data(), (*array).size()));
			array = nullptr;
		}
		VerbTrayCollectionProp = nullptr;
	}

	static void Dispose() {
	}

	static void AttachDetours();
};

enum eFruitState {
	kFruitStateValid = 0,
	kFruitStateInvalidCount = 1,
	kFruitStateInvalidPosition = 2,
	kFruitStateInvalidPositionUnderground = 3
};

/// fruit validation check
eFruitState GetFruitState()
{
	if (Editor.IsActive())
	{
		PropertyListPtr AppPropList = GetAppProperties();
		bool DisableValidation;
		Property::GetBool(AppPropList.get(), 0x55d7ca1, DisableValidation);

		if (!DisableValidation)
		{
			uint32_t modelType = Editor.mpEditorModel->mModelType;
			if ((modelType == id("PlantSmall") || modelType == id("PlantMedium")))
				//&& Editor.mEditHistoryIndex > 1)
			{
				int capCount = 0;
				int validCapPos = 0;
				int rigBlockCount = Editor.mpEditorModel->GetRigblocksCount();
				bool checkFruitPos = modelType == id("PlantSmall");
				bool fruitUnderground = false;
				for (const EditorRigblockPtr& part : Editor.mpEditorModel->mRigblocks)
				{
					if (!part->mCapabilities.empty())
					{
						for (const EditorRigblockCapability& capabality : part->mCapabilities)
						{
							if (capabality.mPropertyID == kModelCapabilityFruit)
							{
								capCount += 1;
								if (checkFruitPos && part->mPosition.z < 1.25f)
									validCapPos += 1;
								if (part->mPosition.z < 0.2f)
									fruitUnderground = true;
								break;
							}
						}
					}
				}
				if (rigBlockCount >= 3 && capCount < 3)
					return kFruitStateInvalidCount;
				else if (checkFruitPos && validCapPos < 3)
					return kFruitStateInvalidPosition;
				else if (fruitUnderground)
					return kFruitStateInvalidPositionUnderground;
			}
		}
	}
	return kFruitStateValid;
};

uint32_t GetHintByFruitState(const eFruitState& state)
{
	switch (state)
	{
	case kFruitStateInvalidCount: return id("ferecon-errorinvalidcapcount");
	case kFruitStateInvalidPosition: return id("ferecon-errorinvalidcapposition");
	case kFruitStateInvalidPositionUnderground: return id("ferecon-errorinvalidcappositionunderground");
	default: return 0;
	}
};

/// show hint if fruits state are invalid
member_detour(EditorUI_SetMessageHint, EditorUI, bool(int))
{
	bool detoured(int SPUI_commandID)
	{
		eFruitState state = GetFruitState();
		if (state != kFruitStateValid) {
			if (SPUI_commandID == 0x101 || SPUI_commandID == 0x102) //Save button/command or "Save and Exit" button
			{
				uint32_t hintID = GetHintByFruitState(state);
				if (hintID != 0)
					HintManager.ShowHint(hintID);
				return original_function(this, 0);
			}
		}
		return original_function(this, SPUI_commandID);
	}
};

/// set text if fruits state are invalid
member_detour(cStringDetokenizer_TranslateToken, cStringDetokenizer, bool(const char16_t*, eastl::string16*))
{
	bool detoured(const char16_t* pToken, eastl::string16 * translatedValue) {
		bool res = original_function(this, pToken, translatedValue);
		if (res && Editor.IsActive() && id(pToken) == id("generic0")
			&& Editor.mSaveExtension == TypeIDs::flr) {
			eFruitState state = GetFruitState();
			if (state != kFruitStateValid) {
				LocalizedString fruitLoc;
				uint32_t instanceID = GetHintByFruitState(state);
				if (instanceID != 0) {
					fruitLoc.SetText(id("fe_02"), instanceID);
					if (fruitLoc.GetText() != u"")
						translatedValue->assign(fruitLoc.GetText());
				}
			}
		}
		return res;
	}
};

/// show message box if fruits state are invalid
static_detour(UTFWin_cSPUIMessageBox, bool(UTFWin::MessageBoxCallback*, const ResourceKey&))
{
	bool detoured(UTFWin::MessageBoxCallback * pcallback, const ResourceKey & key)
	{
		eFruitState state = GetFruitState();
		if (state != kFruitStateValid)
		{
			Editor.mpEditorUI->field_C8 = id("editor_NoSaveAndContinue");
			ResourceKey key2 = key;

			if (key.instanceID == id("editor_SaveOrCopyAndContinueMakeNew")
				|| key.instanceID == id("editor_SaveNewAndContinueMakeNew")
				|| key.instanceID == id("Editor_SaveNewAndContinueMakeNew_ForceSaveOver"))
				key2.instanceID = id("Editor_NoSaveAndContinueMakeNew");

			else if (key.instanceID == id("editor_SaveOrCopyAndContinueLoad")
				|| key.instanceID == id("editor_SaveNewAndContinueLoad")
				|| key.instanceID == id("Editor_SaveNewAndContinueLoad_ForceSaveOver"))
				key2.instanceID = id("Editor_NoSaveAndContinueLoad");

			else if (key.instanceID == id("editor_SaveOrCopyAndContinueExit")
				|| key.instanceID == id("editor_SaveNewAndContinueExit")
				|| key.instanceID == id("Editor_SaveNewAndContinueExit_ForceSaveOver"))
				key2.instanceID = id("Editor_NoSaveAndContinueExit");

			return original_function(pcallback, key2);
		}
		return original_function(pcallback, key);
	}
};

///Fruit counter
static_detour(SP_EditorUitls_ComputeVerbIcons, bool(uint32_t, uint32_t, uint32_t, ModelTypes, int))
{
	bool detoured(uint32_t instanceID, uint32_t typeID, uint32_t groupID, ModelTypes modelType, int verbTrayCollection)
	{
		//change type to add creature abilities verbtrays support in sporepedia card
		if (modelType == kPlantLarge || modelType == kPlantMedium || modelType == kPlantSmall)
			modelType = kCreature;

		return original_function(instanceID, typeID, groupID, modelType, verbTrayCollection);
	}
};

///Fruit capability
// adds fruit capability to show it as verbtray
static_detour(GetPropIDForAbility, uint32_t(uint32_t, int))
{
	uint32_t detoured(uint32_t modelCapability, int lvl)
	{
		if (modelCapability == kModelCapabilityFruit) return id("fruit");
		return original_function(modelCapability, lvl);
	}
};

// get fruits count
static_detour(GetLevelForAbility, float(Simulator::cSpeciesProfile*, uint32_t))
{
	float detoured(Simulator::cSpeciesProfile * profile, uint32_t verbIconCategory)
	{
		if (verbIconCategory == id("fruit"))
			return static_cast<float>(profile->mFruitIndexes.size());

		return original_function(profile, verbIconCategory);
	}
};

void FEReconEditorsFruits::AttachDetours()
{

	//Editor hints
	EditorUI_SetMessageHint::attach(Address(ModAPI::ChooseAddress(0x5d6a30, 0x5dfd40)));
	UTFWin_cSPUIMessageBox::attach(GetAddress(UTFWin::cSPUIMessageBox, ShowDialog));
	cStringDetokenizer_TranslateToken::attach(Address(ModAPI::ChooseAddress(0x6a56a0, 0x6b60f0)));

	//VerbIcons
	SP_EditorUitls_ComputeVerbIcons::attach(Address(ModAPI::ChooseAddress(0x4df6b0, 0x4e5480)));

	//Capability
	GetPropIDForAbility::attach(Address(ModAPI::ChooseAddress(0x457e80, 0x459840)));
	GetLevelForAbility::attach(Address(ModAPI::ChooseAddress(0x5db9b0, 0x5e4de0)));
}