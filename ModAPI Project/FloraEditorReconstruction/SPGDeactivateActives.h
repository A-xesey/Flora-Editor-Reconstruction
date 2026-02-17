#pragma once
#include <Spore\BasicIncludes.h>

using namespace Simulator;

class SPGDeactivateActives
	//: public Object
	//: public DefaultRefCounted
{
public:

	/*SPGDeactivateActives();
	~SPGDeactivateActives();*/

	static void  DeactivateActives(cPlayerInventory* inventory);
	/*int AddRef() override;
	int Release() override;*/
};