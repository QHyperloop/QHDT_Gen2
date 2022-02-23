exports.readToLocal = (message) => {
    // Convert to proper schema

    // Maybe json to object?

    var messageData = {
        batteryTemp: message.batteryTemp || localStorage.batteryTemp || 0,
        motorTemp: message.motorTemp || localStorage.batteryTemp || 0,
        podTemp: message.podTemp || localStorage.batteryTemp || 0,
        motorVoltage: message.motorVoltage || localStorage.batteryTemp || 0,
        electronicsVoltage: message.electronicsVoltage || localStorage.batteryTemp || 0,
        tankPressure: message.tankPressure || localStorage.batteryTemp || 0,
        vesselPressure: message.vesselPressure || localStorage.batteryTemp || 0
    }
    
    return messageData
}