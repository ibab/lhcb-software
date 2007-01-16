<? 
$canwrite=0;
// for the moment, web server must know the DB password to allow read/write or 
// read-only access. This won't be needed when the DB will be in production
// on the LHCB oracle server 
$VerySecretPassword="";

// ----------------------------------------------------
// functions for connecting to DB

function HistDBconnect($exitonfailure=0,$user="nobody",$password="nobody",$db="devdb10") 
{  
  global $canwrite;
  global $VerySecretPassword;
  if ($user == "nobody") {
    $user=$_COOKIE["user"];
    $password=$_COOKIE["password"];    
  }
  $canwrite= ( ($password == $VerySecretPassword) ? 1 : 0);  
  $theconnection=ocilogon($user,$VerySecretPassword,$db);    
  if ($exitonfailure) {
    if (!$theconnection) {
      $e = ocierror();
      print htmlentities($e['message']);
      exit;
    }
  }
  return $theconnection;
}

// ----------------------------------------------------
// functions for manipulating dates and special characters in SQL fields

function mydate($sqldate) {
  return date("d M Y",strtotime($sqldate));
}

function mytoday() {
  return date("d M Y",time());
}
function humantime($time) {
  return date("D  d F Y G:i",$time);
}

function sqlstring($string) {
  $string=preg_replace("/\\\\/","",$string);
  //  echo "string id ${string} <br>";
  $string=preg_replace("/'{1}/","''",$string);
  return $string;
}

function RemoveSpaces($string) {
 return   preg_replace("/^ *(.*\S) *$/","\${1}",$string);
}

function PutUnderscore($string) {
 return   preg_replace("/ /","_",$string);
}

function toGet($string) {
 return   preg_replace("/ /","%20",$string);
}

function fromGet($string) {
 return   preg_replace("/%20/"," ",$string);
}

// ----------------------------------------------------
// functions for handling DB's internal histogram ID :  ID for an histogram set is an integer number (hsid)
//                                                      ID for a single histogram is <setid>/<subnumber> (hid) 
// a generic id can be either an hsid or an hid

function HistoSet($histogram) {
  // get histo set id from generic id 
 return preg_replace("/\/.*$/","",$histogram);
}
function SingleHist($histogram) {
  // gets a hid from generic id (if input is hid, doesn't change it, if it's hsid, adds "/1")
 return preg_replace("/^(\d+)$/","\${1}/1",$histogram);
}

// ----------------------------------------------------
// various tricks
function myfloatformat($fl,$exp=4) {
  // nice display of floats 
  if ($fl==0 || (abs($fl)<pow(10,$exp) &&  abs($fl)>pow(10,-$exp)))
    return "%s";
  else
    return "%.3e";
 }

?>
