#include "gamesfmlstorage.h"
#include <iostream>

namespace AWE {
    const std::string GameSFMLStorage::DEFAULT_RESOURCES_LOCATION = "res";

    AWESpriteType GameSFMLStorage::TxrTypToSprTyp(GameTextureType val) const {
        auto ret = AWESpriteType::BACKGROUND;

        switch (val) {
        case GameTextureType::ENEMY:
            ret = AWESpriteType::ENEMY;
            break;

        case GameTextureType::EPPLER:
            ret = AWESpriteType::CHARACTER;
            break;

        case GameTextureType::REMI:
            ret = AWESpriteType::CHARACTER;
            break;

        case GameTextureType::SKILL:
            ret = AWESpriteType::SKILL;
            break;
        }

        return ret;
    }

    bool GameSFMLStorage::loadFailed() const { return _loadFailed; }

    const BattleMenu& GameSFMLStorage::battleMenu() const { return *_battleMenu; }
    const std::unordered_map<GameTextureType, sf::Sprite>& GameSFMLStorage::backgrounds() const { return _backgrounds; }
    const sf::RectangleShape& GameSFMLStorage::fadebox() const { return _fadebox; }
    const sf::Font& GameSFMLStorage::font() const { return _font; }
    const sf::Music& GameSFMLStorage::music() const { return _music; }
    const std::map<std::string, sf::SoundBuffer>& GameSFMLStorage::sounds() const { return _sounds; }
    const std::map<unsigned int, AWESprite>& GameSFMLStorage::sprites() const { return _sprites; }
    const std::unordered_map<GameTextboxType, TextBox>& GameSFMLStorage::textboxes() const { return _textboxes; }
    const std::unordered_map<GameTextureType, sf::Texture>& GameSFMLStorage::textures() const { return _textures; }

    BattleMenu* GameSFMLStorage::battleMenu() { return _battleMenu.get(); }
    sf::RectangleShape* GameSFMLStorage::fadebox() { return &_fadebox; }
    std::map<unsigned int, AWESprite>* GameSFMLStorage::sprites() { return &_sprites; }
    std::unordered_map<GameTextboxType, TextBox>* GameSFMLStorage::textboxes() { return &_textboxes; }

    const sf::SoundBuffer* GameSFMLStorage::GetSound(GameSoundType soundtype) const {
        const sf::SoundBuffer* sound = nullptr;

        auto namefound = _soundnames.find(soundtype);
        if (namefound != _soundnames.end()) {
            auto soundfound = _sounds.find(namefound->second);
            if (soundfound != _sounds.end()) {
                sound = &soundfound->second;
            }
        }

        return sound;
    }

    const AWESprite* GameSFMLStorage::GetSprite(GameTextureType txrtype) const {
        const AWESprite* sprite = nullptr;

        auto found = _sprites.find(static_cast<unsigned int>(txrtype));
        if (found != _sprites.end()) {
            sprite = &found->second;
        }

        return sprite;
    }

    AWESprite* GameSFMLStorage::GetSprite(GameTextureType txrtype) {
        AWESprite* sprite = nullptr;

        auto found = _sprites.find(static_cast<unsigned int>(txrtype));
        if (found != _sprites.end()) {
            sprite = &found->second;
        }

        return sprite;
    }

    void GameSFMLStorage::DrawBackground(sf::RenderWindow& window, GameTextureType txrtype) const {
        auto found = _backgrounds.find(txrtype);
        if (found != _backgrounds.end()) {
            window.draw(found->second);
        }
    }

    void GameSFMLStorage::DrawBattleMenu(sf::RenderWindow& window) const { DrawBattleMenu(window, *_battleMenu); }
    void GameSFMLStorage::DrawBattleMenu(sf::RenderWindow& window, const BattleMenu& menu) const {
        if (menu.isVisible()) {
            window.draw(menu.background());
            DrawTextBox(window, menu.skillTextBox(), true);
            DrawTextBox(window, menu.characterTextBox(), true);

            if (menu.skillSelector().isVisible()) {
                window.draw(menu.skillSelector().rect());
            }

            if (menu.characterSelector().isVisible()) {
                window.draw(menu.characterSelector().rect());
            }
        }
    }

    void GameSFMLStorage::DrawAWESprite(sf::RenderWindow& window, GameTextureType txrtype) const {
        auto found = _sprites.find(static_cast<unsigned int>(txrtype));
        if (found != _sprites.end()) {
            DrawAWESprite(window, found->second);
        }
    }

    void GameSFMLStorage::DrawAWESprite(sf::RenderWindow& window, const AWESprite& sprite) const {
        if (sprite.isVisible()) {
            window.draw(sprite.sprite());
        }
    }

