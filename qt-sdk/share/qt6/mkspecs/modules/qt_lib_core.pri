QT.core.VERSION = 6.11.1
QT.core.name = QtCore
QT.core.module = Qt6Core
QT.core.libs = $$QT_MODULE_LIB_BASE
QT.core.ldflags = 
QT.core.includes = $$QT_MODULE_INCLUDE_BASE $$QT_MODULE_INCLUDE_BASE/QtCore
QT.core.frameworks = 
QT.core.bins = $$QT_MODULE_BIN_BASE
QT.core.depends =  
QT.core.uses = libatomic
QT.core.module_config = v2
QT.core.CONFIG = moc resources
QT.core.DEFINES = QT_CORE_LIB
QT.core.enabled_features = cxx11_future cxx17_filesystem glib std-atomic64 mimetype sharedmemory shortcut systemsemaphore xmlstream cpp-winrt xmlstreamreader xmlstreamwriter textdate datestring process processenvironment temporaryfile library settings filesystemwatcher filesystemiterator itemmodel proxymodel sortfilterproxymodel identityproxymodel transposeproxymodel concatenatetablesproxymodel stringlistmodel translation easingcurve animation gestures jalalicalendar islamiccivilcalendar timezone commandlineparser cborstreamreader cborstreamwriter permissions version_tagging shared pkg-config separate_debug_info c++20 c++2a signaling_nan zstd thread future concurrent dbus opensslv30 test_gui test_squish shared intelcet trivial_auto_var_init_pattern stack_protector stack_clash_protection libstdcpp_assertions shared separate_debug_info c++20 c++2a reduce_exports openssl
QT.core.disabled_features = clock-monotonic inotify jemalloc timezone_tzdb static cross_compile debug_and_release appstore-compliant simulator_and_device rpath force_asserts framework c++2b c++2c reduce_relocations wasm-simd128 wasm-exceptions wasm-jspi openssl-linked opensslv11
QT_CONFIG += cxx11_future cxx17_filesystem glib std-atomic64 mimetype sharedmemory shortcut systemsemaphore xmlstream cpp-winrt xmlstreamreader xmlstreamwriter textdate datestring process processenvironment temporaryfile library settings filesystemwatcher filesystemiterator itemmodel proxymodel sortfilterproxymodel identityproxymodel transposeproxymodel concatenatetablesproxymodel stringlistmodel translation easingcurve animation gestures jalalicalendar islamiccivilcalendar timezone commandlineparser cborstreamreader cborstreamwriter permissions version_tagging shared pkg-config separate_debug_info c++20 c++2a signaling_nan zstd thread future concurrent dbus opensslv30 test_gui test_squish shared intelcet trivial_auto_var_init_pattern stack_protector stack_clash_protection libstdcpp_assertions shared separate_debug_info c++20 c++2a reduce_exports openssl
QT_MODULES += core

