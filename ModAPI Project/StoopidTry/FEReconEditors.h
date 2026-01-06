#pragma once
#include <Spore\BasicIncludes.h>

#define FEReconEditorsPtr intrusive_ptr<FEReconEditors>

using namespace App;
using namespace Editors;

class FEReconEditors
{
public:

	static void Initialize() {
	}

	static void Dispose() {
	}

	static void AttachDetours();
};

/// fix freeze after paint mode
member_detour(Editor_Update, cEditor, void(float, float)) {
	void detoured(float delta1, float delta2) {
		auto manipulatorID_fixAnim = eastl::find(Editor.mEnabledManipulators.begin(), Editor.mEnabledManipulators.end(), id("FloraEditorDisableAnimation"));
		if (Editor.IsMode(Mode::BuildMode) && manipulatorID_fixAnim != Editor.mEnabledManipulators.end())
			Editor.field_385 = false;
		/*if (Editor.mEditorName == id("FloraEditorSetup") || Editor.mEditorName == id("FloraEditorSetupSmall")
			|| Editor.mEditorName == id("FloraEditorSetupMedium") || Editor.mEditorName == id("FloraEditorSetupLarge"))
			Editor.ComputeCreatureVerbIcons(Editor.mpEditorSkin->GetMesh(1)->mpCreatureData, Editor.mVerbIconTray, Editor.mnDefaultBrainLevel, -1.0f);*/

		original_function(this, delta1, delta2);
	}
};

/// select limbs
member_detour(Editor_OnMouseUp, cEditor, bool(MouseButton, float, float, MouseState)) {
	bool detoured(MouseButton mouseButton, float mouseX, float mouseY, MouseState mouseState) {
		auto manipulatorID_showLimbs = eastl::find(Editor.mEnabledManipulators.begin(), Editor.mEnabledManipulators.end(), id("FloraEditorShowBranchHandles"));
		if (Editor.IsMode(Mode::BuildMode) && manipulatorID_showLimbs != Editor.mEnabledManipulators.end()) {
			for (EditorRigblockPtr part : Editor.GetEditorModel()->mRigblocks)
			{
				if (part->mBooleanAttributes[kEditorRigblockModelUseSkin])
				{
					if (Editor.mpMovingPart == part)// && mouseState.IsAltDown == 0)
						Editor.mpSelectedPart = part;

					if (Editor.mpSelectedPart != nullptr && mouseButton == MouseButton::kMouseButtonLeft)
						Editor.mpSelectedPart->mUIState.field_1 = true;

					if (Editor.mPreviousSelectedBlock != nullptr && Editor.mPreviousSelectedBlock->mUIState.field_1 == false)
						Editor.mPreviousSelectedBlock->mUIState.field_1 == true;
				}
			}
		}
		return original_function(this, mouseButton, mouseX, mouseY, mouseState);
	}
};


/// cap count valid
bool IsEnoughCap()
{
	if (Editor.IsActive())
	{
		PropertyListPtr AppPropList = GetAppProperties();
		bool DisableValidation;
		Property::GetBool(AppPropList.get(), 0x55d7ca1, DisableValidation);

		if (!DisableValidation)
		{
			uint32_t modelType = Editor.GetEditorModel()->mModelType;
			if ((modelType == id("PlantSmall") || modelType == id("PlantMedium")))
				//&& Editor.mEditHistoryIndex > 1)
			{
				int capCount = 0;
				int rigBlockCount = Editor.mpEditorModel->GetRigblocksCount();
				for (EditorRigblockPtr part : Editor.GetEditorModel()->mRigblocks)
				{
					if (part->mCapabilities.mpBegin != part->mCapabilities.mpEnd)
					{
						for (const EditorRigblockCapability& capabality : part->mCapabilities)
						{
							if (capabality.mPropertyID == 0xd00f0ffb)//kModelCapabilityFruit)
								capCount += 1;
						}
					}
				}
				if (rigBlockCount >= 3 && capCount < 3)
					return false;
			}
		}
	}
	return true;
};

