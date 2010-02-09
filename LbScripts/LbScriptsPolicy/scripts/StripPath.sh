if [[ "x$LB_NO_STRIP_PATH" ==  "x" ]] ; then
  StripPath_tmpfile=`runpy LbScriptsPolicy.PathStripper --shell=sh --mktemp -e PATH -e LD_LIBRARY_PATH -e PYTHONPATH -e JOBOPTSEARCHPATH -e HPATH `
  StripPathStatus="$?"
  if [ "$StripPathStatus" = 0 -a -n "$StripPath_tmpfile" ]; then
    . $StripPath_tmpfile
  fi
  rm -f $StripPath_tmpfile
  unset StripPath_tmpfile
  unset LB_NO_STRIP_PATH
fi
