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
  //  echo $_POST["DESCR"]." cosi'<br>";
  $command="UPDATE HISTOGRAMSET SET DESCR='".sqlstring($_POST["DESCR"])."',DOC='".
    sqlstring($_POST["DOC"])."' WHERE HSID=".$_POST["id"];
  //echo $command."<br>";
  $stid = OCIParse($conn,$command);
  OCIExecute($stid);
  return ocirowcount($stid);
}

$id=$_POST["id"]=preg_replace("/\/.*$/","",$_POST["id"]);
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
  histo_header($id,$_POST["htype"],"Update");
}

ocilogoff($conn);
echo "<br> <a href='../Histogram.php?hsid=${id}> Back to Histogram Set $id </a><br>";
?>

</body>
</html>
