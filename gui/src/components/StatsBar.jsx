import React from "react";
import filedata from "./filedata.json";

function StatsBar() {
  return (
    <div className="allStatsContainer">
      <table>
        <tbody>
          <tr>
            <td>
              <div className="programStats">
                <div className="statBox">
                  PIPELINE: {filedata["pipeline"] ? "ON" : "OFF"}
                </div>
                <div className="statBox">
                  FORWARDING: {filedata["forward"] ? "ON" : "OFF"}
                </div>
                <div className="statBox">PL Reg Print: {"OFF"}</div>
              </div>
            </td>
            <td>
              <div className="programStats">
                <div className="statBox">
                  1. Total Cycles: {filedata["totalCycles"]}
                </div>
                <div className="statBox">
                  2. Total Instructions: {filedata["tInstructions"]}
                </div>
                <div className="statBox">3. CPI: {filedata["cpi"]}</div>
              </div>
            </td>
            <td>
              <div className="programStats">
                <div className="statBox">
                  4. {"\n"} Data Instructions: {filedata["dataInstructions"]}
                </div>
                <div className="statBox">
                  5. ALU Instructions: {filedata["aluInstructions"]}
                </div>
                <div className="statBox">
                  6. Control Instructions: {filedata["controlIns"]}
                </div>
              </div>
            </td>
            <td>
              <div className="programStats">
                <div className="statBox">7. Bubbles: {filedata["bubbles"]}</div>
                <div className="statBox">
                  8. Data Hazards: {filedata["dataHazards"]}
                </div>
                <div className="statBox">
                  9. Control Hazards: {filedata["controlHazards"]}
                </div>
              </div>
            </td>
            <td>
              <div className="programStats">
                <div className="statBox">
                  10. Branch Mispredicts: {filedata["branchMispredicts"]}
                </div>
                <div
                  className="statBox"
                  style={{ lineHeight: 1, paddingTop: 8 }}
                >
                  11. Data Hazard Bubbles: {filedata["dataBubbles"]}
                </div>
                <div
                  className="statBox"
                  style={{ lineHeight: 1, paddingTop: 8 }}
                >
                  12. {"\n"} Control Hazard Bubbles:{" "}
                  {filedata["controlBubbles"]}
                </div>
              </div>
            </td>
          </tr>
        </tbody>
      </table>
    </div>
  );
}

export default StatsBar;
