#include <Core/UI/LoadTextureFromFile.h>

namespace fu {
namespace fusion {
namespace ui {
bool LoadTextureFromFile(const char* filepath, GLuint* outTexture, int* outWidth, int* outHeight)
{
	/// Load From File
	int imageWidth = 0;
	int imageHeight = 0;
	unsigned char* imageData = stbi_load(filepath, &imageWidth, &imageHeight, NULL, 4);
	if (imageData == nullptr)
	{
		return false;
	}
	/// create gl texture id
	GLuint imageTexture;
	glGenTextures(1, &imageTexture);
	glBindTexture(GL_TEXTURE_2D, imageTexture);
	/// texture filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	/// upload data to device
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
	/// clean stb data
	stbi_image_free(imageData);

	*outTexture = imageTexture;
	*outWidth = imageWidth;
	*outHeight = imageHeight;

	return true;
}
}	///	!namespace ui
}	///	!namespace fusion
}	///	!namespace fu