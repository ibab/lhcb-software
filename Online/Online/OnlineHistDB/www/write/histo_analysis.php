<?php 
include '../util.php'; include '../dbforms.php'; 
$conn=HistDBconnect();
$id=$_POST["id"];
$ia=$_POST["Iana"];
$name=$_POST["NAME"];
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

function update_histo_analysis() {
  global $conn,$id;
  global $debug;

  $ia=$_POST["Iana"];
  $aid=$_POST["a${ia}_id"];
  $warn=$alr=$inps=array();
  $out=0;
  for ($ip=1;$ip<=$_POST["a${ia}_np"];$ip++) {
    $warn[$ip]=$_POST["a${ia}_p${ip}_w"];
    $alr[$ip]=$_POST["a${ia}_p${ip}_a"];
  }
  for ($ip=1;$ip<=$_POST["a${ia}_ni"];$ip++) {
    if($debug)
      echo "inp parameters $ip=".$_POST["a${ia}_i${ip}_v"]." length=".strlen($_POST["a${ia}_i${ip}_v"])."<br>\n";
    if(strlen($_POST["a${ia}_i${ip}_v"])>0) 
      $inps[$ip]=$_POST["a${ia}_i${ip}_v"];
    else
      break;
  }
  $warnings="vthresholds(".implode(",",$warn).")";
  $alarms="vthresholds(".implode(",",$alr).")";
  $inputs= "vthresholds(".implode(",",$inps).")";

  $Docs="";
  if ($_POST["a${ia}_doc"]) {
    $Docs .= ",Doc => '".sqlstring($_POST["a${ia}_doc"])."'";
  }
  if ($_POST["a${ia}_mes"]) {
    $Docs .= ",Message => '".sqlstring($_POST["a${ia}_mes"])."'";
  }

  $statusBits=0;
  for ($b=0;$b<32;$b++) {
    if ($_POST["a${ia}_stb_${b}"]) {
      $statusBits += pow(2,$b);
    }
  }
  $statCond=$upCond="";
  if (strlen($_POST["a${ia}_mst"])>0) {
    $statCond .= ",theMinStat => ".$_POST["a${ia}_mst"];
    $upCond .= ",MINBINSTAT=".$_POST["a${ia}_mst"];
  }
  if (strlen($_POST["a${ia}_msf"])>0) {
    $statCond .= ",theMinStatFrac => ".$_POST["a${ia}_msf"];
    $upCond .= ",MINBINSTATFRAC=".$_POST["a${ia}_msf"];
  }

  if ( $_POST["htype"] == "HSID") {
    $command="begin :out := OnlineHistDB.DeclareAnalysisWithID(".$_POST["id"].",'".$_POST["a${ia}_alg"]."',$warnings,$alarms,$aid,$inputs$Docs,theBits => $statusBits $statCond); end;";
    $gAna=1;
  }
  else { // just update settings for this single histogram
    $command= "update ANASETTINGS set VWARNINGS=$warnings,VALARMS=$alarms,VINPUTPARS=$inputs,STATUSBITS=${statusBits}${upCond} ".
      "where ANA=$aid and HISTO='$id'";
    $gAna=0; $out=1;
  }  


  if($debug) echo "command is $command<br>\n";
  $stid = OCIParse($conn,$command);
  if ($gAna)
    ocibindbyname($stid,":out",$out,10);
  $r=OCIExecute($stid,OCI_DEFAULT);
  if (!$out || !$r) return 0;
  ocicommit($conn);
  ocifreestatement($stid);
  return 1;  
}

function mask_histo_analysis($action) {
  global $conn,$id;
  $newmask= ($action=="Unmask") ? 0 : 1;
  $ia=$_POST["Iana"];
  $aid=$_POST["a${ia}_id"];
  $command= "update ANASETTINGS set MASK=$newmask where ANA=$aid and ".
      ( $_POST["htype"] == "HID" ? "HISTO='$id'" : "REGEXP_REPLACE(HISTO,'^(.*)/.*\$','\\1')=$id");

  $stid = OCIParse($conn,$command);
  $r=OCIExecute($stid,OCI_DEFAULT);
  if (!$r) return 0;
  ocicommit($conn);
  ocifreestatement($stid);   
  return 1;  
}

