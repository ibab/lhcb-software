
if [ "x$SOURCESHELLALIASLIST" !=  "x" ] ; then
  for a in $SOURCESHELLALIASLIST ; do 
    if [ -a $SOURCESHELLDIR/$a.sh ] ; then
      alias $a=". $SOURCESHELLDIR/$a.sh"
    fi
  done
  unset SOURCESHELLALIASLIST
  unset a
fi

