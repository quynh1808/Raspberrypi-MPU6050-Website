<?php
// doc du lieu tu website gui ve
$acc = $_POST["username"];

// ket noi database
include("config.php");

// gui data xuong database
//$sql = "insert into chart3(data2) values ('$acc')";
$sql = "update tempsample set data = ('$acc')";
mysqli_query($conn, $sql);

// ngat ket noi voi database
mysqli_close($conn);
?>