<?php 
$script= (isset($_COOKIE["login"]) && $_COOKIE["login"]) ? "mainframe.php" : "login.html";
?>
<HTML>
 <HEAD>
  <LINK REL=STYLESHEET TYPE="text/css" HREF="styles_screen.css">
  <TITLE>Online Histograms Web Interface </TITLE>
 </HEAD>
<frameset rows="100pixels,*" >
  <frame SCROLLING='no' MARGINWIDTH=0 MARGINHEIGHT=0 src="header.html" FRAMEBORDER="0" name="header">
   <frame MARGINWIDTH=0 MARGINHEIGHT=0 src=<?echo $script;?> FRAMEBORDER="0" name="mainframe">
  <noframes>
  Welcome to Online Histogram Web interface<br>
  <a href=<?echo $script;?>> Enter <\a>
  </noframes>
</frameset>
</html>

