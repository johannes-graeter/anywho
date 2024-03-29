macro(anywho_enable_ipo)
  include(CheckIPOSupported)
  if (NOT (CMAKE_CXX_COMPILER_ID MATCHES ".*GNU.*"))
      check_ipo_supported(RESULT result OUTPUT output)
      if(result)
        set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)
      else()
        message(SEND_ERROR "IPO is not supported: ${output}")
      endif()
  endif()
endmacro()
