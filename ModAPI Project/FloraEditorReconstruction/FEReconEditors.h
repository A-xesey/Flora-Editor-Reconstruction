#pragma once
#include <Spore\BasicIncludes.h>
#include "ModelTypeChanged.h"

using namespace App;
using namespace Editors;

uint32_t EditorPreviousModelType = 0;
Mode EditorPreviousMode = Mode::PaintMode;
ULONG64 EditorTotalTime = 0;

ModelTypeChangedPtr modelTypeChanged;

class FEReconEditors
{
public:

	static void Initialize() {
		//adds fruit counter
		PropertyListPtr VerbTrayCollectionProp;
		PropertyListPtr VerbTrayCollectionModAPIProp;
		PropManager.GetPropertyList(id("Ferecon_VerbTrayCollection"), id("verbtrays"), VerbTrayCollectionProp);
		PropManager.GetPropertyList(id("Ferecon_VerbTrayCollectionModAPI"), id("verbtrays"), VerbTrayCollectionModAPIProp);
		if (VerbTrayCollectionProp != nullptr && VerbTrayCollectionModAPIProp != nullptr) {
			VerbTrayCollectionProp->Clear();
			VerbTrayCollectionProp->CopyAllPropertiesFrom(VerbTrayCollectionModAPIProp.get());
		}
		VerbTrayCollectionProp = nullptr;
		VerbTrayCollectionModAPIProp = nullptr;
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
		if (modelType == kPlantLarge) return id("FloraEditorLarge");
		else if (modelType == kPlantMedium) return id("FloraEditorMedium");
		else if (modelType == kPlantSmall) return id("FloraEditorSmall");
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
			//adds headers for each plant types
			if (EditorPreviousModelType != Editor.mpEditorModel->mModelType || EditorTotalTime != Editor.mnTotalTime
				&& (Editor.mpEditorModel->mModelType == kPlantLarge
				|| Editor.mpEditorModel->mModelType == kPlantMedium
				|| Editor.mpEditorModel->mModelType == kPlantSmall))
			{
				EditorPreviousModelType = Editor.mpEditorModel->mModelType;
				EditorTotalTime = Editor.mnTotalTime;
				MessageManager.MessageSend(id("Editors_ModelWasChanged"), 0);
			}

			//rename editor to add creature abilities verbtrays support
			if (Editor.mpEditorSkin != nullptr && Editor.mpEditorSkin->GetMesh(1)->mpCreatureData != nullptr
				&& Editor.mVerbIconTray != nullptr)
			{
				/*if (Editor.mEditorName == id("FloraEditorSetup") || Editor.mEditorName == id("FloraEditorSmall")
					|| Editor.mEditorName == id("FloraEditorMedium") || Editor.mEditorName == id("FloraEditorLarge")
					|| Editor.mEditorName == id("FloraEditorSmallUFO") || Editor.mEditorName == id("FloraEditorMediumUFO")
					|| Editor.mEditorName == id("FloraEditorLargeUFO"))*/
				if (Editor.mSaveExtension == TypeIDs::flr)
					Editor.mEditorName = id("CreatureEditorSmall");
			}
		}
	}
};

