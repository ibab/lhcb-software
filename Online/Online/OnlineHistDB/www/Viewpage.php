<HTML>
 <HEAD>
 <LINK REL=STYLESHEET TYPE="text/css" HREF="styles_screen.css">
</HEAD>
<body class=listing>
<?
$debug=0;
include 'util.php';

function showhisto_display($hid,$doid,$instance)
{
  global $canwrite;
  global $conn;
  
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
       echo "Using Special Display Options for this Histogram in this page<br>";
    }
    else {
      $act= $canwrite ? 'Specify' : 0;
      echo "Using Histogram Default Display Options<br>";
    }
    $page=$_POST["PAGENAME"];
    if ($act) {
      $type=$histo["HSTYPE"];
      $getp=toGet($page);
      echo "<a href=shisto_display.php?doid=${doid}&hid=${hid}&page=${getp}&instance=${instance}&type=${type}>$act special Display Options for Histogram in this page </a>\n";
    }
    echo "</table>";
  }
  ocifreestatement($hstid);
}

function page_form($page) {
  global $canwrite;
  global $conn;
  echo "<form method='post' action='write/page.php'>\n";
  $readonly=$canwrite ? "" : "readonly";
  $epreadonly= ($page=="new__") ? $readonly :  "readonly";
  echo "<table align=\"center\" border=0><tr><td>\n";
  


  if ($page=="new__") {
    echo "Folder <select name='FOLDER'>\n";
    
    $stid = OCIParse($conn,"SELECT PAGEFOLDERNAME FROM PAGEFOLDER");
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
     printf("Folder <input class='normal' type='text' size=40 name='FOLDER' value='%s' READONLY>\n",$_POST["FOLDER"]);
  }
  printf("</td><td> &nbsp&nbsp Name <input class='normal' type='text' size=20 name='PAGENAME' value='%s' $epreadonly ><br>\n",PageSimpleName($_POST["PAGENAME"]));

  echo "</td></tr></table>\n";
  echo "<table align=\"center\"><tr><td>Description <td><textarea valign='center' cols='50' rows='2' name='PAGEDOC'".
    " $readonly >".$_POST["PAGEDOC"]."</textarea></tr></table><br>\n";
  echo "displays <span class='normal'> ".$_POST["NHISTO"]." </span> histograms:<br>";
  echo "<table border=0 >\n";
  echo"<tr><td colspan=6><hr></tr>\n";
  if ($canwrite) {
    for ($i=1;$i<=max(16,$_POST["NHISTO"]+5);$i++) {
      printf("<tr><td>Histogram ID <input type='text' size=7 name='HISTO_SH${i}' value='%s'>".
	     "<td>X position <input type='text' size=5 name='CENTER_X_SH${i}' value='%.2f'></td>".
	     "<td>Y position <input type='text' size=5 name='CENTER_Y_SH${i}' value='%.2f'></td>".
	     "<td>X size <input type='text' size=5 name='SIZE_X_SH${i}' value='%.2f'></td>".
	     "<td>Y size <input type='text' size=5 name='SIZE_Y_SH${i}' value='%.2f'></td>".
	     ($i<=$_POST["NHISTO"] ?  "<td><B>Delete</B><input type='checkbox' name='REMOVE${i}' value=1> </td></tr>\n" : ""),
	     $_POST["HISTO_SH${i}"],$_POST["CENTER_X_SH${i}"],$_POST["CENTER_Y_SH${i}"],
	     $_POST["SIZE_X_SH${i}"],$_POST["SIZE_Y_SH${i}"]);
      if ($i<=$_POST["NHISTO"]){
	echo "<tr><td colspan=6>";
	showhisto_display($_POST["HISTO_SH${i}"],$_POST["SDISPLAY_SH${i}"],$_POST["INSTANCE_SH${i}"]);
	echo"</td></tr><tr><td colspan=6><hr></tr>\n";
      }

      if ($i==$_POST["NHISTO"])
	echo"</table><table border=0><tr><td colspan=6>&nbsp</tr><tr><td colspan=6> Add other histograms:</tr>";
    }
    echo "</table>";
    echo "<table align=right><tr><td><input type='submit' name='Go' value='".($page=='new__' ? "Insert new page" : "Update Page")."'></tr></table><br>\n";
  }
  else {
    for ($i=1;$i<=$_POST["NHISTO"];$i++) {
      echo "<tr><td>Histogram ID <span class='normal' >".$_POST["HISTO_SH${i}"]."</span>".
	"<td>&nbsp&nbsp&nbsp X position <span class='normal' >".$_POST["CENTER_X_SH${i}"]."</span></td>".
	"<td>&nbsp&nbsp&nbsp Y position <span class='normal' >".$_POST["CENTER_Y_SH${i}"]."</span></td>".
	"<td>&nbsp&nbsp&nbsp X size <span class='normal' >".$_POST["SIZE_X_SH${i}"]."</span></td>".
	"<td>&nbsp&nbsp&nbsp Y size <span class='normal' >".$_POST["SIZE_Y_SH${i}"]."</span></td></tr>\n";    
      echo "<tr><td colspan=6>";
      showhisto_display($_POST["HISTO_SH${i}"],$_POST["SDISPLAY_SH${i}"],$_POST["INSTANCE_SH${i}"]);
      echo"</td></tr><tr><td colspan=6><hr></tr>\n";
    }
  } 
} 

