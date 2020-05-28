#ifndef __OPTIXRAYTRACER_PUBLIC_RESOURCETABLES_H__
#define __OPTIXRAYTRACER_PUBLIC_RESOURCETABLES_H__

#include <RayTypes.h>
#include <string>
#include <map>

namespace fu {
namespace rt {

enum class ResType
{
	Filepath,
	ProgramName
};

using GeomFilepathKeyT = std::pair<GeometryType, VertexAttributes>;
static const std::map<GeomFilepathKeyT, std::string> GeometryProgramFilepaths =
{
	{ { GeometryType::Triangle, VertexAttributes::None },				"FusionLib/Resources/Programs/TriangleMeshGeometry.ptx" },
	{ { GeometryType::Triangle, VertexAttributes::Normals },			"FusionLib/Resources/Programs/TriangleMeshGeometryNormals.ptx" },
	{ { GeometryType::Triangle, VertexAttributes::Colors },				"FusionLib/Resources/Programs/TriangleMeshGeometryColors.ptx" },
	{ { GeometryType::Triangle, VertexAttributes::NormalsColors },		"FusionLib/Resources/Programs/TriangleMeshGeometryNormalsColors.ptx" },
	{ { GeometryType::Triangle, VertexAttributes::NormalsTexcoords },	"FusionLib/Resources/Programs/TriangleMeshGeometryNormalsTexcoords.ptx" },
};

using GeomProgramKeyT = std::pair<GeometryType, GeometryProgType>;
static const std::map<GeomProgramKeyT, std::string> GeometryProgramNames =
{
	{ {GeometryType::Triangle, GeometryProgType::Intersection},	"triangle_mesh_intersect" },
	{ {GeometryType::Triangle, GeometryProgType::BoundingBox},	"triangle_mesh_bounds" },
};

static const std::map<MaterialType, std::string> MaterialProgramFilepaths = 
{
	{ MaterialType::SolidColor,			"FusionLib/Resources/Programs/SolidColorHitGroup.ptx" },
	{ MaterialType::RadianceTexture,	"FusionLib/Resources/Programs/RadianceTexture.ptx" },
	{ MaterialType::Phong,				"FusionLib/Resources/Programs/Phong.ptx" },
	{ MaterialType::VertexColor,		"FusionLib/Resources/Programs/VertexColor.ptx" },
};

using MatProgramKeyT = std::pair<MaterialType, MaterialProgType>;
static const std::map<MatProgramKeyT, std::string> MaterialProgramNames =
{
	{ { MaterialType::SolidColor,		MaterialProgType::ClosestHit },		"solid_color_closest_hit" },
	{ { MaterialType::SolidColor,		MaterialProgType::AnyHit },			"solid_color_any_hit" },
	{ { MaterialType::RadianceTexture,	MaterialProgType::ClosestHit },		"radiance_texture_closest_hit" },
	{ { MaterialType::RadianceTexture,	MaterialProgType::AnyHit },			"radiance_texture_any_hit" },
	{ { MaterialType::Phong,			MaterialProgType::ClosestHit },		"phong_closest_hit" },
	{ { MaterialType::Phong,			MaterialProgType::AnyHit },			"phong_any_hit" },
};
}	///	!namespace rt
}	///	!namespace fu
#endif	///	!__OPTIXRAYTRACER_PUBLIC_RESOURCETABLES_H__