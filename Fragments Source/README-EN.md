# Flora fragments
These folders contain source models of release version fragments and fragment models taken from plant models. Use them to create your own fragments or to recreate fragments that are not yet deformed.

P.S: I will immediately answer the question that may arise at the first acquaintance with my release versions of fragments: I had and have no experience in animating modelling and did everything on pure enthusiasm, so the skeletons of animations of fragments look.... like this. Yanderedev in the modelling world lol

# To work with fragments you will need:
1. SporeModderFX (yes) https://github.com/emd4600/SporeModder-FX/releases
2. Spore Blender addon https://github.com/emd4600/SporeModder-Blender-Addons/releases
	*Tutorials on how to work with SMFX and addon: https://github.com/emd4600/SporeModder-FX/wiki

Also blender addons that might be useful:
3. Magic UV (for copying UV-unwrapping of fragments)
4. Corrective Shape Keys (to use source models as shape keys)
These addons can be connected in Preferences -> Add-ons

# Designations
-release — fragments with ready-made animations and UV-unwrapping, which are used in the release version.
(not release)/-nr — fragments, which has no animations except placeholder animations, but has a ready-made UV unwrapping.
-models — models sources for working on animations of fragments or using to create your own fragments.
(full recon) — fragments that have been recreated from scratch or based on some other fragments.
(need fix) — fragments that need to be improved or corrected in the animation. Inside the fragment folders there are notes that describe exactly what the problem is.

# There should have been something else written here, but I forgot. X_X

# How to get a fragment from a creation model?
1. Make a project with files from the GETFRAGMENTS3000 folder.
2. Write boolProp disableValidation true in ConfigManager and property disableValidation 0x055d7ca1 bool in Properties.
3. Check this video: https://youtu.be/8uKc4hSvaBY
	*0.000001 -state:BuildingEditor
4. PROFIT
