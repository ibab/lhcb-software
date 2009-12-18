<?php 
include '../util.php'; include '../dbforms.php'; 
$conn=HistDBconnect();
?>
<HTML>
 <HEAD>
<LINK REL=STYLESHEET TYPE="text/css" HREF="../styles_screen.css">
 </HEAD>
<body>
<?php 
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
  $command="begin OnlineHistDB.DeclareBinLabels($hsid,$inset,LABELS(";
  $first=1;
  $nlabx=$nlaby=0;
  for ($i=1 ; $i<=$_POST["nlabx"]; $i++) {
     if(strlen($_POST["LABX${i}"]) > 0)
       $nlabx=$i;
  }
  for ($i=1 ; $i<=$nlabx; $i++) {
    if ($first == 0) $command .= ",";
    $command .= "'".$_POST["LABX${i}"]."'";
    $first=0;
  }
  for ($i=1 ; $i<=$_POST["nlaby"]; $i++) {
    if(strlen($_POST["LABY${i}"]) >0 )
      $nlaby=$i;
  }
  for ($i=1 ; $i<=$nlaby; $i++) {
    if ($first == 0) $command .= ",";
    $command .= "'".$_POST["LABY${i}"]."'";
    $first=0;
  }
  $command .= "),$nlabx); end;";
  if($debug) echo "command is $command<br>\n";
  $stid = OCIParse($conn,$command);
  $r=OCIExecute($stid);
  if (!$r) {
    echo "<font color=red> <B>Got errors when updating labels </B></font><br><br>\n";
  }
  else {
    //   ocicommit($conn);
    echo "Histogram Bin Labels updated successfully<br><br>\n";
  }
  ocifreestatement($stid);   
}
else {
  echo "Please check your data and confirm <br><br>";
  histo_labels($id,$_POST["htype"],"update");
}


ocilogoff($conn);
echo "<br> <a href='../Histogram.php?".strtolower($_POST["htype"])."=${id}'> Back to Histogram Record $id </a><br>";
?>

</body>
</html>