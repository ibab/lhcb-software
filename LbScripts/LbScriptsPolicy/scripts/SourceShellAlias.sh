
if [ "x$SOURCESHELLALIASLIST" !=  "x" ] ; then
  for a in $SOURCESHELLALIASLIST ; do 
    alias $a=". $LbScripts_HOME/InstallArea/scripts/$a.sh"
  done
  unset SOURCESHELLALIASLIST
  unset a
fi

