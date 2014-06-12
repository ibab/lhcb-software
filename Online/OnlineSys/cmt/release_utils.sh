#!/bin/bash
make_tag()
{
    if test "$1" != "";
    then
	echo "+++ Package name = Online/$1";
    else
	echo "+++ The first argument to make_tag must be the package name";
    fi;
    if test "$2" != "";
    then
	echo "+++ Package version = $2";
    else
	echo "+++ The second argument to make_tag must be the package version";
    fi;
#
#
#
    if test "$ONLINESYSROOT" = "";
    then
	eval `cmt run printenv|grep ONLINESYSROOT`;
    fi;
#
#
#
    while true; do
	read -p "+++ Is this correct? Y/N Cr=[Y]" yn;
	case $yn in
            [Yy]* ) break;;
            [Nn]* ) return 0;;
            * ) break;;
	esac;
    done;
#
#
#
    cd ${ONLINESYSROOT}/../Online/$1/cmt;
    svn commit -m "New package tag $2";
    echo $2 > version.cmt;
    echo "+++ Tagging package: tag_package Online/$1 $2 ";
    tag_package Online/$1 $2;
    cd ${ONLINESYSROOT}/cmt;
}

remove_tag()
{
    if test "$1" != "";
    then
	echo "+++ Package name = Online/$1";
    else
	echo "+++ The first argument to make_tag must be the package name";
    fi;
    if test "$2" != "";
    then
	echo "+++ Package version = $2";
    else
	echo "+++ The second argument to make_tag must be the package version";
    fi;
#
#
#
    if test "$ONLINESYSROOT" = "";
    then
	eval `cmt run printenv|grep ONLINESYSROOT`;
    fi;
#
#
#
    while true; do
	read -p "+++ Is this correct? Y/N Cr=[Y]" yn;
	case $yn in
            [Yy]* ) break;;
            [Nn]* ) return 0;;
            * ) break;;
	esac;
    done;
#
#
#
    echo "+++ Removing tag from package: Online/$1 $2 ";
    svn rm -m "Need to remove tag Online/$1 version $2" svn+ssh://svn.cern.ch/reps/lhcb/Online/tags/Online/${1}/${2};
}
