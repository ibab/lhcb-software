<?
include 'util.php';
$conn=HistDBconnect(1);
$debug=0;

function showhisto_display($hid,$doid,$instance)
{
  global $canwrite;
  global $conn;
  $page=$_POST["ORIGINALNAME"];
  
  $query="select NAME,HSTYPE from VIEWHISTOGRAM where HID='$hid'";
  $hstid = OCIParse($conn,$query);
  OCIExecute($hstid);
  OCIFetchInto($hstid, $histo, OCI_ASSOC );
  if (ocirowcount($hstid) == 0) {
    echo "Histogram $hid is not known to DB";
  }
  else {
    echo " <a href=Histogram.php?hid=${hid}>".$histo["NAME"]."</a><br>";
    echo "<table align=right> <tr><td align=right>";
    if ($doid) {
      $act= $canwrite ? 'See/Update' : 'See';
      echo "Using Histogram Display Options specific to this page<br>";
      if($canwrite) {
	echo "<center><a class='submit' href='write/delete_opt.php?doid=${doid}&hid=${hid}&name=".
	  toGet($histo["NAME"])."&page=".toGet(${page})."'>  Delete Specific Options </a></center><br>\n";
      }
    }
    else {
      $act= $canwrite ? 'Define' : 0;
      $actd= $canwrite ? 'See/Update' : 'See';
      echo "Using Histogram Default Display Options. <a href=Histogram.php?hid=${hid}>$actd them</a> , or<br>";
    }
    if ($act) {
      $type=$histo["HSTYPE"];
      $getp=toGet($page);
      echo "<a href=shisto_display.php?doid=${doid}&hid=${hid}&page=${getp}&instance=${instance}&type=${type}>$act specific Display Options for this Histogram on this page </a>\n";
    }
    echo "</table>";
  }
  ocifreestatement($hstid);
}

