import { ipcRenderer } from "electron";
import React, { useState } from "react";
import { useSelector, useDispatch } from "react-redux";
import {
  addLocalPackage,
  removeLocalPackage
} from "../state/reducers/localPackages";
import { Button, FormControlLabel, Grid } from "@material-ui/core";
import Box from "@material-ui/core/Box";
import Checkbox from "@material-ui/core/Checkbox";
import Select from "@material-ui/core/Select";
import MenuItem from "@material-ui/core/MenuItem";
import TextField from "@material-ui/core/TextField";
import Tooltip from "@material-ui/core/Tooltip";
import { flexbox } from "@material-ui/system";

import Autocomplete from "@material-ui/lab/Autocomplete";

import styled from "styled-components";

// import { isAbsolute } from "path";
import Modal from "@material-ui/core/Modal";
import Search from "./Search";

import InstallButton from "./buttons/Install";
const StyledInstallButton = styled(InstallButton)`
  height: 100%;
`;

const Headline = styled.h2``;

const PackageInfo = styled(Box)`
  margin-top: 15px;
  margin-left: 15px;
`;

const StyledSelect = styled(Select)``;
const StyledBox = styled(Box)`
  padding-left: 5px;
  padding-right: 5px;
`;

const StyledModal = styled(Modal)`
  overflow: scroll;
  display: flex;
  align-items: center;
  justify-content: center;
`;

const Container = styled.div`
  background-color: white;
  position: relative;
  width: 90%;
  height: 90%;
  padding: 15px;
  overflow: auto;
`;

export default () => {
  const dispatch = useDispatch();
  const config = useSelector(state => state.config);
  const cwd = useSelector(state => state.project.cwd);
  const packages = useSelector(state => state.localPackages.packages);
  const database = useSelector(state => state.localPackages.database);

  const [searchType, setSearchType] = useState("database");
  const [packageToInstall, setPackageToInstall] = useState({
    name: "",
    website: "",
    license: "",
    cloneUrl: "",
    author: ""
  });
  const [githubToInstall, setGithubToInstall] = useState("");
  const [urlToInstall, setUrlToInstall] = useState("");
  const [idCheckout, idSetCheckout] = useState({ value: "latest" });

  const [searchModalOpen, setSearchModalOpen] = useState(false);

  const handleChange = (event, value) => {
    if (value) {
      setPackageToInstall(value);
    } else {
      setPackageToInstall({
        name: "",
        website: "",
        license: "",
        cloneUrl: "",
        author: ""
      });
    }
  };

  return (
    <>
      <Box display="flex" justifyContent="flex-end">
        {/* <Grid container alignItems="flex-start" justify="flex-end"> */}
        {/* <Grid item> */}
        <Box>
          <Button
            variant="contained"
            onClick={event => setSearchModalOpen(true)}
          >
            install additional addons
          </Button>
        </Box>
        {/* </Grid> */}
        {/* </Grid> */}
      </Box>

      <StyledModal
        open={searchModalOpen}
        onClose={event => {
          setSearchModalOpen(false);
        }}
      >
        <Container>
          <Search></Search>
        </Container>
      </StyledModal>
    </>
  );
};