    void GameSFMLStorage::DrawTextBox(sf::RenderWindow& window, GameTextboxType textboxtype) const {
        auto found = _textboxes.find(textboxtype);
        if (found != _textboxes.end()) {
            DrawTextBox(window, found->second);
        }
    }

    void GameSFMLStorage::DrawTextBox(sf::RenderWindow& window, const TextBox& textbox, bool ignoreIsVisible) const {
        if (ignoreIsVisible || textbox.isVisible()) {
            window.draw(textbox.box());
            window.draw(textbox.text());
        }
    }

    bool GameSFMLStorage::PlayMusic(const std::string& filename, unsigned int loopstart, unsigned int loopdur, unsigned int startoffset, const std::string& location) {
        if (_music.getStatus() == sf::SoundSource::Status::Playing) {
            return false;
        }

        if (!_music.openFromFile(location + "/" + filename)) {
            return false;
        }

        if (loopdur > 0U) {
            _music.setLoopPoints(sf::Music::TimeSpan(sf::milliseconds(loopstart), sf::milliseconds(loopdur)));
        }

        _music.play();

        if (startoffset > 0U) {
            _music.setPlayingOffset(sf::milliseconds(startoffset));
        }

        return true;
    }

    bool GameSFMLStorage::StopMusic() {
        if (_music.getStatus() != sf::SoundSource::Status::Playing && _music.getStatus() != sf::SoundSource::Status::Paused) {
            return false;
        }

        _music.stop();
        return true;
    }

    std::vector<GameTextureType> GameSFMLStorage::SetSpritesVisible(const std::vector<GameTextureType>& spritesToSetVisible, bool setAllOthersInvisible) {
        std::vector<GameTextureType> invalidSprites;
        std::unordered_map<unsigned int, bool> results;

        if (setAllOthersInvisible) {
            for (const std::map<unsigned int, AWESprite>::value_type& sprite : _sprites) {
                results.insert(std::make_pair(sprite.first, false));
            }
        }

        for (const GameTextureType tx : spritesToSetVisible) {
            auto i = static_cast<unsigned int>(tx);
            auto found = _sprites.find(i);
            if (found == _sprites.end()) {
                invalidSprites.push_back(tx);
            } else {
                auto result = results.find(i);
                if (result == results.end()) {
                    result = results.insert(results.begin(), std::pair(i, true));
                }
                result->second = true;
            }
        }

        for (std::unordered_map<unsigned int, bool>::value_type& result : results) {
            _sprites.at(result.first).isVisible(result.second);
        }

        return invalidSprites;
    }


    /* Ctor */

