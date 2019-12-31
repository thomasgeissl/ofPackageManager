import { ipcRenderer } from "electron";
import React, { useState } from "react";
import { useSelector, useDispatch } from "react-redux";
import {
  addLocalPackage,
  removeLocalPackage
} from "../state/reducers/localPackages";
import { Button, FormControlLabel, Grid } from "@material-ui/core";
import Checkbox from "@material-ui/core/Checkbox";
import Select from "@material-ui/core/Select";
import MenuItem from "@material-ui/core/MenuItem";
import TextField from "@material-ui/core/TextField";
import Tooltip from "@material-ui/core/Tooltip";
import Autocomplete from "@material-ui/lab/Autocomplete";

import styled from "styled-components";

import SearchIcon from "@material-ui/icons/Search";
// import { isAbsolute } from "path";
import Modal from "@material-ui/core/Modal";
import Search from "./Search";
import store from "../state/store";

import InstallButton from "./InstallButton";

const PackageList = styled.ul`
  text-align: left;
  columns: 5;
  list-style-type: none;
`;

const Headline = styled.h3`
  text-align: left;
`;

const SearchButton = styled(Grid)`
  flex-grow: 1;
  text-align: right;
`;

const PackageInfo = styled.section`
  text-align: left;
`;

const StyledSelect = styled(Select)``;

const StyledModal = styled(Modal)`
  overflow: scroll;
  display: flex;
  align-items: center;
  justify-content: center;
`;

export default () => {
  const dispatch = useDispatch();
  const config = useSelector(state => state.config);
  const cwd = useSelector(state => state.project.cwd);
  const packages = useSelector(state => state.localPackages.packages);
  const database = useSelector(state => state.localPackages.database);
  const selectedPackages = useSelector(state => state.localPackages.selected);
  const isPackageInstalled = value => {
    const index = packages.findIndex(item => item.path === value.path);
    return index > -1;
  };
  const isPackageSelected = value => {
    const index = selectedPackages.findIndex(item => item.path === value.path);
    return index > -1;
  };

  const hasMissingPackages = () => {
    let value = false;
    selectedPackages.map(p => {
      if (!isPackageInstalled(p)) {
        value = true;
      }
    });
    return value;
  };
  const [searchType, setSearchType] = useState("id");
  const [packageToInstall, setPackageToInstall] = useState({
    name: "",
    website: "",
    license: "",
    cloneUrl: "",
    author: ""
  });
  const [githubToInstall, setGithubToInstall] = useState("");
  const [urlToInstall, setUrlToInstall] = useState("");
  const [checkout, setCheckout] = useState({ value: "latest" });
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
  const handleSelectedChange = p => {
    return (event, value) => {
      if (value) {
        dispatch(addLocalPackage(p));
      } else {
        dispatch(removeLocalPackage(p));
      }
    };
  };

  return (
    <>
      <Headline>install</Headline>
      <Grid
        alignItems="flex-start"
        justify="flex-start"
        direction="row"
        // justify="space-between" // Add it here :)
        container
        spacing={2}
      >
        <Grid item style={{ width: 150 }}>
          <StyledSelect
            value={searchType}
            onChange={event => {
              setSearchType(event.target.value);
            }}
            variant="outlined"
            fullWidth
          >
            <MenuItem value={"id"} size="small">
              id
            </MenuItem>
            <MenuItem value={"github"} size="small">
              github handle
            </MenuItem>
            <MenuItem value={"url"} size="small">
              url
            </MenuItem>
          </StyledSelect>
        </Grid>
        {searchType === "id" && (
          <Grid item>
            <Autocomplete
              options={database}
              getOptionLabel={option => option.name}
              style={{ width: 300 }}
              // freeSolo
              onChange={handleChange}
              renderInput={params => (
                <TextField
                  {...params}
                  label="id"
                  variant="outlined"
                  fullWidth
                />
              )}
            />
          </Grid>
        )}

        {searchType === "github" && (
          <Grid item style={{ width: 600 }}>
            <TextField
              label="github handle"
              variant="outlined"
              onChange={event => {
                setGithubToInstall(event.target.value);
              }}
              fullWidth
            />
          </Grid>
        )}
        {searchType === "url" && (
          <Grid item style={{ width: 600 }}>
            <TextField
              label="url"
              variant="outlined"
              onChange={event => {
                setUrlToInstall(event.target.value);
              }}
              fullWidth
            />
          </Grid>
        )}

        <Grid item>
          <TextField
            label="commit hash or tag"
            variant="outlined"
            defaultValue={checkout.value}
            onChange={event => {
              setCheckout({ value: event.target.value });
            }}
          />
        </Grid>
        <Grid item>
          <Button
            variant="contained"
            onClick={event => {
              if (searchType === "id") {
                ipcRenderer.send("installPackageById", {
                  config,
                  id: packageToInstall.name,
                  checkout,
                  cwd
                });
              }
              if (searchType === "github") {
                ipcRenderer.send("installPackageByGithub", {
                  config,
                  github: githubToInstall,
                  checkout,
                  cwd
                });
              }
              if (searchType === "url") {
                ipcRenderer.send("installPackageByUrl", {
                  config,
                  url: urlToInstall,
                  checkout,
                  cwd
                });
              }
            }}
            size="small"
          >
            clone and checkout
          </Button>
        </Grid>
        <SearchButton item>
          <Button
            variant="contained"
            onClick={event => {
              setSearchModalOpen(true);
            }}
          >
            <SearchIcon></SearchIcon>
          </Button>
        </SearchButton>
      </Grid>
      {packageToInstall.name !== "" && (
        <PackageInfo>
          {/* {count.name} */}
          author: {packageToInstall.author} <br></br>
          <a href={packageToInstall.website}>{packageToInstall.website}</a>
          <br></br>
          license: {packageToInstall.license}
        </PackageInfo>
      )}
      <PackageList>
        {packages.map(function(value, index) {
          return (
            <li key={index}>
              <Tooltip title={`${value.url}@${value.checkout}`}>
                <FormControlLabel
                  control={
                    <Checkbox
                      color="primary"
                      checked={isPackageSelected(value)}
                      onChange={handleSelectedChange(value)}
                      value={value.path}
                      size="small"
                    />
                  }
                  label={value.path}
                />
              </Tooltip>
            </li>
          );
        })}
      </PackageList>
      {hasMissingPackages() && (
        <>
          <Headline>missing packages</Headline>
          <PackageList>
            {selectedPackages.map(function(value, index) {
              return (
                !isPackageInstalled(value) && (
                  <li key={index}>
                    {value.path}
                    {value.url && (
                      <InstallButton
                        onClick={event => {
                          ipcRenderer.send("installPackageByUrl", {
                            config,
                            ...value,
                            cwd
                          });
                        }}
                      ></InstallButton>
                    )}
                  </li>
                )
              );
            })}
          </PackageList>
        </>
      )}
      <StyledModal
        aria-labelledby="simple-modal-title"
        aria-describedby="simple-modal-description"
        open={searchModalOpen}
        onClose={event => {
          setSearchModalOpen(false);
        }}
      >
        <Search></Search>
      </StyledModal>
    </>
  );
};
