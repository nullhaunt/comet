#pragma once

#include <cstdint>

namespace comet::components {

struct BootState {
  std::uint64_t fixed_ticks = 0;
};

} // namespace comet::components