function show_pagefolder($sel='') {
  global $conn;
  $stid = OCIParse($conn,"SELECT PAGEFOLDERNAME FROM PAGEFOLDER $sel");
  OCIExecute($stid);
  $first=0;
  while (OCIFetchInto($stid, $pagef, OCI_ASSOC )) {
    if ($first == 0) {
      echo "<table rule=cols border=1 cellpadding=6><thead><tr><td><B>Folder</B></td><td><B>Page</B></td></tr></thead>";
      $first=1;
    }
    echo "<tr><TD><span class='normal'>".$pagef["PAGEFOLDERNAME"],"</td> <td>\n";
    $pstid = OCIParse($conn,"SELECT PAGENAME FROM PAGE where FOLDER='".$pagef["PAGEFOLDERNAME"]."'");
    OCIExecute($pstid);
    while (OCIFetchInto($pstid, $page, OCI_ASSOC )) {
      $getp=toGet($page["PAGENAME"]);
      $p=PageSimpleName($page["PAGENAME"]);
      echo "<a class=normal href=$PHP_SELF?page=${getp}> $p</a><br>\n";
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



$conn=HistDBconnect(1);

if (array_key_exists("page",$_GET)) {
  $page=fromGet($_GET["page"]);
  if ($page != "new__") {
    $stid = OCIParse($conn,"SELECT * from PAGE where PAGENAME='${page}'");
    OCIExecute($stid);
    OCIFetchInto($stid, $mypage, OCI_ASSOC ) or die ("Don't know this page: $page <br>");
    foreach (array("PAGENAME","NHISTO","PAGEDOC","FOLDER")
             as $field)
      $_POST[$field]=$mypage[$field];
    $shtid = OCIParse($conn,"SELECT * from SHOWHISTO where PAGE='${page}' order by HISTO,INSTANCE");
    OCIExecute($shtid);
    $i=0;
    while( OCIFetchInto($shtid, $showhisto, OCI_ASSOC )) {
      $i++;
      foreach (array("HISTO","INSTANCE","CENTER_X","CENTER_Y","SIZE_X","SIZE_Y","SDISPLAY")
	       as $field)
	$_POST[$field."_SH${i}"]=$showhisto[$field];
    }
    ($i==$mypage["NHISTO"]) or die ("Severe inconsistency in Page record $page");
    ocifreestatement($shtid);
    ocifreestatement($stid);
  }
  if (!$canwrite && $page == "new__") 
    echo "sorry, you don't have permission to write on the DB";
  else 
    page_form($page);
}
else {
  show_pagefolder('WHERE PARENT is NULL');
}
ocilogoff($conn);

?>
</body></html>