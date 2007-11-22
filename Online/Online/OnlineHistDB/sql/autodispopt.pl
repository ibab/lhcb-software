#!/usr/bin/perl

open LIST,"<displayoptions";
$ix=0;
for (<LIST>) {
  chomp;
  ( $opt, $typ ) = split;
  (length($typ) == 0 || length($opt) == 0) && next;
  $size=$typ;
  $size =~ s/^.*\((\d+)\).*$/${1}/;
  $typ =~ s/\(.*\)//;
  $opts[$ix++]=$opt;
  if ($typ eq "VARCHAR2") {
    $sqltyp{$opt} =  "VARCHAR2";
    $size{$opt} =  $size;
    $ctyp{$opt} = "OCIString *";
    $mytyp{$opt} = "STRING";
    $textyp{$opt} = "(string of max length ${size})";
  }
  if ($typ eq "FLOAT") {
    $sqltyp{$opt} =  "FLOAT";
    $ctyp{$opt} = "OCINumber";
    $mytyp{$opt} = "FLOAT";
    $textyp{$opt} = "(float)";
  }
  if ($typ eq "NUMBER") {
    $sqltyp{$opt} =  "INT";
    $ctyp{$opt} = "OCINumber";
    $mytyp{$opt} = "INT";
    $textyp{$opt} = "(int)";
  }
}
close LIST;
$nopts=@opts;

open CSTRUCT,">OHDB_ocitypes.h";
print CSTRUCT '// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/OnlineHistDB/sql/autodispopt.pl,v 1.2 2007-11-22 17:38:35 ggiacomo Exp $
#ifndef OHDB_OCITYPES_H
#define OHDB_OCITYPES_H 1
struct dispopt_s
{
';
for ($i=0; $i<$nopts ; $i++) {
  $opt=$opts[$i];
  print CSTRUCT "  ".$ctyp{$opt}." ".$opt.";\n";  
}
print CSTRUCT "};
typedef struct dispopt_s dispopt;

struct dispopt_ind
{
  OCIInd _atomic;\n";


for ($i=0; $i<$nopts ; $i++) {
  $opt=$opts[$i];

  print CSTRUCT "  OCIInd ".$opt.";\n";
}

print CSTRUCT "};
typedef struct dispopt_ind dispoptInd;

#endif // OHDB_OCITYPES_H\n";

close CSTRUCT;

open CINIT,">initDO.cpp";
print CINIT "    m_do.reserve($nopts);\n";
for ($i=0; $i<$nopts ; $i++) {
  $opt=$opts[$i];
  print CINIT "    m_do.push_back(new OnlineDisplayOption(\"${opt}\",OnlineDisplayOption::".
    $mytyp{$opt}.",\n					   (void*) &(m_dispopt->${opt}),\n".
      "					   &(m_dispopt_null->${opt}), this ) );\n";
    
}
close CINIT;


open PLSQL,">Getdo.sql";
print PLSQL "procedure GET_DISPLAYOPTIONS(theDOID IN int\n";
$il=0;
for ($i=0; $i<$nopts ; $i++) {
  $opt=$opts[$i];
  print PLSQL ",${opt} OUT ".$sqltyp{$opt};
  if($il++ == 5) {
    print PLSQL "\n";
    $il =0;
  }
}
print PLSQL ') is
 mydo dispopt; 
begin
 SELECT OPT INTO mydo FROM DISPLAYOPTIONS WHERE DOID = theDOID;
';

for ($i=0; $i<$nopts ; $i++) {
  $opt=$opts[$i];
  print PLSQL " ${opt} := mydo.${opt};\n";
}
print PLSQL "end GET_DISPLAYOPTIONS;\n";
close PLSQL;



open PHPV,">dispopt.php";
print PHPV '<?
$DispOpt = array(';


for ($i=0; $i<$nopts ; $i++) {
  $opt=$opts[$i];
  print PHPV "\"${opt}\"".($i == ($nopts-1) ? ");" :",")."\n";
}

print PHPV 'function get_displayoptions($disp) {
  global $conn;
  $query="BEGIN ONLINEHISTDB.GET_DISPLAYOPTIONS($disp";
';
for ($i=0; $i<$nopts ; $i++) {
  $opt=$opts[$i];
  print PHPV "  \$query .= \",:${opt}\";\n";
  if ($size{$opt}) {
    print PHPV "  \$_POST\[\"".$opt."\"\] = str_repeat(\" \",".$size{$opt}.");\n";
  }
  else {
    print PHPV "  \$_POST\[\"".$opt."\"\] = 9999999999.;\n";
  }
}
print PHPV '  $query .= "); END;";
  $dstid = OCIParse($conn,$query);
';
for ($i=0; $i<$nopts ; $i++) {
  $opt=$opts[$i];
  $thesize = $size{$opt} ? ",".$size{$opt} : "";
  print PHPV "  ocibindbyname(\$dstid,\":${opt}\",\$_POST\[\"".$opt."\"\]".$thesize.");\n"
}
print PHPV '  OCIExecute($dstid);
  ocifreestatement($dstid);
}
?>
';
close PHPV;


open TEXDOC,">dodoc.tex";
for ($i=0; $i<$nopts ; $i++) {
  $opt=$opts[$i];
  $type=$textyp{$opt};
  $opt =~ s/_/\\_/g;
  print TEXDOC '\item{'.$opt.'}  '.$type."\n";
}
close TEXDOC;


