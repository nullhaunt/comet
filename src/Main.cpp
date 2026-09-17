#include "components/components_BootState.hpp"
#include "components/components_Registration.hpp"
#include "gameplay/gameplay_Systems.hpp"
#include "presentation/presentation_RenderFixture.hpp"
#include "presentation/presentation_State.hpp"

#include <doggo/Core.hpp>
#include <doggo/Platform.hpp>

#include <cstring>
#include <memory>
#include <utility>

int main(int argumentCount, char** arguments) {
  const bool smoke = argumentCount > 1 && std::strcmp(arguments[1], "--smoke") == 0;
  const bool renderSmoke = argumentCount > 1 && std::strcmp(arguments[1], "--render-smoke") == 0;
  doggo::core::TypeRegistry registry;
  if (!comet::components::RegisterComponents(registry)) {
    return 1;
  }
  comet::components::BootState bootState{};
  const auto presentation = comet::presentation::MakeInitialState();
  static_cast<void>(presentation);

  doggo::platform::ApplicationConfig applicationConfig{};
  applicationConfig.m_Title = "Comet Gate 2 Fixture";
  applicationConfig.m_CreateWindow = !smoke;
  applicationConfig.m_MountRomfs = !smoke;
  applicationConfig.m_EnableDiagnostics = COMET_DEVELOPMENT_SERVICES != 0;
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

  int result = 0;
  std::unique_ptr<comet::presentation::RenderFixture> renderFixture;
  if (!smoke) {
    renderFixture = comet::presentation::RenderFixture::Create(*application);
    if (renderFixture == nullptr) {
      result = 5;
    }
  }

  while (result == 0 && !application->ShouldQuit()) {
    if (!application->PollEvents() || !engine->RunFrame()) {
      result = 6;
      break;
    }
    comet::gameplay::RunFixedTick(bootState);
    if (renderFixture != nullptr && !renderFixture->RenderFrame()) {
      result = 7;
      break;
    }
    if ((smoke && bootState.m_FixedTicks == 3) ||
        (renderSmoke && renderFixture != nullptr && renderFixture->GetRenderedFrameCount() == 3)) {
      application->RequestQuit();
    }
  }

  if (renderFixture != nullptr && !renderFixture->Shutdown() && result == 0) {
    result = 8;
  }

  engine->Shutdown();
  application->Shutdown();
  return result;
}
