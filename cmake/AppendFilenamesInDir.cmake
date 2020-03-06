function(AppendFilenamesInDir InputPath InputFilenames OutputList)
	set(FilepathList)
	# iterate through the input fileanmes
	foreach(Filename ${InputFilenames})
		# appen the filename in the list
		list(APPEND FilepathList "${InputPath}/${Filename}")
	endforeach(Filename ${InputFilenames})
	set(${OutputList} ${FilepathList} PARENT_SCOPE)
endfunction()