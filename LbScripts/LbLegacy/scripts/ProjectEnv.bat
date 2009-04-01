@echo off
set project=%1
set version=%2


if '%project%' equ 'LHCb' set release_area=%LHCb_release_area%
if '%project%' equ 'Online' set release_area=%Online_release_area%
if '%project%' equ 'LbScripts' set release_area=%LbScripts_release_area%
if '%project%' equ 'Curie' set release_area=%Curie_release_area%
if '%project%' equ 'Bender' set release_area=%Bender_release_area%
if '%project%' equ 'Lbcom' set release_area=%Lbcom_release_area%
if '%project%' equ 'Rec' set release_area=%Rec_release_area%
if '%project%' equ 'Hlt' set release_area=%Hlt_release_area%
if '%project%' equ 'Phys' set release_area=%Phys_release_area%
if '%project%' equ 'Boole' set release_area=%Boole_release_area%
if '%project%' equ 'Brunel' set release_area=%Brunel_release_area%
if '%project%' equ 'Gauss' set release_area=%Gauss_release_area%
if '%project%' equ 'DaVinci' set release_area=%Davinci_release_area%
if '%project%' equ 'Panoramix' set release_area=%Panoramix_release_area%
if '%project%' equ 'Gaudi' set release_area=%Gaudi_release_area%
if '%project%' equ 'Geant4' set release_area=%Geant4_release_area%


if exist %release_area%\%project%Env\%version%\cmt\setup.bat (
  echo Setting Environment for %project% version %version%
  call %release_area%\%project%Env\%version%\cmt\setup.bat
  set CMTPATH
) else (
  echo Selected %project% version %version% not available in release area %release_area% 
)














