#pragma once
#include <Spore\BasicIncludes.h>
#include "ModelTypeChanged.h"

using namespace App;
using namespace Editors;

uint32_t EditorPreviousModelType = 0;
Mode EditorPreviousMode = Mode::PaintMode;
int EditorTotalTime = 0;

ModelTypeChangedPtr modelTypeChanged;

class FEReconEditors
{
public:

	static void Initialize() {
		modelTypeChanged = new ModelTypeChanged();
		MessageManager.AddListener(modelTypeChanged.get(), id("Editors_ModelWasChanged"));
	}

	static void Dispose() {
		MessageManager.RemoveListener(modelTypeChanged.get(), id("Editors_ModelWasChanged"));
		modelTypeChanged = nullptr;
	}

	static void AttachDetours();
};

//To make work editor configs for each plant type
static_detour(Editor_ReturnEditorIDByModel, uint32_t(ModelTypes))
{
	uint32_t detoured(ModelTypes modelType)
	{
		if (modelType == kPlantLarge) return id("FloraEditorSetupLarge");
		else if (modelType == kPlantMedium) return id("FloraEditorSetupMedium");
		else if (modelType == kPlantSmall) return id("FloraEditorSetupSmall");
		else return original_function(modelType);
	}
};

member_detour(Editor_Update, cEditor, void(float, float)) {
	void detoured(float delta1, float delta2) {
		original_function(this, delta1, delta2);
		auto manipulatorID_fixAnim = eastl::find(Editor.mEnabledManipulators.begin(), Editor.mEnabledManipulators.end(), id("FloraEditorDisableAnimation"));
		
		// fix freeze after paint mode
		if (manipulatorID_fixAnim != Editor.mEnabledManipulators.end() && Editor.mMode != EditorPreviousMode)
		{
			if (Editor.mMode != Mode::PaintMode && Editor.mMode != Mode::PlayMode)
				Editor.field_385 = false;	//mAnimatingCreatureActive
			EditorPreviousMode = Editor.mMode;
		}
		if (Editor.IsMode(Mode::BuildMode))
		{
			//add headers for each plant types
			if (EditorPreviousModelType != Editor.mpEditorModel->mModelType || EditorTotalTime != Editor.field_448 //mnTotalTime
				&& (Editor.mpEditorModel->mModelType == kPlantLarge
				|| Editor.mpEditorModel->mModelType == kPlantMedium
				|| Editor.mpEditorModel->mModelType == kPlantSmall))
			{
				EditorPreviousModelType = Editor.mpEditorModel->mModelType;
				EditorTotalTime = Editor.field_448;
				MessageManager.MessageSend(id("Editors_ModelWasChanged"), 0);
			}

			//rename editor to add creature abilities verbtrays support
			if (Editor.mpEditorSkin != nullptr && Editor.mpEditorSkin->GetMesh(1)->mpCreatureData != nullptr && Editor.mVerbIconTray != nullptr)
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
		//auto manipulatorID_showLimbs = eastl::find(Editor.mEnabledManipulators.begin(), Editor.mEnabledManipulators.end(), id("FloraEditorShowBranchHandles"));
		if (Editor.IsMode(Mode::BuildMode) && Editor.mbShowBoneLengthHandles)//&& manipulatorID_showLimbs != Editor.mEnabledManipulators.end()) {
		{
			//we click on limb and we select him
			if (Editor.mpMovingPart != nullptr && Editor.mpMovingPart->mBooleanAttributes[kEditorRigblockModelUseSkin])
				Editor.mpSelectedPart = Editor.mpMovingPart;
			
			//click again to deselect the limb
			if (Editor.mpSelectedPart != nullptr && mouseButton == MouseButton::kMouseButtonLeft
				&& Editor.mpSelectedPart->mBooleanAttributes[kEditorRigblockModelUseSkin])
				Editor.mpSelectedPart->mUIState.field_1 = true;
		}
		return original_function(this, mouseButton, mouseX, mouseY, mouseState);
	}
};

/// fruit count valid
bool IsEnoughFruits()
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
		if (!IsEnoughFruits())
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
		if (!IsEnoughFruits())
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

///Fruit capability
//add fruit capability to show it as verbtray
static_detour(GetPropIDForAbility, uint32_t(uint32_t, int))
{
	uint32_t detoured(uint32_t modelCapability, int lvl)
	{
		if (modelCapability == kModelCapabilityFruit)
			return id("fruit");

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

void FEReconEditors::AttachDetours()
{
	//Editor
	Editor_ReturnEditorIDByModel::attach(Address(ModAPI::ChooseAddress(0x432f00, 0x4333e0)));
	Editor_Update::attach(GetAddress(cEditor, Update));
	Editor_OnMouseUp::attach(GetAddress(cEditor, OnMouseUp));

	//Editor hints
	EditorUI_SetMessageHint::attach(Address(ModAPI::ChooseAddress(0x005d6a30, 0x005dfd40)));
	UTFWin_cSPUIMessageBox::attach(GetAddress(UTFWin::cSPUIMessageBox, ShowDialog));

	//VerbIcons
	SP_EditorUitls_ComputeVerbIcons::attach(Address(ModAPI::ChooseAddress(0x4df6b0, 0x4e5480)));

	//Capability
	GetPropIDForAbility::attach(Address(ModAPI::ChooseAddress(0x457e80, 0x459840)));
	GetLevelForAbility::attach(Address(ModAPI::ChooseAddress(0x5db9b0, 0x5e4de0)));
}
