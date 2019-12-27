import React from "react";
import { useSelector, useDispatch } from "react-redux";
import styled from "styled-components";

import Header from "./Header";
import CorePackageSelector from "./CorePackageSelector";
import GloballyInstalledPackageSelector from "./GloballyInstalledPackageSelector";
import LocallyInstalledPackageSelector from "./LocallyInstalledPackageSelector";
import Generator from "./Generator";

// import config from "../../public/assets/config.json";
// const ofPath = config.ofPath;

const Headline = styled.h2`
  text-align: left;
`;
const Subline = styled.span`
  font-size: 0.5em;
`;

const ProjectInfo = styled.section`
  text-align: left;
`;

const StyledGenerator = styled(Generator)`
  position: fixed;
  bottom: 0;
  right: 0;
`;

export default () => {
  const cwd = useSelector(state => state.config.cwd);
  const ofPath = useSelector(state => state.config.ofPath);
  return (
    <>
      <Header></Header>
      <ProjectInfo>
        <div>project path: {cwd}</div>
        <div>oF path: {ofPath}</div>
      </ProjectInfo>
      <Headline>
        core addons{" "}
        <Subline>addons that come bundled with openFrameworks</Subline>
      </Headline>
      <CorePackageSelector></CorePackageSelector>
      <Headline>
        global addons{" "}
        <Subline>
          addons that have been installed globally to openFrameworks/addons
          directory and are shared between multiple projects
        </Subline>
      </Headline>
      <GloballyInstalledPackageSelector></GloballyInstalledPackageSelector>
      <Headline>
        local addons <Subline>addons that live inside your project</Subline>
      </Headline>
      <LocallyInstalledPackageSelector></LocallyInstalledPackageSelector>
      <StyledGenerator></StyledGenerator>
    </>
  );
};
