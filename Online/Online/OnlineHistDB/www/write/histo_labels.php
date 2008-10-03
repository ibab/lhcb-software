<HTML>
 <HEAD>
<LINK REL=STYLESHEET TYPE="text/css" HREF="../styles_screen.css">
 </HEAD>
<body>
<?php 
include '../util.php'; include '../dbforms.php'; 
$conn=HistDBconnect();
if (!$conn) {
  $e = ocierror();
  print htmlentities($e['message']);
  exit;
}

$id=$_POST["id"];
?>
<H2 ALIGN="CENTER">Add/Update Custom Bin Labels for histogram <?php echo $id ?></H2><hr>

<?php

if ($_POST["Update_labels"] == 'Confirm') {
  $hsid = HistoSet($id);
  $inset = ($htype == "HID") ? "'${id}'" : "NULL";
  $command="begin OnlineHistDB.DeclareBinLabels($hsid,$inset,PARAMETERS(";
  $first=1;
  for ($i=1 ; $i<=$nlab; $i++) {
    if(strlen($_POST["LAB${i}"])>0) {
      if ($first == 0) $command .= ",";
      $command .= "'".$_POST["LAB${i}"]."'";
      $first=0;
    }
  }
  $command .= ")); end;";
  if($debug) echo "command is $command<br>\n";
  $stid = OCIParse($conn,$command);
  $r=OCIExecute($stid,OCI_DEFAULT);
  if (!$r) {
    echo "<font color=red> <B>Got errors when updating labels </B></font><br><br>\n";
  }
  else {
    ocicommit($conn);
    echo "Histogram Bin Labels updated successfully<br><br>\n";
  }
  ocifreestatement($stid);   
}
else {
  echo "Please check your data and confirm <br><br>";
  histo_labels($id,$_POST["htype"],"update");
}


ocilogoff($conn);
echo "<br> <a href='../Histogram.php?".strtolower($_POST["htype"])."=${id}> Back to Histogram Record $id </a><br>";
?>

</body>
</html>