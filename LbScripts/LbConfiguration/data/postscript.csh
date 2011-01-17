
if ( ! $?LHCB_POST_DONE ) then
  if ( $?LHCB_POST_SCRIPT ) then
    if ( -r $LHCB_POST_SCRIPT.csh ) then
      source $LHCB_POST_SCRIPT.csh
      setenv LHCB_POST_DONE yes
    endif
  else
    if ( $?VO_LHCB_SW_DIR ) then
      if ( -r $VO_LHCB_SW_DIR/lib/etc/postscript.csh ) then
        source $VO_LHCB_SW_DIR/lib/etc/postscript.csh
        setenv LHCB_POST_DONE yes
      endif
    endif
  endif
endif

