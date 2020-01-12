import React from "react";
import ReactDOM from "react-dom";
import "./index.css";
import App from "./App";
import * as serviceWorker from "./serviceWorker";
import { ipcRenderer } from "electron";
import store from "./state/store";
import { appendOutput } from "./state/reducers/console";
import { showNotification } from "./state/reducers/notification";
import { setOfPackageManagerVersion } from "./state/reducers/meta";
import { setConfig } from "./state/reducers/config";
import {
  setAvailableCoreAddons,
  addCoreAddon
} from "./state/reducers/corePackages";
import {
  setAvailableGlobalPackages,
  addGlobalPackage
} from "./state/reducers/globalPackages";
import {
  setAvailableLocalPackages,
  setPackagesDatabase,
  addLocalPackage
} from "./state/reducers/localPackages";
import { addPlatform } from "./state/reducers/platforms";
import {
  setAvailableTemplates,
  setSelectedTemplate
} from "./state/reducers/templates";

ReactDOM.render(<App />, document.getElementById("root"));
serviceWorker.unregister();

ipcRenderer.on("readJsonFileResponse", (event, arg) => {
  store.dispatch(setConfig(arg.content));
  ipcRenderer.send("getVersion", { config: arg.content });
  ipcRenderer.send("getTemplates", { config: arg.content });
});
ipcRenderer.on("getVersionResponse", (event, arg) => {
  if (arg.success) {
    store.dispatch(setOfPackageManagerVersion(arg.payload));
  }
});

ipcRenderer.on("getCoreAddonsResponse", (event, arg) => {
  store.dispatch(setAvailableCoreAddons(arg.payload.data));
});
ipcRenderer.on("getGloballyInstalledPackagesResponse", (event, arg) => {
  store.dispatch(setAvailableGlobalPackages(arg.payload.data));
});
ipcRenderer.on("getLocallyInstalledPackagesResponse", (event, arg) => {
  store.dispatch(setAvailableLocalPackages(arg.payload.data));
});
ipcRenderer.on("getAvailablePackagesResponse", (event, arg) => {
  store.dispatch(setPackagesDatabase(arg.payload.data));
});
ipcRenderer.on("getPackagesListedInAddonsMakeResponse", (event, arg) => {
  arg.payload.data.forEach(p => {
    if (p.path.startsWith("local_addons")) {
      store.dispatch(addLocalPackage(p));
    } else {
      const coreAddons = store.getState().corePackages.coreAddons;
      let isCorePackage = false;
      for (const [index, el] of coreAddons.entries()) {
        if (el === p.path) {
          isCorePackage = true;
          break;
        }
      }
      if (isCorePackage) {
        store.dispatch(addCoreAddon(p.path));
      } else {
        store.dispatch(addGlobalPackage(p));
      }
    }
  });
});
ipcRenderer.on("output", (event, arg) => {
  store.dispatch(appendOutput(arg.value));
});
ipcRenderer.on("createProjectResponse", (event, arg) => {
  store.dispatch(appendOutput(arg.value));
});
ipcRenderer.on("updateProjectResponse", (event, arg) => {
  store.dispatch(showNotification("successfully updated project"));
});
ipcRenderer.on("updateMultipleResponse", (event, arg) => {
  store.dispatch(showNotification("successfully updated multiple projects"));
});
ipcRenderer.on("installPackageByIdResponse", (event, arg) => {
  store.dispatch(showNotification("successfully installed package"));
});
ipcRenderer.on("installPackageByGithubResponse", (event, arg) => {
  store.dispatch(showNotification("successfully installed package"));
});
ipcRenderer.on("installPackageByUrlResponse", (event, arg) => {
  store.dispatch(showNotification("successfully installed package"));
});
ipcRenderer.on("getPlatformResponse", (event, arg) => {
  console.log("on get platform response ", arg);
  if (arg.platform === "win32") {
    store.dispatch(addPlatform("vs"));
    store.dispatch(setSelectedTemplate("vs"));
  } else if (arg.platform === "darwin") {
    store.dispatch(addPlatform("osx"));
    store.dispatch(setSelectedTemplate("osx"));
  } else if (arg.platform === "linux") {
    // TODO: linux64?
    store.dispatch(addPlatform("linux"));
    store.dispatch(setSelectedTemplate("linux"));
  }
});
ipcRenderer.on("getTemplatesResponse", (event, arg) => {
  store.dispatch(setAvailableTemplates(arg.templates));
});

ipcRenderer.send("readJsonFile", { path: "assets/config.json" });
ipcRenderer.send("getPlatform", {});
