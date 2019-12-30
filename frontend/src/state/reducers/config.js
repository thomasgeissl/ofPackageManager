const initialState = {
  cwd: "",
  name: "",
  showAdvancedFeatures: false,
  showConsole: false,
  verboseOutput: false,
  ofPackageManagerVersion: { major: -1, minor: -1, patch: -1 },
  localAddonsPath: "",
  ofPath: "",
  defaultProjectPath: "",
  packagesPath: "",
  ofPackageManagerPath: "",
  pgPath: ""
};

const types = {
  SETCONFIG: "SETCONFIG",
  SETSHOWADVANCEDFEATURES: "SETSHOWADVANCEDFEATURES",
  SETSHOWCONSOLE: "SETSHOWCONSOLE",
  SETVERBOSEOUTPUT: "SETVERBOSEOUTPUT",
  SETPACKAGEMANAGERVERSION: "SETPACKAGEMANAGERVERSION",
  SETCWD: "SETCWD",
  SETNAME: "SETNAME"
};

export default (state = initialState, action) => {
  switch (action.type) {
    case types.SETCONFIG:
      const newState = {
        ...state,
        ...action.payload.value
      };
      // console.log("payload.value", action.payload.value);
      // console.log("new state", newState);
      return newState;
    case types.SETSHOWADVANCEDFEATURES:
      return {
        ...state,
        showAdvancedFeatures: action.payload.value
      };
    case types.SETSHOWCONSOLE:
      return {
        ...state,
        showConsole: action.payload.value
      };
    case types.SETVERBOSEOUTPUT:
      return {
        ...state,
        verboseOutput: action.payload.value
      };
    case types.SETPACKAGEMANAGERVERSION:
      return {
        ...state,
        ofPackageManagerVersion: action.payload.value
      };
    case types.SETCWD:
      return {
        ...state,
        cwd: action.payload.value
      };
    case types.SETNAME:
      return {
        ...state,
        name: action.payload.value
      };
    default:
      return state;
  }
};

export const setConfig = value => {
  return {
    type: types.SETCONFIG,
    payload: {
      value
    }
  };
};
export const setShowAdvancedFeatures = value => {
  return {
    type: types.SETSHOWADVANCEDFEATURES,
    payload: {
      value
    }
  };
};
export const setShowConsole = value => {
  return {
    type: types.SETSHOWCONSOLE,
    payload: {
      value
    }
  };
};
export const setVerboseOutput = value => {
  return {
    type: types.SETVERBOSEOUTPUT,
    payload: {
      value
    }
  };
};
export const setPackageManagerVersion = value => {
  return {
    type: types.SETPACKAGEMANAGERVERSION,
    payload: {
      value
    }
  };
};
export const setCwd = value => {
  return {
    type: types.SETCWD,
    payload: {
      value
    }
  };
};

export const setName = value => {
  return {
    type: types.SETNAME,
    payload: {
      value
    }
  };
};
