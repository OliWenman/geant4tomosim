# Find the native numpy includes
# This module defines
#  NUMPY_INCLUDE_DIR, where to find numpy/arrayobject.h, etc.
#  NUMPY_FOUND, If false, do not try to use numpy headers.

if (NUMPY_INCLUDE_DIR)
  # in cache already
  set (Numpy_FIND_QUIETLY TRUE)
endif (NUMPY_INCLUDE_DIR)

if(NOT Numpy_FIND_QUIETLY)
    execute_process(COMMAND ${PYTHON_EXECUTABLE} -c "import numpy; print(numpy.get_include())"
        OUTPUT_VARIABLE NUMPY_INCLUDE_DIR
        RESULT_VARIABLE NUMPY_NOT_FOUND
        OUTPUT_STRIP_TRAILING_WHITESPACE)
endif()

if (NUMPY_INCLUDE_DIR MATCHES "Traceback")
    # Did not successfully include numpy
    message(ERROR "Unexpected NUMPY_INCLUDE_DIR='${NUMPY_INCLUDE_DIR}'.")
    set(NUMPY_FOUND FALSE)
else()
    # successful
    message(STATUS "Found NUMPY_INCLUDE_DIR='${NUMPY_INCLUDE_DIR}'.")
    if(NOT EXISTS ${NUMPY_INCLUDE_DIR}/numpy/arrayobject.h)
        message(STATUS "Can't find numpy/arrayobject.h, please install python-numpy-devel package")
    else()
        set (NUMPY_FOUND TRUE)
        execute_process(COMMAND ${PYTHON_EXECUTABLE} -c "import numpy; print(numpy.__version__)"
            OUTPUT_VARIABLE numpy_version_number
            RESULT_VARIABLE numpy_return_value
            OUTPUT_STRIP_TRAILING_WHITESPACE)
        set (NUMPY_VERSION_STRING ${numpy_version_number})
    endif()
endif()

if (NUMPY_FOUND)
    if (NOT Numpy_FIND_QUIETLY)
        message (STATUS "Numpy headers found at ${NUMPY_INCLUDE_DIR}, version ${numpy_version_number}.")
    endif()
else()
    if (Numpy_FIND_REQUIRED)
        message (FATAL_ERROR "Numpy headers missing, please install python-numpy-devel package")
    endif()
endif()

if(WIN32)
  STRING(REGEX REPLACE "\\\\" "/" NUMPY_INCLUDE_DIR ${NUMPY_INCLUDE_DIR})
endif()

MARK_AS_ADVANCED (NUMPY_INCLUDE_DIR)
MARK_AS_ADVANCED (NUMPY_VERSION_STRING)
