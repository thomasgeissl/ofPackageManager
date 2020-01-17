import { ipcRenderer } from "electron";
import React, { useState } from "react";
import { useSelector } from "react-redux";
import { Button, Grid, TextField } from "@material-ui/core";
import Box from "@material-ui/core/Box";
import Select from "@material-ui/core/Select";
import MenuItem from "@material-ui/core/MenuItem";
import InputAdornment from "@material-ui/core/InputAdornment";
import styled from "styled-components";

import GitHubIcon from "@material-ui/icons/GitHub";
import StarIcon from "@material-ui/icons/Star";
import ForkIcon from "@material-ui/icons/Restaurant";
import InstallIcon from "@material-ui/icons/GetApp";
import LastUpdatedIcon from "@material-ui/icons/Timer";
import Autocomplete from "@material-ui/lab/Autocomplete";

import SearchButton from "./buttons/Search";

import parseISODate from "date-fns/parseISO";
import formatDate from "date-fns/format";

const Container = styled.div`
  background-color: white;
  position: relative;
  overflow: auto;
`;

const Results = styled.div`
  background-color: lightgrey;
  ul {
    padding-left: 0;
    list-style-type: none;
    li {
      padding-left: 5px;
      padding-top: 5px;
      padding-bottom: 5px;
    }
    li:nth-child(even) {
      background-color: #c3c3c3;
    }
  }
`;
const StyledListItemContent = styled.div`
  width: 90%;
`;

const StyledSelect = styled(Select)`
  width: 10%;
`;
const StyledButton = styled(Button)`
  width: 10%;
`;
const StyledSearchButton = styled(SearchButton)`
  height: 100%;
`;

const Download = styled(Grid)`
  margin-top: 15px;
  padding-top: 5px;
  width: 100%;
`;
const StyledBox = styled(Box)`
  padding-left: 5px;
  padding-right: 5px;
`;

const DatabaseResultItem = styled.li`
  /* border-left: 5px solid rgba(231, 27, 116, 1); */
`;

const isGitUrl = value => {
  const suffix = ".git";
  return value.indexOf(suffix, value.length - suffix.length) !== -1;
};

