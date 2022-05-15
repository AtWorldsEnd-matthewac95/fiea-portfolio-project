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
    /// <summary>
    /// Enumerates all of the current sounds available to the game engine. These values have no XML equivalent and are simply meant for ease of use to programmers.
    /// </summary>
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

    /// <summary>
    /// Enumerates all of the current textures available to the game engine. These values do have XML equivalents! (Which is why these are unsigned ints rather than shorts.)
    /// Take that into consideration when new values are added.
    /// </summary>
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

    /// <summary>
    /// Enumerates all of the current text box types designed. These are not binding to anything and are mostly used for ease-of-use and organizational purposes.
    /// </summary>
    enum class GameTextboxType : unsigned short {
        DAMAGE,
        GENERIC,
        SKILL
    };

    /// <summary>
    /// Stores objects loaded using SFML as well as AWE analogues to SFML objects. Also provides several utility functions to properly interact with these objects.
    /// </summary>
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
