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
  width: 100%;
`;
const StyledBox = styled(Box)`
  padding-left: 5px;
  padding-right: 5px;
`;

export default () => {
  const config = useSelector(state => state.config);
  const cwd = useSelector(state => state.project.cwd);
  const [query, setQuery] = useState("");
  const [checkout, setCheckout] = useState({ value: "latest" });
  const [searchType, setSearchType] = useState("name");
  const [result, setResult] = useState([]);

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
      <Box display="flex" flexDirection="row">
        <StyledBox item style={{ width: 150 }}>
          <StyledSelect
            value={searchType}
            onChange={handleSearchTypeChange}
            variant="outlined"
            fullWidth
          >
            <MenuItem value={"name"}>name</MenuItem>
            <MenuItem value={"user"}>user</MenuItem>
          </StyledSelect>
        </StyledBox>
        <StyledBox flexGrow={1}>
          <TextField
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
            fullWidth
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
                  <Download container spacing="3">
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
