<? if(isset($_POST["DBLogin"])) {
  setcookie("user",$_POST["user"],time()+10000);
  setcookie("password",$_POST["password"],time()+10000);
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
