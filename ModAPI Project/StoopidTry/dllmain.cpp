// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include <Spore\Editors\BakeManager.h>
#include "FEBlackWaterFixMsg.h"
#include "FETest.h"
#include "FESpaceTools.h"

using namespace App;

void Initialize()
{
	MessageManager.AddListener(new FEBlackWaterFixMsg(), id("BakeManager_BakeSprites"));
	//ToolManager.AddStrategy(new FESpaceTools(), id("FEReconPlantStrategy"));
	CheatManager.AddCheat("FE", new FETest());
}

////Editor
/// fix freeze after paint mode
member_detour(Editor_Update, Editors::cEditor, void(float, float)) {
	void detoured(float delta1, float delta2) {
		auto manipulatorID_fixAnim = eastl::find(Editor.mEnabledManipulators.begin(), Editor.mEnabledManipulators.end(), id("FloraEditorDisableAnimation"));
		if (Editor.IsMode(Editors::Mode::BuildMode) && manipulatorID_fixAnim != Editor.mEnabledManipulators.end())
			Editor.field_385 = false;
		original_function(this, delta1, delta2);
	}
};

/// select limbs
member_detour(Editor_OnMouseUp, Editors::cEditor, bool(MouseButton, float, float, MouseState)) {
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


/// cap count valid
bool IsEnoughCap()
{
	if (Editor.IsActive())
	{
		App::PropertyList* propList = App::GetAppProperties();
		App::Property* prop = 0;
		AppProperties.GetProperty(0x55d7ca1, prop);
		bool DisableValidation;
		prop->GetBool(propList, 0x55d7ca1, DisableValidation);

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
						for (Editors::EditorRigblockCapability capabality : part->mCapabilities)
						{
							if (capabality.mPropertyID == 0xD00F0FFB) //modelCapabilityFruit
								capCount += 1;
						}
					}
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
				|| key.instanceID == 0xEB3BC709) //Save changes before making new
					key2.instanceID = id("FERecon_New");

			else if (key.instanceID == id("editor_SaveOrCopyAndContinueLoad")
				|| key.instanceID == id("editor_SaveNewAndContinueLoad")
				|| key.instanceID == 0xC2B30789) //Save changes before loading
					key2.instanceID = id("FERecon_Sporepedia");

			else if (key.instanceID == id("editor_SaveOrCopyAndContinueExit")
				|| key.instanceID == id("editor_SaveNewAndContinueExit")
				|| key.instanceID == 0xCF4C3D25) //Save changes before exiting
				key2.instanceID = id("FERecon_Exit");

			return original_function(pcallback, key2);
		}
		return original_function(pcallback, key);
	}
};

////Sporepedia
///Makes all editable (except locking creations)
virtual_detour(Sporepedia_IsEditable, Sporepedia::cSPAssetDataOTDB, Sporepedia::IAssetData, bool())
{
	bool detoured()
	{
		if (this->mIsViewableLarge)
			return this->mIsEditable = true;
		else
			return this->mIsEditable = false;
	}
};

///Delete creations
static_detour(ObjectTemplateDB_GetSaveAreaID, Resource::SaveAreaID(GroupIDs::Names))
{
	Resource::SaveAreaID detoured(GroupIDs::Names groupID)
	{
		if (groupID == GroupIDs::CreatureModels)
			return Resource::SaveAreaID::Creatures;

		else if (groupID == GroupIDs::BuildingModels)
			return Resource::SaveAreaID::Buildings;

		else if (groupID == GroupIDs::VehicleModels)
			return Resource::SaveAreaID::Vehicles;

		else if (groupID == GroupIDs::UFOModels)
			return Resource::SaveAreaID::UFOs;

		else if (groupID == GroupIDs::Adventures)
			return Resource::SaveAreaID::Adventures;

		else if (groupID == GroupIDs::CellModels)
			return Resource::SaveAreaID::Cells;

		else if (groupID == GroupIDs::CityMusic)
			return Resource::SaveAreaID::CityMusic;

		else if (groupID == GroupIDs::FloraModels)
			return Resource::SaveAreaID::Plants;

		else
			return (Resource::SaveAreaID)0;
	}
};