export default () => {
  const config = useSelector(state => state.config);
  const cwd = useSelector(state => state.project.cwd);
  const database = useSelector(state => state.localPackages.database);
  const [query, setQuery] = useState("");
  const [checkout, setCheckout] = useState({ value: "latest" });
  const [result, setResult] = useState([]);
  const [databaseResult, setDatabaseResult] = useState([]);
  const [urlResult, setUrlResult] = useState([]);

  const handleChange = (event, value) => {
    if (value) {
      console.log(value);
      setQuery(value);
    }
    // setPackageToInstall({
    //   name: "",
    //   website: "",
    //   license: "",
    //   cloneUrl: "",
    //   author: ""
    // });
  };
  const handleSearch = () => {
    setResult([]);
    setDatabaseResult([]);
    setUrlResult([]);
    // database
    let databaseR = [];
    database.forEach(value => {
      if (value.name.toLowerCase().search(query.toLowerCase()) > -1) {
        databaseR.push(value);
      }
    });
    setDatabaseResult(databaseR);
    // github
    if (query.search("user") === 0) {
      fetch(
        `https://api.github.com/users/${query
          .split(" ")
          .pop()}/repos?per_page=100`
      )
        .then(res => res.json())
        .then(data => {
          setResult(data.filter(item => item.name.startsWith("ofx")));
        })
        .catch(console.log);
    } else {
      fetch(
        `https://api.github.com/search/repositories?q=${query}&sort=stars&order=desc`
      )
        .then(res => res.json())
        .then(data => {
          setResult(data.items);
        })
        .catch(console.log);
    }

    // not needed anymore, github api supports that already with the normal repo search
    //github handle
    // if (query.split("/").length === 2) {
    //   setUrlResult([`https://github.com/${query}.git`]);
    // }
    //git clone url
    if (isGitUrl(query)) {
      setUrlResult([query]);
    }
  };
  return (
    <Container>
      <Box display="flex" flexDirection="row">
        <StyledBox flexGrow={1}>
          <Autocomplete
            freeSolo
            options={database.map(option => option.name)}
            onChange={handleChange}
            renderInput={params => (
              <TextField
                {...params}
                label="name, user (e.g. user ofZach) or git url"
                margin="normal"
                variant="outlined"
                onChange={event => {
                  setQuery(event.target.value);
                }}
                onKeyPress={event => {
                  if (event.charCode === 13) {
                    handleSearch();
                  }
                }}
                fullWidth
              />
            )}
          />
        </StyledBox>

        <StyledBox>
          <StyledSearchButton
            variant="contained"
            onClick={event => {
              handleSearch();
            }}
          ></StyledSearchButton>
        </StyledBox>
      </Box>
      {/* {result.length} */}
      <Results>
        <ul>
          {databaseResult.map(function(item, index) {
            return (
              <DatabaseResultItem key={index}>
                <StyledListItemContent>
                  <div>
                    <a href={item.website} target="_bank">
                      {item.name}
                    </a>{" "}
                    by {item.author}
                  </div>
                  <div>{item.description}</div>
                  <div>{item.license}</div>
                </StyledListItemContent>
                <Download container spacing={3}>
                  <Grid item>
                    <TextField
                      size="small"
                      label="commit hash or tag"
                      variant="outlined"
                      defaultValue={checkout.value}
                      onChange={event => {
                        setCheckout({ value: event.target.value });
                      }}
                    />
                  </Grid>
                  <Grid item>
                    <StyledButton
                      variant="contained"
                      onClick={event => {
                        ipcRenderer.send("installPackageByUrl", {
                          config,
                          url: item.cloneUrl,
                          checkout,
                          cwd
                        });
                      }}
                    >
                      <InstallIcon></InstallIcon>
                    </StyledButton>
                  </Grid>
                </Download>
              </DatabaseResultItem>
            );
          })}
          {Array.isArray(result) &&
            urlResult.map(function(item, index) {
              return (
                <li key={index}>
                  <StyledListItemContent>{item}</StyledListItemContent>
                  <Download container spacing={3}>
                    <Grid item>
                      <TextField
                        size="small"
                        label="commit hash or tag"
                        variant="outlined"
                        defaultValue={checkout.value}
                        onChange={event => {
                          setCheckout({ value: event.target.value });
                        }}
                      />
                    </Grid>
                    <Grid item>
                      <StyledButton
                        variant="contained"
                        onClick={event => {
                          ipcRenderer.send("installPackageByUrl", {
                            config,
                            url: item,
                            checkout,
                            cwd
                          });
                        }}
                      >
                        <InstallIcon></InstallIcon>
                      </StyledButton>
                    </Grid>
                  </Download>
                </li>
              );
            })}
          {Array.isArray(result) &&
            result.map(function(item, index) {
              return (
                <li key={index}>
                  <StyledListItemContent>
                    <a href={item.owner.html_url} target="_blank">
                      {item.owner.login}
                    </a>{" "}
                    /{" "}
                    <a href={item.html_url} target="_blank">
                      {item.name}
                    </a>
                    <div>{item.description}</div>
                    <Grid container spacing={3}>
                      <Grid item>
                        <StarIcon></StarIcon> {item.stargazers_count}
                      </Grid>
                      <Grid item>
                        <a href={item.html_url + "/network"} target="_blank">
                          <ForkIcon></ForkIcon> {item.forks_count}
                        </a>
                      </Grid>
                      <Grid item>
                        <LastUpdatedIcon></LastUpdatedIcon>
                        {formatDate(
                          parseISODate(item.updated_at),
                          "yyyy/MM/dd"
                        )}
                      </Grid>
                    </Grid>
                  </StyledListItemContent>
                  <Download container spacing={3}>
                    <Grid item>
                      <TextField
                        size="small"
                        label="commit hash or tag"
                        variant="outlined"
                        defaultValue={checkout.value}
                        onChange={event => {
                          setCheckout({ value: event.target.value });
                        }}
                      />
                    </Grid>
                    <Grid item>
                      <StyledButton
                        variant="contained"
                        onClick={event => {
                          ipcRenderer.send("installPackageByUrl", {
                            config,
                            url: item.clone_url,
                            checkout,
                            cwd
                          });
                        }}
                      >
                        <InstallIcon></InstallIcon>
                      </StyledButton>
                    </Grid>
                  </Download>
                </li>
              );
            })}
        </ul>
      </Results>
    </Container>
  );
};
