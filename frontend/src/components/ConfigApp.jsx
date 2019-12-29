import React from "react";
import { useSelector, useDispatch } from "react-redux";
import FormControlLabel from "@material-ui/core/FormControlLabel";
import Switch from "@material-ui/core/Switch";
import styled from "styled-components";

import {
  setShowAdvancedFeatures,
  setVerboseOutput
} from "../state/reducers/config";
const Container = styled.div`
  background-color: white;
  position: relative;
  width: 50%;
  height: 20%;
  padding: 15px;
  overflow: auto;
`;
const List = styled.ul`
  list-style-type: none;
`;
export default () => {
  const showAdvancedFeatures = useSelector(
    state => state.config.showAdvancedFeatures
  );
  const verboseOutput = useSelector(state => state.config.verboseOutput);
  const dispatch = useDispatch();
  return (
    <Container>
      <List>
        <li>
          <FormControlLabel
            control={
              <Switch
                color="primary"
                checked={showAdvancedFeatures}
                onChange={(event, value) => {
                  dispatch(setShowAdvancedFeatures(value));
                }}
                value={showAdvancedFeatures}
              />
            }
            label="show advanced features"
          />
        </li>
        <li>
          <FormControlLabel
            control={
              <Switch
                color="primary"
                checked={verboseOutput}
                onChange={(event, value) => {
                  dispatch(setVerboseOutput(value));
                }}
                value={verboseOutput}
              />
            }
            label="verbose output"
          />
        </li>
      </List>
    </Container>
  );
};
