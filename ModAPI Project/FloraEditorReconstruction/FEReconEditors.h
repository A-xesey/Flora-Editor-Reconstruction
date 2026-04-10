#pragma once
#include <Spore\BasicIncludes.h>
#include "ModelTypeChanged.h"
#include "HasGAProp.h"
#include "FEReconEditorsFruits.h"
#include "FEReconEditorsHandles.h"
#include "FEReconEditorsLimbs.h"
#include "FEReconEditorsHotKeys.h"

extern bool showLog;

using namespace App;
using namespace Editors;

uint32_t EditorPreviousModelType = 0;
Mode EditorPreviousMode = Mode::PaintMode;
ULONG64 EditorTotalTime = 0;
eastl::string16 editorTagField = u"";

ModelTypeChangedPtr modelTypeChanged;

class FEReconEditors
{
public:

	static void Initialize() {
		modelTypeChanged = new ModelTypeChanged();
		MessageManager.AddListener(modelTypeChanged.get(), id("Editors_ModelWasChanged"));
		FEReconEditorsFruits::Initialize();
		FEReconEditorsHandles::Initialize();
		FEReconEditorsLimbs::Initialize();
		FEReconEditorsHotKeys::Initialize();
	}

	static void Dispose() {
		MessageManager.RemoveListener(modelTypeChanged.get(), id("Editors_ModelWasChanged"));
		modelTypeChanged = nullptr;
		FEReconEditorsFruits::Dispose();
		FEReconEditorsHandles::Dispose();
		FEReconEditorsLimbs::Dispose();
		FEReconEditorsHotKeys::Dispose();
	}

	static void AttachDetours();
};

//To make work editor configs for each plant type
static_detour(Editor_ReturnEditorIDByModel, uint32_t(ModelTypes))
{
	uint32_t detoured(ModelTypes modelType)
	{
		switch (modelType)
		{
		case kPlantSmall: return id("FloraEditorSmall");
		case kPlantMedium: return id("FloraEditorMedium");
		case kPlantLarge: return id("FloraEditorLarge");
		default: return original_function(modelType);
		}
	}
};

member_detour(Editor_Update, cEditor, void(float, float)) {
	void detoured(float delta1, float delta2) {
		original_function(this, delta1, delta2);
		if (Editor.mSaveExtension == TypeIDs::flr) {
			auto manipulatorID_fixAnim = eastl::find(Editor.mEnabledManipulators.begin(), Editor.mEnabledManipulators.end(), id("FloraEditorDisableAnimation"));

			// fixes freeze after paint mode
			if (manipulatorID_fixAnim != Editor.mEnabledManipulators.end() && Editor.mMode != EditorPreviousMode)
			{
				if (showLog) ConsolePrintF("Editor_Update: EditorPreviousMode was changed");
				if (Editor.mMode != Mode::PaintMode && Editor.mMode != Mode::PlayMode)
					Editor.field_385 = false;	//mAnimatingCreatureActive
				EditorPreviousMode = Editor.mMode;
			}
			if (Editor.IsMode(Mode::BuildMode))
			{
				//adds headers for each plant types
				if ((EditorPreviousModelType != Editor.mpEditorModel->mModelType || EditorTotalTime != Editor.mnTotalTime)
					&& (Editor.mpEditorModel->mModelType == kPlantLarge ||
						Editor.mpEditorModel->mModelType == kPlantMedium ||
						Editor.mpEditorModel->mModelType == kPlantSmall))
				{
					if (showLog) ConsolePrintF("Editor_Update: model type was changed");
					EditorPreviousModelType = Editor.mpEditorModel->mModelType;
					EditorTotalTime = Editor.mnTotalTime;
					MessageManager.MessageSend(id("Editors_ModelWasChanged"), 0);
				}

				IWindowPtr tagsWin = Editor.mpEditorNamePanel->mpLayout->FindWindowByID(0x5415e48);
				if (tagsWin != nullptr && tagsWin->GetCaption() != editorTagField) {
					if (showLog) ConsolePrintF("Editor_Update: tagsWin exists, editorTagField was changed and ChangeBounds was executed");
					editorTagField = tagsWin->GetCaption();
					ChangeBounds(Editor.mpEditorModel->mModelType);
				}
				tagsWin = nullptr;

				//rename editor to add creature abilities verbtrays support
				if (Editor.mpEditorSkin != nullptr && Editor.mpEditorSkin->GetMesh(1)->mpCreatureData != nullptr
					&& Editor.mVerbIconTray != nullptr)
				{
					/*if (Editor.mEditorName == id("FloraEditorSetup") || Editor.mEditorName == id("FloraEditorSmall")
						|| Editor.mEditorName == id("FloraEditorMedium") || Editor.mEditorName == id("FloraEditorLarge")
						|| Editor.mEditorName == id("FloraEditorSmallUFO") || Editor.mEditorName == id("FloraEditorMediumUFO")
						|| Editor.mEditorName == id("FloraEditorLargeUFO"))*/
					if (Editor.mEditorName != id("CreatureEditorSmall")) {
						if (showLog) ConsolePrintF("Editor_Update: editor names was changed");
						Editor.mEditorName = id("CreatureEditorSmall");
					}
				}
			}
		}
	}
};

