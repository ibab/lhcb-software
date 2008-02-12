<?
$Installation="Pit";
$Reference_home="/group/online/Histograms/Reference";
function HistDBconnect($exitonfailure=0,$user="nobody",$password="nobody",$db="HISTDB") 
{  
  global $canwrite;

  if ($user == "nobody") {
    $user=$_COOKIE["user"];
    $password=$_COOKIE["password"];    
  }
  $canwrite= (($user == "HIST_ADMIN" || $user == "HIST_WRITER")  ? 1 : 0);
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
