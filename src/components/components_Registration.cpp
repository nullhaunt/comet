#include "components/components_Registration.hpp"

namespace comet::components {

doggo::core::Result< void > register_components(doggo::core::TypeRegistry& registry) {
  constexpr auto canonical_name = "comet.BootState";
  return registry.register_type(doggo::core::make_type_id(canonical_name), canonical_name);
}

} // namespace comet::components
