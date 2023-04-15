import React from "react";
import Stage from "./PipelineStage";
import Arrows from "./PipelineArrows";
import BranchPredict from "./BranchPredict";
import Dep from "./Dependency";
import DATA from "./filedata.json";

function Pipeline(props) {
  //props.currentcycle will tell me which cycle is going on right now.

  //props.currentcycle-1 because the indexing starts from 0
  const thiscycledata = DATA["cycle"][props.currentcycle - 1];
  const forwardStatus = thiscycledata["forwarding"];
  const fPath = thiscycledata["fPath"];

  return (
    <div>
      <BranchPredict
        predict={thiscycledata["branchPredict"]}
        predictS={thiscycledata["bPSuccess"]}
      />
      <Dep
        controldep={thiscycledata["controlDep"]}
        datadep={thiscycledata["dataDep"]}
      />
      {DATA["forward"] ? (
        <Arrows forwarding={forwardStatus} fPath={fPath} />
      ) : (
        <Arrows forwarding={0} />
      )}
      <Stage
        name="IF"
        number={thiscycledata["number"]}
        pc={thiscycledata["IFpc"]}
        mc={
          props.currentcycle <= DATA["totalCycles"] && "FETCHING INSTRUCTION."
        }
      />
      <Stage
        name="DE"
        number={thiscycledata["number"]}
        pc={thiscycledata["DEpc"]}
        mc={thiscycledata["DEhexcode"]}
      />
      <Stage
        name="EX"
        number={thiscycledata["number"]}
        pc={thiscycledata["EXpc"]}
        mc={thiscycledata["EXhexcode"]}
      />
      <Stage
        name="MA"
        number={thiscycledata["number"]}
        pc={thiscycledata["MApc"]}
        mc={thiscycledata["MAhexcode"]}
      />
      <Stage
        name="WB"
        number={thiscycledata["number"]}
        pc={thiscycledata["WBpc"]}
        mc={thiscycledata["WBhexcode"]}
      />
    </div>
  );
}

export default Pipeline;
