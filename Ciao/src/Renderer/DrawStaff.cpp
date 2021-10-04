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

	Grid::Grid()
	{
		glCreateVertexArrays(1, &vao_);
	}

	Grid::~Grid()
	{
		glDeleteVertexArrays(1, &vao_);
	}

	void Grid::draw()
	{
		progGrid_.useProgram();
		glBindVertexArray(vao_);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDrawArraysInstancedBaseInstance(GL_TRIANGLES, 0, 6, 1, 0);
		glDisable(GL_BLEND);
	}
}
