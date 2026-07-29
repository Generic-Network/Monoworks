#include <common/Base.hh>
#include <common/Memory.hh>
#include <common/Math.hh>

#include <rhi/GraphicsAPI.hh>

namespace Monoworks
{
    class CStaticRenderer
    {
    public:
        static void Init() NOEXCEPT;
        static void Shutdown() NOEXCEPT;

        static void BeginRendering() NOEXCEPT;
        static void EndRendering() NOEXCEPT;

        NODISCARD static u32 GetCurrentFrameIndex() NOEXCEPT { return m_CurrentFrameIndex; };
    private:
        static u32 m_CurrentFrameIndex;
        static Ref<RHI::IGraphicsAPI> m_pInstance;
    };
}