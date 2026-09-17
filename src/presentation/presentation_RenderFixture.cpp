#include "presentation/presentation_RenderFixture.hpp"

#include <doggo/Core.hpp>
#include <doggo/Rhi.hpp>

#include <algorithm>
#include <array>
#include <cstdio>
#include <string_view>
#include <utility>
#include <vector>

namespace comet::presentation {
namespace {

static_assert(sizeof(TexturedFixtureVertex) == 20);

constexpr std::array<TexturedFixtureVertex, 3> g_Vertices = {{
  {.m_Position = {0.0F, -0.75F, 0.0F}, .m_Texcoord = {0.5F, 0.0F}},
  {.m_Position = {0.75F, 0.75F, 0.0F}, .m_Texcoord = {1.0F, 1.0F}},
  {.m_Position = {-0.75F, 0.75F, 0.0F}, .m_Texcoord = {0.0F, 1.0F}},
}};

constexpr std::array<std::uint16_t, 3> g_Indices = {0, 1, 2};

constexpr std::array<std::byte, 16> g_TexturePixels = {
  std::byte{0x20},
  std::byte{0xD0},
  std::byte{0xFF},
  std::byte{0xFF},
  std::byte{0xFF},
  std::byte{0x40},
  std::byte{0xC0},
  std::byte{0xFF},
  std::byte{0xFF},
  std::byte{0xD0},
  std::byte{0x20},
  std::byte{0xFF},
  std::byte{0x30},
  std::byte{0x30},
  std::byte{0x50},
  std::byte{0xFF},
};

constexpr std::array<doggo::rhi::VertexAttributeDesc, 2> g_VertexAttributes = {{
  {.m_Location = 0, .m_Format = doggo::rhi::VertexFormat::Float3, .m_Offset = 0},
  {.m_Location = 1, .m_Format = doggo::rhi::VertexFormat::Float2, .m_Offset = 12},
}};

constexpr std::array<doggo::rhi::BindingLayoutDesc, 1> g_Bindings = {{
  {
    .m_Binding = 0,
    .m_Type = doggo::rhi::BindingType::CombinedImageSampler,
    .m_Count = 1,
    .m_Stages = doggo::rhi::ShaderStageMask::Fragment,
  },
}};

constexpr doggo::rhi::ShaderCodeFormat g_ShaderCodeFormat =
  COMET_FIXTURE_SHADER_IS_DKSH != 0 ? doggo::rhi::ShaderCodeFormat::Dksh : doggo::rhi::ShaderCodeFormat::SpirV;
constexpr std::string_view g_VertexShaderPath = COMET_FIXTURE_VERTEX_SHADER_PATH;
constexpr std::string_view g_FragmentShaderPath = COMET_FIXTURE_FRAGMENT_SHADER_PATH;
constexpr std::string_view g_DiagnosticLogPath = COMET_DIAGNOSTIC_LOG_PATH;

std::FILE* OpenFile(const char* path, const char* mode) noexcept {
#if defined(_WIN32)
  std::FILE* file = nullptr;
  return fopen_s(&file, path, mode) == 0 ? file : nullptr;
#else
  return std::fopen(path, mode);
#endif
}

class DiagnosticLog final {
 public:
  DiagnosticLog() {
    if (!g_DiagnosticLogPath.empty()) {
      m_File = OpenFile(g_DiagnosticLogPath.data(), "w");
    }
  }

  ~DiagnosticLog() {
    Close();
  }

  DiagnosticLog(const DiagnosticLog&) = delete;
  DiagnosticLog& operator=(const DiagnosticLog&) = delete;

  void Write(std::string_view message) noexcept {
    static_cast<void>(std::fwrite(message.data(), 1, message.size(), stderr));
    static_cast<void>(std::fwrite("\n", 1, 1, stderr));
    std::fflush(stderr);
    if (m_File != nullptr) {
      static_cast<void>(std::fwrite(message.data(), 1, message.size(), m_File));
      static_cast<void>(std::fwrite("\n", 1, 1, m_File));
      std::fflush(m_File);
    }
  }

  void Close() noexcept {
    if (m_File != nullptr) {
      std::fclose(m_File);
      m_File = nullptr;
    }
  }

