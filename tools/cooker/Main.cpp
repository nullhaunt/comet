#include <cstdio>
#include <doggo/Core.hpp>

#include "components/components_Registration.hpp"

int main() {
  doggo::core::TypeRegistry registry;
  if (!comet::components::RegisterComponents(registry)) {
    return 1;
  }
  std::puts("comet-cooker: Gate 1 registration bootstrap");
  return 0;
}
