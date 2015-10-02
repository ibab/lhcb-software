<?php
include '../util.php';
$conn=HistDBconnect(1);
include '../dbforms.php'; 
?>
<HTML>
 <HEAD>
<LINK REL=STYLESHEET TYPE="text/css" HREF="../styles_screen.css">
 </HEAD>
<body>
<?php 

$taskname=$_POST["TASKNAME"];

function update_task($taskname,$conn) {
  global $debug;
  $out=0;
  for ($id=1;$id<=3;$id++) {
    $d=$_POST["SUBSYS${id}"];
    if ($d == "") $_POST["SUBSYS${id}"] = "NULL";
  }
  $command="begin OnlineHistDB.DeclareTask(Name => '${taskname}', ss1 =>'". $_POST["SUBSYS1"].
    "',ss2 =>'". $_POST["SUBSYS2"]. "',ss3 =>'". $_POST["SUBSYS3"]. 
    "',KRunOnPhysics => ". ($_POST["RUNONPHYSICS"] ? 1 :0).
    ",KRunOnCalib => ". ($_POST["RUNONCALIB"] ? 1 :0).
    ",KRunOnEmpty => ". ($_POST["RUNONEMPTY"] ? 1 :0).
    ",SFreq => ". $_POST["SAVEFREQUENCY"]."); end;";
  if($debug) echo "command is $command <br>\n";
  $stid = OCIParse($conn,$command);
  $r=OCIExecute($stid,OCI_DEFAULT);
  if ($r) {
    ocicommit($conn);
    ocifreestatement($stid);
    $out=1;
  }
  else
    echo "<font color=red> <B>Got errors from task.php </B></font><br>Writing aborted<br>\n";
  return $out;
}


?>
<H2 ALIGN="CENTER">Update record for Task <?php echo $taskname ?></H2><hr>
<?
$conn=HistDBconnect(1);

if ($_POST["Update_task"] == 'Confirm') {
  if (update_task($taskname,$conn))
    echo "Task record updated successfully<br><br>\n";
  else
    echo "<font color=red> <B>Got errors from update_task </B></font><br><br>\n";
} 
else {
  echo "Please check your data and confirm <br><br>";
  task_form($taskname,"Update");
}

ocilogoff($conn);
echo "<br> <a href='../Task.php?task=${taskname}'> Back to Task Record ${taskname} </a><br>";

?>