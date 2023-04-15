import React from "react";

function Arrows(props) {
  //props.forwarding will give info if lines are active or not?
  //props.fPath will give which path is active.
  const lines = "stageLines";
  const pointers = "stageLinePointer";
  var l1 = lines + "WBEX";
  var p1 = pointers + "WBEX";
  var l2 = lines + "WBMA";
  var p2 = pointers + "WBMA";
  var l3 = lines + "MAEX";
  var p3 = pointers + "MAEX";
  if (props.forwarding === 1) {
    if (props.fPath === "WB-EX") {
      l1 = l1 + " colorLineWBEX";
      p1 = p1 + " colorWBEX";
    } else if (props.fPath === "WB-MA") {
      l2 = l2 + " colorLineWBMA";
      p2 = p2 + " colorWBMA";
    } else {
      l3 = l3 + " colorLineMAEX";
      p3 = p3 + " colorMAEX";
    }
  }
  return (
    <div>
      <div id="WB-EX">
        <div className={l1}></div>
        <div className={p1}>&#x2193;</div>
      </div>
      <div id="WB-MA">
        <div className={l2}></div>
        <div className={p2}>&#x2193;</div>
      </div>
      <div id="MA-EX">
        <div className={l3}></div>
        <div className={p3}>&#x2193;</div>
      </div>
    </div>
  );
}

export default Arrows;
