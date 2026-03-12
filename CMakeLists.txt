cmake_minimum_required(VERSION 3.15)
project(Notepad2 LANGUAGES C CXX RC)

# Require Windows
if(NOT WIN32)
    message(FATAL_ERROR "Notepad2 is a Windows-only application.")
endif()

# Verify Scintilla 2.24 source is present
if(NOT EXISTS "${CMAKE_SOURCE_DIR}/scintilla/include/Scintilla.h")
    message(FATAL_ERROR
        "Scintilla source not found. "
        "Unzip Scintilla 2.24 source into '${CMAKE_SOURCE_DIR}/scintilla/'.\n"
        "See Readme.txt for details."
    )
endif()

# ---------------------------------------------------------------------------
# Scintilla sources
# ---------------------------------------------------------------------------

set(SCI ${CMAKE_SOURCE_DIR}/scintilla)

set(SCINTILLA_LEXERS
    ${SCI}/lexers/LexAsm.cxx
    ${SCI}/lexers/LexCPP.cxx
    ${SCI}/lexers/LexCSS.cxx
    ${SCI}/lexers/LexConf.cxx
    ${SCI}/lexers/LexHTML.cxx
    ${SCI}/lexers/LexOthers.cxx
    ${SCI}/lexers/LexPascal.cxx
    ${SCI}/lexers/LexPerl.cxx
    ${SCI}/lexers/LexPowerShell.cxx
    ${SCI}/lexers/LexPython.cxx
    ${SCI}/lexers/LexSQL.cxx
    ${SCI}/lexers/LexVB.cxx
)

set(SCINTILLA_LEXLIB
    ${SCI}/lexlib/Accessor.cxx
    ${SCI}/lexlib/CharacterSet.cxx
    ${SCI}/lexlib/LexerBase.cxx
    ${SCI}/lexlib/LexerModule.cxx
    ${SCI}/lexlib/LexerSimple.cxx
    ${SCI}/lexlib/PropSetSimple.cxx
    ${SCI}/lexlib/StyleContext.cxx
    ${SCI}/lexlib/WordList.cxx
)

set(SCINTILLA_SRC
    ${SCI}/src/AutoComplete.cxx
    ${SCI}/src/CallTip.cxx
    # Catalogue.cxx is replaced by our project-local version (see Catalogue.cxx)
    # to avoid linker errors from LINK_LEXER() refs to uncompiled lexers.
    ${CMAKE_SOURCE_DIR}/Catalogue.cxx
    ${SCI}/src/CellBuffer.cxx
    ${SCI}/src/CharClassify.cxx
    ${SCI}/src/ContractionState.cxx
    ${SCI}/src/Decoration.cxx
    ${SCI}/src/Document.cxx
    ${SCI}/src/Editor.cxx
    ${SCI}/src/ExternalLexer.cxx
    ${SCI}/src/Indicator.cxx
    ${SCI}/src/KeyMap.cxx
    ${SCI}/src/LineMarker.cxx
    ${SCI}/src/PerLine.cxx
    ${SCI}/src/PositionCache.cxx
    ${SCI}/src/RESearch.cxx
    ${SCI}/src/RunStyles.cxx
    ${SCI}/src/ScintillaBase.cxx
    ${SCI}/src/Selection.cxx
    ${SCI}/src/Style.cxx
    ${SCI}/src/UniConversion.cxx
    ${SCI}/src/ViewStyle.cxx
    ${SCI}/src/XPM.cxx
)

set(SCINTILLA_WIN32
    ${SCI}/win32/PlatWin.cxx
    ${SCI}/win32/ScintillaWin.cxx
)

# ---------------------------------------------------------------------------
# Notepad2 application sources
# ---------------------------------------------------------------------------

set(NP2_SOURCES
    src/Dialogs.c
    src/Dlapi.c
    src/Edit.c
    src/Helpers.c
    src/Notepad2.c
    src/Print.cpp
    src/Styles.c
    src/Notepad2.rc
)

