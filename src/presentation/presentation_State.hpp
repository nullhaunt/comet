#pragma once

#include <doggo/Math.hpp>

namespace comet::presentation {

struct State {
  doggo::math::Vec3 camera_origin{0.0F, 0.0F, 0.0F};
};

State make_initial_state() noexcept;

} // namespace comet::presentation
