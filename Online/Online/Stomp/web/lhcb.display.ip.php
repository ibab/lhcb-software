function get_ip_address() {
  return "<?php echo $_SERVER['REMOTE_ADDR']?>";
}
function get_ip_name() {
  return "<?php echo gethostbyaddr($_SERVER['REMOTE_ADDR'])?>";
}
