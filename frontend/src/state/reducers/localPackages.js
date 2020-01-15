const initialState = { packages: [], selected: [], database: [] };
const types = {
  SETPACKAGESDATABASE: "SETPACKAGESDATABASE",
  SETAVAILABLELOCALPACKAGES: "SETAVAILABLELOCALPACKAGES",
  ADDLOCALPACKAGE: "ADDLOCALPACKAGE",
  REMOVELOCALPACKAGE: "REMOVELOCALPACKAGE",
  CLEARLOCALPACKAGES: "CLEARLOCALPACKAGES"
};

export default (state = initialState, action) => {
  switch (action.type) {
    case types.SETPACKAGESDATABASE:
      return {
        ...state,
        database: action.payload.value.sort()
      };
    case types.SETAVAILABLELOCALPACKAGES: {
      let newState = { ...state, packages: [...state.packages] };
      action.payload.value.forEach(p => {
        const index = newState.packages.findIndex(x => x.path === p.path);
        if (index === -1) {
          newState.packages.push(p);
        }
      });
      return newState;
    }
    case types.ADDLOCALPACKAGE: {
      let newState = { ...state, selected: [...state.selected] };
      const index = newState.selected.findIndex(
        x => x.path === action.payload.value.path
      );
      if (index === -1) {
        newState.selected.push(action.payload.value);
      }

      return newState;
    }
    case types.REMOVELOCALPACKAGE:
      const index = state.selected.findIndex(
        item => item.path === action.payload.value.path
      );
      if (index > -1) {
        let newState = { ...state, selected: [...state.selected] };
        newState.selected.splice(index, 1);
        return newState;
      }
      break;
    case types.CLEARLOCALPACKAGES:
      return { ...state, packages: [], selected: [] };
    default:
      return state;
  }
};

export const setPackagesDatabase = value => {
  return {
    type: types.SETPACKAGESDATABASE,
    payload: {
      value
    }
  };
};
export const setAvailableLocalPackages = value => {
  return {
    type: types.SETAVAILABLELOCALPACKAGES,
    payload: {
      value
    }
  };
};
export const addLocalPackage = value => {
  return {
    type: types.ADDLOCALPACKAGE,
    payload: {
      value
    }
  };
};

export const removeLocalPackage = value => {
  return {
    type: types.REMOVELOCALPACKAGE,
    payload: {
      value
    }
  };
};
export const clearLocalPackages = () => {
  return {
    type: types.CLEARLOCALPACKAGES
  };
};

export { types };
