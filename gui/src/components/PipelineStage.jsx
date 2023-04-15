import React from "react";

function Stage(props) {
  //props.stagename will tell name of stage [header of this component].
  //props.cyclenumber will tell current cycle number.
  //props.pc will tell pc address of instruction being used in this stage.
  //props.mc will give machine code of instrucion being used in this stage.
  var stagestyle = "stagebox stage" + props.name;
  return (
    <div className={stagestyle}>
      <div className="stageHeader">{props.name}</div>
      <div className="stageInfo">{props.number}</div>
      <div className="stageInfo">{props.pc}</div>
      <div className="stageInfo stageCurvesPosh">{props.mc}</div>
    </div>
  );
}

export default Stage;
