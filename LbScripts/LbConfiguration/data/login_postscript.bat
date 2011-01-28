IF DEFINED LOGIN_POST_SCRIPT (
  IF EXIST %LOGIN_POST_SCRIPT%.bat (
    call %LOGIN_POST_SCRIPT%.bat
  )
) ELSE (
  IF DEFINED VO_LHCB_SW_DIR (
    IF EXIST %VO_LHCB_SW_DIR%/lib/etc/login_postscript.bat (
      call %VO_LHCB_SW_DIR%/lib/etc/login_postscript.bat
    )
  )
)


