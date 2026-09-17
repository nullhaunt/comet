#include "components/components_BootState.hpp"
#include "components/components_Registration.hpp"
#include "gameplay/gameplay_Systems.hpp"
#include "presentation/presentation_RenderFixture.hpp"
#include "presentation/presentation_State.hpp"

#include <doggo/Core.hpp>

#include <cstdint>

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

  const auto fixture = comet::presentation::GetTexturedFixtureInputs();
  if (fixture.m_Vertices.size() != 3 || fixture.m_Indices.size() != 3 || fixture.m_TextureWidth != 2 ||
      fixture.m_TextureHeight != 2 || fixture.m_TexturePixels.size() != 16) {
    return 6;
  }
  for (const std::uint16_t index : fixture.m_Indices) {
    if (index >= fixture.m_Vertices.size()) {
      return 7;
    }
  }
  static_assert((COMET_DEVELOPMENT_SERVICES != 0) != (COMET_SHIPPING != 0));
  return 0;
}
