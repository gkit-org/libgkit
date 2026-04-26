#include "gkit/graphic/opengl/config.hpp"

#include <glad/gl.h>

namespace gkit::graphic::opengl::viewport {

	auto SetViewport(int x, int y, int width, int height) -> void
	{
		glViewport(x, y, width, height);
	}

	auto SetViewport(int width, int height) -> void
	{
		glViewport(0, 0, width, height);
	}
}
