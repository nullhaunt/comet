#include "components/components_BootState.hpp"
#include "components/components_Registration.hpp"
#include "gameplay/gameplay_Systems.hpp"
#include "presentation/presentation_State.hpp"

#include <doggo/Core.hpp>
#include <doggo/Platform.hpp>

#include <cstring>
#include <utility>

int main(int argument_count, char** arguments) {
  const bool smoke = argument_count > 1 && std::strcmp(arguments[1], "--smoke") == 0;
  doggo::core::TypeRegistry registry;
  if (!comet::components::register_components(registry)) {
    return 1;
  }
  comet::components::BootState boot_state{};
  const auto presentation = comet::presentation::make_initial_state();
  static_cast< void >(presentation);

  doggo::platform::ApplicationConfig application_config{};
  application_config.title = "Comet";
  application_config.create_window = !smoke;
  auto application_result = doggo::platform::Application::create(application_config);
  if (!application_result) {
    return 2;
  }
  auto application = std::move(application_result).value();

  doggo::core::EngineConfig engine_config{};
  engine_config.application_name = "Comet";
  engine_config.development_services = COMET_DEVELOPMENT_SERVICES != 0;
  auto engine_result = doggo::core::Engine::create(engine_config);
  if (!engine_result) {
    return 3;
  }
  auto engine = std::move(engine_result).value();
  if (!engine->initialize()) {
    return 4;
  }

  while (!application->should_quit()) {
    if (!application->poll_events() || !engine->run_frame()) {
      return 5;
    }
    comet::gameplay::run_fixed_tick(boot_state);
    if (smoke && boot_state.fixed_ticks == 3) {
      application->request_quit();
    }
  }

  engine->shutdown();
  application->shutdown();
  return 0;
}
