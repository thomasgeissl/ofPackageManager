exports.logAndSendToWebConsole = (value, event) => {
  console.log(value);
  event.reply("output", { value });
};
