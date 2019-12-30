const { logAndSendToWebConsole } = require("./utils.js");
const { ipcMain } = require("electron");
const { execSync } = require("child_process");

ipcMain.on("createProject", (event, arg) => {
  let output = "creating project at " + arg.path;
  logAndSendToWebConsole(output, event);

  const { config } = arg;

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
  let output = "updating project at " + arg.path;
  logAndSendToWebConsole(output, event);

  const { config } = arg;
  let response;

  if (process.platform == "win32") {
    const command = `${config.pgPath} /ofPath"${config.ofPath}" /dryRun ${arg.path}`;
    logAndSendToWebConsole(command, event);
    response = execSync(command);
  } else {
    const command = `${config.pgPath} -o"${config.ofPath}" -a"${arg.packagesList}" ${arg.path}`;
    logAndSendToWebConsole(command, event);
    response = execSync(command);
  }
  console.log(response.toString());
  event.reply("output", { value: response.toString() });
});

ipcMain.on("updateMultiple", (event, arg) => {
  let output = "updating multiple projects at " + arg.path;
  logAndSendToWebConsole(output, event);

  const { config } = arg;
  let response;

  if (process.platform == "win32") {
    response = execSync(
      `${config.pgPath} /ofPath"${config.ofPath}" /r ${
        arg.verboseOutput ? " /verbose " : " "
      } ${arg.path}`
    );
  } else {
    response = execSync(
      `${config.pgPath} -o"${config.ofPath}" -r ${
        arg.verboseOutput ? " -v " : " "
      } ${arg.path}`
    );
  }

  logAndSendToWebConsole(response.toString(), event);
});
