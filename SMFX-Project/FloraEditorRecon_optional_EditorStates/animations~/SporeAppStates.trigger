################################################################
# State script for Spore game
#
# This basically defines the different parts of the game,
# and what has to be turned on or off as we move between
# those parts.
#
# Note: if you run with the -demo flag, you'll start up
# with the Demo state. Otherwise, you can run with
# -state:<stateName> to get a specific state.
#


state GameCinematicsMode
   mode GameCinematics
end

#################################################################################
# Main
#################################################################################

# default entry state.
state Main -id 0
   description "Spore Launch Screen"
   
   gotoState GalaxyGameEntry
   # gotoState DevLauncher
end

state DevLauncher -id 1
   entry
      mode EffectsViewer # just to get a black background.
      cheat "uilayout -load E3LaunchScreen 0x027996d8 0x0279bf9e"
      effect  I_splashscreen
      effect UI_splashscreen_start
   exit
      cheat "uilayout -unload E3LaunchScreen"
end

state BasicLauncher
   entry
      mode Launcher
   exit
end

# backwards compat. for sniffs
# TODO: shouldn't they be calling Main?
state LaunchScreen
   gotoState DevLauncher
end

seti kMessageLoadGame 0xf8b1a2af

state LoadGame
  description "Load Game"
  entry
    mode LoadGame
    message -id $kMessageLoadGame
  exit
end 


#################################################################################
# Demo States
#################################################################################

# Run game with -demo to get this.
state Demo
   description "Spore EAP3 Demo"
   
   cheat "prop demo true"
   gotoState GalaxyGameEntry
end

#################################################################################
# Preload
#################################################################################

# Run game with -preload to get this.
state Preload 

end




#################################################################################
# Main game
#################################################################################


# Editors, direct entry
seti kMessageSwitchEditor 0x022d308b

# assign manual IDs just so I can fill in the UI layout stuff more easily.
state PlannerThumbnailGen -id 0x734444b0
   mode Editors
   message -id $kMessageSwitchEditor -data1 (hash(PlannerThumbnailGen))
end

state CellEditor -id 0x022d9e00
   mode Editors
   message -id $kMessageSwitchEditor -data1 (hash(CellTemplate))
end

state CellToCreatureEditor -id 0x0588b78b
   mode Editors
   message -id $kMessageSwitchEditor -data1 (hash(CellToCreatureEditor))
end

state CreatureEditor -id 0x022d9e01
   mode Editors
   message -id $kMessageSwitchEditor -data1 (hash(CreatureEditorExtraLarge))
end

state BuildingEditor -id 0x022d9e02
   mode Editors
   message -id $kMessageSwitchEditor -data1 (hash(BuildingTemplate))
end

state VehicleEditor -id 0x022d9e03
   mode Editors
   message -id $kMessageSwitchEditor -data1 (hash(VehicleTemplate))
end

state VehicleLandEditor -id 0x052b3270
   mode Editors
   message -id $kMessageSwitchEditor -data1 (hash(VehicleMilitaryLand))
end

state VehicleAirEditor -id 0x052b3271
   mode Editors
   message -id $kMessageSwitchEditor -data1 (hash(VehicleMilitaryAir))
end

state VehicleWaterEditor -id 0x052b3272
   mode Editors
   message -id $kMessageSwitchEditor -data1 (hash(VehicleMilitaryWater))
end

state UFOEditor -id 0xf12178d3
   mode Editors
   message -id $kMessageSwitchEditor -data1 (hash(UFOEditorSetup))
end

state FloraEditor -id 0x022d9e05
   mode Editors
   message -id $kMessageSwitchEditor -data1 (hash(FloraEditorSetup))
end

state FloraEditorSmall -id 0x983626eb
   mode Editors
   message -id $kMessageSwitchEditor -data1 (hash(FloraEditorSmall))
end

state FloraEditorMedium -id 0xff5906cb
   mode Editors
   message -id $kMessageSwitchEditor -data1 (hash(FloraEditorMedium))
end

state FloraEditorLarge -id 0xa4e33b1f
   mode Editors
   message -id $kMessageSwitchEditor -data1 (hash(FloraEditorLarge))
end

state FloraEditorSmallUFO -id 0x052763f1
   mode Editors
   message -id $kMessageSwitchEditor -data1 (hash(FloraEditorSmallUFO))
end

state FloraEditorMediumUFO -id 0x44040451
   mode Editors
   message -id $kMessageSwitchEditor -data1 (hash(FloraEditorMediumUFO))
end

state FloraEditorLargeUFO -id 0xd88f3025
   mode Editors
   message -id $kMessageSwitchEditor -data1 (hash(FloraEditorLargeUFO))
