// Copyright 2017 Dolphin Emulator Project
// Licensed under GPLv2+
// Refer to the license.txt file included.

#pragma once

#include <memory>
#include <vector>

#include "Common/GL/GLUtil.h"

#include "VideoCommon/AbstractFramebuffer.h"
#include "VideoCommon/AbstractStagingTexture.h"
#include "VideoCommon/AbstractTexture.h"

namespace OGL
{
class OGLTexture final : public AbstractTexture
{
public:
  explicit OGLTexture(const TextureConfig& tex_config);
  ~OGLTexture();

  void CopyRectangleFromTexture(const AbstractTexture* src,
                                const MathUtil::Rectangle<int>& src_rect, u32 src_layer,
                                u32 src_level, const MathUtil::Rectangle<int>& dst_rect,
                                u32 dst_layer, u32 dst_level) override;
  void ScaleRectangleFromTexture(const AbstractTexture* source,
                                 const MathUtil::Rectangle<int>& srcrect,
                                 const MathUtil::Rectangle<int>& dstrect) override;
  void ResolveFromTexture(const AbstractTexture* src, const MathUtil::Rectangle<int>& rect,
                          u32 layer, u32 level) override;
  void Load(u32 level, u32 width, u32 height, u32 row_length, const u8* buffer,
            size_t buffer_size) override;

  GLuint GetRawTexIdentifier() const;
  GLuint GetFramebuffer() const;

private:
  void BlitFramebuffer(OGLTexture* srcentry, const MathUtil::Rectangle<int>& src_rect,
                       u32 src_layer, u32 src_level, const MathUtil::Rectangle<int>& dst_rect,
                       u32 dst_layer, u32 dst_level);

  GLuint m_texId;
  GLuint m_framebuffer = 0;
};

class OGLStagingTexture final : public AbstractStagingTexture
{
public:
  OGLStagingTexture() = delete;
  ~OGLStagingTexture();

  void CopyFromTexture(const AbstractTexture* src, const MathUtil::Rectangle<int>& src_rect,
                       u32 src_layer, u32 src_level,
                       const MathUtil::Rectangle<int>& dst_rect) override;
  void CopyToTexture(const MathUtil::Rectangle<int>& src_rect, AbstractTexture* dst,
                     const MathUtil::Rectangle<int>& dst_rect, u32 dst_layer,
                     u32 dst_level) override;

  bool Map() override;
  void Unmap() override;
  void Flush() override;

  static std::unique_ptr<OGLStagingTexture> Create(StagingTextureType type,
                                                   const TextureConfig& config);

private:
  OGLStagingTexture(StagingTextureType type, const TextureConfig& config, GLenum target,
                    GLuint buffer_name, size_t buffer_size, char* map_ptr, size_t map_stride);

private:
  GLenum m_target;
  GLuint m_buffer_name;
  size_t m_buffer_size;
  GLsync m_fence = 0;
};

class OGLFramebuffer final : public AbstractFramebuffer
{
public:
  OGLFramebuffer(AbstractTextureFormat color_format, AbstractTextureFormat depth_format, u32 width,
                 u32 height, u32 layers, u32 samples, GLuint fbo);
  ~OGLFramebuffer() override;

  GLuint GetFBO() const { return m_fbo; }
  static std::unique_ptr<OGLFramebuffer> Create(const OGLTexture* color_attachment,
                                                const OGLTexture* depth_attachment);

protected:
  GLuint m_fbo;
};

}  // namespace OGL
