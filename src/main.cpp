#include "ofMain.h"
#include "ofxUnitTests.h"
#include "ofApp.h"
#include "./interfaces/cliInterface.h"
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
	if (jsonI.exec(argc, argv))
	{
	}
	else
	{
		cliInterface cli(app);
		cli.exec(argc, argv);
	}

	return 0;
}