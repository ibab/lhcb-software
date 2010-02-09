if (! ${?LB_NO_STRIP_PATH} ) then
  set StripPath_tmpfile = `runpy LbScriptsPolicy.PathStripper --shell=csh --mktemp -e PATH -e LD_LIBRARY_PATH -e PYTHONPATH -e HPATH `
  set StripPathStatus = $?
  if ( ! $StripPathStatus && "$StripPath_tmpfile" != "" ) then
    source $StripPath_tmpfile
  endif  
  rm -f $StripPath_tmpfile
  unset StripPath_tmpfile
  unsetenv LB_NO_STRIP_PATH
endif
