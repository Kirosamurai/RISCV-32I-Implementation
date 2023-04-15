import React, { useState } from "react";
import Reg from "./RegisterFile";
import Mem from "./MemoryFile";
import PLFile from "./PipelineRegFile";

function Memory(props) {
  //props.currentcycle (will tell me the current cycle who's info I need to render.)
  const [memState, memCall] = useState(1);
  function regPress() {
    memCall(1);
  }
  function memPress() {
    memCall(0);
  }
  function plPress() {
    memCall(2);
  }
  return (
    <div>
      <div className="memoryHeading">
        <button
          className={
            memState === 1
              ? "memoryButton RegisterButton activeRightNow"
              : "memoryButton RegisterButton"
          }
          onClick={regPress}
        >
          Registers
        </button>
        <button
          className={
            memState === 0
              ? "memoryButton MemButton activeRightNow"
              : "memoryButton MemButton "
          }
          onClick={memPress}
        >
          Memory
        </button>
        <button
          className={
            memState === 2
              ? "memoryButton PipelineRegButton activeRightNow"
              : "memoryButton PipelineRegButton"
          }
          onClick={plPress}
        >
          Pipeline Registers
        </button>
      </div>
      <div className="memorybox">
        {memState === 1 && <Reg myCycle={props.currentcycle} />}
        {memState === 0 && <Mem myCycle={props.currentcycle} />}
        {memState === 2 && <PLFile myCycle={props.currentcycle} />}
      </div>
    </div>
  );
}

export default Memory;
