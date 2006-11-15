#/bin/sh
#
${CMTROOT}/${CMTBIN}/cmt.exe broadcast "rm -rf ../${CMTCONFIG} || /bin/true"
${CMTROOT}/${CMTBIN}/cmt.exe broadcast "${CMTROOT}/${CMTBIN}/cmt.exe config"
${CMTROOT}/${CMTBIN}/cmt.exe broadcast make
