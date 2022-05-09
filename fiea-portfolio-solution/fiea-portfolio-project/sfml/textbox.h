#pragma once
#include <string>
#include <SFML/Graphics.hpp>
#include "awedrawable.h"

namespace AWE {
    enum class TextAlignment : unsigned short {
        LEFT,
        CENTER,
        RIGHT
    };

    struct TextBoxVisuals {
        const sf::Font* font;
        unsigned short margin;
        sf::Color textColor;
        sf::Color boxColor;
        unsigned int letterWidth;
        unsigned int letterHeight;

        TextBoxVisuals(const sf::Font& font, unsigned short margin, sf::Color textColor, sf::Color boxColor, unsigned int letterWidth, unsigned int letterHeight)
            : font(&font), margin(margin), textColor(std::move(textColor)), boxColor(std::move(boxColor)), letterWidth(letterWidth), letterHeight(letterHeight) {}
    };

    class TextBox : public AWEDrawable {
    private:
        sf::Text _text;
        sf::RectangleShape _box;
        unsigned short _margin;
        unsigned short _fixedWidth;
        unsigned int _fixedHeight;
        unsigned short _lines;
        TextAlignment _alignment;
        unsigned int _letterWidth;
        unsigned int _letterHeight;

        size_t FindLongestLineLength() const;
        sf::Vector2f RepositionText();

    public:
        static const std::string CONFIGURED_FONT_FILENAME;
        static const unsigned int TEXT_PIXEL_HEIGHT;
        static const unsigned int TEXT_PIXEL_WIDTH;
        static const unsigned int TEXT_SHARPNESS_SCALE;
        static const float TEXT_POS_VERT_OFFSET_DIV;

        TextBox(
            const sf::Font&,
            unsigned short margin,
            sf::Color textColor,
            sf::Color boxColor,
            std::string str = "",
            unsigned short fixedWidth = 0,
            unsigned int fixedHeight = 0,
            TextAlignment alignment = TextAlignment::CENTER,
            unsigned int letterWidth = TEXT_PIXEL_WIDTH,
            unsigned int letterHeight = TEXT_PIXEL_HEIGHT
        );

        TextBox(
            TextBoxVisuals,
            std::string str = "",
            unsigned short fixedWidth = 0,
            unsigned int fixedHeight = 0,
            TextAlignment alignment = TextAlignment::CENTER
        );

        const sf::Text& text() const;
        const sf::RectangleShape& box() const;
        unsigned short margin() const;
        unsigned short fixedWidth() const;
        unsigned int fixedHeight() const;
        unsigned short lines() const;
        TextAlignment alignment() const;
        unsigned int letterWidth() const;
        unsigned int letterHeight() const;

        unsigned short margin(unsigned short);
        unsigned short fixedWidth(unsigned short);
        unsigned int fixedHeight(unsigned int);
        TextAlignment alignment(TextAlignment);
        unsigned int letterWidth(unsigned int);
        unsigned int letterHeight(unsigned int);

        sf::Vector2f SetPosition(float x, float y);
        sf::Vector2f SetPosition(const sf::Vector2f&);
        sf::String SetString(std::string);
    };
}
