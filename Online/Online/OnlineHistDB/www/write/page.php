<HTML>
 <HEAD>
<LINK REL=STYLESHEET TYPE="text/css" HREF="../styles_screen.css">
 </HEAD>
<body>
<?php 
include '../util.php';
$conn=HistDBconnect(1);
$page=RemoveSpaces($_POST["PAGENAME"]);
$folder= ($_POST["NEWFOLDER"]) ? $_POST["NEWFOLDER"] : $_POST["FOLDER"];

$nh=0;
for ($i=1;$i<=15;$i++) {
  if  ($_POST["HISTO_SH${i}"]) {
    $nh++;
    foreach (array("HISTO","CENTER_X","CENTER_Y","SIZE_X","SIZE_Y","SDISPLAY")
	     as $field)
      $data[$field][$nh]=$_POST[$field."_SH${i}"];
  }
}
if ($nh == 0)
  echo "No histograms specified.. writing aborted<br>";
else {
  $command="begin OnlineHistDB.DeclarePage(theName => '${page}',theFolder => '${folder}',theDoc => '".$_POST["PAGEDOC"]."',".
    "hlist => OnlineHistDB.histotlist('".implode("','",SingleHist($data["HISTO"]))."'),".
    "Cx => OnlineHistDB.floattlist(".implode(",",$data["CENTER_X"])."),".
    "Cy => OnlineHistDB.floattlist(".implode(",",$data["CENTER_Y"])."),".
    "Sx => OnlineHistDB.floattlist(".implode(",",$data["SIZE_X"])."),".
    "Sy => OnlineHistDB.floattlist(".implode(",",$data["SIZE_Y"]).")); end;";
  //echo "command is $command <br>";
  $stid = OCIParse($conn,$command);
  $r=OCIExecute($stid,OCI_DEFAULT);
  if ($r) {
    ocicommit($conn);
    ocifreestatement($stid);
    echo "Page $page successfully defined<br>";
  }
  else
    echo "<font color=red> <B>Got errors from page.php </B></font><br>Writing aborted<br>\n";
}


ocilogoff($conn);
$gpage=toGet($page);
echo "<br> <a href='../Viewpage.php?page=${gpage}> Back to Page Record ${page} </a><br>";
?>

</body>
</html>
