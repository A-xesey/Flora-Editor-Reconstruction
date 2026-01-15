#pragma once
#include <Spore\BasicIncludes.h>
#include <Spore\Sporepedia\cSPUILargeAssetView.h>

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

member_detour(Editor_Update, cEditor, void(float, float)) {
	void detoured(float delta1, float delta2) {

		original_function(this, delta1, delta2);
		auto manipulatorID_fixAnim = eastl::find(Editor.mEnabledManipulators.begin(), Editor.mEnabledManipulators.end(), id("FloraEditorDisableAnimation"));
		if (Editor.IsMode(Mode::BuildMode))
		{
			/// fix freeze after paint mode
			if (manipulatorID_fixAnim != Editor.mEnabledManipulators.end())
				Editor.field_385 = false;

			//rename editor to add creature abilities verbtrays support
			if (Editor.mpEditorSkin != nullptr && Editor.mpEditorSkin->GetMesh(1)->mpCreatureData != nullptr && Editor.mVerbIconTray != 0)
			{
				if (Editor.mEditorName == id("FloraEditorSetup") || Editor.mEditorName == id("FloraEditorSetupSmall")
					|| Editor.mEditorName == id("FloraEditorSetupMedium") || Editor.mEditorName == id("FloraEditorSetupLarge"))
					Editor.mEditorName = id("CreatureEditorSmall");
			}
		}
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
					//we click on limb and we select him
					if (Editor.mpMovingPart == part)// && mouseState.IsAltDown == 0)
						Editor.mpSelectedPart = part;

					//click again to deselect the limb
					if (Editor.mpSelectedPart != nullptr && mouseButton == MouseButton::kMouseButtonLeft)
						Editor.mpSelectedPart->mUIState.field_1 = true;

					//tried to fix deselecting...
					/*if (Editor.mPreviousSelectedBlock != nullptr && Editor.mPreviousSelectedBlock->mUIState.field_1 == false)
						Editor.mPreviousSelectedBlock->mUIState.field_1 = true;*/
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
							if (capabality.mPropertyID == kModelCapabilityFruit)
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

//add creature abilities verbtrays support in sporepedia large view
member_detour(cSPUILargeAssetView_LoadLayout, Sporepedia::cSPUILargeAssetView, void())
{
	void detoured()
	{
		original_function(this);

		if (this->field_AB)
		{
			if (this->mWinStatsContainer != nullptr && this->mAssetData != nullptr)
			{
				if (this->mVerbIcons != 0)
				{
					ResourceKey key = this->mAssetData->GetKey();
					if (key.typeID == TypeIDs::flr)
					{
						Editors::cCreatureDataResource* creatureData;
						bool isload = Editor.LoadCreatureData(&key, &creatureData);
						if (isload && creatureData != nullptr)
						{
							float unk = this->mAssetData->func3Ch();
							Editor.ComputeCreatureVerbIcons(creatureData, this->mVerbIcons, -1, unk);
						}
						creatureData = nullptr;
					}
				}
			}
		}
	}
};

///Fruit capability
//add fruit capability to show it as verbtray
static_detour(GetPropIDForAbility, uint32_t(uint32_t, int))
{
	uint32_t detoured(uint32_t modelCapability, int lvl)
	{
		if (modelCapability == kModelCapabilityFruit)
			return id("fruit");
		/*if (modelCapability == id("ModelCapabilityTrans"))
		{
			switch (lvl) {
			case 1:
				return id("transgender1");
			case 2:
				return id("transgender2");
			case 3:
				return id("transgender3");
			case 4:
				return id("transgender4");
			case 5:
				return id("transgender5");
			default:
				return id("transgender");
			}
		}*/
		return original_function(modelCapability, lvl);
	}
};

//get fruits count
static_detour(GetLevelForAbility, float(Simulator::cSpeciesProfile*, uint32_t))
{
	float detoured(Simulator::cSpeciesProfile * profile, uint32_t verbIconCategory)
	{
		if (verbIconCategory == id("fruit"))
			return static_cast<float>(profile->mFruitIndexes.size());
		/*if (verbIconCategory == id("transgender"))
			for (auto ability : profile->mPassiveAbilities)
			{
				App::ConsolePrintF("meow :3");
				if (ability->mType == id("transgender"))
					return static_cast<float>(ability->mSpeedGear);
			}*/
		return original_function(profile, verbIconCategory);
	}
};

//static_detour(BlockGetAbilities, void(eastl::vector<eastl::intrusive_ptr<int>>*, ResourceKey*, uint32_t))
//{
//	void detoured(eastl::vector<eastl::intrusive_ptr<int>>*verbIcons, ResourceKey * part, uint32_t modelMainAbilityList)
//	{
//		original_function(verbIcons, part, modelMainAbilityList);
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

//static_detour(SnapBlockToSocket, void(EditorRigblock*, eastl::vector<EditorRigblockPtr>*, EditorRigblock*))
//{
//	void detoured(EditorRigblock* partWithSocket, eastl::vector<EditorRigblockPtr>* parts, EditorRigblock * partToSnap)
//	{
//		original_function(partWithSocket, parts, partToSnap);
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

	//VerbIcons
	SP_EditorUitls_ComputeVerbIcons::attach(Address(ModAPI::ChooseAddress(0x4df6b0, 0x4e5480)));
	cSPUILargeAssetView_LoadLayout::attach(Address(ModAPI::ChooseAddress(0x6631c0, 0x66dac0)));

	//Capability
	GetPropIDForAbility::attach(Address(ModAPI::ChooseAddress(0x457e80, 0x459840)));
	GetLevelForAbility::attach(Address(ModAPI::ChooseAddress(0x5db9b0, 0x5e4de0)));

	//SnapBlockToSocket::attach(Address(0x4a2eb0));
	// 
	//BlockGetAbilities::attach(Address(ModAPI::ChooseAddress(0x598360, 0x59fc40)));
}
