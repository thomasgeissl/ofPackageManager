import React from "react";
import { useSelector, useDispatch } from "react-redux";
import Checkbox from "@material-ui/core/Checkbox";
import FormControlLabel from "@material-ui/core/FormControlLabel";
import Tooltip from "@material-ui/core/Tooltip";
import styled from "styled-components";
import {
  addGlobalPackage,
  removeGlobalPackage
} from "../state/reducers/globalPackages";

import PackageList from "./PackageList";

// const Info = styled.span`
//   font-size: 8px;
// `;

export default () => {
  const dispatch = useDispatch();
  const handleChange = p => {
    return (event, value) => {
      if (value) {
        dispatch(addGlobalPackage(p));
      } else {
        dispatch(removeGlobalPackage(p));
      }
    };
  };
  const packages = useSelector(state => state.globalPackages.globalPackages);
  const selectedPackages = useSelector(state => state.globalPackages.selected);
  // const isPackageInstalled = value => {
  //   const index = packages.findIndex(item => item.path === value.path);
  //   return index > -1;
  // };
  const isPackageSelected = value => {
    const index = selectedPackages.findIndex(item => item.path === value.path);
    return index > -1;
  };
  return (
    <>
      <PackageList>
        {packages.map(function(value, index) {
          return (
            <li key={index}>
              <Tooltip title={`${value.url}@${value.checkout}`}>
                <FormControlLabel
                  control={
                    <Checkbox
                      color="primary"
                      checked={isPackageSelected(value)}
                      onChange={handleChange(value)}
                      value={value.path}
                      size="small"
                    />
                  }
                  label={value.path}
                />
                {/* <Info>
                  {value.url}@{value.checkout}
                </Info> */}
              </Tooltip>
            </li>
          );
        })}
      </PackageList>
    </>
  );
};