member_detour(EditorUI_SetMessageHint, EditorUI, bool(int))
{
	bool detoured(int SPUI_commandID)
	{
		if (!IsEnoughCap())
		{
			if (SPUI_commandID == 0x101 || SPUI_commandID == 0x102) //Save button/command or "Save and Exit" button
			{
				HintManager.ShowHint(id("ferecon-errorinvalidcapcount"));
				return original_function(this, 0);
			}
		}
		return original_function(this, SPUI_commandID);
	}
};

static_detour(UTFWin_cSPUIMessageBox, bool(UTFWin::MessageBoxCallback*, const ResourceKey&))
{
	bool detoured(UTFWin::MessageBoxCallback * pcallback, const ResourceKey & key)
	{
		if (!IsEnoughCap())
		{
			Editor.mpEditorUI->field_C8 = id("editor_NoSaveAndContinue");
			ResourceKey key2 = key;

			if (key.instanceID == id("editor_SaveOrCopyAndContinueMakeNew")
				|| key.instanceID == id("editor_SaveNewAndContinueMakeNew")
				|| key.instanceID == id("Editor_SaveNewAndContinueMakeNew_ForceSaveOver"))
				key2.instanceID = id("FERecon_New");

			else if (key.instanceID == id("editor_SaveOrCopyAndContinueLoad")
				|| key.instanceID == id("editor_SaveNewAndContinueLoad")
				|| key.instanceID == id("Editor_SaveNewAndContinueLoad_ForceSaveOver"))
				key2.instanceID = id("FERecon_Sporepedia");

			else if (key.instanceID == id("editor_SaveOrCopyAndContinueExit")
				|| key.instanceID == id("editor_SaveNewAndContinueExit")
				|| key.instanceID == id("Editor_SaveNewAndContinueExit_ForceSaveOver"))
				key2.instanceID = id("FERecon_Exit");

			return original_function(pcallback, key2);
		}
		return original_function(pcallback, key);
	}
};

//Capabilities
static_detour(SP_EditorUitls_ComputeVerbIcons, bool(uint32_t, uint32_t, uint32_t, ModelTypes, int))
{
	bool detoured(uint32_t instanceID, uint32_t typeID, uint32_t groupID, ModelTypes modelType, int verbTrayCollection)
	{
		if (modelType == kPlantLarge || modelType == kPlantMedium || modelType == kPlantSmall)
			modelType = kCreature;
		return original_function(instanceID, typeID, groupID, modelType, verbTrayCollection);
	}
};


/*static_detour(GetPropIDForAbility, uint32_t(uint32_t, int))
{
	uint32_t detoured(uint32_t modelCapability, int lvl)
	{
		if (modelCapability == kModelCapabilityFruit)
			return id("fruit");
		return original_function(modelCapability, lvl);
	}
};*/


/*static_detour(GetLevelForAbility, float(Simulator::cSpeciesProfile*, uint32_t))
{
	float detoured(Simulator::cSpeciesProfile * profile, uint32_t verbIconCategory)
	{
		if (verbIconCategory == id("fruit"))
			return static_cast<float>(profile->mFruitIndexes.size());
		return original_function(profile, verbIconCategory);
	}
};*/


//uint32_t GetPropIDForAbilityMod(uint32_t modelCapability, int lvl)
//{
//	if (modelCapability == kModelCapabilityFruit)
//		return id("fruit");
//	if (modelCapability == kModelCapabilityStealth)
//	{
//		switch (lvl) {
//		case 1:
//			return id("stealth1");
//		case 2:
//			return id("stealth2");
//		case 3:
//			return id("stealth3");
//		case 4:
//			return id("stealth4");
//		default:
//			return id("stealth5");
//		}
//	}
//	if (modelCapability == id("ModelCapabilityTrans"))
//	{
//		switch (lvl) {
//		case 1:
//			return id("transgender1");
//		case 2:
//			return id("transgender2");
//		case 3:
//			return id("transgender3");
//		case 4:
//			return id("transgender4");
//		default:
//			return id("transgender5");
//		}
//	}
//	return 0;
//};

