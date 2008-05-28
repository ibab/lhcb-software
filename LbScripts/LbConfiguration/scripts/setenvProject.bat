@echo off
set project=%1
set version=%2

rem call %~d0%~p0\setcmt.bat
set CMTBIN=VisualC
if not defined User_release_area set User_release_area=%HOME%\cmtuser

if '%CMTSITE%' equ 'LOCAL' goto local

rem ---------------------------- use AFS to find project ---------------------------
if not defined OSC_release_area set OSC_release_area=%SITEROOT%\sw\contrib
if not defined LCG_release_area set LCG_release_area=%SITEROOT%\sw\lcg\app\releases
if not defined Gaudi_release_area set Gaudi_release_area=%SITEROOT%\sw\Gaudi\releases
if not defined LHCb_release_area set LHCb_release_area=%SITEROOT%\lhcb\software\releases
if not defined Lbcom_release_area set Lbcom_release_area=%SITEROOT%\lhcb\software\releases
if not defined Rec_release_area set Rec_release_area=%SITEROOT%\lhcb\software\releases
if not defined Hlt_release_area set Hlt_release_area=%SITEROOT%\lhcb\software\releases
if not defined Phys_release_area set Phys_release_area=%SITEROOT%\lhcb\software\releases
if not defined Geant4_release_area set Geant4_release_area=%LHCb_release_area%
if not defined Brunel_release_area set Brunel_release_area=%LHCb_release_area%
if not defined Moore_release_area set Moore_release_area=%LHCb_release_area%
if not defined DaVinci_release_area set DaVinci_release_area=%LHCb_release_area%
if not defined Online_release_area set Online_release_area=%LHCb_release_area%
if not defined LbScripts_release_area set LbScripts_release_area=%LHCb_release_area%
if not defined Analysis_release_area set Analysis_release_area=%LHCb_release_area%
if not defined %project%_release_area set %project%_release_area=%SITEROOT%\lhcb\software\releases
set cmtproject=%User_release_area%;%LHCb_release_area%;%Gaudi_release_area%;%LCG_release_area%
goto project

rem --------------------------- CMTSITE = 'LOCAL' ------------------------------------
:local
if not defined OSC_release_area set OSC_release_area=%SITEROOT%\contrib
if not defined LCG_release_area set LCG_release_area=%SITEROOT%\lcg\external
if not defined Gaudi_release_area set Gaudi_release_area=%SITEROOT%\lhcb
if not defined LHCb_release_area set LHCb_release_area=%SITEROOT%\lhcb
if not defined Lbcom_release_area set Lbcom_release_area=%SITEROOT%\lhcb
if not defined Rec_release_area set Rec_release_area=%SITEROOT%\lhcb
if not defined Hlt_release_area set Hlt_release_area=%SITEROOT%\lhcb
if not defined Phys_release_area set Phys_release_area=%SITEROOT%\lhcb
if not defined Geant4_release_area set Geant4_release_area=%LHCb_release_area%
if not defined Brunel_release_area set Brunel_release_area=%LHCb_release_area%
if not defined Moore_release_area set Moore_release_area=%LHCb_release_area%
if not defined DaVinci_release_area set DaVinci_release_area=%LHCb_release_area%
if not defined Online_release_area set Online_release_area=%LHCb_release_area%
if not defined LbScripts_release_area set LbScripts_release_area=%LHCb_release_area%
if not defined Analysis_release_area set Analysis_release_area=%LHCb_release_area%
if not defined %project%_release_area set %project%_release_area=%SITEROOT%\lhcb
set cmtproject=%User_release_area%;%SITEROOT%\lhcb;%SITEROOT%\lcg\external
rem --------------------------- continue ---------------------------------------------
:project
if '%project%' equ 'LHCb' (
   set release_area=%LHCb_release_area%
   set UpProject='LHCB')
if '%project%' equ 'Online' (
   set release_area=%Online_release_area%
   set UpProject='ONLINE')
if '%project%' equ 'Bender' (
   set release_area=%Bender_release_area%
   set UpProject='BENDER')
if '%project%' equ 'Lbcom' (
   set release_area=%Lbcom_release_area%
   set UpProject='LBCOM' )
if '%project%' equ 'Rec' (
   set release_area=%Rec_release_area%
   set UpProject='REC')
if '%project%' equ 'Hlt' (
   set release_area=%Hlt_release_area%
   set UpProject='HLT')
if '%project%' equ 'Phys' (
   set release_area=%Phys_release_area%
   set UpProject='PHYS' )
if '%project%' equ 'Boole' (
   set release_area=%Boole_release_area%
   set UpProject='BOOLE')
if '%project%' equ 'Brunel' ( 
   set release_area=%Brunel_release_area%
   set UpProject='BRUNEL')
if '%project%' equ 'Moore' ( 
   set release_area=%Moore_release_area%
   set UpProject='MOORE')
if '%project%' equ 'Gauss' (
   set release_area=%Gauss_release_area%
   set UpProject='GAUSS')
if '%project%' equ 'DaVinci' (
   set release_area=%Davinci_release_area%
   set UpProject='DaVinci')
if '%project%' equ 'Panoramix' ( 
   set release_area=%Panoramix_release_area%
   set UpProject='PANORAMIX')
if '%project%' equ 'Gaudi' (
   set release_area=%Gaudi_release_area%
   set UpProject='GAUDI')
if '%project%' equ 'Geant4' (
   set release_area=%Geant4_release_area%
   set UpProject= 'GEANT4')
if '%project%' equ 'Analysis' (
   set release_area=%Analysis_release_area%
   set UpProject= 'ANALYSIS')
rem echo UpProject= %UpProject%
rem echo release_area= %release_area%

if exist %release_area%\%project%Env\%version%\cmt\setup.bat (
  set CMTPROJECTPATH=
  echo Setting Environment for %project% version %version%
  call %release_area%\%project%Env\%version%\cmt\setup.bat
  set CMTPATH
) else (
  if not defined CMTPROJECTPATH set CMTPROJECTPATH=%cmtproject%
  set CMTPATH=
  if not exist %User_release_area%\%project%_%version%\cmt (
     cd %User_release_area%
     cmt create_project %project%_%version% -use="%UpProject% %UpProject%_%version%"
  )
  cd /d %User_release_area%\%project%_%version%
)














