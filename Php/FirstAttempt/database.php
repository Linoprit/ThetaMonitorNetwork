<?php
$servername = "localhost";
$username = "harald";
$password = "sOvjV9mYHIKRQDiZ";

// Create connection
$conn = new mysqli($servername, $username, $password);

// Check connection
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
}
echo "Connected successfully<br><br>";

$sql = "use ThetaSensorData";
$result = $conn->query($sql);

 // select * from Sensor_ID_Table;
 // PrimKey, ID0, ID1, ID2, ID3, ID4, ID5, ID6, ID7, ID_text, Location_ID, Location 
 // select * from TestSensordata;
 // PrimKey, ID_text, Temperature, Location_ID, Location, Time_measured
 // SELECT * FROM TestSensordata where Location_ID = 'WST_O';
 
//$sql = "SELECT * FROM TestSensordata";
$sql = "SELECT * FROM TestSensordata where Location_ID = 'WST_O'";
$result = $conn->query($sql);

if ($result->num_rows > 0) {
    // output data of each row
    while($row = $result->fetch_assoc()) {
        echo "PrimKey: " . $row["PrimKey"]. " - Temperature: " . $row["Temperature"]. " / " . $row["Location"]. "<br>";
    }
} else {
    echo "0 results";
}
$conn->close();


// SELECT * FROM `TestSensordata` WHERE Time_measured > '2019-03-05 17:00:00';
// SELECT * FROM `TestSensordata` WHERE Time_measured > CURRENT_TIMESTAMP() - INTERVAL 2 MINUTE;


?> 