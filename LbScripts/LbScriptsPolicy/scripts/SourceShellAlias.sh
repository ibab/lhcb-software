
if [ "x$SOURCESHELLALIASLIST" !=  "x" ] ; then
  for a in $SOURCESHELLALIASLIST ; do 
    if [ -a $a.sh ] ; then
      alias ${a##*/}=". $a.sh"
    fi
  done
  unset SOURCESHELLALIASLIST
  unset a
fi

