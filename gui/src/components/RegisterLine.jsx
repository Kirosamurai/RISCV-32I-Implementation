import React from "react";

function RegLine(props) {
  return (
    <div className="registerLines">
      <table>
        <tbody>
          <tr>
            <td className="registerLinesX">x{props.regnumber}</td>
            <td className="registerLinesVal">{props.regvalue}</td>
          </tr>
        </tbody>
      </table>
    </div>
  );
}

export default RegLine;
