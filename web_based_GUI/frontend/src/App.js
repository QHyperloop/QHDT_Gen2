import logo from "./images/QH_logo.png";
import styles from "./stylesheets/custom-style.css";
import stlyes from "./stylesheets/bootstrap/bootstrap.min.css";
import { initialize, emergencyStop, safe, ready, getSensors } from "./api";
import React, { useEffect, useRef, useState } from "react";

import {
  LineChart,
  Line,
  XAxis,
  YAxis,
  CartesianGrid,
  Tooltip,
  Legend,
  ResponsiveContainer,
} from "recharts";

function App() {
  document.body.style.background = "black";
  document.title = "QHDT Dashboard";

  const [width, setWidth] = useState(window.innerWidth);
  const [height, setHeight] = useState(window.innerHeight);
  const [currentState, setCurrentState] = useState("N/A");
  const [sensors, setSensors] = useState([]);

  function handleWindowSizeChange() {
    setWidth(window.innerWidth);
    setHeight(window.innerHeight);
  }
  useEffect(() => {
    window.addEventListener("resize", handleWindowSizeChange);
    return () => {
      window.removeEventListener("resize", handleWindowSizeChange);
    };
  }, []);

  let updatedSensors = [];
  const handleSensors = async () => {
    const response = await getSensors();
    let newSensors = [...updatedSensors, response];
    if (newSensors.length > 10) {
      newSensors.shift();
    }
    setSensors(newSensors);
    updatedSensors = newSensors;
  };

  useEffect(() => {
    const interval = setInterval(() => {
      handleSensors();
    }, 5000);
    return () => clearInterval(interval);
  }, []);

  const handleInitialize = async () => {
    console.log("Initialize");
    const response = await initialize();
    setCurrentState("Initialize");
  };

  const handleEmergencyStop = async () => {
    console.log("Emergency Stop");
    const response = await emergencyStop();
    setCurrentState("Emergency Stop");
  };

  const handleReady = async () => {
    console.log("Ready");
    const response = await ready();
    setCurrentState("Ready");
  };

  const handleSafe = async () => {
    console.log("Safe");
    const response = await safe();
    setCurrentState("Safe");
  };

  const handleColor = (currentState) => {
    if (currentState === "Emergency Stop") {
      return "#dc3545";
    } else if (currentState === "Safe" || currentState === "Ready") {
      return "green";
    } else {
      return "white";
    }
  };

  return (
    <div style={{ color: "white", backgroundColor: "black", width: "100%" }}>
      <div class="row" id="image-blurred-edge">
        <img src={logo} />
      </div>
      <div class="container-fluid" id="drop-top">
        <div class="row">
          <div class="col-2" id="control-panel">
            <div class="row" id="control-row">
              <button
                class="btn btn-info"
                id="safe-approach"
                onClick={() => {
                  handleInitialize();
                }}
              >
                Initialize
              </button>
              <button
                class="btn btn-info"
                id="ready-launch"
                onClick={() => {
                  handleReady();
                }}
              >
                Ready To Launch
              </button>
              <button
                class="btn btn-info"
                id="safe-approach"
                onClick={() => {
                  handleSafe();
                }}
              >
                Safe To Approach
              </button>
              <button
                class="btn btn-danger"
                id="emerg-stop"
                onClick={() => {
                  handleEmergencyStop();
                }}
              >
                Emergency Stop
              </button>
            </div>
            <div class="current-state">
              <h4 class="current-state-title">Current State:</h4>
              <h4
                class="current-state-value"
                style={{
                  color: handleColor(currentState),
                }}
              >
                {currentState}
              </h4>
            </div>
          </div>
          <div class="col-6" id="pod-status-pannel">
            <div class="row">
              <h4 class="telemetry-title">Telemetry</h4>
            </div>
            <LineChart
              width={width / 2.1}
              height={height - 300}
              data={sensors}
              margin={{ top: 5, right: 20, left: 10, bottom: 5 }}
            >
              <XAxis dataKey="name" />
              <Legend />
              <Line
                type="monotone"
                dataKey="batteryTemp"
                stroke="red"
                dot={false}
                yAxisId={0}
              />
              <Line
                type="monotone"
                dataKey="motorTemp"
                stroke="yellow"
                dot={false}
                yAxisId={1}
              />
              <Line
                type="monotone"
                dataKey="podTemp"
                stroke="blue"
                dot={false}
                yAxisId={1}
              />
            </LineChart>
          </div>
          <div class="col-4" id="sensors-table">
            <div class="row" id="data-row">
              <div class="col-6">
                <h4>Sensor Data</h4>
              </div>
            </div>
            <div class="row" id="data-row">
              <div class="col-4">Battery Temperature</div>
              <div class="col-2">
                <meter
                  class="bar"
                  id="battery-temp"
                  value={
                    updatedSensors.length &&
                    updatedSensors[updatedSensors.length - 1].batteryTemp
                  }
                >
                  60%
                </meter>
              </div>
            </div>
            <div class="row" id="data-row">
              <div class="col-4">Motor Temperature</div>
              <div class="col-2">
                <meter
                  class="bar"
                  id="motor-temp"
                  value={
                    updatedSensors.length &&
                    updatedSensors[updatedSensors.length - 1].motorTemp
                  }
                >
                  50%
                </meter>
              </div>
            </div>
            <div class="row" id="data-row">
              <div class="col-4">Pod Temperature</div>
              <div class="col-2">
                <meter
                  class="bar"
                  id="pod-temp"
                  value={
                    updatedSensors.length &&
                    updatedSensors[updatedSensors.length - 1].podTemp
                  }
                >
                  50%
                </meter>
              </div>
            </div>
            <div class="row" id="data-row">
              <div class="col-4">Motor Voltage</div>
              <div class="col-2">
                <meter
                  class="bar"
                  id="motor-voltage"
                  value={
                    updatedSensors.length &&
                    updatedSensors[updatedSensors.length - 1].motorVoltage
                  }
                >
                  50%
                </meter>
              </div>
            </div>
            <div class="row" id="data-row">
              <div class="col-4">Electronics Voltage</div>
              <div class="col-2">
                <meter
                  class="bar"
                  id="electronics-voltage"
                  value={
                    updatedSensors.length &&
                    updatedSensors[updatedSensors.length - 1].electronicsVoltage
                  }
                >
                  50%
                </meter>
              </div>
            </div>
            <div class="row" id="data-row">
              <div class="col-4">Air Tank Pressure</div>
              <div class="col-2">
                <meter
                  class="bar"
                  id="tank-pressure"
                  value={
                    updatedSensors.length &&
                    updatedSensors[updatedSensors.length - 1].tankPressure
                  }
                >
                  50%
                </meter>
              </div>
            </div>
            <div class="row" id="data-row">
              <div class="col-4">Vessel Pressure</div>
              <div class="col-2">
                <meter
                  class="bar"
                  id="vessel-pressure"
                  value={
                    updatedSensors.length &&
                    updatedSensors[updatedSensors.length - 1].vesselPressure
                  }
                >
                  50%
                </meter>
              </div>
            </div>
          </div>
        </div>
      </div>
    </div>
  );
}

export default App;

const colorMap = [
  "#fc0303",
  "#fc4103",
  "#fc7f03",
  "#fcb903",
  "#fcf703",
  "#b9fc03",
  "#7bfc03",
  "#3dfc03",
  "#03fc3d",
];
