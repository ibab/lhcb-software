@echo off

if exist ..\xml (goto :copydtd) else (goto :end)

:copydtd
  echo Copying gdd.dtd to local xml-directory
  copy %GAUDIOBJDESCROOT%\xml_files\gdd.dtd ..\xml\gdd.dtd 

:end