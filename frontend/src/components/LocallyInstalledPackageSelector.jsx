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

export default () => {
  const dispatch = useDispatch();
  const packages = useSelector(state => state.localPackages.packages);
  const selectedPackages = useSelector(state => state.localPackages.selected);

  const isPackageSelected = value => {
    const index = selectedPackages.findIndex(item => item.path === value.path);
    return index > -1;
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
        {packages.map(function(value, index) {
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
