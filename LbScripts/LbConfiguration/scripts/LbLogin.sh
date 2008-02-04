
# set -x

# get the location of this very script


a=`history 1 | awk '{print $3}'`
#b=`basename $a`
l=`readlink $a`
if [ "$l" = "" ]; then
  l=$a
  scriptsdir=`dirname $a`
else
  pushd `dirname $a` &> /dev/null
  pushd `dirname $l` &> /dev/null
  scriptsdir=`pwd`
  popd &> /dev/null
  popd &> /dev/null
fi
echo $scriptsdir
