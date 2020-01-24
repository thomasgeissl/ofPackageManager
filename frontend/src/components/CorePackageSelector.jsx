import React from "react";
import { useSelector, useDispatch } from "react-redux";
import Checkbox from "@material-ui/core/Checkbox";
import FormControlLabel from "@material-ui/core/FormControlLabel";
import SvgIcon from "@material-ui/core/SvgIcon";
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
              <SvgIcon viewBox="0 0 256 256">
                <g
                  id="logo"
                  stroke="none"
                  strokeWidth="1"
                  fill="none"
                  fillRule="evenodd"
                >
                  <g
                    id="Group"
                    transform="translate(6.000000, 63.000000)"
                    fill="#000000"
                  >
                    <rect
                      id="Rectangle"
                      stroke="#979797"
                      x="189"
                      y="58"
                      width="34"
                      height="34"
                    ></rect>
                    <rect
                      id="Rectangle"
                      stroke="#979797"
                      x="132"
                      y="1"
                      width="54"
                      height="128"
                    ></rect>
                    <polygon
                      id="Triangle"
                      stroke="#979797"
                      points="189 1 242 1 189 54"
                    ></polygon>
                    <circle id="Oval" cx="65" cy="65" r="65"></circle>
                  </g>
                </g>
              </SvgIcon>
            </li>
          );
        })}
      </PackageList>
    </>
  );
};
