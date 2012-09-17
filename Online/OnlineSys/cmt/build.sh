#/bin/bash
#
show=;
config=0;
clean=0;
quiet=0;
nobuild=0;
args="";
#
#
for i in $*;
do
  if test $i = "--clean"; then clean=1;
  elif test $i = "--config"; then config=1;
  elif test $i = "--quiet"; then quiet=1;
  elif test $i = "--nobuild"; then nobuild=1;
  elif test $i = "--help"; then args=""; break;
  elif test $i = "-h"; then args=""; break;
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
            export QUIET;
	    unset VERBOSE;
	fi;
        if test $clean = 1; 
	then
	    $show ${CMTROOT}/${CMTBIN}/cmt.exe broadcast "rm -rf ../${CMTCONFIG} || /bin/true";
	    rm -rf ../../InstallArea;
	fi;
        if test $config = 1;
        then
            $show ${CMTROOT}/${CMTBIN}/cmt.exe broadcast "${CMTROOT}/${CMTBIN}/cmt.exe config";
	fi;
        if test $nobuild = 1;
        then 
           echo "Do not build ..... ${i}";
           continue;
        fi;
        if test -e build_${CMTCONFIG}.log; then $show rm -f build_${CMTCONFIG}.log; fi;
        if test -e build_${CMTCONFIG}.log.tmp; then $show rm -f build_${CMTCONFIG}.log.tmp; fi;
	$show ${CMTROOT}/${CMTBIN}/cmt.exe broadcast cmt make -j 8 >build_${CMTCONFIG}.log.tmp 2>&1;
        $show cat build_${CMTCONFIG}.log.tmp \
	    | grep -v "#CMT---> " \
	    | grep -v "${CMTCONFIG}.make ok" \
	    | grep -v "CMTCONFIG=${CMTCONFIG}" \
	    | grep -v "INFO:zipdir:" \
	    | grep -v "Makefile.header" \
	    | grep -v "(constituents.make)" \
	    | grep -v "dependencies.make" \
	    | grep -v "setup.make" \
	    | grep -v "installation done" \
	    | grep -v ": library ok" \
	    | grep -v ": Protos ok" \
	    | grep -v "ar: creating " \
	    | grep -v "do-merge" \
	    | grep -v "input-file" \
	    | grep -v install_more_includes \
	    | grep -v "merged-file" \
	    | grep -v "/merge_files.py" \
	    | grep -v "/bin/genmap" \
	    | grep -v merge_rootmap \
	    | grep -v "Create Link to" \
	    | grep -v "/bin/genreflex" \
	    | grep -v "gccxmlpath" \
	    | grep -v " merge_genconfDb " \
            | grep -v "::::" \
            | grep -v "Copy '../../.." \
	    | grep -v -x "Document[[:space:]]\+[a-zA-Z]*ConfDbMerge" \
	    | grep -v -x "Document[[:space:]]\+[a-zA-Z]*RootMap" \
	    | grep -v -x "Document[[:space:]]\+[a-zA-Z]*MergeMap" \
	    | grep -v "/Boost/" \
	    > build_${CMTCONFIG}.log;
    done;
else
    echo "Usage: build.sh <option> [<option>] <platform specifications>";
    echo "   Options:";
    echo "        --config            execute cmt config";
    echo "        --clean             remove target directory";
    echo "        --quiet             execute in quiet mode";
    echo "        --help              show this help";
    echo " ";
    echo "   Platform specifications: i686-slc5-gcc43-dbg";
    echo "                            i686-slc5-gcc43-opt";
    echo "                            ";
    echo "                            x86_64-slc5-gcc43-opt";
    echo "                            x86_64-slc5-gcc43-dbg";
    echo "                            ";
    echo "                            x86_64-slc5-icc11-opt";
    echo "                            x86_64-slc5-icc11-dbg";
    echo "                            ";
    echo "                            x86_64-slc5-gcc46-opt";
    echo "                            x86_64-slc5-gcc46-dbg";
    echo "                            ";
    echo "                            x86_64-slc6-gcc46-opt";
    echo "                            x86_64-slc6-gcc46-dbg";
    echo "                            ";
    echo "                            ";
    echo "                            ....";
fi;
