BUILD_DIR = builds
RELEASE_DIR = release
PROJECT_NAME = AuroraDrive

build-debug:
	cmake --build ${BUILD_DIR} --config Debug

build-release:
	cmake --build ${BUILD_DIR} --config Release

run: build-release
	killall ${PROJECT_NAME} || true
	open ${BUILD_DIR}/src/${PROJECT_NAME}_artefacts/Standalone/${PROJECT_NAME}.app

debug: build-debug
	lldb builds/src/${PROJECT_NAME}_artefacts/Debug/Standalone/${PROJECT_NAME}.app/Contents/MacOS/${PROJECT_NAME}
