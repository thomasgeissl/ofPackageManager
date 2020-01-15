import React from "react";
import { useSelector, useDispatch } from "react-redux";
import Box from "@material-ui/core/Box";
import Grid from "@material-ui/core/Grid";
import styled from "styled-components";

import Header from "./Header";
import CorePackageSelector from "./CorePackageSelector";
import GloballyInstalledPackageSelector from "./GloballyInstalledPackageSelector";
import LocallyInstalledPackageSelector from "./LocallyInstalledPackageSelector";
import PackageInstaller from "./PackageInstaller";
import PlatformSelector from "./PlatformSelector";
import TemplateSelector from "./TemplateSelector";
import Generator from "./Generator";

const Headline = styled.h2`
  margin-top: 10px;
  margin-bottom: 10px;
`;
const Subline = styled.h3``;
const Description = styled.span`
  font-size: 0.5em;
`;

const ProjectInfo = styled.section`
  margin-top: 15px;
  margin-bottom: 15px;
  text-align: left;
`;

const StyledGenerator = styled(Generator)`
  position: fixed;
  bottom: 0;
  right: 0;
`;

const StyledBox = styled(Box)`
  margin-bottom: 15px;
  padding: 15px;
`;

export default () => {
  const cwd = useSelector(state => state.project.cwd);
  const name = useSelector(state => state.project.name);
  const ofPath = useSelector(state => state.config.ofPath);
  const locallyInstalledPackages = useSelector(
    state => state.localPackages.packages
  );
  const showAdvancedFeatures = useSelector(
    state => state.config.showAdvancedFeatures
  );
  return (
    <>
      <Header></Header>
      <ProjectInfo>
        {/* <div>project path: {cwd}</div> */}
        {/* <div>oF path: {ofPath}</div> */}
        <div>{name}</div>
      </ProjectInfo>
      <StyledBox border={1} borderRadius={8} borderColor="primary.main">
        <Headline>Select or install addons</Headline>
        <Subline>
          global addons{" "}
          <Description>
            addons that come bundled with openFrameworks or have been installed
            globally
          </Description>
        </Subline>
        <CorePackageSelector></CorePackageSelector>
        {/* <Headline>
        global addons{" "}
        <Subline>
          addons that have been installed globally to openFrameworks/addons
          directory and are shared between multiple projects
        </Subline>
      </Headline> */}
        <GloballyInstalledPackageSelector></GloballyInstalledPackageSelector>
        {locallyInstalledPackages.length > 0 && (
          <>
            <Subline>
              local addons{" "}
              <Description>addons that live inside your project</Description>
            </Subline>
            <LocallyInstalledPackageSelector></LocallyInstalledPackageSelector>
          </>
        )}
        <PackageInstaller></PackageInstaller>
      </StyledBox>

      <StyledBox border={1} borderRadius={8} borderColor="primary.main">
        <Headline>Generate IDE project files</Headline>
        {showAdvancedFeatures && (
          <Grid container spacing={3}>
            <Grid item xs>
              <PlatformSelector></PlatformSelector>
            </Grid>
            <Grid item xs>
              <TemplateSelector></TemplateSelector>
            </Grid>
          </Grid>
        )}
        <StyledGenerator></StyledGenerator>
      </StyledBox>
    </>
  );
};