# ---------------------------------------------------------------------------
# Executable target
# ---------------------------------------------------------------------------

add_executable(Notepad2 WIN32
    ${NP2_SOURCES}
    ${SCINTILLA_LEXERS}
    ${SCINTILLA_LEXLIB}
    ${SCINTILLA_SRC}
    ${SCINTILLA_WIN32}
)

# ---------------------------------------------------------------------------
# Include directories
# ---------------------------------------------------------------------------

target_include_directories(Notepad2 PRIVATE
    ${SCI}/include
    ${SCI}/src
    ${SCI}/lexlib
    ${SCI}/win32
    src
)

# ---------------------------------------------------------------------------
# Preprocessor definitions
# ---------------------------------------------------------------------------

target_compile_definitions(Notepad2 PRIVATE
    STATIC_BUILD
    SCI_LEXER
    WIN32
    _WINDOWS
    UNICODE
    _UNICODE
    $<$<CONFIG:Debug>:_DEBUG>
    $<$<NOT:$<CONFIG:Debug>>:NDEBUG>
)

# ---------------------------------------------------------------------------
# Compiler options (MSVC)
# ---------------------------------------------------------------------------

if(MSVC)
    target_compile_options(Notepad2 PRIVATE
        /W3
        /EHsc
        /GF             # String pooling
        $<$<CONFIG:Debug>:/Od /RTC1 /MDd /Zi>
        $<$<NOT:$<CONFIG:Debug>>:/O1 /Oy /GS- /MD /GL>
    )

    # Release: whole-program optimization requires /LTCG on the linker
    target_link_options(Notepad2 PRIVATE
        $<$<NOT:$<CONFIG:Debug>>:/LTCG /OPT:REF /OPT:ICF /INCREMENTAL:NO>
        $<$<CONFIG:Debug>:/DEBUG /INCREMENTAL>
        # The .rc already embeds res/Notepad2.exe.manifest via RT_MANIFEST.
        # Suppress the linker's auto-generated manifest to avoid CVT1100
        # "duplicate resource" / LNK1123 errors.
        /MANIFEST:NO
    )
endif()

# ---------------------------------------------------------------------------
# Windows libraries
# ---------------------------------------------------------------------------

target_link_libraries(Notepad2 PRIVATE
    kernel32
    user32
    gdi32
    advapi32
    shell32
    shlwapi
    comdlg32
    comctl32
    winspool
    imm32
    ole32
    oleaut32
    psapi
    uxtheme
    dwmapi
)

# ---------------------------------------------------------------------------
# Resource compiler definitions (passed via RC flags)
# ---------------------------------------------------------------------------

# Tell the RC compiler where to find included files (resource.h, version.h, res/)
set_property(TARGET Notepad2 APPEND PROPERTY
    COMPILE_OPTIONS
        # RC flags are set separately below
)

# Point the resource compiler at the src/ and res/ directories so it can
# find resource.h, version.h, Notepad2.ver, and the bitmap/icon assets.
if(MSVC)
    set(CMAKE_RC_FLAGS "${CMAKE_RC_FLAGS} /l 0x0409 /I \"${CMAKE_SOURCE_DIR}/src\" /I \"${CMAKE_SOURCE_DIR}/res\"")
endif()

# ---------------------------------------------------------------------------
# Output name
# ---------------------------------------------------------------------------

set_target_properties(Notepad2 PROPERTIES OUTPUT_NAME "Notepad2")

# ---------------------------------------------------------------------------
# Copy Notepad2.ini next to the executable after build
# ---------------------------------------------------------------------------

add_custom_command(TARGET Notepad2 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/Notepad2.ini"
        "$<TARGET_FILE_DIR:Notepad2>/Notepad2.ini"
    COMMENT "Copying Notepad2.ini to output directory"
)
