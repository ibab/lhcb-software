<?
include 'dispopt.php';

function show_anahist($id) {
 global $conn;
 $stid = OCIParse($conn,"SELECT HCID,ALGORITHM,SOURCEH1,SOURCEH2,SOURCEH3,SOURCEH4,SOURCEH5,SOURCEH6,SOURCEH7,SOURCEH8,SOURCESET from HCREATOR where HCID='".SingleHist($id)."'");
 OCIExecute($stid, OCI_DEFAULT);
 OCIFetchInto($stid, $hcreator, OCI_ASSOC );
 echo "<p>This is a <B> Display Histogram </B> obtained automatically by Analysis Task<br>
  through Algorithm <span class=normal>".$hcreator["ALGORITHM"]."</span>\n ";
 if ($shsid=$hcreator["SOURCESET"]) 
   echo " from histogram set <a href='Histogram.php?hsid=${shsid}'> ${shsid} </a>\n";
 $ssh=0;
 $ih=0;
 $ssh_string= " from histograms<br><center>\n"; 
 while($ih++<8) {
   if($shid=$hcreator["SOURCEH".$ih])  {
     $ssh_string.="<a href='Histogram.php?hid=${shid}'> ${shid} </a><br>\n";
     $ssh=1;
   }
 }
 $ssh_string.="</center>";
 if($ssh) {
   if($shsid) echo " and ";
   echo $ssh_string;
 }
 else {
   echo "<br>\n";
 }
 get_ana_parameters($hcreator["ALGORITHM"],"HCREATOR",0,1,SingleHist($id),0,1,0,1);
 if(($np=$_POST["a1_np"])>0) {
   $ip=0;
   echo " with parameters <br><center>\n"; 
   while($ip++<$np) 
     echo $_POST["a1_p${ip}_name"]." &nbsp&nbsp = &nbsp&nbsp ".$_POST["a1_p${ip}"]."<br>\n";
   echo "</center>";
 }
 

}

function histo_header($id,$htype,$mode)
{
  global $canwrite;
  global $histo;
  global $debug;
  global $conn;
  global $fulllist;
  global $Reference_home;
  $script=$_SERVER[PHP_SELF];
  if($mode == "display") {
    $script='write/histo_header.php';
    foreach (array("DESCR","DOC","TASKNAME","HSALGO","HSTYPE","HSTITLE","SUBTITLE","NHS","NAME") 
	     as $field)
      $_POST[$field]=$histo[$field];
  }
  $identifier= ($htype == "HSID" && $_POST["NHS"] > 1) ? $_POST["TASKNAME"]."/".$_POST["HSALGO"]."/".$_POST["HSTITLE"]
    : $_POST["NAME"] ;
  echo "<form action='${script}' method='POST'>\n"; 
  echo " ID <span class=normal>$id</span> &nbsp&nbsp&nbsp Task <a class=normal href='Task.php?task=".$_POST["TASKNAME"]."'>".$_POST["TASKNAME"]."</a>".
    " &nbsp&nbsp&nbsp Algorithm <span class=normal>".$_POST["HSALGO"]."</span>".
    " &nbsp&nbsp&nbsp Type <span class=normal>".$_POST["HSTYPE"]."</span><br>\n".    
    " Name <span class=normal>".$_POST["HSTITLE"].
    (($htype == "HID" || $_POST["NHS"] == 1) ? " ".$_POST["SUBTITLE"] : "").
    " </span><br>Identifier   <span class=normal>".$identifier.
    "</span><br><br>\n";
  foreach (array("TASKNAME","HSALGO","HSTYPE","HSTITLE","SUBTITLE","NHS") as $field)
    echo "<input type='hidden' name='${field}' value='".$_POST[$field]."'>\n";
  echo "<input type='hidden' name='id' value='${id}'>\n";
  echo "<input type='hidden' name='htype' value='${htype}'>\n";

  if($mode == "display") {
    $detectors= ($histo["SUBSYS1"] or $histo["SUBSYS2"] or  $histo["SUBSYS3"])  ?
      (($histo["SUBSYS1"] ? $histo["SUBSYS1"] : "")." ".
       ($histo["SUBSYS2"] ? $histo["SUBSYS2"] : "")." ".
       ($histo["SUBSYS3"] ? $histo["SUBSYS3"] : "")) : "none";
    echo "Related detectors: <B>${detectors}</B><br>";
    if($histo["REFERENCE"])
      echo "Latest Reference histogram available in ${Reference_home}/".$histo["REFERENCE"]."<br>\n";
    else
      echo "No Reference histogram available<br>";
    echo "Created on <span class=normal>".$histo["CRE_DATE"]."</span><br>\n";
    if($histo["OBSOLETENESS"])
      echo "Obsolete from <span class=normal>".$histo["OBSOLETENESS"]."</span><br>\n";

    $nhs=$histo["NHS"];
    if($histo["ISANALYSISHIST"] && ($htype == "HID" || $nhs == 1)) {
      show_anahist($id);
    }

    if ($htype == "HID") {
      $hsid=HistoSet($id);
      if ($nhs>1)
	echo "<p>This Histogram is part of a <a href=$_SERVER[PHP_SELF]?hsid=${hsid}> Set </a> of $nhs similar histograms </p>\n";
    }
    else {
      if($nhs>1) {
	echo "<p>This is a <B> Set of $nhs similar histograms:</B> &nbsp&nbsp&nbsp";
	if ($fulllist) {
	  $query="select HID,SUBTITLE from HISTOGRAM  where HSET=:id order by IHS";
	  $lstid = OCIParse($conn,$query);
	  ocibindbyname($lstid,':id', $id);
	  OCIExecute($lstid, OCI_DEFAULT);
	  $ihs=1; 
	  while (OCIFetchInto($lstid, $list, OCI_ASSOC)) {
	    if ($ihs==1)
	      echo "<center><table id='LIST' rules='cols' border=1><thead><tr><td><B>HID 
</B><td><B>Subtitle</B></tr></thead><tbody>\n";
	    echo "<tr><td><a href='Histogram.php?hid=".$list["HID"]."'>".$list["HID"]."</a><td>".$list["SUBTITLE"]."</tr>\n";
	    $ihs++;
	  } 
	  echo "</tbody></table></center>";
	  ocifreestatement($lstid);
	  echo "<a href='$_SERVER[PHP_SELF]?hsid=${id}'>hide histogram list</a><br>\n";
	}
	else 
	  echo "<a href='$_SERVER[PHP_SELF]?hsid=${id}&fulllist=1'>see histogram list</a><br>\n";
      }
    }
  }
  echo "<p><table align=\"center\"><tr><td>Description <td><textarea valign='center' cols='66' rows='5' name='DESCR'".
    ($canwrite ? "" : "readonly")
    .">".$_POST["DESCR"]."</textarea></tr></table><br>\n";
  if($mode == "display") {
    if($_POST["DOC"]) echo "Documentation <a>".$_POST["DOC"]." </a><br>\n"; 
    echo "<input type='hidden' name='DOC' value='".$_POST["DOC"]."'>\n";
  }
  else {
    echo "Documentation (HTTP link)  <textarea cols='80' rows='1' name='DOC'>".
      $_POST["DOC"]."</textarea><br>";
  }


  if( $canwrite) {
    $action= ($mode == "display") ? "Update header" : "Confirm";
    echo "<table align=right><tr><td> <input align=right type='submit' name='Update_header' value='${action}'></tr></table>";
  }
  echo "</form>";

}



