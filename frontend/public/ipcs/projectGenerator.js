const { ipcMain } = require("electron");
const { execSync } = require("child_process");
const config = require("../assets/config.json");

ipcMain.on("createProject", (event, arg) => {
  let response;
  if (process.platform == "win32") {
    response = execSync(
      `${config.pgPath} /ofPath"${config.ofPath}" ${arg.path}`
    );
  } else {
    response = execSync(`${config.pgPath} -o"${config.ofPath}" ${arg.path}`);
  }
  console.log(response.toString());
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
});
