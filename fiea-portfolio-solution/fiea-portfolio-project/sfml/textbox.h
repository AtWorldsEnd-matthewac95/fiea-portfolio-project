#pragma once
#include <string>
#include <SFML/Graphics.hpp>
#include "awedrawable.h"

namespace AWE {
    /// <summary>
    /// Enumerates text alignments. Think Microsoft Word.
    /// </summary>
    enum class TextAlignment : unsigned short {
        LEFT,
        CENTER,
        RIGHT
    };

    /// <summary>
    /// Helper struct to consolidate some of the potential visual attributes of a text box.
    /// </summary>
    struct TextBoxVisuals {
        /// <summary>Font for text.</summary>
        const sf::Font* font;
        /// <summary>Amount of space in pixels between edges of the text and edges of the box.</summary>
        unsigned short margin;
        /// <summary>Color for text.</summary>
        sf::Color textColor;
        /// <summary>Color for box.</summary>
        sf::Color boxColor;
        /// <summary>Width in pixels of the font letters. This value is not used to set the width of the letters - rather, this should describe the width which already exists.</summary>
        unsigned int letterWidth;
        /// <summary>Height in pixels of the font letters. This value is not used to set the height of the letters - rather, this should describe the height which already exists.</summary>
        unsigned int letterHeight;

        /// <summary>
        /// Constructor.
        /// </summary>
        /// <param name="font">Font for text.</param>
        /// <param name="margin">Amount of space in pixels between edges of the text and edges of the box.</param>
        /// <param name="textColor">Color for text.</param>
        /// <param name="boxColor">Color for box.</param>
        /// <param name="letterWidth">Width in pixels of the font letters. This value is not used to set the width of the letters - rather, this should describe the width which already exists.</param>
        /// <param name="letterHeight">Height in pixels of the font letters. This value is not used to set the height of the letters - rather, this should describe the height which already exists.</param>
        TextBoxVisuals(const sf::Font& font, unsigned short margin, sf::Color textColor, sf::Color boxColor, unsigned int letterWidth, unsigned int letterHeight)
            : font(&font), margin(margin), textColor(std::move(textColor)), boxColor(std::move(boxColor)), letterWidth(letterWidth), letterHeight(letterHeight) {}
    };

    /// <summary>
    /// Wraps a SFML RectangleShape element and a SFML Text element. Provides utility functionality to aid in orienting the text and sizing the box.
    /// </summary>
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

        /// <returns>The length in characters of the longest line in the text. Obviously this will just return the string size if no newlines are in the text.</returns>
        size_t FindLongestLineLength() const;
        /// <summary>
        /// Positions the text such that it's correctly aligned with the box.
        /// </summary>
        /// <returns>The old position of the text.</returns>
        sf::Vector2f RepositionText();

    public:
        /// <summary>
        /// File name of the font this project is configured to use by default.
        /// </summary>
        static const std::string CONFIGURED_FONT_FILENAME;
        /// <summary>
        /// Height in pixels of the letters of the font this project is configured to use by default.
        /// </summary>
        static const unsigned int TEXT_PIXEL_HEIGHT;
        /// <summary>
        /// Width in pixels of the letters of the font this project is configured to use by default.
        /// </summary>
        static const unsigned int TEXT_PIXEL_WIDTH;
        /// <summary>
        /// Set to 4. The text is made "sharper" (meaning no blurry edges) by making the font size X times bigger than it should be and then scaling it down to proper size, where X is this value.
        /// </summary>
        static const unsigned int TEXT_SHARPNESS_SCALE;
        /// <summary>
        /// Unsure why this happens, but the text ends up needing a vertical offset to be truly centered on the box. This value is involved with calculating that offset.
        /// </summary>
        static const float TEXT_POS_VERT_OFFSET_DIV;

