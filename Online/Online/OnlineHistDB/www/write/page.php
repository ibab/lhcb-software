<?php 
include '../util.php';
$conn=HistDBconnect(1);
?>
<HTML>
 <HEAD>
<LINK REL=STYLESHEET TYPE="text/css" HREF="../styles_screen.css">
 </HEAD>
<body>
<?php 
$folder= ($_POST["NEWFOLDER"]) ? $_POST["NEWFOLDER"] : $_POST["FOLDER"];
$page=RemoveSpaces($_POST["SHORTPAGENAME"]);
$newpagename="$folder/$page";
if (array_key_exists("ORIGINALNAME",$_POST)) {
  $fullpage=$_POST["ORIGINALNAME"];
 }
 else {
   $fullpage=$newpagename;
 }


$nh=0;
for ($i=1;$i<=$_POST["maxH"] ;$i++) {
  if  ($_POST["HISTO_SH${i}"] && $_POST["REMOVE${i}"] != 1 ) {
    $nh++;
    foreach (array("HISTO","CENTER_X","CENTER_Y","SIZE_X","SIZE_Y","SDISPLAY","MOTHERH","IOVERLAP")
	     as $field)
      $data[$field][$nh]=$_POST[$field."_SH${i}"];
  }
}

$mainfields = "theFullName => '${fullpage}', theName => :pn,theFolder => :pf";
if ($_POST["PAGEPATTERN"]) 
  $mainfields .= ",thePattern => '".$_POST["PAGEPATTERN"]."'";
if ($_POST["PAGEDOC"]) 
  $mainfields .= ",theDoc => '".$_POST["PAGEDOC"]."'";
if ($nh == 0)
  $command="begin :fn := OnlineHistDB.DeclarePage(${mainfields} ".
    ",hlist => OnlineHistDB.histotlist()".
    ",Cx => OnlineHistDB.floattlist()".
    ",Cy => OnlineHistDB.floattlist()".
    ",Sx => OnlineHistDB.floattlist()".
    ",Sy => OnlineHistDB.floattlist()); end;";
else {
  $command="begin :fn := OnlineHistDB.DeclarePage(${mainfields} ".
    ",hlist => OnlineHistDB.histotlist('".implode("','",SingleHist($data["HISTO"]))."')".
    ",Cx => OnlineHistDB.floattlist(".implode(",",$data["CENTER_X"]).")".
    ",Cy => OnlineHistDB.floattlist(".implode(",",$data["CENTER_Y"]).")".
    ",Sx => OnlineHistDB.floattlist(".implode(",",$data["SIZE_X"]).")".
    ",Sy => OnlineHistDB.floattlist(".implode(",",$data["SIZE_Y"]).")".
    ",theOverlap => inttlist(".implode(",",$data["MOTHERH"]).")".
    ",theOvOrder => inttlist(".implode(",",$data["IOVERLAP"]).")".
    "); end;";
}
if($debug) echo "command is $command <br>";
$stid = OCIParse($conn,$command);
ocibindbyname($stid,":fn",$outpage,500);
ocibindbyname($stid,":pn",$out_pn,500);
ocibindbyname($stid,":pf",$out_pf,500);
$r=OCIExecute($stid,OCI_DEFAULT);
if ($r) {
  ocicommit($conn);
  ocifreestatement($stid);
  
  if ($newpagename != $fullpage && $newpagename != $outpage) { //also change page name
    $command="begin :fn := OnlineHistDB.RenamePage(oldName => '${fullpage}', newName => '${newpagename}',".
      "newFolder => :nf); end;";

    $stid = OCIParse($conn,$command);
    ocibindbyname($stid,":nf",$outfolder,500);
    ocibindbyname($stid,":fn",$outpage,500);
    $r=OCIExecute($stid,OCI_DEFAULT);
    if ($r) {
      ocicommit($conn);
      ocifreestatement($stid);
      $fullpage = $newpagename;
    }
    else {
      echo "<font color=red> <B>Got errors from page.php when renaming page </B></font><br>Renaming aborted<br>\n";
    }
  }
  if ($outpage !=$fullpage) {
    echo "<font color=magenta> Warning: page name has been changed from <br> $fullpage to <br>$outpage<br>\n";
    $fullpage = $outpage;
  }
  echo "Page $fullpage successfully updated<br>";
 }
 else {
   echo "<font color=red> <B>Got errors from page.php when updating page </B></font><br>Writing aborted<br>\n";
 }



ocilogoff($conn);
$gpage=toGet($fullpage);
echo "<br> <a href='../Viewpage.php?page=${gpage}> Back to Page Record ${fullpage} </a><br>";
?>

</body>
</html>
