import { ipcRenderer } from "electron";
import React, { useState } from "react";
import { useSelector, useDispatch } from "react-redux";
import { Button, Grid, TextField } from "@material-ui/core";
import List from "@material-ui/core/List";
import ListItem from "@material-ui/core/ListItem";
import Input from "@material-ui/core/Input";
import InputLabel from "@material-ui/core/InputLabel";
import InputAdornment from "@material-ui/core/InputAdornment";
import FormControl from "@material-ui/core/FormControl";
import styled from "styled-components";
import store from "../state/store";

import CloseIcon from "@material-ui/icons/Close";
import SearchIcon from "@material-ui/icons/Search";
import GitHubIcon from "@material-ui/icons/GitHub";
import StarIcon from "@material-ui/icons/Star";
import ForkIcon from "@material-ui/icons/Restaurant";
import InstallIcon from "@material-ui/icons/GetApp";

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
  width: 90%;
`;
const StyledButton = styled(Button)`
  width: 10%;
`;

export default () => {
  const [query, setQuery] = useState("");
  const [result, setResult] = useState([]);
  const cwd = useSelector(state => state.config.cwd);
  return (
    <Container>
      <h2>Search</h2>
      <StyledTextField
        id="input-with-icon-textfield"
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
          if (event.charCode == 13) {
            fetch(
              `https://api.github.com/search/repositories?q=${query}&sort=stars&order=desc`
            )
              .then(res => res.json())
              .then(data => {
                console.log(data.items);
                setResult(data.items);
              })
              .catch(console.log);
          }
        }}
        // fullWidth
      />
      <StyledButton
        variant="contained"
        onClick={event => {
          fetch(
            `https://api.github.com/search/repositories?q=${query}&sort=stars&order=desc`
          )
            .then(res => res.json())
            .then(data => {
              console.log(data.items);
              setResult(data.items);
            })
            .catch(console.log);
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
                    <Grid item>{item.updated_at}</Grid>
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
