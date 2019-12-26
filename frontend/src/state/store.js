import { combineReducers, createStore } from "redux";

import config from "./reducers/config";
import corePackages from "./reducers/corePackages";
import globalPackages from "./reducers/globalPackages";
import localPackages from "./reducers/localPackages";

export default createStore(
  combineReducers({
    config,
    corePackages,
    globalPackages,
    localPackages
  })
);
