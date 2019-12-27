import { ipcRenderer } from "electron";
import React from "react";
import { useSelector } from "react-redux";
import { Button, Grid } from "@material-ui/core";
import styled from "styled-components";
import store from "../state/store";

const Container = styled.footer`
  text-align: left;
  position: fixed;
  bottom: 0;
  width: 100%;
`;
export default () => {
  const version = useSelector(state => state.config.ofPackageManagerVersion);
  return (
    <Container>
      <a
        href="https://github.com/thomasgeissl/ofPackageManager"
        target="_blank"
      >
        ofPackageManager
      </a>{" "}
      version: {version.major}.{version.minor}.{version.patch}
    </Container>
  );
};
