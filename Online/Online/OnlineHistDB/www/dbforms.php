<?
include 'dispopt.php';

function show_anahist($id) {
 global $conn;
 $stid = OCIParse($conn,"SELECT HCID,ALGORITHM,SOURCEH1,SOURCEH2,SOURCEH3,SOURCEH4,SOURCEH5,SOURCEH6,SOURCEH7,SOURCEH8,SOURCESET from HCREATOR where HCID='".SingleHist($id)."'");
 OCIExecute($stid, OCI_DEFAULT);
 OCIFetchInto($stid, $hcreator, OCI_ASSOC );
 echo "<p>This is an <B> Analysis Histogram </B> produced by Presenter/Analysis Task<br>
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
  $script=$_SERVER["PHP_SELF"];
  $maxPl=100;
  if($mode == "display") {
    $script='write/histo_header.php';
    foreach (array("DESCR","DOC","TASKNAME","HSALGO","HSTYPE","HSTITLE","SUBTITLE","NHS","NAME","REFPAGE") 
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
  foreach (array("TASKNAME","HSALGO","HSTYPE","HSTITLE","SUBTITLE","NHS","NAME") as $field)
    echo "<input type='hidden' name='${field}' value='".$_POST[$field]."'>\n";
  echo "<input type='hidden' name='id' value='${id}'>\n";
  echo "<input type='hidden' name='htype' value='${htype}'>\n";

  if($mode == "display") {
    $detectors= ($histo["SUBSYS1"] or $histo["SUBSYS2"] or  $histo["SUBSYS3"])  ?
      (($histo["SUBSYS1"] ? $histo["SUBSYS1"] : "")." ".
       ($histo["SUBSYS2"] ? $histo["SUBSYS2"] : "")." ".
       ($histo["SUBSYS3"] ? $histo["SUBSYS3"] : "")) : "none";
    echo "Related detectors: <B>${detectors}</B><br>";
//    if($histo["REFERENCE"])
//      echo "Latest Reference histogram available in ${Reference_home}/".$histo["REFERENCE"]."<br>\n";
//    else
//      echo "No Reference histogram available<br>";
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
	echo "<p>This Histogram is part of a <a href=".$_SERVER["PHP_SELF"]."?hsid=${hsid}> Set </a> of $nhs similar histograms </p>\n";
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
	  echo "<a href='".$_SERVER["PHP_SELF"]."?hsid=${id}'>hide histogram list</a><br>\n";
	}
	else 
	  echo "<a href='".$_SERVER["PHP_SELF"]."?hsid=${id}&fulllist=1'>see histogram list</a><br>\n";
        echo "</p>";
      }
    }
  }
  echo "<p><table align=\"center\"><tr><td>Description <td><textarea valign='center' cols='66' rows='5' name='DESCR'".
    ($canwrite ? "" : "readonly")
    .">".$_POST["DESCR"]."</textarea></tr></table><br>\n";
  if ($mode == "display") {
    if($_POST["DOC"]) echo "Documentation <a>".$_POST["DOC"]." </a><br>\n"; 
    echo "<input type='hidden' name='DOC' value='".$_POST["DOC"]."'>\n";
  }
  else {
    echo "Documentation (HTTP link)  <textarea cols='80' rows='1' name='DOC'>".
      $_POST["DOC"]."</textarea><br>";
  }
  echo "</p>";

  if ($htype == "HID" || $_POST["NHS"] == 1) {
    echo "Associated Page (to be displayed when clicking on histogram) <select name='REFPAGE'><option class='normal'>none</option>\n";
    $stid = OCIParse($conn,"SELECT PAGENAME FROM PAGE order by PAGENAME");
    OCIExecute($stid);
    while (OCIFetchInto($stid, $page, OCI_ASSOC )) 
       printf("<option class='mysmall' %s value='%s'> %s </option>\n",
               ($_POST["REFPAGE"] == $page["PAGENAME"]) ? "selected" : "",
               $page["PAGENAME"],
               (strlen($page["PAGENAME"])>$maxPl ? "..." : "").substr($page["PAGENAME"],-$maxPl));
    ocifreestatement($stid);
    echo "</select> <br>";

    if($mode == "display") { // show pages including this histogram
      $stid = OCIParse($conn,"SELECT PAGE,SDISPLAY FROM SHOWHISTO where HISTO='".SingleHist($id)."' order by PAGE");
      OCIExecute($stid);
      $mypages=array();
      while (OCIFetchInto($stid, $hpages, OCI_ASSOC )) {
        array_push($mypages, $hpages["PAGE"]);
        if ($hpages["SDISPLAY"]) $adhocopt[$hpages["PAGE"]]=1;
      }
      echo "<p>";
      if(count($mypages)) {
        echo "Histograms is displayed on ".count($mypages)." pages:<br>\n";
        foreach ($mypages as $page) {
          printf("&nbsp&nbsp&nbsp&nbsp <a href='Viewpage.php?page=%s'>$page</a> %s <br>\n",
                 toGet($page), $adhocopt[$page] ? "<b>(with ad-hoc display options)</b>" : "");
        }
      }
      else {
        echo "Histogram is not displayed on any page<br>\n";
      }
      echo "</p>\n";
      ocifreestatement($stid);
    }
  }

  if( $canwrite) {
    $action= ($mode == "display") ? "Update header" : "Confirm";
    echo "<table><tr><td> <input align=right type='submit' name='Update_header' value='${action}'></td></tr></table>";
  }
  echo "</form><br>\n";

  if ( $mode == "display" && $canwrite && ($htype == "HID" || $_POST["NHS"] == 1)) {
    echo "<br><br><table border=1 align='center'>\n<tr><td> \n";
    if (count($mypages)) {
      echo "If you want to delete this histogram record, remove it first from the pages including it\n";
    }
    else {
      echo "<form action='write/deleteHisto.php' method='POST'>\n";
      echo "<input type='hidden' name='id' value='${id}'>\n";
      echo "<input type='hidden' name='htype' value='${htype}'>\n";
      echo "<input type='hidden' name='identifier' value='${identifier}'>\n";
      echo "<input align=right type='submit' name='Delete_Histo' value='Delete this Histogram Record'>\n";
    }
    echo "</form></td></tr></table>";
  }

}



