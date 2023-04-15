import React, { useState } from "react";
import Header from "./components/Header";
import Footer from "./components/Footer";
import Taskbar from "./components/Taskbar";
import Codebox from "./components/Codebox";
import Pipeline from "./components/PipelineDiagram";
import Memory from "./components/Memory";
import StatsBar from "./components/StatsBar";
import filedata from "./components/filedata.json";

function App() {
  const maxCycles = filedata["totalCycles"];
  const [runState, runCall] = useState(true);
  const [stepState, stepCall] = useState(1);
  const [prevState, prevCall] = useState(0);
  const [resetState, resetCall] = useState(false);
  function runPress() {
    runCall(false);
    stepCall(maxCycles + 1); //make this equal to last cycle number
    prevCall(maxCycles);
    resetCall(true);
  }

  function stepPress() {
    stepCall(stepState + 1);
    if (stepState + 1 > maxCycles) {
      runCall(false);
    }
    prevCall(prevState + 1);
    resetCall(true);
  }

  function prevPress() {
    stepCall(stepState - 1);
    prevCall(prevState - 1);
    runCall(true);
    if (prevState < 2) {
      resetCall(false);
    }
  }

  function resetPress() {
    runCall(true);
    stepCall(1);
    prevCall(0);
    resetCall(false);
  }
  return (
    <div>
      <Header />
      <Taskbar
        maxCycles={maxCycles}
        runState={runState}
        runPress={runPress}
        stepState={stepState}
        stepPress={stepPress}
        prevState={prevState}
        prevPress={prevPress}
        resetState={resetState}
        resetPress={resetPress}
      />
      {filedata["pipeline"] ? (
        <div>
          <Pipeline currentcycle={stepState} />
          <StatsBar />
        </div>
      ) : (
        <Codebox redcycle={stepState} />
      )}
      <Memory currentcycle={stepState} />

      <Footer />
    </div>
  );
}

export default App;
