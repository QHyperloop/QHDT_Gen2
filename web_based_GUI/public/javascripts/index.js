$(document).ready(function(){
        // Initially set pod-status in local storage to safe to approach.
        localStorage.setItem('pod-status', 2);
});

// Set pod-status in local storage according to button press:
$('#emerg-stop').click(function() {
        localStorage.setItem('pod-status', 0);
});

$('#ready-launch').click(function() {
        localStorage.setItem('pod-status', 1);
});

$('#safe-approach').click(function() {
        localStorage.setItem('pod-status', 2);
});

// Set an event that polls the local storage for pod-status and calls to the back-end to transmit the appropriate status to the pod.
setInterval(function() {
        $.get('/sensors',
        function(data, status) {
                document.getElementById("battery-temp").value = data.batteryTemp
                document.getElementById("motor-temp").value = data.motorTemp
                document.getElementById("pod-temp").value = data.podTemp
                document.getElementById("motor-voltage").value = data.motorVoltage
                document.getElementById("electronics-voltage").value = data.electronicsVoltage
                document.getElementById("tank-pressure").value = data.tankPressure
                document.getElementById("vessel-pressure").value = data.vesselPressure
        });
}, 1000);