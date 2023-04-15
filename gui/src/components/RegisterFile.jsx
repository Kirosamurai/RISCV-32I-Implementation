import React from "react";
import RegisterLine from "./RegisterLine";
import filedata from "./filedata.json";

function Reg(props) {
  const lastCycle = filedata["totalCycles"];
  //props.myCycle (will tell me the current cycle who's info I need to render.)
  var registers = filedata["initialProgramStatus"]["registerFile"];

  //function to render each Register Line
  function CreateRegisterFile(regfiledata, index) {
    return (
      <RegisterLine key={index} regnumber={index} regvalue={regfiledata} />
    );
  }

  const cycling = filedata["cycle"];
  if (props.myCycle > 0 && props.myCycle < lastCycle) {
    const currentCycle = cycling[props.myCycle - 1];
    registers = currentCycle["registerFile"];
  } else if (props.myCycle === 0) {
    registers = filedata["initialProgramStatus"]["registerFile"];
  } else {
    registers = cycling[lastCycle - 2]["registerFile"];
  }
  return <div>{registers.map(CreateRegisterFile)}</div>;
}

export default Reg;
