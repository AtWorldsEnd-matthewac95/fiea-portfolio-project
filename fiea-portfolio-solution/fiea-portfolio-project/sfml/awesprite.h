#pragma once
#include <SFML/Graphics.hpp>
#include "awedrawable.h"

namespace AWE {
    enum class AWESpriteType : unsigned short {
        BACKGROUND,
        CHARACTER,
        ENEMY,
        SKILL
    };

    class AWESprite : public AWEDrawable {
    private:
        sf::Sprite _sprite;
        AWESpriteType _spriteType;
        unsigned int _textureIndex;

        void UpdateTextureRect();

    public:
        AWESprite(const sf::Texture&, AWESpriteType, unsigned int textureIndex, float x = 0.f, float y = 0.f);

        static const unsigned int WIDTH_BACKGROUND;
        static const unsigned int HEIGHT_BACKGROUND;
        static const unsigned int WIDTH_CHARACTER;
        static const unsigned int HEIGHT_CHARACTER;
        static const unsigned int WIDTH_ENEMY;
        static const unsigned int HEIGHT_ENEMY;
        static const unsigned int WIDTH_SKILL;
        static const unsigned int HEIGHT_SKILL;
        static const sf::Vector2i RELATIVE_SKILL_CHARACTER_POSITION;
        static const sf::Vector2i RELATIVE_SKILL_ENEMY_POSITION;

        const sf::Sprite& sprite() const;
        AWESpriteType spriteType() const;
        unsigned int textureIndex() const;

        unsigned int textureIndex(unsigned int);

        const sf::Vector2f& GetPosition() const;
        sf::Vector2f SetPosition(float x, float y);
        sf::Vector2f GetSkillPosition() const;
        sf::Color SetSpriteColor(sf::Color);
    };
}
