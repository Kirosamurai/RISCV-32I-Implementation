import React from "react";

function Dep(props) {
  //props.controldep will tell whether control dependency exists.
  //props.datadep will tell whether data dependency exists.
  return (
    <div>
      {props.controldep && (
        <div className="Cdependent"> Control Dependency!</div>
      )}
      {props.datadep && <div className="Ddependent"> Data Dependency!</div>}
    </div>
  );
}

export default Dep;