enum eVirtualKey {
	VirtualKeyTab = 0x9,
	VirtualKeyEnter = 0xD,
	VirtualKeyShift = 0x10,
	VirtualKeyCtrl = 0x11,
	VirtualKeyAlt = 0x12,
	VirtualKeyCapsLock = 0x14,
	VirtualKeySpace = 0x20,
	VirtualKeyArrowLeft = 0x25,
	VirtualKeyArrowUp = 0x26,
	VirtualKeyArrowDown = 0x28,
	VirtualKeyArrowRight = 0x27,
	VirtualKeyInsert = 0x2D,
	VirtualKeyDelete = 0x2E,
	VirtualKeyZero = 0x30,
	VirtualKeyOne = 0x31,
	VirtualKeyTwo = 0x32,
	VirtualKeyTree = 0x33,
	VirtualKeyFour = 0x34,
	VirtualKeyFive = 0x35,
	VirtualKeySix = 0x36,
	VirtualKeySeven = 0x37,
	VirtualKeyEight = 0x38,
	VirtualKeyNine = 0x39,
	VirtualKeyA = 0x41,
	VirtualKeyB = 0x42,
	VirtualKeyC = 0x43,
	VirtualKeyD = 0x44,
	VirtualKeyE = 0x45,
	VirtualKeyF = 0x46,
	VirtualKeyG = 0x47,
	VirtualKeyH = 0x48,
	VirtualKeyI = 0x49,
	VirtualKeyJ = 0x4A,
	VirtualKeyK = 0x4B,
	VirtualKeyL = 0x4C,
	VirtualKeyN = 0x4E,
	VirtualKeyM = 0x4D,
	VirtualKeyO = 0x4F,
	VirtualKeyP = 0x50,
	VirtualKeyQ = 0x51,
	VirtualKeyR = 0x52,
	VirtualKeyS = 0x53,
	VirtualKeyT = 0x54,
	VirtualKeyU = 0x55,
	VirtualKeyW = 0x57,
	VirtualKeyX = 0x58,
	VirtualKeyY = 0x59,
	VirtualKeyZ = 0x5A,
	VirtualKeyWin = 0x5B,
	VirtualKeyNumZero = 0x60,
	VirtualKeyNumOne = 0x61,
	VirtualKeyNumTwo = 0x62,
	VirtualKeyNumTree = 0X63,
	VirtualKeyNumFour = 0x64,
	VirtualKeyNumFive = 0x65,
	VirtualKeyNumSix = 0x66,
	VirtualKeyNumSeven = 0x67,
	VirtualKeyNumEight = 0x68,
	VirtualKeyNumNine = 0x69,
	VirtualKeyNumMultiply = 0x6A,
	VirtualKeyNumPlus = 0x6B,
	VirtualKeyNumMinus = 0x6D,
	VirtualKeyNumDelete = 0x6E,
	VirtualKeyNumDiv = 0x6F,
	VirtualKeyF1 = 0x70,
	VirtualKeyF2 = 0x71,
	VirtualKeyF3 = 0x72,
	VirtualKeyF4 = 0x73,
	VirtualKeyF5 = 0x74,
	VirtualKeyF6 = 0x75,
	VirtualKeyF7 = 0x76,
	VirtualKeyF8 = 0x77,
	VirtualKeyF9 = 0x78,
	VirtualKeyF10 = 0x79,
	VirtualKeyF11 = 0x7A,
	VirtualKeyF12 = 0x7B,
	VirtualKeyNumLock = 0x90,
	VirtualKeySemicolon = 0xBA,
	VirtualKeyEqual = 0xBB,
	VirtualKeyComma = 0xBC,
	VirtualKeyMinus = 0xBD,
	VirtualKeyDot = 0xBE,
	VirtualKeySlash = 0xBF,
	VirtualKeyTilde = 0xC0,
	VirtualKeyApostrophe = 0xDE
};

bool saveChanges = false;
/// makes model type chooser invisibility by default in plant type editors
member_detour(EditorUI_Load, EditorUI, bool(cEditor*, uint32_t, uint32_t, bool))
{
	bool detoured(cEditor * pEditor, uint32_t instanceID, uint32_t groupID, bool editorModelForceSaveover)
	{
		bool res = original_function(this, pEditor, instanceID, groupID, editorModelForceSaveover);
		if (res && Editor.mSaveExtension == TypeIDs::flr)
		{
			IWindowPtr TypeChooser = Editor.mpEditorUI->mSharedUI.FindWindowByID(0xD0353720);
			if (TypeChooser != nullptr)
				TypeChooser->SetVisible(saveChanges);
		}
		return res;
	}
};

/// adds hotkey to show up/off the model chooser
member_detour(Editor_OnKeyDown, cEditor, bool(int, KeyModifiers))
{
	bool detoured(int virtualKey, KeyModifiers modifiers)
	{
		if (virtualKey == eVirtualKey::VirtualKeyQ && Editor.mSaveExtension == TypeIDs::flr) {
			//if (Editor.mpActivePart != nullptr
			//	&& !Editor.mpActivePart->mBooleanAttributes[kEditorRigblockModelIsVertebra]
			//	&& !Editor.mpActivePart->mBooleanAttributes[kEditorRigblockModelIsPlantRoot])
			//{
			//	EditorRigblockPtr activePart = Editor.mpActivePart;
			//	uint32_t validID = activePart->mModelRigBlockType.instanceID;
			//	if (validID == id("branch") || validID == id("limb")
			//		&& !activePart->mBooleanAttributes[0xb]) {
			//		activePart->mBooleanAttributes[0xb] = !activePart->mBooleanAttributes[0xb];
			//		bool modelHasBallConnector;
			//		bool modelHasSocketConnector;
			//		Property::GetBool(Editor.mpActivePart->mpPropList.get(), 0x4FF31EEC, modelHasBallConnector);
			//		Property::GetBool(Editor.mpActivePart->mpPropList.get(), 0x6FF31F12, modelHasSocketConnector);
			//		Editor.mpActivePart->mpPropList->SetProperty(0x4FF31EEC, &Property().SetValueBool(!modelHasBallConnector));
			//		Editor.mpActivePart->mpPropList->SetProperty(0x6FF31F12, &Property().SetValueBool(!modelHasBallConnector));
			//		//SP::cSPEditorBlock::BuildBlock(EditorRigblock * this, uint32_t instanceID, uint32_t groupID,
			//		// IModelWorld * modelWorld, int physicsWorld, float unk, bool initHandles, bool initModel, bool createEffects)
			//		if (CALL(
			//			Address(ModAPI::ChooseAddress(0x441080, 0x441910)),
			//			bool,
			//			Args(const EditorRigblock*, uint32_t, uint32_t, Graphics::IModelWorld*, int, float, bool, bool, bool),
			//			Args(Editor.mpActivePart.get(), Editor.mpActivePart->mInstanceID, Editor.mpActivePart->mGroupID, Editor.mpActivePart->mpModelWorld.get(), Editor.mpActivePart->field_188, Editor.mpActivePart->field_30, true, true, true)
			//		))
			//		{
			//			Editor.mpActivePart->mpEditorModel = Editor.mpEditorModel;
			//			Editor.mpEditorModel->mChanged = true;
			//		}
			//	}
			//}
			if (modifiers.IsCtrlDown) {
				IWindowPtr TypeChooser = Editor.mpEditorUI->mSharedUI.FindWindowByID(0xD0353720);
				if (TypeChooser != nullptr) {
					TypeChooser->SetVisible(!TypeChooser->IsVisible());
					saveChanges = TypeChooser->IsVisible();
				}
			}
		}
		return original_function(this, virtualKey, modifiers);
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
			/*if (Editor.mPreviousSelectedBlock != nullptr
				&& Editor.mpSelectedPart->mBooleanAttributes[kEditorRigblockModelUseSkin])
				Editor.mPreviousSelectedBlock->mUIState.field_1 = true;*/
		}
		return original_function(this, mouseButton, mouseX, mouseY, mouseState);
	}
};

