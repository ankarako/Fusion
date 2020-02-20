#ifndef __FUSION_PUBLIC_CORE_UI_LOADTEXTUREFROMFILE_H__
#define __FUSION_PUBLIC_CORE_UI_LOADTEXTUREFROMFILE_H__

#include <GL/gl3w.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace fusion {
namespace ui {
bool LoadTextureFromFile(const char* filepath, GLuint* outTexture, int* outWidth, int* outHeight);
}	///	!namespace ui
}	///	!namespace fusion
#endif	///	!__FUSION_PUBLIC_CORE_UI_LOADTEXTUREFROMFILE_H__