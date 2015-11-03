#include "ShaderProgram.h"

#include <fstream>
using std::ifstream;
using std::ios;

#include <sstream>
#include <sys/stat.h>

namespace ShaderInfo
{
	struct ShaderFileExtension
	{
		const char* ext;
		Shaders::ShaderType type;
	};

	struct ShaderFileExtension Extensions[]
	{
		{".vs", Shaders::VERTEX},
		{".vert", Shaders::VERTEX},
		{".gs", Shaders::GEOMETRY},
		{".geom", Shaders::GEOMETRY},
		{".tcs", Shaders::TESS_CONTROL},
		{".tes", Shaders::TESS_EVALUATION},
		{".fs", Shaders::FRAGMENT},
		{".frag", Shaders::FRAGMENT},
		{".cs", Shaders::COMPUTE}
	};
}

