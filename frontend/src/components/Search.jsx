import { ipcRenderer } from "electron";
import React, { useState } from "react";
import { useSelector } from "react-redux";
import { Button, Grid, TextField } from "@material-ui/core";
import Select from "@material-ui/core/Select";
import MenuItem from "@material-ui/core/MenuItem";
import InputAdornment from "@material-ui/core/InputAdornment";
import styled from "styled-components";

import SearchIcon from "@material-ui/icons/Search";
import GitHubIcon from "@material-ui/icons/GitHub";
import StarIcon from "@material-ui/icons/Star";
import ForkIcon from "@material-ui/icons/Restaurant";
import InstallIcon from "@material-ui/icons/GetApp";
import LastUpdatedIcon from "@material-ui/icons/Timer";

import toDate from "date-fns/toDate";
import parseISODate from "date-fns/parseISO";
import formatDate from "date-fns/format";

const Container = styled.div`
  background-color: white;
  position: relative;
  width: 90%;
  height: 90%;
  padding: 15px;
  overflow: auto;
`;
const CloseButton = styled(Button)`
  position: fixed;
  top: 15px;
  right: 15px;
`;

const Results = styled.div`
  background-color: lightgrey;
  ul {
    list-style-type: none;
    li {
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

const StyledTextField = styled(TextField)`
  width: 80%;
`;
const StyledSelect = styled(Select)`
  width: 10%;
`;
const StyledButton = styled(Button)`
  width: 10%;
`;

export default () => {
  const [query, setQuery] = useState("");
  const [searchType, setSearchType] = useState("name");
  const [result, setResult] = useState([]);
  const cwd = useSelector(state => state.config.cwd);

  const handleSearchTypeChange = event => {
    setSearchType(event.target.value);
  };
  const handleSearch = () => {
    if (searchType === "name") {
      fetch(
        `https://api.github.com/search/repositories?q=${query}&sort=stars&order=desc`
      )
        .then(res => res.json())
        .then(data => {
          setResult(data.items);
        })
        .catch(console.log);
    } else if (searchType === "user") {
      fetch(`https://api.github.com/users/${query}/repos?per_page=100`)
        .then(res => res.json())
        .then(data => {
          setResult(data.filter(item => item.name.startsWith("ofx")));
        })
        .catch(console.log);
    }
  };
  return (
    <Container>
      <h2>Search</h2>
      <StyledTextField
        variant="outlined"
        label="name"
        InputProps={{
          startAdornment: (
            <InputAdornment position="start">
              <GitHubIcon />
            </InputAdornment>
          )
        }}
        onChange={event => {
          setQuery(event.target.value);
        }}
        onKeyPress={event => {
          if (event.charCode === 13) {
            handleSearch();
          }
        }}
        // fullWidth
      />
      <StyledSelect
        value={searchType}
        onChange={handleSearchTypeChange}
        variant="outlined"
      >
        <MenuItem value={"name"}>name</MenuItem>
        <MenuItem value={"user"}>user</MenuItem>
      </StyledSelect>
      <StyledButton
        variant="contained"
        onClick={event => {
          handleSearch();
        }}
      >
        <SearchIcon></SearchIcon>
      </StyledButton>
      {/* {result.length} */}
      <Results>
        <ul>
          {result.map(function(item, index) {
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
                      {formatDate(parseISODate(item.updated_at), "yyyy/MM/dd")}
                    </Grid>
                  </Grid>
                </StyledListItemContent>
                <StyledButton
                  variant="contained"
                  onClick={event => {
                    ipcRenderer.send("installPackageByUrl", {
                      url: item.clone_url,
                      checkout: "latest",
                      cwd
                    });
                  }}
                >
                  <InstallIcon></InstallIcon>
                </StyledButton>
              </li>
            );
          })}
        </ul>
      </Results>
    </Container>
  );
};
