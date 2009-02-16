<HTML>
 <HEAD>
<LINK REL=STYLESHEET TYPE="text/css" HREF="../styles_screen.css">
 </HEAD>
<body>
<?php 
include '../util.php'; include '../dbforms.php'; 
$conn=HistDBconnect(1);

function update_histo_header() {
  global $conn;

  $command="UPDATE HISTOGRAMSET SET DESCR='".sqlstring($_POST["DESCR"])."',DOC='".
    sqlstring($_POST["DOC"])."' WHERE HSID=".HistoSet($_POST["id"]);
  $stid = OCIParse($conn,$command);
  OCIExecute($stid);

  if($_POST["htype"] == "HID" || $_POST["NHS"] == 1 && ocirowcount($stid)>0) {
    $refpage = $_POST["REFPAGE"];
    if ($refpage == "none") 
      $rpcom="NULL";
    else 
      $rpcom="'${refpage}'";
    $pcommand = "UPDATE HISTOGRAM set REFPAGE=${rpcom} where HID='".
      SingleHist($_POST["id"])."'";
    $stid = OCIParse($conn,$pcommand);
    OCIExecute($stid);
  }
  

  return ocirowcount($stid);
}

?>
<H2 ALIGN="CENTER">Update header for histogram set <?php echo $id ?></H2><hr>

<?php

if ($_POST["Update_header"] == 'Confirm') {
  if (update_histo_header())
    echo "Histogram Header updated successfully<br><br>\n";
  else
    echo "<font color=red> <B>Got errors from update_histo_header() </B></font><br><br>\n";
} 
else {
  echo "Please check your data and confirm <br><br>";
  histo_header($_POST["id"],$_POST["htype"],"Update");
}

ocilogoff($conn);
echo "<br> <a href='../Histogram.php?".strtolower($_POST["htype"])."=${id}> Back to Histogram Set $id </a><br>";
?>

</body>
</html>
