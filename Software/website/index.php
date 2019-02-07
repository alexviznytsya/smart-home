<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<title>Title of the document</title>
<script type="text/javascript" src="javascript/jquery-3.2.1.min.js"></script>
<script>


</script>

</head>
<body>
<button id="turnOn" type="button">Turn Light ON</button>
<button id="turnOff" type="button">Turn Light OFF</button>

<script>
$("#turnOn").click(function(){ 
	$.get("turn_power.php?device_id=1&state=1", function(data, status){
        
    	});
}); 
$("#turnOff").click(function(){ 
	$.get("turn_power.php?device_id=1&state=0", function(data, status){
        
    });
}); 
</script>
</body>
</html>



<?php

?>
