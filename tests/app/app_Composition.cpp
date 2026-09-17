#include "components/components_BootState.hpp"
#include "components/components_Registration.hpp"
#include "gameplay/gameplay_Systems.hpp"
#include "presentation/presentation_State.hpp"

#include <doggo/Core.hpp>

int main() {
  doggo::core::TypeRegistry registry;
  if (!comet::components::RegisterComponents(registry)) {
    return 1;
  }
  if (!comet::components::RegisterComponents(registry)) {
    return 2;
  }
  const auto id = doggo::core::MakeTypeId("comet.BootState");
  if (registry.Find(id) != "comet.BootState") {
    return 3;
  }

  comet::components::BootState state{};
  comet::gameplay::RunFixedTick(state);
  if (state.m_FixedTicks != 1) {
    return 4;
  }

  const auto presentation = comet::presentation::MakeInitialState();
  if (presentation.m_CameraOrigin != doggo::math::Vec3(0.0F)) {
    return 5;
  }
  static_assert((COMET_DEVELOPMENT_SERVICES != 0) != (COMET_SHIPPING != 0));
  return 0;
}
