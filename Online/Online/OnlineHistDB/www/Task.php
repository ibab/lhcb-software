<HTML>
 <HEAD>
<LINK REL=STYLESHEET TYPE="text/css" HREF="styles_screen.css">
 </HEAD>
<body>
<?
include 'util.php';
include 'dbforms.php';
$task=$_GET["task"];
if(isset($task)) {
  $conn=HistDBconnect(1);
  // query general variables${htype}
  $query="select SUBSYS1,SUBSYS2,SUBSYS3,RUNONPHYSICS,RUNONCALIB,RUNONEMPTY,SAVEFREQUENCY,REFERENCE FROM TASK where TASKNAME='${task}'";
  $stid = OCIParse($conn,$query);
  OCIExecute($stid);
  OCIFetchInto($stid, $taskRec, OCI_ASSOC );
  if (ocirowcount($stid) == 0) {
    echo "Task $task is not known to DB";
  }
  else {
    echo "<center><H2> Record of Task <font color='red'> $task </font></H2>";
    task_form($task,"display");
  }
  ocilogoff($conn);
}
else {
  echo "No task specified";
}
?>

</body>
</html>