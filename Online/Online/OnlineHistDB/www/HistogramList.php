<?
 // this scripts shows a list of histogram sets matching different kinds of queries
include 'util.php';
$conn=HistDBconnect();
?>
<HTML>
 <HEAD>
 <LINK REL=STYLESHEET TYPE="text/css" HREF="styles_screen.css">
</HEAD>
<body class=listing>
<?
$maxresults=50;
$debug=0;

// first is index in histogram list 
$first= (array_key_exists("first",$_GET) ? $_GET["first"] : 1); 


if (!$conn) {
  $e = ocierror();
  print htmlentities($e['message']);
}
else {
  $onlyset="WHERE ( VH.NHS = 1 or VH.IHS=(select min(ihs) from VIEWHISTOGRAM where HSID=VH.HSID) )";
  $conta="SELECT COUNT(*) FROM VIEWHISTOGRAM VH ";
  // the following query gives ordered results with an index (IORDER) allowing to take only part of the list
  $showvars="SELECT row_number() OVER (ORDER BY TASK,ALGO,TITLE) as IORDER,VH.HID,VH.HSID,VH.NHS,VH.TASK,VH.ALGO,VH.TITLE FROM VIEWHISTOGRAM VH ";
  if (array_key_exists("query",$_POST)) {
    //script is calling itself, query passed as post hidden variable
    $query=preg_replace("/\\\'/","'",$_POST["query"]);
    $ncount=$_POST["ncount"];
  }
  else {
    //define query according to input variables
    unset($specs);
    // full list
    if(array_key_exists("full",$_GET)) $specs="$onlyset";
    // list by task
    if(array_key_exists("task",$_GET)) $specs=" $onlyset AND VH.TASK='".fromGet($_GET["task"])."'";
    // list by algorythm
    if(array_key_exists("algo",$_GET)) {
      if(preg_match("/^(.*)\/(.*)$/",fromGet($_GET["algo"]),$matched)) {
	$specs=" $onlyset AND VH.TASK='".$matched[1]."' AND VH.ALGO='".$matched[2]."'";
      }
      else {
	$specs="$onlyset";
	echo "I don't know this algorithm ".$_GET["algo"]." ... showing all histograms<br>";
      }
    }
    // list by subsystem
    if(array_key_exists("subsys",$_GET)) $specs=
      " , TASK T $onlyset AND VH.TASK=T.TASKNAME AND (T.SUBSYS1 ='".$_GET["subsys"].
      "' OR  T.SUBSYS2 ='".$_GET["subsys"]."' OR  T.SUBSYS3 ='".$_GET["subsys"]."')";
    // list by viewer page
    if(array_key_exists("page",$_GET)) $specs=
      " , SHOWHISTO SH, PAGE P $onlyset AND REGEXP_SUBSTR(SH.HISTO,'^(\d+)') = VH.HSID AND SH.PAGE = P.PAGENAME ".
      "AND SH.PAGEFOLDER = P.FOLDER AND SH.PAGEFOLDER||'/'||SH.PAGE='".fromGet($_GET["page"])."'";
    // list alphabetically
    if(array_key_exists("letter",$_GET)) $specs=
      " $onlyset AND UPPER(SUBSTR(VH.TITLE,1,1))=UPPER('".$_GET["letter"]."') ";
    // list by analysis features
    if(array_key_exists("feature",$_GET)) {
      if ($_GET["feature"] == 'Show Virtual Histograms') 
	$specs=",HISTOGRAM H $onlyset and VH.HID=H.HID AND H.ISANALYSISHIST=1";
      if ($_GET["feature"] == 'Show Histograms with automatic analysis') 
	$specs=",HISTOGRAMSET HS $onlyset AND VH.HSID=HS.HSID AND HS.NANALYSIS>0";
    }

    // search 
    if (array_key_exists("SearchForm",$_POST)) {
      echo "<table border=0 cellpadding=0> \n";
      echo"<tr><td><b>Your query:&nbsp&nbsp&nbsp</b><td>HID:</td><td>".($_POST["searchHID"] ? $_POST["searchHID"] : "any")."</td></tr>\n";
      echo"<tr><td></td><td>Task:</td><td>".$_POST["searchtask"]."</td></tr>\n";
      echo"<tr><td></td><td>SubSystem:</td><td>".$_POST["searchsubsys"]."</td></tr>\n";
      echo"<tr><td></td><td>Page:</td><td>".$_POST["searchpage"]."</td></tr>\n";
      
      if($_POST["searchHID"]) {
	$id=preg_replace("/\/\d+$/","",$_POST["searchHID"]);
	$specs=" $onlyset AND VH.HSID='".$id."'";
      } 
      else {
	if($_POST["searchpage"] != "any") {
	  $specs=",TASK T,SHOWHISTO SH, PAGE P $onlyset AND VH.TASK=T.TASKNAME ".
	    "AND REGEXP_SUBSTR(SH.HISTO,'^(\d+)') = VH.HSID AND SH.PAGE = P.PAGENAME ".
	    "AND SH.PAGEFOLDER = P.FOLDER AND SH.PAGEFOLDER||'/'||SH.PAGE='".$_POST["searchpage"]."'";
	} else {
	  $specs=",TASK T $onlyset AND VH.TASK=T.TASKNAME ";
	}
	if($_POST["searchstring"]) {
	  $sstrings=explode(" ",$_POST["searchstring"]);
	  echo"<tr><td></td><td>Strings in Title:</td><td>".implode(" AND ",$sstrings)."</td></tr>\n";
	  $specs.=" AND ( 1=1 ";
	  foreach ($sstrings as $s)
	    $specs.=" AND REGEXP_INSTR(UPPER(VH.TITLE||VH.SUBTITLE),UPPER('${s}'))>0";
	  $specs.=")";
	}
	else 
	  echo"<tr><td></td><td>Strings in Title:</td><td>any</td></tr>\n";
	if($_POST["searchtask"] != "any" ) 
	  $specs.=" AND VH.TASK='".$_POST["searchtask"]."'";
	if($_POST["searchsubsys"] != "any") 
	  $specs.=" AND (T.SUBSYS1 ='".$_POST["searchsubsys"].
	    "' OR  T.SUBSYS2 ='".$_POST["searchsubsys"].
	    "' OR  T.SUBSYS3 ='".$_POST["searchsubsys"]."')";
      }
      echo"</table><hr>";
    }

    // none of previous choices
    if (isset($specs) == 0) {
      echo "I don't understand what you want ... showing all histograms<br>";
      $specs="$onlyset";
    }  
    $query=$showvars.$specs;

    // first, count histograms matching the query
    if ($debug) echo "my query is ".$conta.$specs." <br>";
    $stid = OCIParse($conn,$conta.$specs);
    OCIExecute($stid, OCI_DEFAULT);
    OCIFetchInto($stid, $counts, OCI_NUM  );
    $ncount=$counts[0];
    ocifreestatement($stid);
  }

  if ($ncount>0) {
    $last=$first+$maxresults-1;
    $shlast=min($last,$ncount);
    echo "Histograms $first - $shlast of $ncount<br>\n";
    
    
    echo "<table   rules=\"groups\" border=1 cellspacing=15>
 <colgroup span=1 align=\"right\"> <colgroup span=1 align=\"right\"><colgroup span=1 align=\"center\">
 <colgroup span=1 align=\"left\">
 <thead>
 <tr align=center> <td>  </td><td><B>HID </B> </td><td><B>is  Set </B> </td>
 <td align=left>&nbsp&nbsp&nbsp<B> Full Name (Task/Algorithm/Title) </B></td> 
 </tr>
 <tbody>";
    
    // query histograms only in the range to be shown on this web page
    if ($debug) echo "my query is $query <br>";
    $stid = OCIParse($conn,"SELECT * FROM (${query}) WHERE IORDER>=${first} and IORDER<=${last}" );
    OCIExecute($stid, OCI_DEFAULT);
    while (OCIFetchInto($stid, $histo, OCI_ASSOC  )) 
      echo "<tr><td align='right'>".$histo["IORDER"]."</td><td align='right'> &nbsp &nbsp ".$histo["HSID"]."</td><td>".
	($histo["NHS"]>1 ? "<img src=\"hmultiple.gif\">" :"").
	"</td><td>&nbsp&nbsp&nbsp<a href='Histogram.php?hsid=".$histo["HSID"]."'>". 
	$histo["TASK"]."/".$histo["ALGO"]."/".$histo["TITLE"]."</a></td></tr>\n";
    echo "</table>";
    $next=$first+$maxresults;
    $prev=$first-$maxresults;
    // to navigate the histogram list, I call myself with the 'first' argument
    if($first>1) {
      echo "<form action='".$_SERVER['PHP_SELF']."?first=${prev}' METHOD='POST'>";
      echo "<input type='hidden' name='query' value=\"$query\">\n";
      echo "<input type='hidden' name='ncount' value='$ncount'>\n";
      echo "<input class='asalink' type='submit'  name='prev' value='previous $maxresults'>";
      echo "</form>";
    }
    if($next<$ncount) {
      echo "<form action='".$_SERVER['PHP_SELF']."?first=${next}' METHOD='POST'>";
      echo "<input type='hidden' name='query' value=\"$query\">\n";
      echo "<input type='hidden' name='ncount' value='$ncount'>\n";
      echo "<input class='asalink' type='submit'   name='next' value='next $maxresults'>";
      echo "</form>";
    }
    ocifreestatement($stid);
  }
  else { 
    echo "No histograms found matching your request<br>";
  }
  ocilogoff($conn);
}
?>