function remove_histo_analysis($aid,$hset) {
  global $conn;
  $stid = OCIParse($conn,"delete from ANALYSIS where AID=$aid and HSET=$hset");
  $r=OCIExecute($stid,OCI_DEFAULT);
  if (!$r) return 0;
  ocifreestatement($stid);
  $stid = OCIParse($conn,"UPDATE HISTOGRAMSET set NANALYSIS=NANALYSIS-1 where HSID=$hset");
  $r=OCIExecute($stid,OCI_DEFAULT);
  if (!$r) return 0;
  ocifreestatement($stid);
  $stid = OCIParse($conn,"begin ONLINEHISTDB.CleanPadColors; end;");
  $r=OCIExecute($stid,OCI_DEFAULT);
  if (!$r) return 0;
  ocifreestatement($stid);
  ocicommit($conn);
  return 1;  
}

?>
<H2 ALIGN="CENTER">Add/Update Automatic Analysis for histogram <?php echo $id ?></H2>
<H3 ALIGN="CENTER"><?php echo $name ?></H3><hr>

<?php

if (array_key_exists("New",$_POST)) {
  histo_analysis($id,$_POST["htype"],"new");
}
else if (array_key_exists("SetMask_Analysis",$_POST)) {
  if (mask_histo_analysis($_POST["SetMask_Analysis"]))
    echo "Histogram Analysis ".$_POST["a${ia}_alg"]." ".$_POST["SetMask_Analysis"]."ed successfully<br><br>\n";
  else
    echo "<font color=red> <B>Got errors from mask_histo_analysis() </B></font><br><br>\n";
}
else if (array_key_exists("Remove_Analysis",$_POST)) {
  if ($_POST["htype"] == 'HSID') { // be sure command comes from a set
    echo "If you confirm, Analysis ".$_POST["a${ia}_alg"]." will be removed and settings of parameters will be lost<br>\n";
    if($_POST["NHS"]>1)
      echo "<B> for all the ".$_POST["NHS"]." histograms of set<br>\n";
    echo "<form action='".$_SERVER["PHP_SELF"]."' method='post'>\n";
    echo "<input type='hidden' name='id' value='${id}'>\n";
    echo "<input type='hidden' name='htype' value='".$_POST["htype"],"'>\n";    
    echo "<input type='hidden' name='algo' value='".$_POST["a${ia}_alg"],"'>\n";
    echo "<input type='hidden' name='aid' value='".$_POST["a${ia}_id"],"'>\n";
    echo "<input type='submit' name='Really_Remove_Analysis' value='Confirm Removal of Analysis'>\n";
  }
}
else if (array_key_exists("Really_Remove_Analysis",$_POST)) {
  if (remove_histo_analysis($_POST["aid"],$id)) 
    echo "Histogram Analysis ".$_POST["algo"]." deleted successfully<br><br>\n";
  else
    echo "<font color=red> <B>Got errors from remove_histo_analysis() </B></font><br><br>\n";
}
else if ($_POST["Update_analysis"] == 'Confirm') {
  if (update_histo_analysis())
    echo "Histogram Analysis updated successfully<br><br>\n";
  else
    echo "<font color=red> <B>Got errors from update_histo_analysis() </B></font><br><br>\n";
} 
else if ( ($_POST["Update_analysis"] == 'Set Parameters' && $_POST["a1_alg"] != 'Fit' ) ||
          ($_POST["Update_analysis"] == 'Set Fit Parameters') ) {
  histo_analysis($id,$_POST["htype"],"newana");
}
else if ( $_POST["Update_analysis"] == 'Set Parameters' && $_POST["a1_alg"] == 'Fit' ) {
  histo_analysis($id,$_POST["htype"],"newfit");
}
else {
  echo "Please check your data and confirm <br><br>";
  histo_analysis($id,$_POST["htype"],"update");
}

ocilogoff($conn);
echo "<br> <a href='../Histogram.php?".strtolower($_POST["htype"])."=${id}'> Back to Histogram Record $id </a><br>";
?>

</body>
</html>
