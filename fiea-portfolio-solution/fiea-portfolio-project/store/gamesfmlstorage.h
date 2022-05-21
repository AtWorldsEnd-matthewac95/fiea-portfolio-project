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

        /// <summary>
        /// Converts GameTextureType to AWESpriteType.
        /// </summary>
        AWESpriteType TxrTypToSprTyp(GameTextureType) const;

    public:
        /// <summary>
        /// Constructor.
        /// </summary>
        /// <param name="resourcesLocation">Location of the resources folder from which to load objects.</param>
        GameSFMLStorage(std::string resourcesLocation = DEFAULT_RESOURCES_LOCATION);

        /// <returns>Did the load fail?</returns>
        bool loadFailed() const;

        /// <returns>const reference to the battle menu.</returns>
        const BattleMenu& battleMenu() const;
        /// <returns>const reference to a map of loaded background sprites.</returns>
        const std::unordered_map<GameTextureType, sf::Sprite>& backgrounds() const;
        /// <returns>const reference to the black box which creates the screen fade effect.</returns>
        const sf::RectangleShape& fadebox() const;
        /// <returns>const reference to the font used for text boxes and menus.</returns>
        const sf::Font& font() const;
        /// <returns>const reference to the music object.</returns>
        const sf::Music& music() const;
        /// <returns>const reference to a map of sound buffers indexed by filename. Recommend using GetSound instead due to ease of use.</returns>
        const std::map<std::string, sf::SoundBuffer>& sounds() const;
        /// <returns>const reference to a map of sprites indexed by texture type value. Recommend using GetSprite instead due to ease of use.</returns>
        const std::map<unsigned int, AWESprite>& sprites() const;
        /// <returns>const reference to a map of textboxes indexed by type.</returns>
        const std::unordered_map<GameTextboxType, TextBox>& textboxes() const;
        /// <returns>const reference to a map of textures indexed by type.</returns>
        const std::unordered_map<GameTextureType, sf::Texture>& textures() const;

        /// <returns>Mutable pointer to the battle menu.</returns>
        BattleMenu* battleMenu();
        /// <returns>Mutable pointer to the black box which creates the screen fade effect.</returns>
        sf::RectangleShape* fadebox();
        /// <returns>Mutable pointer to a map of sprites indexed by texture type value. Keys should match up with GameTextureType. Make sure you know what you're doing.</returns>
        std::map<unsigned int, AWESprite>* sprites();
        /// <returns>Mutable pointer to a map of textboxes indexed by type.</returns>
        std::unordered_map<GameTextboxType, TextBox>* textboxes();

        /// <returns>const pointer to the sound buffer that corresponds to the given sound type. Returns nullptr if no such sound buffer exists.</returns>
        const sf::SoundBuffer* GetSound(GameSoundType) const;
        /// <returns>const pointer to the sprite that corresponds to the given texture type. Returns nullptr if no such sprite exists.</returns>
        const AWESprite* GetSprite(GameTextureType) const;

        /// <returns>Pointer to the sprite that corresponds to the given texture type. Returns nullptr if no such sprite exists.</returns>
        AWESprite* GetSprite(GameTextureType);

        /// <summary>
        /// Properly draws the background of the given type if it exists.
        /// </summary>
        void DrawBackground(sf::RenderWindow&, GameTextureType) const;
        /// <summary>
        /// Properly draws the battle menu.
        /// </summary>
        void DrawBattleMenu(sf::RenderWindow&) const;
        /// <summary>
        /// Properly draws the given battle menu.
        /// </summary>
        void DrawBattleMenu(sf::RenderWindow&, const BattleMenu&) const;
        /// <summary>
        /// Properly draws the sprite of the given type if it exists.
        /// </summary>
        void DrawAWESprite(sf::RenderWindow&, GameTextureType) const;
        /// <summary>
        /// Properly draws the given sprite.
        /// </summary>
        void DrawAWESprite(sf::RenderWindow&, const AWESprite&) const;
        /// <summary>
        /// Properly draws the textbox of the given type if it exists.
        /// </summary>
        void DrawTextBox(sf::RenderWindow&, GameTextboxType) const;
        /// <summary>
        /// Properly draws the given textbox. Allows for the option of ignoring "isVisible", useful if the textbox is an embedded element in another object, such as the battle menu.
        /// </summary>
        void DrawTextBox(sf::RenderWindow&, const TextBox&, bool ignoreIsVisible = false) const;
        /// <summary>
        /// Plays music.
        /// </summary>
        /// <param name="filename">Filename of the music.</param>
        /// <param name="loopstart">Time in milliseconds the loop should reset to when it's reached.</param>
        /// <param name="loopdur">Length of time in milliseconds of the loop. Length of 0 means no loop.</param>
        /// <param name="startoffset">Time in milliseconds from which the music should start, if not from the beginning.</param>
        /// <param name="location">Location of the file.</param>
        /// <returns>Did the music start successfully? A very likely reason false may be returned is the file not existing.</returns>
        bool PlayMusic(const std::string& filename, unsigned int loopstart = 0U, unsigned int loopdur = 0U, unsigned int startoffset = 0U, const std::string& location = DEFAULT_RESOURCES_LOCATION);
        /// <summary>
        /// Stops whatever music track is playing. Note that no memory of this track will remain, so starting it again will require full information being passed to PlayMusic.
        /// </summary>
        /// <returns>Did the music stop successfully? This returns false if no music is currently playing.</returns>
        bool StopMusic();

        /// <summary>
        /// Makes the given sprites visible.
        /// </summary>
        /// <param name="setAllOthersInvisible">If set to true, all other sprites will be set to not visible.</param>
        /// <returns>Any requested sprite types which did not have corresponding sprites to set visible.</returns>
        std::vector<GameTextureType> SetSpritesVisible(const std::vector<GameTextureType>& spritesToSetVisible, bool setAllOthersInvisible = false);
    };
}