function hidpost_displayoptions() {
  global  $DispOpt;
  foreach ( $DispOpt as $field)
    echo "<input type='hidden' name='${field}' value='".$_POST[$field]."'>\n";
}



function histo_display($id,$htype,$mode)
{
  global $canwrite;
  global $histo;
  global $debug;
  global $conn;
  $cw_dis=  ($canwrite ? "" : "DISABLED");
  $cw_ro=  ($canwrite ? "" : "READONLY");
  $script=$_SERVER[PHP_SELF];
  $setlist=0;
  if($mode == "displaylist") {
    $mode = "display";
    $setlist=1;
  }
  if($mode == "display") {
    $script='write/histo_display.php';
    foreach (array("HSTYPE","NHS","DISPLAY","HSDISPLAY","TASKNAME") 
	     as $field) 
      $_POST[$field]=$histo[$field];
    if($htype == 'HID'){
      if($histo["DISPLAY"]) {
	echo "<B>Display options have been set individually for this histogram</B><br>";
	$MYDISPLAY=$histo["DISPLAY"];
      }
      else {
	$MYDISPLAY=$histo["HSDISPLAY"];
      }
    }
    else if($htype == 'HSID'){
      if ($histo["NHS"]>1) 
	echo "<B>Default display options for the histogram set</B><br>";
      $MYDISPLAY=$histo["HSDISPLAY"];
    }
    else if($htype == 'SHID'){
      $MYDISPLAY=$histo["DISPLAY"];
      $_POST["PAGE"]=$histo["PAGE"];
      $_POST["INSTANCE"]=$histo["INSTANCE"];
    }
    get_displayoptions($MYDISPLAY);
  }
  echo "<form action='${script}' method='POST'>\n"; 
  if ($mode == "update") {
    echo " Defining Display Options for Histogram ID <span class=normal>$id</span>";
    if ($htype == "SHID") 
      echo " in page ".$_POST["PAGE"];
    echo "<br>\n";
    if($htype == "HID") {
     $hsid=HistoSet($id);
     echo "<B>You are setting options for this particular histogram only.<br>
           Go to <a href='../Histogram.php?hsid=${hsid}'> Histogram set record $hsid </a> to change settings for 
           all histograms in set </B><br>";
    }
    if($htype == "HSID" && $_POST["NHS"]>1 )
     echo "<B>You are setting options for all histograms in set $id.<br>
           To change options for a single histogram, select it from the 
           <a href='../Histogram.php?hsid=${id}&fulllist=1#LIST'> histogram list </a> </B><br>\n";
  }
  foreach (array("HSTYPE","NHS","DISPLAY","HSDISPLAY","TASKNAME") as $field)
    echo "<input type='hidden' name='${field}' value='".$_POST[$field]."'>\n";
  echo "<input type='hidden' name='id' value='${id}'>\n";
  echo "<input type='hidden' name='htype' value='${htype}'>\n";
  if ($htype == "SHID") {
    echo "<input type='hidden' name='PAGE' value='".$_POST["PAGE"]."'>\n";
    echo "<input type='hidden' name='INSTANCE' value='".$_POST["INSTANCE"]."'>\n";
  }
  if ($mode != "display") {
    echo "<font size=+1> Leave options blank for default values</font><br><br>";
  }
  
  printf("Histogram title window &nbsp&nbsp size X <input name='HTIT_X_SIZE' size=5 value='%s' ${cw_ro}> &nbsp&nbsp \n",$_POST["HTIT_X_SIZE"]);
  printf(" &nbsp size Y <input name='HTIT_Y_SIZE' size=5 value='%s' ${cw_ro}>  \n",$_POST["HTIT_Y_SIZE"]);
  printf(" &nbsp offset X <input name='HTIT_X_OFFS' size=5 value='%s' ${cw_ro}> \n",$_POST["HTIT_X_OFFS"]);
  printf(" &nbsp offset Y <input name='HTIT_Y_OFFS' size=5 value='%s' ${cw_ro}> <br><br>\n",$_POST["HTIT_Y_OFFS"]);

  printf("Stat window &nbsp&nbsp size X <input name='STAT_X_SIZE' size=5 value='%s' ${cw_ro}> &nbsp&nbsp \n",$_POST["STAT_X_SIZE"]);
  printf(" &nbsp size Y <input name='STAT_Y_SIZE' size=5 value='%s' ${cw_ro}>  \n",$_POST["STAT_Y_SIZE"]);
  printf(" &nbsp offset X <input name='STAT_X_OFFS' size=5 value='%s' ${cw_ro}> \n",$_POST["STAT_X_OFFS"]);
  printf(" &nbsp offset Y <input name='STAT_Y_OFFS' size=5 value='%s' ${cw_ro}> <br><br>\n",$_POST["STAT_Y_OFFS"]);

  printf("X Axis &nbsp&nbsp  title <input name='LABEL_X' size=15 value='%s' ${cw_ro}> &nbsp&nbsp \n",$_POST["LABEL_X"]);
  printf("size <input name='TIT_X_SIZE' size=5 value='%s' ${cw_ro}> &nbsp&nbsp \n",$_POST["TIT_X_SIZE"]);
  printf("offset <input name='TIT_X_OFFS' size=5 value='%s' ${cw_ro}> &nbsp&nbsp <br>\n",$_POST["TIT_X_OFFS"]);
  echo "&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp";
  printf(" labels size <input name='LAB_X_SIZE' size=5 value='%s' ${cw_ro}> &nbsp&nbsp \n",$_POST["LAB_X_SIZE"]);
  printf("offset <input name='LAB_X_OFFS' size=5 value='%s' ${cw_ro}> &nbsp&nbsp <br>\n",$_POST["LAB_X_OFFS"]);
  echo "&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp";
  printf(" Log scale <input ${cw_dis} type='checkbox' name='LOGX' value='1' %s>&nbsp&nbsp\n",
	 $_POST["LOGX"] ? "checked" : "");
  printf(" Grid <input ${cw_dis} type='checkbox' name='GRIDX' value='1' %s> &nbsp&nbsp \n",
	 $_POST["GRIDX"] ? "checked" : "");
  printf("Number of divisions <input name='NDIVX' size=4 value='%s' ${cw_ro}> <br>\n",$_POST["NDIVX"]);
  
  printf("<br>Y Axis &nbsp&nbsp  title <input name='LABEL_Y' size=15 value='%s' ${cw_ro}> &nbsp&nbsp \n",$_POST["LABEL_Y"]);
  printf("size <input name='TIT_Y_SIZE' size=5 value='%s' ${cw_ro}> &nbsp&nbsp \n",$_POST["TIT_Y_SIZE"]);
  printf("offset <input name='TIT_Y_OFFS' size=5 value='%s' ${cw_ro}> &nbsp&nbsp <br>\n",$_POST["TIT_Y_OFFS"]);
  echo "&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp";
  printf(" labels size <input name='LAB_Y_SIZE' size=5 value='%s' ${cw_ro}> &nbsp&nbsp \n",$_POST["LAB_Y_SIZE"]);
  printf("offset <input name='LAB_Y_OFFS' size=5 value='%s' ${cw_ro}> &nbsp&nbsp <br>\n",$_POST["LAB_Y_OFFS"]);
  echo "&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp";
  printf(" Log scale <input ${cw_dis} type='checkbox' name='LOGY' value='1' %s>&nbsp&nbsp\n",
	 $_POST["LOGY"] ? "checked" : "");
  printf(" Grid <input ${cw_dis} type='checkbox' name='GRIDY' value='1' %s>&nbsp&nbsp\n",
	 $_POST["GRIDY"] ? "checked" : "");
  printf("Number of divisions <input name='NDIVY' size=4 value='%s' ${cw_ro}> <br>\n",$_POST["NDIVY"]);

  if ($_POST["HSTYPE"]=='H2D' || $_POST["HSTYPE"]=='P2D') {
    printf("<br>Z Axis &nbsp&nbsp  title <input name='LABEL_Z' size=15 value='%s' ${cw_ro}> &nbsp&nbsp \n",$_POST["LABEL_Z"]);
    printf("size <input name='TIT_Z_SIZE' size=5 value='%s' ${cw_ro}> &nbsp&nbsp \n",$_POST["TIT_Z_SIZE"]);
    printf("offset <input name='TIT_Z_OFFS' size=5 value='%s' ${cw_ro}> &nbsp&nbsp <br>\n",$_POST["TIT_Z_OFFS"]);
    echo "&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp";
    printf(" labels size <input name='LAB_Z_SIZE' size=5 value='%s' ${cw_ro}> &nbsp&nbsp \n",$_POST["LAB_Z_SIZE"]);
    printf("offset <input name='LAB_Z_OFFS' size=5 value='%s' ${cw_ro}> &nbsp&nbsp <br>\n",$_POST["LAB_Z_OFFS"]);
    echo "&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp";
    printf(" Log scale <input ${cw_dis} type='checkbox' name='LOGZ' value='1' %s>&nbsp&nbsp\n",
	   $_POST["LOGZ"] ? "checked" : "");
    printf(" Grid <input ${cw_dis} type='checkbox' name='GRIDZ' value='1' %s><br>\n",
	   $_POST["GRIDZ"] ? "checked" : "");
  }
  
  printf("<br>X miminum <input name='XMIN' size=7 value='%s' ${cw_ro}> &nbsp&nbsp ",$_POST["XMIN"]);
  printf("X maximum <input name='XMAX' size=7 value='%s' ${cw_ro}> <br>\n",$_POST["XMAX"]);
  printf("Y miminum <input name='YMIN' size=7 value='%s' ${cw_ro}> &nbsp&nbsp ",$_POST["YMIN"]);
  printf("Y maximum <input name='YMAX' size=7 value='%s' ${cw_ro}> <br>\n",$_POST["YMAX"]);
  if ($_POST["HSTYPE"]=='H2D' || $_POST["HSTYPE"]=='P2D') {
    printf("Z miminum <input name='ZMIN' size=7 value='%s' ${cw_ro}> &nbsp&nbsp ",$_POST["ZMIN"]);
    printf("Z maximum <input name='ZMAX' size=7 value='%s' ${cw_ro}> <br>\n",$_POST["ZMAX"]);

    echo"3D plots rendering: &nbsp&nbsp Engine <select name='TIMAGE'>";
    foreach (array("","HIST","IMAGE","AUTO") as $val) {
      $SELECTED[$val] = ( ($_POST["TIMAGE"] == $val) ? "SELECTED" : "") ;
      echo "<option ".$SELECTED[$val]."> ".$val." </option>";
    }
    echo "</select>";
    echo "&nbsp&nbsp Angles (deg): ";
     printf("Theta <input name='THETA' size=5 value='%s' ${cw_ro}> &nbsp&nbsp ",$_POST["THETA"]);
     printf("Phi <input name='PHI' size=5 value='%s' ${cw_ro}> <br>",$_POST["PHI"]);     
  }

  printf("<br>Stats option <input name='STATS' size=7 value='%s' ${cw_ro}> &nbsp&nbsp ",$_POST["STATS"]);
  printf("Fill Style <input name='FILLSTYLE' size=2 value='%s' ${cw_ro}> &nbsp&nbsp ",$_POST["FILLSTYLE"]);
  printf("Fill Color <input name='FILLCOLOR' size=2 value='%s' ${cw_ro}><br>\n",$_POST["FILLCOLOR"]);
  printf("Line Width <input name='LINEWIDTH' size=2 value='%s' ${cw_ro}> &nbsp&nbsp ",$_POST["LINEWIDTH"]);
  printf("Line Style <input name='LINESTYLE' size=2 value='%s' ${cw_ro}> &nbsp&nbsp ",$_POST["LINESTYLE"]);
  printf("Line Color <input name='LINECOLOR' size=2 value='%s' ${cw_ro}><br>\n",$_POST["LINECOLOR"]);
  printf("Marker Style <input name='MARKERSTYLE' size=2 value='%s' ${cw_ro}> &nbsp&nbsp ",$_POST["MARKERSTYLE"]);
  printf("Marker Color <input name='MARKERCOLOR' size=2 value='%s' ${cw_ro}>\n",$_POST["MARKERCOLOR"]);
  printf("Marker Size <input name='MARKERSIZE' size=2 value='%s' ${cw_ro}><br>\n",$_POST["MARKERSIZE"]);

  printf("ROOT Draw options <input name='DRAWOPTS' size=20 value='%s' ${cw_ro}><br>\n",$_POST["DRAWOPTS"]);
  // printf("Display Refresh Time (s)  <input name='REFRESH' size=5 value='%s' ${cw_ro}><br>\n",$_POST["REFRESH"]);
  printf("<br>Overplot reference (if available)    <select name='REF'>");
  foreach (array("","NOREF","NONE","AREA","ENTR") as $val) {
      $SELECTED[$val] = ($_POST["REF"] == $val) ? "SELECTED" : "" ;
      echo "<option ".$SELECTED[$val]."> ".$val." </option>";
    }
  echo "</select> &nbsp&nbsp(Possible Normalization is NONE, same AREA, same ENTRies)<br>";
  printf("<br>Pattern to be overdrawn on histogram (must be a ROOT file name)<br> \$GROUPDIR/online/Histograms/Reference/%s/",
         $_POST["TASKNAME"]);
  printf("<input name='DRAWPATTERN' size=40 value='%s' ${cw_ro}><br>\n",$_POST["DRAWPATTERN"]);


  if( $canwrite) {
    $action= ($mode == "display" && $htype != "SHID") ? "Update Display Options" : "Confirm";
    echo "<table align=right><tr><td> <input align=right type='submit' name='Update_display' value='${action}'></tr></table>";
  }
  echo "</form>";
  
  if (!$setlist && $mode=='display' && $htype=="HSID" && $_POST["NHS"]>1) {
    echo "<form action='DispList.php' method='post'>\n";
    foreach (array("TASKNAME","HSALGO","HSTYPE","HSTITLE","NHS","HSDISPLAY") as $field)
      echo "<input type='hidden' name='${field}' value='".$_POST[$field]."'>\n";
    echo "<input type='hidden' name='hsid' value='${id}'>\n";
    echo "<center> <input type='submit' value='See Options for all histograms in set' name='analist'></center>\n";
    echo "</form>";
  }
}

