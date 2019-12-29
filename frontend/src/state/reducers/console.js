const initialState = {
  output: ""
};

const types = {
  APPENDOUTPUT: "APPENDOUTPUT"
};

export default (state = initialState, action) => {
  switch (action.type) {
    case types.APPENDOUTPUT:
      const newState = {
        ...state,
        output: state.output + action.payload.value + "\n"
      };
      return newState;
    default:
      return state;
  }
};

export const appendOutput = value => {
  return {
    type: types.APPENDOUTPUT,
    payload: {
      value
    }
  };
};
