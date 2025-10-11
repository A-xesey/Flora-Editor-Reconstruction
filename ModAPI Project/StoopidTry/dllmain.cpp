// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include "FETest.h"
#include "FEListener_BlackWaterFix.h"
#include <Spore\Editors\BakeManager.h>
#include <Spore\Sporepedia\AssetViewManager.h>

using namespace App;

void Initialize()
{
	CheatManager.AddCheat("FE", new FETest());
	MessageManager.AddListener(new FEListener_BlackWaterFix(), OnModeEnterMessage::ID); //Black water fix
}

///Editor
// fix freeze after paint mode
member_detour(Editor__Update__detour, Editors::cEditor, void(float, float)) {
	void detoured(float delta1, float delta2) {
		auto manipulatorID_fixAnim = eastl::find(Editor.mEnabledManipulators.begin(), Editor.mEnabledManipulators.end(), id("FloraEditorDisableAnimation"));
		if (Editor.IsMode(Editors::Mode::BuildMode) && manipulatorID_fixAnim != Editor.mEnabledManipulators.end())
			Editor.field_385 = false;
		original_function(this, delta1, delta2);
	}
};

// select limbs
member_detour(Editor_OnMouseUp__detour, Editors::cEditor, bool(MouseButton, float, float, MouseState)) {
	bool detoured(MouseButton mouseButton, float mouseX, float mouseY, MouseState mouseState) {
		auto manipulatorID_showLimbs = eastl::find(Editor.mEnabledManipulators.begin(), Editor.mEnabledManipulators.end(), id("FloraEditorShowBranchHandles"));
		if (Editor.IsMode(Editors::Mode::BuildMode) && manipulatorID_showLimbs != Editor.mEnabledManipulators.end()) {
			for (EditorRigblockPtr part : Editor.GetEditorModel()->mRigblocks)
			{
				if (part->mBooleanAttributes[Editors::EditorRigblockBoolAttributes::kEditorRigblockModelUseSkin])
				{
					if (Editor.mpMovingPart == part)
						Editor.mpSelectedPart = part;

					if (Editor.mpSelectedPart && mouseButton == MouseButton::kMouseButtonLeft)
						Editor.mpSelectedPart->field_38.field_1 = true;
				}
			}
		}
		 return original_function(this, mouseButton, mouseX, mouseY, mouseState);
	}
};

// cap count valid
bool IsEnoughCap()
{
	if (Editor.IsActive())
	{
		App::PropertyList* propList = App::GetAppProperties();
		App::Property* prop = 0;
		AppProperties.GetProperty(0x55d7ca1, prop);
		bool DisableValidation;
		prop->GetBool(propList, 0x55d7ca1, DisableValidation);

		if (!DisableValidation && Editor.mSaveExtension == id("flr"))
		{
			uint32_t modelType = Editor.GetEditorModel()->mModelType;
			if ((modelType == id("PlantSmall") || modelType == id("PlantMedium"))
				&& Editor.mEditHistoryIndex > 1)
			{
				int capCount = 0;
				int rigBlockCount = Editor.mpEditorModel->GetRigblocksCount();
				for (EditorRigblockPtr part : Editor.GetEditorModel()->mRigblocks)
				{
					int partCount = part->mModelRigBlockType.instanceID == id("cap");
					capCount += partCount;
				}
				if (rigBlockCount >= 3 && capCount < 3)
				{
					return false;
				}
			}
		}
	}
	return true;
};

member_detour(EditorUI_SetMessageHint, Editors::EditorUI, uint32_t(int))
{
	uint32_t detoured(int helpme)
	{
		if (!IsEnoughCap())
		{
			if (helpme == 0x101 || helpme == 0x102) //Save button/command or "Save and Exit" button
			{
				HintManager.ShowHint(id("ferecon-errorinvalidcapcount"));
				return original_function(this, 0);
			}
		}
		return original_function(this, helpme);
	}
};

static_detour(UTFWin_cSPUIMessageBox__detour, bool(UTFWin::MessageBoxCallback*, const ResourceKey&))
{
	bool detoured(UTFWin::MessageBoxCallback * pcallback, const ResourceKey & key)
	{
		if (!IsEnoughCap())
		{
			Editor.mpEditorUI->field_C8 = 0x5CD172A9;
			ResourceKey key2 = key;

			if (key.instanceID == 0x12B8BA66 || key.instanceID == 0x48EE60F0 || key.instanceID == 0xEB3BC709)
				key2.instanceID = id("FERecon_New");

			else if (key.instanceID == 0xBE186A9A || key.instanceID == 0xB016B570 || key.instanceID == 0xC2B30789)
				key2.instanceID = id("FERecon_Sporepedia");

			return original_function(pcallback, key2);
		}
		return original_function(pcallback, key);
	}
};