function get_labels($hid,$nl) {
 global $conn;
 global $debug;
 $query=" SELECT ";
 for ($i=1 ; $i<=$nl; $i++) {
   $query .= "parcomponent(BINLABELS,${i}) LAB${i}";
   if ($i<$nl) {
     $query .= ",";
   }
 }
 $query .= " from HISTOGRAM where HID='${hid}'";
 if($debug) echo "query is $query <br>\n";
 $stid = OCIParse($conn,$query);
 OCIExecute($stid);
 OCIFetchInto($stid, $hlabels, OCI_ASSOC );
 for ($i=1 ; $i<=$nl; $i++) {
   $_POST["LAB${i}"]=$hlabels["LAB${i}"];
 }
 ocifreestatement($stid);
}

function histo_labels($id,$htype,$mode)
{
  global $canwrite;
  global $histo;
  global $debug;
  global $conn;
  $cw_dis=  ($canwrite ? "" : "DISABLED");
  $cw_ro=  ($canwrite ? "" : "READONLY");
  $script=$_SERVER[PHP_SELF];
  $setlist=0;
  if($mode == "display") {
    $script='write/histo_labels.php';
    foreach (array("NHS","NLABELS") 
	     as $field)  
      $_POST[$field]=$histo[$field];
    if($htype == 'HSID'){
      if ($histo["NHS"]>1) 
	echo "<B> labels for the the first histogram in set</B><br>";
    }
    get_labels(SingleHist($id),$histo['NLABELS']);
    $nlab = $_POST["NLABELS"];
  } 
  else {
    $nlab = 20;
    if($_POST["nlab"] > $nlab) {
      $nlab = $_POST["nlab"];
    }
  }
  echo "<form action='${script}' method='POST'>\n"; 
  if ($mode == "update") {
    echo " Defining Custom Bin Labels for Histogram ID <span class=normal>$id</span><br>\n";
    if($htype == "HID") {
     $hsid=HistoSet($id);
     echo "<B>You are setting bin labels for this particular histogram only.<br>
           Go to <a href='../Histogram.php?hsid=${hsid}'> Histogram set record $hsid </a> to change labels for 
           all histograms in set </B><br>";
    }
    if($htype == "HSID" && $_POST["NHS"]>1 )
     echo "<B>You are setting bin labels for all histograms in set $id.<br>
           To change bin labels for a single histogram, select it from the 
           <a href='../Histogram.php?hsid=${id}&fulllist=1#LIST'> histogram list </a> </B><br>\n";
  }
  foreach (array("NHS","NLABELS") as $field)
    echo "<input type='hidden' name='${field}' value='".$_POST[$field]."'>\n";
  echo "<input type='hidden' name='id' value='${id}'>\n";
  echo "<input type='hidden' name='htype' value='${htype}'>\n";
  echo "<input type='hidden' name='nlab' value='${nlab}'>\n";
  for ($i=1 ; $i<=$nlab; $i++) {
    echo "Label for Bin ${i}  <input type='text' name='LAB${i}' size=15 value='".$_POST["LAB${i}"]."'><br>\n";
  }


  if( $canwrite) {
    $action= ($mode == "display") ? "Update Bin Labels" : "Confirm";
    echo "<table align=right><tr><td> <input align=right type='submit' name='Update_labels' value='${action}'></tr></table>";
  }
  echo "</form>";
  
  if ($mode != "display") {
    $nlab += 20;
    echo "<form action=${script} method='POST'>\n";
    foreach (array("NHS","NLABELS") as $field)
      echo "<input type='hidden' name='${field}' value='".$_POST[$field]."'>\n";
    echo "<input type='hidden' name='id' value='${id}'>\n";
    echo "<input type='hidden' name='htype' value='${htype}'>\n";
    echo "<input type='hidden' name='nlab' value='${nlab}'>\n";
    for ($i=1 ; $i<=$nlab; $i++) {
      echo "<input type='hidden' name='LAB${i}' value='".$_POST["LAB${i}"]."'>\n";
  }
    echo "<center> <input type='submit' value='Need More Labels' name='moreLabels'></center>\n";
    echo "</form>";
  }
}

