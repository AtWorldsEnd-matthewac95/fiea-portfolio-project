#pragma once
#include <string>
#include <vector>
#include "../store/gamesfmlstorage.h"

namespace AWE {
    struct AWEMusicInfo {
    private:
        std::string _filename;
        unsigned int _loopstart;
        unsigned int _loopdur;
        unsigned int _startoffset;

    public:
        AWEMusicInfo() : _filename(""), _loopstart(0U), _loopdur(0U), _startoffset(0U) {}
        AWEMusicInfo(std::string filename, unsigned int loopstart, unsigned int loopdur) : _filename(filename), _loopstart(loopstart), _loopdur(loopdur), _startoffset(0U) {}
        AWEMusicInfo(std::string filename, unsigned int loopstart, unsigned int loopdur, unsigned int startoffset)
            : _filename(filename), _loopstart(loopstart), _loopdur(loopdur), _startoffset(startoffset) {}

        const std::string& filename() const { return _filename; }
        unsigned int loopstart() const { return _loopstart; }
        unsigned int loopdur() const { return _loopdur; }
        unsigned int startoffset() const { return _startoffset; }
    };

    class GameSceneInfo {
    private:
        GameTextureType _background;
        std::vector<GameTextureType> _sprites;
        AWEMusicInfo _music;

    public:
        GameSceneInfo();
        GameSceneInfo(GameTextureType background, AWEMusicInfo);

        GameTextureType background() const;
        const std::vector<GameTextureType>& sprites() const;
        const AWEMusicInfo& music() const;

        std::vector<GameTextureType>* sprites();

        GameTextureType background(GameTextureType);
        AWEMusicInfo music(AWEMusicInfo);
    };

    typedef std::vector<GameSceneInfo> GameScenes;

    class GameSceneTransitioner {
    private:
        GameSFMLStorage* _sfmls;
        GameScenes _scenes;
        unsigned int _index;
        bool _isInitialized;

        void InitializeScene();

    public:
        GameSceneTransitioner(GameSFMLStorage&);

        unsigned int index() const;
        bool isInitialized() const;

        GameSceneInfo* GetCurrent();
        GameSceneInfo* GetScene(unsigned int);
        unsigned int AddScene(GameSceneInfo);
        bool RemoveScene(unsigned int);
        bool RemoveScene(unsigned int, GameSceneInfo& removed);

        void Initialize();
        bool GoToNextScene();
        bool GoToPreviousScene();
    };
}
