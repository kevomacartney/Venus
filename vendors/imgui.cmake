set(root ${CMAKE_CURRENT_SOURCE_DIR}/imgui)

# ============ sources ============
set(phdrs ${root}/imgui.h)
set(hdrs  ${root}/imstb_rectpack.h ${root}/imstb_textedit.h ${root}/imstb_truetype.h ${root}/imgui_internal.h)
set(srcs  ${root}/imgui.cpp ${root}/imgui_draw.cpp ${root}/imgui_widgets.cpp)
# =================================

add_library(imgui STATIC ${phdrs} ${hdrs} ${srcs})

target_include_directories(imgui PUBLIC
        $<BUILD_INTERFACE:${root}>
        $<INSTALL_INTERFACE:include>)
target_compile_definitions(imgui PUBLIC IMGUI_DISABLE_INCLUDE_IMCONFIG_H)
set_target_properties(imgui PROPERTIES DEBUG_POSTFIX "d")
