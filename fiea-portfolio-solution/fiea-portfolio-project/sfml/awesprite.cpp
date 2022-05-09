#include "awesprite.h"

namespace AWE {
    AWESprite::AWESprite(const sf::Texture& texture, AWESpriteType spriteType, unsigned int textureIndex, float x, float y) : AWEDrawable(), _spriteType(spriteType), _textureIndex(textureIndex) {
        _sprite = sf::Sprite();
        _sprite.setTexture(texture);

        UpdateTextureRect();

        _sprite.setPosition(x, y);
    }

    const unsigned int AWESprite::WIDTH_BACKGROUND = 320;
    const unsigned int AWESprite::HEIGHT_BACKGROUND = 240;
    const unsigned int AWESprite::WIDTH_CHARACTER = 32;
    const unsigned int AWESprite::HEIGHT_CHARACTER = 32;
    const unsigned int AWESprite::WIDTH_ENEMY = 128;
    const unsigned int AWESprite::HEIGHT_ENEMY = 128;
    const unsigned int AWESprite::WIDTH_SKILL = 64;
    const unsigned int AWESprite::HEIGHT_SKILL = 64;
    const sf::Vector2i AWESprite::RELATIVE_SKILL_CHARACTER_POSITION = sf::Vector2i(
        (static_cast<int>(WIDTH_CHARACTER) - static_cast<int>(WIDTH_SKILL)) / 2,
        (static_cast<int>(HEIGHT_CHARACTER) - static_cast<int>(HEIGHT_SKILL)) / 2
    );
    const sf::Vector2i AWESprite::RELATIVE_SKILL_ENEMY_POSITION = sf::Vector2i(
        (static_cast<int>(WIDTH_ENEMY) - static_cast<int>(WIDTH_SKILL)) / 2,
        (static_cast<int>(HEIGHT_ENEMY) - static_cast<int>(HEIGHT_SKILL)) / 2
    );

    void AWESprite::UpdateTextureRect() {
        switch (_spriteType) {
        case AWESpriteType::BACKGROUND:
            _sprite.setTextureRect(sf::IntRect(WIDTH_BACKGROUND * _textureIndex, 0, WIDTH_BACKGROUND, HEIGHT_BACKGROUND));
            break;

        case AWESpriteType::CHARACTER:
            _sprite.setTextureRect(sf::IntRect(WIDTH_CHARACTER * _textureIndex, 0, WIDTH_CHARACTER, HEIGHT_CHARACTER));
            break;

        case AWESpriteType::ENEMY:
            _sprite.setTextureRect(sf::IntRect(WIDTH_ENEMY * _textureIndex, 0, WIDTH_ENEMY, HEIGHT_ENEMY));
            break;

        case AWESpriteType::SKILL:
            _sprite.setTextureRect(sf::IntRect(WIDTH_SKILL * _textureIndex, 0, WIDTH_SKILL, HEIGHT_SKILL));
            break;
        }
    }

    const sf::Sprite& AWESprite::sprite() const { return _sprite; }
    AWESpriteType AWESprite::spriteType() const { return _spriteType; }
    unsigned int AWESprite::textureIndex() const { return _textureIndex; }

    unsigned int AWESprite::textureIndex(unsigned int newval) { unsigned int oldval = _textureIndex; _textureIndex = newval; UpdateTextureRect();  return oldval; }

    const sf::Vector2f& AWESprite::GetPosition() const { return _sprite.getPosition(); }

    sf::Vector2f AWESprite::SetPosition(float x, float y) {
        sf::Vector2f prev = _sprite.getPosition();
        _sprite.setPosition(x, y);
        return prev;
    }

    sf::Vector2f AWESprite::GetSkillPosition() const {
        const sf::Vector2i& offset = (_spriteType == AWESpriteType::CHARACTER) ? AWESprite::RELATIVE_SKILL_CHARACTER_POSITION : AWESprite::RELATIVE_SKILL_ENEMY_POSITION;
        const sf::Vector2f& position = _sprite.getPosition();
        return sf::Vector2f(position.x + offset.x, position.y + offset.y);
    }

    sf::Color AWESprite::SetSpriteColor(sf::Color newval) {
        sf::Color oldval = _sprite.getColor();
        _sprite.setColor(newval);
        return oldval;
    }
}