function get_ana_parameters($alg,$type,$aid,$ia,$hhid,$mask=1,$names=1,$values=1,$hcvalues=0) {
  global $conn;
  
  if($mask) {
    $stid = OCIParse($conn,"SELECT MASK from ANASETTINGS WHERE ANA='$aid' and HISTO='${hhid}'");
    OCIExecute($stid);
    OCIFetchInto($stid,$myanas,OCI_ASSOC );
    $_POST["a${ia}_mask"]=$myanas["MASK"];
    ocifreestatement($stid);
  }
  if($names || $values || $hcvalues) {
    $dstid = OCIParse($conn,"begin OnlineHistDB.GetAlgoNpar('$alg',:np,:ni); end;");
    ocibindbyname($dstid,":np",$np);
    ocibindbyname($dstid,":ni",$ni);
    OCIExecute($dstid);
    $_POST["a${ia}_np"]=$np;
    $_POST["a${ia}_ni"]=$ni;
    ocifreestatement($dstid);
  }
  else {
    $np=0;
  }
  $ip=0;
  while ($ip++<$np){
    //  parameters to be checked for check algorithms (names and values),
    // input parameters for creator algorithms (names and $hcvalues)
    if($names) {
      $dstid = OCIParse($conn,"begin OnlineHistDB.GetAlgoParname('$alg',$ip,:pname,:pdefv); end;");
      ocibindbyname($dstid,":pname",$pname,500);
      ocibindbyname($dstid,":pdefv",$pdefv,50);
      OCIExecute($dstid);
      $_POST["a${ia}_p${ip}_name"]=$pname;
      $_POST["a${ia}_p${ip}_defv"]=$pdefv;
      ocifreestatement($dstid);
    }
    if($values && $type == "CHECK") {
      $dstid = OCIParse($conn,"begin OnlineHistDB.GetAnaSettings(${aid},'${hhid}',$ip,:warn,:alr); end;");
      ocibindbyname($dstid,":warn",$warn,50);
      ocibindbyname($dstid,":alr",$alr,50);
      OCIExecute($dstid);
      $_POST["a${ia}_p${ip}_w"]=$warn;
      $_POST["a${ia}_p${ip}_a"]=$alr;
      ocifreestatement($dstid);
    }
    if($hcvalues && $type == "HCREATOR") {
      $dstid = OCIParse($conn,"begin OnlineHistDB.GetHCSettings('${hhid}',$ip,:value); end;");
      ocibindbyname($dstid,":value",$warn,50);
      OCIExecute($dstid);
      $_POST["a${ia}_p${ip}"]=$warn;
      ocifreestatement($dstid);
    }
  }
  $ip=$np;
  if ($type == "CHECK") {
    while ($ip++<$np+$ni){
      // input parameters for check algorithms (names and values)
      $ipa=$ip-$np;
      if($names) {
        $dstid = OCIParse($conn,"begin OnlineHistDB.GetAlgoParname('$alg',$ip,:pname,:pdefv); end;");
        ocibindbyname($dstid,":pname",$pname,500);
        ocibindbyname($dstid,":pdefv",$pdefv,50);
        OCIExecute($dstid);
        $_POST["a${ia}_i${ipa}_name"]=$pname;
	$_POST["a${ia}_i${ipa}_defv"]=$pdefv;
        ocifreestatement($dstid);
      }
      if($values) {
        $dstid = OCIParse($conn,"begin OnlineHistDB.GetAnaInput(${aid},'${hhid}',$ipa,:inp); end;");
        ocibindbyname($dstid,":inp",$inp,50);
        OCIExecute($dstid);
        $_POST["a${ia}_i${ipa}_v"]=$inp;
        ocifreestatement($dstid);
      }
    }
  }
}

