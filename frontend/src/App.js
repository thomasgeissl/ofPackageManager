import React, { useState } from "react";
import { BrowserRouter as Router, Switch, Route } from "react-router-dom";
import { Provider as StoreProvider } from "react-redux";
import store from "./state/store";
import Home from "./components/Home";
import New from "./components/New";
import Update from "./components/Open";
import UpdateMultiple from "./components/UpdateMultiple";
import ConfigProject from "./components/ConfigProject";
import Button from "@material-ui/core/Button";
import Modal from "@material-ui/core/Modal";
import SettingsIcon from "@material-ui/icons/Settings";
import { createMuiTheme, ThemeProvider } from "@material-ui/core/styles";
import "./App.css";
import Footer from "./components/Footer";
import Console from "./components/Console";
import ConfigApp from "./components/ConfigApp";
import Notification from "./components/Notification";
import styled from "styled-components";

const StyledApp = styled.div`
  height: 100vh;
  display: flex;
  flex-direction: column;
  justify-content: flex-start;
`;
const StyledContent = styled.div`
  flex-grow: 1;
  overflow-y: scroll;
  padding-bottom: 15px;
`;
const StyledModal = styled(Modal)`
  overflow: scroll;
  display: flex;
  align-items: center;
  justify-content: center;
`;
const ConfigButton = styled(Button)`
  /* TODO: fix the evil important  */
  position: fixed !important;
  top: 15px;
  right: 15px;
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
  const [configModalOpen, setConfigModalOpen] = useState(false);
  return (
    <StyledApp className="App">
      <StoreProvider store={store}>
        <ThemeProvider theme={theme}>
          <StyledContent>
            <ConfigButton
              onClick={event => {
                setConfigModalOpen(true);
              }}
            >
              <SettingsIcon></SettingsIcon>
            </ConfigButton>
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
          <Console></Console>
          <Footer></Footer>
          <StyledModal
            open={configModalOpen}
            onClose={event => {
              setConfigModalOpen(false);
            }}
          >
            <ConfigApp></ConfigApp>
          </StyledModal>
          <Notification></Notification>
        </ThemeProvider>
      </StoreProvider>
    </StyledApp>
  );
}

export default App;
