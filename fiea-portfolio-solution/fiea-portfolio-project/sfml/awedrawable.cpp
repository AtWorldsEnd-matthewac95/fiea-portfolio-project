#include "awedrawable.h"
#include <utility>

namespace AWE {
    AWEDrawable::AWEDrawable() : _isVisible(true) {}
    bool AWEDrawable::isVisible() const { return _isVisible; }
    bool AWEDrawable::isVisible(bool newval) { bool oldval = std::move(_isVisible); _isVisible = std::move(newval); return oldval; }
}
