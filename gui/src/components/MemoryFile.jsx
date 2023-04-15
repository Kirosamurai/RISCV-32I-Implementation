import React from "react";
import filedata from "./filedata.json";
import MemLine from "./MemoryLine";

function Mem(props) {
  //props.myCycle (will tell me the current cycle who's info I need to render.)
  const lastCycle = filedata["totalCycles"];

  var savedMemory = filedata["initialProgramStatus"]["memoryFile"];

  //function to render each Memory Line
  function CreateMemoryFile(memfiledata, index) {
    return (
      <MemLine
        key={index}
        memnumber={memfiledata}
        memvalue={savedMemory[memfiledata]}
      />
    );
  }
  const cycling = filedata["cycle"];
  if (props.myCycle > 0 && props.myCycle < lastCycle) {
    const currentCycle = cycling[props.myCycle - 1];
    savedMemory = currentCycle["memFile"];
  } else if (props.myCycle === 0) {
    savedMemory = filedata["initialProgramStatus"]["memoryFile"];
  } else {
    savedMemory = cycling[lastCycle - 2]["memFile"];
  }

  //props.myCycle (will tell me the current cycle who's info I need to render.)
  return <div>{Object.keys(savedMemory).map(CreateMemoryFile)}</div>;
}

export default Mem;
