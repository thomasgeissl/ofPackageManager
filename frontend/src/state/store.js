import { combineReducers, createStore } from "redux";

import config from "./reducers/config";
import console from "./reducers/console";
import corePackages from "./reducers/corePackages";
import globalPackages from "./reducers/globalPackages";
import localPackages from "./reducers/localPackages";

export default createStore(
  combineReducers({
    config,
    console,
    corePackages,
    globalPackages,
    localPackages
  })
);
