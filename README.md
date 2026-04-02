<img src="https://github.com/user-attachments/assets/f6cf2c13-38f9-41f8-ba92-6c9204ac11c6" width="900" height="900" align="center"/>

This mod makes flora editor and plants in game usable and adds extra features:
**Editor:**
* Adds parts from pre-release version;
* Fix freezing after entered paint mode and then back to build mode;
* Adds header by each plant type;
* Fruit counter (**requires [Spore UI Fixer Framework](https://github.com/Valla-Chan/Valla_SporeUIFixerFramework/releases) mod by @Valla-Chan**);
* Fruit validation check;
* Hotkeys;
* Limb selecting and resizing rigblock-limbs;
* Background by each plant type;
* Music by Delta Unit!
* Fixing shadow in paint mode;
* Parts names by @Valla-Chan
* reconstructions of fe_leaf_19 and fe_leaf_33 by Valla-chan;
* Adds parts from demo-version of the game (fe_leaves);

**Sporepedia:**
* Deleting plants;
* Editing plant by pressing editing button.

**Game:**
* Tools to editing/creating plants in Space Stage;
* Baking plants;
* Replacing non-existing plants to existing;
* Black water fix.
* Fix plants randomizer (randomizer core and player plants instead only players);
* Adding bump-channel support; 

There are probably something else, but i'm already forget about that. ( . x .' )

**WARNING: Mod can't complicated with mods that:**
* **Fully changes flora editor;**
* **Uses custom shader fragments.**
**If you find out that some graphical mods uncomplitable with another mod, please report it!** (maybe I can fix that but i'm not sure)

# For fully mod working you need:
**To edit files in Config folder**
There are bug with plants in paint mode and it thumbnails, where are plants totally black and parts out of place. To fix that, you need to write code below:
* In the Properties.txt file: property FragmentCompilation  0x05fb85a3           bool
* In the ConfigManager.txt file: boolProp FragmentCompilation true

**To update database**
For plants randomizer fixing you need to update your game database. For that I recommend to download [FixTags](https://github.com/A-xesey/FixTags/releases/) mod and use cheat "updateDatabase".

# Editor features
* To select limbs you need to... click on it, yes-
* Ctrl + Q will show up the type chooser window, where you can choose the plant type (small, medium or large);
* Shift + F will change building/flora parts movement behaviour to vehicle and back if you pressed it again;

There a fruit validation check:
* You need to add at least 3 fruit nodes to save the plant;
* Fruits shouldn't be too close to the grounds or undergrounds;
* At least 3 fruits should be closer to ground (for small plant type);

# Optional components
* **Additional pages:** Adds additional pages on flora editor: one by each section (except Bases and Details);
* **Flora parts to Creature Editor:** Adds flora parts to creature editor as a separate section;
* **High-quality plant models:** Increase plants baking for High Player Model Quality option and quality of core-plant models;
* **2006-style background:** Changes editor background in every plant editors to background which was inspired by plant background from Spore artbook. Also changes editor music to "Homeworld, Sweet Homeworld";
* **Pre-release background:** Changes editor background in every plant editors  to background from pre-release version of the game;

# Notes (Mostly for player without modAPI)
  * In new games or in the space stage, you may come across player plants, but their models are not preloaded into the game. To fix this, you need to download plant models by viewing them in the sporopedia and re-enter the game.
  * If you don't add the "gaprop" tag to your plants, then add a couple of fruits to them so that during your play in the creature and tribe stages you can collect fruits from them.
  * The size of the fruit fragment determines the size of the fruit itself in the game.
  * Try to use the trunks as little as possible, during texturing the trunks take up more than half of the texture space, which is why the fragments will look terrible at the end of texturing.
  * You can use your own plants in your adventures, but only until you delete the Pollation.package file in the AppData\Spore folder.
  * Don't raise the trunks too high/low from the root, otherwise the game may crash.
  * I recommend you use ForceSave mod or this instruction: https://davoonline.com/phpBB3/viewtopic.php?t=4912
  * If you want to delete your plant, then rename the file !FE_eDELETEPLANTS to !FE_dDELETEPLANTS (for the second file you need FORCESAVE). This way, the file will be read first by the game in alphabetical order, and the file to be saved will be the last. Then resave the creation you want to delete and you can delete it. IMPORTANT AFTER THIS, RENAME THE FILE BACK TO DELETE, OTHERWISE THE PLANTS WILL NOT BE DISPLAYED CORRECTLY.
  * There are some problems with trunks after they are raised above the body:
1. If the top of a plant's trunk points straight up and you try to lengthen it, then at some point the lengthened trunk will simply disappear. Just place the top of the trunk at an angle.
2. For editors, there are variables that determine how many polygons will be rendered for the body. After being picked up, these variables no longer work on the trunks, and they are rendered in a better quality, which is why the game may stutter, and a huge number of such trunks can lead to the disappearance of all the trunks of the plant. To fix this a little, you need to lower the quality of shadows and lighting and increase the Graphic Cache Size.

# Special Thanks
* [emd4600](https://github.com/emd4600): For the article "Tutorial: Creating custom shaders for Spore", thanks to which, using the "FragmentCompilation" variable, I was able to accidentally fix this editor and started to development the mod.
* [Splitwirez](https://github.com/Splitwirez): conributor, gave ideas for editor. The mod was based of his "Branching Evolution" mod;
* [Auntie Owl](https://github.com/plencka): gaves access to her own version of flora editor mod;
* [VanillaCold](https://github.com/VanillaCold): for ideas;
* [Delta Unit](https://github.com/TheDeltaUnit): conributor, made a music "Branching Out" for flora editor;
* [Valla-chan](https://github.com/Valla-chan): contributor, adds names, part and background effects for main flora editor;
* [0KepOnline](https://github.com/0KepOnline): Thanks to his PNG-creation decompiler, I was able to reconstruct the fragments more accurately before pre-release game leaking.
