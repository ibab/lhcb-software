# $Id: Common.py,v 1.1 2001-11-29 07:49:16 mato Exp $
#**************************************************************
#
# Standard job options python file
#
#==============================================================

#--- Useful Functions -----------------------------------------
def include(f) :
  from string import find
  from string import replace
  from os import getenv
  if '$' in f :
    env = f[find(f,'$')+1:]
    if '/' in env : env = env[:find(env,'/')]
    fname = replace(f,'$'+env, getenv(env))
  else :
    fname = f
  execfile(fname)

#--- Global Variables -----------------------------------------
ApplicationMgr = g

#--- Useful Parameters-----------------------------------------
true , false = 1, 0
DEBUG, INFO, WARNING, ERROR, FATAL = 2, 3, 4, 5, 6

#--- Include Files 
include('$STDOPTS/Dlls.py')
include('$STDOPTS/Services.py')

