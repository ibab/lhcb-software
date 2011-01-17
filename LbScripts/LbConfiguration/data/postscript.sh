if [[ ! -n "$LHCB_POST_DONE" ]]; then
  if [[ -n "$LHCB_POST_SCRIPT" ]]; then
    if [[ -r $LHCB_POST_SCRIPT.sh ]]; then
      . $LHCB_POST_SCRIPT.sh
      export LHCB_POST_DONE=yes
    fi
  else
    if [[ -n "$VO_LHCB_SW_DIR" ]]; then
      if [[ -r $VO_LHCB_SW_DIR/lib/etc/postscript.sh ]]; then
        . $VO_LHCB_SW_DIR/lib/etc/postscript.sh
        export LHCB_POST_DONE=yes
      fi
    fi
  fi
fi


