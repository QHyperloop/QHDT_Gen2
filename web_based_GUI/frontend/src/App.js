import logo from './images/QH_logo.png';
import styles from './stylesheets/custom-style.css';
import stlyes from './stylesheets/bootstrap/bootstrap.min.css';

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

  const data = [
    {
      name: "-50",
      battery: 200,
      motor: 300
    },
    {
      name: "-40",
      battery: 500,
      motor: 1198
    },
    {
      name: "-30",
      battery: 600,
      motor: 1398
    },
    {
      name: "-20",
      battery: 800,
      motor: 1100
    },
    {
      name: "-10",
      battery: 1000,
      motor: 900
    },
    {
      name: "0",
      battery: 700,
      motor: 1100
    }
  ]
  return (

    
    <div style={{color: 'white', backgroundColor: 'black', width: '100%'}}>

      <div class="row" id="image-blurred-edge">
        <img src={logo}/>
      </div>
      <div class="container-fluid" id="drop-top">
        <div class="row">
          <div class="col-2" id="control-panel">
            <div class="row" id="control-row">
              <button class="btn btn-danger" id="emerg-stop">Emergency Stop</button>
              <button class="btn btn-info"  id="ready-launch">Ready To Launch</button>
              <button class="btn btn-info" id="safe-approach">Safe To Approach</button>
            </div>
          </div>
          <div class="col-6" id="pod-status-pannel">
            <div class="row" >
              <h4>Telemetry</h4>
            </div>
            <LineChart
              width={800}
              height={600}
              data = {data}
              margin={{ top: 5, right: 20, left: 10, bottom: 5 }}
            >
              <XAxis dataKey="name" />
              <Legend />
              {/* <Tooltip /> */}
              {/* <CartesianGrid strokeDasharray="#f5f5f5" /> */}
              <Line type="monotone" dataKey="battery" stroke="red" dot={false} yAxisId={0} />
              <Line type="monotone" dataKey="motor" stroke="yellow" dot={false} yAxisId={1} />
            </LineChart>
          </div>
          <div class="col-4" id="sensors-table">
            <div class="row" id="data-row">
              <div class="col-6">
                <h4>Sensor Data</h4>
              </div>
            </div>
            <div class="row" id="data-row">
              <div class="col-4">
                Battery Temperature
              </div>
              <div class="col-2">
                <meter class="bar" id="battery-temp" value="0.6">60%</meter>
              </div>
            </div>
            <div class="row" id="data-row">
              <div class="col-4">
                Motor Temperature
              </div>
              <div class="col-2">
                <meter class="bar" id="motor-temp" value="0.5">50%</meter>
              </div>
            </div>
            <div class="row" id="data-row">
              <div class="col-4">
                Pod Temperature
              </div>
              <div class="col-2">
                <meter class="bar" id="pod-temp" value="0.5">50%</meter>
              </div>
            </div>
            <div class="row" id="data-row">
              <div class="col-4">
                Motor Voltage
              </div>
              <div class="col-2">
                <meter class="bar" id="motor-voltage" value="0.5">50%</meter>
              </div>
            </div>
            <div class="row" id="data-row">
              <div class="col-4">
              Electronics Voltage
              </div>
              <div class="col-2">
                <meter class="bar" id="electronics-voltage" value="0.5">50%</meter>
              </div>
            </div>
            <div class="row" id="data-row">
              <div class="col-4">
                Air Tank Pressure
              </div>
              <div class="col-2">
                <meter class="bar" id="tank-pressure" value="0.5">50%</meter>
              </div>
            </div>
            <div class="row" id="data-row">
              <div class="col-4">
                Vessel Pressure
              </div>
              <div class="col-2">
                <meter class="bar" id="vessel-pressure" value="0.5">50%</meter>
              </div>
            </div>
          </div>
        </div>
      </div>
    </div>
  );
}

export default App;

