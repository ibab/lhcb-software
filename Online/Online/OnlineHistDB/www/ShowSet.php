<?
include 'util.php';
$id=$_GET["id"];
$conn=HistDBconnect();
?>
<HTML>
 <HEAD>
 <LINK REL=STYLESHEET TYPE="text/css" HREF="styles_screen.css">
</HEAD>
<body class=listing>
<?
if (!$conn) {
  $e = ocierror();
  print htmlentities($e['message']);
  exit;
}
$query="select H.HID,HS.HSTITLE,H.SUBTITLE from HISTOGRAM H,HISTOGRAMSET HS where H.HSET=HS.HSID and  H.HSET=:id order by IHS";
$stid = OCIParse($conn,$query);
ocibindbyname($stid,':id', $id);
OCIExecute($stid, OCI_DEFAULT);
$ihs=1; 
while (OCIFetchInto($stid, $list, OCI_ASSOC)) {
  if ($ihs==1){
    echo "<center>Histogram set <span class=normal> $id </span>: <span class=normal> &nbsp ".$list["HSTITLE"]."</span><br>\n";
    echo "<table  rules='cols' border=1><thead><tr><td><B>HID </B><td><B>Subtitle</B></tr></thead><tbody>\n";
  } 
  echo "<td><a href='Histogram.php?hid='".$list["HID"]."'>".$list["HID"]."</a><td>".$list["SUBTITLE"]."</tr>\n";
  $ihs++;
} 
echo "</tbody></table>";
?>
</body></html>