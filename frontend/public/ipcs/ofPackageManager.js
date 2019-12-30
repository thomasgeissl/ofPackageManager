const { ipcMain } = require("electron");
const { execSync } = require("child_process");
const { logAndSendToWebConsole } = require("./utils");
const config = require("../assets/config.json");

const getVersion = () => {
  return {
    type: "GETVERSION"
  };
};
const getAvailablePackages = () => {
  return {
    type: "GETAVAILABLEPACKAGES",
    payload: {
      config
    }
  };
};
const installPackageById = values => {
  return {
    type: "INSTALLPACKAGEBYID",
    payload: {
      id: values.id,
      checkout: typeof checkout == "undefined" ? "latest" : values.checkout,
      destination: config.localAddonsPath,
      config,
      cwd: values.cwd
    }
  };
};

const installPackageByGithub = values => {
  return {
    type: "INSTALLPACKAGEBYGITHUB",
    payload: {
      github: values.github,
      checkout: typeof checkout == "undefined" ? "latest" : values.checkout,
      destination: config.localAddonsPath,
      config,
      cwd: values.cwd
    }
  };
};

const installPackageByUrl = values => {
  return {
    type: "INSTALLPACKAGEBYURL",
    payload: {
      url: values.url,
      checkout: typeof checkout == "undefined" ? "latest" : values.checkout,
      destination: config.localAddonsPath,
      config,
      cwd: values.cwd
    }
  };
};

const getCoreAddons = () => {
  return {
    type: "GETCOREADDONS",
    payload: {
      config
    }
  };
};
const getGloballyInstalledPackages = () => {
  return {
    type: "GETGLOBALLYINSTALLEDPACKAGES",
    payload: {
      config
    }
  };
};
const getLocallyInstalledPackages = values => {
  return {
    type: "GETLOCALLYINSTALLEDPACKAGES",
    payload: {
      config,
      cwd: values.cwd
    }
  };
};
const addPackageToAddonsMakeFile = values => {
  return {
    type: "ADDPACKAGETOADDONSMAKEFILE",
    payload: {
      package: {
        url: values.url,
        path: values.path,
        checkout: values.checkout
      },
      config,
      cwd: values.cwd
    }
  };
};
const getPackagesListedInAddonsMake = values => {
  return {
    type: "GETPACKAGESLISTEDINADDONSMAKE",
    payload: {
      config,
      cwd: values.cwd
    }
  };
};

const packageManager = command => {
  const rawResponse = execSync(
    `${config.ofPackageManagerPath} "${JSON.stringify(command).replace(
      /\\([\s\S])|(")/g,
      "\\$1$2"
    )}"`
  ).toString();
  try {
    const response = JSON.parse(rawResponse);
    return response;
  } catch (e) {
    console.log("could not parse response", rawResponse);
    return {};
  }
};

ipcMain.on("getVersion", (event, arg) => {
  logAndSendToWebConsole("getting version", event);
  const response = packageManager(getVersion());
  event.reply("getVersionResponse", response);
  logAndSendToWebConsole(JSON.stringify(response, {}, 4), event);
});

ipcMain.on("getAvailablePackages", (event, arg) => {
  const response = packageManager(getAvailablePackages());
  event.reply("getAvailablePackagesResponse", response);
});

ipcMain.on("installPackageById", (event, arg) => {
  const response = packageManager(installPackageById(arg));
  if (response.success) {
    const response = packageManager(getLocallyInstalledPackages(arg));
    event.reply("getLocallyInstalledPackagesResponse", response);
  }
  // event.reply("installPackageByIdResponse", JSON.stringify(response));
});

ipcMain.on("installPackageByGithub", (event, arg) => {
  const response = packageManager(installPackageByGithub(arg));
  // console.log(response);
  // event.reply("installPackageByGithubResponse", response);
  if (response.success) {
    const response = packageManager(getLocallyInstalledPackages(arg));
    event.reply("getLocallyInstalledPackagesResponse", response);
  }
});

ipcMain.on("installPackageByUrl", (event, arg) => {
  const response = packageManager(installPackageByUrl(arg));
  // event.reply("installPackageByUrlResponse", response);
  if (response.success) {
    const response = packageManager(getLocallyInstalledPackages(arg));
    event.reply("getLocallyInstalledPackagesResponse", response);
  }
});
ipcMain.on("getCoreAddons", (event, arg) => {
  const response = packageManager(getCoreAddons());
  event.reply("getCoreAddonsResponse", response);
});
ipcMain.on("getGloballyInstalledPackages", (event, arg) => {
  const response = packageManager(getGloballyInstalledPackages());
  event.reply("getGloballyInstalledPackagesResponse", response);
});
ipcMain.on("getLocallyInstalledPackages", (event, arg) => {
  const response = packageManager(getLocallyInstalledPackages(arg));
  event.reply("getLocallyInstalledPackagesResponse", response);
});
ipcMain.on("addPackageToAddonsMakeFile", (event, arg) => {
  const response = packageManager(addPackageToAddonsMakeFile(arg));
  event.reply("addPackageToAddonsMakeFileResponse", response);
});
ipcMain.on("getPackagesListedInAddonsMake", (event, arg) => {
  const response = packageManager(getPackagesListedInAddonsMake(arg));
  event.reply("getPackagesListedInAddonsMakeResponse", response);
});
ipcMain.on("getConfig", (event, arg) => {
  event.reply("getConfigResponse", config);
});
