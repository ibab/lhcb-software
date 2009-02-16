<HTML>
<body class=listing>
<?
include 'util.php';
echo "<table><thead>";
echo "<tr><td><B>Function name</B></td><td><B>Description</B></td></tr></thead>";
$conn=HistDBconnect();
$stid = OCIParse($conn,"SELECT NAME,DOC from FITFUNCTION order by NAME");
OCIExecute($stid);
while (OCIFetchInto($stid,$myfun,OCI_ASSOC )) {
  echo "<tr><td>".$myfun["NAME"]."</td><td>".$myfun["DOC"]."</td></tr>";
 }
echo "</table>";
?>
</body></html>