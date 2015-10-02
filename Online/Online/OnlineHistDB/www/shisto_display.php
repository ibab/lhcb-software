<?php 
include 'util.php'; 
$conn=HistDBconnect(1);
include 'dbforms.php'; 
$act = $canwrite ? 'Display/Update' : '';

$doid=$_GET["doid"];
$hid=$_GET["hid"];
$page=fromGet($_GET["page"]);
$instance=$_GET["instance"];
?>
<HTML>
 <HEAD>
<LINK REL=STYLESHEET TYPE="text/css" HREF="styles_screen.css">
 </HEAD>
<body>
<H2 ALIGN="CENTER"><?php echo $act ?> Default Display Options for histogram <?php echo $hid ?> <br>on Page <?php echo $page ?></H2><hr>

<?php
if (!$doid) {
  $stid = OCIParse($conn,"begin :out := OnlineHistDB.GetBestDO('${hid}','${page}'); end;");
  ocibindbyname($stid,":out",$doid,10);
  OCIExecute($stid);
}
$histo["NHS"]=1; $histo["DISPLAY"]=$doid;$histo["PAGE"]=$page;
$histo["HSTYPE"]=$type;
$histo["INSTANCE"]=$instance;

histo_display($hid,"SHID",$doid ? "display" : "update");

ocilogoff($conn);
echo "<hr><p><a href='Viewpage.php?page=${page}'> Back to Page Record $page </a><br>";
?>

</body>
</html>
