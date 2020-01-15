import React, { useRef } from "react";
import { useSelector, useDispatch } from "react-redux";
import styled from "styled-components";

const Container = styled.div`
  max-height: 20%;
  overflow-y: scroll;
`;
const Console = styled.div`
  background-color: black;
  color: white;
  text-align: left;
  padding-left: 5px;
  padding-right: 5px;
`;
export default () => {
  const output = useSelector(state => state.console.output);
  const showConsole = useSelector(state => state.config.showConsole);
  const consoleRef = useRef(null);
  if (consoleRef.current) {
    window.scrollTo(0, consoleRef.current.scrollHeight);
  }

  return (
    <Container>
      {showConsole && (
        <Console ref={consoleRef}>
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
        </Console>
      )}
    </Container>
  );
};
