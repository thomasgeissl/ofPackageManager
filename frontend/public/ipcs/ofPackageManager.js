const { ipcMain } = require("electron");
const { execSync } = require("child_process");
const { logAndSendToWebConsole } = require("./utils");

const getVersion = values => {
  const { config } = values;
  return {
    type: "GETVERSION"
  };
};
const getAvailablePackages = values => {
  const { config } = values;
  return {
    type: "GETAVAILABLEPACKAGES",
    payload: {
      config
    }
  };
};
const install = values => {
  const { config } = values;
  return {
    type: "INSTALL",
    payload: {
      config,
      cwd: values.cwd
    }
  };
};
const installPackageById = values => {
  const { config } = values;
  return {
    type: "INSTALLPACKAGEBYID",
    payload: {
      id: values.id,
      checkout:
        typeof values.checkout == "undefined" ? "latest" : values.checkout,
      destination: config.localAddonsPath,
      config,
      cwd: values.cwd
    }
  };
};

const installPackageByGithub = values => {
  const { config } = values;
  return {
    type: "INSTALLPACKAGEBYGITHUB",
    payload: {
      github: values.github,
      checkout:
        typeof values.checkout == "undefined" ? "latest" : values.checkout,
      destination: config.localAddonsPath,
      config,
      cwd: values.cwd
    }
  };
};

const installPackageByUrl = values => {
  const { config } = values;
  return {
    type: "INSTALLPACKAGEBYURL",
    payload: {
      config,
      url: values.url,
      checkout:
        typeof values.checkout == "undefined" ? "latest" : values.checkout,
      destination: config.localAddonsPath,
      cwd: values.cwd
    }
  };
};

const getCoreAddons = values => {
  const { config } = values;
  return {
    type: "GETCOREADDONS",
    payload: {
      config
    }
  };
};
const getGloballyInstalledPackages = values => {
  const { config } = values;
  return {
    type: "GETGLOBALLYINSTALLEDPACKAGES",
    payload: {
      config
    }
  };
};
const getLocallyInstalledPackages = values => {
  const { config } = values;
  return {
    type: "GETLOCALLYINSTALLEDPACKAGES",
    payload: {
      config,
      cwd: values.cwd
    }
  };
};
const addPackageToAddonsMakeFile = values => {
  const { config } = values;
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
  const { config } = values;
  return {
    type: "GETPACKAGESLISTEDINADDONSMAKE",
    payload: {
      config,
      cwd: values.cwd
    }
  };
};

const packageManager = (config, command) => {
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
  const { config } = arg;
  const response = packageManager(config, getVersion(arg));
  event.reply("getVersionResponse", response);
  logAndSendToWebConsole(JSON.stringify(response, {}, 4), event);
});

ipcMain.on("getAvailablePackages", (event, arg) => {
  const { config } = arg;
  const response = packageManager(config, getAvailablePackages(arg));
  event.reply("getAvailablePackagesResponse", response);
});

ipcMain.on("install", (event, arg) => {
  console.log("install");
  const { config } = arg;
  const response = packageManager(config, install(arg));
  event.reply("installResponse", response);

  const globalResponse = packageManager(
    config,
    getGloballyInstalledPackages(arg)
  );
  event.reply("getGloballyInstalledPackagesResponse", globalResponse);
  const localResponse = packageManager(
    config,
    getLocallyInstalledPackages(arg)
  );
  event.reply("getLocallyInstalledPackagesResponse", localResponse);
});

ipcMain.on("installPackageById", (event, arg) => {
  const { config } = arg;
  const response = packageManager(config, installPackageById(arg));
  if (response.success) {
    const response = packageManager(config, getLocallyInstalledPackages(arg));
    event.reply("getLocallyInstalledPackagesResponse", response);
  }
  event.reply("installPackageByIdResponse", response);
});

ipcMain.on("installPackageByGithub", (event, arg) => {
  const { config } = arg;
  const response = packageManager(config, installPackageByGithub(arg));
  // console.log(response);
  if (response.success) {
    const response = packageManager(config, getLocallyInstalledPackages(arg));
    event.reply("getLocallyInstalledPackagesResponse", response);
  }
  event.reply("installPackageByGithubResponse", response);
});

ipcMain.on("installPackageByUrl", (event, arg) => {
  const { config } = arg;
  const response = packageManager(config, installPackageByUrl(arg));
  // event.reply("installPackageByUrlResponse", response);
  if (response.success) {
    const response = packageManager(config, getLocallyInstalledPackages(arg));
    event.reply("getLocallyInstalledPackagesResponse", response);
  }
  event.reply("installPackageByUrlResponse", response);
});
ipcMain.on("getCoreAddons", (event, arg) => {
  const { config } = arg;
  const response = packageManager(config, getCoreAddons(arg));
  event.reply("getCoreAddonsResponse", response);
});
ipcMain.on("getGloballyInstalledPackages", (event, arg) => {
  const { config } = arg;
  const response = packageManager(config, getGloballyInstalledPackages(arg));
  event.reply("getGloballyInstalledPackagesResponse", response);
});
ipcMain.on("getLocallyInstalledPackages", (event, arg) => {
  const { config } = arg;
  const response = packageManager(config, getLocallyInstalledPackages(arg));
  event.reply("getLocallyInstalledPackagesResponse", response);
});
ipcMain.on("addPackageToAddonsMakeFile", (event, arg) => {
  const { config } = arg;
  const response = packageManager(config, addPackageToAddonsMakeFile(arg));
  event.reply("addPackageToAddonsMakeFileResponse", response);
});
ipcMain.on("getPackagesListedInAddonsMake", (event, arg) => {
  const { config } = arg;
  const response = packageManager(config, getPackagesListedInAddonsMake(arg));
  event.reply("getPackagesListedInAddonsMakeResponse", response);
});
