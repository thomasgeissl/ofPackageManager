const initialState = {
  //   platforms: [],
  platforms: [
    "linux",
    "linux64",
    "linuxarmv6l",
    "linuxarmv7l",
    "msys2",
    "vs",
    "osx",
    "ios",
    "android"
  ],
  selected: []
};

const types = {
  SETAVAILABLEPLATFORMS: "SETAVAILABLEPLATFORMS",
  SETSELECTEDPLATFORMS: "SETSELECTEDPLATFORMS",
  ADDPLATFORM: "ADDPLATFORM",
  REMOVEPLATFORM: "REMOVEPLATFORM"
};

export default (state = initialState, action) => {
  switch (action.type) {
    case types.SETAVAILABLEPLATFORMS:
      return {
        ...state,
        platforms: action.payload.value
      };
    case types.SETSELECTEDPLATFORMS:
      return {
        ...state,
        selected: action.payload.value
      };
    case types.ADDPLATFORM:
      let newState = { ...state, selected: [...state.selected] };
      newState.selected.push(action.payload.value);
      console.log("adding platform", newState);
      return newState;
    case types.REMOVEPLATFORM:
      var index = state.selected.indexOf(action.payload.value);
      if (index > -1) {
        let newState = { ...state, selected: [...state.selected] };
        newState.selected.splice(index, 1);
        return newState;
      }
      return state;
    default:
      return state;
  }
};

export const setAvailablePlatforms = value => {
  return {
    type: types.SETAVAILABLEPLATFORMS,
    payload: {
      value
    }
  };
};
export const setSelectedPlatforms = value => {
  return {
    type: types.SETSELECTEDPLATFORMS,
    payload: {
      value
    }
  };
};

export const addPlatform = value => {
  return {
    type: types.ADDPLATFORM,
    payload: {
      value
    }
  };
};
export const removePlatform = value => {
  return {
    type: types.REMOVEPLATFORM,
    payload: {
      value
    }
  };
};
