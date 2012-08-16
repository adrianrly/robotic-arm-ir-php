<?php
include "php_serial.class.php";

$serial = new phpSerial;
$serial->deviceSet("/dev/ttyUSB0");
$serial->confBaudRate(9600);

$serial->deviceOpen();

// scriem pe port
$read = $serial->readPort();
echo $read;
$serial->deviceClose();
?>
