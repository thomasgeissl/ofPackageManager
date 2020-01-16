import React from "react";
import Button from "@material-ui/core/Button";
import Icon from "@material-ui/icons/Refresh";

export default props => {
  return (
    <Button {...props}>
      <Icon></Icon>
    </Button>
  );
};
