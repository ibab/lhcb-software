<HTML>
 <HEAD>
 <LINK REL=STYLESHEET TYPE="text/css" HREF="styles_screen.css">
</HEAD><?
 include '../util.php';
 if ($_POST["Confirm"]) {
   $conn=HistDBconnect(1);
   $page=$_POST["PAGETODELETE"];
   
   $command="begin :out := OnlineHistDB.DeletePage('${page}'); end;";$stid = OCIParse($conn,$command);
   
   $stid = OCIParse($conn,$command);
   ocibindbyname($stid,":out",$out);
   $r=OCIExecute($stid,OCI_DEFAULT);
   if ($r) {
     ocicommit($conn);
     ocifreestatement($stid);
    echo "<font color=blue> <B>Page $page has been deleted </font><br>\n";
   }
   else
     echo "<font color=red> <B>Got errors from deletePage.php </B></font><br>Deleting aborted<br>\n";
   ocilogoff($conn);
 }
 else {
   echo "Please confirm you want to delete page ".$_POST["PAGETODELETE"]."\n";
   echo "<hr><form method='post' action='$_SYSTEM[PHP_SELF]'>\n";
   echo "<input type='hidden' name=PAGETODELETE value='".$_POST["PAGETODELETE"]."'>\n";
   echo "<input type='submit' class=bad name='Confirm' value='Confirm'>\n";
 }
?>

</body>
</html>
