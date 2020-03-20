#ifndef __IO_PUBLIC_PERFCAP_TEXTUREATTRIBUTE_H__
#define __IO_PUBLIC_PERFCAP_TEXTUREATTRIBUTE_H__

namespace fu {
namespace io {

struct TextureAttribute
{
	float 	u1, u2;
	float 	v1, v2;
	int 	texId1, texId2;
	float 	w;

	template <typename F>
	void serialize(F& buf) const
	{
		buf << u1 << v1 << u2 << v2 << texId1 << texId2 << w;
	}

	template <typename F>
	void parse(F& buf)
	{
		buf >> u1 >> v1 >> u2 >> v2 >> texId1 >> texId2 >> w;
	}

	bool operator==(const TextureAttribute& other) const
	{
		return ((u1 == other.u1) 
			&& (v1 == other.v1) 
			&& (u2 == other.u2) 
			&& (v2 == other.v2) 
			&& (texId1 == other.texId1) 
			&& (texId2 == other.texId2) 
			&& (w == other.w));
	}
};	///	!struct TextureAttribute
}	///	!namespace io
}	///	!namespace fu
#endif	///	!__IO_PUBLIC_PERFCAP_TEXTUREATTRIBUTE_H__