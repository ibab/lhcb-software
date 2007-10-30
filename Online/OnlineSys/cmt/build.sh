#/bin/bash
#
show=;
config=0;
clean=0;
quiet=0
args="";
#
#
for i in $*;
do
  if test $i = "--clean"; then clean=1;
  elif test $i = "--config"; then config=1;
  elif test $i = "--quiet"; then quiet=1;
  else args=`echo $args $i`;
  fi;
done;
echo $args
if test -n "$args"; 
then
    for i in $args;
    do 
	export CMTCONFIG=$i;
        echo "Now building configuration: $CMTCONFIG "
        if test $quiet = 1; 
	then
            export QUIET=1
	fi;
        if test $clean = 1; 
	then
	    $show ${CMTROOT}/${CMTBIN}/cmt.exe broadcast "rm -rf ../${CMTCONFIG} || /bin/true";
	fi;
        if test $config = 1;
        then
            $show ${CMTROOT}/${CMTBIN}/cmt.exe broadcast "${CMTROOT}/${CMTBIN}/cmt.exe config";
	fi;
        if test -e build_${CMTCONFIG}.log; then $show rm -f build_${CMTCONFIG}.log; fi;
	$show ${CMTROOT}/${CMTBIN}/cmt.exe broadcast make >build_${CMTCONFIG}.log 2>&1;
    done;
else
    echo "No platform specification given: slc4_ia32_gcc34_dbg";
    echo "                                 slc4_ia32_gcc34";
    echo "                                 slc4_amd64_gcc34_dbg";
    echo "                                 slc4_amd64_gcc34";
    echo "                                 ....";
fi;
