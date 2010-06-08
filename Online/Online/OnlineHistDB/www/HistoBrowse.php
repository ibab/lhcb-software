<?
include 'util.php';
$conn=HistDBconnect();
$query=0;
$items=array();
if (isset($_GET["bytask"])) {
  echo "Select Task Name<br><br>";
  $parameter="task";
  $ncol=1;
  $query="select TASKNAME from TASK";
}
if (isset($_GET["bysubsys"])) {
  echo "Select Subsystem Name<br><br>";
  $parameter="subsys";
  $ncol=3;
  $query="select SSName from SUBSYSTEM";
}
if (isset($_GET["bypage"])) {
  echo "Select Page Name<br>";
  $parameter="page";
  $ncol=1;
  $query="select PageFolderName from PAGEFOLDER order by PageFolderName";
}
if (isset($_GET["byalpha"])) {
  echo "List histograms with titles beginning with letter<br><br>";
  for ($i = 97; $i < 123; $i++) {
    $items[] = chr($i);
  }
  $parameter="letter";
  $ncol=7;
}
if (isset($_GET["byfeature"])) {
  $items=array('Show Virtual Histograms','Show Histograms with automatic analysis');
  $parameter="feature";
  $ncol=1;
}

if ($query) {
  if($conn) {
    $i=0;
    $stid = OCIParse($conn,$query);
    OCIExecute($stid, OCI_DEFAULT);
    while( OCIFetchInto($stid, $row, OCI_NUM)) 
      $items[$i++]=$row[0];
  }
  else {
    echo "session expired <br><a href='login.html' target=_parent> Login again </a>";
    exit;
  }
}

echo "<table border=3 rules=cols,rows cellpadding=8>";
if ($parameter=="task") echo "<tr><td><b>Task</b><td><b>Algorithm</b></tr>\n";
if ($parameter=="page") echo "<tr><td><b>Page Folder</b><td><b>Page</b></tr>\n";
$pos=0;
foreach  ($items as $it) {
  if($pos == 0) echo "<tr>";
  $getit=toGet($it);
  if ($parameter=="page")
    echo "<td> <span class=normal> ${it}</span>&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp </td> ";
  else
    echo "<td> <a href='HistogramList.php?${parameter}=${it}'> ${it}</a> &nbsp&nbsp&nbsp&nbsp&nbsp&nbsp </td> ";
  if ($parameter=="task") { // put also algorithm links
    $stid2 = OCIParse($conn,"select min(HSALGO) from HISTOGRAMSET where HSTASK='$it' group by HSALGO");
    OCIExecute($stid2, OCI_DEFAULT);
    echo "<td><table>";
    while( OCIFetchInto($stid2, $row, OCI_NUM)) 
      echo "<tr><td> <a href='HistogramList.php?algo=${it}/$row[0]'> ${it}/$row[0] </a> &nbsp&nbsp&nbsp&nbsp&nbsp&nbsp </td></tr> ";
    echo "</table></td>";
    ocifreestatement($stid2);
  }
  if ($parameter=="page") { // put also page links
    $stid2 = OCIParse($conn,"select PageName from PAGE where FOLDER='$it'");
    OCIExecute($stid2, OCI_DEFAULT);
    echo "<td><table>";
    while( OCIFetchInto($stid2, $row, OCI_NUM)) 
      echo "<tr><td> <a href='HistogramList.php?page=$it/$row[0]'> $row[0] </a> &nbsp&nbsp&nbsp&nbsp&nbsp&nbsp </td></tr> ";
    echo "</table></td>";
    ocifreestatement($stid2);
  }
  if(++$pos == $ncol) {
    echo "</tr>\n";
    $pos=0;
  }
}
if ($query) {
  ocifreestatement($stid);
  ocilogoff($conn);
}
echo "</table>\n";

?>