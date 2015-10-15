#!/bin/bash
echo "+++"
echo "+++ Define macro \$> make_tag <Package-without-hat> <version> "
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
    cd ${ONLINESYSROOT}/../Online/$1;
    svn commit -m "New package tag $2";
    cd cmt;
    echo $2 > version.cmt;
    echo "+++ Tagging package: tag_package Online/$1 $2 ";
    tag_package Online/$1 $2;
    cd ${ONLINESYSROOT}/cmt;
}
echo "+++"
echo "+++ Define macro \$> remove_tag <Package-without-hat> <version> "
remove_tag()
{
    if test "$1" != "";
    then
	echo "+++ Package name = Online/$1";
    else
	echo "+++ The first argument to remove_tag must be the package name";
    fi;
    if test "$2" != "";
    then
	echo "+++ Package version = $2";
    else
	echo "+++ The second argument to remove_tag must be the package version";
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
echo "+++"
echo "+++ Define macro \$> redo_tag <Package-without-hat> <version> "
redo_tag()
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
    cd ${ONLINESYSROOT}/../Online/$1;
    svn commit -m "New package tag $2";
    cd cmt;
    echo $2 > version.cmt;
    echo "+++ Removing tag from package: Online/$1 $2 ";
    svn rm -m "Need to remove tag Online/$1 version $2" svn+ssh://svn.cern.ch/reps/lhcb/Online/tags/Online/${1}/${2};
    echo "+++ Tagging package: tag_package Online/$1 $2 ";
    tag_package Online/$1 $2;
    cd ${ONLINESYSROOT}/cmt;
}
echo "+++"
echo "+++ Define macro \$> remove_sys_tag <version> "
remove_sys_tag()
{
    if test "$1" != "";
    then
	echo "+++ Package OnlineSys version = $1";
    else
	echo "+++ argument to remove_sys_tag must be the package version";
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
    echo "+++ Removing tag from package: OnlineSys $1 ";
    svn rm -m "Need to remove tag OnlineSys version $1" svn+ssh://svn.cern.ch/reps/lhcb/Online/tags/OnlineSys/${1};
}
echo "+++"
echo "+++ Define macro \$> redo_sys_tag <version> "
redo_sys_tag()
{
    if test "$1" != "";
    then
	echo "+++ Package OnlineSys version = $1";
    else
	echo "+++ argument to remove_sys_tag must be the package version";
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
    echo "+++ Removing tag from package: OnlineSys $1 ";
    svn rm -m "Need to remove tag OnlineSys version $1" svn+ssh://svn.cern.ch/reps/lhcb/Online/tags/OnlineSys/${1};
    cd ${ONLINESYSROOT};
    svn commit -m ${1};
    tag_package OnlineSys ${1};
}
echo "+++"
echo "+++ Define macro \$> make_release <version> "
make_release()
{
    if test "$1" != "";
    then
	echo "+++ Project ONLINE version = $1";
    else
	echo "+++ The second argument to make_release must be the project version";
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
    echo "+++ Copy project area .... ";
    cd ${ONLINESYSROOT}/..;
    svn commit -m ONLINE_${1};
    cd ${ONLINESYSROOT}/../cmt;
    svn commit -m ONLINE_${1};
    cd ${ONLINESYSROOT}/cmt;
    svn mkdir -m ONLINE_${1} svn+ssh://svn.cern.ch/reps/lhcb/Online/tags/ONLINE/ONLINE_${1};
    svn cp -m ONLINE_${1} svn+ssh://svn.cern.ch/reps/lhcb/Online/trunk/cmt svn+ssh://svn.cern.ch/reps/lhcb/Online/tags/ONLINE/ONLINE_${1}/;
    svn cp -m ONLINE_${1} svn+ssh://svn.cern.ch/reps/lhcb/Online/trunk/CMakeLists.txt svn+ssh://svn.cern.ch/reps/lhcb/Online/tags/ONLINE/ONLINE_${1}/;
}
echo "+++"
echo "+++ Define macro \$> checkVersions "
checkVersions()
{
    if test "$ONLINESYSROOT" = "";
    then
	eval `cmt run printenv|grep ONLINESYSROOT`;
    fi;
    cd ${ONLINESYSROOT}/cmt;
    `which python` ./checkVersions.py;
    cd -;
}
echo "+++"
echo "+++ Define macro \$> provide_tags "
provide_tags()
{
    python <<EOF
import os
lines=os.popen('svn diff requirements |grep "+use" | cut -b 5-32').readlines()
cmd=''
for i in lines:
  cmd = cmd + 'make_tag  '+i[:-1]+'; '
print cmd
EOF
}
echo "+++"
