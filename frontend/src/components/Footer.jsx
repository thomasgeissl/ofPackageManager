import React from "react";
import { useSelector } from "react-redux";
import styled from "styled-components";

const Container = styled.footer`
  text-align: left;
  padding-top: 5px;
  padding-bottom: 5px;
`;
export default () => {
  const version = useSelector(state => state.meta.ofPackageManagerVersion);
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
