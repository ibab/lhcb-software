<?
include 'util.php';
$conn=HistDBconnect();
if (!$conn) {
  $e = ocierror();
  print htmlentities($e['message']);
  echo "<a href=\"login.html\" target=_parent> Retry login </a>";
}
else {
  $stid = OCIParse($conn,"select COUNT(*) from viewhistogram");
  OCIExecute($stid, OCI_DEFAULT);
  if (OCIFetchInto($stid, $row, OCI_NUM  )) 
    echo "Connected to LHCB OnlineHistDB<br><B>".$row[0]." </B> Histograms stored in the DB<br>\n";
  
  ocilogoff($conn);
 }


?>