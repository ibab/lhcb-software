<?
include 'util.php';
$conn=HistDBconnect();
//$st= 
//ociparse($conn,"begin OnlineHistDB.DeclareCheckAlgorithm('CompareToReference',parameters('MaxDelta'),'compares histogram to reference, with maximum deviation in sigma'); end;");
//ociexecute($st);
//echo "page is ".$_GET["page"];
// $x = array("zero","uno","due");
// echo "$x[0] $x[2]<br>\n";
// echo count($x)."<br>\n";
// foreach ( $x as $field) {
//   echo $field."<br>\n";
// }
$query="BEGIN TESTGET(:PIPPO_X,:PIPPO_A,:F); END;";
$dstid = OCIParse($conn,$query);
$POST["xa"]="pippo";
ocibindbyname($dstid,":PIPPO_X",$POST["xx"]);
ocibindbyname($dstid,":PIPPO_A",$POST["xa"],5);
ocibindbyname($dstid,":F",$POST["xf"],5);
OCIExecute($dstid);
ocifreestatement($dstid);
$xx=$POST["xx"];
$xa=$POST["xa"];
$xf=$POST["xf"];
echo "ecco $xx<br>\n";
echo "ecco ${xa}--<br>\n";
echo "ecco $xf<br>\n";

foreach (array("a","b","c") as $x) {
  echo $x."\n<br>";
}


?>
