<?php 
include '../util.php'; include '../dbforms.php'; 
$conn=HistDBconnect(1);
$id=$_POST["id"];
$name=$_POST["NAME"];
?>
<HTML>
 <HEAD>
<LINK REL=STYLESHEET TYPE="text/css" HREF="../styles_screen.css">
 </HEAD>
<body>
<?php 
function remove_histo_display($doid) {
  global $conn;
  $stid = OCIParse($conn,"delete from DISPLAYOPTIONS where DOID=$doid");
  $r=OCIExecute($stid,OCI_DEFAULT);
  if (!$r) return 0;
  ocifreestatement($stid);
  ocicommit($conn);
  return 1;  
}

function update_histo_display() {
  global $conn;
  global  $DispOpt;
  global $debug;
  $command="begin :out := OnlineHistDB.".
    ( $_POST["htype"] == "SHID" ? 
      "DeclareHistoPageDisplayOptions('".$_POST["id"]."','".$_POST["PAGE"]."',".$_POST["INSTANCE"] :
      ($_POST["htype"] == "HSID" ? 
       "DeclareHistoSetDisplayOptions(".$_POST["id"] :
       "DeclareHistDisplayOptions('".$_POST["id"]."'") );
  $command.=",DISPOPT(";
  $nopts = count($DispOpt);
  for ($i=0 ; $i< $nopts; $i++) {
    $var = $DispOpt[$i];
    $command.= ($_POST[$var] != '') ? "'".$_POST[$var]."'" : "NULL";
    if($i < $nopts-1) $command.=",";
  }
  $command.=")";
  if ($_POST["FITFUN"]) {
    $command.=",'".$_POST["FITFUNNAME"]."',";
    $ffpar=array();
    for ($ip=1 ; $ip<= $_POST["FITNP"]; $ip++) {
      if( strlen($_POST["FITPAR_${ip}"])>0 ) 
        $ffpar[$ip]=$_POST["FITPAR_${ip}"];
      else
        $ffpar[$ip]=-999999.;
    }
    for ($ip=1 ; $ip<= $_POST["FITNINPUT"]; $ip++) {
      if( strlen($_POST["FITINPPAR_${ip}"])>0 ) 
        $ffpar[$_POST["FITNP"]+$ip]=$_POST["FITINPPAR_${ip}"];
      else
        $ffpar[$_POST["FITNP"]+$ip]=-999999.;
    }
    $command .= "vthresholds(".implode(",",$ffpar).")";
    if (strlen($_POST["FITMIN"])>0 && strlen($_POST["FITMAX"])>0) {
      $command.=",".$_POST["FITMIN"].",".$_POST["FITMAX"];
    }
  }
  else {
    $command.=",NULL,vthresholds()";
  }

  $command .= "); END;";
  if ($debug>0) echo "command is $command <br>";
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

function hidtype($Htype) {
  if ($Htype == "HID" || $Htype == "SHID")
    return "hid";
  else
    return "hsid";
}

function fitok() {
  if ($_POST["FITFUN"] == 0) return 1;
  if ($_POST["FITFUN"] ==$_POST["OLDFITFUN"]) return 1;
  return 0;
}

?>
<H2 ALIGN="CENTER">Update Default Display Options for histogram <?php echo $id ?></H2>
<H3 ALIGN="CENTER"><?php echo $name ?></H3>

<?php
$page= array_key_exists("PAGE",$_POST) ? $_POST["PAGE"] : 0;
if ($page)
  echo "<H2 ALIGN='CENTER'> in Page $page</H2>";


if (array_key_exists("Remove_DO",$_POST)) {
  echo "<B>If you confirm, display options for histogram record $id will be removed and lost ";
  if ($_POST["htype"] == 'HSID' && $_POST["NHS"]>1) { 
    echo "<br><font color='red'> for the whole histogram set (".$_POST["NHS"]." histograms)<br></font>";
  }
  echo "</B><br>\n";
  echo "<form action='".$_SERVER["PHP_SELF"]."' method='post'>\n";
  echo "<input type='hidden' name='id' value='${id}'>\n";
  echo "<input type='hidden' name='htype' value='".$_POST["htype"],"'>\n";    
  echo "<input type='hidden' name='doid' value='".$_POST["doid"],"'>\n";
  echo "<input type='submit' name='Really_Remove_DO' value='Confirm Removal of Display Options'>\n";
 }
 else if (array_key_exists("Really_Remove_DO",$_POST)) {
   if (remove_histo_display($_POST["doid"])) 
     echo "Histogram Display options  deleted successfully<br><br>\n";
   else
     echo "<font color=red> <B>Got errors from remove_histo_display() </B></font><br><br>\n";
 }
 else if ($_POST["Update_display"] == 'Confirm' && fitok()) {
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
echo "<hr><p><a href='../Histogram.php?".hidtype($_POST["htype"])."=${id}'> Back to Histogram Record $id </a></p>";

if ($page) {
  $getp=toGet($page);
  echo "<br> <a href='../Viewpage.php?page=${getp}'> Back to Page Record $page </a>";
}

?>

</body>
</html>
