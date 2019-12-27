import React from "react";
import { BrowserRouter as Router, Switch, Route } from "react-router-dom";
import { Provider as StoreProvider } from "react-redux";
import store from "./state/store";
import NewOrOpen from "./components/NewOrOpen";
import New from "./components/New";
import Open from "./components/Open";
import ConfigProject from "./components/ConfigProject";
import { createMuiTheme, ThemeProvider } from "@material-ui/core/styles";
import "./App.css";
import Footer from "./components/Footer";
import Header from "./components/Header";

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
    <div className="App">
      <StoreProvider store={store}>
        <ThemeProvider theme={theme}>
          <Router>
            <Switch>
              <Route path="/configProject">
                <ConfigProject></ConfigProject>
              </Route>
              <Route path="/new">
                <New></New>
              </Route>
              <Route path="/open">
                <Open></Open>
              </Route>
              <Route path="/">
                <NewOrOpen></NewOrOpen>
              </Route>
            </Switch>
            <Footer></Footer>
          </Router>
        </ThemeProvider>
      </StoreProvider>
    </div>
  );
}

export default App;