function histo_analysis($id,$htype,$mode) {
  global $canwrite;
  global $histo;
  global $debug;
  global $conn;
  $script=$_SERVER[PHP_SELF];
  $firstana=$lastana=1;
  $showpars=1;
  if($mode == "display") {
    $script='write/histo_analysis.php';
    foreach (array("TASKNAME","HSALGO","HSTYPE","HSTITLE","NHS","NANALYSIS") 
	     as $field)
      $_POST[$field]=$histo[$field];
    $lastana=$histo["NANALYSIS"]; 
    // get analysis entries
    $ia=0;
    $hsid=HistoSet($id);
    $stid = OCIParse($conn,"SELECT AID,ALGORITHM from ANALYSIS WHERE HSET=$hsid order by AID");
    OCIExecute($stid);
    while (OCIFetchInto($stid,$myana,OCI_ASSOC )) {
      $ia++;
      $_POST["a${ia}_id"]=$myana["AID"];
      $_POST["a${ia}_alg"]=$myana["ALGORITHM"];
    }
    ocifreestatement($stid);
    $showpars=0;

    if($htype == "HID" || $_POST["NHS"]==1) { // get parameters to be shown
      $showpars=1;
      $hhid= ($htype == "HID") ? $id : "${hsid}/1";
      $ia=0;
      while ($ia++<$lastana)
	get_ana_parameters($_POST["a${ia}_alg"],"CHECK",$_POST["a${ia}_id"],$ia,$hhid);
    }
    else {
      $ia=0;
      while ($ia++<$lastana)
	get_ana_parameters($_POST["a${ia}_alg"],"CHECK",0,1,0,0,1,0);
    }
  }
  else if ($mode == "update") {
    $firstana=$lastana=$_POST["Iana"];
  }
  else if ($mode == "newana") {
    $_POST["a1_id"]=0;
    get_ana_parameters($_POST["a1_alg"],"CHECK",0,1,0,0,1,0);
  }

  for ($ia=$firstana; $ia<=$lastana; $ia++) {
    echo "<table align='center' rules=none border=1 cellpadding=0> <tr><td><form action='${script}' method='POST'>\n"; 
    foreach (array("TASKNAME","HSALGO","HSTYPE","HSTITLE","NHS","NANALYSIS") as $field)
      echo "<input type='hidden' name='${field}' value='".$_POST[$field]."'>\n";
    echo "<input type='hidden' name='id' value='${id}'>\n";
    echo "<input type='hidden' name='htype' value='${htype}'>\n";
    echo "<input type='hidden' name='Iana' value='${ia}'>\n";
    
    if ($mode != 'new') {
      echo "<input type='hidden' name='a${ia}_id' value='".$_POST["a${ia}_id"],"'>\n";
      echo "<input type='hidden' name='a${ia}_np' value='".$_POST["a${ia}_np"],"'>\n";
      echo "<input type='hidden' name='a${ia}_ni' value='".$_POST["a${ia}_ni"],"'>\n";
      echo "<input type='hidden' name='a${ia}_mask' value='".$_POST["a${ia}_mask"],"'>\n";
      echo "Analysis <span class='normal'>". $_POST["a${ia}_alg"]." </span>\n";
      echo "<input type='hidden' name='a${ia}_alg' value='".$_POST["a${ia}_alg"],"'>\n";
    } 
    else{
      echo "Analysis <select name='a${ia}_alg'>\n";
      $stid = OCIParse($conn,"SELECT ALGNAME,ALGDOC from ALGORITHM WHERE ALGTYPE='CHECK'");
      OCIExecute($stid);
      while (OCIFetchInto($stid,$myalg,OCI_ASSOC )) {
	echo "<option class='normal'>".$myalg["ALGNAME"]."</option>\n";
	$algdoc[$myalg["ALGNAME"]]=$myalg["ALGDOC"];
      }
      ocifreestatement($stid);
      echo "</select>";
      if (isset($algdoc))
	foreach ($algdoc as $key => $value) 
	  echo "<input type='hidden' name='doc_$key' value='$value'>\n";
    } 
    if($showpars && $_POST["a${ia}_mask"]) echo "&nbsp <B><font color=red>(masked)</font></B>";
    echo "<br>";
    if ($mode == "newana") echo "<p>".$_POST["doc_".PutUnderscore($_POST["a${ia}_alg"])]."</p>";
    if ($mode == 'update') {
      if($htype == "HID") {
	$hsid=HistoSet($id);
	echo "<B>You are setting parameters for this particular histogram only.<br>
           Go to <a href='../Histogram.php?hsid=${hsid}'> Histogram set record $hsid </a> to change settings for 
           all histograms in set </B><br>";
      }
      if($htype == "HSID" && $_POST["NHS"]>1 )
	echo "<B>You are setting parameters for all histograms in set $id<br>".
	  "<B> overwriting settings for single histograms</B><br>".
	  "To change options for a single histogram, select it from the 
           <a href='../Histogram.php?hsid=${id}&fulllist=1#LIST'> histogram list </a> </B><br>\n";
    }
    if ($showpars &&  ($_POST["a${ia}_np"]>0 || $_POST["a${ia}_ni"]>0) ) {
      if ($_POST["a${ia}_ni"]>0) {
        echo "<table align='center'><thead><tr><td><B>Input Parameter</B><td><B>Value</B></tr></thead>\n";
        $ip=0;
        while ($ip++<$_POST["a${ia}_ni"]){
          printf("<tr><td> <input READONLY type='text' size=15 name='a${ia}_i${ip}_name' value='%s'>\n",$_POST["a${ia}_i${ip}_name"]);
	  if($mode == "newana") { 
	    $_POST["a${ia}_i${ip}_v"] = $_POST["a${ia}_i${ip}_defv"]; 
	  }
	  $fw=myfloatformat($_POST["a${ia}_i${ip}_v"]);
          printf("<td> <input type='text' size=8 name='a${ia}_i${ip}_v' value='$fw'>\n",$_POST["a${ia}_i${ip}_v"]);
        }
        echo "</table>";
      }
      if ($_POST["a${ia}_np"]>0) {
        echo "<table align='center'><thead><tr><td><B>Parameter</B><td><B>Warning</B><td><B>Alarm </B></tr></thead>\n";
        $ip=0;
        while ($ip++<$_POST["a${ia}_np"]){
          printf("<tr><td> <input READONLY type='text' size=15 name='a${ia}_p${ip}_name' value='%s'>\n",$_POST["a${ia}_p${ip}_name"]);
	  if($mode == "newana") {
	    $_POST["a${ia}_p${ip}_w"] = $_POST["a${ia}_p${ip}_defv"];
	    $_POST["a${ia}_p${ip}_a"] = $_POST["a${ia}_p${ip}_defv"];
	  }
          $fw= myfloatformat($_POST["a${ia}_p${ip}_w"]);
	  $fa= myfloatformat($_POST["a${ia}_p${ip}_a"]);
          printf("<td> <input type='text' size=8 name='a${ia}_p${ip}_w' value='$fw'>\n",$_POST["a${ia}_p${ip}_w"]);
          printf("<td> <input type='text' size=8 name='a${ia}_p${ip}_a' value='$fa'></tr>\n",$_POST["a${ia}_p${ip}_a"]);
        }
        echo "</table>";
      }
    }
    else {
      $ip=0;
      while ($ip++<$_POST["a${ia}_np"])
	printf("<input type='hidden' name='a${ia}_p${ip}_name' value='%s'>\n",$_POST["a${ia}_p${ip}_name"]);
      $ip=0;
      while ($ip++<$_POST["a${ia}_ni"])
	printf("<input type='hidden' name='a${ia}_i${ip}_name' value='%s'>\n",$_POST["a${ia}_i${ip}_name"]);
    }
    if ($canwrite) {
      echo "<table align='center'><tr>";
      $action= ($mode == "display") ? "Update" : ( ($mode == "new") ? "Set Parameters" : "Confirm");
      echo "<td> <input type='submit' name='Update_analysis' value='${action}'>\n";
      if ($mode=='display' && $showpars) {
	$action= $_POST["a${ia}_mask"] ? "Unmask" : "Mask";
	echo "<td> <input type='submit' name='SetMask_Analysis' value='${action}'>\n";
      }
      if ($mode=='display' && $htype == 'HSID')
	echo "<td> <input type='submit' name='Remove_Analysis' value='Remove'>\n";
      echo "</tr></table>";
    }
    echo "</form>";
    echo "</table>";
  }
  if (!$showpars) {
    echo "<form action='AnaList.php' method='post'>\n";
    foreach (array("TASKNAME","HSALGO","HSTYPE","HSTITLE","NHS","NANALYSIS") as $field)
      echo "<input type='hidden' name='${field}' value='".$_POST[$field]."'>\n";
    echo "<input type='hidden' name='hsid' value='${id}'>\n";
    echo "<center> <input type='submit' value='See Parameters for all histograms in set' name='analist'></center>\n";
    echo "</form>";
  }
}

