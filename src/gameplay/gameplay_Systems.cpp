#include "gameplay/gameplay_Systems.hpp"

namespace comet::gameplay {

void RunFixedTick(components::BootState& state) noexcept { ++state.m_FixedTicks; }

}  // namespace comet::gameplay
