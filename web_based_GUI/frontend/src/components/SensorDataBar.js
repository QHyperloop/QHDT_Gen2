function SensorDataBar({
  sensors,
  updatedSensors,
  sensorName,
  unit,
  min,
  max,
}) {
  const nameMapping = {
    speed: "Speed",
    batteryTemp: "Battery Temperature",
    motorTemp: "Motor Temperature",
    batteryVoltage: "Battery Voltage",
    airSystemTemp: "Air System Temperature",
    tankPressure: "Tank Pressure",
    caliper1Pressure: "Caliper 1 Pressure",
    caliper2Pressure: "Caliper 2 Pressure",
    distanceTravelled: "Distance Travelled",
  };

  return (
    <div class="col" id="data-row">
      <div class="col-10">{nameMapping[sensorName]}</div>
      <div class="row" id="val-row">
        <div class="col-4" id="sensor-val">
          {`${
            sensors.length && sensors[sensors.length - 1][sensorName]
          } ${unit}`}
        </div>
        <div class="col-8">
          <meter
            class="bar"
            value={
              ((sensors.length && sensors[sensors.length - 1][sensorName]) -
                min) /
              (max - min)
            }
          />
        </div>
      </div>
    </div>
  );
}

export default SensorDataBar;
