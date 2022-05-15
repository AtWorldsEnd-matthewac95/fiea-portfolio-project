#pragma once
#include <SFML/Graphics.hpp>
#include "awedrawable.h"

namespace AWE {
    /// <summary>
    /// Designation of the type of sprite this is. Mostly used for sizing and positioning purposes.
    /// </summary>
    enum class AWESpriteType : unsigned short {
        BACKGROUND,
        CHARACTER,
        ENEMY,
        SKILL
    };

    /// <summary>
    /// Wraps an SFML sprite instance. Provides AWEDrawable functionality, as well as designating this sprite with a type and with basic sprite sheet functionality.
    /// </summary>
    class AWESprite : public AWEDrawable {
    private:
        sf::Sprite _sprite;
        AWESpriteType _spriteType;
        unsigned int _textureIndex;

        /// <summary>
        /// Used to reposition the texture Rect when the texture index changes.
        /// </summary>
        void UpdateTextureRect();

    public:
        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="Texture">const reference to the SFML texture to create the SFML sprite instance with.</param>
        /// <param name="x">The X position the SFML sprite instance will be set to.</param>
        /// <param name="y">The Y position the SFML sprite instance will be set to.</param>
        AWESprite(const sf::Texture&, AWESpriteType, unsigned int textureIndex, float x = 0.f, float y = 0.f);

        /// <summary>
        /// Width in pixels of BACKGROUND sprites.
        /// </summary>
        static const unsigned int WIDTH_BACKGROUND;
        /// <summary>
        /// Height in pixels of BACKGROUND sprites.
        /// </summary>
        static const unsigned int HEIGHT_BACKGROUND;
        /// <summary>
        /// Width in pixels of CHARACTER sprites.
        /// </summary>
        static const unsigned int WIDTH_CHARACTER;
        /// <summary>
        /// Height in pixels of CHARACTER sprites.
        /// </summary>
        static const unsigned int HEIGHT_CHARACTER;
        /// <summary>
        /// Width in pixels of ENEMY sprites.
        /// </summary>
        static const unsigned int WIDTH_ENEMY;
        /// <summary>
        /// Height in pixels of ENEMY sprites.
        /// </summary>
        static const unsigned int HEIGHT_ENEMY;
        /// <summary>
        /// Width in pixels of SKILL sprites.
        /// </summary>
        static const unsigned int WIDTH_SKILL;
        /// <summary>
        /// Height in pixels of SKILL sprites.
        /// </summary>
        static const unsigned int HEIGHT_SKILL;
        /// <summary>
        /// The character's current position + this will be the position where a skill should be placed to appear as though it's on the character.
        /// </summary>
        static const sf::Vector2i RELATIVE_SKILL_CHARACTER_POSITION;
        /// <summary>
        /// The enemy's current position + this will be the position where a skill should be placed to appear as though it's on the enemy.
        /// </summary>
        static const sf::Vector2i RELATIVE_SKILL_ENEMY_POSITION;

        /// <returns>const reference to the inner SFML Sprite instance.</returns>
        const sf::Sprite& sprite() const;
        /// <returns>The sprite type.</returns>
        AWESpriteType spriteType() const;
        /// <returns>Which rectangular section of the sprite's texture it's occupying.</returns>
        unsigned int textureIndex() const;

        /// <summary>
        /// Setter
        /// </summary>
        /// <param name="">The new value.</param>
        /// <returns>The old value.</returns>
        unsigned int textureIndex(unsigned int);

        /// <returns>const reference to the inner SFML Sprite instance's position.</returns>
        const sf::Vector2f& GetPosition() const;
        /// <summary>
        /// Sets the inner SFML Sprite instance's position.
        /// </summary>
        /// <param name="x">The new X value.</param>
        /// <param name="y">The new Y value.</param>
        /// <returns>The old position.</returns>
        sf::Vector2f SetPosition(float x, float y);
        /// <summary>
        /// Uses the RELATIVE_SKILL_..._POSITION constants to create a position object for skill sprites to occupy. If the sprite's type is not CHARACTER it will use the _ENEMY_ constant.
        /// </summary>
        /// <returns>The position that a skill should occupy to appear over this sprite.</returns>
        sf::Vector2f GetSkillPosition() const;
        /// <summary>
        /// Sets the inner SFML Sprite instance's color.
        /// </summary>
        /// <param name="">The new color.</param>
        /// <returns>The old color.</returns>
        sf::Color SetSpriteColor(sf::Color);
    };
}
