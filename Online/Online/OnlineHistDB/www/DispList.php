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
?>
<HTML>
 <HEAD>
 <LINK REL=STYLESHEET TYPE="text/css" HREF="styles_screen.css">
</HEAD>
<body class=listing>
<?
$settitle=$_POST["TASKNAME"]."/".$_POST["HSALGO"]."/".$_POST["HSTITLE"];
$stid = OCIParse($conn,"select HID,SUBTITLE,DISPLAY from HISTOGRAM where HSET='".$_POST["hsid"]."' order by IHS");
OCIExecute($stid);

echo "<center><H2> Display Options ".
     " for <a href=Histogram.php?hsid=".$_POST["hsid"].">Histogram set ".$_POST["hsid"]."</a>:<br>$settitle </H2>\n";
echo "<table border=1 cellpadding=8>";

foreach (array("TASKNAME","HSALGO","HSTYPE","HSTITLE","NHS") as $field)
     $histo[$field]=$_POST[$field];

echo "<tr><td align='left'><H3>Default Options for Histogram Set </H3></td><td>\n";  
// show default options for set
if ($_POST["HSDISPLAY"] ) {
  // in this case histo_display will get options from $histo["HSDISPLAY"]
  $histo["HSDISPLAY"]=$_POST["HSDISPLAY"];
  histo_display($_POST["hsid"],"HSID","displaylist");
}
else {
  echo "No Default Display Options<br>";
  if( $canwrite) {
    echo "<form method='post' action='write/histo_display.php'>";
    printf("<input type='hidden' name='id' value='%s'>\n",$_POST["hsid"]);
    printf("<input type='hidden' name='htype' value='%s'>\n","HSID");
    printf("<input type='hidden' name='NHS' value='%s'>\n",$_POST["NHS"]);
    printf("<input type='hidden' name='HSTYPE' value='%s'>\n",$_POST["HSTYPE"]);
    echo "<table align=right><tr><td><input type='submit' name='New' value='Specify Display Option for single histogram'></tr></table>";
    echo "</form>";
  }
}

// loop on histograms in set and show specific options 
while (OCIFetchInto($stid, $subhisto, OCI_ASSOC )) {
  echo "<tr><td align='left'><H4>".$subhisto["SUBTITLE"]." </H4></td><td>\n";  
  if ($subhisto["DISPLAY"] ) {
    // in this case histo_display will get options from $histo["DISPLAY"]
    $histo["DISPLAY"]=$subhisto["DISPLAY"];
    histo_display($subhisto["HID"],"HID","displaylist");
  }
  else {
    echo "Using Default Display Options (if any)<br>";
    if( $canwrite) {
      echo "<form method='post' action='write/histo_display.php'>";
      printf("<input type='hidden' name='id' value='%s'>\n",$subhisto["HID"]);
      printf("<input type='hidden' name='htype' value='%s'>\n","HID");
      printf("<input type='hidden' name='NHS' value='%s'>\n",$_POST["NHS"]);
      printf("<input type='hidden' name='HSTYPE' value='%s'>\n",$_POST["HSTYPE"]);
      if ($_POST["HSDISPLAY"]) { // use options for set as default
	get_displayoptions($_POST["HSDISPLAY"]);
	hidpost_displayoptions();
      }
      echo "<table align=right><tr><td><input type='submit' name='New' value='Specify Display Option for single histogram'></tr></table>";
      echo "</form>";
    }
  }
  echo "</tr></td>\n";
}

echo "</table>";
ocifreestatement($stid);
ocilogoff($conn);
?>
</body></html>