import React from "react";
import { useHistory } from "react-router-dom";
import styled from "styled-components";
import ArrowBackIcon from "@material-ui/icons/ArrowBack";

const Container = styled.footer`
  text-align: left;
  width: 100%;
`;
export default () => {
  const history = useHistory();
  return (
    <Container>
      <ArrowBackIcon
        onClick={event =>
          // history.goBack()
          history.push("/")
        }
      ></ArrowBackIcon>
    </Container>
  );
};
