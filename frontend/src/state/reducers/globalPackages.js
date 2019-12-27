const initialState = { globalPackages: [], selected: [] };

export default (state = initialState, action) => {
  switch (action.type) {
    case "SETAVAILABLEGLOBALPACKAGES":
      return {
        ...state,
        globalPackages: action.payload.value.sort()
      };
    case "ADDGLOBALPACKAGE":
      let newState = { ...state, selected: [...state.selected] };
      newState.selected.push(action.payload.value);
      return newState;
    case "REMOVEGLOBALPACKAGE":
      const index = state.selected.findIndex(
        item => item.path === action.payload.value.path
      );
      if (index > -1) {
        let newState = { ...state, selected: [...state.selected] };
        newState.selected.splice(index, 1);
        return newState;
      }
    default:
      return state;
  }
};

export const setAvailableGlobalPackages = value => {
  return {
    type: "SETAVAILABLEGLOBALPACKAGES",
    payload: {
      value
    }
  };
};
export const addGlobalPackage = value => {
  return {
    type: "ADDGLOBALPACKAGE",
    payload: {
      value
    }
  };
};

export const removeGlobalPackage = value => {
  return {
    type: "REMOVEGLOBALPACKAGE",
    payload: {
      value
    }
  };
};
