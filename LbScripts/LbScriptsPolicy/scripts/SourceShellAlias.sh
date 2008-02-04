
if [ "x$SOURCESHELLALIASLIST" !=  "x" ] ; then
  for a in $SOURCESHELLALIASLIST ; do 
    alias $a=". $SOURCESHELLDIR/$a.sh"
  done
  unset SOURCESHELLALIASLIST
  unset a
fi

