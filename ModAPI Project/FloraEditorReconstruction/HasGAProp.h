#pragma once
#include <Spore\BasicIncludes.h>

inline bool HasGAProp()
{
	IWindowPtr tagsWin = Editor.mpEditorNamePanel->mpLayout->FindWindowByID(0x5415e48);
	if (tagsWin != nullptr) {
		eastl::string16 tags = u"";
		tags = tagsWin->GetCaption();
		tags.make_lower();
		if (tags.find(u"gaprop") != eastl::string16::npos)
			return true;
	}
	return false;
}

inline void ChangeBounds(uint32_t uModelType)
{
	PropertyListPtr editorProp;
	uint32_t editorID = id("FloraEditorLarge");
	if (!HasGAProp()) {
		if (uModelType == kPlantMedium)
			editorID = id("FloraEditorMedium");
		else if (uModelType == kPlantSmall)
			editorID = id("FloraEditorSmall");
	}

	PropManager.GetPropertyList(editorID, 0x40600100, editorProp);
	if (editorProp != nullptr)
	{
		bool boundsChanged = false;
		float editorBoundSize;
		float editorMaxHeight;
		float editorMinHeight;
		App::Property::GetFloat(editorProp.get(), 0x700DB77D, editorBoundSize);
		App::Property::GetFloat(editorProp.get(), 0x2704959D, editorMaxHeight);
		App::Property::GetFloat(editorProp.get(), 0x44C7F29F, editorMinHeight);
		if (editorBoundSize != Editor.mBoundSize || editorBoundSize != Editor.mpEditorModel->mBounds) {
			Editor.mBoundSize = editorBoundSize;
			Editor.mpEditorModel->mBounds = editorBoundSize;
			boundsChanged = true;
		}
		if (editorMaxHeight != Editor.mfMaxHeight || editorMaxHeight != Editor.mpEditorModel->mMaxHeight) {
			Editor.mfMaxHeight = editorMaxHeight;
			Editor.mpEditorModel->mMaxHeight = editorMaxHeight;
			boundsChanged = true;
		}
		if (editorMinHeight != Editor.mMinHeight || editorMinHeight != Editor.mpEditorModel->mMinHeight) {
			Editor.mMinHeight = editorMinHeight;
			Editor.mpEditorModel->mMinHeight = editorMinHeight;
			boundsChanged = true;
		}
		if (boundsChanged) {
			cEditorResourcePtr editorRes = new Editors::cEditorResource();
			Editor.mpEditorModel->Save(editorRes.get());
			if (editorRes != nullptr)
			{
				//SP::cSPEditorModelValidity::TestBounds
				if (STATIC_CALL
				(
					Address(ModAPI::ChooseAddress(0x4e9280, 0x4ef1b0)),
					bool,
					Args(Editors::cEditorResource*, int* /*eastl::bitset<128>*/),
					Args(editorRes.get(), /*&Editor.mModelValidity*/ &Editor.field_48)
				))
				{
					//App::ConsolePrintF("test bounds passed");
					Editor.field_48 = Editor.field_48 & 0xfffffeff;
					/*Editor.mModelValidity[Editors::kValidityOutOfBounds] = false;*/
				}
				/*else
					App::ConsolePrintF("no.");*/
			}
			//App::ConsolePrintF("kValidityOutOfBounds (editor) %i", /*Editor.mModelValidity[Editors::kValidityOutOfBounds]*/ Editor.field_48 & 0x100 == true);
		}
	}
}
