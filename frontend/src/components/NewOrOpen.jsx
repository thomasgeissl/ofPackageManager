import React from "react";
import Button from "@material-ui/core/Button";
import Grid from "@material-ui/core/Grid";
import { useHistory } from "react-router-dom";
import styled from "styled-components";

const Container = styled.div`
  padding: 100px;
`;
const StyledButton = styled(Button)`
  width: 200px;
  height: 100px;
`;

export default () => {
  const history = useHistory();

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
            open
          </StyledButton>
        </Grid>
      </Grid>
    </Container>
  );
};
