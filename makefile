BUILD_DEBUG_DIR = builds/debug
BUILD_RELEASE_DIR = builds/release
PROJECT_NAME = AuroraDrive

init-debug:
	cmake -S . -B builds/debug -D CMAKE_BUILD_TYPE=Debug

build-debug:
	cmake --build ${BUILD_DEBUG_DIR} --config Debug

build-release:
	cmake --build ${BUILD_RELEASE_DIR} --config Release

run: build-release
	killall ${PROJECT_NAME} || true
	open ${BUILD_RELEASE_DIR}/src/${PROJECT_NAME}_artefacts/Standalone/${PROJECT_NAME}.app

debug: build-debug
	lldb ${BUILD_DEBUG_DIR}/src/${PROJECT_NAME}_artefacts/Debug/Standalone/${PROJECT_NAME}.app/Contents/MacOS/${PROJECT_NAME}
