function(PrintList InputList InputText)
	string(REPLACE ";" "\n" text "${InputList}")
	message(STATUS "${InputText}: \n${text}\n")
endfunction()