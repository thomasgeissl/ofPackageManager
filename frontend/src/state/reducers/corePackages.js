const initialState = { coreAddons: [], selected: [] };
const types = {
  SETAVAILABLEGLOBALPACKAGES: "SETAVAILABLECOREPACKAGES",
  ADDCOREPACKAGE: "ADDCOREPACKAGE",
  REMOVECOREPACKAGE: "REMOVECOREPACKAGE",
  CLEARCOREPACKAGES: "CLEARCOREPACKAGES"
};

export default (state = initialState, action) => {
  switch (action.type) {
    case "SETAVAILABLECOREADDONS":
      return {
        ...state,
        coreAddons: action.payload.value.sort()
      };
    case "ADDCOREADDON":
      console.log("add core addon", action.payload);
      let newState = { ...state, selected: [...state.selected] };
      newState.selected.push(action.payload.value);
      return newState;
    case "REMOVECOREADDON":
      var index = state.selected.indexOf(action.payload.value);
      if (index > -1) {
        let newState = { ...state, selected: [...state.selected] };
        newState.selected.splice(index, 1);
        return newState;
      }
      break;
    case types.CLEARCOREPACKAGES:
      return {
        ...state,
        selected: []
      };
    default:
      return state;
  }
};

export const setAvailableCoreAddons = value => {
  return {
    type: "SETAVAILABLECOREADDONS",
    payload: {
      value
    }
  };
};
export const addCoreAddon = value => {
  return {
    type: "ADDCOREADDON",
    payload: {
      value
    }
  };
};

export const removeCoreAddon = value => {
  return {
    type: "REMOVECOREADDON",
    payload: {
      value
    }
  };
};
export const clearCorePackages = () => {
  return {
    type: types.CLEARCOREPACKAGES
  };
};
