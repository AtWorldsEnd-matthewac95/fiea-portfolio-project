#include "gamescene.h"

namespace AWE {
    // GameSceneInfo

    GameSceneInfo::GameSceneInfo() : _background(GameTextureType::WOODS) {}
    GameSceneInfo::GameSceneInfo(GameTextureType background, AWEMusicInfo music) : _background(background), _music(std::move(music)) {}

    GameTextureType GameSceneInfo::background() const { return _background; }
    const std::vector<GameTextureType>& GameSceneInfo::sprites() const { return _sprites; }
    const AWEMusicInfo& GameSceneInfo::music() const { return _music; }

    std::vector<GameTextureType>* GameSceneInfo::sprites() { return &_sprites; }

    GameTextureType GameSceneInfo::background(GameTextureType newval) { GameTextureType oldval = _background; _background = newval; return oldval; }
    AWEMusicInfo GameSceneInfo::music(AWEMusicInfo newval) { AWEMusicInfo oldval = std::move(_music); _music = std::move(newval); return oldval; }


    // GameSceneTransitioner

    GameSceneTransitioner::GameSceneTransitioner(GameSFMLStorage& sfmls) : _sfmls(&sfmls), _isInitialized(false), _index(0) {}

    unsigned int GameSceneTransitioner::index() const { return _index; }
    bool GameSceneTransitioner::isInitialized() const { return _isInitialized; }

    GameSceneInfo* GameSceneTransitioner::GetCurrent() { return GetScene(_index); }
    GameSceneInfo* GameSceneTransitioner::GetScene(unsigned int index) { return index < _scenes.size() ? &_scenes.at(index) : nullptr; }
    unsigned int GameSceneTransitioner::AddScene(GameSceneInfo scene) { unsigned int index = _scenes.size(); _scenes.push_back(scene); return index; }
    bool GameSceneTransitioner::RemoveScene(unsigned int index) { GameSceneInfo _; return RemoveScene(index, _); }
    bool GameSceneTransitioner::RemoveScene(unsigned int index, GameSceneInfo& removed) {
        if (index < _scenes.size()) {
            removed = _scenes.at(index);
            GameScenes::const_iterator itr = _scenes.begin();
            std::advance(itr, index);
            _scenes.erase(itr);

            if (_scenes.empty()) {
                _index = 0;
                _isInitialized = false;
            } else if (index <= _index) {
                if (index == index) {
                    _isInitialized = false;
                }
                _index--;
            }

            return true;
        }
        return false;
    }

    void GameSceneTransitioner::Initialize() {
        if (_isInitialized || _scenes.empty()) {
            return;
        }

        InitializeScene();
        _isInitialized = true;
    }

    bool GameSceneTransitioner::GoToNextScene() {
        if (_scenes.empty()) {
            return false;
        }

        if (!_isInitialized) {
            Initialize();
            return true;
        }

        if ((_index + 1) < _scenes.size()) {
            _index++;
            InitializeScene();
            return true;
        }

        return false;
    }

    bool GameSceneTransitioner::GoToPreviousScene() {
        if (_scenes.empty()) {
            return false;
        }

        if (_index > 0) {
            _index--;
            InitializeScene();
            _isInitialized = true;
            return true;
        }

        return false;
    }

    void GameSceneTransitioner::InitializeScene() {
        GameSceneInfo* current = GetCurrent();
        if (current) {
            _sfmls->SetSpritesVisible(*current->sprites(), true);
        }
    }
}
