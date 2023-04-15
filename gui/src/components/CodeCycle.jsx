import React from "react";

function Cycleblock(props) {
  return (
    <div className={props.cycleStyle}>
      <table>
        <tbody>
          <tr>
            <td className="codeblockCycle" id={props.id}>
              {props.number}
            </td>
            <td className="codeblockPC">{props.pc}</td>
            <td className="codeblockMC">{props.machinecode}</td>
          </tr>
        </tbody>
      </table>
    </div>
  );
}

export default Cycleblock;
