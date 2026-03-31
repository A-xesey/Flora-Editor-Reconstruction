#pragma once
#include <Spore\BasicIncludes.h>

using namespace App;
using namespace Editors;

class FEReconEditorsHotKeys
{
public:

	static void Initialize() {}

	static void Dispose() {}

	static void AttachDetours();
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

int GetManipulatorIndex(const uint32_t& find)
{
	if (!Editor.mEnabledManipulators.empty()) {
		for (int index = 0; index < Editor.mEnabledManipulators.size(); index++) {
			if (Editor.mEnabledManipulators[index] == find) return index;
		}
	}
	return Editor.mEnabledManipulators.size();
}

uint32_t GetInterpenetrationManipulator()
{
	uint32_t* findManipulator = eastl::find(
		Editor.mEnabledManipulators.begin(),
		Editor.mEnabledManipulators.end(),
		id("cSPEditorManipulationInterpenetration")
	);
	if (findManipulator != Editor.mEnabledManipulators.end()) return *findManipulator;
	else {
		findManipulator = eastl::find(
			Editor.mEnabledManipulators.begin(),
			Editor.mEnabledManipulators.end(),
			id("cSPEditorManipulationPlanarInterpenetration")
		);
		if (findManipulator != Editor.mEnabledManipulators.end()) return *findManipulator;
	}
	return *Editor.mEnabledManipulators.end();
}

/// adds hotkey to show up/off the model chooser
member_detour(Editor_OnKeyDown, cEditor, bool(int, KeyModifiers))
{
	bool detoured(int virtualKey, KeyModifiers modifiers)
	{
		if (Editor.mSaveExtension == TypeIDs::flr)
		{
			if (virtualKey == 'Q' && modifiers.IsCtrlDown) {
				IWindowPtr TypeChooser = Editor.mpEditorUI->mSharedUI.FindWindowByID(0xD0353720);
				if (TypeChooser != nullptr) {
					TypeChooser->SetVisible(!TypeChooser->IsVisible());
					saveChanges = TypeChooser->IsVisible();
				}
			}
			if (virtualKey == 'F' && modifiers.IsShiftDown) {
				uint32_t findManipulator = GetInterpenetrationManipulator();
				if (findManipulator != *Editor.mEnabledManipulators.end()) {
					int index = GetManipulatorIndex(findManipulator);
					if (index != Editor.mEnabledManipulators.size()) {
						Editor.mEnabledManipulators.erase(Editor.mEnabledManipulators.begin() + index);
						if (findManipulator == id("cSPEditorManipulationInterpenetration")) {
							Editor.mEnabledManipulators.push_back(id("cSPEditorManipulationPlanarInterpenetration"));
							ConsolePrintF("Manipulator behaviour was changed to vehicle editor");
						}
						else {
							Editor.mEnabledManipulators.push_back(id("cSPEditorManipulationInterpenetration"));
							ConsolePrintF("Manipulator behaviour was changed to default");
						}
					}
				}
			}
		}
		return original_function(this, virtualKey, modifiers);
	}
};

void FEReconEditorsHotKeys::AttachDetours() {
	Editor_OnKeyDown::attach(GetAddress(cEditor, OnKeyDown));
	EditorUI_Load::attach(GetAddress(EditorUI, Load));
}
