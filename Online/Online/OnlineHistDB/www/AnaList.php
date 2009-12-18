<?
$debug=0;
include 'util.php';
include 'dbforms.php';
$conn=HistDBconnect();
if (!$conn) {
  $e = ocierror();
  print htmlentities($e['message']);
  exit;
}

$settitle=$_POST["TASKNAME"]."/".$_POST["HSALGO"]."/".$_POST["HSTITLE"];
$stid = OCIParse($conn,"select HID,SUBTITLE from HISTOGRAM where HSET='".$_POST["hsid"]."' order by IHS");
OCIExecute($stid);
?>
<HTML>
 <HEAD>
 <LINK REL=STYLESHEET TYPE="text/css" HREF="styles_screen.css">
</HEAD>
<body class=listing>
  <?
echo "<center><H2> Parameters of Automatic Analysis ".
     " for <a href=Histogram.php?hsid=".$_POST["hsid"].">Histogram set ".$_POST["hsid"]."</a>:<br>$settitle </H2>\n";
echo "<table border=1 cellpadding=8>";

foreach (array("HSTITLE","NHS","NANALYSIS") as $field)
     $histo[$field]=$_POST[$field];
while (OCIFetchInto($stid, $subhisto, OCI_ASSOC )) {
  echo "<tr><td align='left'><H4>".$subhisto["SUBTITLE"]." </H4></td><td>\n";
  histo_analysis($subhisto["HID"],"HID","display");
  echo "</tr></td>\n";
}

echo "</table>";
ocifreestatement($stid);
ocilogoff($conn);
?>
</body></html>