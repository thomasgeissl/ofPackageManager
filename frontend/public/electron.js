const electron = require("electron");
const open = require("open");
const app = electron.app;
const BrowserWindow = electron.BrowserWindow;

const path = require("path");
const isDev = require("electron-is-dev");
require("./ipcs/ofPackageManager.js");
require("./ipcs/projectGenerator.js");
require("./ipcs/fs.js");

let mainWindow;

function createWindow() {
  mainWindow = new BrowserWindow({
    width: 1200,
    height: 1000,
    webPreferences: {
      nodeIntegration: true
    }
  });
  mainWindow.loadURL(
    isDev
      ? "http://localhost:3000"
      : `file://${path.join(__dirname, "../build/index.html")}`
  );
  if (isDev) {
    // Open the DevTools.
    //BrowserWindow.addDevToolsExtension('<location to your react chrome extension>');
    // mainWindow.webContents.openDevTools();
  }
  mainWindow.webContents.on("new-window", function(event, url) {
    console.log(event, url);
    event.preventDefault();
    open(url);
  });
  mainWindow.on("closed", () => (mainWindow = null));
}

app.on("ready", createWindow);

app.on("window-all-closed", () => {
  if (process.platform !== "darwin") {
    app.quit();
  }
});

app.on("activate", () => {
  if (mainWindow === null) {
    createWindow();
  }
});