end

state TribalAccessoriesEditor -id 0x022d9e06
   mode Editors
   message -id $kMessageSwitchEditor -data1 (hash(SpaceAccessoriesEditorSetup))
end

state AdventureAccessoriesEditor -id 0x07429067
   mode Editors
   message -id $kMessageSwitchEditor -data1 (hash(AdventurerAccessoriesEditorSetup))
end

state TerrainEditor -id 0x022d9e10
   mode TerrainEditor
end

########################
# Level Editor
########################

state LevelEditor
   entry
      mode Game_EditMode
   exit
end

state LevelEditorFromSpaceGame
   entry
      gotoState LevelEditor
   exit
end

state LevelEditorFromCreatureGame
   entry
      gotoState LevelEditor
   exit
end

state LevelEditorFromTribeGame
   entry
      gotoState LevelEditor
   exit
end

state LevelEditorFromCivilisationGame
   entry
      gotoState LevelEditor
   exit
end

########################
# Sceanrio mode
########################
state ScenarioMode
   entry
      mode ScenarioMode
   exit
end

########################
# Cell Game
########################


# Games, direct entry

state CellGame
   entry
      mode Game_Cell
   exit
   
end

state CellGameFromLaunchScreen
    entry
        gotoState CellGame
    exit
end

state CellGameFromLoadGame
    entry
        gotoState CellGame
    exit
end
        



########################
# Creature Game
########################

state CreatureGame
   entry
      mode Game_Creature
   exit
   
end

state CreatureGameFromLevelEditor
    entry
        gotoState CreatureGame
    exit
end

state CreatureGameFromLaunchScreen
    entry
        gotoState CreatureGame
    exit
end


seti kMessageCellToCreatureTransition 0x02A4f8f0
state CreatureGameFromCellGame
   entry
      gotoState CreatureGame
   exit
      message -id $kMessageCellToCreatureTransition
end

state CreatureGameFromLoadGame
    entry
        gotoState CreatureGame
    exit
end


########################
# Tribe Game
########################

state TribeGame
   entry
      mode Game_Tribe
   exit
   
end

state TribeGameFromLevelEditor
    entry
        gotoState TribeGame
    exit
end

seti kMessageCreatureToTribeTransition 0x0234e60a
state TribeGameFromCreatureGame
    entry
        gotoState TribeGame
    exit
end

state TribeGameFromLaunchScreen
    entry
        gotoState TribeGame
    exit
end

state TribeGameFromLoadGame
    entry
        gotoState TribeGame
    exit
end


########################
# Civilisation Game
########################

state CivilisationGame
   entry
      mode Game_Civ   
   exit
   
end

state CivilisationGameFromLevelEditor
    entry
        gotoState CivilisationGame
    exit
end

state CivilisationGameFromLaunchScreen
    entry
        gotoState CivilisationGame
    exit
end

state CivilisationGameFromLoadGame
    entry
        gotoState CivilisationGame
    exit
end

state CivilisationGameFromTribeGame
    entry
	gotoState CivilisationGame
    exit
end

########################
# Space Game
#######################

state SpaceGame
   entry
      mode Game_Space
   exit
   
end


state SpaceGameFromLevelEditor
    entry
        gotoState SpaceGame
    exit
end

state SpaceGameFromCivilisationGame
   entry
      gotoState SpaceGame
   exit
end

seti kMessageSpaceGameFromLaunchScreen 0x02364016
state SpaceGameFromLaunchScreen
    entry
        message -id $kMessageSpaceGameFromLaunchScreen
        gotoState SpaceGame
    exit
end

state SpaceGameFromLoadGame
    entry
        gotoState SpaceGame
    exit
end

############################################################
# Multiverse Journal
#

state MultiverseJournal -id 0x036af998
   mode MultiverseJournal
end

############################################################
# Development
#

state EffectsViewer
   mode EffectsViewer
end

state ModelViewer
   mode ModelViewer
end

state AnimViewer -id 0x679d892f
   mode AnimViewer
end

state SniffTest
   cheat "tester -run TestScript sniff_test -quit -ea_asserts"
end

# triggered from GGE by End key in a non-demo production build
state SniffTestWithAllAsserts
   cheat "tester -run TestScript sniff_test -quit -ea_asserts"
end

state SniffTestWithTestAsserts
   cheat "tester -run TestScript sniff_test -quit -test_asserts"
end

# triggered from GGE by shift-End key in a non-demo production build
state SniffTestLoadSave
   cheat "tester -run TestScript sniff_test_load_save -quit -ea_asserts"
end

state GalaxyGameEntry -id 0x02ccda9c
   mode GalaxyGameEntry
end
