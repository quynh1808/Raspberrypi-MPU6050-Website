<?php
$servername = "localhost";
$username = "admin";
$password = "123456";
$dbname = "IoT_Exam";

// Tạo kết nối đến cơ sở dữ liệu MySQL
$conn = new mysqli($servername, $username, $password, $dbname);

// Kiểm tra kết nối
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
}

// Truy vấn dữ liệu từ cơ sở dữ liệu MySQL
$sql = "SELECT x, y FROM xyz";
$result = $conn->query($sql);



$table = array();
$table['cols'] = array(
  array('label' => 'X', 'type' => 'number'),
  array('label' => 'Y', 'type' => 'number')
);

$rows = array();
while($row = $result->fetch_assoc()) {
  $temp = array();
  $temp[] = array('v' => (float) $row['x']);
  $temp[] = array('v' => (float) $row['y']);
  $rows[] = array('c' => $temp);
}

$table['rows'] = $rows;
$jsonTable = json_encode($table);

// Tạo mảng chứa dữ liệu cho biểu đồ Scatter Chart

// Đóng kết nối
$conn->close();
echo $jsonTable;

?>
