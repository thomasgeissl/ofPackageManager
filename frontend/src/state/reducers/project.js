const initialState = {
  cwd: "",
  name: ""
};

const types = {
  SETCWD: "SETCWD",
  SETNAME: "SETNAME",
  CLEARPROJECT: "CLEARPROJECT"
};

export default (state = initialState, action) => {
  switch (action.type) {
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
    case types.CLEARPROJECT:
      return initialState;
    default:
      return state;
  }
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

export const clearProject = () => {
  return {
    type: types.CLEARPROJECT
  };
};
