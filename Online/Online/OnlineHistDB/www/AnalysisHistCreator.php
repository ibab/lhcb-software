
<HTML>
 <HEAD>
 <LINK REL=STYLESHEET TYPE="text/css" HREF="styles_screen.css">
</HEAD>
<body class=listing>
<?
include 'util.php';
include 'dbforms.php';
echo "<center><H2> Definition of \"Analysis\" Histogram </H2>";

if (array_key_exists("Alg",$_POST)) {
  // specify input histograms (or set) and parameters
  $alg=$_POST["Alg"];
  $nin=$_POST["nin_".PutUnderscore($alg)];
  $doc=$_POST["doc_".PutUnderscore($alg)];
  echo "Algorithm: <span class=normal> $alg </span><br>"; 
  echo "$doc <br><br>";
  echo "<form action='write/create_analysisHist.php' method='POST'>\n"; 
  echo "Title &nbsp <input type='text' name='hctitle' size=60 value='".$_POST["hctitle"]."'><br>\n";
  echo "<input type='hidden' name=nin value=$nin>\n";
  echo "<input type='hidden' name=alg value=$alg>\n";
  if (!$nin) {
    echo "Input Histogram Set ID <input type=text name='SOURCESET' size=7><br>\n";
  } 
  else { 
    for ($ih=1; $ih<=$nin; $ih++) {
      echo "Input Histogram $ih <input type=text name='SOURCEH${ih}' size=7><br>\n";
    }
  } 
  echo "<input type='submit' name='Go' value='Create Analysis Histogram'> </form>";
}
else {
  // first call: choose algorithm and title
  $conn=HistDBconnect(1);
  if (!$canwrite) {
    echo "sorry, you don't have permission to write in the DB";
  }
  else {
    echo " You can define here an Histogram that is produced automatically by the Analysis Task from the normal Monitoring Histograms<br> <br> <br>  ";
    echo "<form action=$PHP_SELF method='POST'>\n"; 
    echo "Title &nbsp <input type='text' name=hctitle size=60><br>\n";
    echo "Choose Algorithm <select name='Alg'>\n";
    $stid = OCIParse($conn,"SELECT ALGNAME,ALGDOC,NINPUT from ALGORITHM WHERE ALGTYPE='HCREATOR'");
    OCIExecute($stid);
    while (OCIFetchInto($stid,$myalg,OCI_ASSOC )) {
      echo "<option class='normal'>".$myalg["ALGNAME"]."</option>\n";
      $algdoc[$myalg["ALGNAME"]]=$myalg["ALGDOC"];
      $algnin[$myalg["ALGNAME"]]=$myalg["NINPUT"];
    }
    echo "</select>";
    ocifreestatement($stid);
    foreach ($algdoc as $key => $value) 
      echo "<input type='hidden' name='doc_$key' value='$value'>\n";
    foreach ($algnin as $key => $value) 
      echo "<input type='hidden' name='nin_$key' value='$value'>\n";
    echo "<input type='submit' name='Go' value='Set Input Histograms'> </form>";
  }
}


echo "</center>";


ocilogoff($conn);
?>
</body></html>