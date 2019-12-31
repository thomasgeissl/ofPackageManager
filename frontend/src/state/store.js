import { combineReducers, createStore } from "redux";

import meta from "./reducers/meta";
import config from "./reducers/config";
import project from "./reducers/project";
import console from "./reducers/console";
import notification from "./reducers/notification";
import corePackages from "./reducers/corePackages";
import globalPackages from "./reducers/globalPackages";
import localPackages from "./reducers/localPackages";

export default createStore(
  combineReducers({
    meta,
    config,
    project,
    console,
    notification,
    corePackages,
    globalPackages,
    localPackages
  })
);
