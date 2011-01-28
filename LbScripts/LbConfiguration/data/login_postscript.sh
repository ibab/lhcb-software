
if [[ -n "$LOGIN_POST_SCRIPT" ]]; then
  if [[ -r $LOGIN_POST_SCRIPT.sh ]]; then
    . $LOGIN_POST_SCRIPT.sh
  fi
else
  if [[ -n "$VO_LHCB_SW_DIR" ]]; then
    if [[ -r $VO_LHCB_SW_DIR/lib/etc/login_postscript.sh ]]; then
      . $VO_LHCB_SW_DIR/lib/etc/login_postscript.sh
    fi
  fi
fi



