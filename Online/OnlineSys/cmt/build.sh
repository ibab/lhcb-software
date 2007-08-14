#/bin/bash
#
if test -n "$1"; 
then
    for i in $*;
    do 
	export CMTCONFIG=$i;
        echo "Now building configuration: $CMTCONFIG "
	#${CMTROOT}/${CMTBIN}/cmt.exe broadcast "rm -rf ../${CMTCONFIG} || /bin/true";
	#${CMTROOT}/${CMTBIN}/cmt.exe broadcast "${CMTROOT}/${CMTBIN}/cmt.exe config";
	if test -e build_${CMTCONFIG}.log; then rm -f build_${CMTCONFIG}.log; fi;
	${CMTROOT}/${CMTBIN}/cmt.exe broadcast make >build_${CMTCONFIG}.log 2>&1;
    done;
else
    echo "No platform specification given: slc4_ia32_gcc34_dbg";
    echo "                                 slc4_ia32_gcc34";
    echo "                                 slc4_amd64_gcc34_dbg";
    echo "                                 slc4_amd64_gcc34";
    echo "                                 ....";
fi;
