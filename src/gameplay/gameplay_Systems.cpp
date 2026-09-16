#include "gameplay/gameplay_Systems.hpp"

namespace comet::gameplay {

void run_fixed_tick(components::BootState& state) noexcept {
  ++state.fixed_ticks;
}

} // namespace comet::gameplay