function hidpost_displayoptions() {
  global  $DispOpt;
  foreach ( $DispOpt as $field)
    echo "<input type='hidden' name='${field}' value='".$_POST[$field]."'>\n";
}

function get_fitoptions($disp) {
  global $conn;
  global $kintsize,$kfloatsize;
  $dstid = OCIParse($conn,"begin OnlineHistDB.GetFitOptions($disp,:ff,:np,:ni,:fmin,:fmax);end;");
  ocibindbyname($dstid,":ff",$_POST["FITFUN"],$kintsize);
  ocibindbyname($dstid,":np",$np,$kintsize);
  ocibindbyname($dstid,":ni",$ni,$kintsize);
  ocibindbyname($dstid,":fmin",$_POST["FITMIN"],$kfloatsize);
  ocibindbyname($dstid,":fmax",$_POST["FITMAX"],$kfloatsize);
  OCIExecute($dstid);
  ocifreestatement($dstid);
  if ($np>0) {
    $dstid = OCIParse($conn,"begin :p := OnlineHistDB.GetFitParam($disp,:i);end;");
    for ($i=1 ; $i<=$np ; $i++) {
      ocibindbyname($dstid,":i",$i);
      ocibindbyname($dstid,":p",$_POST["FITPAR_$i"],$kfloatsize);
      OCIExecute($dstid);
    }
    ocifreestatement($dstid);
  }
  if ($ni>0) {
    $dstid = OCIParse($conn,"begin :p := OnlineHistDB.GetFitParam($disp,:i);end;");
    for ($i=1 ; $i<=$ni ; $i++) {
      $j=$i+$np;
      ocibindbyname($dstid,":i",$j);
      ocibindbyname($dstid,":p",$_POST["FITINPPAR_$i"],$kfloatsize);
      OCIExecute($dstid);
    }
    ocifreestatement($dstid);
  }
}

