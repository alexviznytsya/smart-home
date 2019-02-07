<?php
error_reporting( E_ALL );
try {
    $conn = new PDO("mysql:host=localhost;dbname=smart_home", "smart_home", "smart");
        $statement_sa = $conn->prepare("SELECT temperature, humidity, light, barometer, altitude FROM sensor_unit WHERE array_id=1 GROUP BY id DESC");
        $statement_sa->execute();
        $statement_po = $conn->prepare("SELECT state FROM power_unit WHERE power_id=1 GROUP BY id DESC");
        $statement_po->execute();
    header('Content-Type: application/json');
        $value = json_encode(array_merge($statement_sa->fetch(PDO::FETCH_ASSOC), $statement_po->fetch(PDO::FETCH_ASSOC)));
        echo $value;
} catch(PDOException $e) {
    echo "Connection failed: " . $e->getMessage();
}
?>