////Baking system
///fix 3 sec freeze
//bool SpriteTextureMIPsDirectlyChange()
//{
//	App::PropertyList* propList = App::GetAppProperties();
//	App::Property* prop = 0;
//
//	AppProperties.GetProperty(id("FERecon_DirectlyChange"), prop);
//	bool DirectlyChange;
//	prop->GetBool(propList, id("FERecon_DirectlyChange"), DirectlyChange);
//
//	return DirectlyChange;
//};
//
//int SpriteTextureMIPsProperties(bool Parameters, bool variable)
//{
//	App::PropertyList* propList = App::GetAppProperties();
//	App::Property* prop = 0;
//	int SpriteTextureMIPsProperties;
//
//	if (!Parameters) //textureQuality
//	{
//		if (!variable) //NMap
//		{
//			AppProperties.GetProperty(id("FERecon_textureQualityNormalMap"), prop);
//			prop->GetInt32(propList, id("FERecon_textureQualityNormalMap"), SpriteTextureMIPsProperties);
//		}
//		else //DMap
//		{
//			AppProperties.GetProperty(id("FERecon_textureQualityDiffuseMap"), prop);
//			prop->GetInt32(propList, id("FERecon_textureQualityDiffuseMap"), SpriteTextureMIPsProperties);
//		}
//	}
//	else //textureDXT
//	{
//		if (!variable)  //NMap
//		{
//			AppProperties.GetProperty(id("FERecon_textureDXTNormalMap"), prop);
//			prop->GetInt32(propList, id("FERecon_textureDXTNormalMap"), SpriteTextureMIPsProperties);
//		}
//		else  //DMap
//		{
//			AppProperties.GetProperty(id("FERecon_textureDXTDiffuseMap"), prop);
//			prop->GetInt32(propList, id("FERecon_textureDXTDiffuseMap"), SpriteTextureMIPsProperties);
//		}
//	}
//
//	if (!Parameters && SpriteTextureMIPsProperties != -1 && SpriteTextureMIPsProperties != 4 && SpriteTextureMIPsProperties != 5
//		&& SpriteTextureMIPsProperties != 6)
//	{
//		if (!variable)
//		{
//			App::ConsolePrintF("The textureQualityNormalMap value has been reset to avoid crashing the game.\nYou can only use -1, 4, 5 or 6.");
//			SpriteTextureMIPsProperties = 4;
//		}
//		else
//		{
//			App::ConsolePrintF("The textureQualityDiffuseMap value has been reset to avoid crashing the game.\nYou can only use -1, 4, 5 or 6.");
//			SpriteTextureMIPsProperties = 6;
//		}
//	}
//	else if (Parameters && SpriteTextureMIPsProperties > 90)
//	{
//		App::ConsolePrintF("The textureDXT value has been reset to avoid totally freezing the game.");
//		SpriteTextureMIPsProperties = 50;
//	}
//	return SpriteTextureMIPsProperties;
//};

static_detour(BakeManager_SpriteTextureMIPs, void(int, int, int, int))
{
	void detoured(int param_1, int textureQuality, int textureDXT, int param_4)
	{
		//if (SpriteTextureMIPsDirectlyChange() == true)
		//{
		//	if (textureQuality == 4) //NormalMap
		//	{
		//		textureQuality = SpriteTextureMIPsProperties(0, 0);
		//		textureDXT = SpriteTextureMIPsProperties(1, 0);
		//	}
		//	if (textureQuality == 6) //DiffuseMap
		//	{
		//		textureQuality = SpriteTextureMIPsProperties(0, 1);
		//		textureDXT = SpriteTextureMIPsProperties(1, 1);
		//	}
		//}
		//original_function(param_1, textureQuality, textureDXT, param_4);
		original_function(param_1, textureQuality, 0, param_4);
		return;
	}
};

///detour it for msg
static_detour(BakeManager_BakeSprites, void(PropertyList*, Graphics::IModelWorld*, uint32_t))
{
	void detoured(PropertyList* templateProp, Graphics::IModelWorld* creationModel, uint32_t message)
	{
		return original_function(templateProp, creationModel, id("BakeManager_BakeSprites"));
	}
};