function histo_display($id,$htype,$mode)
{
  global $canwrite;
  global $histo;
  global $debug;
  global $conn;
  global $kintsize,$kfloatsize,$kvparsize;
  $cw_dis=  ($canwrite ? "" : "DISABLED");
  $cw_ro=  ($canwrite ? "" : "READONLY");
  $script=$_SERVER["PHP_SELF"];
  $setlist=0;

  
  if($htype == 'SHID'){
    $_POST["PAGE"]=$histo["PAGE"];
    $_POST["INSTANCE"]=$histo["INSTANCE"];
  }
  if($mode == "displaylist") {
    $mode = "display";
    $setlist=1;
  }
  if($mode == "display") {
    $script='write/histo_display.php';
    foreach (array("HSTYPE","NHS","DISPLAY","HSDISPLAY","TASKNAME","NAME") 
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
    }
    get_displayoptions($MYDISPLAY);
    get_fitoptions($MYDISPLAY);
  }
  
  if ($mode == "update" && $htype == "SHID") 
    $script='write/histo_display.php';

  echo "<form action='${script}' method='POST'>\n"; 
  if ($mode == "update") {
    echo " Defining Display Options for Histogram ID <span class=normal>$id</span>";
    if ($htype == "SHID") 
      echo " on page ".$_POST["PAGE"];
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
  foreach (array("HSTYPE","NHS","DISPLAY","HSDISPLAY","TASKNAME","NAME") as $field)
    echo "<input type='hidden' name='${field}' value='".$_POST[$field]."'>\n";
  echo "<input type='hidden' name='id' value='${id}'>\n";
  echo "<input type='hidden' name='htype' value='${htype}'>\n";
  echo "<input type='hidden' name='doid' value='${MYDISPLAY}'>\n";
  if ($htype == "SHID") {
    echo "<input type='hidden' name='PAGE' value='".$_POST["PAGE"]."'>\n";
    echo "<input type='hidden' name='INSTANCE' value='".$_POST["INSTANCE"]."'>\n";
  }
  if ($mode != "display") {
    echo "<font size=+1> Leave options blank for default values</font><br><br>";
  }

  printf(" Suppress Title window ?  <input ${cw_dis} type='checkbox' name='NOTITLE' value='1' %s>&nbsp&nbsp<br>\n",
	 $_POST["NOTITLE"] ? "checked" : "");
  printf("Histogram title to be shown (leave blank for using ROOT title) <input name='SHOWTITLE' size=50 maxlength=80 value='%s' ${cw_ro}> <br><br>\n",$_POST["SHOWTITLE"]);

  
  echo "<br><B>Sizes and Margins (relative to pad size)</B><br>\n";
  
  printf("Histogram title window &nbsp&nbsp size X <input name='HTIT_X_SIZE' size=5 value='%s' ${cw_ro}> \n",$_POST["HTIT_X_SIZE"]);
  printf(" &nbsp size Y <input name='HTIT_Y_SIZE' size=5 value='%s' ${cw_ro}>  \n",$_POST["HTIT_Y_SIZE"]);
  printf(" &nbsp offset X <input name='HTIT_X_OFFS' size=5 value='%s' ${cw_ro}> \n",$_POST["HTIT_X_OFFS"]);
  printf(" &nbsp offset Y <input name='HTIT_Y_OFFS' size=5 value='%s' ${cw_ro}> <br><br>\n",$_POST["HTIT_Y_OFFS"]);

  printf("Stat window &nbsp&nbsp size X <input name='STAT_X_SIZE' size=5 value='%s' ${cw_ro}> \n",$_POST["STAT_X_SIZE"]);
  printf(" &nbsp size Y <input name='STAT_Y_SIZE' size=5 value='%s' ${cw_ro}>  \n",$_POST["STAT_Y_SIZE"]);
  printf(" &nbsp offset X <input name='STAT_X_OFFS' size=5 value='%s' ${cw_ro}> \n",$_POST["STAT_X_OFFS"]);
  printf(" &nbsp offset Y <input name='STAT_Y_OFFS' size=5 value='%s' ${cw_ro}> <br><br>\n",$_POST["STAT_Y_OFFS"]);

  printf("Margins: &nbsp&nbsp Top <input name='MARGIN_TOP' size=5 value='%s' ${cw_ro}> \n",$_POST["MARGIN_TOP"]);
  printf(" &nbsp Bottom <input name='MARGIN_BOTTOM' size=5 value='%s' ${cw_ro}> \n",$_POST["MARGIN_BOTTOM"]);
  printf(" &nbsp Left <input name='MARGIN_LEFT' size=5 value='%s' ${cw_ro}> \n",$_POST["MARGIN_LEFT"]);
  printf(" &nbsp Right <input name='MARGIN_RIGHT' size=5 value='%s' ${cw_ro}> <br><br>\n",$_POST["MARGIN_RIGHT"]);

  printf("<B>X Axis</B> &nbsp&nbsp  title <input name='LABEL_X' size=15 value='%s' ${cw_ro}> &nbsp&nbsp \n",$_POST["LABEL_X"]);
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
  printf("Number of divisions <input name='NDIVX' size=4 value='%s' ${cw_ro}> \n",$_POST["NDIVX"]);
  printf("&nbsp Tick option <input name='TICK_X' size=1 value='%s' ${cw_ro}> <br>\n",$_POST["TICK_X"]);
  echo "&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp ";
  printf("X miminum <input name='XMIN' size=7 value='%s' ${cw_ro}> &nbsp&nbsp ",$_POST["XMIN"]);
  printf("X maximum <input name='XMAX' size=7 value='%s' ${cw_ro}> <br><br>\n",$_POST["XMAX"]);

  printf("<br><B>Y Axis</B> &nbsp&nbsp  title <input name='LABEL_Y' size=15 value='%s' ${cw_ro}> &nbsp&nbsp \n",$_POST["LABEL_Y"]);
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
  printf("Number of divisions <input name='NDIVY' size=4 value='%s' ${cw_ro}> \n",$_POST["NDIVY"]);
  printf("&nbsp Tick option <input name='TICK_Y' size=1 value='%s' ${cw_ro}> <br>\n",$_POST["TICK_Y"]);
  echo "&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp ";
  printf("Y miminum <input name='YMIN' size=7 value='%s' ${cw_ro}> &nbsp&nbsp ",$_POST["YMIN"]);
  printf("Y maximum <input name='YMAX' size=7 value='%s' ${cw_ro}> <br><br>\n",$_POST["YMAX"]);

  if ($_POST["HSTYPE"]=='H2D' || $_POST["HSTYPE"]=='P2D') {
    printf("<br><B>Z Axis</B> &nbsp&nbsp  title <input name='LABEL_Z' size=15 value='%s' ${cw_ro}> &nbsp&nbsp \n",$_POST["LABEL_Z"]);
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
    echo "&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp ";
    printf("Z miminum <input name='ZMIN' size=7 value='%s' ${cw_ro}> &nbsp&nbsp ",$_POST["ZMIN"]);
    printf("Z maximum <input name='ZMAX' size=7 value='%s' ${cw_ro}> <br><br>\n",$_POST["ZMAX"]);

    echo"<B>3D plot rendering</B>: &nbsp&nbsp Engine <select name='TIMAGE'>";
    foreach (array("","HIST","IMAGE","AUTO") as $val) {
      $SELECTED[$val] = ( ($_POST["TIMAGE"] == $val) ? "SELECTED" : "") ;
      echo "<option ".$SELECTED[$val]."> ".$val." </option>";
    }
    echo "</select>";
    echo "&nbsp&nbsp Angles (deg): ";
     printf("Theta <input name='THETA' size=5 value='%s' ${cw_ro}> &nbsp&nbsp ",$_POST["THETA"]);
     printf("Phi <input name='PHI' size=5 value='%s' ${cw_ro}> <br><br>",$_POST["PHI"]);     
  }

  echo"<B>Miscellanea</B>:<br>";
  printf("<br>Stats option <input name='STATS' size=7 value='%s' ${cw_ro}> &nbsp&nbsp ",$_POST["STATS"]);
  printf("Disable Stats Transparency <input ${cw_dis} type='checkbox' name='STATTRANSP' value='1' %s>&nbsp&nbsp\n",
	   $_POST["STATTRANSP"] ? "checked" : "");
  printf("Fill Style <input name='FILLSTYLE' size=2 value='%s' ${cw_ro}> &nbsp&nbsp ",$_POST["FILLSTYLE"]);
  printf("Fill Color <input name='FILLCOLOR' size=2 value='%s' ${cw_ro}><br>\n",$_POST["FILLCOLOR"]);
  printf("Line Width <input name='LINEWIDTH' size=2 value='%s' ${cw_ro}> &nbsp&nbsp ",$_POST["LINEWIDTH"]);
  printf("Line Style <input name='LINESTYLE' size=2 value='%s' ${cw_ro}> &nbsp&nbsp ",$_POST["LINESTYLE"]);
  printf("Line Color <input name='LINECOLOR' size=2 value='%s' ${cw_ro}><br>\n",$_POST["LINECOLOR"]);
  printf("Marker Style <input name='MARKERSTYLE' size=2 value='%s' ${cw_ro}> &nbsp&nbsp ",$_POST["MARKERSTYLE"]);
  printf("Marker Color <input name='MARKERCOLOR' size=2 value='%s' ${cw_ro}>\n",$_POST["MARKERCOLOR"]);
  printf("Marker Size <input name='MARKERSIZE' size=2 value='%s' ${cw_ro}><br>\n",$_POST["MARKERSIZE"]);
  //printf("Pad Color <input name='PADCOLOR' size=2 value='%s' ${cw_ro}><br><br>\n",$_POST["PADCOLOR"]);

  printf("<B>ROOT Draw options</B> <input name='DRAWOPTS' size=20 value='%s' ${cw_ro}><br>\n",$_POST["DRAWOPTS"]);
  printf("ROOT Draw options for reference <input name='REFDRAWOPTS' size=20 value='%s' ${cw_ro}><br>\n",$_POST["REFDRAWOPTS"]);
  // printf("Display Refresh Time (s)  <input name='REFRESH' size=5 value='%s' ${cw_ro}><br>\n",$_POST["REFRESH"]);

  printf("<br>Draw with area <B>normalized</B> to (leave blank for no normalization) ".
         "<input name='NORM' size=3 value='%s' ${cw_ro}><br><br>\n",$_POST["NORM"]);


  printf("<br><B>Overplot reference</B> (if available)    <select name='REF'>");
  foreach (array("","NOREF","NONE","AREA","ENTR") as $val) {
      $SELECTED[$val] = ($_POST["REF"] == $val) ? "SELECTED" : "" ;
      echo "<option ".$SELECTED[$val]."> ".$val." </option>";
    }
  echo "</select> &nbsp&nbsp(Possible Normalization is NONE, same AREA, same ENTRies)<br>";

  // fit function
  echo "<br><B>Fit</B> to be performed on every display: Function ";
  echo "<input type='hidden' name='OLDFITFUN' value='".$_POST["FITFUN"]."'>\n";
  echo "<select name='FITFUN'><option value=0> none</option>\n";
  $stid = OCIParse($conn,"SELECT NAME,CODE,MUSTINIT,NP,NINPUT from FITFUNCTION order by NAME");
  OCIExecute($stid);
  $fitNP=0;
  while (OCIFetchInto($stid,$myfun,OCI_ASSOC )) {
    if ($_POST["FITFUN"] == $myfun["CODE"]) {
      $selected = "selected";
      $MUSTIN=$myfun["MUSTINIT"];
      $_POST["FITNP"]=$myfun["NP"];
      $_POST["FITNINPUT"]=$myfun["NINPUT"];
      $_POST["FITFUNNAME"] = $myfun["NAME"];
    }
    else {
      $selected = "";
    }
    echo "<option class='normal' value='".$myfun["CODE"]."' $selected>".$myfun["NAME"]."</option>\n";
  }
  ocifreestatement($stid);
  echo "</select><br>";
  echo "<input type='hidden' name='FITFUNNAME' value='".$_POST["FITFUNNAME"]."'>\n";
  echo "<input type='hidden' name='FITNP' value='".$_POST["FITNP"]."'>\n";
  echo "<input type='hidden' name='FITNINPUT' value='".$_POST["FITNINPUT"]."'>\n";

  
  if ($_POST["FITNINPUT"]>0 && $_POST["FITFUN"]>0) {
    echo "&nbsp&nbsp&nbsp<table align=center>";
    echo "<tr><td colspan=2 align=center> Input parameters for fit function:</td></tr>\n";
    $ip=0;
    while ($ip++ < $_POST["FITNINPUT"]) {
      $fcode=$_POST["FITFUN"];
      $dstid = OCIParse($conn,"begin OnlineHistDB.GetFitFunInputParname($fcode,$ip,:pname,:def); end;");
      ocibindbyname($dstid,":pname",$pname,$kvparsize);
      ocibindbyname($dstid,":def",$defv,$kfloatsize);
      OCIExecute($dstid);
      ocifreestatement($dstid);
      if ($mode != "display" && !array_key_exists("FITINPPAR_${ip}",$_POST)) {
        $_POST["FITINPPAR_${ip}"] = $defv;
      }
      echo "<tr><td><input type='text' name='FITINPPARNAME_${ip}' value='${pname}' READONLY>";
      printf("<td><input type='text' name='FITINPPAR_${ip}' value=%s></tr>\n",$_POST["FITINPPAR_${ip}"]);
    }
    echo "</table>";
  }
  if ($_POST["FITNP"]>0 && $_POST["FITFUN"]>0) {
    echo "&nbsp&nbsp&nbsp<table align=center>";
    echo "<tr><td colspan=2 align=center> Fit Range (leave blank to use full histogram range)</td></tr>\n";
    if($_POST["FITMAX"] < $_POST["FITMIN"]) { // means default values
      $_POST["FITMAX"] = $_POST["FITMIN"] ="";
    }
    printf("<tr><td> Fit Range Minimum  </td><td><input type='text' name='FITMIN' value=%s></td></tr>\n",$_POST["FITMIN"]);
    printf("<tr><td> Fit Range Maximum  </td><td><input type='text' name='FITMAX' value=%s></td></tr>\n",$_POST["FITMAX"]);

    echo "<tr><td colspan=2 align=center> Init. values for fit parameters ";
    if ( $MUSTIN ) 
      echo " (MUST BE PROVIDED):";
    else
      echo " (leave blank for auto):";
    echo "</td></tr>\n";
    $ip=0;
    while ($ip++ < $_POST["FITNP"]) {
      $fcode=$_POST["FITFUN"];
      $dstid = OCIParse($conn,"begin OnlineHistDB.GetFitFunParname($fcode,$ip,:pname,:def); end;");
      ocibindbyname($dstid,":pname",$pname,$kvparsize);
      ocibindbyname($dstid,":def",$defv,$kfloatsize);
      OCIExecute($dstid);
      ocifreestatement($dstid);
      if ($mode != "display" && $MUSTIN) {
        $_POST["FITPAR_${ip}"] = $defv;
      }
      if ($_POST["FITPAR_${ip}"] < -999990 && $MUSTIN ==0) { // very negative value means auto
        $_POST["FITPAR_${ip}"]="";
      }
      echo "<tr><td><input type='text' name='FITPARNAME_${ip}' value='${pname}' READONLY>";
      printf("</td><td><input type='text' name='FITPAR_${ip}' value=%s></td></tr>\n",$_POST["FITPAR_${ip}"]);
    }
    echo "</table>";
  }
  echo "<br>\n";

  printf("<br><B>Pattern to be overdrawn</B> on histogram (must be a ROOT file name)<br> \$GROUPDIR/online/Histograms/Reference/%s/",
         $_POST["TASKNAME"]);
  printf("<input name='DRAWPATTERN' size=40 value='%s' ${cw_ro}><br>\n",$_POST["DRAWPATTERN"]);


  if( $canwrite) {
    $action= ($mode == "display" && $htype != "SHID") ? "Update Display Options" : "Confirm";
    echo "<br><table><tr><td> <input align=right type='submit' name='Update_display' value='${action}'>";
    if ($mode == "display") {
      echo "<td> <input align='right' type='submit' name='Remove_DO' value='Remove Display Options'>\n";
    }
    echo "</tr></table><br>";
  }
  echo "</form>";
  
  if (!$setlist && $mode=='display' && $htype=="HSID" && $_POST["NHS"]>1) {
    echo "<br><br><form action='DispList.php' method='post'>\n";
    foreach (array("TASKNAME","HSALGO","HSTYPE","HSTITLE","NHS","HSDISPLAY") as $field)
      echo "<input type='hidden' name='${field}' value='".$_POST[$field]."'>\n";
    echo "<input type='hidden' name='hsid' value='${id}'>\n";
    echo "<center> <input type='submit' value='See Options for all histograms in set' name='analist'></center>\n";
    echo "</form>";
  }
}

function get_labels($hid,$nlx,$nly) {
 global $conn;
 global $debug;
 $nl=$nlx+$nly;
 $query=" SELECT ";
 for ($i=1 ; $i<=$nl; $i++) {
   $query .= "vlabel(VBINLABELS,${i}) LAB${i}";
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
   if ($i <= $nlx) {
     $_POST["LABX${i}"]=$hlabels["LAB${i}"];
   }
   else {
     $j=$i-$nlx;
     $_POST["LABY${j}"]=$hlabels["LAB${i}"];
   }
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
  $script=$_SERVER["PHP_SELF"];
  $setlist=0;
  if($mode == "display") {
    $script='write/histo_labels.php';
    foreach (array("NHS","NBINLABX","NBINLABY","NAME") 
	     as $field)  
      $_POST[$field]=$histo[$field];
    if($htype == 'HSID'){
      if ($histo["NHS"]>1) 
	echo "<B> labels for the the first histogram in set</B><br>";
    }
    get_labels(SingleHist($id),$histo['NBINLABX'],$histo['NBINLABY']);
    $nlabx = $_POST["NBINLABX"];
    $nlaby = $_POST["NBINLABY"];
  } 
  else {
    $nlabx = $nlaby = 20;
    if($_POST["nlabx"]  > $nlabx) {
      $nlabx = $_POST["nlabx"];
    }
    if($_POST["nlaby"]  > $nlaby) {
      $nlaby = $_POST["nlaby"];
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
  foreach (array("NHS","NBINLABX","NBINLABY","NAME") as $field)
    echo "<input type='hidden' name='${field}' value='".$_POST[$field]."'>\n";
  echo "<input type='hidden' name='id' value='${id}'>\n";
  echo "<input type='hidden' name='htype' value='${htype}'>\n";
  echo "<input type='hidden' name='nlabx' value='${nlabx}'>\n";
  echo "<input type='hidden' name='nlaby' value='${nlaby}'>\n";
  echo "<table> <tr><td>\n";
  for ($i=1 ; $i<=$nlabx; $i++) {
    echo "Label for X Bin ${i}  <input type='text' name='LABX${i}' size=15 value='".$_POST["LABX${i}"]."'><br>\n";
  }
  echo "</td><td>\n";
  for ($i=1 ; $i<=$nlaby; $i++) {
    echo "Label for Y Bin ${i}  <input type='text' name='LABY${i}' size=15 value='".$_POST["LABY${i}"]."'><br>\n";
  }
  echo "</td></tr></table>";
  
  if( $canwrite) {
    $action= ($mode == "display") ? "Update Bin Labels" : "Confirm";
    echo "<table><tr><td> <input align=right type='submit' name='Update_labels' value='${action}'></tr></table>";
  }
  echo "</form>";
  
  if ($mode != "display") {
    $nlabx += 20;
    $nlaby += 20;
    echo "<form action=${script} method='POST'>\n";
    foreach (array("NHS","NBINLABX","NBINLABY") as $field)
      echo "<input type='hidden' name='${field}' value='".$_POST[$field]."'>\n";
    echo "<input type='hidden' name='id' value='${id}'>\n";
    echo "<input type='hidden' name='htype' value='${htype}'>\n";
    echo "<input type='hidden' name='nlabx' value='${nlabx}'>\n";
    echo "<input type='hidden' name='nlaby' value='${nlaby}'>\n";
    for ($i=1 ; $i<=$nlabx; $i++) {
      echo "<input type='hidden' name='LABX${i}' value='".$_POST["LABX${i}"]."'>\n";
    }
    for ($i=1 ; $i<=$nlaby; $i++) {
      echo "<input type='hidden' name='LABY${i}' value='".$_POST["LABY${i}"]."'>\n";
    }
    echo "<center> <input type='submit' value='Need More Labels' name='moreLabels'></center>\n";
    echo "</form>";
  }
}

function get_ana_parameters($alg,$type,$aid,$ia,$hhid,$mask=1,$names=1,$values=1,$hcvalues=0,$fcode=0) {
  global $conn;
  global $kintsize,$kfloatsize,$kvparsize;
  if ($alg == "Fit" && $aid >0 ) {
    // retrieve associated fit function
    $dstid = OCIParse($conn,"begin OnlineHistDB.GetAnaInput(${aid},'${hhid}',1,:inp); end;");
    ocibindbyname($dstid,":inp",$fcode,$kfloatsize);
    OCIExecute($dstid);
    ocifreestatement($dstid);
  }

  $wantfitpars = ($alg == "Fit" && $fcode > 0);

  if($mask) {
    $stid = OCIParse($conn,"SELECT MASK from ANASETTINGS WHERE ANA='$aid' and HISTO='${hhid}'");
    OCIExecute($stid);
    OCIFetchInto($stid,$myanas,OCI_ASSOC );
    $_POST["a${ia}_mask"]=$myanas["MASK"];
    ocifreestatement($stid);
  }
  if($names || $values || $hcvalues) {
    if ($wantfitpars) { // special case
      $dstid = OCIParse($conn,"begin OnlineHistDB.GetFitAlgNpar($fcode,:np,:ni); end;");
    }
    else {
      $dstid = OCIParse($conn,"begin OnlineHistDB.GetAlgoNpar('$alg',:np,:ni); end;");
    }
    ocibindbyname($dstid,":np",$np,$kintsize);
    ocibindbyname($dstid,":ni",$ni,$kintsize);
    OCIExecute($dstid);

    $_POST["a${ia}_np"]=$np;
    $_POST["a${ia}_ni"]=$ni;
    if ($wantfitpars) { // number of input parameters of fit function
      $_POST["a${ia}_nfi"]=$ni-(($np-1)/2)-4;
    }
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
      if ($wantfitpars) { // special case
        $dstid = OCIParse($conn,"begin OnlineHistDB.GetFitAlgParname($fcode,$ip,:pname,:pdefv); end;");
      }
      else {
        $dstid = OCIParse($conn,"begin OnlineHistDB.GetAlgoParname('$alg',$ip,:pname,:pdefv); end;");
      }
      ocibindbyname($dstid,":pname",$pname,$kvparsize);
      ocibindbyname($dstid,":pdefv",$pdefv,$kfloatsize);
      OCIExecute($dstid);
      $_POST["a${ia}_p${ip}_name"]=$pname;
      $_POST["a${ia}_p${ip}_defv"]=$pdefv;
      ocifreestatement($dstid);
    }
    if($values && $type == "CHECK") {
      $dstid = OCIParse($conn,"begin OnlineHistDB.GetAnaSettings(${aid},'${hhid}',$ip,:warn,:alr); end;");
      ocibindbyname($dstid,":warn",$warn,$kfloatsize);
      ocibindbyname($dstid,":alr",$alr,$kfloatsize);
      OCIExecute($dstid);
      $_POST["a${ia}_p${ip}_w"]=$warn;
      $_POST["a${ia}_p${ip}_a"]=$alr;
      ocifreestatement($dstid);
          }
    if($hcvalues && $type == "HCREATOR") {
      $dstid = OCIParse($conn,"begin OnlineHistDB.GetHCSettings('${hhid}',$ip,:value); end;");
      ocibindbyname($dstid,":value",$warn,$kfloatsize);
      OCIExecute($dstid);
      $_POST["a${ia}_p${ip}"]=$warn;
      ocifreestatement($dstid);
    }
  }
  $ip=$np;
  if ($type == "CHECK") {
    if($values) {
      $dstid = OCIParse($conn,"begin OnlineHistDB.GetAnaConditionals(${aid},'${hhid}',:stb,:mst,:msf); end;");
      ocibindbyname($dstid,":stb",$stb,$kintsize);
      ocibindbyname($dstid,":mst",$mst,$kfloatsize);
      ocibindbyname($dstid,":msf",$msf,$kfloatsize);
      OCIExecute($dstid);
      $b=0;
      while ($b<32) {
        $_POST["a${ia}_stb_${b}"]= (($stb&pow(2,$b))>0 ? 1 : 0);
        $b++;
      }
      $_POST["a${ia}_mst"]=$mst;
      $_POST["a${ia}_msf"]=$msf;
      ocifreestatement($dstid);
    }
    while ($ip++<$np+$ni){
      // input parameters for check algorithms (names and values)
      $ipa=$ip-$np;
      if($names) {
        if ($wantfitpars) { // special case
          $dstid = OCIParse($conn,"begin OnlineHistDB.GetFitAlgParname('$fcode',$ip,:pname,:pdefv); end;");
        }
        else {
          $dstid = OCIParse($conn,"begin OnlineHistDB.GetAlgoParname('$alg',$ip,:pname,:pdefv); end;");
        }

        ocibindbyname($dstid,":pname",$pname,$kvparsize);
        ocibindbyname($dstid,":pdefv",$pdefv,$kfloatsize);
        OCIExecute($dstid);
        $_POST["a${ia}_i${ipa}_name"]=$pname;
	$_POST["a${ia}_i${ipa}_defv"]=$pdefv;
        ocifreestatement($dstid);
      }
      if($values) {
        $dstid = OCIParse($conn,"begin OnlineHistDB.GetAnaInput(${aid},'${hhid}',$ipa,:inp); end;");
        ocibindbyname($dstid,":inp",$inp,$kfloatsize);
        OCIExecute($dstid);
        $_POST["a${ia}_i${ipa}_v"]=$inp;
        ocifreestatement($dstid);
      }
    }
  }
}




function fitfunctionselect($ia,$mode) {
  global $conn;
  if($mode=="newfit") {
    echo "<tr><td>Fit Function </td><td><select name='a${ia}_i1_v' $ro>\n";
    $stid = OCIParse($conn,"SELECT NAME,CODE,MUSTINIT from FITFUNCTION order by NAME");
    OCIExecute($stid);
    while (OCIFetchInto($stid,$myfun,OCI_ASSOC )) {
      $selected = ($_POST["a${ia}_i1_v"] == $myfun["CODE"]) ? "selected":"";
      echo "<option class='normal' value='".$myfun["CODE"].
        "' $selected>".$myfun["NAME"]."</option>\n";
      $funMI[$myfun["CODE"]]=$myfun["MUSTINIT"];
    }
    ocifreestatement($stid);
    echo "</select></td></tr>";
    foreach ($funMI as $key => $value) 
      echo "<input type='hidden' name='mi_$key' value='$value'>\n";

    echo "<tr><td align=center colspan=2> ";
    echo "<a href=\"javascript:window.open('../fitdoc.php','gal','width=600,height=400');void(0);\"> ";
    echo " see function descriptions </a></td></tr>\n";

  }
  else {
    $fcode=$_POST["a${ia}_i1_v"];
    echo "<input type='hidden' name='a${ia}_i1_v' value='$fcode'>\n";
    $stid = OCIParse($conn,"SELECT NAME,MUSTINIT from FITFUNCTION where CODE=$fcode");
    OCIExecute($stid);
    OCIFetchInto($stid,$myfun,OCI_ASSOC);
    echo "<tr><td>Fit Function </td><td><span class='normal'>".$myfun["NAME"]." </span></td></tr>\n";
    echo "<input type='hidden' name='mi_$fcode' value='".$myfun["MUSTINIT"]."'>\n";
  }
  
}

function histo_analysis($id,$htype,$mode) {
  global $canwrite;
  global $histo;
  global $debug;
  global $conn;
  $ro=  ($canwrite ? "" : "DISABLED");
  $script=$_SERVER["PHP_SELF"];
  $firstana=$lastana=1;
  $showpars=1;
  if($mode == "display") {
    $script='write/histo_analysis.php';
    $action= "Update";
    foreach (array("TASKNAME","HSALGO","HSTYPE","HSTITLE","NHS","NANALYSIS","NAME") 
	     as $field)
      $_POST[$field]=$histo[$field];
    $lastana=$histo["NANALYSIS"]; 
    // get analysis entries
    $ia=0;
    $hsid=HistoSet($id);
    $stid = OCIParse($conn,"SELECT AID,ALGORITHM,ANADOC,ANAMESSAGE from ANALYSIS WHERE HSET=$hsid order by AID");
    OCIExecute($stid);
    while (OCIFetchInto($stid,$myana,OCI_ASSOC )) {
      $ia++;
      $_POST["a${ia}_id"]=$myana["AID"];
      $_POST["a${ia}_alg"]=$myana["ALGORITHM"];
      $_POST["a${ia}_doc"]=$myana["ANADOC"];
      $_POST["a${ia}_mes"]=$myana["ANAMESSAGE"];
    }
    ocifreestatement($stid);

    // get parameters to be shown
    $hhid=($htype == "HID") ? $id : "${hsid}/1";
    $ia=0;
    while ($ia++<$lastana) {
      get_ana_parameters($_POST["a${ia}_alg"],"CHECK",$_POST["a${ia}_id"],$ia,$hhid);
    }

    // show parameters only for single histograms
    $showpars= ($htype == "HID" || $_POST["NHS"]==1);
  }
  else if ($mode == "update") {
    $firstana=$lastana=$_POST["Iana"];
    $action="Confirm";
  }
  else if ($mode == "newana") {
    $action="Confirm";
    $fcode = ($_POST["a1_alg"] == 'Fit') ? $_POST["a1_i1_v"] : 0;
    $_POST["a1_id"]=0;
    get_ana_parameters($_POST["a1_alg"],"CHECK",0,1,0,0,1,0,0,$fcode);
    // default values for conditional variables
    $_POST["a1_stb_0"] =1;
    $_POST["a1_stb_1"] =1;
    $_POST["a1_stb_2"] =1;
    $_POST["a1_mst"] = $_POST["a1_msf"] ="";
  }
  else if ($mode == "newfit") {
    $action="Set Fit Parameters";
    get_ana_parameters($_POST["a1_alg"],"CHECK",0,1,0,0,1,0,0,0);
  }
  else if ($mode == "new") {
    $action="Set Parameters";
  }
  // loop on analyses 
  for ($ia=$firstana; $ia<=$lastana; $ia++) {
    echo "<table align='center' rules=none border=1 cellpadding=0> <tr><td><form action='${script}' method='POST'>\n"; 
    foreach (array("TASKNAME","HSALGO","HSTYPE","HSTITLE","NHS","NANALYSIS","NAME") as $field)
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
      // selection of algorithm among the available ones
      echo "Analysis <select name='a${ia}_alg'>\n";
      $stid = OCIParse($conn,"SELECT ALGNAME,ALGDOC from ALGORITHM WHERE ALGTYPE='CHECK' order by ALGNAME");
      OCIExecute($stid);
      while (OCIFetchInto($stid,$myalg,OCI_ASSOC )) {
	echo "<option class='normal'>".$myalg["ALGNAME"]."</option>\n";
	$algdoc[$myalg["ALGNAME"]]=$myalg["ALGDOC"];
      }
      ocifreestatement($stid);
      echo "</select>";
      echo "<a href=\"javascript:window.open('../algdoc.php?type=CHECK','gal','width=800,height=400');void(0);\"> ";
      echo " see algorithm descriptions </a>\n";
      if (isset($algdoc))
	foreach ($algdoc as $key => $value) 
	  echo "<input type='hidden' name='doc_$key' value='$value'>\n";
    }
    if ($mode != 'display') {
      echo "<p><table align=\"center\"><tr><td>Documentation <td><textarea valign='center' cols='75' rows='3' name='a${ia}_doc'".
        ($canwrite ? "" : "readonly")
        .">".$_POST["a${ia}_doc"]."</textarea></tr>\n";
      echo "<tr><td>Specific Message (when going in warning/alarm) <td><textarea valign='center' cols='75' rows='1' name='a${ia}_mes'".
        ($canwrite ? "" : "readonly")
        .">".$_POST["a${ia}_mes"]."</textarea></tr>\n";
      echo "</table><br>\n";
    }
    else {
      echo "<input type='hidden' name='a${ia}_doc' value='".$_POST["a${ia}_doc"],"'>\n";
      echo "<input type='hidden' name='a${ia}_mes' value='".$_POST["a${ia}_mes"],"'>\n";
    }

    if($showpars && $_POST["a${ia}_mask"]) echo "&nbsp <B><font color=red>(masked)</font></B>";
    echo "<br>";
    if ($mode == "newana" || $mode == "newfit") echo "<p>".$_POST["doc_".PutUnderscore($_POST["a${ia}_alg"])]."</p>";
    if ($mode == 'update') { // print some messages
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
      $docstid = OCIParse($conn,"SELECT ALGDOC from ALGORITHM where ALGNAME='".$_POST["a${ia}_alg"]."'");
      OCIExecute($docstid, OCI_DEFAULT);
      OCIFetchInto($docstid, $myalgdoc, OCI_ASSOC );
      echo "<p>".$myalgdoc["ALGDOC"]."</p>";
      ocifreestatement($docstid);
    }
    if ($showpars &&  ($_POST["a${ia}_np"]>0 || $_POST["a${ia}_ni"]>0) ) {
      if ($_POST["a${ia}_ni"]>0) { // input parameters
        echo "\n<table align='center'><thead><tr><td><B>Input Parameter</B><td><B>Value</B></tr></thead>\n";
        $ip=0;
        while ($ip++<$_POST["a${ia}_ni"]){
          if ($ip==1 && $_POST["a${ia}_alg"] == "Fit") { //special case:convert parameter to fit function name
            fitfunctionselect($ia,$mode);
          }
          else {
            if($_POST["a${ia}_alg"] == "Fit") {
              if($ip==5 && $_POST["a${ia}_nfi"]>0) {
                echo "<tr><td colspan=2>Input parameters for fit function: ";
              }
              if($ip== (5+$_POST["a${ia}_nfi"])) {
                echo "<tr><td colspan=2>Init. values for fit parameters: ";
                if ( $_POST["mi_".$_POST["a${ia}_i1_v"]])
                  echo " (MUST BE PROVIDED)";
                else
                  echo " (leave blank for auto)";
                echo "</td></tr>\n";
              }
            }
            printf("<tr><td> <input READONLY type='text' size=15 name='a${ia}_i${ip}_name' value='%s'>\n",$_POST["a${ia}_i${ip}_name"]);
            if($mode == "newana" || $mode == "newfit") { 
              $_POST["a${ia}_i${ip}_v"] = $_POST["a${ia}_i${ip}_defv"]; 
            }
            $fw=myfloatformat($_POST["a${ia}_i${ip}_v"]);
            printf("</td><td> <input type='text' size=8 name='a${ia}_i${ip}_v' value='$fw'></td></tr>\n",$_POST["a${ia}_i${ip}_v"]);
          }
        }
        echo "</table>";
      }
      if ($_POST["a${ia}_np"]>0) {
        echo "<table align='center'><thead><tr><td><B>Parameter</B><td><B>Warning</B><td><B>Alarm </B></tr></thead>\n";
        $ip=0;
        while ($ip++<$_POST["a${ia}_np"]){
          printf("<tr><td> <input READONLY type='text' size=15 name='a${ia}_p${ip}_name' value='%s'>\n",$_POST["a${ia}_p${ip}_name"]);
	  if($mode == "newana" || $mode == "newfit") {
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
      echo "<br><table align='center'><tr><td><B> Conditions required to perform analysis:</B></td></tr>\n ";
      $query="select IBIT,TEXT from CONDITIONS order by IBIT";
      $condstm = OCIParse($conn,$query);
      OCIExecute($condstm);
      
      while (OCIFetchInto($condstm, $cond, OCI_ASSOC)) {
        $bit = $cond['IBIT'];
        printf("<tr><td>%s<td><input $ro type='checkbox' name='a${ia}_stb_%d' value='1' %s></td></tr>\n",
               $cond['TEXT'], $bit, $_POST["a${ia}_stb_${bit}"]  ? "checked" : "");
      }
      printf("<tr><td> Minimum statistics per bin <td><input type='text' size=5 name='a${ia}_mst' value='%s'></td></tr>\n",$_POST["a${ia}_mst"]);
      printf("<tr><td> Minimum fraction of bins above stat limit <td><input type='text' size=5 name='a${ia}_msf' value='%s'></td></tr>\n",$_POST["a${ia}_msf"]);
      echo "</table>\n";
      ocifreestatement($condstm);
    }
    else {
      $ip=0;
      while ($ip++<$_POST["a${ia}_np"]) {
	printf("<input type='hidden' name='a${ia}_p${ip}_name' value='%s'>\n",$_POST["a${ia}_p${ip}_name"]);
        printf("<input type='hidden' name='a${ia}_p${ip}_w' value='%s'>\n",$_POST["a${ia}_p${ip}_w"]);
        printf("<input type='hidden' name='a${ia}_p${ip}_a' value='%s'>\n",$_POST["a${ia}_p${ip}_a"]);
      }
      $ip=0;
      while ($ip++<$_POST["a${ia}_ni"]) {
	printf("<input type='hidden' name='a${ia}_i${ip}_name' value='%s'>\n",$_POST["a${ia}_i${ip}_name"]);
        printf("<input type='hidden' name='a${ia}_i${ip}_v' value='%s'>\n",$_POST["a${ia}_i${ip}_v"]);
      }
      $b=0;
      while ($b<32) {
        printf("<input type='hidden' name='a${ia}_stb_${b}' value='%s'>\n",$_POST["a${ia}_stb_${b}"]);
        $b++;
      }
      printf("<input type='hidden' name='a${ia}_mst' value='%s'>\n",$_POST["a${ia}_mst"]);
      printf("<input type='hidden' name='a${ia}_msf' value='%s'>\n",$_POST["a${ia}_msf"]);
    }
    if ($canwrite) {
      echo "<table align='center'><tr>";
      $disabled = ($mode == "display" && $_POST["a${ia}_alg"] == "Fit" && $htype == "HSID" && $_POST["NHS"]>1 ) ? "disabled" : "";
      echo "<td> <input $disabled  type='submit' name='Update_analysis' value='${action}'>\n";
      if ($mode=='display' && $showpars) {
	$maction= $_POST["a${ia}_mask"] ? "Unmask" : "Mask";
	echo "<td> <input type='submit' name='SetMask_Analysis' value='${maction}'>\n";
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
  $script=$_SERVER["PHP_SELF"];
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

  $refOk=0;
  if($mode == "display" && $refOk) {
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
    echo "<table><tr><td> <input align=center type='submit' name='Update_task' value='${action}'></tr></table>";
  }
  echo "</form>";

}


?>
