const { ipcMain } = require("electron");
const fs = require("fs");
const path = require("path");
const { logAndSendToWebConsole } = require("./utils.js");

ipcMain.on("doesDirectoryExist", (event, arg) => {
  event.reply("doesDirectoryExistResponse", {
    path: arg.name ? path.join(arg.location, arg.name) : arg.location,
    value: arg.name
      ? fs.existsSync(path.join(arg.location, arg.name))
      : fs.existsSync(arg.location)
  });
});

ipcMain.on("createDirectory", (event, arg) => {
  if (!fs.existsSync(arg.path)) {
    fs.mkdirSync(arg.path);
  }
  event.reply("createDirectoryResponse", { success: true });
  // event.reply("doesDirectoryExistResponse", {
  //   path: path.join(arg.location, arg.name),
  //   value: fs.existsSync(path.join(arg.location, arg.name))
  // });
});
ipcMain.on("removeAddonsMakeFile", (event, arg) => {
  if (fs.existsSync(path.join(arg.cwd, "addons.make"))) {
    fs.unlinkSync(path.join(arg.cwd, "addons.make"));
  }
});
ipcMain.on("writeJsonFile", (event, arg) => {
  fs.writeFileSync(
    path.join(__dirname, "..", arg.path),
    JSON.stringify(arg.content, {}, 2)
  );
});
ipcMain.on("readJsonFile", (event, arg) => {
  const rawdata = fs.readFileSync(path.join(__dirname, "..", arg.path));
  const data = JSON.parse(rawdata);
  // console.log(data);
  event.reply("readJsonFileResponse", {
    path: arg.path,
    content: data
  });
});

ipcMain.on("getPlatform", (event, arg) => {
  logAndSendToWebConsole("getting platform", event);
  logAndSendToWebConsole(process.platform, event);

  event.reply("getPlatformResponse", {
    platform: process.platform
  });
});

ipcMain.on("getTemplates", (event, arg) => {
  logAndSendToWebConsole("getting templates", event);
  const { config } = arg;

  // TODO: filter only directories
  const templates = fs
    .readdirSync(path.join(config.ofPath, "scripts/templates"), {
      withFileTypes: true
    })
    .filter(dirent => dirent.isDirectory())
    .map(dirent => dirent.name);

  logAndSendToWebConsole(JSON.stringify(templates), event);

  event.reply("getTemplatesResponse", {
    templates
  });
});