        /// <summary>
        /// Constructor.
        /// </summary>
        /// <param name="fixedWidth">If 0, the width depends on the text. Otherwise, the width will be this value regardless of text. Note this value does not include margins.</param>
        /// <param name="fixedHeight">If 0, the height depends on the text. Otherwise, the height will be this value regardless of text. Note this value does not include margins.</param>
        /// <param name="letterWidth">Width in pixels of the font letters. This value is not used to set the width of the letters - rather, this should describe the width which already exists.</param>
        /// <param name="letterHeight">Height in pixels of the font letters. This value is not used to set the height of the letters - rather, this should describe the height which already exists.</param>
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

        /// <summary>
        /// Constructor.
        /// </summary>
        /// <param name="fixedWidth">If 0, the width depends on the text. Otherwise, the width will be this value regardless of text. Note this value does not include margins.</param>
        /// <param name="fixedHeight">If 0, the height depends on the text. Otherwise, the height will be this value regardless of text. Note this value does not include margins.</param>
        TextBox(
            TextBoxVisuals,
            std::string str = "",
            unsigned short fixedWidth = 0,
            unsigned int fixedHeight = 0,
            TextAlignment alignment = TextAlignment::CENTER
        );

        /// <returns>const reference to the internal SFML Text element.</returns>
        const sf::Text& text() const;
        /// <returns>const reference to the internal SFML box element.</returns>
        const sf::RectangleShape& box() const;
        /// <returns>Amount of space in pixels between edges of the text and edges of the box. Note that neither fixed width nor fixed height account for this margin.</returns>
        unsigned short margin() const;
        /// <returns>If 0, the width depends on the text. Otherwise, the width will be this value regardless of text. Note this value does not include margins.</returns>
        unsigned short fixedWidth() const;
        /// <returns>If 0, the height depends on the text. Otherwise, the height will be this value regardless of text. Note this value does not include margins.</returns>
        unsigned int fixedHeight() const;
        /// <returns>Amount of lines in the text. Examples: "hello world" -> 1 line, "hello\nworld" -> 2 lines, etc.</returns>
        unsigned short lines() const;
        /// <returns>Alignment of the text.</returns>
        TextAlignment alignment() const;
        /// <returns>Width in pixels of the font letters. This value is not used to set the width of the letters - rather, this should describe the width which already exists.</returns>
        unsigned int letterWidth() const;
        /// <returns>Height in pixels of the font letters. This value is not used to set the height of the letters - rather, this should describe the height which already exists.</returns>
        unsigned int letterHeight() const;

        /// <param name="">New value for the margin. Note that neither fixed width nor fixed height account for this margin.</param>
        /// <returns>Old value for the margin.</returns>
        unsigned short margin(unsigned short);
        /// <param name="">New value for the fixed width. Otherwise, the width will be this value regardless of text. Note this value does not include margins.</param>
        /// <returns>Old value for the fixed width.</returns>
        unsigned short fixedWidth(unsigned short);
        /// <param name="">New value for the fixed height. Otherwise, the height will be this value regardless of text. Note this value does not include margins.</param>
        /// <returns>Old value for the fixed height.</returns>
        unsigned int fixedHeight(unsigned int);
        /// <param name="">New value for the text alignment.</param>
        /// <returns>Old value for the text alignment.</returns>
        TextAlignment alignment(TextAlignment);
        /// <param name="">New value for the letter width. This value is not used to set the width of the letters - rather, this should describe the width which already exists.</param>
        /// <returns>Old value for the letter width.</returns>
        unsigned int letterWidth(unsigned int);
        /// <param name="">New value for the letter height. This value is not used to set the height of the letters - rather, this should describe the height which already exists.</param>
        /// <returns>Old value for the letter height.</returns>
        unsigned int letterHeight(unsigned int);

        /// <summary>
        /// Sets the position of the textbox. This will update the position of both the box element and the text element.
        /// </summary>
        /// <returns>Old value for the textbox position.</returns>
        sf::Vector2f SetPosition(float x, float y);
        /// <summary>
        /// Sets the position of the textbox. This will update the position of both the box element and the text element.
        /// </summary>
        /// <returns>Old value for the textbox position.</returns>
        sf::Vector2f SetPosition(const sf::Vector2f&);
        /// <param name="">New string value for the text.</param>
        /// <returns>Old string value for the text.</returns>
        sf::String SetString(std::string);
    };
}
