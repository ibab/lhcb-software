function displayIP() {
  document.getElementById('usersIP').innerHTML = "Your IP is <?php echo $_SERVER['REMOTE_ADDR']?>";
}
