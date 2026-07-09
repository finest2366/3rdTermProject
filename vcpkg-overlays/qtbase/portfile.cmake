# portfile.cmake — qtbase wrapper for MSYS2 MinGW Qt6
#
# When GitHub is unreachable, this overlay port wraps the MSYS2 system Qt6
# so vcpkg manifest mode still works without downloading anything.
#
# To switch to PURE vcpkg mode (downloads + builds Qt6 from source):
#   1. Delete vcpkg-overlays/ and vcpkg-configuration.json
#   2. Add "builtin-baseline" back to vcpkg.json
#
# Auto-detection order for MSYS2 UCRT64 root:
#   1. Env var MSYS2_UCRT64_ROOT
#   2. Default MSYS2 installs: C:/msys64/ucrt64, D:/msys64/ucrt64, E:/msys64/ucrt64

set(VCPKG_POLICY_SKIP_COPYRIGHT_CHECK enabled)

# --- Locate MSYS2 UCRT64 ---
set(MSYS2_ROOT "NOTFOUND")

# 1. Environment variable
if(DEFINED ENV{MSYS2_UCRT64_ROOT})
    set(MSYS2_ROOT "$ENV{MSYS2_UCRT64_ROOT}")
endif()

# 2. Check common install locations
if(MSYS2_ROOT STREQUAL "NOTFOUND")
    foreach(DRIVE_LETTER "C" "D" "E" "F" "G" "H" "I" "J" "K" "L" "M" "N" "O" "P" "Q" "R" "S" "T" "U" "V" "W" "X" "Y" "Z")
        set(CANDIDATE "${DRIVE_LETTER}:/msys64/ucrt64")
        if(EXISTS "${CANDIDATE}/include/qt6/QtCore/QtCore")
            set(MSYS2_ROOT "${CANDIDATE}")
            break()
        endif()
    endforeach()
endif()

# 3. Validate
if(MSYS2_ROOT STREQUAL "NOTFOUND")
    message(FATAL_ERROR
        "MSYS2 UCRT64 with Qt6 not found.\n\n"
        "Options:\n"
        "  a) Install MSYS2 to C:\\msys64 and run:\n"
        "       pacman -S mingw-w64-ucrt-x86_64-qt6-base\n"
        "  b) Set MSYS2_UCRT64_ROOT env var to your MSYS2 ucrt64 dir.\n"
        "  c) If you have network access to GitHub, delete these to use\n"
        "     pure vcpkg mode: vcpkg-overlays/  vcpkg-configuration.json\n"
    )
endif()

message(STATUS "Wrapping MSYS2 Qt6 from: ${MSYS2_ROOT}")

# --- Helper: copy if exists (silent) ---
function(msys2_copy FROM_GLOB DEST)
    file(GLOB FILES ${FROM_GLOB})
    if(FILES)
        file(COPY ${FILES} DESTINATION "${DEST}")
    endif()
endfunction()

# --- Copy Qt headers ---
foreach(MOD QtCore QtGui QtWidgets)
    if(EXISTS "${MSYS2_ROOT}/include/qt6/${MOD}")
        file(COPY "${MSYS2_ROOT}/include/qt6/${MOD}"
             DESTINATION "${CURRENT_PACKAGES_DIR}/include/")
    endif()
endforeach()

# --- Copy Qt DLLs + build tools + dep DLLs ---
msys2_copy("${MSYS2_ROOT}/bin/Qt6*.dll"           "${CURRENT_PACKAGES_DIR}/bin/")
msys2_copy("${MSYS2_ROOT}/bin/moc.exe"            "${CURRENT_PACKAGES_DIR}/bin/")
msys2_copy("${MSYS2_ROOT}/bin/rcc.exe"            "${CURRENT_PACKAGES_DIR}/bin/")
msys2_copy("${MSYS2_ROOT}/bin/uic.exe"            "${CURRENT_PACKAGES_DIR}/bin/")