static_detour(cSPEditorModelValidity_TestBounds, bool(cEditorResource*, /*bitset<128>*/int*))
{
	bool detoured(cEditorResource * pEditorResource, /*bitset<128>*/int* pFlags)
	{
		if (pEditorResource != nullptr &&
			(pEditorResource->mProperties.mModelType == kPlantLarge
			|| pEditorResource->mProperties.mModelType == kPlantMedium
			|| pEditorResource->mProperties.mModelType == kPlantSmall)
			&& HasGAProp() && Editor.mSaveExtension == TypeIDs::flr)
		{
			if (showLog) ConsolePrintF("==== cSPEditorModelValidity_TestBounds ====");
			PropertyListPtr pEditorPropList;
			PropManager.GetPropertyList(id("FloraEditorLarge"), 0x40600100, pEditorPropList);
			if (pEditorPropList == nullptr)
			{
				if (showLog) ConsolePrintF("pEditorPropList is null");
				if (pFlags != nullptr) pFlags[kValidityOutOfBounds] = true;
				return false;
			}

			float modelBoundSize;
			float modelMaxHeight;
			float modelMinHeight;
			Property::GetFloat(pEditorPropList.get(), 0x700db77d, modelBoundSize);
			Property::GetFloat(pEditorPropList.get(), 0x2704959d, modelMaxHeight);
			Property::GetFloat(pEditorPropList.get(), 0x44c7f29f, modelMinHeight);

			for (const cEditorResourceBlock& block : pEditorResource->mBlocks) {
				PropertyListPtr pBlockPropList = BakeManager.GetBlockPropertyList(block.instanceID, block.groupID);
				if (pBlockPropList == nullptr) {
					if (showLog) ConsolePrintF("pBlockPropList is null");
					if (pFlags != nullptr) /*pFlags[kValidityOutOfBounds] = true;*/
						*pFlags = *pFlags | 0x100;
					return false;
				}
				else {
					bool modelAllowedOut;
					Property::GetBool(pBlockPropList.get(), 0x538a895, modelAllowedOut);
					if (modelAllowedOut == false) {
						if ((modelMaxHeight < block.position.z) ||
							(block.position.z < modelMinHeight)) {
							if (modelMaxHeight < block.position.z)
								if (showLog) ConsolePrintF("block height is higher than max height");
								if (block.position.z < modelMinHeight)
									if (showLog) ConsolePrintF("block height is lower than min height");
									if (pFlags != nullptr) /*pFlags[kValidityOutOfBounds] = true;*/
										*pFlags = *pFlags | 0x100;
							return false;
						}
						float blockRadius = sqrt(block.position.x * block.position.x + block.position.y * block.position.y);
						if (modelBoundSize / 2.0f < blockRadius) {
							if (showLog) ConsolePrintF("blockRadius bigger than modelBoundSize/2");
							if (pFlags != nullptr) /*pFlags[kValidityOutOfBounds] = true;*/
								*pFlags = *pFlags | 0x100;
							return false;
						}
					}
				}
			}
			if (showLog) ConsolePrintF("test was passed");
			if (pFlags != nullptr) /*pFlags[kValidityOutOfBounds] = false;*/
				*pFlags = *pFlags & 0xfffffeff;
			return true;
		}
		return original_function(pEditorResource, pFlags);
	}
};

void FEReconEditors::AttachDetours()
{
	//Editor
	Editor_ReturnEditorIDByModel::attach(Address(ModAPI::ChooseAddress(0x432f00, 0x4333e0)));
	Editor_Update::attach(GetAddress(cEditor, Update));
	cSPEditorModelValidity_TestBounds::attach(Address(ModAPI::ChooseAddress(0x4e9280, 0x4ef1b0)));

	FEReconEditorsFruits::AttachDetours();
	FEReconEditorsHandles::AttachDetours();
	FEReconEditorsLimbs::AttachDetours();
	FEReconEditorsHotKeys::AttachDetours();
}
