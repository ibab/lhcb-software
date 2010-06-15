<?
include 'util.php';
$conn=HistDBconnect(0,"HIST_READER","reader");
$type=$_GET["type"];
echo "<table border=1><thead>";
$inputdoc= ($type == "HCREATOR") ? "Input <br> Histograms" : "Input <br>Parameters";
$outputdoc= ($type == "HCREATOR") ? "Parameters" : "Output <br>Parameters";
echo "<tr><td><B>Algorithm name</B></td><td># $inputdoc <br></td><td># $outputdoc <td><B>Description</B></td></tr></thead>";
$stid = OCIParse($conn,"SELECT ALGNAME,NINPUT,NPARS,ALGDOC from ALGORITHM where ALGTYPE='$type' order by ALGNAME");
OCIExecute($stid);
echo "<HTML>
<body class=listing>\n";
while (OCIFetchInto($stid,$myfun,OCI_ASSOC )) {
  echo "<tr><td>".$myfun["ALGNAME"]."</td><td>".
    $myfun["NINPUT"]."</td><td>".$myfun["NPARS"]."</td><td>".
    $myfun["ALGDOC"]."</td></tr>";
 }
echo "</table>";
ocilogoff($conn);
?>
</body></html>