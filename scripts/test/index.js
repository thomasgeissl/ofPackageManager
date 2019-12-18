const { execSync } = require("child_process");

const getVersion = {
  type: "GETVERSION",
  payload: {}
};

const installById = {
  type: "INSTALLPACKAGEBYID",
  payload: {
    id: "ofxMidi",
    destination: "local_addons",
    config: {
      localAddonsPath: "local_addons",
      ofPath: "/Users/thomas.geissl/libs/of_v0.11.0_osx_release/",
      packagesPath: "/Users/thomas.geissl/.ofPackages",
      pgPath:
        "/Users/thomas.geissl/libs/of_v0.11.0_osx_release/projectGenerator-osx"
    }
  }
};
const installByIdWithTag = {
  type: "INSTALLPACKAGEBYID",
  payload: {
    id: "ofxLua",
    checkout: "1.3.0",
    destination: "local_addons",
    config: {
      localAddonsPath: "local_addons",
      ofPath: "/Users/thomas.geissl/libs/of_v0.11.0_osx_release/",
      packagesPath: "/Users/thomas.geissl/.ofPackages",
      pgPath:
        "/Users/thomas.geissl/libs/of_v0.11.0_osx_release/projectGenerator-osx"
    }
  }
};

const installByIdWithCommit = {
  type: "INSTALLPACKAGEBYID",
  payload: {
    id: "ofxPd",
    checkout: "c5f76a0ae5bcbcdf41af898c3b9a3db0960f72b8",
    destination: "local_addons",
    config: {
      localAddonsPath: "local_addons",
      ofPath: "/Users/thomas.geissl/libs/of_v0.11.0_osx_release/",
      packagesPath: "/Users/thomas.geissl/.ofPackages",
      pgPath:
        "/Users/thomas.geissl/libs/of_v0.11.0_osx_release/projectGenerator-osx"
    }
  }
};
const install = {
  type: "INSTALL",
  payload: {
    config: {
      localAddonsPath: "local_addons",
      ofPath: "/Users/thomas.geissl/libs/of_v0.11.0_osx_release/",
      packagesPath: "/Users/thomas.geissl/.ofPackages",
      pgPath:
        "/Users/thomas.geissl/libs/of_v0.11.0_osx_release/projectGenerator-osx"
    }
  }
};

const packageManager = command => {
  console.log(
    execSync(
      `../../bin/ofPackageManager.app/Contents/MacOS/ofPackageManager "${JSON.stringify(
        command
      ).replace(/\\([\s\S])|(")/g, "\\$1$2")}"`
    ).toString()
  );
};

execSync("rm -rf ./local_addons");
packageManager(getVersion);
// packageManager(installById);
// packageManager(installByIdWithTag);
// packageManager(installByIdWithCommit);
// execSync("ofPackageManager add -A ./local_addons");
// execSync("rm -rf ./local_addons");
// packageManager(install);
