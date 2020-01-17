import { ipcRenderer } from "electron";
import React, { useState, useReducer } from "react";
import { useSelector } from "react-redux";
import { Button, Grid, TextField } from "@material-ui/core";
import Box from "@material-ui/core/Box";
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
const StyledListItemContent = styled.div``;

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

const initialState = {
  query: "",
  result: [],
  databaseResult: [],
  urlResult: []
};
function reducer(state, action) {
  switch (action.type) {
    case "SETQUERY":
      return { ...state, query: action.payload.value };
    case "SETRESULT":
      return { ...state, result: action.payload.value };
    case "SETDATABASERESULT":
      return { ...state, databaseResult: action.payload.value };
    case "SETURLRESULT":
      return { ...state, urlResult: action.payload.value };
    case "SETDATABASECHECKOUT":
      let { databaseResult } = state;
      databaseResult[action.payload.index].checkout = action.payload.value;
      return { ...state, databaseResult };
    case "SETULRCHECKOUT":
      let { urlResult } = state;
      urlResult[action.payload.index].checkout = action.payload.value;
      return { ...state, urlResult };
    case "SETCHECKOUT":
      let { result } = state;
      result[action.payload.index].checkout = action.payload.value;
      return { ...state, result };
    default:
      return state;
    // throw new Error();
  }
  return state;
}
export default () => {
  const [state, dispatch] = useReducer(reducer, initialState);
  const config = useSelector(state => state.config);
  const cwd = useSelector(state => state.project.cwd);
  const database = useSelector(state => state.localPackages.database);

  const { query, result, databaseResult, urlResult } = state;

  const handleChange = (event, value) => {
    if (value) {
      dispatch({
        type: "SETQUERY",
        payload: { value }
      });
    }
  };
  const handleSearch = () => {
    dispatch({ type: "SETRESULT", payload: { value: [] } });
    dispatch({ type: "SETDATABASERESULT", payload: { value: [] } });
    dispatch({ type: "SETURLRESULT", payload: { value: [] } });

    // database
    let databaseR = [];
    database.forEach(value => {
      if (value.name.toLowerCase().search(query.toLowerCase()) > -1) {
        value.tags = ["latest"];
        value.checkout = "latest";
        databaseR.push(value);
      }
    });
    dispatch({ type: "SETDATABASERESULT", payload: { value: databaseR } });
    // github
    if (query.search("user") === 0) {
      fetch(
        `https://api.github.com/users/${query
          .split(" ")
          .pop()}/repos?per_page=100`
      )
        .then(res => res.json())
        .then(data => {
          data.forEach(item => {
            item.tags = ["latest"];
            item.checkout = "latest";
          });
          return data;
        })
        .then(data => {
          dispatch({
            type: "SETRESULT",
            payload: { value: data.filter(item => item.name.startsWith("ofx")) }
          });
        })
        .catch(console.log);
    } else {
      fetch(
        `https://api.github.com/search/repositories?q=${query}&sort=stars&order=desc`
      )
        .then(res => res.json())
        .then(data => (data ? data.items : []))
        .then(data => {
          data.forEach(item => {
            item.tags = ["latest"];
            item.checkout = "latest";
          });
          return data;
        })
        .then(data => {
          dispatch({ type: "SETRESULT", payload: { value: data } });
        })
        .catch(console.log);
    }

    if (isGitUrl(query)) {
      dispatch({
        type: "SETURLRESULT",
        payload: {
          checkout: "latest",
          value: [{ value: query, checkout: "latest" }]
        }
      });
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
                  dispatch({
                    type: "SETQUERY",
                    payload: { value: event.target.value }
                  });
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
          {/* database results */}
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
                  <Grid item xs={4}>
                    <Autocomplete
                      freeSolo
                      options={item.tags}
                      value={item.checkout}
                      onChange={(event, value) => {
                        dispatch({
                          type: "SETDATABASECHECKOUT",
                          payload: { index, value }
                        });
                      }}
                      renderInput={params => (
                        <TextField
                          {...params}
                          label="tag or commit hash"
                          margin="none"
                          variant="outlined"
                          onChange={event => {
                            dispatch({
                              type: "SETDATABASECHECKOUT",
                              payload: { index, value: event.target.value }
                            });
                          }}
                          fullWidth
                        />
                      )}
                    />
                  </Grid>
                  <Grid item>
                    <StyledButton
                      variant="contained"
                      onClick={event => {
                        console.log("install package by url", item);
                        ipcRenderer.send("installPackageByUrl", {
                          config,
                          url: item.cloneUrl,
                          checkout: item.checkout,
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
                    <Grid item xs={4}>
                      <Autocomplete
                        freeSolo
                        options={item.tags}
                        value={item.checkout}
                        onChange={(event, value) => {
                          dispatch({
                            type: "SETURLCHECKOUT",
                            payload: { index, value }
                          });
                        }}
                        renderInput={params => (
                          <TextField
                            {...params}
                            label="tag or commit hash"
                            margin="none"
                            variant="outlined"
                            onChange={event => {
                              dispatch({
                                type: "SETURLCHECKOUT",
                                payload: { index, value: event.target.value }
                              });
                            }}
                            fullWidth
                          />
                        )}
                      />
                    </Grid>
                    <Grid item>
                      <StyledButton
                        variant="contained"
                        onClick={event => {
                          ipcRenderer.send("installPackageByUrl", {
                            config,
                            url: item.value,
                            checkout: item.checkout,
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
                    <Grid item xs={4}>
                      <Autocomplete
                        freeSolo
                        options={item.tags}
                        value={item.checkout}
                        onChange={(event, value) => {
                          dispatch({
                            type: "SETCHECKOUT",
                            payload: { index, value }
                          });
                        }}
                        renderInput={params => (
                          <TextField
                            {...params}
                            label="tag or commit hash"
                            margin="none"
                            variant="outlined"
                            onChange={event => {
                              dispatch({
                                type: "SETCHECKOUT",
                                payload: { index, value: event.target.value }
                              });
                            }}
                            fullWidth
                          />
                        )}
                      />
                    </Grid>
                    <Grid item>
                      <StyledButton
                        variant="contained"
                        onClick={event => {
                          ipcRenderer.send("installPackageByUrl", {
                            config,
                            url: item.clone_url,
                            checkout: item.checkout,
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
