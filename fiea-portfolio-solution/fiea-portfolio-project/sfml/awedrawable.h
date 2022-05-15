#pragma once

namespace AWE {
    /// <summary>
    /// Wrapper class to SFML drawable elements. Provides basic additional functionality.
    /// </summary>
    class AWEDrawable {
    protected:
        bool _isVisible;

    public:
        AWEDrawable();

        /// <returns>Should this object be drawn?</returns>
        bool isVisible() const;

        /// <summary>
        /// Setter
        /// </summary>
        /// <param name="">The new value.</param>
        /// <returns>The old value.</returns>
        bool isVisible(bool);
    };
}
