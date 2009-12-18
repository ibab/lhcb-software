<?php 
include '../util.php';
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

if (array_key_exists("Confirm",$_POST)) {
  $page=$_POST["page"];
  $command="delete from displayoptions where doid=".$_POST["doid"];
  $stid = OCIParse($conn,$command);
  $r=OCIExecute($stid,OCI_DEFAULT);
  if (!$r) {
    echo "Something wrong deleting display options<br>";
  }
  else {
    echo "display options successfully deleted";
    ocicommit($conn);
  }
  ocifreestatement($stid);
 }
else {
  $page=fromGet($_GET["page"]);
  echo "<font size=+1> You requested to Delete Special display options for histogram <br>\n";
  echo "<font color=red>".fromGet($_GET["name"])." </font> <br> \n";
  echo "on Page <font color=red >${page} </font></font><br>\n";
  echo "<form action=".$_SERVER[PHP_SELF]." method='post'>";
  echo "<input type='hidden' name='doid' value='".$_GET["doid"]."'>\n";
  echo "<input type='hidden' name='page' value='${page}'>\n";
  echo "<input type='submit' name='Confirm' value='Confirm'>\n";
 }

ocilogoff($conn);
echo "<br> <a href='../Viewpage.php?page=${page}> Back to Page Record $page </a>";

?>
