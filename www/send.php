<?php
include "php_serial.class.php";

$serial = new phpSerial;
$serial->deviceSet("/dev/ttyUSB0");
$serial->confBaudRate(9600);
$serial->deviceOpen();


// scriem pe port
$serial->sendMessage(chr(255));
$serial->sendMessage(chr($_GET['servo']));
$serial->sendMessage(chr($_GET['pos']));

echo("Am mutat servo-ul #".$_GET['servo']." la ".$_GET['pos']." grade");


$serial->deviceClose();
?>