///Baking
member_detour(cPlantSpeciesManager_CreatePlantItem, Simulator::cPlantSpeciesManager, Simulator::cPlantCargoInfo* (const ResourceKey&))
{
	Simulator::cPlantCargoInfo* detoured(const ResourceKey& speciesID)
	{
		Simulator::cPlantCargoInfo* plantItem = original_function(this, speciesID);
		if (plantItem != 0)
		{
			ResourceKey plantKey = plantItem->GetItemID();
			if (plantKey.instanceID != 0 && plantKey.groupID != 0 && plantKey.typeID != 0)
			{
				bool IsBaked = BakeManager.IsBaked(plantKey, false);

				if (!IsBaked)
					BakeManager.Bake(plantKey, Editors::BakeParameters::BakeParameters(0, 4, 0x609b763));

				PropertyListPtr PlantItemProperty;
				PropManager.GetPropertyList(plantItem->GetItemID().instanceID, plantItem->GetItemID().groupID, PlantItemProperty);

				if (PlantItemProperty != 0)
				{
					ResourceKey modelLOD0;
					ResourceKey modelLOD1;
					ResourceKey modelLOD2;
					ResourceKey modelLOD3;
					ResourceKey modelSprite0;
					ResourceKey modelSprite1;

					float mBaseRadius;
					float mCanopyRadius;
					float mHeight;
					float mmodelBoundingRadius;

					Property::GetKey(PlantItemProperty.get(), 0x00F9EFBB, modelLOD0);
					Property::GetKey(PlantItemProperty.get(), 0x00F9EFBC, modelLOD1);
					Property::GetKey(PlantItemProperty.get(), 0x00F9EFBD, modelLOD2);
					Property::GetKey(PlantItemProperty.get(), 0x00F9EFBE, modelLOD3);
					Property::GetKey(PlantItemProperty.get(), 0xD124FCE4, modelSprite0);
					Property::GetKey(PlantItemProperty.get(), 0xD124FCE7, modelSprite1);
					Property::GetFloat(PlantItemProperty.get(), 0x0254CF89, mBaseRadius);
					Property::GetFloat(PlantItemProperty.get(), 0x0254CF8F, mCanopyRadius);
					Property::GetFloat(PlantItemProperty.get(), 0x0254CF97, mHeight);

					if (!IsBaked)
					{
						float PseudoFloraImposterScale = (mBaseRadius + mCanopyRadius + mHeight) / 3;

						ResourceKey SetPlantModelLOD0(plantItem->GetItemID().instanceID, TypeIDs::gmdl, 0x40667100);
						ResourceKey SetPlantModelLOD1(plantItem->GetItemID().instanceID, TypeIDs::gmdl, 0x40666201);
						ResourceKey SetPlantModelLOD2(plantItem->GetItemID().instanceID, TypeIDs::gmdl, 0x40666202);
						ResourceKey SetPlantModelLOD3(plantItem->GetItemID().instanceID, TypeIDs::gmdl, 0x40666203);
						ResourceKey SetPlantSpriteTop(plantItem->GetItemID().instanceID, TypeIDs::raster, 0x40662800);
						ResourceKey SetPlantSpriteSide(plantItem->GetItemID().instanceID, TypeIDs::raster, 0x40662801);

						PlantItemProperty->SetProperty(0x00F9EFBB, &Property().SetValueKey(SetPlantModelLOD0));
						PlantItemProperty->SetProperty(0x00F9EFBC, &Property().SetValueKey(SetPlantModelLOD1));
						PlantItemProperty->SetProperty(0x00F9EFBD, &Property().SetValueKey(SetPlantModelLOD2));
						PlantItemProperty->SetProperty(0x00F9EFBE, &Property().SetValueKey(SetPlantModelLOD3));
						PlantItemProperty->SetProperty(0xD124FCE7, &Property().SetValueKey(SetPlantSpriteTop));
						PlantItemProperty->SetProperty(0xD124FCE4, &Property().SetValueKey(SetPlantSpriteSide));
						PlantItemProperty->SetProperty(0x04057881, &Property().SetValueFloat(PseudoFloraImposterScale));

						plantItem->mFloraImposterScale = PseudoFloraImposterScale;
					}
					else
					{
						float mFloraImposterScale;
						Property::GetFloat(PlantItemProperty.get(), 0x04057881, mFloraImposterScale);
						plantItem->mFloraImposterScale = mFloraImposterScale;
					}

					plantItem->mAlphaModel = plantKey.instanceID;
					plantItem->mModelIDLOD0 = plantKey.instanceID;
					plantItem->mModelIDLOD1 = plantKey.instanceID;
					plantItem->mModelIDLOD2 = plantKey.instanceID;
					plantItem->mModelIDLOD3 = plantKey.instanceID;
					plantItem->mBaseRadius = mBaseRadius;
					plantItem->mCanopyRadius = mCanopyRadius;
					plantItem->mHeight = mHeight;
				}
			}
		}
		return plantItem;
	}
};

////for thumbs
//member_detour(Thumbnail_cImportExport_SavePNG, Thumbnail_cImportExport,
//	bool(ResourceObjectPtr, RenderWare::Raster*, DatabasePtr, bool, bool))
//{
//	bool detoured(ResourceObjectPtr pObject, RenderWare::Raster * pImage, DatabasePtr pDatabase,
//		bool forceReplace, bool disableSteganography)
//	{
//		return original_function(this, pObject, pImage, pDatabase, forceReplace, true);
//	}
//};

void Dispose()
{
	// This method is called when the game is closing
}
void AttachDetours()
{
	//Editor
	Editor_Update::attach(GetAddress(Editors::cEditor, Update));
	Editor_OnMouseUp::attach(GetAddress(Editors::cEditor, OnMouseUp));

	//Editor hints
	EditorUI_SetMessageHint::attach(Address(0x005dfd40));
	UTFWin_cSPUIMessageBox::attach(GetAddress(UTFWin::cSPUIMessageBox, ShowDialog));
	
	//Sporepedia/Pollinator
	Sporepedia_IsEditable::attach(GetAddress(Sporepedia::cSPAssetDataOTDB, IsEditable));
	ObjectTemplateDB_GetSaveAreaID::attach(Address(0x00563340));

	//Baking system
	BakeManager_BakeSprites::attach(Address(0x00431340));
	BakeManager_SpriteTextureMIPs::attach(Address(0x00431170));
	cPlantSpeciesManager_CreatePlantItem::attach(GetAddress(Simulator::cPlantSpeciesManager, CreatePlantItem));

	//for thumbnails
	//Thumbnail_cImportExport_SavePNG::attach(GetAddress(App::Thumbnail_cImportExport, SavePNG));

	//FUN_0054e530 creation is deleted?
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

