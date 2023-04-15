import React from "react";
import CodeCycle from "./CodeCycle";
import filedata from "./filedata.json";

function CreateCodeCycle(cycledata) {
  if (cycledata.number == this) {
    return (
      <CodeCycle
        cycleStyle="codeboxRedCycle"
        id="redcode"
        key={cycledata.number}
        number={cycledata.number}
        pc={cycledata.IFpc}
        machinecode={cycledata.IFhexcode}
      />
    );
  } else {
    return (
      <CodeCycle
        cycleStyle="codeboxEachCycle"
        key={cycledata.number}
        number={cycledata.number}
        pc={cycledata.IFpc}
        machinecode={cycledata.IFhexcode}
      />
    );
  }
}

function Codebox(props) {
  const element = document.getElementById("redcode");
  return (
    <div>
      <div className="codebox">
        <div className="codeboxHeading">
          <p className="codeboxCycle">CYCLE NUMBER</p>
          <p className="codeboxPC">PC</p>
          <p className="codeboxMC">MACHINE CODE</p>
        </div>
        <div className="codeBlock" id="codes">
          {element ? element.scrollIntoView({ block: "center" }) : null}
          {
            filedata["cycle"].map(
              CreateCodeCycle,
              props.redcycle
            ) /*array.map(function, thisValue)*/
          }
        </div>
      </div>
    </div>
  );
}

export default Codebox;
