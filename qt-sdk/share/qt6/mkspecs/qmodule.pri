QT_CPU_FEATURES.x86_64 = cx16 sse3
QT.global_private.enabled_features = reduce_exports x86intrin sse2 sse3 ssse3 sse4_1 sse4_2 avx f16c avx2 avx512f avx512er avx512cd avx512pf avx512dq avx512bw avx512vl avx512ifma avx512vbmi avx512vbmi2 aesni vaes rdrnd rdseed shani localtime_s system-zlib dbus dbus-linked gui network printsupport sql testlib widgets xml openssl relocatable intelcet trivial_auto_var_init_pattern stack_protector stack_clash_protection libstdcpp_assertions force_debug_info largefile sse2 sse3 ssse3 sse4_1 sse4_2 avx f16c avx2 avx512f avx512er avx512cd avx512pf avx512dq avx512bw avx512vl avx512ifma avx512vbmi avx512vbmi2 aesni vaes rdrnd rdseed shani
QT.global_private.disabled_features = use_bfd_linker use_gold_linker use_lld_linker use_mold_linker android-style-assets gc_binaries developer-build private_tests doc_snippets debug elf_private_full_version no_direct_extern_access lsx lasx mips_dsp mips_dspr2 neon arm_crc32 arm_crypto arm_sve localtime_r posix_fallocate force-system-libs force-bundled-libs stdlib-libcpp libudev dlopen glibc_fortify_source libcpp_hardening relro_now_linker android_16kb_pages
CONFIG += force_debug_info largefile sse2 sse3 ssse3 sse4_1 sse4_2 avx f16c avx2 avx512f avx512er avx512cd avx512pf avx512dq avx512bw avx512vl avx512ifma avx512vbmi avx512vbmi2 aesni vaes rdrnd rdseed shani
PKG_CONFIG_EXECUTABLE = /ucrt64/bin/pkg-config.exe
QT_COORD_TYPE = double
QT_BUILD_PARTS = libs tools

QMAKE_LIBS_ZLIB = -lz
QMAKE_LIBS_ZSTD = -lzstd -lzstd
