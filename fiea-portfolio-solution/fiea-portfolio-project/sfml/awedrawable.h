#pragma once

namespace AWE {
    class AWEDrawable {
    protected:
        bool _isVisible;

    public:
        AWEDrawable();

        bool isVisible() const;

        bool isVisible(bool);
    };
}
