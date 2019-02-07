<?php
error_reporting( E_ALL );
try {
  $conn = new PDO("mysql:host=localhost;dbname=smart_home", "smart_home", "smart");
  if($_GET["device_id"] != "" && $_GET["state"] != "") {
  	$device_id = $_GET["device_id"];
  	$state = $_GET["state"];
  	$date_time = date("Y/m/d h:i:s");
  	$statement = $conn->prepare("INSERT INTO power_unit (power_id, state, date_time) VALUES('".$device_id."','".$state."','".$date_time."')");
  	$statement->execute();
  } else {
  	echo "Error: one of the parameters has not been set...";
  }  
} catch(PDOException $e) {
 echo "Connection failed: " . $e->getMessage();
}
?>