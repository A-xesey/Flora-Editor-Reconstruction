#pragma once
#include <Spore\BasicIncludes.h>
#include <Spore\Sporepedia\cSPUILargeAssetView.h>
#include <Spore\Sporepedia\AssetViewManager.h>
#include "ModelTypeChanged.h"

#define FEReconEditorsPtr intrusive_ptr<FEReconEditors>

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
			if (Editor.mpMovingPart != nullptr && Editor.mpMovingPart->mBooleanAttributes[kEditorRigblockModelUseSkin])
				Editor.mpSelectedPart = Editor.mpMovingPart;

			if (Editor.mpSelectedPart != nullptr && mouseButton == MouseButton::kMouseButtonLeft)
				Editor.mpSelectedPart->mUIState.field_1 = true;
			//for (EditorRigblockPtr part : Editor.GetEditorModel()->mRigblocks)
			//{
			//	if (part->mBooleanAttributes[kEditorRigblockModelUseSkin])
			//	{
			//		//we click on limb and we select him
			//		if (Editor.mpMovingPart == part)// && mouseState.IsAltDown == 0)
			//			Editor.mpSelectedPart = part;
			//		//click again to deselect the limb
			//		if (Editor.mpSelectedPart != nullptr && mouseButton == MouseButton::kMouseButtonLeft)
			//			Editor.mpSelectedPart->mUIState.field_1 = true;
			//		//tried to fix deselecting...
			//		/*if (Editor.mPreviousSelectedBlock != nullptr && Editor.mPreviousSelectedBlock->mUIState.field_1 == false)
			//			Editor.mPreviousSelectedBlock->mUIState.field_1 = true;*/
			//	}
			//}
		}
		return original_function(this, mouseButton, mouseX, mouseY, mouseState);
	}
};

//member_detour(Editor_OnKeyDown, cEditor, bool(int, KeyModifiers)) {
//	bool detoured(int virtualKey, KeyModifiers modifiers) {
//		int key = modifiers.value & (kModifierAltDown | kModifierCtrlDown | kModifierShiftDown);
//		if (((key == kModifierAltDown) && (this->mbShowBoneLengthHandles != false)) && ((this->mpActivePart != nullptr
//			|| ((this->mpActiveHandle != nullptr && (this->mpActiveHandle->mpRigblock != nullptr))))))
//		{
//			EditorRigblockPtr activePart = this->mpActiveHandle->mpRigblock;
//			if ((activePart->mBooleanAttributes[0xb]) &&
//				(uVar34 = *(uint*)&pcVar31->mFlags >> 10,
//					param_2 = CONCAT13((char)uVar34, (int3)param_2) & 0x1ffffff, (uVar34 & 1) != 0)) {
//				cSPEditorBlock::ShowAllHandles(pcVar31);
//				return false;
//			}
//		}
//		return original_function(this, virtualKey, modifiers);
//	}
//};

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
								for (auto verbTray : this->mVerbIcons->mVerbTrays) {
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
	//LocalStringSetText_detour::attach(GetAddress(LocalizedString, SetText));
	Editor_ReturnEditorIDByModel::attach(Address(ModAPI::ChooseAddress(0x432f00, 0x4333e0)));
	Editor_Update::attach(GetAddress(cEditor, Update));
	Editor_OnMouseUp::attach(GetAddress(cEditor, OnMouseUp));
	//Editor_OnKeyDown::attach(GetAddress(cEditor, OnKeyDown));

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
