<?php
error_reporting( E_ALL );
try {
  $conn = new PDO("mysql:host=localhost;dbname=smart_home", "smart_home", "smart");
  if($_GET["said"] != "" && $_GET["tmp"] != "" &&
     $_GET["prs"]  != "" && $_GET["alt"] != "" &&
     $_GET["vis"]  != "" && $_GET["hmd"] != "") {
     
  	$device_id = $_GET["said"];
  	$temperature = $_GET["tmp"];
  	$preasure = $_GET["prs"];
  	$altitude = $_GET["alt"];
  	$light = $_GET["vis"];
  	$humidity = $_GET["hmd"];
  	$date_time = date("Y/m/d h:i:s");
  	$statement = $conn->prepare("INSERT INTO sensor_unit (array_id, temperature, humidity, light, barometer, altitude, date_time) VALUES('".$device_id."', '".$temperature."', '".$humidity."', '".$light."', '".$preasure."', '".$altitude."','".$date_time."')");
  	$statement->execute();
  } else {
  	echo "Error: one of the parameters has not been set...";
  }  
} catch(PDOException $e) {
 echo "Connection failed: " . $e->getMessage();
}
?>
