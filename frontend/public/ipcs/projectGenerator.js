const { ipcMain } = require("electron");
const { execSync } = require("child_process");
const config = require("../assets/config.json");

const logAndSendToWebConsole = (value, event) => {
  console.log(value);
  event.reply("output", { value });
};

ipcMain.on("createProject", (event, arg) => {
  let output = "creating project at " + arg.path;
  logAndSendToWebConsole(output, event);

  let response;
  if (process.platform == "win32") {
    response = execSync(
      `${config.pgPath} /ofPath"${config.ofPath}" ${arg.path}`
    );
  } else {
    response = execSync(`${config.pgPath} -o"${config.ofPath}" ${arg.path}`);
  }
  logAndSendToWebConsole(response.toString(), event);
});

ipcMain.on("updateProject", (event, arg) => {
  let response;
  if (process.platform == "win32") {
    response = execSync(
      `${config.pgPath} /ofPath"${config.ofPath}" /dryRun ${arg.path}`
    );
  } else {
    response = execSync(`${config.pgPath} -o"${config.ofPath}" -d ${arg.path}`);
  }
  console.log(response.toString());
  event.reply("output", { value: response.toString() });
});

ipcMain.on("updateMultiple", (event, arg) => {
  let output = "updating multiple projects at " + arg.path;
  logAndSendToWebConsole(output, event);
  let response;
  if (process.platform == "win32") {
    response = execSync(
      `${config.pgPath} /ofPath"${config.ofPath}" /r ${arg.path}`
    );
  } else {
    response = execSync(`${config.pgPath} -o"${config.ofPath}" -r ${arg.path}`);
  }

  logAndSendToWebConsole(response.toString(), event);
});
