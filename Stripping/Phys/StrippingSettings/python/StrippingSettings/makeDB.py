import os
import sys

'''
Generate a dbase module from a set of stripping line config
dictionaries.

Usage: 
  > python makeDB python/StrippingSettings/StrippingDev 
'''

def usage( sys.argv ):
   print "Usage:\n \t python " + sys.argv[0] + " <relative>/<path>" 

def generate_dbase( rel_path ):
   full_path = os.path.abspath( rel_path )
   if os.path.exists( full_path ):
      sys.path.append( full_path )

      print "Appending path to python path"
      
      from StrippingSettings.Utils import addModuleToDbase
      
      import LineConfigDictionaries_CharmXsec
      import LineConfigDictionaries_GTWG    
      import LineConfigDictionaries_Semileptonic
      import LineConfigDictionaries_BetaS 
      import LineConfigDictionaries_EWWG  
      import LineConfigDictionaries_MiniBias  
      import LineConfigDictionaries_Calib 
      import LineConfigDictionaries_FWG       
      import LineConfigDictionaries_Charm 
      import LineConfigDictionaries_GLWG
      import LineConfigDictionaries_RDWG          
      
      configs = (
	 LineConfigDictionaries_CharmXsec
         ,LineConfigDictionaries_GTWG    
         ,LineConfigDictionaries_Semileptonic
         ,LineConfigDictionaries_BetaS 
         ,LineConfigDictionaries_EWWG      
         ,LineConfigDictionaries_MiniBias    
         ,LineConfigDictionaries_Calib 
         ,LineConfigDictionaries_FWG      
         ,LineConfigDictionaries_Charm 
         ,LineConfigDictionaries_GLWG
         ,LineConfigDictionaries_RDWG 
         )
      
      for config in configs :
         addModuleToDbase('stripping.tmp', config, verbose=True)


if 2 == len(sys.argv):
   generate_dbase( sys.argv[1] )
else:
   usage( sys.argv )
