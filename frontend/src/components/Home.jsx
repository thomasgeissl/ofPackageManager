import React, { useState } from "react";
import { useHistory } from "react-router-dom";
import { useSelector } from "react-redux";
import Button from "@material-ui/core/Button";
import Grid from "@material-ui/core/Grid";
import Modal from "@material-ui/core/Modal";
import SettingsIcon from "@material-ui/icons/Settings";
import styled from "styled-components";

import ConfigApp from "./ConfigApp";

const Container = styled.div`
  padding: 100px;
`;

const StyledCliSection = styled.section`
  margin-top: 100px;
  margin-bottom: 50px;
  font-size: 1.25em;
  text-align: left;
`;
const StyledModal = styled(Modal)`
  overflow: scroll;
  display: flex;
  align-items: center;
  justify-content: center;
`;
const StyledButton = styled(Button)`
  width: 200px;
  height: 100px;
`;

const ConfigButton = styled(Button)`
  /* TODO: fix the evil important  */
  position: fixed !important;
  top: 15px;
  right: 15px;
`;

export default () => {
  const history = useHistory();
  const showAdvancedFeatures = useSelector(
    state => state.config.showAdvancedFeatures
  );
  const [configModalOpen, setConfigModalOpen] = useState(false);

  return (
    <Container>
      <ConfigButton
        onClick={event => {
          setConfigModalOpen(true);
        }}
      >
        <SettingsIcon></SettingsIcon>
      </ConfigButton>
      <Grid
        container
        direction="row"
        justify="center"
        alignItems="center"
        spacing={4}
      >
        <Grid item>
          <StyledButton
            variant="contained"
            onClick={() => {
              history.push("/new");
            }}
            size="large"
          >
            new
          </StyledButton>
        </Grid>
        <Grid item>
          <StyledButton
            variant="contained"
            onClick={() => {
              history.push("/open");
            }}
            size="large"
          >
            update
          </StyledButton>
        </Grid>
        {showAdvancedFeatures && (
          <Grid item>
            <StyledButton variant="contained" onClick={() => {}} size="large">
              update multiple
            </StyledButton>
          </Grid>
        )}
      </Grid>
      <StyledCliSection>
        By the way, <br></br>
        there is also a cli version of the package manager available.<br></br>
        Feel free to check out the its{" "}
        <a
          href="https://thomasgeissl.github.io/ofPackageManager/"
          target="_blank"
        >
          website
        </a>
        .
      </StyledCliSection>
      <StyledModal
        open={configModalOpen}
        onClose={event => {
          setConfigModalOpen(false);
        }}
      >
        <ConfigApp></ConfigApp>
      </StyledModal>
    </Container>
  );
};
