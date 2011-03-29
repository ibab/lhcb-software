from StrippingSettings.Utils import *
import LineConfigDictionaries_CharmXsec
import LineConfigDictionaries_GTWG    
import LineConfigDictionaries_Semileptonic
import LineConfigDictionaries_BetaS 
import LineConfigDictionaries_EWWG  
import LineConfigDictionaries_MiniBias  
import LineConfigDictionaries_Calib 
import LineConfigDictionaries_FWG       
import LineConfigDictionaries_Charm 
import LineConfigDictionaries_GTWG
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
	,LineConfigDictionaries_GTWG
	,LineConfigDictionaries_GLWG
	,LineConfigDictionaries_RDWG 
	)

for config in configs :
   addModuleToDbase('Stripping13', config, verbose=True)


