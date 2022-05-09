#pragma once
#include <map>
#include <unordered_map>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include "../models/battler.h"
#include "../sfml/battlemenu.h"
#include "../sfml/awesprite.h"
#include "../sfml/textbox.h"

namespace AWE {
    enum class GameSoundType : unsigned short {
        CURSOR,
        FIRE,
        MAGIC,
        PEPSI,
        SAVE,
        SKITTER,
        SLASH,
        WIND
    };

    enum class GameTextureType : unsigned int {
        ENEMY,
        EPPLER,
        FIRETOWN,
        LEVELUP_EPPLER,
        LEVELUP_REMI,
        REMI,
        SKILL,
        TEMPLE,
        WOODS
    };

    enum class GameTextboxType : unsigned short {
        DAMAGE,
        GENERIC,
        SKILL
    };

    class GameSFMLStorage {
    private:
        static const std::string DEFAULT_RESOURCES_LOCATION;

        bool _loadFailed;

        std::unique_ptr<BattleMenu> _battleMenu;
        std::unordered_map<GameTextureType, sf::Sprite> _backgrounds;
        sf::RectangleShape _fadebox;
        sf::Font _font;
        sf::Music _music;
        std::unordered_map<GameSoundType, std::string> _soundnames;
        std::map<std::string, sf::SoundBuffer> _sounds;
        std::map<unsigned int, AWESprite> _sprites;
        std::unordered_map<GameTextboxType, TextBox> _textboxes;
        std::unordered_map<GameTextureType, sf::Texture> _textures;

        AWESpriteType TxrTypToSprTyp(GameTextureType) const;

    public:
        GameSFMLStorage(std::string resourcesLocation = DEFAULT_RESOURCES_LOCATION);

        bool loadFailed() const;

        const BattleMenu& battleMenu() const;
        const std::unordered_map<GameTextureType, sf::Sprite>& backgrounds() const;
        const sf::RectangleShape& fadebox() const;
        const sf::Font& font() const;
        const sf::Music& music() const;
        const std::map<std::string, sf::SoundBuffer>& sounds() const;
        const std::map<unsigned int, AWESprite>& sprites() const;
        const std::unordered_map<GameTextboxType, TextBox>& textboxes() const;
        const std::unordered_map<GameTextureType, sf::Texture>& textures() const;

        BattleMenu* battleMenu();
        sf::RectangleShape* fadebox();
        std::map<unsigned int, AWESprite>* sprites();
        std::unordered_map<GameTextboxType, TextBox>* textboxes();

        const sf::SoundBuffer* GetSound(GameSoundType) const;
        const AWESprite* GetSprite(GameTextureType) const;

        AWESprite* GetSprite(GameTextureType);

        void DrawBackground(sf::RenderWindow&, GameTextureType) const;
        void DrawBattleMenu(sf::RenderWindow&) const;
        void DrawBattleMenu(sf::RenderWindow&, const BattleMenu&) const;
        void DrawAWESprite(sf::RenderWindow&, GameTextureType) const;
        void DrawAWESprite(sf::RenderWindow&, const AWESprite&) const;
        void DrawTextBox(sf::RenderWindow&, GameTextboxType) const;
        void DrawTextBox(sf::RenderWindow&, const TextBox&, bool ignoreIsVisible = false) const;
        bool PlayMusic(const std::string& filename, unsigned int loopstart = 0U, unsigned int loopdur = 0U, unsigned int startoffset = 0U, const std::string& location = DEFAULT_RESOURCES_LOCATION);
        bool StopMusic();

        std::vector<GameTextureType> SetSpritesVisible(const std::vector<GameTextureType>& spritesToSetVisible, bool setAllOthersInvisible = false);
    };
}
