# HazelDash
A 2D demo game project made with the [Hazel engine](https://github.com/TheCherno/Hazel) and based on the popular [Boulder Dash](https://boulder-dash.com) game from the 1980's.

As featured in episode 65 of [TheCherno's Game Engine series](https://www.youtube.com/playlist?list=PLlrATfBNZ98dC-V-N3m0Go4deliWHPFwT).  Check it out if you haven't already.

This branch uses Hazel code as it was at episode 65 of the series.

Sharp eyed people may notice that the graphics and levels are different here to the ones featured in the youtube clip.  Thanks to iHelp (https://github.com/dovker) for this version of the  graphics.

If you wish for a more authentic retro "Boulder Dash" look, then you can find the original assets on the internet and it is a simple matter to incorporate them into a fork of this code. 

There are many BoulderDash clones out there, and most of them are far better than this one.  The goal of this repository is not to make the perfect BoulderDash clone, but rather just to make something, for educational purposes, using the Hazel game engine.  Check out the series if you havent already - [TheCherno's Game Engine series](https://www.youtube.com/playlist?list=PLlrATfBNZ98dC-V-N3m0Go4deliWHPFwT).

In particular, I wanted something to show off the "batch renderer" developed in episodes 61 to 64.


## Repository Structure
This is the Game-Engine-Series-Episode-65 branch of the repository.
Refer README.md in the master branch for details of what the different branches are about.

## Build
Only Visual Studio 2019 on Windows has been tested.

* Clone the repository with `git clone --recursive https://github.com/Freeman40/HazelDash`
* Run the script `VS2019-GenProjects.bat` from the Build subdirectory
* Open the generated VS2019 solution file from the Build/VS2019 subdirectory.  Build project.
