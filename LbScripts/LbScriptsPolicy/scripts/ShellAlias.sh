
if [ "x$SHELLALIASLIST" !=  "x" ] ; then
  for a in $SHELLALIASLIST ; do
    alias $a=$a.csh
  done
  unset SHELLALIASLIST
  unset a
fi