///Sporepedia
//Makes flora editable
virtual_detour(Sporepedia_IsEditable__detour, Sporepedia::cSPAssetDataOTDB, Sporepedia::IAssetData, bool())
{
	bool detoured()
	{
		uint32_t subType = this->GetAssetSubtype();

		if (subType == ModelTypes::kPlantSmall || subType == ModelTypes::kPlantMedium || subType == ModelTypes::kPlantLarge)
			return this->mIsEditable = true;

		if (subType != TypeIDs::cmp)
			return this->mIsEditable;

		return false;
	}
};

//Delete creations
virtual_detour(DeleteFunc__Detour, Sporepedia::cSPAssetDataOTDB, Sporepedia::IAssetData, void()) //Sporepedia::OTDB::cObjectTemplateDB,  App::IMessageListener/Manager or  App::IUnmanagedMessageListener
{
	void detoured()
	{
		return original_function(this);
	}
};

virtual_detour(DeleteFuncPart__Detour, Sporepedia::cSPAssetDataOTDB, Sporepedia::IAssetData, void*(uint32_t*, ResourceKey*, int))
{
	void* detoured(uint32_t * unk1, ResourceKey* creation, int unk3) //unk3 is bool?
	{
		return original_function(this, unk1, creation, unk3);
	}
};

//member_detour(Thumbnail_cImportExport_SavePNG__detour, App::Thumbnail_cImportExport,
//	bool(ResourceObjectPtr, RenderWare::Raster*, DatabasePtr, bool, bool))
//{
//	bool detoured(ResourceObjectPtr pObject, RenderWare::Raster * pImage, DatabasePtr pDatabase,
//		bool forceReplace, bool disableSteganography)
//	{
//		return original_function(this, pObject, pImage, pDatabase, forceReplace, 1);
//	}
//};

/////I hate this guy
////Bake Sprites sub function
///fix 3 sec freeze
bool SpriteTextureMIPsDirectlyChange()
{
	App::PropertyList* propList = App::GetAppProperties();
	App::Property* prop = 0;

	AppProperties.GetProperty(id("FERecon_DirectlyChange"), prop);
	bool DirectlyChange;
	prop->GetBool(propList, id("FERecon_DirectlyChange"), DirectlyChange);

	return DirectlyChange;
};

int SpriteTextureMIPsProperties(bool Parameters, bool variable)
{
	App::PropertyList* propList = App::GetAppProperties();
	App::Property* prop = 0;
	int SpriteTextureMIPsProperties;

	if (!Parameters) //textureQuality
	{
		if (!variable) //NMap
		{
			AppProperties.GetProperty(id("FERecon_textureQualityNormalMap"), prop);
			prop->GetInt32(propList, id("FERecon_textureQualityNormalMap"), SpriteTextureMIPsProperties);
		}
		else //DMap
		{
			AppProperties.GetProperty(id("FERecon_textureQualityDiffuseMap"), prop);
			prop->GetInt32(propList, id("FERecon_textureQualityDiffuseMap"), SpriteTextureMIPsProperties);
		}
	}
	else //textureDXT
	{
		if (!variable)  //NMap
		{
			AppProperties.GetProperty(id("FERecon_textureDXTNormalMap"), prop);
			prop->GetInt32(propList, id("FERecon_textureDXTNormalMap"), SpriteTextureMIPsProperties);
		}
		else  //DMap
		{
			AppProperties.GetProperty(id("FERecon_textureDXTDiffuseMap"), prop);
			prop->GetInt32(propList, id("FERecon_textureDXTDiffuseMap"), SpriteTextureMIPsProperties);
		}
	}

	if (!Parameters && SpriteTextureMIPsProperties != -1 && SpriteTextureMIPsProperties != 4 && SpriteTextureMIPsProperties != 5
		&& SpriteTextureMIPsProperties != 6)
	{
		if (!variable)
		{
			App::ConsolePrintF("The textureQualityNormalMap value has been reset to avoid crashing the game.\nYou can only use -1, 4, 5 or 6.");
			SpriteTextureMIPsProperties = 4;
		}
		else
		{
			App::ConsolePrintF("The textureQualityDiffuseMap value has been reset to avoid crashing the game.\nYou can only use -1, 4, 5 or 6.");
			SpriteTextureMIPsProperties = 6;
		}
	}
	else if (Parameters && SpriteTextureMIPsProperties > 90)
	{
		App::ConsolePrintF("The textureDXT value has been reset to avoid totally freezing the game.");
		SpriteTextureMIPsProperties = 50;
	}
	return SpriteTextureMIPsProperties;
};

