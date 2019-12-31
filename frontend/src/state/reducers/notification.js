const initialState = {
  message: "",
  active: false
};

const types = {
  SHOWNOTIFICATION: "SHOWNOTIFICATION",
  HIDENOTIFICATION: "HIDENOTIFICATION"
};

export default (state = initialState, action) => {
  switch (action.type) {
    case types.SHOWNOTIFICATION:
      return {
        ...state,
        message: action.payload.value,
        show: true
      };
    case types.HIDENOTIFICATION:
      return {
        ...state,
        show: false
      };
    default:
      return state;
  }
};

export const showNotification = value => {
  return {
    type: types.SHOWNOTIFICATION,
    payload: {
      value
    }
  };
};
export const hideNotification = () => {
  return {
    type: types.HIDENOTIFICATION
  };
};
