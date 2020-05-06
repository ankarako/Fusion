#include <Models/PerfcapPlayerModel.h>
#include <Perfcap/PerfcapImporter.h>
#include <Core/Helpers.h>
#include <plog/Log.h>
#include <filesystem>

namespace fu {
namespace fusion {

struct PerfcapPlayerModel::Impl
{
	using importer_t = std::shared_ptr<io::PerfcapImporter>;

	importer_t	m_PerfcapImporter;
	rxcpp::subjects::subject<std::string>	m_PerfcapFilepathFlowInSubj;
	rxcpp::subjects::subject<io::MeshData>	m_TemplateMeshDataFlowOutSubj;
	rxcpp::subjects::subject<SequenceItem>	m_SequenceItemFlowOutSubj;
	/// Construction
	Impl() { }
};	///	!struct Impl
/// Construction
PerfcapPlayerModel::PerfcapPlayerModel()
	: m_Impl(spimpl::make_unique_impl<Impl>())
{ 
	///==============================
	///	perfcap filepath flow in task
	///==============================
	m_Impl->m_PerfcapFilepathFlowInSubj.get_observable().as_dynamic()
		.subscribe([this](const std::string& filepath) 
	{
		
		m_Impl->m_PerfcapImporter = std::make_shared<io::PerfcapImporter>(filepath);
		/// map template mesh
		io::MeshData data = io::CreateMeshData();
		auto& tmp_mesh = m_Impl->m_PerfcapImporter->GetTemplateMesh();
		
		if (!tmp_mesh.Vertices.empty())
		{
			int count = tmp_mesh.Vertices.size();
			int bsize = tmp_mesh.Vertices.size() * sizeof(io::Vec3<float>);
			data->VertexBuffer = CreateBufferCPU<float3>(count);
			for (int v = 0; v < tmp_mesh.Vertices.size(); v++)
			{
				float3 vertex = make_float3(
					tmp_mesh.Vertices[v].x,
					-tmp_mesh.Vertices[v].y,
					tmp_mesh.Vertices[v].z
				);
				data->VertexBuffer->Data()[v] = vertex;
			}
		}
		if (!tmp_mesh.Normals.empty())
		{
			data->HasNormals = true;
			int count = tmp_mesh.Normals.size();
			int bsize = count * sizeof(io::Vec3<float>);
			data->NormalBuffer = CreateBufferCPU<float3>(count);
			std::memcpy(data->NormalBuffer->Data(), tmp_mesh.Normals.data(), bsize);
		}
		if (!tmp_mesh.Colors.empty())
		{
			data->HasColors = true;
			int count = tmp_mesh.Colors.size();
			int bsize = count * sizeof(io::Vec3<float>);
			data->ColorBuffer = CreateBufferCPU<uchar4>(count);
			/// first we have to convert float3 colors to uchar4 :)
			for (int v = 0; v < tmp_mesh.Colors.size(); v++)
			{
				float3 color = make_float3(
					tmp_mesh.Colors[v].x,
					tmp_mesh.Colors[v].y,
					tmp_mesh.Colors[v].z);
				data->ColorBuffer->Data()[v] = MakeColor(color);
			}
			
		}
		if (!tmp_mesh.Faces.empty())
		{
			data->HasFaces = true;
			int count = tmp_mesh.Faces.size();
			int bsize = count * sizeof(io::Vec3<int>);
			data->TIndexBuffer = CreateBufferCPU<uint3>(count);
			std::memcpy(data->TIndexBuffer->Data(), tmp_mesh.Faces.data(), bsize);
		}
		m_Impl->m_TemplateMeshDataFlowOutSubj.get_subscriber().on_next(data);
		/// create a sequence item
		SequenceItem item;
		item.Name = std::filesystem::path(filepath).filename().generic_string();
		item.Duration = m_Impl->m_PerfcapImporter->GetFrameCount();
		item.SeqFrameStart = 0;
		item.SeqFrameEnd = item.Duration;
		item.FrameStart = 0;
		item.FrameEnd = item.Duration;
		item.Expanded = false;
		item.Type = SequenceItemType::Animation;
		m_Impl->m_SequenceItemFlowOutSubj.get_subscriber().on_next(item);
	});
}
rxcpp::observer<std::string> fu::fusion::PerfcapPlayerModel::PerfcapFilepathFlowIn()
{
	return m_Impl->m_PerfcapFilepathFlowInSubj.get_subscriber().get_observer().as_dynamic();
}

rxcpp::observable<io::MeshData> fu::fusion::PerfcapPlayerModel::TemplateMeshDataFlowOut()
{
	return m_Impl->m_TemplateMeshDataFlowOutSubj.get_observable().as_dynamic();
}
rxcpp::observable<SequenceItem> fu::fusion::PerfcapPlayerModel::SequenceItemFlowOut()
{
	return m_Impl->m_SequenceItemFlowOutSubj.get_observable().as_dynamic();
}
}	///	!namespace fusion
}	///	!namespace fu