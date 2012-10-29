<?php
// Small php script to redirect to the reference page of a class.
// Usage:  class.php?n=MyClass
// If the class is found, it redirects to the reference page, otherwise
// to the search resuls.
// If the class is not specified, redirects to the classes index page.

// check if we have the parameter 'n'
$n = $_GET["n"];
if ($n) {
  // Open the database of classes.
  $classes = dba_open("classes.db", "r", "db4");
  if (!$classes) {
    echo "Cannot open the database of classes.\\n";
    exit;
  }
  // check if it is in the database
  if (dba_exists($n, $classes)) {
    // get the url
    $url = dba_fetch($n, $classes);
  } else {
    // not found: search for it
    $url = "search.php?query=" . urlencode($n);
  }
  // Close the database
  dba_close($classes);
} else {
  // not given, go to all classes.
  $url = "classes.html";
}

// Redirect
header("Location: ". $url);
?>
