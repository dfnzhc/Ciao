#pragma once

namespace Ciao
{
    enum class BitmapType
	{
		_2D, _Cube
	};

	enum class BitmapFormat
	{
		UnsignedByte, Float,
	};

	struct Bitmap
	{
		Bitmap() = default;

		Bitmap(int w, int h, int comp, BitmapFormat fmt)
			: w_(w), h_(h), comp_(comp), fmt_(fmt), data_(w * h * comp * getBytesPerComponent(fmt))
		{
			initGetSetFuncs();
		}

		Bitmap(int w, int h, int d, int comp, BitmapFormat fmt)
			: w_(w), h_(h), d_(d), comp_(comp), fmt_(fmt), data_(w * h * d * comp * getBytesPerComponent(fmt))
		{
			initGetSetFuncs();
		}

		Bitmap(int w, int h, int comp, BitmapFormat fmt, const void* ptr)
			: w_(w), h_(h), comp_(comp), fmt_(fmt), data_(w * h * comp * getBytesPerComponent(fmt))
		{
			initGetSetFuncs();
			memcpy(data_.data(), ptr, data_.size());
		}

		int w_ = 0;
		int h_ = 0;
		int d_ = 1;
		int comp_ = 3;
		BitmapFormat fmt_ = BitmapFormat::UnsignedByte;
		BitmapType type_ = BitmapType::_2D;
		std::vector<uint8_t> data_;

		static int getBytesPerComponent(BitmapFormat fmt)
		{
			if (fmt == BitmapFormat::UnsignedByte) return 1;
			if (fmt == BitmapFormat::Float) return 4;
			return 0;
		}

		void setPixel(int x, int y, const glm::vec4& c)
		{
			(*this.*setPixelFunc)(x, y, c);
		}

		glm::vec4 getPixel(int x, int y) const
		{
			return ((*this.*getPixelFunc)(x, y));
		}
		
		void SaveEXR(const std::string& filename);

		void SavePNG(const std::string& filename);

	private:
		using setPixel_t = void(Bitmap::*)(int, int, const glm::vec4&);
		using getPixel_t = glm::vec4(Bitmap::*)(int, int) const;
		setPixel_t setPixelFunc = &Bitmap::setPixelUnsignedByte;
		getPixel_t getPixelFunc = &Bitmap::getPixelUnsignedByte;

		void initGetSetFuncs()
		{
			switch (fmt_)
			{
			case BitmapFormat::UnsignedByte:
				setPixelFunc = &Bitmap::setPixelUnsignedByte;
				getPixelFunc = &Bitmap::getPixelUnsignedByte;
				break;
			case BitmapFormat::Float:
				setPixelFunc = &Bitmap::setPixelFloat;
				getPixelFunc = &Bitmap::getPixelFloat;
				break;
			}
		}

		void setPixelFloat(int x, int y, const glm::vec4& c)
		{
			const int ofs = comp_ * (y * w_ + x);
			float* data = reinterpret_cast<float*>(data_.data());
			if (comp_ > 0) data[ofs + 0] = c.x;
			if (comp_ > 1) data[ofs + 1] = c.y;
			if (comp_ > 2) data[ofs + 2] = c.z;
			if (comp_ > 3) data[ofs + 3] = c.w;
		}

		glm::vec4 getPixelFloat(int x, int y) const
		{
			const int ofs = comp_ * (y * w_ + x);
			const float* data = reinterpret_cast<const float*>(data_.data());
			return glm::vec4(
				comp_ > 0 ? data[ofs + 0] : 0.0f,
				comp_ > 1 ? data[ofs + 1] : 0.0f,
				comp_ > 2 ? data[ofs + 2] : 0.0f,
				comp_ > 3 ? data[ofs + 3] : 0.0f);
		}

		void setPixelUnsignedByte(int x, int y, const glm::vec4& c)
		{
			const int ofs = comp_ * (y * w_ + x);
			if (comp_ > 0) data_[ofs + 0] = uint8_t(c.x * 255.0f);
			if (comp_ > 1) data_[ofs + 1] = uint8_t(c.y * 255.0f);
			if (comp_ > 2) data_[ofs + 2] = uint8_t(c.z * 255.0f);
			if (comp_ > 3) data_[ofs + 3] = uint8_t(c.w * 255.0f);
		}

		glm::vec4 getPixelUnsignedByte(int x, int y) const
		{
			const int ofs = comp_ * (y * w_ + x);
			return glm::vec4(
				comp_ > 0 ? float(data_[ofs + 0]) / 255.0f : 0.0f,
				comp_ > 1 ? float(data_[ofs + 1]) / 255.0f : 0.0f,
				comp_ > 2 ? float(data_[ofs + 2]) / 255.0f : 0.0f,
				comp_ > 3 ? float(data_[ofs + 3]) / 255.0f : 0.0f);
		}
	};
}
