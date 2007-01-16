<HTML>
 <HEAD>
<LINK REL=STYLESHEET TYPE="text/css" HREF="../styles_screen.css">
 </HEAD>
<body>
<?php 
include '../util.php';
$conn=HistDBconnect(1);
$nin=$_POST["nin"];
if ($nin ==0) {
  $sourceh=$_POST["SOURCESET"];
}
else {
  $sh=array();
  for ($ih=1;$ih<=$nin;$ih++) 
    $sh[$ih]=SingleHist($_POST["SOURCEH${ih}"]);
  $sourceh="0,OnlineHistDB.sourceh('".implode("','",$sh)."')";
}
$command="begin OnlineHistDB.DeclareAnalysisHistogram('".$_POST["alg"].
  "','".sqlstring($_POST["hctitle"])."',${sourceh}); end;";
// echo "command is $command <br>";
$stid = OCIParse($conn,$command);
$r=OCIExecute($stid,OCI_DEFAULT);
if ($r) {
  ocicommit($conn);
  ocifreestatement($stid);
  $stid = OCIParse($conn,"select HID from viewhistogram where TASK='ANALYSIS' and ALGO='".$_POST["alg"].
		   "' and TITLE='".sqlstring($_POST["hctitle"])."'");
  $r=OCIExecute($stid,OCI_DEFAULT);
  if ($r) $r=OCIFetchInto($stid, $h, OCI_ASSOC);
} 
if (!$r)
  echo "<font color=red> <B>Got errors from create_AnalysisHist.php </B></font><br>Writing aborted<br>Check that your input Histograms exist and that the histogram name is not already in use<br>\n";
else {
  echo "Analysis Histogram created successfully<br>";
  echo "<a href=../Histogram.php?hsid=".HistoSet($h["HID"])."> Go to its record </a>";
}
ocifreestatement($stid);
ocilogoff($conn);
?>
</body></html>
