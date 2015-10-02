<?
$Installation="Pit";
$Reference_home="/group/online/Histograms/Reference";
function HistDBconnect($exitonfailure=0,$user="nobody",$password="nobody",$db="lhcbonr_hist") 
{  
  

    
    
    global $canwrite;
  if ($user == "nobody" && isset($_COOKIE["login"])) {
    if ($_COOKIE["login"] == md5($_SESSION["user"] .$_SESSION["password"] . $_SESSION["histdb"] .$_SESSION['REMOTE_ADDR']."lhcbsalt")){
    	$user=$_SESSION["user"];
    	$password=$_SESSION["password"];   
    	$db=$_SESSION["histdb"];
    }
  }

  
  $canwrite= (($user == "HIST_ADMIN" || $user == "HIST_WRITER"
	        || $user == "LHCB_MON_GIACOMO" || $user == "LHCB_DEVMON_GIACOMO")  ? 1 : 0);
#echo "user: ", $user, ", pass: ", $password, ", db: ", $db;  

$theconnection=ocilogon($user,$password,$db);    
 #print $user;
 #print $password;
 #print $db;
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
