import React, { useState } from "react";
import { useSelector, useDispatch } from "react-redux";
import Input from "@material-ui/core/Input";
import InputLabel from "@material-ui/core/InputLabel";
import Select from "@material-ui/core/Select";
import MenuItem from "@material-ui/core/MenuItem";
import Chip from "@material-ui/core/Chip";
import FormControl from "@material-ui/core/FormControl";

import {
  setSelectedPlatforms,
  addPlatform,
  removePlatform
} from "../state/reducers/platforms";

const ITEM_HEIGHT = 48;
const ITEM_PADDING_TOP = 8;
const MenuProps = {
  PaperProps: {
    style: {
      maxHeight: ITEM_HEIGHT * 4.5 + ITEM_PADDING_TOP,
      width: 250
    }
  }
};

export default () => {
  const dispatch = useDispatch();
  const platforms = useSelector(state => state.platforms.platforms);
  const selected = useSelector(state => state.platforms.selected);
  const handleChange = event => {
    dispatch(setSelectedPlatforms(event.target.value));
  };
  return (
    <FormControl>
      <InputLabel>platforms</InputLabel>
      <Select
        multiple
        value={selected}
        onChange={handleChange}
        input={<Input />}
        renderValue={selected => (
          <div>
            {selected.map(value => (
              <Chip key={value} label={value} />
            ))}
          </div>
        )}
        MenuProps={MenuProps}
      >
        {platforms.map(name => (
          <MenuItem key={name} value={name}>
            {name}
          </MenuItem>
        ))}
      </Select>
    </FormControl>
  );
};
