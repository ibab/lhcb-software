<HEAD>
<meta http-equiv="no-cache">
<meta http-equiv="refresh" content="500; URL=<?php echo "$_SERVER[PHP_SELF]"; ?>">
  <LINK REL=STYLESHEET TYPE="text/css" HREF="styles_screen.css">
</HEAD>
<BODY>
<?
include 'util.php';
if (isset($_COOKIE["user"])) {
  $conn=HistDBconnect();
  if($conn) {
    echo "connected as <span class=\"connected_name\">". $_COOKIE["user"]."</span> ".
      ($canwrite ? "" : " <B>(READ-ONLY)</B>").
      "<br><a href=logout.php target=_parent> Logout </a><br><hr>\n";
  // get tasks, subsystems, pages
  $i=0;$task=array();
  $stid = OCIParse($conn,"select TASKNAME from TASK");
  OCIExecute($stid, OCI_DEFAULT);
  while( OCIFetchInto($stid, $row, OCI_NUM)) 
    $task[$i++]=$row[0];

  $task=getTasks($conn);

  $subsys=getSubSystems($conn);

  $folder=array();
  $page=getPages($conn);
  
  ocilogoff($conn);
  //  $alltask=implode("_%_",$task);
  //$allsubsys=implode("_%_",$subsys);
  //$allpage=implode("_%_",$page);
?>
<h4> Search Histogram</h4> 
<form action="HistogramList.php" METHOD="POST" target="mymain">
title contains <input type="Text" name="searchstring" size=10> <br>
Task <select name="searchtask"> <option> any
<?
   foreach ($task as $t) 
     echo "<option> $t";
?>
</select><br>
Subsystem <select name="searchsubsys"> <option> any
<?
   foreach ($subsys as $s) 
     echo "<option> $s";
?>
</select><br>
Page <select name="searchpage"> <option> any
<?
    foreach ($page as $p) {
     $fullp=$folder[$p]."/$p";
     echo "<option value=\"${fullp}\">$fullp";
    }
?>
</select> <br>
or HID <input type="Text" name="searchHID" size=10> <br>
<input type="submit" target="mymain" name="SearchForm" value="Search">
</form>
<hr>
<h4> Browse Histograms</h4> 
    
<?
    //echo "<form action=\"HistoBrowse.php\" METHOD=\"POST\" target=\"mymain\">\n"; 
    //echo "<input type='hidden' name='alltask' value='$alltask'>\n"; 
    //echo "<input type='hidden' name='allsubsys' value='$allsubsys'>\n"; 
    //echo "<input type='hidden' name='allpage' value='$allpage'>\n"; 
    //   echo "<a class='asalink' href='HistogramList.php?full=1' target='mymain'> Full list</a><br>\n"; 
    //   echo "<input class='asalink' type='submit'   name='bytask' value='By Task/Algorithm'><br>\n"; 
    //   echo "<input class='asalink'  type='submit'  name='bypage' value='By Page'><br>\n";
    //   echo "<input class='asalink' type='submit'  name='bysubsys' value='By Subsystem'><br>\n"; 
    //   echo "<input class='asalink' type='submit'  name='byalpha' value='Alphabetically'><br>\n";   
    //   echo "<input class='asalink' type='submit'   name='byfeature' value='By Analysis Features'>\n"; 
?>
 <a class='asalink' href='HistogramList.php?full=1' target='mymain'> Full list</a><br>
 <a class='asalink' href='HistoBrowse.php?bytask=1' target='mymain'> By Task/Algorithm</a><br>
 <a class='asalink' href='HistoBrowse.php?bypage=1' target='mymain'> By Page </a><br>
 <a class='asalink' href='HistoBrowse.php?bysubsys=1' target='mymain'> By Subsystem</a><br>
 <a class='asalink' href='HistoBrowse.php?byalpha=1' target='mymain'> Alphabetically</a><br>
 <a class='asalink' href='HistoBrowse.php?byfeature=1' target='mymain'> By Analysis Features</a><br>

<hr>

<h4> Page Configuration</h4> 
<a class='asalink' href="Viewpage.php" target="mymain"> View Pages</a><br>
<a class='asalink' href="Viewpage.php?page=new__" target="mymain"> Create new Page</a><br>
<hr>
<h4> Task Editor & <br> Reference Histograms</h4> 
 <form action="Task.php" METHOD="GET" target="mymain">
 Task <select name="task"> 
<?
 foreach ($task as $t) 
    if ($t != "ANALYSIS") echo "<option> $t";
?>
</select>
<input type="submit" target="mymain" name="Go" value="Go">
<hr>
<a href='AnalysisHistCreator.php' target=mymain> <h4> Create Analysis Histogram</h4> </a>
<?
  }
}
else {
  echo "session expired <br><a href='login.html' target=_parent> Login again </a>";
  
}?>
</BODY>