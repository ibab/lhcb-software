<?
$DispOpt = array("LABEL_X",
"LABEL_Y",
"LABEL_Z",
"YMIN",
"YMAX",
"STATS",
"FILLSTYLE",
"FILLCOLOR",
"LINESTYLE",
"LINECOLOR",
"LINEWIDTH",
"DRAWOPTS",
"XMIN",
"XMAX",
"ZMIN",
"ZMAX",
"LOGX",
"LOGY",
"LOGZ",
"TIMAGE",
"REF",
"REFRESH",
"TIT_X_SIZE",
"TIT_X_OFFS",
"TIT_Y_SIZE",
"TIT_Y_OFFS",
"TIT_Z_SIZE",
"TIT_Z_OFFS",
"LAB_X_SIZE",
"LAB_X_OFFS",
"LAB_Y_SIZE",
"LAB_Y_OFFS",
"LAB_Z_SIZE",
"LAB_Z_OFFS",
"GRIDX",
"GRIDY",
"THETA",
"PHI");
function get_displayoptions($disp) {
  global $conn;
  $query="BEGIN ONLINEHISTDB.GET_DISPLAYOPTIONS($disp";
  $query .= ",:LABEL_X";
  $_POST["LABEL_X"] = str_repeat(" ",50);
  $query .= ",:LABEL_Y";
  $_POST["LABEL_Y"] = str_repeat(" ",50);
  $query .= ",:LABEL_Z";
  $_POST["LABEL_Z"] = str_repeat(" ",50);
  $query .= ",:YMIN";
  $_POST["YMIN"] = 9999999999.;
  $query .= ",:YMAX";
  $_POST["YMAX"] = 9999999999.;
  $query .= ",:STATS";
  $_POST["STATS"] = 9999999999.;
  $query .= ",:FILLSTYLE";
  $_POST["FILLSTYLE"] = 9999999999.;
  $query .= ",:FILLCOLOR";
  $_POST["FILLCOLOR"] = 9999999999.;
  $query .= ",:LINESTYLE";
  $_POST["LINESTYLE"] = 9999999999.;
  $query .= ",:LINECOLOR";
  $_POST["LINECOLOR"] = 9999999999.;
  $query .= ",:LINEWIDTH";
  $_POST["LINEWIDTH"] = 9999999999.;
  $query .= ",:DRAWOPTS";
  $_POST["DRAWOPTS"] = str_repeat(" ",50);
  $query .= ",:XMIN";
  $_POST["XMIN"] = 9999999999.;
  $query .= ",:XMAX";
  $_POST["XMAX"] = 9999999999.;
  $query .= ",:ZMIN";
  $_POST["ZMIN"] = 9999999999.;
  $query .= ",:ZMAX";
  $_POST["ZMAX"] = 9999999999.;
  $query .= ",:LOGX";
  $_POST["LOGX"] = 9999999999.;
  $query .= ",:LOGY";
  $_POST["LOGY"] = 9999999999.;
  $query .= ",:LOGZ";
  $_POST["LOGZ"] = 9999999999.;
  $query .= ",:TIMAGE";
  $_POST["TIMAGE"] = str_repeat(" ",5);
  $query .= ",:REF";
  $_POST["REF"] = str_repeat(" ",5);
  $query .= ",:REFRESH";
  $_POST["REFRESH"] = 9999999999.;
  $query .= ",:TIT_X_SIZE";
  $_POST["TIT_X_SIZE"] = 9999999999.;
  $query .= ",:TIT_X_OFFS";
  $_POST["TIT_X_OFFS"] = 9999999999.;
  $query .= ",:TIT_Y_SIZE";
  $_POST["TIT_Y_SIZE"] = 9999999999.;
  $query .= ",:TIT_Y_OFFS";
  $_POST["TIT_Y_OFFS"] = 9999999999.;
  $query .= ",:TIT_Z_SIZE";
  $_POST["TIT_Z_SIZE"] = 9999999999.;
  $query .= ",:TIT_Z_OFFS";
  $_POST["TIT_Z_OFFS"] = 9999999999.;
  $query .= ",:LAB_X_SIZE";
  $_POST["LAB_X_SIZE"] = 9999999999.;
  $query .= ",:LAB_X_OFFS";
  $_POST["LAB_X_OFFS"] = 9999999999.;
  $query .= ",:LAB_Y_SIZE";
  $_POST["LAB_Y_SIZE"] = 9999999999.;
  $query .= ",:LAB_Y_OFFS";
  $_POST["LAB_Y_OFFS"] = 9999999999.;
  $query .= ",:LAB_Z_SIZE";
  $_POST["LAB_Z_SIZE"] = 9999999999.;
  $query .= ",:LAB_Z_OFFS";
  $_POST["LAB_Z_OFFS"] = 9999999999.;
  $query .= ",:GRIDX";
  $_POST["GRIDX"] = 9999999999.;
  $query .= ",:GRIDY";
  $_POST["GRIDY"] = 9999999999.;
  $query .= ",:THETA";
  $_POST["THETA"] = 9999999999.;
  $query .= ",:PHI";
  $_POST["PHI"] = 9999999999.;
  $query .= "); END;";
  $dstid = OCIParse($conn,$query);
  ocibindbyname($dstid,":LABEL_X",$_POST["LABEL_X"],50);
  ocibindbyname($dstid,":LABEL_Y",$_POST["LABEL_Y"],50);
  ocibindbyname($dstid,":LABEL_Z",$_POST["LABEL_Z"],50);
  ocibindbyname($dstid,":YMIN",$_POST["YMIN"]);
  ocibindbyname($dstid,":YMAX",$_POST["YMAX"]);
  ocibindbyname($dstid,":STATS",$_POST["STATS"]);
  ocibindbyname($dstid,":FILLSTYLE",$_POST["FILLSTYLE"]);
  ocibindbyname($dstid,":FILLCOLOR",$_POST["FILLCOLOR"]);
  ocibindbyname($dstid,":LINESTYLE",$_POST["LINESTYLE"]);
  ocibindbyname($dstid,":LINECOLOR",$_POST["LINECOLOR"]);
  ocibindbyname($dstid,":LINEWIDTH",$_POST["LINEWIDTH"]);
  ocibindbyname($dstid,":DRAWOPTS",$_POST["DRAWOPTS"],50);
  ocibindbyname($dstid,":XMIN",$_POST["XMIN"]);
  ocibindbyname($dstid,":XMAX",$_POST["XMAX"]);
  ocibindbyname($dstid,":ZMIN",$_POST["ZMIN"]);
  ocibindbyname($dstid,":ZMAX",$_POST["ZMAX"]);
  ocibindbyname($dstid,":LOGX",$_POST["LOGX"]);
  ocibindbyname($dstid,":LOGY",$_POST["LOGY"]);
  ocibindbyname($dstid,":LOGZ",$_POST["LOGZ"]);
  ocibindbyname($dstid,":TIMAGE",$_POST["TIMAGE"],5);
  ocibindbyname($dstid,":REF",$_POST["REF"],5);
  ocibindbyname($dstid,":REFRESH",$_POST["REFRESH"]);
  ocibindbyname($dstid,":TIT_X_SIZE",$_POST["TIT_X_SIZE"]);
  ocibindbyname($dstid,":TIT_X_OFFS",$_POST["TIT_X_OFFS"]);
  ocibindbyname($dstid,":TIT_Y_SIZE",$_POST["TIT_Y_SIZE"]);
  ocibindbyname($dstid,":TIT_Y_OFFS",$_POST["TIT_Y_OFFS"]);
  ocibindbyname($dstid,":TIT_Z_SIZE",$_POST["TIT_Z_SIZE"]);
  ocibindbyname($dstid,":TIT_Z_OFFS",$_POST["TIT_Z_OFFS"]);
  ocibindbyname($dstid,":LAB_X_SIZE",$_POST["LAB_X_SIZE"]);
  ocibindbyname($dstid,":LAB_X_OFFS",$_POST["LAB_X_OFFS"]);
  ocibindbyname($dstid,":LAB_Y_SIZE",$_POST["LAB_Y_SIZE"]);
  ocibindbyname($dstid,":LAB_Y_OFFS",$_POST["LAB_Y_OFFS"]);
  ocibindbyname($dstid,":LAB_Z_SIZE",$_POST["LAB_Z_SIZE"]);
  ocibindbyname($dstid,":LAB_Z_OFFS",$_POST["LAB_Z_OFFS"]);
  ocibindbyname($dstid,":GRIDX",$_POST["GRIDX"]);
  ocibindbyname($dstid,":GRIDY",$_POST["GRIDY"]);
  ocibindbyname($dstid,":THETA",$_POST["THETA"]);
  ocibindbyname($dstid,":PHI",$_POST["PHI"]);
  OCIExecute($dstid);
  ocifreestatement($dstid);
}
?>
