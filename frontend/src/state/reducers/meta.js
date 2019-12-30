const initialState = {
  ofPackageManagerVersion: {
    major: -1,
    minor: -1,
    patch: -1
  }
};

const types = {
  SETOFPACKAGEMANAGERVERSION: "SETOFPACKAGEMANAGERVERSION"
};

export default (state = initialState, action) => {
  switch (action.type) {
    case types.SETOFPACKAGEMANAGERVERSION:
      return {
        ...state,
        ofPackageManagerVersion: action.payload.value
      };
    default:
      return state;
  }
};

export const setOfPackageManagerVersion = value => {
  return {
    type: types.SETOFPACKAGEMANAGERVERSION,
    payload: {
      value
    }
  };
};
