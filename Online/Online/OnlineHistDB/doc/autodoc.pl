#!/usr/bin/perl

#texygen($ARGV[0],"all","all");
$doHistDB = 1;
$doTask = 1;
$doHistogram = 1;
$doHistoOnPage = 1;
$doPage = 1;
$doEnv =1;

if ($doHistDB) {
  open HISTDB,">OnlineHistDB.tex";
  select HISTDB;
  texygen("../OnlineHistDB/OnlineHistDB.h","all","declarations of main DB objects");
  texygen("../OnlineHistDB/OnlineHistTask.h","OnlineTaskStorage","all");
  texygen("../OnlineHistDB/OnlineHistogram.h","OnlineHistogramStorage","all");
  texygen("../OnlineHistDB/OnlineHistPage.h","OnlinePageStorage","all");
  texygen("../OnlineHistDB/OnlineHistDB.h","declarations of main DB objects","all");
  close HISTDB;
}

if($doHistogram) {
  open HISTDB,">OnlineHistogram.tex";
  select HISTDB;
  texygen("../OnlineHistDB/OnlineHistogram.h","all","OnlineHistogramStorage");
  close HISTDB;
}

if($doTask) {
  open HISTDB,">OnlineHistTask.tex";
  select HISTDB;
  texygen("../OnlineHistDB/OnlineHistTask.h","all","OnlineTaskStorage");
  close HISTDB;
}

if($doPage) {
  open HISTDB,">OnlineHistPage.tex";
  select HISTDB;
  texygen("../OnlineHistDB/OnlineHistPage.h","all","OnlinePageStorage");
  close HISTDB;
}

if($doHistoOnPage) {
  open HISTDB,">OnlineHistoOnPage.tex";
  select HISTDB;
  texygen("../OnlineHistDB/OnlineHistoOnPage.h","all","all");
  close HISTDB;
}

if($doEnv) {
  open HISTDB,">OnlineHistDBEnv.tex";
  select HISTDB;
  texygen("../OnlineHistDB/OnlineHistDBEnv.h","all","all");
  close HISTDB;
}

sub texygen {
  $in = $_[0];
  $start = $_[1];
  $end = $_[2];

  $started = 0;
  $ended = 0;
  $isadec=0;
  $fundoc="";
  $isadoc=0;

  open IN,"<$in";
  for (<IN>) {
    chomp;
    if (/$start/ || $start eq "all") { $started = 1;}    
    if (/$end/ && !($end eq "all")) { $ended = 1;}
    if ($started && !$ended) {
      
      if(/^ +\/\/\/(.*)$/) {
	$fundoc .= $1."\n";
	$isadoc=1;
	$cont=0;
      }
      else {
	if ($isadoc) {
	  if (/([^\(]+) +([^\{]+)(\(.*)$/) {
	    $isadec=1;
	    $isadoc=0;
	    $funtype=$1;
	    $funname=$2;
	    $funarg=$3;
	    $cont=1;
	    if ($funarg =~ /(.*)\{.*/) {
	      $funarg=$1.";";
	      texywrite($funtype,$funname,$funarg,$fundoc);
	      $funtype=$funname=$funarg=$fundoc="";
	      $isadec=0;
	    }
	    else {
	      $funarg.="\n";
	    }
	  }
	  elsif(/typedef/) {
	    texenywrite($_);
	  }
	}
      }
      if ($isadec) {
	if(/^([^\{]*\) *(const|) *)(;|\{)/) {
	  $thesearg=$1.";";
	  ($cont) || ($funarg.= $thesearg);
	  $isadec=0;
	  texywrite($funtype,$funname,$funarg,$fundoc);
	  $funtype=$funname=$funarg=$fundoc="";
	}
	else {
	  ($cont) || ($funarg.= $_."\n");
	}
	$cont=0;
      }
    }
  }
  close IN;

}


sub texenywrite {
  print "\\item ".$_[0]."\n";
}

sub texywrite {
  $funtype = texaggiusta($_[0]);
  $funname = texaggiusta($_[1]);
  $funarg  = texaggiusta($_[2]);
  $fundoc  = texaggiusta($_[3]);
  $funarg  =~ s/,/,\\\\\\mbox{}~~~~~~~~~/g;
  print "\\item  ${funtype} \{\\bf ${funname}\}${funarg}\n\n";
  print "${fundoc}\n\n";
}

sub texaggiusta {
  $out = $_[0];
  $out =~ s/(_|&)/\\$1/g;
  $out =~ s/(<|>)/\$$1\$/g;
  return $out;
}
