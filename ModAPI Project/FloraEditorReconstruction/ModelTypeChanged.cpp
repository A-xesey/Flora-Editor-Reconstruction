#include "stdafx.h"
#include "ModelTypeChanged.h"

ModelTypeChanged::ModelTypeChanged()
{
}


ModelTypeChanged::~ModelTypeChanged()
{
}

// For internal use, do not modify.
int ModelTypeChanged::AddRef()
{
	return DefaultRefCounted::AddRef();
}

// For internal use, do not modify.
int ModelTypeChanged::Release()
{
	return DefaultRefCounted::Release();
}

// The method that receives the message. The first thing you should do is checking what ID sent this message...
bool ModelTypeChanged::HandleMessage(uint32_t messageID, void* message)
{
	if (messageID == id("Editors_ModelWasChanged") && Editor.IsActive())
	{
		if (Editor.mVerbIconTray != nullptr && Editor.mVerbIconTray->mVerbTrays.size() > 0) {
			for (const cSPEditorVerbIconTrayPtr& verbTray : Editor.mVerbIconTray->mVerbTrays) {
				IWindowPtr floraHeader = verbTray->mLayout->FindWindowByID(0x811C9DC5);
				if (floraHeader != nullptr && verbTray->mWinText != nullptr && verbTray->mRollover != nullptr) {
					ResourceKey iconKey{ 0, TypeIDs::png, id("VerbIcons") };
					LocalizedString localHeader;
					//LocalizedString localDesc;
					App::Property* propHeader = nullptr;
					App::Property* propDesc = nullptr;
					if (Editor.mpEditorModel != nullptr)
					{
						if (Editor.mpEditorModel->mModelType == kPlantLarge)
						{
							localHeader.SetText(id("sporepedia"), 0x0002000F);
							//localDesc.SetText(id("fe_02"), 0x000000A1);
							verbTray->mVerbTrayProperties->GetProperty(id("verbTrayNameLarge"), propHeader);
							verbTray->mVerbTrayProperties->GetProperty(id("verbTrayDescriptionLarge"), propDesc);
							iconKey.instanceID = id("flora_large");
						}
						else if (Editor.mpEditorModel->mModelType == kPlantMedium)
						{
							localHeader.SetText(id("sporepedia"), 0x00020010);
							//localDesc.SetText(id("fe_02"), 0x000000A2);
							verbTray->mVerbTrayProperties->GetProperty(id("verbTrayNameMedium"), propHeader);
							verbTray->mVerbTrayProperties->GetProperty(id("verbTrayDescriptionMedium"), propDesc);
							iconKey.instanceID = id("flora_medium");
						}
						else if (Editor.mpEditorModel->mModelType == kPlantSmall)
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
						return true;
					}
					propHeader = nullptr;
					propDesc = nullptr;
				}
			}
		}
	}
	// Return true if the message has been handled. Other listeners will receive the message regardless of the return value.
	return false;
}