void SwitchAttributes(EditorRigblock* block, bool value)
{
	if (block != nullptr && block->mLimbType == kLimbTypeRigblock
		&& Editor.IsMode(Mode::BuildMode) && Editor.mSaveExtension == TypeIDs::flr) {
		block->mBooleanAttributes[kEditorRigblockIsSnapped] = value;
		block->mBooleanAttributes[kEditorRigblockModelHasBallConnector] = value;
		block->mBooleanAttributes[kEditorRigblockModelHasSocketConnector] = value;
		block->mBooleanAttributes[0xb] = value;	//kEditorRigblockModelHasSocketAndBallConnector
	}
}

/// resize limbs
member_detour(Editor_OnMouseWheel, cEditor, bool(int, float, float, MouseState)) {
	bool detoured(int wheelDelta, float mouseX, float mouseY, MouseState mouseState) {
		SwitchAttributes(Editor.mpActivePart.get(), false);
		bool res = original_function(this, wheelDelta, mouseX, mouseY, mouseState);
		SwitchAttributes(Editor.mpActivePart.get(), true);
		return res;
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
			uint32_t modelType = Editor.mpEditorModel->mModelType;
			if ((modelType == id("PlantSmall") || modelType == id("PlantMedium")))
				//&& Editor.mEditHistoryIndex > 1)
			{
				int capCount = 0;
				int rigBlockCount = Editor.mpEditorModel->GetRigblocksCount();
				for (const EditorRigblockPtr& part : Editor.mpEditorModel->mRigblocks)
				{
					if (!part->mCapabilities.empty())
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

/// show hint if fruit nodes are not enough
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

/// show message box if fruit nodes are not enough
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
// adds fruit capability to show it as verbtray
static_detour(GetPropIDForAbility, uint32_t(uint32_t, int))
{
	uint32_t detoured(uint32_t modelCapability, int lvl)
	{
		if (modelCapability == kModelCapabilityFruit)
			return id("fruit");

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

void FEReconEditors::AttachDetours()
{
	//Editor
	Editor_ReturnEditorIDByModel::attach(Address(ModAPI::ChooseAddress(0x432f00, 0x4333e0)));
	Editor_Update::attach(GetAddress(cEditor, Update));
	Editor_OnMouseUp::attach(GetAddress(cEditor, OnMouseUp));
	Editor_OnKeyDown::attach(GetAddress(cEditor, OnKeyDown));
	Editor_OnMouseWheel::attach(GetAddress(cEditor, OnMouseWheel));
	EditorUI_Load::attach(GetAddress(EditorUI, Load));

	//Editor hints
	EditorUI_SetMessageHint::attach(Address(ModAPI::ChooseAddress(0x5d6a30, 0x5dfd40)));
	UTFWin_cSPUIMessageBox::attach(GetAddress(UTFWin::cSPUIMessageBox, ShowDialog));

	//VerbIcons
	SP_EditorUitls_ComputeVerbIcons::attach(Address(ModAPI::ChooseAddress(0x4df6b0, 0x4e5480)));

	//Capability
	GetPropIDForAbility::attach(Address(ModAPI::ChooseAddress(0x457e80, 0x459840)));
	GetLevelForAbility::attach(Address(ModAPI::ChooseAddress(0x5db9b0, 0x5e4de0)));
}
