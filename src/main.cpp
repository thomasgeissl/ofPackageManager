#include "ofMain.h"
#include "ofxUnitTests.h"
#include "ofApp.h"
#include "./interfaces/gui/gui.h"
#include "./interfaces/cli.h"
#include "./interfaces/jsonInterface.h"

int main(int argc, char **argv)
{
	ofInit();

#if defined(TARGET_OSX) || defined(TARGET_LINUX)
	std::shared_ptr<ofColorsLoggerChannel> logger{
		new ofColorsLoggerChannel};
	ofSetLoggerChannel(logger);
#endif
	//    ofSetLogLevel(OF_LOG_VERBOSE);
	filesystem::path cwdPath = filesystem::current_path();

	ofPackageManager app(cwdPath.string());
	jsonInterface jsonI(app);
	if (argc == 1)
	{
		ofSetupOpenGL(600, 768, OF_WINDOW);
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