# Dependency DLLs (runtime)
set(DEP_DLL_PATTERNS
    "${MSYS2_ROOT}/bin/libb2-1.dll"
    "${MSYS2_ROOT}/bin/libbrotlicommon.dll"
    "${MSYS2_ROOT}/bin/libbrotlidec.dll"
    "${MSYS2_ROOT}/bin/libbz2-1.dll"
    "${MSYS2_ROOT}/bin/libdouble-conversion.dll"
    "${MSYS2_ROOT}/bin/libfreetype-6.dll"
    "${MSYS2_ROOT}/bin/libgcc_s_seh-1.dll"
    "${MSYS2_ROOT}/bin/libglib-2.0-0.dll"
    "${MSYS2_ROOT}/bin/libgraphite2.dll"
    "${MSYS2_ROOT}/bin/libharfbuzz-0.dll"
    "${MSYS2_ROOT}/bin/libiconv-2.dll"
    "${MSYS2_ROOT}/bin/libicudt78.dll"
    "${MSYS2_ROOT}/bin/libicuin78.dll"
    "${MSYS2_ROOT}/bin/libicuuc78.dll"
    "${MSYS2_ROOT}/bin/libintl-8.dll"
    "${MSYS2_ROOT}/bin/libmd4c.dll"
    "${MSYS2_ROOT}/bin/libpcre2-16-0.dll"
    "${MSYS2_ROOT}/bin/libpcre2-8-0.dll"
    "${MSYS2_ROOT}/bin/libpng16-16.dll"
    "${MSYS2_ROOT}/bin/libstdc++-6.dll"
    "${MSYS2_ROOT}/bin/libwinpthread-1.dll"
    "${MSYS2_ROOT}/bin/libzstd.dll"
    "${MSYS2_ROOT}/bin/zlib1.dll"
)
foreach(DLL ${DEP_DLL_PATTERNS})
    if(EXISTS "${DLL}")
        file(COPY "${DLL}" DESTINATION "${CURRENT_PACKAGES_DIR}/bin/")
    endif()
endforeach()

# --- Copy import libraries ---
msys2_copy("${MSYS2_ROOT}/lib/libQt6*.dll.a"      "${CURRENT_PACKAGES_DIR}/lib/")
msys2_copy("${MSYS2_ROOT}/lib/libQt6EntryPoint.a" "${CURRENT_PACKAGES_DIR}/lib/")
msys2_copy("${MSYS2_ROOT}/lib/Qt6*.prl"           "${CURRENT_PACKAGES_DIR}/lib/")
msys2_copy("${MSYS2_ROOT}/lib/pkgconfig/Qt6*.pc"  "${CURRENT_PACKAGES_DIR}/lib/pkgconfig/")

# --- Copy CMake configs (relocatable — Qt6 computes prefix at cmake time) ---
file(GLOB CMAKE_DIRS "${MSYS2_ROOT}/lib/cmake/Qt6*")
foreach(MOD_DIR ${CMAKE_DIRS})
    if(IS_DIRECTORY "${MOD_DIR}")
        get_filename_component(MOD_NAME "${MOD_DIR}" NAME)
        file(COPY "${MOD_DIR}" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${MOD_NAME}/")
    endif()
endforeach()

# --- Copy plugins (platforms, styles, imageformats, tls) ---
foreach(PLUGIN platforms styles imageformats tls)
    set(PLUGIN_DIR "${MSYS2_ROOT}/share/qt6/plugins/${PLUGIN}")
    if(EXISTS "${PLUGIN_DIR}")
        file(COPY "${PLUGIN_DIR}/" DESTINATION "${CURRENT_PACKAGES_DIR}/plugins/${PLUGIN}/")
    endif()
endforeach()

# --- CMake wrapper: Qt6Plugins.cmake expects plugins at a specific location ---
# Qt6 computes QT6_INSTALL_PREFIX from its config location → share/Qt6 → ../.. = PACKAGES_DIR
# Plugins are at plugins/ relative to that.  ✓

# --- Release/Debug symmetry ---
file(COPY "${CURRENT_PACKAGES_DIR}/bin/"
     DESTINATION "${CURRENT_PACKAGES_DIR}/debug/bin/")
file(COPY "${CURRENT_PACKAGES_DIR}/lib/"
     DESTINATION "${CURRENT_PACKAGES_DIR}/debug/lib/")

message(STATUS "MSYS2 Qt6 wrapped successfully → ${CURRENT_PACKAGES_DIR}")
