<?
include 'util.php';
$conn=HistDBconnect();
echo "<html><body><table><thead>";
echo "<tr><td><B>Function name</B></td><td><B>Description</B></td></tr></thead>";
$stid = OCIParse($conn,"SELECT NAME,DOC from FITFUNCTION order by NAME");
OCIExecute($stid);
echo "<HTML>
<body class=listing>\n";
while (OCIFetchInto($stid,$myfun,OCI_ASSOC )) {
  echo "<tr><td>".$myfun["NAME"]."</td><td>".$myfun["DOC"]."</td></tr>";
 }
echo "</table>";
?>
</body></html>