#pragma once

namespace Ciao
{
    enum BitmapType
    {
        BitmapType_2D,
        BitmapType_Cube
    };

    enum BitmapFormat
    {
        BitmapFormat_UnsignedByte,
        BitmapFormat_Float,
    };


    struct Bitmap
    {
        Bitmap() = default;
        Bitmap(int w, int h, int comp, BitmapFormat fmt)
            : m_W(w), m_H(h), m_Comp(comp), m_Fmt(fmt), m_Data(w * h * comp * GetBytesPerComponent(fmt))
        {
            InitGetSetFuncs();
        }

        Bitmap(int w, int h, int d, int comp, BitmapFormat fmt)
            : m_W(w), m_H(h), m_D(d), m_Comp(comp), m_Fmt(fmt), m_Data(w * h * d * comp * GetBytesPerComponent(fmt))
        {
            InitGetSetFuncs();
        }

        Bitmap(int w, int h, int comp, BitmapFormat fmt, const void* ptr)
            : m_W(w), m_H(h), m_Comp(comp), m_Fmt(fmt), m_Data(w * h * comp * GetBytesPerComponent(fmt))
        {
            InitGetSetFuncs();
            memcpy(m_Data.data(), ptr, m_Data.size());
        }

        
        int m_W = 0;
        int m_H = 0;
        int m_D = 1;
        int m_Comp = 3;
        BitmapType m_Type = BitmapType_2D;
        BitmapFormat m_Fmt = BitmapFormat_UnsignedByte;
        std::vector<uint8_t> m_Data;

        static int GetBytesPerComponent(BitmapFormat fmt)
        {
            if (fmt == BitmapFormat_UnsignedByte) return 1;
            if (fmt == BitmapFormat_Float) return 4;
            return 0;
        }

        void SetPixel(int x, int y, const glm::vec4& c)
        {
            (*this.*SetPixelFunc)(x, y, c);
        }

        glm::vec4 GetPixel(int x, int y) const
        {
            return ((*this.*GetPixelFunc)(x, y));
        }

    private:
        using SetPixel_t = void(Bitmap::*)(int, int, const glm::vec4&);
        using GetPixel_t = glm::vec4(Bitmap::*)(int, int) const;
        SetPixel_t SetPixelFunc = &Bitmap::SetPixelUnsignedByte;
        GetPixel_t GetPixelFunc = &Bitmap::GetPixelUnsignedByte;
        
        void InitGetSetFuncs()
        {
            switch (m_Fmt)
            {
            case BitmapFormat_UnsignedByte:
                SetPixelFunc = &Bitmap::SetPixelUnsignedByte;
                GetPixelFunc = &Bitmap::GetPixelUnsignedByte;
                break;
            case BitmapFormat_Float:
                SetPixelFunc = &Bitmap::SetPixelFloat;
                GetPixelFunc = &Bitmap::GetPixelFloat;
                break;
            }
        }

        void SetPixelFloat(int x, int y, const glm::vec4& c)
        {
            const int ofs = m_Comp * (y * m_W + x);
            float* data = reinterpret_cast<float*>(m_Data.data());
            if (m_Comp > 0) data[ofs + 0] = c.x;
            if (m_Comp > 1) data[ofs + 1] = c.y;
            if (m_Comp > 2) data[ofs + 2] = c.z;
            if (m_Comp > 3) data[ofs + 3] = c.w;
        }

        glm::vec4 GetPixelFloat(int x, int y) const
        {
            const int ofs = m_Comp * (y * m_W + x);
            const float* data = reinterpret_cast<const float*>(m_Data.data());
            return glm::vec4{
                m_Comp > 0 ? data[ofs + 0] : 0.0f,
                m_Comp > 1 ? data[ofs + 1] : 0.0f,
                m_Comp > 2 ? data[ofs + 2] : 0.0f,
                m_Comp > 3 ? data[ofs + 3] : 0.0f};
        }

        void SetPixelUnsignedByte(int x, int y, const glm::vec4& c)
        {
            const int ofs = m_Comp * (y * m_W + x);
            if (m_Comp > 0) m_Data[ofs + 0] = uint8_t(c.x * 255.0f);
            if (m_Comp > 1) m_Data[ofs + 1] = uint8_t(c.y * 255.0f);
            if (m_Comp > 2) m_Data[ofs + 2] = uint8_t(c.z * 255.0f);
            if (m_Comp > 3) m_Data[ofs + 3] = uint8_t(c.w * 255.0f);
        }

        glm::vec4 GetPixelUnsignedByte(int x, int y) const
        {
            const int ofs = m_Comp * (y * m_W + x);
            return glm::vec4{
                m_Comp > 0 ? float(m_Data[ofs + 0]) / 255.0f : 0.0f,
                m_Comp > 1 ? float(m_Data[ofs + 1]) / 255.0f : 0.0f,
                m_Comp > 2 ? float(m_Data[ofs + 2]) / 255.0f : 0.0f,
                m_Comp > 3 ? float(m_Data[ofs + 3]) / 255.0f : 0.0f};
        }
    };

}


