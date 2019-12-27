import React from "react";
import { useHistory } from "react-router-dom";
import styled from "styled-components";
import Button from "@material-ui/core/Button";
import ArrowBackIcon from "@material-ui/icons/ArrowBack";

const Container = styled.footer`
  text-align: left;
  width: 100%;
`;
export default () => {
  const history = useHistory();
  return (
    <Container>
      <Button
        variant="contained"
        onClick={event =>
          // history.goBack()
          history.push("/")
        }
      >
        <ArrowBackIcon></ArrowBackIcon>
      </Button>
    </Container>
  );
};
