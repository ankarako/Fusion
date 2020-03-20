#ifndef __OPTIXRAYTRACER_PUBLIC_SYSTEMS_PROGRAMMAP_H__
#define __OPTIXRAYTRACER_PUBLIC_SYSTEMS_PROGRAMMAP_H__

#include <tuple>
#include <map>
#include <string>

namespace fu {
namespace rt {
///
enum class ResType
{
	Unknown = 0,
	Filepath,
	ProgramName
};

using ResKeyType = std::pair<ResType, std::pair<std::string, std::string>>;
using ResourceDesc = std::map<ResKeyType, std::string>;

static const ResourceDesc k_ResourceDesc =
{
	{ { ResType::Filepath,		{"Pointcloud",				"Color" }},		"FusionLib/Resources/Programs/PointCloudColor.ptx" },
	{ { ResType::ProgramName,	{"PointcloudIntersection",	"Color" }}, 	"pointcloud_intersect" },
	{ { ResType::ProgramName,	{"PointcloudBoundingBox",	"Color" }}, 	"pointcloud_bounds" },
	{ { ResType::ProgramName,	{"PointcloudClosestHit",	"Color" }}, 	"pointcloud_closest_hit" },
	{ { ResType::ProgramName,	{"PointcloudAnyHit",		"Color" }}, 	"pointcloud_any_hit" },
	/// TODO: Point cloud with normals
	/// Triangle Meshes
	{ { ResType::Filepath,		{"TriangleMesh",				"NoAttrib" }},	"FusionLib/Resources/Programs/TriangleMesh.ptx" },
	{ { ResType::Filepath,		{"TriangleMeshHitgroup",		"NoAttrib" }},	"FusionLib/Resources/Programs/TriangleColorHitGroup.ptx" },
	{ { ResType::ProgramName,	{"TriangleMeshIntersection",	"NoAttrib" }}, 	"triangle_mesh_intersect" },
	{ { ResType::ProgramName,	{"TriangleMeshBoundingBox",		"NoAttrib" }}, 	"triangle_mesh_bounds" },
	{ { ResType::ProgramName,	{"TriangleMeshClosestHit",		"NoAttrib" }}, 	"closest_hit_radiance" },
	{ { ResType::ProgramName,	{"TriangleMeshAnyHit",			"NoAttrib" }}, 	"any_hit" },
	/// Triangle meshes with normals
	{ { ResType::Filepath,		{"TriangleMesh",				"Normals" }},	"FusionLib/Resources/Programs/TriangleMesh.ptx" },
	{ { ResType::Filepath,		{"TriangleMeshHitgroup",		"Normals" }},	"FusionLib/Resources/Programs/TriangleColorHitGroup.ptx" },
	{ { ResType::ProgramName,	{"TriangleMeshIntersection",	"Normals" }}, 	"triangle_mesh_intersect" },
	{ { ResType::ProgramName,	{"TriangleMeshBoundingBox",		"Normals" }}, 	"triangle_mesh_bounds" },
	{ { ResType::ProgramName,	{"TriangleMeshClosestHit",		"Normals" }}, 	"closest_hit_radiance" },
	{ { ResType::ProgramName,	{"TriangleMeshAnyHit",			"Normals" }}, 	"any_hit" },
	/// triangle meshes with vertex colors
	{ { ResType::Filepath,		{"TriangleMesh",				"NormalsColor" }},	"FusionLib/Resources/Programs/TriangleMesh.ptx" },
	{ { ResType::Filepath,		{"TriangleMeshHitgroup",		"NormalsColor" }},	"FusionLib/Resources/Programs/TriangleColorHitGroup.ptx" },
	{ { ResType::ProgramName,	{"TriangleMeshIntersection",	"NormalsColor" }}, 	"triangle_mesh_intersect" },
	{ { ResType::ProgramName,	{"TriangleMeshBoundingBox",		"NormalsColor" }}, 	"triangle_mesh_bounds" },
	{ { ResType::ProgramName,	{"TriangleMeshClosestHit",		"NormalsColor" }}, 	"closest_hit_radiance" },
	{ { ResType::ProgramName,	{"TriangleMeshAnyHit",			"NormalsColor" }}, 	"any_hit" },
};

static std::string GetResourceFilepath(const std::string& intersectType, const std::string & attributes)
{
	ResKeyType key = std::make_pair(ResType::Filepath, std::make_pair(intersectType, attributes));
	auto& it = k_ResourceDesc.find(key);
	if (it == k_ResourceDesc.end())
	{
		return "invalid";
	}
	return k_ResourceDesc.at(key);
}

static std::string GetResourceProgName(const std::string& intersectType, const std::string& attributes)
{
	ResKeyType key = std::make_pair(ResType::ProgramName, std::make_pair(intersectType, attributes));
	auto& it = k_ResourceDesc.find(key);
	if (it == k_ResourceDesc.end())
	{
		return "invalid";
	}
	return k_ResourceDesc.at(key);
}
}	///	!namespace rt
}	///	!namespace fu
#endif	///	!__OPTIXRAYTRACER_PUBLIC_SYSTEMS_PROGRAMMAP_H__