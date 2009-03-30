<HTML>
<body class=listing>
<?
include 'util.php';
echo "<table border=1><thead>";
echo "<tr><td><B>Algorithm name</B></td><td># Input <br>Parameters</td><td># Output <br>Parameters<td><B>Description</B></td></tr></thead>";
$conn=HistDBconnect();
$stid = OCIParse($conn,"SELECT ALGNAME,NINPUT,NPARS,ALGDOC from ALGORITHM where algtype='CHECK' order by ALGNAME");
OCIExecute($stid);
while (OCIFetchInto($stid,$myfun,OCI_ASSOC )) {
  echo "<tr><td>".$myfun["ALGNAME"]."</td><td>".
    $myfun["NINPUT"]."</td><td>".$myfun["NPARS"]."</td><td>".
    $myfun["ALGDOC"]."</td></tr>";
 }
echo "</table>";
?>
</body></html>

