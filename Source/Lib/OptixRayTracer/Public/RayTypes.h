#ifndef __OPTIXRAYTRACER_PUBLIC_RAYTYPES_H__
#define __OPTIXRAYTRACER_PUBLIC_RAYTYPES_H__

namespace fu {
namespace rt {
///	\enum GeometryType
///	\brief RayTracer's Available Geometries
enum class GeometryType : unsigned int
{
	Triangle,		
	Sphere,
	Parallelogram,
	Pointcloud,
	///
	GeometryTypeCount,
};
///	\enum VertexAttributes
///	\brief VertexAttributes for Triagle Geometries
enum class VertexAttributes : unsigned int
{
	None,
	Normals,
	Colors,
	Texcoords,
	NormalsColors,
	NormalsTexcoords,
	NormalsColorsTexcoords,
	///
	VertexAttributesCount
};
///	\enum MaterialType
///	\brief RayTracer's AvailableMaterials
enum class MaterialType : unsigned int
{
	SolidColor,
	VertexColor,
	RadianceTexture,
	Phong
};
///	\enum GeometryProgType
///	\brief the geomtry program type
enum class GeometryProgType : unsigned int
{
	Intersection,
	BoundingBox
};
///	\enum MetrialProType
///	\brief the material program types
enum class MaterialProgType : unsigned int
{
	ClosestHit,
	AnyHit
};
}	///	!namespace rt
}	///	!namespace fu
#endif	///	!__OPTIXRAYTRACER_PUBLIC_RAYTYPES_H__