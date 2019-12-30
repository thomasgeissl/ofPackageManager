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

          let packagesList = "";
          state.corePackages.selected.forEach(p => {
            packagesList += `${p}, `;
          });

          [
            ...state.globalPackages.selected,
            ...state.localPackages.selected
          ].forEach(p => {
            // packagesList += `${p.path}` + "#" + `${p.url}@${p.checkout}, `;
            packagesList += `${p.path}, `;
          });
          ipcRenderer.send("updateProject", {
            path: state.config.cwd,
            packagesList
          });

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
          });

          [
            ...state.globalPackages.selected,
            ...state.localPackages.selected
          ].forEach(p => {
            ipcRenderer.send("addPackageToAddonsMakeFile", {
              ...p,
              cwd: state.config.cwd
            });
          });
        }}
      >
        generate
      </Button>
    </Grid>
  </Grid>
);
