import { ipcRenderer } from "electron";
import React, { useState } from "react";
import { useDispatch } from "react-redux";
import { useHistory } from "react-router-dom";
import Button from "@material-ui/core/Button";
import TextField from "@material-ui/core/TextField";
import { setCwd as setCwdCreator } from "../state/reducers/config";
import Header from "./Header";
const { dialog } = require("electron").remote;

export default () => {
  const dispatch = useDispatch();
  const [location, setLocation] = useState("");
  const [cwd, setCwd] = useState("");
  const [valid, setValid] = useState(false);
  const history = useHistory();
  ipcRenderer.on("doesDirectoryExistResponse", (event, arg) => {
    setValid(!arg.value);
    setCwd(arg.path);
    dispatch(setCwdCreator(arg.path));
  });
  return (
    <>
      <Header></Header>
      <TextField
        id="input-with-icon-textfield"
        label="location"
        value={location}
        onChange={event => {}}
        onKeyPress={event => {}}
        onClick={event => {
          dialog
            .showOpenDialog({ properties: ["openDirectory"] })
            .then(result => {
              if (result.filePaths.length) {
                setLocation(result.filePaths[0]);
              }
            })
            .catch(err => {
              console.log(err);
            });
        }}
        fullWidth
      />
      <TextField
        id="input-with-icon-textfield"
        label="name"
        onChange={event => {
          ipcRenderer.send("doesDirectoryExist", {
            location,
            name: event.target.value
          });
        }}
        onKeyPress={event => {}}
        fullWidth
      />

      <Button
        disabled={!valid}
        onClick={event => {
          //   ipcRenderer.send("createDirectory", { path: cwd });
          ipcRenderer.send("getConfig", {});
          ipcRenderer.send("createProject", { path: cwd });
          ipcRenderer.send("getCoreAddons", {});
          ipcRenderer.send("getGloballyInstalledPackages", {});
          ipcRenderer.send("getLocallyInstalledPackages", {
            cwd
          });
          ipcRenderer.send("getAvailablePackages", {});
          history.push("/configProject");
        }}
      >
        next
      </Button>
    </>
  );
};
