import React from "react";

function Taskbar(props) {
  return (
    <div>
      <p className="taskbar">
        <button
          className="taskbarButton run"
          disabled={!props.runState}
          onClick={props.runPress}
        >
          RUN
        </button>
        <button
          className="taskbarButton step"
          disabled={!(props.stepState < props.maxCycles + 1 ? true : false)}
          onClick={props.stepPress}
        >
          STEP
        </button>
        <button
          className="taskbarButton prev"
          disabled={!(props.prevState > 0 ? true : false)}
          onClick={props.prevPress}
        >
          PREV
        </button>
        <button
          className="taskbarButton reset"
          disabled={!props.resetState}
          onClick={props.resetPress}
        >
          RESET
        </button>
      </p>
    </div>
  );
}

export default Taskbar;
