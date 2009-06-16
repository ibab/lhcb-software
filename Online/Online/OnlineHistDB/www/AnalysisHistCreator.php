
<HTML>
 <HEAD>
 <LINK REL=STYLESHEET TYPE="text/css" HREF="styles_screen.css">
</HEAD>
<body class=listing>
<?
include 'util.php';
include 'dbforms.php';
echo "<center><H2> Definition of \"Analysis\" Histogram </H2>";
$conn=HistDBconnect(1);
if (array_key_exists("Alg",$_POST)) {
  // specify input histograms (or set) and parameters
  $alg=$_POST["Alg"];
  $nin=$_POST["nin_".PutUnderscore($alg)];
  $np =$_POST["np_".PutUnderscore($alg)];
  $gs =$_POST["gs_".PutUnderscore($alg)];
  $doc=$_POST["doc_".PutUnderscore($alg)];
  echo "Algorithm: <span class=normal> $alg </span><br>"; 
  echo "$doc <br><br>";
  echo "<form action='write/create_analysisHist.php' method='POST'>\n"; 
  echo "Title &nbsp <input type='text' name='hctitle' size=60 value='".$_POST["hctitle"]."'><br>\n";
  echo "<input type='hidden' name=nin value=$nin>\n";
  echo "<input type='hidden' name=np value=$np>\n";
  echo "<input type='hidden' name=alg value='$alg'>\n";
  echo "<input type='hidden' name=gs value='$gs'>\n";
  if ($gs) {
    echo "Input Histogram Set ID <input type=text name='SOURCESET' size=7><br>\n";
  } 
  for ($ih=1; $ih<=$nin; $ih++) {
    echo "Input Histogram ID $ih <input type=text name='SOURCEH${ih}' size=7><br>\n";
  }
  if ($np>0) {
    echo "<table><tr><td>Input Parameter values: <td>";
    get_ana_parameters($alg,"HCREATOR",0,1,0,0,1,0);
    for ($ip=1; $ip<=$np; $ip++) {
      echo "&nbsp&nbsp&nbsp ".$_POST["a1_p${ip}_name"]."  &nbsp <input type=text name='a1_p${ip}' size=10><br>\n";
    }
    echo "</tr></table>\n";
  }
  echo "<input type='submit' name='Go' value='Create Analysis Histogram'> </form>";
}
else {
  // first call: choose algorithm and title
  if (!$canwrite) {
    echo "sorry, you don't have permission to write in the DB";
  }
  else {
    echo " You can define here a virtual Histogram that is produced automatically by the Presenter or at Analysis time from the normal Monitoring Histograms<br> <br> <br>  ";
    echo "<form action=$_SERVER[PHP_SELF] method='POST'>\n"; 
    echo "Title &nbsp <input type='text' name=hctitle size=60><br>\n";
    echo "Choose Algorithm <select name='Alg'>\n";
    $stid = OCIParse($conn,"SELECT ALGNAME,ALGDOC,NINPUT,NPARS,GETSET from ALGORITHM WHERE ALGTYPE='HCREATOR'");
    OCIExecute($stid);
    while (OCIFetchInto($stid,$myalg,OCI_ASSOC )) {
      echo "<option class='normal'>".$myalg["ALGNAME"]."</option>\n";
      $algdoc[$myalg["ALGNAME"]]=$myalg["ALGDOC"];
      $algnin[$myalg["ALGNAME"]]=$myalg["NINPUT"];
      $algnp[$myalg["ALGNAME"]]=$myalg["NPARS"];
      $alggs[$myalg["ALGNAME"]]=$myalg["GETSET"];
    }
    echo "</select>";
    ocifreestatement($stid);
    foreach ($algdoc as $key => $value) 
      echo "<input type='hidden' name='doc_$key' value='$value'>\n";
    foreach ($algnin as $key => $value) 
      echo "<input type='hidden' name='nin_$key' value='$value'>\n";
    foreach ($algnp as $key => $value) 
      echo "<input type='hidden' name='np_$key' value='$value'>\n";
    foreach ($alggs as $key => $value) 
      echo "<input type='hidden' name='gs_$key' value='$value'>\n";
    echo "<input type='submit' name='Go' value='Set Input Histograms'> </form>";
    echo "<br><a href=\"javascript:window.open('algdoc.php?type=HCREATOR','gal','width=800,height=400');void(0);\"> ";
    echo " see algorithm descriptions </a>\n";
  }
}


echo "</center>";


ocilogoff($conn);
?>
</body></html>