function page_form($page,$mode) {
  global $canwrite;
  global $conn;
  if ($mode == "display") {
    $action=$_SERVER["PHP_SELF"]."?page=${page}";
    $submit= ($page=='new__' ? "Insert new page" : "Modify Page");
  }
  else {
    $action="write/page.php";
    $submit="Confirm";
  }
  echo "<form method='post' action='${action}'>\n";
  $readonly=$canwrite ? "" : "readonly";
  echo "<table align=\"center\" border=0><tr><td>\n";
  


  if ($page=="new__") {
    echo "Folder <select name='FOLDER'>\n";
    
    $stid = OCIParse($conn,"SELECT PAGEFOLDERNAME FROM PAGEFOLDER order by PAGEFOLDERNAME");
    OCIExecute($stid);
    while (OCIFetchInto($stid, $pagef, OCI_ASSOC )) 
      printf("<option class='normal' %s> %s </option>\n",($_POST["FOLDER"] == $pagef["PAGEFOLDERNAME"]) ? "selected" : "",
	     $pagef["PAGEFOLDERNAME"]);
    ocifreestatement($stid);
    
    echo "</select> <br>";
    //    echo "<tr><td> &nbsp </td><td> &nbsp&nbsp or \n";
    printf("or New Folder <input class='normal' type='text' size=40 name='NEWFOLDER' value='%s' $readonly ><br>\n",$_POST["NEWFOLDER"]);
  }
  else {
    printf("Folder <input class='normal' type='text' size=40 name='FOLDER' value='%s'>\n",$_POST["FOLDER"]);
  }
  printf("</td><td> &nbsp&nbsp Name <input class='normal' type='text' size=20 name='SHORTPAGENAME' value='%s' ><br>\n",$_POST["SHORTPAGENAME"]);
  if ($page!="new__") {
    echo "<input type='hidden' name='ORIGINALNAME'  value='".$_POST["ORIGINALNAME"]."'>\n";
  }
  echo "</td></tr></table>\n";
  echo "<table align=\"center\"><tr><td>Description <td><textarea valign='center' cols='50' rows='15' name='PAGEDOC'".
    " $readonly >".$_POST["PAGEDOC"]."</textarea></tr></table><br>\n";
  printf("File name containing optional ROOT background pattern  <input class='normal' type='text' size=30 name='PAGEPATTERN' value='%s'><br>\n",
         $_POST["PAGEPATTERN"]);
  echo "displays <span class='normal'> ".$_POST["NHISTO"]." </span> histograms:<br>";
  echo "<input type='hidden' name='NHISTO'  value='".$_POST["NHISTO"]."'>\n";
  echo "<table border=0 >\n";
  echo"<tr><td colspan=6><hr></tr>\n";
  if ($canwrite) {
    $maxf = ($mode == "display") ? $_POST["NHISTO"] : max(16,$_POST["NHISTO"]+5);
    echo "<input type='hidden' name='maxH' value='${maxf}'>\n";
    for ($i=1;$i<=$maxf;$i++) {
      echo "<tr>";
      if ($i<= $_POST["NHISTO"]) 
        printf("<td>Pad ID <input readonly type='text' size=6 name='SHID_SH${i}' value='%s'></td>",
               $_POST["SHID_SH${i}"]);
      printf("<td>Histogram ID <input type='text' size=9 name='HISTO_SH${i}' value='%s'></td>",
             $_POST["HISTO_SH${i}"]);
      $padFields= sprintf("<td>X min <input type='text' size=4 name='CENTER_X_SH${i}' value='%.3f'></td>".
                          "<td>Y min <input type='text' size=4 name='CENTER_Y_SH${i}' value='%.3f'></td>".
                          "<td>X max <input type='text' size=4 name='SIZE_X_SH${i}' value='%.3f'></td>".
                          "<td>Y max <input type='text' size=4 name='SIZE_Y_SH${i}' value='%.3f'></td>",
                          $_POST["CENTER_X_SH${i}"],$_POST["CENTER_Y_SH${i}"],
                          $_POST["SIZE_X_SH${i}"],$_POST["SIZE_Y_SH${i}"]);
      $ovFields = sprintf("<td colspan=4> Overlap on Pad ID <input type='text' size=6 name='MOTHERH_SH${i}' value=%d>".
                          " order <input type='text' size=1 name='IOVERLAP_SH${i}' value=%d></td>",
                          $_POST["MOTHERH_SH${i}"],$_POST["IOVERLAP_SH${i}"]);
      $delField= ($i<=$_POST["NHISTO"] ?  
                  "<td><B>Delete</B><input type='checkbox' name='REMOVE${i}' value=1> </td>\n" : "");
      if ($mode == "display") {
        if ($_POST["MOTHERH_SH${i}"]) {
          echo $ovFields;
        }
        else {
          echo $padFields;
        }
        echo "$delField </tr>";
      }
      else {
        echo "$padFields $delField </tr>";
        echo "<tr> <td align='right'> Or </td> $ovFields <td></td></tr>\n";
      }
      if ($i<=$_POST["NHISTO"]){
	echo "<tr><td colspan=6>";
	showhisto_display($_POST["HISTO_SH${i}"],$_POST["SDISPLAY_SH${i}"],$_POST["INSTANCE_SH${i}"]);
	echo"</td></tr><tr><td colspan=6><hr></tr>\n";
      }

      if ($i==$_POST["NHISTO"] && $maxf>$i )
	echo"</table><table border=0><tr><td colspan=6>&nbsp</tr><tr><td colspan=6> Add other histograms:</tr>";
    }
    echo "</table>";
    echo "<table align=right><tr><td><input type='submit' name='Edit' value='${submit}'></tr></table><br>\n";
  }
  else { // read only
    for ($i=1;$i<=$_POST["NHISTO"];$i++) {
      echo "<tr><td>Pad ID <span class='normal' >".$_POST["SHID_SH${i}"]."</span></td>".
        "<td>&nbsp Histogram ID <span class='normal' >".$_POST["HISTO_SH${i}"]."</span>";
      if($_POST["MOTHERH_SH${i}"]) {
        echo "<td colspan=4> &nbsp&nbsp Overlaps on Pad ID <span class='normal' >".$_POST["MOTHERH_SH${i}"]."</span>".
          " order <span class='normal' >".$_POST["IOVERLAP_SH${i}"]."</span></td>";
      }
      else {
	echo "<td>&nbsp&nbsp&nbsp X min <span class='normal' >".$_POST["CENTER_X_SH${i}"]."</span></td>".
          "<td>&nbsp&nbsp&nbsp Y min <span class='normal' >".$_POST["CENTER_Y_SH${i}"]."</span></td>".
          "<td>&nbsp&nbsp&nbsp X max <span class='normal' >".$_POST["SIZE_X_SH${i}"]."</span></td>".
          "<td>&nbsp&nbsp&nbsp Y max <span class='normal' >".$_POST["SIZE_Y_SH${i}"]."</span></td></tr>\n";    
      }
      echo "<tr><td colspan=6>";
      showhisto_display($_POST["HISTO_SH${i}"],$_POST["SDISPLAY_SH${i}"],$_POST["INSTANCE_SH${i}"]);
      echo"</td></tr><tr><td colspan=6><hr></tr>\n";
    }
  }
  echo "</form>";
  if ($page != 'new__' && (!$readonly) ) {
    echo "<hr><form method='post' action='write/deletePage.php'>\n";
    echo "<input type='hidden' name=PAGETODELETE value='".$_POST["ORIGINALNAME"]."'>\n";
    echo "<input type='submit' class=bad name='Delete' value='Delete This Page'>\n";
  }

 
} 

