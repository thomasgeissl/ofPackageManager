import { ipcRenderer } from "electron";
import React, { useState } from "react";
import { useDispatch, useSelector } from "react-redux";
import { useHistory } from "react-router-dom";
import Button from "@material-ui/core/Button";
import TextField from "@material-ui/core/TextField";
import Grid from "@material-ui/core/Grid";
import { setCwd as setCwdCreator } from "../state/reducers/config";
import Header from "./Header";
import styled from "styled-components";
const { dialog } = require("electron").remote;

const Container = styled.div``;
const StyledForm = styled.form`
  width: 50%;
  margin: auto;
`;
const Actions = styled(Grid)`
  margin-top: 25px;
`;

let doesDirectoryExistResponseCallback = (event, arg) => {};

ipcRenderer.on("doesDirectoryExistResponse", (event, arg) => {
  doesDirectoryExistResponseCallback(event, arg);
});

export default () => {
  const dispatch = useDispatch();
  const defaultProjectPath = useSelector(state => state.config);
  const [location, setLocation] = useState("");
  const [name, setName] = useState("");
  const [cwd, setCwd] = useState("");
  const [valid, setValid] = useState(false);
  const history = useHistory();

  doesDirectoryExistResponseCallback = (event, arg) => {
    setValid(!arg.value);
    setCwd(arg.path);
    dispatch(setCwdCreator(arg.path));
  };

  return (
    <Container>
      <Header></Header>
      <StyledForm>
        <TextField
          label="location"
          value={location}
          onChange={event => {}}
          onKeyPress={event => {}}
          onClick={event => {
            dialog
              .showOpenDialog({
                // defaultPath: defaultProjectPath,
                properties: ["openDirectory"]
              })
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
          label="name"
          value={name}
          onChange={event => {
            setName(event.target.value);
            ipcRenderer.send("doesDirectoryExist", {
              location,
              name: event.target.value
            });
          }}
          onKeyPress={event => {}}
          fullWidth
        />
        <Actions container alignItems="flex-start" justify="flex-end">
          <Grid item>
            <Button
              variant="contained"
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
          </Grid>
        </Actions>
      </StyledForm>
    </Container>
  );
};