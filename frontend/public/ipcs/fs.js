const { ipcMain } = require("electron");
const fs = require("fs");
const path = require("path");

ipcMain.on("doesDirectoryExist", (event, arg) => {
  event.reply("doesDirectoryExistResponse", {
    path: path.join(arg.location, arg.name),
    value: fs.existsSync(path.join(arg.location, arg.name))
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
