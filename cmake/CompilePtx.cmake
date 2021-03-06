function(CompilePtx Target PtxTargetProject CudaSourceFilepath FolderName CudaOutputDir PtxName)
	# message(STATUS "Compile PTX Target: ")
	# message(STATUS "Target         : ${Target}")
	# message(STATUS "PtxTarget      : ${PtxTargetProject}")
	# message(STATUS "Cuda src path  : ${CudaSourceFilepath}")
	# message(STATUS "Project Folder : ${FolderName}")
	# message(STATUS "CUDA output dir: ${CudaOutputDir}")
	# message(STATUS "Ptx Filename   : ${PtxName}")
	set(PTXTarget ${PtxTargetProject})
	cuda_compile_ptx(PTXTarget "${CudaSourceFilepath}")
	add_custom_target(${PtxTargetProject}PTX ALL DEPENDS "${PtxTargetProject}" "${CudaSourceFilepath}" SOURCES "${CudaSourceFilepath}")
	# tedious command to rename the generated ptx files
	add_custom_command(TARGET ${Target} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${PTXTarget} ${CudaOutputDir}/${PtxName})
	add_custom_command(TARGET ${PtxTargetProject}PTX POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${PTXTarget} ${CudaOutputDir}/${PtxName})
	set_target_properties(${PtxTargetProject}PTX PROPERTIES FOLDER "${FolderName}")
endfunction(CompilePtx Target PtxTargetProject CudaSourceFilepath FolderName CudaOutputDir PtxName)