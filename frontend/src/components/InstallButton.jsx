import React from "react";
import { Button } from "@material-ui/core";
import InstallIcon from "@material-ui/icons/GetApp";

export default props => {
  return (
    <Button {...props}>
      <InstallIcon></InstallIcon>
    </Button>
  );
};
