<?php
try {
  $conn = new PDO("mysql:host=localhost;dbname=smart_home", "smart_home", "smart");
  $statement = $conn->query("SELECT * FROM adruino_1 ORDER BY id DESC");
  //  echo "fff";/
} catch(PDOException $e) {
 echo "Connection failed: " . $e->getMessage();
}

  $value = $statement->fetch(PDO::FETCH_ASSOC);

?>

<!DOCTYPE html>
<html>
<head>
<title>Selfupdated page</title>
</head>
<script>
setTimeout(function() {
	window.location.reload(1);
}, 1000 )
</script>

<body>

<div>Received from adruino: </div>
<div id="update"><?php echo $value["value"]; ?></div>
</body>
</html>
