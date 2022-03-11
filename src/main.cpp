#include "ofMain.h"
#include "ofxUnitTests.h"
#include "ofApp.h"
#include "./interfaces/gui/gui.h"
#include "./interfaces/cli.h"
#include "./interfaces/jsonInterface.h"
#include "./whereami.h"

int main(int argc, char **argv)
{
	ofInit();

#if defined(TARGET_OSX) || defined(TARGET_LINUX)
	std::shared_ptr<ofColorsLoggerChannel> logger{
		new ofColorsLoggerChannel};
	ofSetLoggerChannel(logger);
#endif
	//    ofSetLogLevel(OF_LOG_VERBOSE);

	std::string executablePath;
	char *path = NULL;
	int length, dirname_length;
	int i;

	length = wai_getExecutablePath(NULL, 0, &dirname_length);
	if (length > 0)
	{
		path = (char *)malloc(length + 1);
		if (!path)
			abort();
		wai_getExecutablePath(path, length, &dirname_length);
		path[length] = '\0';
		printf("executable path: %s\n", path);
		path[dirname_length] = '\0';
		printf("  dirname: %s\n", path);
		printf("  basename: %s\n", path + dirname_length + 1);
		executablePath = path;
		free(path);
	}

	std::string cwdPath = filesystem::current_path().string();
	ofLogNotice() << cwdPath << " " << executablePath;
	ofPackageManager app(cwdPath != "/" ? cwdPath : executablePath);
	jsonInterface jsonI(app);
	if (argc == 1)
	{
		ofSetupOpenGL(1024, 768, OF_WINDOW);
		return ofRunApp(new gui(app));
	}
	else if (jsonI.accept(argc, argv))
	{
		auto result = jsonI.exec(argc, argv);
		std::cout << result.dump() << std::endl;
		if (result["success"].get<bool>())
		{
			return 0;
		}
	}
	else
	{
		cli c(app);
		if (c.exec(argc, argv))
		{
			return 0;
		}
	}

	return -1;
}