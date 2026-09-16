#pragma once

#include <doggo/Math.hpp>

namespace comet::presentation {

struct State {
  doggo::math::Vec3 m_CameraOrigin{0.0F, 0.0F, 0.0F};
};

State MakeInitialState() noexcept;

}  // namespace comet::presentation
