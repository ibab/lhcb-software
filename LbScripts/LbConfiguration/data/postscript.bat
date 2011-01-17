IF NOT DEFINED LHCB_POST_DONE (
  IF DEFINED LHCB_POST_SCRIPT (
    IF EXIST %LHCB_POST_SCRIPT%.bat (
      call %LHCB_POST_SCRIPT%.bat
      set LHCB_POST_DONE=yes
    )
  ) ELSE (
    IF DEFINED VO_LHCB_SW_DIR (
      IF EXIST %VO_LHCB_SW_DIR%/lib/etc/postscript.bat (
        call %VO_LHCB_SW_DIR%/lib/etc/postscript.bat
        set LHCB_POST_DONE=yes
      )
    )
  )
)