//static_detour(BlockGetAbilities, void(eastl::vector<eastl::intrusive_ptr<int>>*, ResourceKey*, uint32_t))
//{
//	void detoured(eastl::vector<eastl::intrusive_ptr<int>>*verbIcons, ResourceKey * part, uint32_t modelMainAbilityList)
//	{
//		original_function(verbIcons, part, modelMainAbilityList);
//		if (verbIcons->mpBegin != verbIcons->mpEnd)
//		{
//			for (int i = 0; i < 0x88; i +=4)
//			{
//				//App::ConsolePrintF("0x%x", *(int*)(verbIcons->mpBegin->get() + i));
//				//App::ConsolePrintF("------------------------------------------------------");
//				App::ConsolePrintF("0x%x", *(int*)(verbIcons->mpBegin->get() + i));
//			}
//		}
//		PropertyListPtr propList;
//		PropManager.GetPropertyList(part->instanceID, part->groupID, propList);
//		bool hasAbility = propList->HasProperty(0x51c3e5b4);
//		if (hasAbility)
//		{
//			int level;
//			App::Property::GetInt32(propList.get(), 0x51c3e5b4, level);
//			PropertyListPtr AbilityList;
//			uint32_t abilityID = GetPropIDForAbilityMod(0x51c3e5b4, level);
//			PropManager.GetPropertyList(abilityID, 0xDD91AC58, AbilityList);
//			if (AbilityList != nullptr)
//			{
//				bool hasVerbIconCategory = AbilityList->HasProperty(0x4abbb0d);
//				if (hasVerbIconCategory)
//				{
//					uint32_t VerbIconCategory;
//					App::Property::GetKeyInstanceID(AbilityList.get(), 0x4abbb0d, VerbIconCategory);
//					if (VerbIconCategory != 0)
//					{
//						//for (int x = 0; x < verbIcons->size(); x++)
//						{
//						}
//						//verbIconData->Init(AbilityList.get());
//						/*verbIconData->mVerbIconLevel = static_cast<float>(level);
//						App::ConsolePrintF("mVerbIconLevel %f", verbIconData->mVerbIconLevel);
//						if (verbIconData->mVerbIconLevel < 0)
//							verbIconData->mVerbIconShowLevel = false;*/
//							/*if (vebIcons->mpEnd < vebIcons->mpCapacity)
//							{
//								App::ConsolePrintF("vebIcons->mpEnd");
//								vebIcons->mpEnd += 1;
//								if (vebIcons->mpEnd != nullptr)
//								{
//								}
//							}*/
//							//verbIconData = nullptr;
//					}
//				}
//			}
//			AbilityList = nullptr;
//		}
//		propList = nullptr;
//	}
//};

//static_detour(UnknownDetour2, void(Editors::EditorRigblock*, vector<int>&, bool)) { // Scaling fix
//	void detoured(Editors::EditorRigblock * a, vector<int> &b, bool c) {
//		if (!a->mpParent->mBooleanAttributes[kEditorRigblockModelIsPlantRoot])
//			return original_function(a, b, c);
//	}
//};

void FEReconEditors::AttachDetours()
{
	//Editor
	Editor_Update::attach(GetAddress(cEditor, Update));
	Editor_OnMouseUp::attach(GetAddress(cEditor, OnMouseUp));

	//Editor hints
	EditorUI_SetMessageHint::attach(Address(ModAPI::ChooseAddress(0x005d6a30, 0x005dfd40)));
	UTFWin_cSPUIMessageBox::attach(GetAddress(UTFWin::cSPUIMessageBox, ShowDialog));
	//GetPropIDForAbility::attach(Address(ModAPI::ChooseAddress(0x457e80, 0x459840)));
	//GetLevelForAbility::attach(Address(ModAPI::ChooseAddress(0x5db9b0, 0x5e4de0)));
	//SP_EditorUitls_ComputeVerbIcons::attach(Address(0x4e5480));

	//UnknownDetour2::attach(Address(ModAPI::ChooseAddress(0x49FF50, 0x4A0530)));
	//BlockGetAbilities::attach(Address(ModAPI::ChooseAddress(0x598360, 0x59fc40)));
	//DisplayVehicleCapabilties::attach(Address(0x04e7680));
	/*DisplayVehicleCapabilties
	* DisplayToVerbtray
	*/
}
