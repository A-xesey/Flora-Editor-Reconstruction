// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"

#include "FEReconEditors.h"
#include "FEReconSporepedia.h"
#include "FEReconBakeManager.h"
#include "FEReconSpaceGame.h"
#include "FETest.h"

void Initialize()
{
	FEReconEditors::Initialize();
	FEReconSporepedia::Initialize();
	FEReconBakeManager::Initialize();
	FEReconSpaceGame::Initialize();
	CheatManager.AddCheat("FE", new FETest());
}

void Dispose()
{
	// This method is called when the game is closing
	FEReconEditors::Dispose();
	FEReconSporepedia::Dispose();
	FEReconBakeManager::Dispose();
	FEReconSpaceGame::Dispose();
}

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

void AttachDetours()
{
	FEReconEditors::AttachDetours();
	FEReconSporepedia::AttachDetours();
	FEReconBakeManager::AttachDetours();
	FEReconSpaceGame::AttachDetours();

	//for thumbnails
	//Thumbnail_cImportExport_SavePNG::attach(GetAddress(App::Thumbnail_cImportExport, SavePNG));
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

