set(PROJECT_NAME arnds)

################################################################################
# Source groups
################################################################################
set(Header_Files
    "first_order_logic/and.h"
    "first_order_logic/atom.h"
    "first_order_logic/atomic_formula.h"
    "first_order_logic/base_formula.h"
    "first_order_logic/base_term.h"
    "first_order_logic/binary_connective.h"
    "first_order_logic/common.h"
    "first_order_logic/constants.h"
    "first_order_logic/exists.h"
    "first_order_logic/first_order_logic.h"
    "first_order_logic/forall.h"
    "first_order_logic/function_term.h"
    "first_order_logic/iff.h"
    "first_order_logic/imp.h"
    "first_order_logic/not.h"
    "first_order_logic/or.h"
    "first_order_logic/parser.h"
    "first_order_logic/propositional_logic.h"
    "first_order_logic/quantifier.h"
    "first_order_logic/unary_connective.h"
    "first_order_logic/variable_term.h"
    "natural_deduction/rules.h"
    "natural_deduction/solver.h"
    "natural_deduction/solverparser.h"
    "natural_deduction/solvertree.h"
    "natural_deduction/types.h"
)
source_group("Header Files" FILES ${Header_Files})

set(Source_Files
    "first_order_logic/and.cpp"
    "first_order_logic/atom.cpp"
    "first_order_logic/atomic_formula.cpp"
    "first_order_logic/base_formula.cpp"
    "first_order_logic/base_term.cpp"
    "first_order_logic/binary_connective.cpp"
    "first_order_logic/constants.cpp"
    "first_order_logic/exists.cpp"
    "first_order_logic/forall.cpp"
    "first_order_logic/function_term.cpp"
    "first_order_logic/iff.cpp"
    "first_order_logic/imp.cpp"
    "first_order_logic/not.cpp"
    "first_order_logic/or.cpp"
    "first_order_logic/parser.cpp"
    "first_order_logic/quantifier.cpp"
    "first_order_logic/unary_connective.cpp"
    "first_order_logic/variable_term.cpp"
    "main.cpp"
    "natural_deduction/rules.cpp"
    "natural_deduction/solver.cpp"
    "natural_deduction/solverparser.cpp"
    "natural_deduction/solvertree.cpp"
)
source_group("Source Files" FILES ${Source_Files})

set(ALL_FILES
    ${Header_Files}
    ${Source_Files}
)

################################################################################
# Target
################################################################################
add_executable(${PROJECT_NAME} ${ALL_FILES})

use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")
set(ROOT_NAMESPACE arnds)

set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_GLOBAL_KEYWORD "Win32Proj"
)
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
    )
endif()
################################################################################
# Include directories
################################################################################
target_include_directories(${PROJECT_NAME} PUBLIC
	"${CMAKE_CURRENT_SOURCE_DIR}/."
)

################################################################################
# Compile definitions
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:Debug>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:Release>:"
            "NDEBUG"
        ">"
        "_CONSOLE;"
        "UNICODE;"
        "_UNICODE"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:Debug>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:Release>:"
            "NDEBUG"
        ">"
        "WIN32;"
        "_CONSOLE;"
        "UNICODE;"
        "_UNICODE"
    )
endif()

################################################################################
# Compile and link options
################################################################################
if(MSVC)
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:
                /Od
            >
            $<$<CONFIG:Release>:
                /O2;
                /Oi;
                /Gy
            >
            /permissive-;
            /sdl;
            /W3;
            ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
            ${DEFAULT_CXX_EXCEPTION_HANDLING};
            /Y-
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:
                /Od
            >
            $<$<CONFIG:Release>:
                /O2;
                /Oi;
                /Gy
            >
            /permissive-;
            /std:c++17;
            /sdl;
            /W3;
            ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
            ${DEFAULT_CXX_EXCEPTION_HANDLING};
            /Y-
        )
    endif()
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:
                /INCREMENTAL
            >
            $<$<CONFIG:Release>:
                /OPT:REF;
                /OPT:ICF;
                /INCREMENTAL:NO
            >
            /DEBUG;
            /SUBSYSTEM:CONSOLE
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:
                /INCREMENTAL
            >
            $<$<CONFIG:Release>:
                /OPT:REF;
                /OPT:ICF;
                /INCREMENTAL:NO
            >
            /DEBUG;
            /SUBSYSTEM:CONSOLE
        )
    endif()
endif()

