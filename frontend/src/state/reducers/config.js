const initialState = {
  showAdvancedFeatures: false,
  showConsole: false,
  verboseOutput: false,
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
  SETOFPATH: "SETOFPATH",
  SETDEFAULTPROJECTPATH: "SETDEFAULTPROJECTPATH",
  SETPROJECTGENERATORPATH: "SETPROJECTGENERATORPATH",
  SETOFPACKAGEMANAGERPATH: "SETOFPACKAGEMANAGERPATH",
  SETOFPACKAGESPATH: "SETOFPACKAGESPATH"
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
    case types.SETOFPATH:
      return {
        ...state,
        ofPath: action.payload.value
      };
    case types.SETDEFAULTPROJECTPATH:
      return {
        ...state,
        defaultProjectPath: action.payload.value
      };
    case types.SETOFPACKAGEMANAGERPATH:
      return {
        ...state,
        ofPackageManagerPath: action.payload.value
      };
    case types.SETOFPACKAGESPATH:
      return {
        ...state,
        packagesPath: action.payload.value
      };
    case types.SETPROJECTGENERATORPATH:
      return {
        ...state,
        pgPath: action.payload.value
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

export const setOfPath = value => {
  return {
    type: types.SETOFPATH,
    payload: {
      value
    }
  };
};
export const setDefaultProjectPath = value => {
  return {
    type: types.SETDEFAULTPROJECTPATH,
    payload: {
      value
    }
  };
};
export const setOfPackageManagerPath = value => {
  return {
    type: types.SETOFPACKAGEMANAGERPATH,
    payload: {
      value
    }
  };
};
export const setOfPackagesPath = value => {
  return {
    type: types.SETOFPACKAGESPATH,
    payload: {
      value
    }
  };
};

export const setProjectGeneratorPath = value => {
  return {
    type: types.SETPROJECTGENERATORPATH,
    payload: {
      value
    }
  };
};
