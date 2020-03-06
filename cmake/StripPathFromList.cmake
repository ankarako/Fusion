function(StripPathFromList InputList OutputList)
	set(FilenameList)
	# iterate the filepaths in the incoming list
	foreach(Filepath ${InputList})
		# get the filename of the current read filepath
		get_filename_component(Filename "${Filepath}" NAME)
		list(APPEND FilenameList ${Filename})
	endforeach(Filepath "${InpuList}")
	set(${OutputList} "${FilenameList}" PARENT_SCOPE)
endfunction()