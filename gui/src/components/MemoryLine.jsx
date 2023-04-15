import React from "react";

function MemLine(props) {
  return (
    <div className="registerLines">
      <table>
        <tbody>
          <tr>
            <td className="memLinesX">{props.memnumber}</td>
            <td className="memLinesVal">{props.memvalue}</td>
          </tr>
        </tbody>
      </table>
    </div>
  );
}

export default MemLine;
