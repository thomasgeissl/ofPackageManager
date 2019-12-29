import { ipcRenderer } from "electron";
import React from "react";
import { useDispatch } from "react-redux";
import { useHistory } from "react-router-dom";
import { setCwd as setCwdCreator } from "../state/reducers/config";
const { dialog } = require("electron").remote;

export default () => {
  const dispatch = useDispatch();
  const history = useHistory();
  dialog
    .showOpenDialog({ properties: ["openDirectory"] })
    .then(result => {
      if (result.canceled) {
        history.push("/");
      } else {
        if (result.filePaths.length) {
          console.log("canceled");
          const cwd = result.filePaths[0];
          dispatch(setCwdCreator(cwd));

          ipcRenderer.send("getConfig", {});
          ipcRenderer.send("getCoreAddons", {});
          ipcRenderer.send("getGloballyInstalledPackages", {});
          ipcRenderer.send("getLocallyInstalledPackages", {
            cwd
          });
          ipcRenderer.send("getAvailablePackages", {});
          ipcRenderer.send("getPackagesListedInAddonsMake", { cwd });

          history.push("/configProject");
        }
      }
    })
    .catch(err => {
      console.log(err);
    });
  return <></>;
};