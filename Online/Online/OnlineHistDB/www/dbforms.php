<?
function histo_header($id,$htype,$mode)
{
  global $canwrite;
  global $histo;
  global $debug;
  global $conn;
  global $fulllist;
  global $PHP_SELF;
  $script=$PHP_SELF;
  if($mode == "display") {
    $script='write/histo_header.php';
    foreach (array("DESCR","DOC","TASKNAME","HSALGO","HSTYPE","HSTITLE","SUBTITLE","NHS") 
	     as $field)
      $_POST[$field]=$histo[$field];
  }
  echo "<form action='${script}' method='POST'>\n"; 
  echo " ID <span class=normal>$id</span> &nbsp&nbsp&nbsp Task <span class=normal>".$_POST["TASKNAME"]."</span>".
    " &nbsp&nbsp&nbsp Algorithm <span class=normal>".$_POST["HSALGO"]."</span>".
    " &nbsp&nbsp&nbsp Type <span class=normal>".$_POST["HSTYPE"]."</span><br>\n".    
    " Title <span class=normal>".$_POST["HSTITLE"].
    (($htype == "HID" || $_POST["NHS"] == 1) ? " ".$_POST["SUBTITLE"] : "").
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
      echo "Reference histogram available at ".$histo["REFERENCE"]."<br>\n";
    else
      echo "No Reference histogram available<br>";
    echo "Created on <span class=normal>".$histo["CRE_DATE"]."</span><br>\n";
    if($histo["OBSOLETENESS"])
      echo "Obsolete from <span class=normal>".$histo["OBSOLETENESS"]."</span><br>\n";

    if($histo["ISANALYSISHIST"]) {
      $stid = OCIParse($conn,"SELECT * from HCREATOR HC where HCID='".SingleHist($id)."'");
      OCIExecute($stid, OCI_DEFAULT);
      OCIFetchInto($stid, $hcreator, OCI_ASSOC );
      echo "<p>This is a <B> Display Histogram </B> obtained automatically by Analysis Task<br>
 through Algorithm <span class=normal>".$hcreator["ALGORITHM"]."</span>\n ";
      if ($shsid=$hcreator["SOURCESET"]) 
	echo " from histogram set <a href='Histogram.php?hsid=${shsid}'> ${shsid} </a><br>\n";
      else {
	$ih=0;
	echo " from histograms<br><center>\n"; 
	while($ih++<8) 
	  if($shid=$hcreator["SOURCEH".$ih]) echo "<a href='Histogram.php?hid=${shid}'> ${shid} </a><br>\n";
	echo "</center>";
      }
    }
    $nhs=$histo["NHS"];
    if ($htype == "HID") {
      $hsid=HistoSet($id);
      if ($nhs>1)
	echo "<p>This Histogram is part of a <a href=${PHP_SELF}?hsid=${hsid}> Set </a> of $nhs identical histograms </p>\n";
    }
    else {
      if($nhs>1) {
	echo "<p>This is a <B> Set of $nhs identical histograms:</B> &nbsp&nbsp&nbsp";
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
	  echo "<a href='${PHP_SELF}?hsid=${id}'>hide histogram list</a><br>\n";
	}
	else 
	  echo "<a href='${PHP_SELF}?hsid=${id}&fulllist=1'>see histogram list</a><br>\n";
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

function get_displayoptions($disp) {
  global $conn;
  $query="select * from DISPLAYOPTIONS where DOID='".$disp."'";
  $dstid = OCIParse($conn,$query);
  OCIExecute($dstid);
  OCIFetchInto($dstid, $mydisp, OCI_ASSOC );
  foreach (array("LABEL_X","LABEL_Y","LABEL_Z","YMIN","YMAX","STATS","FILLCOLOR","FILLSTYLE","LINEWIDTH","LINESTYLE",
		 "LINECOLOR","DRAWOPTS")
	   as $field)
    $_POST[$field]=$mydisp[$field];
  ocifreestatement($dstid);
}

function hidpost_displayoptions() {
  foreach (array("LABEL_X","LABEL_Y","LABEL_Z","YMIN","YMAX","STATS","FILLCOLOR","FILLSTYLE","LINEWIDTH","LINESTYLE",
		 "LINECOLOR","DRAWOPTS")
	   as $field)
    echo "<input type='hidden' name='${field}' value='".$_POST[$field]."'>\n";
}



function histo_display($id,$htype,$mode)
{
  global $canwrite;
  global $histo;
  global $debug;
  global $conn;
  global $PHP_SELF;
  $script=$PHP_SELF;
  $setlist=0;
  if($mode == "displaylist") {
    $mode = "display";
    $setlist=1;
  }
  if($mode == "display") {
    $script='write/histo_display.php';
    foreach (array("HSTYPE","NHS","DISPLAY","HSDISPLAY") 
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
  foreach (array("HSTYPE","NHS","DISPLAY","HSDISPLAY") as $field)
    echo "<input type='hidden' name='${field}' value='".$_POST[$field]."'>\n";
  echo "<input type='hidden' name='id' value='${id}'>\n";
  echo "<input type='hidden' name='htype' value='${htype}'>\n";
  if ($htype == "SHID") 
    echo "<input type='hidden' name='PAGE' value='".$_POST["PAGE"]."'>\n";

  printf("X Label <input name='LABEL_X' size=15 value='%s' %s> &nbsp&nbsp \n",$_POST["LABEL_X"],
    ($canwrite ? "" : "readonly"));
  printf("Y Label <input name='LABEL_Y' size=15 value='%s' %s><br>\n",$_POST["LABEL_Y"],
    ($canwrite ? "" : "readonly"));
  if ($_POST["HSTYPE"]=='2D')
    printf("Z Label <input name='LABEL_Z' size=15 value='%s' %s><br>\n",$_POST["LABEL_Z"],
      ($canwrite ? "" : "readonly"));
   printf("Y miminum <input name='YMIN' size=7 value='%s' %s> &nbsp&nbsp ",$_POST["YMIN"], 
    ($canwrite ? "" : "readonly"));
   printf("Y maximum <input name='YMAX' size=7 value='%s' %s> <br>\n",$_POST["YMAX"], 
    ($canwrite ? "" : "readonly"));
   printf("Stats option <input name='STATS' size=7 value='%s' %s> &nbsp&nbsp ",$_POST["STATS"], 
    ($canwrite ? "" : "readonly"));
   printf("Fill Style <input name='FILLSTYLE' size=2 value='%s' %s> &nbsp&nbsp ",$_POST["FILLSTYLE"], 
    ($canwrite ? "" : "readonly"));
   printf("Fill Color <input name='FILLCOLOR' size=2 value='%s' %s><br>\n",$_POST["FILLCOLOR"],
    ($canwrite ? "" : "readonly"));
   printf("Line Width <input name='LINEWIDTH' size=2 value='%s' %s> &nbsp&nbsp ",$_POST["LINEWIDTH"],
    ($canwrite ? "" : "readonly"));
   printf("Line Style <input name='LINESTYLE' size=2 value='%s' %s> &nbsp&nbsp ",$_POST["LINESTYLE"], 
    ($canwrite ? "" : "readonly"));
   printf("Line Color <input name='LINECOLOR' size=2 value='%s' %s><br>\n",$_POST["LINECOLOR"],
    ($canwrite ? "" : "readonly"));
   printf("ROOT Draw options <input name='DRAWOPTS' size=20 value='%s' %s><br>\n",$_POST["DRAWOPTS"],
    ($canwrite ? "" : "readonly"));


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

function get_ana_parameters($alg,$aid,$ia,$hhid,$mask=1,$names=1,$values=1) {
  global $conn;
  
  if($mask) {
    $stid = OCIParse($conn,"SELECT MASK from ANASETTINGS WHERE ANA='$aid' and HISTO='${hhid}'");
    OCIExecute($stid);
    OCIFetchInto($stid,$myanas,OCI_ASSOC );
    $_POST["a${ia}_mask"]=$myanas["MASK"];
    ocifreestatement($stid);
  }
  if($names || $values) {
    $dstid = OCIParse($conn,"begin OnlineHistDB.GetAlgoNpar('$alg',:np); end;");
    ocibindbyname($dstid,":np",$np);
    OCIExecute($dstid);
    $_POST["a${ia}_np"]=$np;
    ocifreestatement($dstid);
  }
  else {
    $np=0;
  }
  $ip=0;
  while ($ip++<$np){
    if($names) {
      $dstid = OCIParse($conn,"begin OnlineHistDB.GetAlgoParname('$alg',$ip,:pname); end;");
      ocibindbyname($dstid,":pname",$pname,500);
      OCIExecute($dstid);
      $_POST["a${ia}_p${ip}_name"]=$pname;
      ocifreestatement($dstid);
    }
    if($values) {
      $dstid = OCIParse($conn,"begin OnlineHistDB.GetAnaSettings(${aid},'${hhid}',$ip,:warn,:alr); end;");
      ocibindbyname($dstid,":warn",$warn,50);
      ocibindbyname($dstid,":alr",$alr,50);
      OCIExecute($dstid);
      $_POST["a${ia}_p${ip}_w"]=$warn;
      $_POST["a${ia}_p${ip}_a"]=$alr;
      ocifreestatement($dstid);
    }
    //	  echo "Analysis $ia Alg ".$_POST["a${ia}_alg"]." Par ".$_POST["a${ia}_p${ip}_name"].
    //" w=".$_POST["a${ia}_p${ip}_w"]." a=".$_POST["a${ia}_p${ip}_a"]." <br>\n";
  }
}

function histo_analysis($id,$htype,$mode) {
  global $canwrite;
  global $histo;
  global $debug;
  global $conn;
  $script=$PHP_SELF;
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
	get_ana_parameters($_POST["a${ia}_alg"],$_POST["a${ia}_id"],$ia,$hhid);
    }
    else {
      $ia=0;
      while ($ia++<$lastana)
	get_ana_parameters($_POST["a${ia}_alg"],0,1,0,0,1,0);
    }
  }
  else if ($mode == "update") {
    $firstana=$lastana=$_POST["Iana"];
  }
  else if ($mode == "newana") {
    $_POST["a1_id"]=0;
    get_ana_parameters($_POST["a1_alg"],0,1,0,0,1,0);
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
    if ($showpars && $_POST["a${ia}_np"]>0) {
      echo "<table align='center'><thead><tr><td><B>Parameter</B><td><B>Warning</B><td><B>Alarm </B></tr></thead>\n";
      $ip=0;
      while ($ip++<$_POST["a${ia}_np"]){
	printf("<tr><td> <input READONLY type='text' size=15 name='a${ia}_p${ip}_name' value='%s'>\n",$_POST["a${ia}_p${ip}_name"]);
	$fw=myfloatformat($_POST["a${ia}_p${ip}_w"]); $fa=myfloatformat($_POST["a${ia}_p${ip}_a"]); 
	printf("<td> <input type='text' size=8 name='a${ia}_p${ip}_w' value='$fw'>\n",$_POST["a${ia}_p${ip}_w"]);
	printf("<td> <input type='text' size=8 name='a${ia}_p${ip}_a' value='$fa'></tr>\n",$_POST["a${ia}_p${ip}_a"]);
      }
      echo "</table>";
    }
    else {
      $ip=0;
      while ($ip++<$_POST["a${ia}_np"])
	printf("<input type='hidden' name='a${ia}_p${ip}_name' value='%s'>\n",$_POST["a${ia}_p${ip}_name"]);
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


?>
