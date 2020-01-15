import React from "react";
import { useSelector, useDispatch } from "react-redux";
import Checkbox from "@material-ui/core/Checkbox";
import FormControlLabel from "@material-ui/core/FormControlLabel";
import SvgIcon from "@material-ui/core/SvgIcon";
import CheckCircleOutlineIcon from "@material-ui/icons/CheckCircleOutline";
import styled from "styled-components";
import { addCoreAddon, removeCoreAddon } from "../state/reducers/corePackages";

import PackageList from "./PackageList";

const StyledFormControlLabel = styled(FormControlLabel)`
  margin-right: 0 !important;
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
              <StyledFormControlLabel
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
              {/* TODO: oF icon */}
              {/* <SvgIcon>
                <path d="M10 20v-6h4v6h5v-8h3L12 3 2 12h3v8z" />
              </SvgIcon> */}
              <CheckCircleOutlineIcon fontSize="small"></CheckCircleOutlineIcon>
            </li>
          );
        })}
      </PackageList>
    </>
  );
};
