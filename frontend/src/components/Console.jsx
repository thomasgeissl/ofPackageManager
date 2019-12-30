import React from "react";
import { useSelector, useDispatch } from "react-redux";
import styled from "styled-components";

const Container = styled.div`
  background-color: black;
  color: white;
  text-align: left;
  overflow-y: scroll;
  padding-left: 5px;
  padding-right: 5px;
  max-height: 20%;
`;
export default () => {
  const output = useSelector(state => state.console.output);
  const showConsole = useSelector(state => state.config.showConsole);
  return (
    <Container>
      {output === ""
        ? output
        : output.split("\n").map(function(item, key) {
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
