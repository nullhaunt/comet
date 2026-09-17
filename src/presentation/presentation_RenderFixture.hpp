#pragma once

#include <doggo/Platform.hpp>

#include <array>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <span>

namespace comet::presentation {

struct TexturedFixtureVertex {
  std::array<float, 3> m_Position;
  std::array<float, 2> m_Texcoord;
};

struct TexturedFixtureInputs {
  std::span<const TexturedFixtureVertex> m_Vertices;
  std::span<const std::uint16_t> m_Indices;
  std::span<const std::byte> m_TexturePixels;
  std::uint32_t m_TextureWidth = 0;
  std::uint32_t m_TextureHeight = 0;
};

[[nodiscard]] TexturedFixtureInputs GetTexturedFixtureInputs() noexcept;

class RenderFixture final {
 public:
  static std::unique_ptr<RenderFixture> Create(doggo::platform::Application& application);

  ~RenderFixture();
  RenderFixture(const RenderFixture&) = delete;
  RenderFixture& operator=(const RenderFixture&) = delete;

  [[nodiscard]] bool RenderFrame();
  [[nodiscard]] bool Shutdown() noexcept;
  [[nodiscard]] std::uint32_t GetRenderedFrameCount() const noexcept;

 private:
  class Impl;
  explicit RenderFixture(std::unique_ptr<Impl> impl) noexcept;

  std::unique_ptr<Impl> m_Impl;
};

}  // namespace comet::presentation
