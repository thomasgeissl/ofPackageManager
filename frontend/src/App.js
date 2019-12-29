import React from "react";
import { BrowserRouter as Router, Switch, Route } from "react-router-dom";
import { Provider as StoreProvider } from "react-redux";
import store from "./state/store";
import Home from "./components/Home";
import New from "./components/New";
import Update from "./components/Open";
import UpdateMultiple from "./components/UpdateMultiple";
import ConfigProject from "./components/ConfigProject";
import { createMuiTheme, ThemeProvider } from "@material-ui/core/styles";
import "./App.css";
import Footer from "./components/Footer";
import Header from "./components/Header";
import Console from "./components/Console";
import styled from "styled-components";

const StyledApp = styled.div`
  height: 100vh;
  display: flex;
  flex-direction: column;
  justify-content: flex-start;
`;
const StyledContent = styled.div`
  flex-grow: 1;
`;
const StyledConsole = styled(Console)`
  max-height: 25%;
`;

const theme = createMuiTheme({
  palette: {
    primary: {
      main: "#E71B74"
    }
  },
  status: {
    danger: "orange"
  }
});

function App() {
  return (
    <StyledApp className="App">
      <StoreProvider store={store}>
        <ThemeProvider theme={theme}>
          <StyledContent>
            <Router>
              <Switch>
                <Route path="/configProject">
                  <ConfigProject></ConfigProject>
                </Route>
                <Route path="/new">
                  <New></New>
                </Route>
                <Route path="/update">
                  <Update></Update>
                </Route>
                <Route path="/updateMultiple">
                  <UpdateMultiple></UpdateMultiple>
                </Route>
                <Route path="/">
                  <Home></Home>
                </Route>
              </Switch>
            </Router>
          </StyledContent>
          <StyledConsole></StyledConsole>
          <Footer></Footer>
        </ThemeProvider>
      </StoreProvider>
    </StyledApp>
  );
}

export default App;
