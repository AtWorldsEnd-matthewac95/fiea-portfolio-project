#include "textbox.h"

namespace AWE {
    const std::string TextBox::CONFIGURED_FONT_FILENAME = "res/novem___.ttf";
    const unsigned int TextBox::TEXT_PIXEL_HEIGHT = 16U;
    const unsigned int TextBox::TEXT_PIXEL_WIDTH = 8U;
    const unsigned int TextBox::TEXT_SHARPNESS_SCALE = 4U;
    const float TextBox::TEXT_POS_VERT_OFFSET_DIV = 8.f;

    TextBox::TextBox(
        const sf::Font& font,
        unsigned short margin,
        sf::Color textColor,
        sf::Color boxColor,
        std::string str,
        unsigned short fixedWidth,
        unsigned int fixedHeight,
        TextAlignment alignment,
        unsigned int letterWidth,
        unsigned int letterHeight
    ) : AWEDrawable(), _margin(std::move(margin)), _fixedWidth(std::move(fixedWidth)), _fixedHeight(std::move(fixedHeight)), _alignment(std::move(alignment)), _letterWidth(std::move(letterWidth)), _letterHeight(std::move(letterHeight)) {
        _text = sf::Text();
        _text.setFont(font);
        _text.setCharacterSize(_letterHeight * TEXT_SHARPNESS_SCALE);
        _text.setScale(1.f / TEXT_SHARPNESS_SCALE, 1.f / TEXT_SHARPNESS_SCALE);
        _text.setFillColor(std::move(textColor));

        _lines = 1 + std::count(str.begin(), str.end(), '\n');
        _text.setString(std::move(str));

        _box = sf::RectangleShape(sf::Vector2f(2 * margin, (_lines * _letterHeight) + (2 * margin)));
        _box.setFillColor(std::move(boxColor));

        RepositionText();
    }

    TextBox::TextBox(TextBoxVisuals visuals, std::string str, unsigned short fixedWidth, unsigned int fixedHeight, TextAlignment alignment)
        : TextBox(*visuals.font, visuals.margin, visuals.textColor, visuals.boxColor, std::move(str), fixedWidth, fixedHeight, alignment, visuals.letterWidth, visuals.letterHeight) {}

    size_t TextBox::FindLongestLineLength() const {
        std::string str = _text.getString();
        size_t longest = 0;
        bool morelines = true;

        while (morelines && str.size() > 0) {
            size_t found = str.find('\n');
            morelines = found != std::string::npos;
            size_t length = morelines ? str.substr(0, found).size() : str.size();
            longest = length > longest ? length : longest;

            if (morelines) {
                str = str.substr(found + 1);
            }
        }

        return longest;
    }

    sf::Vector2f TextBox::RepositionText() {
        bool fixed = _fixedWidth > 0;
        size_t strsize = FindLongestLineLength();
        int boxwidth = (_fixedWidth > 0 ? _fixedWidth : (strsize * _letterWidth)) + (2 * _margin);
        int boxheight = (_fixedHeight > 0 ? _fixedHeight : (_lines * _letterHeight)) + (2 * _margin);
        _box.setSize(sf::Vector2f(boxwidth, boxheight));
        const sf::Vector2f& boxpos = _box.getPosition();

        const sf::Vector2f oldpos = _text.getPosition();
        float xoffset = 0.f;

        if (fixed && _alignment != TextAlignment::LEFT) {
            switch (_alignment) {
            case TextAlignment::CENTER:
                xoffset = (boxwidth / 2.f) - _margin - (strsize * (_letterWidth / 2.f));
                break;

            case TextAlignment::RIGHT:
                xoffset = boxwidth - (2.f * _margin) - (strsize * _letterWidth);
                break;
            }
        }

        _text.setPosition(sf::Vector2f(boxpos.x + xoffset + _margin, boxpos.y + _margin - (_letterHeight / TEXT_POS_VERT_OFFSET_DIV)));

        return oldpos;
    }

    const sf::Text& TextBox::text() const { return _text; }
    const sf::RectangleShape& TextBox::box() const { return _box; }
    unsigned short TextBox::margin() const { return _margin; }
    unsigned short TextBox::fixedWidth() const { return _fixedWidth; }
    unsigned int TextBox::fixedHeight() const { return _fixedHeight; }
    unsigned short TextBox::lines() const { return _lines; }
    TextAlignment TextBox::alignment() const { return _alignment; }
    unsigned int TextBox::letterWidth() const { return _letterWidth; }
    unsigned int TextBox::letterHeight() const { return _letterHeight; }

    unsigned short TextBox::margin(unsigned short newval) {
        unsigned short oldval = std::move(_margin);
        _margin = std::move(newval);
        RepositionText();
        return oldval;
    }

    unsigned short TextBox::fixedWidth(unsigned short newval) {
        unsigned short oldval = std::move(_fixedWidth);
        _fixedWidth = std::move(newval);
        RepositionText();
        return oldval;
    }

    unsigned int TextBox::fixedHeight(unsigned int newval) {
        unsigned int oldval = std::move(_fixedHeight);
        _fixedHeight = std::move(newval);
        RepositionText();
        return oldval;
    }

    TextAlignment TextBox::alignment(TextAlignment newval) {
        TextAlignment oldval = std::move(_alignment);
        _alignment = std::move(newval);
        RepositionText();
        return oldval;
    }

    unsigned int TextBox::letterWidth(unsigned int newval) {
        unsigned int oldval = std::move(_letterWidth);
        _letterWidth = std::move(newval);
        RepositionText();
        return oldval;
    }

    unsigned int TextBox::letterHeight(unsigned int newval) {
        unsigned int oldval = std::move(_letterHeight);
        _letterHeight = std::move(newval);
        RepositionText();
        return oldval;
    }

    sf::Vector2f TextBox::SetPosition(float x, float y) { sf::Vector2f _(x, y); return SetPosition(_); }
    sf::Vector2f TextBox::SetPosition(const sf::Vector2f& position) {
        sf::Vector2f oldval = _box.getPosition();
        _box.setPosition(position);
        RepositionText();
        return oldval;
    }

    sf::String TextBox::SetString(std::string str) {
        sf::String oldval = _text.getString();
        _lines = 1 + std::count(str.begin(), str.end(), '\n');
        _text.setString(str);
        RepositionText();
        return oldval;
    }
}
