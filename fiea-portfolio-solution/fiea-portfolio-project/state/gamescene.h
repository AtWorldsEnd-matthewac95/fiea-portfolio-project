#pragma once
#include <string>
#include <vector>
#include "../store/gamesfmlstorage.h"

namespace AWE {
    /// <summary>
    /// Information about music. Meant to be used in conjunction with sf::Music from SFML.
    /// </summary>
    struct AWEMusicInfo {
    private:
        std::string _filename;
        unsigned int _loopstart;
        unsigned int _loopdur;
        unsigned int _startoffset;

    public:
        /// <summary>
        /// Default constructor. Initializes filename to the empty string and all `int`s to 0. This will essentially be an invalid object.
        /// </summary>
        AWEMusicInfo() : _filename(""), _loopstart(0U), _loopdur(0U), _startoffset(0U) {}
        /// <param name="loopstart">Time the loop should start in milliseconds. The song will play normally up to this point.</param>
        /// <param name="loopdur">Duration of the loop in milliseconds.</param>
        AWEMusicInfo(std::string filename, unsigned int loopstart, unsigned int loopdur) : _filename(filename), _loopstart(loopstart), _loopdur(loopdur), _startoffset(0U) {}
        /// <param name="loopstart">Time the loop should start in milliseconds. The song will play normally up to this point.</param>
        /// <param name="loopdur">Duration of the loop in milliseconds.</param>
        /// <param name="startoffset">Offset in milliseconds from the beginning of the file from which the track should start playing.</param>
        AWEMusicInfo(std::string filename, unsigned int loopstart, unsigned int loopdur, unsigned int startoffset)
            : _filename(filename), _loopstart(loopstart), _loopdur(loopdur), _startoffset(startoffset) {}

        /// <returns>const reference to the filename string.</returns>
        const std::string& filename() const { return _filename; }
        /// <returns>Time the loop should start in milliseconds. The song will play normally up to this point.</returns>
        unsigned int loopstart() const { return _loopstart; }
        /// <returns>Duration of the loop in milliseconds.</returns>
        unsigned int loopdur() const { return _loopdur; }
        /// <returns>Offset in milliseconds from the beginning of the file from which the track should start playing.</returns>
        unsigned int startoffset() const { return _startoffset; }
    };

    /// <summary>
    /// Represents information about a "scene". Scenes can also be thought of as "levels", "stages", or "screens".
    /// </summary>
    class GameSceneInfo {
    private:
        GameTextureType _background;
        std::vector<GameTextureType> _sprites;
        AWEMusicInfo _music;

    public:
        GameSceneInfo();
        /// <summary>
        /// Constructor. To add sprites, use the mutable sprites pointer.
        /// </summary>
        GameSceneInfo(GameTextureType background, AWEMusicInfo);

        /// <returns>Current background texture type.</returns>
        GameTextureType background() const;
        /// <returns>const reference to the list of sprites which should be set to visible when this scene is initialized.</returns>
        const std::vector<GameTextureType>& sprites() const;
        /// <returns>const reference to the current music info.</returns>
        const AWEMusicInfo& music() const;

        /// <returns>Mutable pointer to the list of sprites which should be set to visible when this scene is initialized.</returns>
        std::vector<GameTextureType>* sprites();

        /// <param name="">New value for the current background texture type.</param>
        /// <returns>Old value for the background texture type.</returns>
        GameTextureType background(GameTextureType);
        /// <param name="">New value for the current music info.</param>
        /// <returns>Old value for the music info.</returns>
        AWEMusicInfo music(AWEMusicInfo);
    };

    typedef std::vector<GameSceneInfo> GameScenes;

    /// <summary>
    /// Object which handles proper transition between scenes.
    /// </summary>
    class GameSceneTransitioner {
    private:
        GameSFMLStorage* _sfmls;
        GameScenes _scenes;
        unsigned int _index;
        bool _isInitialized;

        /// <summary>
        /// Initializes the current scene. Does not set the initialized flag, because the transitioner itself may already be initialized.
        /// </summary>
        void InitializeScene();

    public:
        GameSceneTransitioner(GameSFMLStorage&);

        /// <returns>Index of the current scene.</returns>
        unsigned int index() const;
        /// <returns>Is the scene transitioner initialized? This is necessary because the first scene may not be initialized by default.</returns>
        bool isInitialized() const;

        /// <returns>Returns the current scene, or nullptr if there is no current scene.</returns>
        GameSceneInfo* GetCurrent();
        /// <returns>Returns the scene with the given index, or nullptr if that index does not correspond to a scene.</returns>
        GameSceneInfo* GetScene(unsigned int);
        /// <param name="">New scene to add to the transitioner.</param>
        /// <returns>Index of the added scene.</returns>
        unsigned int AddScene(GameSceneInfo);
        /// <param name="">Index of the scene to remove.</param>
        /// <returns>Whether the scene was removed successfully. If the index does not currently correspond to a scene, this will return false.</returns>
        bool RemoveScene(unsigned int);
        /// <param name="">Index of the scene to remove.</param>
        /// <param name="removed">If a scene is removed, this ref will have that scenes information.</param>
        /// <returns>Whether the scene was removed successfully. If the index does not currently correspond to a scene, this will return false.</returns>
        bool RemoveScene(unsigned int, GameSceneInfo& removed);

        /// <summary>
        /// Initializes the scene transitioner.
        /// </summary>
        void Initialize();
        /// <summary>
        /// Attempts to transition to the next scene.
        /// </summary>
        /// <returns>Whether the operation was successful.</returns>
        bool GoToNextScene();
        /// <summary>
        /// Attempts to transition to the previous scene.
        /// </summary>
        /// <returns>Whether the operation was successful.</returns>
        bool GoToPreviousScene();
    };
}
