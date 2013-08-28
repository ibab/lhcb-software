islogin=0

if (shopt -q login_shell || [[ -o login ]]) 2> /dev/null ; then 
  islogin=1
fi


if [ "$CMTCONFIG" = "x86_64-slc5-icc11-opt" ] || [ "$CMTCONFIG" = "x86_64-slc5-icc11-dbg" ]; then
  if [[ ! -n "$ICC_SETUP_DONE" ]]; then
    echo "Setting up the 64 bits icc compiler"
    . ${GROUP_DIR}/post/icc_setup.sh
    export ICC_SETUP_DONE=1
    unset ICC13_SETUP_DONE
  fi
fi 

if [ "$CMTCONFIG" = "x86_64-slc6-icc13-opt" ] || [ "$CMTCONFIG" = "x86_64-slc6-icc13-dbg" ]; then
  if [[ ! -n "$ICC13_SETUP_DONE" ]]; then
    echo "Setting up the 64 bits icc13 compiler"
    . ${GROUP_DIR}/post/icc13_setup.sh
    export ICC13_SETUP_DONE=1
    unset ICC_SETUP_DONE
  fi
fi

if [ "$CMTCONFIG" = "i686-slc5-icc11-opt" ] || [ "$CMTCONFIG" = "i686-slc5-icc11-dbg" ]; then
  if [[ ! -n "$ICC_SETUP_DONE" ]]; then
    echo "Setting up the 32 bits icc compiler"
    . ${GROUP_DIR}/post/icc_setup32.sh
    export ICC_SETUP_DONE=1
    unset ICC13_SETUP_DONE
  fi
fi 

unset islogin
