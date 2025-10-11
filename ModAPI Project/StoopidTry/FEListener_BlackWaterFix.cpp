#include "stdafx.h"
#include "FEListener_BlackWaterFix.h"

FEListener_BlackWaterFix::FEListener_BlackWaterFix()
{
}


FEListener_BlackWaterFix::~FEListener_BlackWaterFix()
{
}

// For internal use, do not modify.
int FEListener_BlackWaterFix::AddRef()
{
	return DefaultRefCounted::AddRef();
}

// For internal use, do not modify.
int FEListener_BlackWaterFix::Release()
{
	return DefaultRefCounted::Release();
}


// The method that receives the message. The first thing you should do is checking what ID sent this message...
bool FEListener_BlackWaterFix::HandleMessage(uint32_t messageID, void* message)
{
	if (messageID == App::OnModeEnterMessage::ID)
	{
		auto data = (App::OnModeEnterMessage*)message;

		if ((GameModeIDs::kGameCell <= data->GetModeID() <= GameModeIDs::kGameSpace && data->GetModeID() != 0x01654c03)
			|| data->GetModeID() == GameModeIDs::kScenarioMode)
		{
			//Simulator::cPlanet* planet = Simulator::GetActivePlanet();
			//Simulator::cPlanetRecord* planetRecord = Simulator::GetActivePlanetRecord();
			
			//if (planet != 0 && planetRecord != 0)
			if (PlanetModel.Get())
			{
				Terrain::DisposeRefractionRTTs();
				Terrain::CreateRefractionRTTs();
				return true;
			}
		}
	}
	// Return true if the message has been handled. Other listeners will receive the message regardless of the return value.
	return false;
}
