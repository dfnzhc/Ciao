#pragma once

namespace Ciao
{
	class DummyVAO final
	{
	public:
		DummyVAO();
		~DummyVAO();
		

		void bind();

	private:
		GLuint handle_;
	};
}
