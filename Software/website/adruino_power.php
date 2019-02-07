<?php 
try {
  $conn = new PDO("mysql:host=localhost;dbname=smart_home", "smart_home", "smart");
  $statement = $conn->query("SELECT * FROM power_unit ORDER BY id DESC");
} catch(PDOException $e) {
 echo "Connection failed: " . $e->getMessage();
}
 $value = $statement->fetch(PDO::FETCH_ASSOC);
 echo "command=".$value["state"];
?>

