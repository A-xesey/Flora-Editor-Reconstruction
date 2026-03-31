#pragma once
#include <Spore\BasicIncludes.h>

using namespace App;
using namespace Editors;

class FEReconEditorsLimbs
{
public:

	static void Initialize() {}

	static void Dispose() {}

	static void AttachDetours();
};

/// select limbs
member_detour(Editor_OnMouseUp, cEditor, bool(MouseButton, float, float, MouseState)) {
	bool detoured(MouseButton mouseButton, float mouseX, float mouseY, MouseState mouseState) {
		//auto manipulatorID_showLimbs = eastl::find(Editor.mEnabledManipulators.begin(), Editor.mEnabledManipulators.end(), id("FloraEditorShowBranchHandles"));
		if (Editor.IsMode(Mode::BuildMode) && Editor.mbShowBoneLengthHandles
			&& Editor.mSaveExtension == TypeIDs::flr)//&& manipulatorID_showLimbs != Editor.mEnabledManipulators.end()) {
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

void FEReconEditorsLimbs::AttachDetours() {
	Editor_OnMouseUp::attach(GetAddress(cEditor, OnMouseUp));
	Editor_OnMouseWheel::attach(GetAddress(cEditor, OnMouseWheel));
}