static_detour(Graphics_SpriteTextureMIPs, void(int, int, int, int))
{
	void detoured(int param_1, int textureQuality, int textureDXT, int param_4)
	{
		if (SpriteTextureMIPsDirectlyChange() == true)
		{
			if (textureQuality == 4) //NormalMap
			{
				textureQuality = SpriteTextureMIPsProperties(0, 0);
				textureDXT = SpriteTextureMIPsProperties(1, 0);
			}

			if (textureQuality == 6) //DiffuseMap
			{
				textureQuality = SpriteTextureMIPsProperties(0, 1);
				textureDXT = SpriteTextureMIPsProperties(1, 1);
			}
		}
		original_function(param_1, textureQuality, textureDXT, param_4);
		return;
	}
};

///Baking system
member_detour(Simulator__cPlantSpeciesManager__Get, Simulator::cPlantSpeciesManager, Simulator::cPlantCargoInfo* (const ResourceKey&))
{
	Simulator::cPlantCargoInfo* detoured(const ResourceKey& speciesID)
	{
		//Simulator::cPlanet* planet = Simulator::GetActivePlanet();
		Simulator::cPlanetRecord* planetRecord = Simulator::GetActivePlanetRecord();

		if ((planetRecord != 0 && planetRecord->mPlantSpecies.size() > 0)
			&& (speciesID.groupID != 0 && speciesID.instanceID != 0 && speciesID.typeID != 0))
			{
				BakeManager.Bake(speciesID, Editors::BakeParameters::BakeParameters(1));
			}
		return original_function(this,speciesID);
	}
};

void Dispose()
{
	// This method is called when the game is closing
}
void AttachDetours()
{
	//Editor
	Editor__Update__detour::attach(GetAddress(Editors::cEditor, Update));
	Editor_OnMouseUp__detour::attach(GetAddress(Editors::cEditor, OnMouseUp));

	//Editor hints
	EditorUI_SetMessageHint::attach(Address(0x005dfd40));
	UTFWin_cSPUIMessageBox__detour::attach(GetAddress(UTFWin::cSPUIMessageBox, ShowDialog));
	
	//Sporepedia/Pollinator
	Sporepedia_IsEditable__detour::attach(GetAddress(Sporepedia::cSPAssetDataOTDB, IsEditable));
	//DeleteFunc__Detour::attach(Address(0x0064b420));
	DeleteFuncPart__Detour::attach(Address(0x00644a40));

	//Another
	Graphics_SpriteTextureMIPs::attach(Address(0x00431170));
	Simulator__cPlantSpeciesManager__Get::attach(GetAddress(Simulator::cPlantSpeciesManager, CreatePlantItem));

	//Thumbnail_cImportExport_SavePNG__detour::attach(GetAddress(App::Thumbnail_cImportExport, SavePNG)); //for thumbnails
	// 
	//FUN_00552300	FUN_00648d00	FUN_0064e520	FUN_00647830	FUN_0064aa90
	//::attach(Address(ModAPI::ChooseAddress(0x, 0x)));
	//FUN_00552300::attach(Address(ModAPI::ChooseAddress(0x00552300, 0x00552300)));
	//FUN_00648d00::attach(Address(ModAPI::ChooseAddress(0x00648d00, 0x00648d00)));
	//FUN_0064e520::attach(Address(ModAPI::ChooseAddress(0x0064e520, 0x0064e520)));
	//FUN_00647830::attach(Address(ModAPI::ChooseAddress(0x00647830, 0x00647830)));
	//FUN_0064aa90::attach(Address(ModAPI::ChooseAddress(0x0064aa90, 0x0064aa90)));
}


// Generally, you don't need to touch any code here
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		ModAPI::AddPostInitFunction(Initialize);
		ModAPI::AddDisposeFunction(Dispose);

		PrepareDetours(hModule);
		AttachDetours();
		CommitDetours();
		break;

	case DLL_PROCESS_DETACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	}
	return TRUE;
}

