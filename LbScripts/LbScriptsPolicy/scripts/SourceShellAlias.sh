
if [ "x$SOURCESHELLALIASLIST" !=  "x" ] ; then
  for a in $SOURCESHELLALIASLIST ; do 
    if [ -a $a.sh ] ; then
      b=${a##*/}
      alias $b=". $b.sh"
    fi
  done
  unset SOURCESHELLALIASLIST
  unset a
fi

