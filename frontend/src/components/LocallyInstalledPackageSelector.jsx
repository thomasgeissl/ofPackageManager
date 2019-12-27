import { ipcRenderer } from "electron";
import React, { useState } from "react";
import { useSelector, useDispatch } from "react-redux";
import {
  addLocalPackage,
  removeLocalPackage
} from "../state/reducers/localPackages";
import { Button, FormControlLabel, Grid, Switch } from "@material-ui/core";
import TextField from "@material-ui/core/TextField";
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
  position: absolute;
  right: 0;
`;

const PackageInfo = styled.section`
  text-align: left;
`;

const StyledModal = styled(Modal)`
  overflow: scroll;
  display: flex;
  align-items: center;
  justify-content: center;
`;

export default () => {
  const dispatch = useDispatch();
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
  const [count, setCount] = useState({
    name: "",
    website: "",
    license: "",
    cloneUrl: "",
    author: ""
  });
  const [checkout, setCheckout] = useState({ value: "latest" });
  const [searchModalOpen, setSearchModalOpen] = useState(false);

  const handleChange = (event, value) => {
    console.log(value);
    if (value) {
      setCount(value);
    } else {
      setCount({
        name: "",
        website: "",
        license: "",
        cloneUrl: "",
        author: ""
      });
    }
  };
  const handleCheckoutChange = event => {
    setCheckout({ value: event.target.value });
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
        <Grid item>
          <Autocomplete
            id="combo-box-demo"
            options={database}
            getOptionLabel={option => option.name}
            style={{ width: 300 }}
            freeSolo
            onChange={handleChange}
            renderInput={params => (
              <TextField
                {...params}
                label="id, github handle or url"
                variant="outlined"
                fullWidth
              />
            )}
          />
        </Grid>
        <Grid item>
          <TextField
            label="commit hash or tag"
            variant="outlined"
            defaultValue={checkout.value}
            onChange={handleCheckoutChange}
          />
        </Grid>
        <Grid item>
          <Button
            variant="contained"
            onClick={event => {
              ipcRenderer.send("installPackageById", {
                id: count.name,
                checkout,
                cwd: store.getState().config.cwd
              });
            }}
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
      {count.name !== "" && (
        <PackageInfo>
          {/* {count.name} */}
          author: {count.author} <br></br>
          <a href={count.website}>{count.website}</a>
          <br></br>
          license: {count.license}
        </PackageInfo>
      )}
      <PackageList>
        {packages.map(function(value, index) {
          return (
            <li key={index}>
              <FormControlLabel
                control={
                  <Switch
                    color="primary"
                    checked={isPackageSelected(value)}
                    onChange={handleSelectedChange(value)}
                    value={value.path}
                  />
                }
                label={value.path}
              />
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
                          console.log("install");
                          ipcRenderer.send("installPackageByUrl", {
                            ...value,
                            cwd: store.getState().config.cwd
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
