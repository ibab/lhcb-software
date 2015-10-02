<?
include '../util.php';$conn=HistDBconnect(1);
?>
<HTML>
 <HEAD>
<LINK REL=STYLESHEET TYPE="text/css" HREF="../styles_screen.css">
 </HEAD>
<body>
<?php 

if ($_POST["DoUpload"]) {
  $task=$_POST["TASK"];
  echo "<center><H2> Uploading Reference Histograms for Task <font color='red'> $task </font></H2>";
  $refdir="${Reference_home}/${task}";
  $ref=$task."/".$_POST["DATATYPE"]."_".$_POST["STARTRUN"];
  $file = "${Reference_home}/${ref}";
  
  // get task's histogram list
  $query="select TITLE||SUBTITSTRING(SUBTITLE) from VIEWHISTOGRAM where TASK='${task}'";
  $stid = OCIParse($conn,$query);
  OCIExecute($stid);
  $hlist='';
  while (OCIFetchInto($stid, $his, OCI_ASSOC )) 
    $hlist .= " '".$his['NAME']."'";  
  ocifreestatement($stid);

  // update link to reference
  $command="UPDATE TASK set REFERENCE='$ref' where TASKNAME='${task}'";
  $stid = OCIParse($conn,$command);
  $r=OCIExecute($stid,OCI_DEFAULT);
  if ($r) {
    ocicommit($conn);
    ocifreestatement($stid);

    // copy uploaded file to right place
    (file_exists($refdir)) or (mkdir($refdir,0775));
    if( move_uploaded_file($_FILES['RefFile']['tmp_name'], $file))
      echo "Reference File for task $task uploaded successfully<br>\n"; 

    //  check uploaded file
    echo "Checking Reference File...<br>\n";
    //$command="LD_LIBRARY_PATH=/afs/cern.ch/sw/lcg/external/root/5.14.00d/slc4_ia32_gcc34/root/lib ./refcheck $task $file $hlist > out 2>&1";
    $command="./refcheck2  $task $file $hlist";
    system($command, $retval);
    if($retval != 0) 
      echo "<font color=red> Something wrong while executing the check command</font><br>";
  }
  else
    echo "<font color=red> <B>Got errors when updating REFERENCE link for task $task </B></font><br>Writing aborted<br>\n";
  
  ocilogoff($conn);
}
else {
  $task=$_GET["task"];
  echo "<center><H2> Upload Reference Histograms for Task <font color='red'> $task </font></H2>";

  echo "<form action='".$_SERVER["PHP_SELF"]."' method='POST' ENCTYPE='multipart/form-data'>\n"; 
  echo "<input type='hidden' name='TASK' value='${task}'>\n";
  echo "Valid from Run &nbsp <input align='right' type='text'  name='STARTRUN' value='1' size=8><br>\n";
  echo "Data Type &nbsp  <select name='DATATYPE'>";
  echo "<option SELECTED>default</option>";
  echo "<option>Physics</option>";
  echo "<option>Calibration</option>";
  echo "<option>Empty</option>";
  echo "<option>etc_etc</option>";
  echo "</select><br>";

  echo "Data File <input type='file' name='RefFile' size='80'><br>\n";
  echo "<input align=center type='submit' name='DoUpload' value='Upload'>";
  
  echo "</form>";
}
echo "<br> <a href='../Task.php?task=${task}'> Back to Task Record ${task} </a><br>";

?>