function show_pagefolder($sel='') {
  global $conn;
  $stid = OCIParse($conn,"SELECT PAGEFOLDERNAME FROM PAGEFOLDER $sel order by PAGEFOLDERNAME");
  OCIExecute($stid);
  $first=0;
  while (OCIFetchInto($stid, $pagef, OCI_ASSOC )) {
    if ($first == 0) {
      echo "<table rule=cols border=1 cellpadding=6><thead><tr><td><B>Folder</B></td><td><B>Page</B></td></tr></thead>";
      $first=1;
    }
    echo "<tr><TD><span class='normal'>".PageSimpleName($pagef["PAGEFOLDERNAME"]),"</td> <td>\n";
    $pstid = OCIParse($conn,"SELECT PAGENAME FROM PAGE where FOLDER='".$pagef["PAGEFOLDERNAME"]."' order by PAGENAME");
    OCIExecute($pstid);
    while (OCIFetchInto($pstid, $page, OCI_ASSOC )) {
      $getp=toGet($page["PAGENAME"]);
      $p=PageSimpleName($page["PAGENAME"]);
      echo "<a class=normal href='".$_SERVER["PHP_SELF"]."?page=${getp}'> $p</a><br>\n";
    }  
    ocifreestatement($pstid);
    show_pagefolder("WHERE PARENT='".$pagef["PAGEFOLDERNAME"]."'");
    echo "</td></tr>";
  }
  if ($first == 1) {
    echo "</table>";
  }
  ocifreestatement($stid);
}
?>
<HTML>
<HEAD>
<LINK REL=STYLESHEET TYPE="text/css" HREF="styles_screen.css">
  </HEAD>
<body class=listing>
  <?
if (array_key_exists("page",$_GET)) {
  $page=fromGet($_GET["page"]);
  if ($page != "new__" && (!array_key_exists("Edit",$_POST)) ) {
    $stid = OCIParse($conn,"SELECT * from PAGE where PAGENAME='${page}'");
    OCIExecute($stid);
    OCIFetchInto($stid, $mypage, OCI_ASSOC ) or die ("Don't know this page: $page <br>");
    foreach (array("PAGENAME","NHISTO","PAGEDOC","PAGEPATTERN","FOLDER")
             as $field)
      $_POST[$field]=$mypage[$field];
    $_POST["ORIGINALNAME"] = $_POST["PAGENAME"];
    $_POST["SHORTPAGENAME"] = PageSimpleName($_POST["PAGENAME"]);
    $shtid = OCIParse($conn,"SELECT * from SHOWHISTO where PAGE='${page}' order by MOTHERH DESC,HISTO,INSTANCE");
    OCIExecute($shtid);
    $i=0;
    while( OCIFetchInto($shtid, $showhisto, OCI_ASSOC )) {
      $i++;
      foreach (array("SHID","HISTO","INSTANCE","CENTER_X","CENTER_Y","SIZE_X","SIZE_Y",
                     "SDISPLAY","MOTHERH","IOVERLAP")
	       as $field)
	$_POST[$field."_SH${i}"]=$showhisto[$field];
    }
    ($i==$mypage["NHISTO"]) or die ("Severe inconsistency in Page record $page");
    ocifreestatement($shtid);
    ocifreestatement($stid);
  }
  if (!$canwrite && $page == "new__") 
    echo "sorry, you don't have permission to write on the DB";
  else {
    $mode = array_key_exists("Edit",$_POST) ? "update" : "display";
    page_form($page,$mode);
  }
}
else {
  show_pagefolder('WHERE PARENT is NULL');
}
ocilogoff($conn);

?>
</body></html>