    GameSFMLStorage::GameSFMLStorage(std::string resourcesLocation) : _loadFailed(false) {

        /* Font load */

        if (!_font.loadFromFile("res/novem___.ttf")) {
            std::cout << "Font load failed.\n";
            _loadFailed = true;
        }


        /* Music settings */

        _music.setVolume(80.f);
        _music.setLoop(true);


        /* Fade box load */

        sf::Color black = sf::Color::Black, blue = sf::Color::Blue, white = sf::Color::White, yellow = sf::Color::Yellow;
        _fadebox.setSize(sf::Vector2f(AWESprite::WIDTH_BACKGROUND, AWESprite::HEIGHT_BACKGROUND));
        _fadebox.setFillColor(black);


        /* Textboxes load */

        std::unordered_map<GameTextboxType, TextBox>::iterator textbox;

        textbox = _textboxes.insert(_textboxes.begin(), std::make_pair(GameTextboxType::DAMAGE, AWE::TextBox(AWE::TextBoxVisuals(_font, 2u, white, black, 4U, 8U))));
        textbox->second.isVisible(false);
        textbox = _textboxes.insert(_textboxes.begin(), std::make_pair(GameTextboxType::GENERIC, AWE::TextBox(AWE::TextBoxVisuals(_font, 2u, white, black, 4U, 8U))));
        textbox->second.SetPosition(sf::Vector2f(140.f, 100.f));
        textbox->second.isVisible(false);
        textbox = _textboxes.insert(_textboxes.begin(), std::make_pair(GameTextboxType::SKILL, AWE::TextBox(_font, 4u, yellow, black, "", 128u)));
        textbox->second.SetPosition(sf::Vector2f(100.f, 5.f));
        textbox->second.isVisible(false);


        /* Sounds & sound names load */

        std::unordered_map<GameSoundType, std::string>::iterator soundname;
        std::map<std::string, sf::SoundBuffer>::iterator sound;

        soundname = _soundnames.insert(_soundnames.begin(), std::make_pair(GameSoundType::CURSOR, "cursor.wav"));
        sound = _sounds.insert(_sounds.begin(), std::make_pair(soundname->second, sf::SoundBuffer()));
        if (!sound->second.loadFromFile(resourcesLocation + "/" + soundname->second)) {
            std::cout << "Cursor sound load failed.\n";
            _loadFailed = true;
        }

        soundname = _soundnames.insert(_soundnames.begin(), std::make_pair(GameSoundType::FIRE, "fire.wav"));
        sound = _sounds.insert(_sounds.begin(), std::make_pair(soundname->second, sf::SoundBuffer()));
        if (!sound->second.loadFromFile(resourcesLocation + "/" + soundname->second)) {
            std::cout << "Fire sound load failed.\n";
            _loadFailed = true;
        }

        soundname = _soundnames.insert(_soundnames.begin(), std::make_pair(GameSoundType::MAGIC, "magic.wav"));
        sound = _sounds.insert(_sounds.begin(), std::make_pair(soundname->second, sf::SoundBuffer()));
        if (!sound->second.loadFromFile(resourcesLocation + "/" + soundname->second)) {
            std::cout << "Magic sound load failed.\n";
            _loadFailed = true;
        }

        soundname = _soundnames.insert(_soundnames.begin(), std::make_pair(GameSoundType::PEPSI, "pepsi.wav"));
        sound = _sounds.insert(_sounds.begin(), std::make_pair(soundname->second, sf::SoundBuffer()));
        if (!sound->second.loadFromFile(resourcesLocation + "/" + soundname->second)) {
            std::cout << "Enemy defeat sound load failed.\n";
            _loadFailed = true;
        }

        soundname = _soundnames.insert(_soundnames.begin(), std::make_pair(GameSoundType::SAVE, "save.wav"));
        sound = _sounds.insert(_sounds.begin(), std::make_pair(soundname->second, sf::SoundBuffer()));
        if (!sound->second.loadFromFile(resourcesLocation + "/" + soundname->second)) {
            std::cout << "Save sound load failed.\n";
            _loadFailed = true;
        }

        soundname = _soundnames.insert(_soundnames.begin(), std::make_pair(GameSoundType::SKITTER, "skitter.wav"));
        sound = _sounds.insert(_sounds.begin(), std::make_pair(soundname->second, sf::SoundBuffer()));
        if (!sound->second.loadFromFile(resourcesLocation + "/" + soundname->second)) {
            std::cout << "Skitter sound load failed.\n";
            _loadFailed = true;
        }

        soundname = _soundnames.insert(_soundnames.begin(), std::make_pair(GameSoundType::SLASH, "slash.wav"));
        sound = _sounds.insert(_sounds.begin(), std::make_pair(soundname->second, sf::SoundBuffer()));
        if (!sound->second.loadFromFile(resourcesLocation + "/" + soundname->second)) {
            std::cout << "Slash sound load failed.\n";
            _loadFailed = true;
        }

        soundname = _soundnames.insert(_soundnames.begin(), std::make_pair(GameSoundType::WIND, "wind.wav"));
        sound = _sounds.insert(_sounds.begin(), std::make_pair(soundname->second, sf::SoundBuffer()));
        if (!sound->second.loadFromFile(resourcesLocation + "/" + soundname->second)) {
            std::cout << "Wind sound load failed.\n";
            _loadFailed = true;
        }


        /* Textures, battler sprites, backgrounds, & battle menu load */

        std::unordered_map<GameTextureType, sf::Texture>::iterator texture;
        std::map<unsigned int, AWESprite>::iterator sprite;
        std::unordered_map<GameTextureType, sf::Sprite>::iterator background;

        texture = _textures.insert(_textures.begin(), std::make_pair(GameTextureType::ENEMY, sf::Texture()));
        if (!texture->second.loadFromFile(resourcesLocation + "/enemies.png")) {
            std::cout << "Enemies load failed.\n";
            _loadFailed = true;
        } else {
            sprite = _sprites.insert(_sprites.begin(), std::make_pair(static_cast<unsigned int>(texture->first), AWESprite(texture->second, TxrTypToSprTyp(texture->first), 0U, 24.f, 16.f)));
        }

        texture = _textures.insert(_textures.begin(), std::make_pair(GameTextureType::EPPLER, sf::Texture()));
        if (!texture->second.loadFromFile(resourcesLocation + "/eppler.png")) {
            std::cout << "Male battler load failed.\n";
            _loadFailed = true;
        } else {
            sprite = _sprites.insert(_sprites.begin(), std::make_pair(static_cast<unsigned int>(texture->first), AWESprite(texture->second, TxrTypToSprTyp(texture->first), 0U, 224.f, 64.f)));
        }

        texture = _textures.insert(_textures.begin(), std::make_pair(GameTextureType::FIRETOWN, sf::Texture()));
        if (!texture->second.loadFromFile(resourcesLocation + "/final.png")) {
            std::cout << "Fire town background load failed.\n";
            _loadFailed = true;
        } else {
            background = _backgrounds.insert(_backgrounds.begin(), std::make_pair(GameTextureType::FIRETOWN, sf::Sprite()));
            background->second.setTexture(texture->second);
        }

        texture = _textures.insert(_textures.begin(), std::make_pair(GameTextureType::LEVELUP_EPPLER, sf::Texture()));
        if (!texture->second.loadFromFile(resourcesLocation + "/levelup-eppler.png")) {
            std::cout << "Male battler level up screen load failed.\n";
            _loadFailed = true;
        } else {
            sprite = _sprites.insert(_sprites.begin(), std::make_pair(static_cast<unsigned int>(texture->first), AWESprite(texture->second, TxrTypToSprTyp(texture->first), 0)));
            sprite->second.isVisible(false);
        }

        texture = _textures.insert(_textures.begin(), std::make_pair(GameTextureType::LEVELUP_REMI, sf::Texture()));
        if (!texture->second.loadFromFile(resourcesLocation + "/levelup-remi.png")) {
            std::cout << "Female battler level up screen load failed.\n";
            _loadFailed = true;
        } else {
            sprite = _sprites.insert(_sprites.begin(), std::make_pair(static_cast<unsigned int>(texture->first), AWESprite(texture->second, TxrTypToSprTyp(texture->first), 0)));
            sprite->second.isVisible(false);
        }

        texture = _textures.insert(_textures.begin(), std::make_pair(GameTextureType::REMI, sf::Texture()));
        if (!texture->second.loadFromFile(resourcesLocation + "/remi.png")) {
            std::cout << "Female battler load failed.\n";
            _loadFailed = true;
        } else {
            sprite = _sprites.insert(_sprites.begin(), std::make_pair(static_cast<unsigned int>(texture->first), AWESprite(texture->second, TxrTypToSprTyp(texture->first), 0U, 240.f, 112.f)));
        }

        texture = _textures.insert(_textures.begin(), std::make_pair(GameTextureType::SKILL, sf::Texture()));
        if (!texture->second.loadFromFile(resourcesLocation + "/skills.png")) {
            std::cout << "Skills load failed.\n";
            _loadFailed = true;
        } else {
            sprite = _sprites.insert(_sprites.begin(), std::make_pair(static_cast<unsigned int>(texture->first), AWESprite(texture->second, TxrTypToSprTyp(texture->first), 0)));
            sprite->second.isVisible(false);
        }

        texture = _textures.insert(_textures.begin(), std::make_pair(GameTextureType::TEMPLE, sf::Texture()));
        if (!texture->second.loadFromFile(resourcesLocation + "/temple.png")) {
            std::cout << "Temple background load failed.\n";
            _loadFailed = true;
        } else {
            background = _backgrounds.insert(_backgrounds.begin(), std::make_pair(GameTextureType::TEMPLE, sf::Sprite()));
            background->second.setTexture(texture->second);
        }

        texture = _textures.insert(_textures.begin(), std::make_pair(GameTextureType::WOODS, sf::Texture()));
        if (!texture->second.loadFromFile(resourcesLocation + "/woods.png")) {
            std::cout << "Background load failed.\n";
            _loadFailed = true;
        } else {
            background = _backgrounds.insert(_backgrounds.begin(), std::make_pair(GameTextureType::WOODS, sf::Sprite()));
            background->second.setTexture(texture->second);


            // Battle menu load

            sf::Color cyan = sf::Color::Cyan, menucolor((black.r * 0.8f) + (blue.r * 0.2f), (black.g * 0.8f) + (blue.g * 0.2f), (black.b * 0.8f) + (blue.b * 0.2f));
            TextBoxVisuals battleMenuTextboxVisuals(_font, 4u, white, menucolor, 6U, 12U);
            _battleMenu = std::make_unique<BattleMenu>(BattleMenu(
                black,
                static_cast<unsigned int>(texture->second.getSize().x),
                static_cast<unsigned int>(texture->second.getSize().y) / 4U,
                4U,
                battleMenuTextboxVisuals,
                battleMenuTextboxVisuals,
                cyan,
                cyan,
                *GetSound(GameSoundType::CURSOR)
            ));
            _battleMenu->SetPosition(sf::Vector2f(0.f, 180.f));
            _battleMenu->isVisible(false);
        }
    }
}
