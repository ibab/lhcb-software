<? 
  session_start();
  if(isset($_POST["DBLogin"])) {
  $_SESSION["user"] = strtoupper($_POST["user"]);
  $_SESSION["password"] = $_POST["password"];
  $_SESSION["histdb"] = $_POST["database"];

  $cookie = md5 (
    $_SESSION["user"] .
    $_SESSION["password"] .
    $_SESSION["histdb"] .
    $_SESSION['REMOTE_ADDR'] .
    "lhcbsalt"
 );
  setcookie("login",$cookie);
}
include 'util.php';
?>
<HTML>
 <HEAD>
  <LINK REL=STYLESHEET TYPE="text/css" HREF="styles_screen.css">
  <TITLE>OnlineHistDB Test Interface </TITLE>

 </HEAD>

<frameset cols="20%,80%" >
<frame src="navigation.php" FRAMEBORDER="1" name="nav">
<frame src="main.php" FRAMEBORDER="0" name="mymain">
<noframes>
<a href = "main.php"> Main Page</a>
</noframes>
</frameset>
