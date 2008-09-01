regexFloats = "([\s+-]\d*?[.]\d*?(e[+-])\d*|[\s+-]\d*?[.]\d*|[\s+-]\d*)"

def open( aFile ) :
   import os, sys, __builtin__
   try:
      f = __builtin__.open( aFile , "r" ) # open file in read mode
      print "Opened file %s \n"%( f.name )
      return f
   except IOError:
      print "ERROR: Couldn't open file %s! \n"%( aFile )
      sys.exit()

def close( aFile ) :
   import os, sys
   try:
      assert( not aFile.closed )
      aFile.close()
   except AssertionError:
      print "ERROR: %s is already closed! \n"%( aFile.name )
      sys.exit()
      
def reDoF( dof ) : 
   import re
   # This is what we're trying to "read"
   # Tx: cur=            0 delta= -0.000732818 +/-    0.0181657 gcc= 0.808774
   # So first try
   # +-x.ye+-z
   # then
   # space+-x.y
   # then
   # space+-x
   # that should cover all possibilities
   floats = "([\s+-]\d*?[.]\d*?(e[+-])\d*|[\s+-]\d*?[.]\d*|[\s+-]\d*)"
   reDoF = re.compile( r"%s: cur=\s*"
                       "(?P<current>%s)" 
                       "\s*delta=\s*"
                       "(?P<delta>%s)"
                       "\s*?([+]/-)\s*"
                       "(?P<error>%s)"%(dof, floats, floats, floats ) )
   return reDoF

def AlDelta( line, reDoF ) :
   import re
   match = reDoF.search( line )
   if not match :
      return None
   else :
      matchGroup = match.group
      return { 'current' : matchGroup( 'current' ), 'delta' : matchGroup( 'delta' ), 'error' : matchGroup( 'error' ) }