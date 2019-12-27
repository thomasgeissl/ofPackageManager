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

// ipcRenderer.on("getVersionResponse", (event, arg) => {
//   console.log(arg);
// });
// ipcRenderer.on("installPackageByIdResponse", (event, arg) => {
//   console.log(arg);
// });
// ipcRenderer.on("installPackageByGithubResponse", (event, arg) => {
//   console.log(arg);
// });
// ipcRenderer.on("installPackageByUrlResponse", (event, arg) => {
//   console.log(arg);
// });

// ipcRenderer.send("getVersion", "");
// ipcRenderer.send("getCoreAddons", "");
// ipcRenderer.send("getAvailablePackages", "");
// ipcRenderer.send("installPackageById", "ofxMidi");
// ipcRenderer.send("installPackageByGithub", "thomasgeissl/ofxGit2");
// ipcRenderer.send(
//   "installPackageByUrl",
//   "https://github.com/danomatika/ofxLua.git"
// );

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
          </Router>
        </ThemeProvider>
      </StoreProvider>
    </div>
  );
}

export default App;
