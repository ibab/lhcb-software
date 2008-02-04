
# get the location of this very script

# set echo
unalias cwdcmd

set a = `history -rh 1 | awk '{print $2}'`
echo $a
#set b = `basename $a`
set l = `readlink $a`
if ( $l == "" ) then
  set l = $a
  set scriptsdir = `dirname $a`
else
  set d = `dirname $a`
  if ( $d != $cwd && $d != "." ) then
    pushd $d # >& /dev/null
  endif
  pushd `dirname $l` # >& /dev/null
  set scriptsdir = `pwd`
  popd # >& /dev/null
  if ( $d != $cwd && $d != "." ) then
    popd # >& /dev/null
  endif
endif
unset a
unset l
unset d
echo $scriptsdir

