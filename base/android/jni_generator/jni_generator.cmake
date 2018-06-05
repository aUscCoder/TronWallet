cmake_minimum_required(VERSION 3.1.0)

function(GenerateJNIFiles JNI_INPUT JNI_ROOT JNI_INCLUDE JNI_GENERATED)
    GET_FILENAME_COMPONENT(JNI_ROOT ${JNI_ROOT} ABSOLUTE)

    SET(GENERATOR "${GenerateJNIFilesPath}/jni_generator.py")
    MESSAGE(STATUS "GENERATOR = ${GENERATOR}")

    FOREACH(JNI_FILE ${JNI_INPUT})
        GET_FILENAME_COMPONENT(JNI_FILE ${JNI_FILE} ABSOLUTE)
        GET_FILENAME_COMPONENT(JNI_NAME ${JNI_FILE} NAME_WE)
        SET(CUR_JNI_GEN ${JNI_ROOT}/${JNI_NAME}_jni.h)
        SET(JNI_GEN_FILES ${JNI_GEN_FILES} ${CUR_JNI_GEN})
        MESSAGE(STATUS "CUR_JNI_GEN = ${CUR_JNI_GEN}")

        # Cannot use ARGN directly with list() command,
        # so copy it to a variable first.
        set (extra_macro_args ${ARGN})

        # Did we get any optional args?
        list(LENGTH extra_macro_args num_extra_args)
        if (${num_extra_args} GREATER 0)
            list(GET extra_macro_args 0 JNI_GEN_CLASS)
            message ("Got an optional arg: ${JNI_GEN_CLASS}")
        endif ()

        if (${JNI_GEN_CLASS})
            SET(CUR_JNI_CLASS_GEN ${JNI_ROOT}/../${JNI_NAME}.tmpl)
            MESSAGE(STATUS "CUR_JNI_CLASS_GEN = ${CUR_JNI_CLASS_GEN}")
            ADD_CUSTOM_COMMAND(
                OUTPUT ${CUR_JNI_GEN} ${CUR_JNI_CLASS_GEN}
                COMMAND ${GENERATOR}
                    "--input_file" ${JNI_FILE}
                    "--optimize_generation=1"
                    "--ptr_type=long"
                    "--output_dir" ${JNI_ROOT}
                    "--includes" "base/android/jni_generator/jni_generator_helper.h${JNI_INCLUDE}"
                    "--native_exports_optional"
                    "--generate_class"
                DEPENDS ${GENERATOR} ${JNI_FILE}
                WORKING_DIRECTORY ${JNI_ROOT}
            )
        else()
            ADD_CUSTOM_COMMAND(
                OUTPUT ${CUR_JNI_GEN}
                COMMAND ${GENERATOR}
                    "--input_file" ${JNI_FILE}
                    "--optimize_generation=1"
                    "--ptr_type=long"
                    "--output_dir" ${JNI_ROOT}
                    "--includes" "base/android/jni_generator/jni_generator_helper.h${JNI_INCLUDE}"
                    "--native_exports_optional"
                DEPENDS ${GENERATOR} ${JNI_FILE}
                WORKING_DIRECTORY ${JNI_ROOT}
            )
        endif()
    endforeach(JNI_FILE ${JNI_INPUT})
    SET(${JNI_GENERATED} ${JNI_GEN_FILES} PARENT_SCOPE)
endfunction()

set(GenerateJNIFilesPath ${CMAKE_CURRENT_LIST_DIR})