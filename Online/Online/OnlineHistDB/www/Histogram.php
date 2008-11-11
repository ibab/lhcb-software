<HTML>
 <HEAD>
 <LINK REL=STYLESHEET TYPE="text/css" HREF="styles_screen.css">
</HEAD>
<body class=listing>
<?
$debug=0;
include 'util.php';
include 'dbforms.php';
// show histogram set or single histogram depending on the GET input variable that specifies ID: hsid for set, hid for single
if (array_key_exists("hsid",$_GET)) {
  $id=$_GET["hsid"];
  $htype='HSID';
}
if (array_key_exists("hid",$_GET)) {
  $id=$_GET["hid"];
  $htype='HID';
}
$fulllist =array_key_exists("fulllist",$_GET);
if(isset($htype) ) {
  $conn=HistDBconnect(1);
  // query general variables
  $query="select VH.NAME,T.TASKNAME,T.SUBSYS1,T.SUBSYS2,T.SUBSYS3,T.REFERENCE,HS.HSID,HS.NHS,HS.HSALGO,HS.HSTITLE,HS.HSTYPE,HS.NANALYSIS,HS.DESCR,HS.DOC,HS.HSDISPLAY,H.HID,H.IHS,H.SUBTITLE,H.ISTEST,H.ISANALYSISHIST,H.CREATION,H.OBSOLETENESS,H.DISPLAY,TO_CHAR(H.CREATION,'DD-Mon-YYYY') CRE_DATE,H.NBINLABX,H.NBINLABY from VIEWHISTOGRAM VH,HISTOGRAM H,HISTOGRAMSET HS, TASK T where H.HSET=HS.HSID and HS.HSTASK=T.TASKNAME and VH.HID=H.HID AND VH.${htype}='${id}' order by H.IHS";
  if ($debug) echo "query is $query <br>\n";
  $stid = OCIParse($conn,$query);
  OCIExecute($stid);
  OCIFetchInto($stid, $histo, OCI_ASSOC );
  if (ocirowcount($stid) == 0) {
    echo "Histogram $id is not known to DB";
  }
  else {
    if($htype=='HID' && $histo["NHS"]==1) {
      // single histogram  set: just use the set record and never mind
      $htype='HSID';
      $id=HistoSet($id);
    }
    echo "<center><H2> Histogram Record $id </H2>";
    echo "<table border=1 cellpadding=8>";
    //header
    echo "<tr id=HEADER><td>";
    histo_header($id,$htype,"display");    
    echo "</td></tr>";
    
    //Display Options
    echo "<tr id=DISPLAY><td><H3> Display Options </H3><br>\n";
    if ($histo["DISPLAY"] && $htype == 'HID' || $histo["HSDISPLAY"]) 
      histo_display($id,$htype,"display");
    else {
      echo "No Display Options";
      if( $canwrite) {
	echo "<br><form method='post' action='write/histo_display.php'>";
	printf("<input type='hidden' name='id' value='%s'>\n",$id);
	printf("<input type='hidden' name='htype' value='%s'>\n",$htype);
	printf("<input type='hidden' name='NHS' value='%s'>\n",$histo["NHS"]);
	printf("<input type='hidden' name='HSTYPE' value='%s'>\n",$histo["HSTYPE"]);
	printf("<input type='hidden' name='TASKNAME' value='%s'>\n",$histo["TASKNAME"]);
	echo "<table align=right><tr><td><input type='submit' name='New' value='Specify Display Option'></tr></table>";
	echo "</form>";
      }
    }
    echo "</td></tr>\n";
    
    // Custom Bin Labels
    echo "<tr id=DISPLAY><td><H3> Custom Bin Labels </H3><br>\n";
    if ($histo["NBINLABX"]+$histo["NBINLABY"] > 0)
      histo_labels($id,$htype,"display");
    else {
      echo "No Custom Bin Labels";
      if( $canwrite) {
	echo "<br><form method='post' action='write/histo_labels.php'>";
	printf("<input type='hidden' name='id' value='%s'>\n",$id);
	printf("<input type='hidden' name='htype' value='%s'>\n",$htype);
	printf("<input type='hidden' name='NHS' value='%s'>\n",$histo["NHS"]);
	echo "<table align=right><tr><td><input type='submit' name='New' value='Specify Custom Bin Labels'></tr></table>";
	echo "</form>";
      }
    }
    echo "</td></tr>\n";

    //Automatic Analysis
    echo "<tr id=ANALYSIS><td><H3> Automatic Analysis </H3><br>\n";
    if ($histo["NANALYSIS"]>0) 
      histo_analysis($id,$htype,"display");
    else 
      echo "No Automatic Analysis   ";
    if( $canwrite && $htype=='HSID') {
      echo "<br><form method='post' action='write/histo_analysis.php'>";
      printf("<input type='hidden' name='id' value='%s'>\n",$id);
      printf("<input type='hidden' name='htype' value='%s'>\n",$htype);
      printf("<input type='hidden' name='NHS' value='%s'>\n",$histo["NHS"]);
      echo "<table align=right><tr><td><input type='submit' name='New' value='Add Automatic Analysis'></tr></table>";
      echo "</form>";
    }
    echo "</td></tr>\n";
    
    echo "</table></center>"; 
  }
  ocifreestatement($stid);
  ocilogoff($conn);
  
  
}
else {
  echo "No histogram ID specified";
}
?>
</body></html>