function task_form($taskname,$mode)
{
  global $taskRec;
  global $canwrite;
  global $debug;
  global $conn;
  global $Reference_home,$Installation;
  $script=$_SERVER[PHP_SELF];
  $ro=  ($canwrite ? "" : "DISABLED");
  if($mode == "display") {
    $script='write/task.php';
    foreach (array("SUBSYS1","SUBSYS2","SUBSYS3","RUNONPHYSICS","RUNONCALIB","RUNONEMPTY","SAVEFREQUENCY","REFERENCE") 
	     as $field)
      $_POST[$field]=$taskRec[$field];
  }
  echo "<form action='${script}' method='POST'>\n"; 
  echo "<input type='hidden' name='TASKNAME' value='${taskname}'>\n";
  $dets=getSubSystems($conn);
  array_unshift($dets,"");
  echo "<B>Related Detectors/Subsystems</B>:<br><table>\n";
  for ($id=1;$id<=3;$id++) {
    $vname="SUBSYS${id}";
    $vcont=$_POST[$vname];
    echo "<select $ro name=\"${vname}\">";
    foreach ($dets as $d) {
      printf( "<option %s> %s", ($d == $vcont) ? "SELECTED" : "", $d);
    }
    echo "</select>\n";
  }
  echo "</table><br>\n";
  
  echo "<B>Run configuration:</B><br><table>\n";
  printf("Runs on Physics events <input $ro type='checkbox' name='RUNONPHYSICS' value='1' %s><br>\n",
	 $_POST["RUNONPHYSICS"] ? "checked" : "");
  printf("Runs on Calibration events <input $ro type='checkbox' name='RUNONCALIB' value='1' %s><br>\n",
	 $_POST["RUNONCALIB"] ? "checked" : "");
  printf("Runs on Empty events <input $ro type='checkbox' name='RUNONEMPTY' value='1' %s><br>\n",
	 $_POST["RUNONEMPTY"] ? "checked" : "");
  echo "</table><br>\n";

  echo "<B>Saveset configuration:</B><br><table>\n";
  printf("Saving frequency <input $ro type='text' name='SAVEFREQUENCY' size=5 value='%.1f'><br>\n",
	 $_POST["SAVEFREQUENCY"]);
  echo "</table><br>\n";

  if($mode == "display") {
    echo "<B>Reference histograms:</B><br><table>\n";
    if($_POST["REFERENCE"])
      echo "Latest Reference histograms available in ${Reference_home}/".$_POST["REFERENCE"]."<br>\n";
    else
      echo "No Reference histogram available<br>";
    if ( ($Installation == "Pit" || $Installation == "test_dev") && $canwrite) {
      echo "<a href=write/reference.php?task=$taskname> Upload Reference Histograms</a>";
    }
    echo "</table><br>\n";
  }

   if( $canwrite) {
    $action= ($mode == "display") ? "Update Task record" : "Confirm";
    echo "<table align=right><tr><td> <input align=center type='submit' name='Update_task' value='${action}'></tr></table>";
  }
  echo "</form>";

}


?>
