const initialState = {
  cwd: "",
  name: ""
};

export default (state = initialState, action) => {
  switch (action.type) {
    case "SETCWD":
      return {
        ...state,
        cwd: action.payload.value
      };
    case "SETNAME":
      return {
        ...state,
        name: action.payload.value
      };
    default:
      return state;
  }
};

export const setCwd = value => {
  return {
    type: "SETCWD",
    payload: {
      value
    }
  };
};

export const setName = value => {
  return {
    type: "SETNAME",
    payload: {
      value
    }
  };
};
