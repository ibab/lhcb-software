<?
$Installation="test";
$Reference_home="/afs/cern.ch/user/g/ggrazian/www/lhcb/OnlineHistDBdev/upload_area";
function HistDBconnect($exitonfailure=0,$user="nobody",$password="nobody",$db="devdb10") 
{  
  global $canwrite;
  if ($user == "nobody" && isset($_COOKIE["login"])) {
    session_start();
    if ($_COOKIE["login"] == 
	md5($_SESSION["user"] .$_SESSION["password"] . $_SESSION["histdb"] .$_SESSION['REMOTE_ADDR']."lhcbsalt")
	) {
      $user=$_SESSION["user"];
      $password=$_SESSION["password"];   
      $db=$_SESSION["histdb"];
    }
  }

  $canwrite= (($user == "HIST_ADMIN" || $user == "HIST_WRITER"
               || $user == "LHCB_MON_GIACOMO" || $user == "LHCB_DEVMON_GIACOMO")  ? 1 : 0);
  $theconnection=ocilogon($user,$password,$db);    
  if ($exitonfailure) {
    if (!$theconnection) {
      $e = ocierror();
      print htmlentities($e['message']);
      exit;
    }
  }
  return $theconnection;
}
?>