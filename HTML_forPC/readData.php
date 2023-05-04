<?php
header('Content-Type: application/json');

// dang nhap vao database
include("config.php");
$dev = $_POST["username"];

// Doc gia tri tu database
//$sql = "select * from temptable where time>(select max(time) from temptable)-20";

$sql = "select time,temp from temptable where device=('$dev')";
//$sql = "select time,temp from temptable";
$result = mysqli_query($conn,$sql);

$data = array();
foreach ($result as $row){
    $data[] = $row;
}


mysqli_close($conn);
echo json_encode($data);

?>
