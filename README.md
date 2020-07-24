# HazelDash
A 2D demo game project made with the [Hazel engine](https://github.com/TheCherno/Hazel) and based on the popular [Boulder Dash](https://boulder-dash.com) game from the 1980's.

As featured in episode 65 of [TheCherno's Game Engine series](https://www.youtube.com/playlist?list=PLlrATfBNZ98dC-V-N3m0Go4deliWHPFwT).  Check it out if you haven't already.

Sharp eyed people may notice that the graphics are different here to the ones featured in the youtube clip.  Thanks to iHelp (https://github.com/dovker) for this version of the  graphics.  If you wish for a more authentic retro "Boulder Dash" look, then you can find the original assets on the internet and it is a simple matter to incorporate them into a fork of this code. 

There are many BoulderDash clones out there, and most of them are far better than this one.  The goal of this repository is not to make the perfect BoulderDash clone, but rather just to make something, for educational purposes, demonstrating use of the Hazel game engine.  Check out the series if you havent already - [TheCherno's Game Engine series](https://www.youtube.com/playlist?list=PLlrATfBNZ98dC-V-N3m0Go4deliWHPFwT).



## Repository Structure
The following branches are active:
branch|description
------|-----------
|master|  The intention is to keep the master branch up to date with the current master branch of Hazel, and so as new features are added in to the Hazel Game Engine, I will update this game to demonstrate use of those features.  Currently using Hazel code as at episode 79
|audio|   This is basically the same as the master branch, except with sound effects.  It uses HazelAudio which is an additional plugin for the Hazel engine that is not (yet) covered in the Game Engine Series.
|Game-Engine-Series-Episode-65|  This branch uses Hazel code as was available at episode 65.  In particular, it makes use of the batch renderer.


## Build
Only Visual Studio 2019 on Windows has been tested.

* Clone the repository with `git clone --recursive https://github.com/Freeman40/HazelDash`
* Run the script `VS2019-GenProjects.bat` from the Build subdirectory
* Open the generated VS2019 solution file from the Build/VS2019 subdirectory.  Build project.
