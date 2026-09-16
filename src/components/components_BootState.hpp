#pragma once

#include <cstdint>

namespace comet::components {

struct BootState {
  std::uint64_t m_FixedTicks = 0;
};

}  // namespace comet::components
