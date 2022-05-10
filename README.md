# fiea-portfolio-project
My submission for a FIEA portfolio project

Please ensure Windows SDK 10, Visual C++ 20 (at least), and Microsoft Visual Studio 2019 are installed in order for this project to build.

The project is currently configured to build for x64 platforms.
x86/Win32 is also possible, all that's needed is to copy the file `openal32.dll` from `external/openal-x32` and to paste it into the same directory as the project file, replacing the current `openal32.dll`. In order to restore x64, simply revert your changes in git or copy the corresponding file from `external/openal-x64`.

Github collaborators @Guillebarq and @Ryanhb helped test that this project would build on their computers. They did not contribute code.

None of the files in the external folder were created by the author of this project. See below, also see the comment header at the top of those files.

Third party libraries used:
* SFML - https://www.sfml-dev.org/documentation/2.5.1/
* timyxml2 - https://github.com/leethomason/tinyxml2

"AWE" is the name of the namespace into which I put everything I wrote.

Art & music credits:
* Romancing SaGa 3 - Minotaur, "Eppler" (Knight character), "Remi" (Magic character)
* Romancing SaGa 2 - "Lord Thomas" (Swordsman), 3rd song when fighting the swordsman (7 heroes battle)
* Final Fantasy 7 - Ability sprites, all sound effects
* Final Fantasy 4 - 1st song when fighting the minotaur (Battle 2)
* Dark Souls 1 - 2nd song when upgrading characters (A Moment's Peace)
* Lunar 2 Eternal Blue - All battle backgrounds
