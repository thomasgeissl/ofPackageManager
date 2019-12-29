import React from "react";
import { useSelector, useDispatch } from "react-redux";
import styled from "styled-components";

const Container = styled.div`
  background-color: black;
  color: white;
  text-align: left;
  overflow-y: scroll;
  padding: 5px;
`;
export default () => {
  const output = useSelector(state => state.console.output);
  return (
    <Container>
      {output.split("\n").map(function(item, key) {
        return (
          <span key={key}>
            {item}
            <br />
          </span>
        );
      })}
    </Container>
  );
};
