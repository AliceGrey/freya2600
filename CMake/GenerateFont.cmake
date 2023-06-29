
macro(generate_font)

    if(NOT EXISTS ${CMAKE_SOURCE_DIR}/Source/Font.hpp)

        set(_ttf_file ${CMAKE_BINARY_DIR}/ttf/Hack-Regular.ttf)

        if(NOT EXISTS ${_ttf_file})

            set(_zip_url "https://github.com/source-foundry/Hack/releases/download/v3.003/Hack-v3.003-ttf.zip")
            set(_zip_file ${CMAKE_BINARY_DIR}/Hack.zip)

            message(STATUS "Downloading ${_zip_url}")

            file(DOWNLOAD
                "${_zip_url}"
                ${_zip_file}
            )

            execute_process(
                COMMAND ${CMAKE_COMMAND} -E tar xf ${_zip_file}
                WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            )

        endif()

        message(STATUS "Generating Font")

        execute_process(
            COMMAND ${Python3_EXECUTABLE} Scripts/generate-font.py --font-size 16 --ttf-file "${_ttf_file}"
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        )

    endif()

endmacro()