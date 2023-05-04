<?php
header('Content-Type: application/json');

// dang nhap vao database
include("config.php");

// Doc gia tri tu database
$sql = "select pitch,roll,yaw from angle where id>(select max(id) from angle)-1";
$result = mysqli_query($conn,$sql);

$data = array();
foreach ($result as $row){
    $data[] = $row;
}


mysqli_close($conn);
echo json_encode($data);

?>