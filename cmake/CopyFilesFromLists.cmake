function(CopyFilesFromLists InputFilepaths OutputFilepaths)
	# get the length of the lists
	# it is assumed that the lists have the smae length
	list(LENGTH InputFilepaths InputLength)
	# decrement the length so we do not get out of bounds in foreach loop
	math(EXPR outputLength "${inputLength} - 1")
	# list iteration 
	foreach(val RANGE ${outputLength})
		# get the element of the input list
		list(GET InputFilepaths ${val} inputFilepath)
		# get the element of the output list
		list(GET OutputFilepaths ${val} outputFilepath)
		# copy the input file to the output file
		message(STATUS "input path: ${inputFilepath}")
		message(STATUS "out path  : ${outputFilepath}")
		configure_file("${inputFilepath}" "${outputFilepath}" COPYONLY)
	endforeach(val RANGE ${outputLength})
endfunction(CopyFilesFromLists InputFilepaths OutputFilepaths)

