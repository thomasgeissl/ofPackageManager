import { ipcRenderer } from "electron";
import React from "react";
import Button from "@material-ui/core/Button";
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
      if (result.filePaths.length) {
        const cwd = result.filePaths[0];
        dispatch(setCwdCreator(cwd));
        ipcRenderer.send("getCoreAddons", {});
        ipcRenderer.send("getGloballyInstalledPackages", {});
        ipcRenderer.send("getLocallyInstalledPackages", {
          cwd
        });
        ipcRenderer.send("getAvailablePackages", {});
        ipcRenderer.send("getPackagesListedInAddonsMake", { cwd });
        history.push("/configProject");
      }
    })
    .catch(err => {
      console.log(err);
    });
  return <>open</>;
};
