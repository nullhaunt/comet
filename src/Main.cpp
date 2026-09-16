#include <cstring>
#include <doggo/Core.hpp>
#include <doggo/Platform.hpp>
#include <utility>

#include "components/components_BootState.hpp"
#include "components/components_Registration.hpp"
#include "gameplay/gameplay_Systems.hpp"
#include "presentation/presentation_State.hpp"

int main(int argumentCount, char** arguments) {
  const bool smoke = argumentCount > 1 && std::strcmp(arguments[1], "--smoke") == 0;
  doggo::core::TypeRegistry registry;
  if (!comet::components::RegisterComponents(registry)) {
    return 1;
  }
  comet::components::BootState bootState{};
  const auto presentation = comet::presentation::MakeInitialState();
  static_cast<void>(presentation);

  doggo::platform::ApplicationConfig applicationConfig{};
  applicationConfig.m_Title = "Comet";
  applicationConfig.m_CreateWindow = !smoke;
  auto applicationResult = doggo::platform::Application::Create(applicationConfig);
  if (!applicationResult) {
    return 2;
  }
  auto application = std::move(applicationResult).Value();

  doggo::core::EngineConfig engineConfig{};
  engineConfig.m_ApplicationName = "Comet";
  engineConfig.m_DevelopmentServices = COMET_DEVELOPMENT_SERVICES != 0;
  auto engineResult = doggo::core::Engine::Create(engineConfig);
  if (!engineResult) {
    return 3;
  }
  auto engine = std::move(engineResult).Value();
  if (!engine->Initialize()) {
    return 4;
  }

  while (!application->ShouldQuit()) {
    if (!application->PollEvents() || !engine->RunFrame()) {
      return 5;
    }
    comet::gameplay::RunFixedTick(bootState);
    if (smoke && bootState.m_FixedTicks == 3) {
      application->RequestQuit();
    }
  }

  engine->Shutdown();
  application->Shutdown();
  return 0;
}
