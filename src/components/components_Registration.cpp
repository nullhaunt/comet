#include "components/components_Registration.hpp"

namespace comet::components {

doggo::core::Result<void> RegisterComponents(doggo::core::TypeRegistry& registry) {
  constexpr auto canonicalName = "comet.BootState";
  return registry.RegisterType(doggo::core::MakeTypeId(canonicalName), canonicalName);
}

}  // namespace comet::components
