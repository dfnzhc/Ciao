#include "pch.h"
#include "DrawStaff.h"

namespace Ciao
{
	DummyVAO::DummyVAO()
	{
		glCreateVertexArrays(1, &handle_);
	}

	DummyVAO::~DummyVAO()
	{
		glDeleteVertexArrays(1, &handle_);
	}

	void DummyVAO::bind()
	{
		glBindVertexArray(handle_);
	}
}
