import { ipcRenderer } from "electron";
import React from "react";
import { useSelector } from "react-redux";
import { Button, Grid } from "@material-ui/core";
import styled from "styled-components";
import store from "../state/store";

export default () => {
  const cwd = useSelector(state => state.project.cwd);
  const config = useSelector(state => state.config);
  const platforms = useSelector(state => state.platforms.selected);
  const templates = useSelector(state => state.templates.selected);

  return (
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
              config,
              path: cwd,
              packagesList,
              platforms,
              templates
            });

            ipcRenderer.send("removeAddonsMakeFile", {
              cwd
            });
            state.corePackages.selected.forEach(p => {
              ipcRenderer.send("addPackageToAddonsMakeFile", {
                config,
                path: p,
                url: "",
                checkout: "",
                cwd
              });
            });

            [
              ...state.globalPackages.selected,
              ...state.localPackages.selected
            ].forEach(p => {
              ipcRenderer.send("addPackageToAddonsMakeFile", {
                config,
                ...p,
                cwd
              });
            });
          }}
        >
          generate
        </Button>
      </Grid>
    </Grid>
  );
};
