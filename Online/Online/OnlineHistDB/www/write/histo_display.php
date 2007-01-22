<HTML>
 <HEAD>
<LINK REL=STYLESHEET TYPE="text/css" HREF="../styles_screen.css">
 </HEAD>
<body>
<?php 
include '../util.php'; include '../dbforms.php'; 
$conn=HistDBconnect(1);


function update_histo_display() {
  global $conn;

  $command="begin :out := OnlineHistDB.".
    ( $_POST["htype"] == "SHID" ? 
      "DeclareHistoPageDisplayOptions('".$_POST["id"]."','".$_POST["PAGE"]."',".$_POST["INSTANCE"] :
      ($_POST["htype"] == "HSID" ? 
       "DeclareHistoSetDisplayOptions(".$_POST["id"] :
       "DeclareHistDisplayOptions('".$_POST["id"]."'") );
  foreach  (array('LABEL_X','LABEL_Y','LABEL_Z','YMIN','YMAX','STATS','FILLSTYLE','FILLCOLOR',
                  'LINESTYLE','LINECOLOR','LINEWIDTH','DRAWOPTS') as $var) 
    if ($_POST[$var] != '') $command.=",K${var} => '".$_POST[$var]."'";
  $command.="); end;";
  //echo "command is $command <br>";
  $stid = OCIParse($conn,$command);
  ocibindbyname($stid,":out",$out,10);
  $r=OCIExecute($stid,OCI_DEFAULT);
  if($r && ($out>0)) {
    ocicommit($conn);
    return 1;
  }
  else
    return 0;
}

$id=$_POST["id"];
?>
<H2 ALIGN="CENTER">Update Default Display Options for histogram <?php echo $id ?></H2>

<?php
$page= array_key_exists("PAGE",$_POST) ? $_POST["PAGE"] : 0;
if ($page)
  echo "<H2 ALIGN='CENTER'> in Page $page</H2>";

if ($_POST["Update_display"] == 'Confirm') {
  if (update_histo_display())
    echo "Histogram Display Options updated successfully<br><br>\n";
  else
    echo "<font color=red> <B>Got errors from update_histo_display() </B></font><br><br>\n";
} 
else {
  echo "Please check your data and confirm <br><br>";
  histo_display($id,$_POST["htype"],"update");
}

ocilogoff($conn);
echo "<p><a href='../Histogram.php?".strtolower($_POST["htype"])."=${id}'> Back to Histogram Record $id </a></p>";

if ($page) {
  $getp=toGet($page);
  echo "<br> <a href='../Viewpage.php?page=${getp}> Back to Page Record $page </a>";
}

?>

</body>
</html>
