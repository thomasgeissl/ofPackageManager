import { ipcRenderer } from "electron";
import React from "react";
import { useSelector, useDispatch } from "react-redux";
import FormControlLabel from "@material-ui/core/FormControlLabel";
import Checkbox from "@material-ui/core/Checkbox";
import Tooltip from "@material-ui/core/Tooltip";
import {
  addLocalPackage,
  removeLocalPackage
} from "../state/reducers/localPackages";

import PackageList from "./PackageList";
import InstallButton from "./buttons/Install";

export default () => {
  const dispatch = useDispatch();
  const config = useSelector(state => state.config);
  const cwd = useSelector(state => state.project.cwd);
  const localPackages = useSelector(state => state.localPackages.packages);
  const globalPackages = useSelector(state => state.globalPackages.packages);
  const database = useSelector(state => state.localPackages.database);
  const selectedPackages = useSelector(state => state.localPackages.selected);
  const selectedGlobalPackages = useSelector(
    state => state.globalPackages.selected
  );
  const isPackageInstalled = value => {
    const index = localPackages.findIndex(item => item.path === value.path);
    return index > -1;
  };
  const isPackageSelected = value => {
    const localIndex = selectedPackages.findIndex(
      item => item.path === value.path
    );
    const globalIndex = selectedGlobalPackages.findIndex(
      item => item.path === value.path
    );
    return localIndex > -1 || globalIndex > -1;
  };

  const hasMissingPackages = () => {
    let value = false;
    selectedPackages.map(p => {
      if (!isPackageInstalled(p)) {
        value = true;
      }
    });
    return value;
  };

  const handleSelectedChange = p => {
    return (event, value) => {
      if (value) {
        dispatch(addLocalPackage(p));
      } else {
        dispatch(removeLocalPackage(p));
      }
    };
  };

  return (
    <>
      <PackageList>
        {localPackages.map(function(value, index) {
          return (
            <li key={index}>
              <Tooltip title={`${value.path}\n${value.url}@${value.checkout}`}>
                <FormControlLabel
                  control={
                    <Checkbox
                      color="primary"
                      checked={isPackageSelected(value)}
                      onChange={handleSelectedChange(value)}
                      value={value.path}
                      size="small"
                    />
                  }
                  label={value.path.split(/[\\/]/).pop()}
                />
              </Tooltip>
            </li>
          );
        })}
      </PackageList>
    </>
  );
};
