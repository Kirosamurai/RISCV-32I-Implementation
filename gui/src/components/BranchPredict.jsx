import React from "react";

function BranchPredict(props) {
  //props.predict will tell if BTB[Branch Target Buffer] is used or not.
  //props.predictS will tell whether prediction was successful or not.
  return (
    <div>
      {props.predict ? (
        props.predictS ? (
          <div className="btbCorrect">Branch Prediction: Take Branch</div>
        ) : (
          <div className="btbWrong">Branch Prediction: Don't Take</div>
        )
      ) : (
        <div className="btbgrey">Branch Predictor not in use.</div>
      )}
    </div>
  );
}

export default BranchPredict;
