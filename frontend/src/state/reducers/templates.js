const initialState = {
  templates: [],
  selected: ""
};

const types = {
  SETAVAILABLETEMPLATES: "SETAVAILABLETEMPLATES",
  SETSELECTEDTEMPLATES: "SETSELECTEDTEMPLATES",
  ADDTEMPLATE: "ADDTEMPLATE",
  REMOVETEMPLATE: "REMOVETEMPLATE"
};

export default (state = initialState, action) => {
  switch (action.type) {
    case types.SETAVAILABLETEMPLATES:
      return {
        ...state,
        templates: action.payload.value
      };
    case types.SETSELECTEDTEMPLATES:
      return {
        ...state,
        selected: action.payload.value
      };
    // case types.ADDTEMPLATE:
    //   let newState = { ...state, selected: [...state.selected] };
    //   //   newState.selected.push(action.payload.value);
    //   newState.selected = action.payload.value;
    //   return newState;
    // case types.REMOVETEMPLATE:
    //   var index = state.selected.indexOf(action.payload.value);
    //   if (index > -1) {
    //     let newState = { ...state, selected: [...state.selected] };
    //     newState.selected.splice(index, 1);
    //     return newState;
    //   }
    //   return state;
    default:
      return state;
  }
};

export const setAvailableTemplates = value => {
  return {
    type: types.SETAVAILABLETEMPLATES,
    payload: {
      value
    }
  };
};
export const setSelectedTemplate = value => {
  return {
    type: types.SETSELECTEDTEMPLATES,
    payload: {
      value
    }
  };
};

export const addTemplate = value => {
  return {
    type: types.ADDTEMPLATE,
    payload: {
      value
    }
  };
};
export const removeTemplate = value => {
  return {
    type: types.REMOVETEMPLATE,
    payload: {
      value
    }
  };
};
