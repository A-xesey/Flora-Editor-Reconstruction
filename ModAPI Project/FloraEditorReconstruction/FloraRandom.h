#pragma once

#include <Spore\BasicIncludes.h>

bool CreateFloraList();
void ClearFloraList();
bool ValidPlant(const ResourceKey& key, const eastl::vector<ResourceKey>& passSamePlants);
uint32_t GetRandomFloraName(bool smallSpecies, bool mediumSpecies, bool largeSpecies, bool isGameMode);