 private:
  std::FILE* m_File = nullptr;
};

struct FixtureResources {
  doggo::rhi::BufferHandle m_VertexBuffer;
  doggo::rhi::BufferHandle m_IndexBuffer;
  doggo::rhi::ImageHandle m_Texture;
  doggo::rhi::ShaderHandle m_VertexShader;
  doggo::rhi::ShaderHandle m_FragmentShader;
  doggo::rhi::SamplerHandle m_Sampler;
  doggo::rhi::PipelineHandle m_Pipeline;
  doggo::rhi::BindingSetHandle m_BindingSet;
};

void ReportError(DiagnosticLog& diagnostics, std::string_view operation, const doggo::core::Error& error) noexcept {
  std::array<char, 512> message{};
  const int written = std::snprintf(message.data(),
                                    message.size(),
                                    "%.*s failed: %.*s (domain %u, code %u)",
                                    static_cast<int>(operation.size()),
                                    operation.data(),
                                    static_cast<int>(error.m_Message.size()),
                                    error.m_Message.data(),
                                    static_cast<unsigned int>(error.m_Domain),
                                    error.m_Code);
  if (written > 0) {
    diagnostics.Write(std::string_view(message.data(),
                                       std::min(static_cast<std::size_t>(written), message.size() - 1)));
  }
}

void Diagnostic(doggo::rhi::DiagnosticSeverity severity, std::string_view message, void* userData) noexcept {
  if (severity != doggo::rhi::DiagnosticSeverity::Warning && severity != doggo::rhi::DiagnosticSeverity::Error) {
    return;
  }
  auto* diagnostics = static_cast<DiagnosticLog*>(userData);
  if (diagnostics == nullptr) {
    return;
  }
  std::array<char, 512> formatted{};
  const int written = std::snprintf(formatted.data(),
                                    formatted.size(),
                                    "RHI: %.*s",
                                    static_cast<int>(message.size()),
                                    message.data());
  if (written > 0) {
    diagnostics->Write(std::string_view(formatted.data(),
                                        std::min(static_cast<std::size_t>(written), formatted.size() - 1)));
  }
}

bool ReadFile(DiagnosticLog& diagnostics, std::string_view path, std::vector<std::byte>& data) {
  std::FILE* file = OpenFile(path.data(), "rb");
  if (file == nullptr) {
    std::array<char, 512> message{};
    const int written = std::snprintf(
      message.data(), message.size(), "Unable to open shader file: %.*s", static_cast<int>(path.size()), path.data());
    if (written > 0) {
      diagnostics.Write(std::string_view(message.data(),
                                         std::min(static_cast<std::size_t>(written), message.size() - 1)));
    }
    return false;
  }
  if (std::fseek(file, 0, SEEK_END) != 0) {
    std::fclose(file);
    diagnostics.Write("Unable to seek shader file");
    return false;
  }
  const long size = std::ftell(file);
  if (size <= 0 || std::fseek(file, 0, SEEK_SET) != 0) {
    std::fclose(file);
    diagnostics.Write("Shader file is empty or unreadable");
    return false;
  }
  data.resize(static_cast<std::size_t>(size));
  const std::size_t read = std::fread(data.data(), 1, data.size(), file);
  std::fclose(file);
  if (read != data.size()) {
    data.clear();
    diagnostics.Write("Shader file read was incomplete");
    return false;
  }
  return true;
}

bool CreateResources(DiagnosticLog& diagnostics,
                     doggo::rhi::Device& device,
                     doggo::rhi::Format colorFormat,
                     std::span<const std::byte> vertexShaderCode,
                     std::span<const std::byte> fragmentShaderCode,
                     FixtureResources& resources) {
  const TexturedFixtureInputs inputs = GetTexturedFixtureInputs();
  const std::span<const std::byte> vertexBytes = std::as_bytes(inputs.m_Vertices);
  auto vertexBufferResult = device.CreateBuffer({
                                                  .m_Size = vertexBytes.size(),
                                                  .m_Usage = doggo::rhi::BufferUsage::Vertex,
                                                  .m_MemoryAccess = doggo::rhi::MemoryAccess::Device,
                                                  .m_DebugName = "Comet fixture vertices",
                                                },
                                                vertexBytes);
  if (!vertexBufferResult) {
    ReportError(diagnostics, "CreateBuffer(vertices)", vertexBufferResult.GetError());
    return false;
  }
  resources.m_VertexBuffer = vertexBufferResult.Value();

  const std::span<const std::byte> indexBytes = std::as_bytes(inputs.m_Indices);
  auto indexBufferResult = device.CreateBuffer({
                                                 .m_Size = indexBytes.size(),
                                                 .m_Usage = doggo::rhi::BufferUsage::Index,
                                                 .m_MemoryAccess = doggo::rhi::MemoryAccess::Device,
                                                 .m_DebugName = "Comet fixture indices",
                                               },
                                               indexBytes);
  if (!indexBufferResult) {
    ReportError(diagnostics, "CreateBuffer(indices)", indexBufferResult.GetError());
    return false;
  }
  resources.m_IndexBuffer = indexBufferResult.Value();

  auto textureResult = device.CreateImage({
                                            .m_Width = inputs.m_TextureWidth,
                                            .m_Height = inputs.m_TextureHeight,
                                            .m_Depth = 1,
                                            .m_MipLevels = 1,
                                            .m_Format = doggo::rhi::Format::R8G8B8A8Srgb,
                                            .m_Usage = doggo::rhi::ImageUsage::Sampled |
                                                       doggo::rhi::ImageUsage::TransferDestination,
                                            .m_DebugName = "Comet fixture texture",
                                          },
                                          inputs.m_TexturePixels);
  if (!textureResult) {
    ReportError(diagnostics, "CreateImage", textureResult.GetError());
    return false;
  }
  resources.m_Texture = textureResult.Value();

  auto samplerResult = device.CreateSampler({
    .m_MinFilter = doggo::rhi::Filter::Nearest,
    .m_MagFilter = doggo::rhi::Filter::Nearest,
    .m_DebugName = "Comet fixture sampler",
  });
  if (!samplerResult) {
    ReportError(diagnostics, "CreateSampler", samplerResult.GetError());
    return false;
  }
  resources.m_Sampler = samplerResult.Value();

  auto vertexShaderResult = device.CreateShader({
                                                  .m_Format = g_ShaderCodeFormat,
                                                  .m_Stage = doggo::rhi::ShaderStage::Vertex,
                                                  .m_DebugName = "Comet fixture vertex shader",
                                                },
                                                vertexShaderCode);
  if (!vertexShaderResult) {
    ReportError(diagnostics, "CreateShader(vertex)", vertexShaderResult.GetError());
    return false;
  }
  resources.m_VertexShader = vertexShaderResult.Value();

  auto fragmentShaderResult = device.CreateShader({
                                                    .m_Format = g_ShaderCodeFormat,
                                                    .m_Stage = doggo::rhi::ShaderStage::Fragment,
                                                    .m_DebugName = "Comet fixture fragment shader",
                                                  },
                                                  fragmentShaderCode);
  if (!fragmentShaderResult) {
    ReportError(diagnostics, "CreateShader(fragment)", fragmentShaderResult.GetError());
    return false;
  }
  resources.m_FragmentShader = fragmentShaderResult.Value();

  auto pipelineResult = device.CreateGraphicsPipeline({
    .m_VertexShader = resources.m_VertexShader,
    .m_FragmentShader = resources.m_FragmentShader,
    .m_ColorFormat = colorFormat,
    .m_VertexStride = sizeof(TexturedFixtureVertex),
    .m_VertexAttributes = g_VertexAttributes,
    .m_Bindings = g_Bindings,
    .m_DebugName = "Comet textured fixture pipeline",
  });
  if (!pipelineResult) {
    ReportError(diagnostics, "CreateGraphicsPipeline", pipelineResult.GetError());
    return false;
  }
  resources.m_Pipeline = pipelineResult.Value();

  const std::array<doggo::rhi::CombinedImageSamplerBinding, 1> sampledImages = {{
    {
      .m_Binding = 0,
      .m_Image = resources.m_Texture,
      .m_Sampler = resources.m_Sampler,
    },
  }};
  auto bindingSetResult = device.CreateBindingSet({
    .m_Pipeline = resources.m_Pipeline,
    .m_CombinedImageSamplers = sampledImages,
    .m_DebugName = "Comet textured fixture bindings",
  });
  if (!bindingSetResult) {
    ReportError(diagnostics, "CreateBindingSet", bindingSetResult.GetError());
    return false;
  }
  resources.m_BindingSet = bindingSetResult.Value();
  return true;
}

bool Render(DiagnosticLog& diagnostics,
            doggo::rhi::Device& device,
            const FixtureResources& resources,
            const doggo::rhi::Frame& frame,
            std::uint64_t& submission) {
  auto result = device.BeginRendering(frame.m_CommandList, {
                                                             .m_ClearColor = {
                                                               .m_Red = 0.015F,
                                                               .m_Green = 0.025F,
                                                               .m_Blue = 0.06F,
                                                               .m_Alpha = 1.0F,
                                                             },
                                                           });
  if (!result) {
    ReportError(diagnostics, "BeginRendering", result.GetError());
    return false;
  }
  result = device.BindPipeline(frame.m_CommandList, resources.m_Pipeline);
  if (!result) {
    ReportError(diagnostics, "BindPipeline", result.GetError());
    return false;
  }
  result = device.BindBindingSet(frame.m_CommandList, resources.m_BindingSet);
  if (!result) {
    ReportError(diagnostics, "BindBindingSet", result.GetError());
    return false;
  }
  result = device.BindVertexBuffer(frame.m_CommandList, resources.m_VertexBuffer);
  if (!result) {
    ReportError(diagnostics, "BindVertexBuffer", result.GetError());
    return false;
  }
  result = device.BindIndexBuffer(frame.m_CommandList, resources.m_IndexBuffer, doggo::rhi::IndexType::Uint16);
  if (!result) {
    ReportError(diagnostics, "BindIndexBuffer", result.GetError());
    return false;
  }
  result = device.DrawIndexed(frame.m_CommandList, static_cast<std::uint32_t>(g_Indices.size()));
  if (!result) {
    ReportError(diagnostics, "DrawIndexed", result.GetError());
    return false;
  }
  result = device.EndRendering(frame.m_CommandList);
  if (!result) {
    ReportError(diagnostics, "EndRendering", result.GetError());
    return false;
  }
  auto submitResult = device.SubmitFrame(frame.m_CommandList);
  if (!submitResult) {
    ReportError(diagnostics, "SubmitFrame", submitResult.GetError());
    return false;
  }
  submission = submitResult.Value();
  return true;
}

bool RetireResources(DiagnosticLog& diagnostics,
                     doggo::rhi::Device& device,
                     const FixtureResources& resources,
                     std::uint64_t submission) noexcept {
  bool successful = true;
  successful = static_cast<bool>(device.DestroyBindingSet(resources.m_BindingSet, submission)) && successful;
  successful = static_cast<bool>(device.DestroyPipeline(resources.m_Pipeline, submission)) && successful;
  successful = static_cast<bool>(device.DestroyShader(resources.m_VertexShader, submission)) && successful;
  successful = static_cast<bool>(device.DestroyShader(resources.m_FragmentShader, submission)) && successful;
  successful = static_cast<bool>(device.DestroySampler(resources.m_Sampler, submission)) && successful;
  successful = static_cast<bool>(device.DestroyImage(resources.m_Texture, submission)) && successful;
  successful = static_cast<bool>(device.DestroyBuffer(resources.m_VertexBuffer, submission)) && successful;
  successful = static_cast<bool>(device.DestroyBuffer(resources.m_IndexBuffer, submission)) && successful;
  auto idleResult = device.WaitIdle();
  if (!idleResult) {
    ReportError(diagnostics, "WaitIdle", idleResult.GetError());
    return false;
  }
  return successful;
}

}  // namespace

class RenderFixture::Impl final {
 public:
  DiagnosticLog m_Diagnostics;
  std::unique_ptr<doggo::rhi::Device> m_Device;
  FixtureResources m_Resources;
  doggo::rhi::Frame m_Frame;
  std::uint64_t m_LastSubmission = 0;
  std::uint32_t m_RenderedFrames = 0;
  bool m_HasFrame = false;
};

TexturedFixtureInputs GetTexturedFixtureInputs() noexcept {
  return {
    .m_Vertices = g_Vertices,
    .m_Indices = g_Indices,
    .m_TexturePixels = g_TexturePixels,
    .m_TextureWidth = 2,
    .m_TextureHeight = 2,
  };
}

std::unique_ptr<RenderFixture> RenderFixture::Create(doggo::platform::Application& application) {
  auto impl = std::make_unique<Impl>();
  impl->m_Diagnostics.Write("Comet Gate 2 render fixture started");

  std::vector<std::byte> vertexShaderCode;
  std::vector<std::byte> fragmentShaderCode;
  if (!ReadFile(impl->m_Diagnostics, g_VertexShaderPath, vertexShaderCode) ||
      !ReadFile(impl->m_Diagnostics, g_FragmentShaderPath, fragmentShaderCode)) {
    return nullptr;
  }
  impl->m_Diagnostics.Write("Shader files loaded");

  doggo::rhi::DeviceConfig deviceConfig{};
  deviceConfig.m_ApplicationName = "Comet Gate 2 Fixture";
  deviceConfig.m_EnableValidation = COMET_DEVELOPMENT_SERVICES != 0;
  deviceConfig.m_DiagnosticCallback = Diagnostic;
  deviceConfig.m_DiagnosticUserData = &impl->m_Diagnostics;
  auto deviceResult = doggo::rhi::CreateDevice(application, deviceConfig);
  if (!deviceResult) {
    ReportError(impl->m_Diagnostics, "CreateDevice", deviceResult.GetError());
    return nullptr;
  }
  impl->m_Device = std::move(deviceResult).Value();

  auto frameResult = impl->m_Device->BeginFrame();
  if (!frameResult) {
    ReportError(impl->m_Diagnostics, "BeginFrame", frameResult.GetError());
    return nullptr;
  }
  impl->m_Frame = frameResult.Value();
  impl->m_HasFrame = true;
  if (!CreateResources(impl->m_Diagnostics,
                       *impl->m_Device,
                       impl->m_Frame.m_ColorFormat,
                       vertexShaderCode,
                       fragmentShaderCode,
                       impl->m_Resources)) {
    return nullptr;
  }
  impl->m_Diagnostics.Write("Comet-owned fixture resources created");
  return std::unique_ptr<RenderFixture>(new RenderFixture(std::move(impl)));
}

RenderFixture::RenderFixture(std::unique_ptr<Impl> impl) noexcept
  : m_Impl(std::move(impl)) {
}

RenderFixture::~RenderFixture() {
  static_cast<void>(Shutdown());
}

bool RenderFixture::RenderFrame() {
  if (m_Impl == nullptr || m_Impl->m_Device == nullptr) {
    return false;
  }
  if (!m_Impl->m_HasFrame) {
    auto frameResult = m_Impl->m_Device->BeginFrame();
    if (!frameResult) {
      ReportError(m_Impl->m_Diagnostics, "BeginFrame", frameResult.GetError());
      return false;
    }
    m_Impl->m_Frame = frameResult.Value();
    m_Impl->m_HasFrame = true;
  }
  if (!Render(m_Impl->m_Diagnostics,
              *m_Impl->m_Device,
              m_Impl->m_Resources,
              m_Impl->m_Frame,
              m_Impl->m_LastSubmission)) {
    return false;
  }
  m_Impl->m_HasFrame = false;
  ++m_Impl->m_RenderedFrames;
  if (m_Impl->m_RenderedFrames == 1) {
    auto waitResult = m_Impl->m_Device->WaitIdle();
    if (!waitResult) {
      ReportError(m_Impl->m_Diagnostics, "WaitIdle(first frame)", waitResult.GetError());
      return false;
    }
    m_Impl->m_Diagnostics.Write("First frame completed on the GPU and was presented");
  }
  return true;
}

bool RenderFixture::Shutdown() noexcept {
  if (m_Impl == nullptr) {
    return true;
  }
  bool successful = true;
  if (m_Impl->m_Device != nullptr) {
    successful = RetireResources(
      m_Impl->m_Diagnostics, *m_Impl->m_Device, m_Impl->m_Resources, m_Impl->m_LastSubmission);
    if (m_Impl->m_Device->GetValidationErrorCount() != 0) {
      std::array<char, 128> message{};
      const int written = std::snprintf(message.data(),
                                        message.size(),
                                        "RHI reported %u validation error(s)",
                                        m_Impl->m_Device->GetValidationErrorCount());
      if (written > 0) {
        m_Impl->m_Diagnostics.Write(
          std::string_view(message.data(), std::min(static_cast<std::size_t>(written), message.size() - 1)));
      }
      successful = false;
    }
    m_Impl->m_Device->Shutdown();
  }
  if (successful) {
    m_Impl->m_Diagnostics.Write("Comet Gate 2 render fixture shut down cleanly");
  }
  m_Impl->m_Diagnostics.Close();
  m_Impl.reset();
  return successful;
}

std::uint32_t RenderFixture::GetRenderedFrameCount() const noexcept {
  return m_Impl != nullptr ? m_Impl->m_RenderedFrames : 0;
}

}  // namespace comet::presentation
