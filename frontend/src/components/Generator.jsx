import { ipcRenderer } from "electron";
import React from "react";
import { Button, Grid } from "@material-ui/core";
import styled from "styled-components";
import store from "../state/store";

export default () => (
  <Grid container alignItems="flex-start" justify="flex-end">
    <Grid item>
      <Button
        color="primary"
        variant="contained"
        onClick={event => {
          const state = store.getState();
          ipcRenderer.send("removeAddonsMakeFile", {
            cwd: state.config.cwd
          });
          state.corePackages.selected.forEach(p => {
            ipcRenderer.send("addPackageToAddonsMakeFile", {
              path: p,
              url: "",
              checkout: "",
              cwd: state.config.cwd
            });
            console.log("install", p);
          });
          state.globalPackages.selected.forEach(p => {
            ipcRenderer.send("addPackageToAddonsMakeFile", {
              ...p,
              cwd: state.config.cwd
            });
            console.log("install", p);
          });
          state.localPackages.selected.forEach(p => {
            ipcRenderer.send("addPackageToAddonsMakeFile", {
              ...p,
              cwd: state.config.cwd
            });
            console.log("install", p);
          });
          ipcRenderer.send("updateProject", { path: state.config.cwd });
        }}
      >
        generate
      </Button>
    </Grid>
  </Grid>
);
