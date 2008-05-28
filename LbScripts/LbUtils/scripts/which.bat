@echo off
set n=
if '%~$PATH:1' == '' (
  echo Name not found
) else (
  echo %~$PATH:1
)