# Groups source files in Visual Studio (maybve XCode too filters/folders)
#   \Name GroupSource
#   \param[in]: SourceFiles, a variable (list) of files to put in a folder
#   \param[in]: FolderName, The folder name to put the given source files into
function (GroupSource SourceFiles FolderName)
    foreach(FILE ${SourceFiles})
    # get the directory of the file    
    get_filename_component(PARENT_DIR "${FILE}" DIRECTORY)
    # Remove common directory prefix to meke the group
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "" GROUP "${PARENTDIR}")
    # make sure for double backslashes
    string(REPLACE "/" "\\" GROUP "${GROUP}")
    source_group("${FolderName}" FILES "${FILE}")
    endforeach()
endfunction()