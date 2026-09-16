#include "components/components_Registration.hpp"

#include <doggo/Core.hpp>

#include <cstdio>

int main() {
  doggo::core::TypeRegistry registry;
  if (!comet::components::register_components(registry)) {
    return 1;
  }
  std::puts("comet-editor: Gate 1 registration bootstrap");
  return 0;
}
