<?php 
include '../util.php';
$conn=HistDBconnect(1);
$id=$_POST["id"];
$htype=$_POST["htype"];
?>
<HTML>
 <HEAD>
<LINK REL=STYLESHEET TYPE="text/css" HREF="../styles_screen.css">
 </HEAD>
<body>

<H2 ALIGN="CENTER">Removing histogram <?php echo $id ?></H2>
<H3 ALIGN="CENTER"><?php echo $_POST["identifier"] ?></H3><hr>

<?php

function delete_histo($id,$htype) {
  global $conn;
  if ($htype=="HID") {
    $command="begin :fn := OnlineHistDB.DeleteHistogram('$id'); end;";
  }
  else {
    $command="begin :fn := OnlineHistDB.DeleteHistogramSet($id); end;";
  }
  if($debug) echo "command is $command <br>";
  $stid = OCIParse($conn,$command);
  ocibindbyname($stid,":fn",$out,50);
  OCIExecute($stid);
  return $out; 
}

if ($_POST["Delete_Histo"] == 'Confirm') {
  if (delete_histo($id,$htype))
    echo "Histogram record removed successfully<br><br>\n";
  else
    echo "<font color=red> <B>Got errors from delete_histo </B></font><br><br>\n";
} 
else {
  
  echo "Are you sure to remove this record? <br>";
  echo "<B> you are going to lose all display and analysis settings for this histogram</B><br>\n";
  echo "<form action='".$_SERVER["PHP_SELF"]."' method='POST'>\n";
  echo "<input type='hidden' name='id' value='${id}'>\n";
  echo "<input type='hidden' name='htype' value='${htype}'>\n";
  echo "<input type='hidden' name='identifier' value='".$_POST["identifier"]."'>\n";
  echo "<input align=right type='submit' name='Delete_Histo' value='Confirm'>\n";
  echo "</form>\n";

  echo "<hr><br> <a href='../Histogram.php?".strtolower($_POST["htype"])."=${id}'> Back to Histogram $id </a><br>";
 }
ocilogoff($conn);
?>
</body>
</html>