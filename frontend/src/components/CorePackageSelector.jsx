import React from "react";
import { useSelector, useDispatch } from "react-redux";
import Checkbox from "@material-ui/core/Checkbox";
import FormControlLabel from "@material-ui/core/FormControlLabel";
import styled from "styled-components";
import { addCoreAddon, removeCoreAddon } from "../state/reducers/corePackages";

const PackageList = styled.ul`
  text-align: left;
  columns: 5;
  list-style-type: none;
`;

export default () => {
  const dispatch = useDispatch();
  const handleChange = name => {
    return (event, value) => {
      if (value) {
        dispatch(addCoreAddon(name));
      } else {
        dispatch(removeCoreAddon(name));
      }
    };
  };
  const coreAddons = useSelector(state => state.corePackages.coreAddons);
  const selectedCoreAddons = useSelector(state => state.corePackages.selected);
  const isCoreAddonSelected = value => {
    return selectedCoreAddons.indexOf(value) > -1;
  };

  return (
    <>
      <PackageList>
        {coreAddons.map(function(name, index) {
          return (
            <li key={index}>
              <FormControlLabel
                control={
                  <Checkbox
                    color="primary"
                    checked={isCoreAddonSelected(name)}
                    onChange={handleChange(name)}
                    value={name}
                    size="small"
                  />
                }
                label={name}
              />
            </li>
          );
        })}
      </PackageList>
    </>
  );
};
