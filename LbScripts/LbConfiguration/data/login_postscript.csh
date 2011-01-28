
if ( $?LOGIN_POST_SCRIPT ) then
  if ( -r $LOGIN_POST_SCRIPT.csh ) then
    source $LOGIN_POST_SCRIPT.csh
  endif
else
  if ( $?VO_LHCB_SW_DIR ) then
    if ( -r $VO_LHCB_SW_DIR/lib/etc/login_postscript.csh ) then
      source $VO_LHCB_SW_DIR/lib/etc/login_postscript.csh
    endif
  endif
endif


