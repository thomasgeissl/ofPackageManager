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
const installByIdWithNonExistingTag = {
  type: "INSTALLPACKAGEBYID",
  payload: {
    id: "ofxCv",
    checkout: "nonexistingtag",
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

const searchInDataBaseById = {
  type: "SEARCHPACKAGEINDATABASEBYID",
  payload: {
    id: "ofxCv",
    config: {
      localAddonsPath: "local_addons",
      ofPath: "/Users/thomas.geissl/libs/of_v0.11.0_osx_release/",
      packagesPath: "/Users/thomas.geissl/.ofPackages",
      pgPath:
        "/Users/thomas.geissl/libs/of_v0.11.0_osx_release/projectGenerator-osx"
    }
  }
};

const searchOnGithubByName = {
  type: "SEARCHPACKAGEONGITHUBBYNAME",
  payload: {
    name: "ofxDmx",
    config: {
      localAddonsPath: "local_addons",
      ofPath: "/Users/thomas.geissl/libs/of_v0.11.0_osx_release/",
      packagesPath: "/Users/thomas.geissl/.ofPackages",
      pgPath:
        "/Users/thomas.geissl/libs/of_v0.11.0_osx_release/projectGenerator-osx"
    }
  }
};

const searchOnGithubByUser = {
  type: "SEARCHPACKAGEONGITHUBBYUSER",
  payload: {
    user: "bakercp",
    config: {
      localAddonsPath: "local_addons",
      ofPath: "/Users/thomas.geissl/libs/of_v0.11.0_osx_release/",
      packagesPath: "/Users/thomas.geissl/.ofPackages",
      pgPath:
        "/Users/thomas.geissl/libs/of_v0.11.0_osx_release/projectGenerator-osx"
    }
  }
};

const addPackageIfSuccess = response => {
  if (!response.success) {
    console.log("could not add package", response);
    return false;
  }
  const install = {
    type: "ADDPACKAGETOADDONSMAKEFILE",
    payload: {
      package: response.payload.package,
      config: {
        localAddonsPath: "local_addons",
        ofPath: "/Users/thomas.geissl/libs/of_v0.11.0_osx_release/",
        packagesPath: "/Users/thomas.geissl/.ofPackages",
        pgPath:
          "/Users/thomas.geissl/libs/of_v0.11.0_osx_release/projectGenerator-osx"
      }
    }
  };
  return packageManager(install);
};

const packageManager = command => {
  const response = JSON.parse(
    execSync(
      `../../bin/ofPackageManager.app/Contents/MacOS/ofPackageManager "${JSON.stringify(
        command
      ).replace(/\\([\s\S])|(")/g, "\\$1$2")}"`
    ).toString()
  );
  console.log(response);
  return response;
};

execSync("rm -rf ./local_addons");
execSync("rm -rf ./addons.make");

console.log("getting version");
const version = packageManager(getVersion);
console.log("installing package by id");
const packageInstalledById = packageManager(installById);
console.log("installing package by id with tag");
const packageInstalledByIdWithTag = packageManager(installByIdWithTag);
console.log("installing package by id with commit");
const packageInstalledByIdWithCommit = packageManager(installByIdWithCommit);
console.log("installing package by id with nonexisting tag");
const packageInstalledByIdWithNonExistingTag = packageManager(
  installByIdWithNonExistingTag
);

console.log("adding package installed by id to addons.make");
addPackageIfSuccess(packageInstalledById);
console.log("adding package installed by id with tag to addons.make");
addPackageIfSuccess(packageInstalledByIdWithTag);
console.log("adding package installed by id with commit to addons.make");
addPackageIfSuccess(packageInstalledByIdWithCommit);
console.log(
  "adding package installed by id with nonexisting tag to addons.make"
);
addPackageIfSuccess(packageInstalledByIdWithNonExistingTag);

execSync("rm -rf ./local_addons");
console.log(
  "reinstalling packages listed in the addons.make file after deleting them"
);
const installed = packageManager(install);

const searchResultsInDataBaseById = packageManager(searchInDataBaseById);
const searchResultsOnGithubByName = packageManager(searchOnGithubByName);
const searchResultsOnGithubByUser = packageManager(searchOnGithubByUser);
