if ( "$CMTCONFIG" == "x86_64-slc5-icc11-opt" || "$CMTCONFIG" == "x86_64-slc5-icc11-dbg" ) then
  if ( ! $?ICC_SETUP_DONE ) then
    echo "Setting up the 64 bits icc compiler"
    unalias cd 
    unalias cwdcmd
    source ${GROUP_DIR}/post/icc_setup.csh
    setenv ICC_SETUP_DONE
    unsetenv ICC13_SETUP_DONE
  endif
endif 

if ( "$CMTCONFIG" == "x86_64-slc6-icc13-opt" || "$CMTCONFIG" == "x86_64-slc6-icc13-dbg" ) then
  if ( ! $?ICC_SETUP_DONE ) then
    echo "Setting up the 64 bits icc13 compiler"
    unalias cd
    unalias cwdcmd
    source ${GROUP_DIR}/post/icc13_setup.csh
    setenv ICC13_SETUP_DONE
    unsetenv ICC_SETUP_DONE
  endif
endif


if ( "$CMTCONFIG" == "i686-slc5-icc11-opt" || "$CMTCONFIG" == "i686-slc5-icc11-dbg" ) then
  if ( $?ICC_SETUP_DONE ) then
    echo "Setting up the 32 bits icc compiler"
    unalias cd 
    unalias cwdcmd
    source ${GROUP_DIR}/post/icc_setup32.csh
    setenv ICC_SETUP_DONE
    unsetenv ICC13_SETUP_DONE
  endif
endif 
