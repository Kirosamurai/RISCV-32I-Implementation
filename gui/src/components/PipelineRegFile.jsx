import React from "react";
import filedata from "./filedata.json";
import MemLine from "./MemoryLine";

function PLFile(props) {
  //props.myCycle (will tell me the current cycle who's info I need to render.)
  //const lastCycle = filedata["totalCycles"];
  console.log(props.myCycle);
  //initializing all pipeline registers to null
  var savedPLRegIFDE = filedata["pipelineRegFile"]["IF-DE"];
  var savedPLRegDEEX = filedata["pipelineRegFile"]["IF-DE"];
  var savedPLRegEXMA = filedata["pipelineRegFile"]["IF-DE"];
  var savedPLRegMAWB = filedata["pipelineRegFile"]["IF-DE"];

  //function to render each Memory Line
  function CreateMemoryFile(plRegFiledata, index) {
    return (
      <MemLine
        key={index}
        memnumber={plRegFiledata}
        memvalue={this[plRegFiledata]}
      />
    );
  }

  //props.myCycle (will tell me the current cycle who's info I need to render.)
  return (
    <div>
      <div>
        <MemLine memnumber="IF-DE VAR" memvalue="VALUE" />
        {Object.keys(savedPLRegIFDE).map(CreateMemoryFile, savedPLRegIFDE)}
      </div>
      <div>
        <MemLine memnumber="DE-EX VAR" memvalue="VALUE" />
        {Object.keys(savedPLRegDEEX).map(CreateMemoryFile, savedPLRegDEEX)}
      </div>
      <div>
        <MemLine memnumber="EX-MA VAR" memvalue="VALUE" />
        {Object.keys(savedPLRegEXMA).map(CreateMemoryFile, savedPLRegEXMA)}
      </div>
      <div>
        <MemLine memnumber="MA-WB VAR" memvalue="VALUE" />
        {Object.keys(savedPLRegMAWB).map(CreateMemoryFile, savedPLRegMAWB)}
      </div>
    </div>
  );
}

export default PLFile;
