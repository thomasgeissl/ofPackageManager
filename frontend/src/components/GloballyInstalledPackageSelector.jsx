import React from "react";
import { useSelector, useDispatch } from "react-redux";
import { FormControlLabel, Switch } from "@material-ui/core";
import styled from "styled-components";
import {
  addGlobalPackage,
  removeGlobalPackage
} from "../state/reducers/globalPackages";

const PackageList = styled.ul`
  text-align: left;
  columns: 5;
  list-style-type: none;
`;

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
              <section>
                <FormControlLabel
                  control={
                    <Switch
                      color="primary"
                      checked={isPackageSelected(value)}
                      onChange={handleChange(value)}
                      value={value.path}
                    />
                  }
                  label={value.path}
                />
                {/* <Info>
                  {value.url}@{value.checkout}
                </Info> */}
              </section>
            </li>
          );
        })}
      </PackageList>
    </>
  );
};
