import React from "react";
import { useHistory } from "react-router-dom";
import { useDispatch, useSelector } from "react-redux";
import Button from "@material-ui/core/Button";
import Grid from "@material-ui/core/Grid";
import styled from "styled-components";
import { clearCorePackages } from "../state/reducers/corePackages";
import { clearGlobalPackages } from "../state/reducers/globalPackages";
import { clearLocalPackages } from "../state/reducers/localPackages";
import { clearProject } from "../state/reducers/project";

const Container = styled.div`
  padding: 100px;
`;

const StyledCliSection = styled.section`
  margin-top: 100px;
  margin-bottom: 50px;
  font-size: 1.25em;
  text-align: left;
`;
const StyledButton = styled(Button)`
  width: 200px;
  height: 100px;
`;

export default () => {
  const history = useHistory();
  const dispatch = useDispatch();
  const showAdvancedFeatures = useSelector(
    state => state.config.showAdvancedFeatures
  );

  return (
    <Container>
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
              dispatch(clearCorePackages());
              dispatch(clearGlobalPackages());
              dispatch(clearLocalPackages());
              dispatch(clearProject());
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
              dispatch(clearCorePackages());
              dispatch(clearGlobalPackages());
              dispatch(clearLocalPackages());
              dispatch(clearProject());
              history.push("/update");
            }}
            size="large"
          >
            update
          </StyledButton>
        </Grid>
        {showAdvancedFeatures && (
          <Grid item>
            <StyledButton
              variant="contained"
              onClick={() => {
                dispatch(clearCorePackages());
                dispatch(clearGlobalPackages());
                dispatch(clearLocalPackages());
                dispatch(clearProject());
                history.push("/updateMultiple");
              }}
              size="large"
            >
              update multiple
            </StyledButton>
          </Grid>
        )}
      </Grid>
      <StyledCliSection>
        By the way, <br></br>
        there is also a cli version of the package manager available.<br></br>
        Feel free to check out its{" "}
        <a
          href="https://thomasgeissl.github.io/ofPackageManager/"
          target="_blank"
        >
          website
        </a>
        .
      </StyledCliSection>
    </Container>
  );
};
