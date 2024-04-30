# Flora-Editor-Reconstruction
This mod is an attempt to reconstruction the Flora Editor and make it usable.
  * All fragments available on the editor pages are available, with the exception of their arrows for deformation.
  * Added and reconstructed skinpaint for Flora Editor.

# How to enter the editor and work in the editor
First, you need to add the following codes to the game Config folder.

In the Properties.txt file:
 property FragmentCompilation  0x05fb85a3           bool

In the ConfigManager.txt file:
 boolProp FragmentCompilation true

Thanks to this, plants will be displayed correctly in coloring mode and on the creation thumbnail, and the game will also correctly save their sprites.

After that you need to go to the editor. This command can be entered in the game shortcut, in Steam properties, etc:
-state:FloraEditor
You can also get to the flora editor by going to some other editor, for example, the creature editor, and then open the sporopedia in the editor and double-click on any plant to get to the flora editor.
In order to change the length of the trunks, you need to add two connected trunks on top of the body, then, holding down the "Ctrl" button, raise the first trunk above the body until the trunks disappear. Then do any action (for example, add a fragment or change the color of a plant) and cancel it.
After this, you will be free to deform the trunks as you wish, with the exception of the first joint of the first trunk. Otherwise, the game will crash.

# Notes
  * In new games or in the space stage, you may come across player plants, but their models are not preloaded into the game. To fix this, you need to download plant models by viewing them in the sporopedia and re-enter the game.
  * If you don't add the "gaprop" tag to your plants, then add a couple of fruits to them so that during your play in the creature and tribe stages you can collect fruits from them.
  * The size of the fruit fragment determines the size of the fruit itself in the game.
  * Try to use the trunks as little as possible, during texturing the trunks take up more than half of the texture space, which is why the fragments will look terrible at the end of texturing.
  * You can use your own plants in your adventures, but only until you delete the Pollation.package file in the AppData\Spore folder.
  * Don't raise the trunks too high/low from the root, otherwise the game may crash.
  * I recommend you use ForceSave mod or this instruction: https://davoonline.com/phpBB3/viewtopic.php?t=4912
  * If you want to delete your plant, then rename the file !FE_eDELETEPLANTS to !FE_dDELETEPLANTS. This way, the file will be read first by the game in alphabetical order, and the file to be saved will be the last. Then resave the creation you want to delete and you can delete it. IMPORTANT AFTER THIS, RENAME THE FILE BACK TO DELETE, OTHERWISE THE PLANTS WILL NOT BE DISPLAYED CORRECTLY.
  * There are some problems with trunks after they are raised above the body:
1. If the top of a plant's trunk points straight up and you try to lengthen it, then at some point the lengthened trunk will simply disappear. Just place the top of the trunk at an angle.
2. For editors, there are variables that determine how many polygons will be rendered for the body. After being picked up, these variables no longer work on the trunks, and they are rendered in a better quality, which is why the game may stutter, and a huge number of such trunks can lead to the disappearance of all the trunks of the plant. To fix this a little, you need to lower the quality of shadows and lighting and increase the Graphic Cache Size.

# Special Thanks
  * rob55rod: It all started with his mod "Branching Evolution"
  * emd: For the article "Tutorial: Creating custom shaders for Spore", thanks to which, using the "FragmentCompilation" variable, I was able to accidentally fix this editor.
  * 0keponline: Thanks to his PNG-creation decompiler, I was able to reconstruct the fragments more accurately.
  * Old community Spore Wiki -ru: I thank them for always being there for me (including 0kepOnline).
