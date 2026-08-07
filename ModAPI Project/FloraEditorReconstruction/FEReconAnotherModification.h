#pragma once
#include <Spore\BasicIncludes.h>

using namespace App;

class FEReconAnotherModification
{
public:

	static void Initialize() {
		//add flora section as disguesingahbsdj option
		//don't ask me why i did it in this way. I am, uh... cuz why not ig-
		PropertyListPtr asset;
		PropManager.GetPropertyList(id("GameplayObjectSkinning"), id("AssetBrowserFilter"), asset);

		vector<ResourceKey>* browserFilterChildrenFilters = new vector<ResourceKey>();
		Property::GetArrayKey(asset.get(), 0x7435A2D3, *browserFilterChildrenFilters);

		ResourceKey florakey = { id("flora"), 0, 0 };
		auto flora_find = find
		(
			(*browserFilterChildrenFilters).begin(),
			(*browserFilterChildrenFilters).end(),
			florakey
		);

		if (flora_find == (*browserFilterChildrenFilters).end()) {
			(*browserFilterChildrenFilters).push_back(florakey);
			PropertyListPtr prop = new PropertyList();
			prop->SetParent(asset.get());
			prop->SetResourceKey({ id("FERecon_GameplayObjectSkinning"), TypeIDs::prop, id("AssetBrowserFilter") });
			prop->SetProperty(0x7435A2D3, &Property().SetArrayKey((*browserFilterChildrenFilters).data(), (*browserFilterChildrenFilters).size()));

			ResourceManager.CacheResource(prop.get(), true);

			asset = nullptr;
			PropManager.GetPropertyList(id("ScenarioGameplaySkinning"), id("AssetBrowserConfig"), asset);
			if (asset != nullptr) {
				//sporepediaConfigRootFilterID
				asset->SetProperty(0x144D3099, &Property().SetValueKey({ id("FERecon_GameplayObjectSkinning"), 0,0 }));
				//sporepediaConfigSelectedFilterID
				asset->SetProperty(0x943D7ED0, &Property().SetValueKey({ id("FERecon_GameplayObjectSkinning"), 0,0 }));
			}
		}
		flora_find = nullptr;
		(*browserFilterChildrenFilters).clear(); (*browserFilterChildrenFilters).resize(0); browserFilterChildrenFilters = nullptr;
	}

	static void Dispose() {}

	static void AttachDetours();
};

void FEReconAnotherModification::AttachDetours() {}
