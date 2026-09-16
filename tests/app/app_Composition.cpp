#include "components/components_BootState.hpp"
#include "components/components_Registration.hpp"
#include "gameplay/gameplay_Systems.hpp"
#include "presentation/presentation_State.hpp"

#include <doggo/Core.hpp>

int main() {
  doggo::core::TypeRegistry registry;
  if (!comet::components::register_components(registry)) {
    return 1;
  }
  if (!comet::components::register_components(registry)) {
    return 2;
  }
  const auto id = doggo::core::make_type_id("comet.BootState");
  if (registry.find(id) != "comet.BootState") {
    return 3;
  }

  comet::components::BootState state{};
  comet::gameplay::run_fixed_tick(state);
  if (state.fixed_ticks != 1) {
    return 4;
  }

  const auto presentation = comet::presentation::make_initial_state();
  if (presentation.camera_origin != doggo::math::Vec3(0.0F)) {
    return 5;
  }
  static_assert((COMET_DEVELOPMENT_SERVICES != 0) != (COMET_SHIPPING != 0));
  return 0;
}
