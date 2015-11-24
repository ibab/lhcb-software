 # $Id: StrippingBu2hhh.py,v 4.0 2015-05-17 Rio team $
'''
Module for construction of Bu->hhh from:   
   Inclusive KKK line for light decay modes (pipipi, Kpipi, KKpi and KKK) 
   Inclusive KpKpKp line for same-sign kaons of light decay modes (pipipi, Kpipi, KKpi and KKK) 
   Inclusive pph line for heavy modes (pppi, ppK)

Exported symbols (use python help!):
   - Bu2hhhBuilder
   - makeKKK_incl  
   - makeKpKpKp_incl  
   - makepph_incl
'''

__author__ = ['Irina Nasteva', 'Jussara Miranda', 'Alvaro Gomes']
__date__ = '17/05/2015'
__version__ = '$Revision: 5.1 $'

__all__ = ('Bu2hhhBuilder',
           'makeKKK_incl', 
           'makeKpKpKp_incl', 
           'makepph_incl', 
	   )


config_params = {
    'MaxTrSIZE'             : 200 ,      ## GEC maximim recSummaryTrack(LHCb.RecSummary.nLongTracks, TrLONG)
    '_h_PT'                 : 100. ,     ## tracks min PT
    '_h_P'                  : 1500. ,    ## tracks min P  
    '_h_IPCHI2'             : 1. ,       ## min tracks IP wrt OWNPV
    '_h_TRCHI2DOF'          : 4.0 ,      ## max tracks CHI2DOF
    '_h_TRGHP'             : .5,       ## Track GhostProbability     
    '_3h_DOCA'              : .2 ,       ## max DOCA between h and 2h 
    '_3h_PTmax'             : 1500 ,     ## min PT of the 3h highest PT track
    '_3h_DIRA'              : .99998 ,   ## min cos angle between 3h momentum and PV decay direction   
    '_3h_FDCHI2'            : 500. ,     ## min 3h FDCHI2 wrt best 3h PV  
    '_3h_PVDOCAmin'         : 3.0 ,      ## min value of the 3h doca wrt any PV
    '_3h_CHI2'              : 12.0 ,     ## max 3h vertex CHI2 
    '_3h_IPCHI2'            : 10. ,      ## max 3h IP CHI2 wrt best 3h PV
    '_3h_PT'                : 1000. ,    ## min 3h PT   
    '_3h_PTsum'             : 4500. ,    ## min of 3h tracks PT sum 
    '_3h_Psum'              : 20000. ,   ## min of 3h tracks P sum 
    '_3h_PVIPCHI2sum'       : 500. ,     ## min of the 3h tracks IP wrt best 3h PV
    '_3h_TRKCHIDOFmin'      : 3.0,       ## max track CHI2DOF for the track with smalest CHI2DOF
    '_3h_CORRMmax'          : 7000. ,    ## max corrected mass for 3h candidate  
    '_3h_CORRMmin'          : 4000. ,    ## min corrected mass for 3h candidate   
    '_3hKKK_Mmax'           : 6300. ,    ## max 3h mass for inclusive KKK line       
    '_3hKKK_Mmin'           : 5050. ,    ## min 3h mass for inclusive KKK line
    '_3hpph_deltaMmax'      : 400,       ## max 3h mass difference for inclusive ppK line
    '_3hpph_deltaMmin'      : 200,       ## min 3h mass difference for inclusive ppK line 
    'KKK_inclLinePrescale'  : 1.0,
    'KKK_inclLinePostscale' : 1.0,
    'KpKpKp_inclLinePrescale'  : 1.0,
    'KpKpKp_inclLinePostscale' : 1.0,
    'pph_inclLinePrescale'  : 1.0,
    'pph_inclLinePostscale' : 1.0
    }


"""
B+ -> h+h-h+ channels
"""

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles

import StandardParticles

if hasattr(StandardParticles, "StdAllNoPIDsKaons"):
  from StandardParticles import StdAllNoPIDsKaons as StdNoPIDsKaons
else:
  from StandardParticles import StdNoPIDsKaons as StdNoPIDsKaons

if hasattr(StandardParticles, "StdAllLooseProtons"):
  from StandardParticles import StdAllLooseProtons as StdLooseProtons
else:
  from StandardParticles import StdLooseProtons as StdLooseProtons



from PhysSelPython.Wrappers import Selection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from Configurables import LoKi__VoidFilter as VoidFilter


default_config = {
  'Bu2hhh' : {
    'WGs'         : ['BnoC'],
    'BUILDERTYPE' : 'Bu2hhhBuilder',
    'CONFIG'      : { 'MaxTrSIZE'             : 200 ,      ## GEC maximim recSummaryTrack(LHCb.RecSummary.nLongTracks, TrLONG)
                      '_h_PT'                 : 100. ,     ## tracks min PT
                      '_h_P'                  : 1500. ,    ## tracks min P  
                      '_h_IPCHI2'             : 1. ,       ## min tracks IP wrt OWNPV
                      '_h_TRCHI2DOF'          : 4.0 ,      ## max tracks CHI2DOF
                      '_h_TRGHP'             : .5,       ## Track GhostProbability     
                      '_3h_DOCA'              : .2 ,       ## max DOCA between h and 2h 
                      '_3h_PTmax'             : 1500 ,     ## min PT of the 3h highest PT track
                      '_3h_DIRA'              : .99998 ,   ## min cos angle between 3h momentum and PV decay direction   
                      '_3h_FDCHI2'            : 500. ,     ## min 3h FDCHI2 wrt best 3h PV  
                      '_3h_PVDOCAmin'         : 3.0 ,      ## min value of the 3h doca wrt any PV
                      '_3h_CHI2'              : 12.0 ,     ## max 3h vertex CHI2 
                      '_3h_IPCHI2'            : 10. ,      ## max 3h IP CHI2 wrt best 3h PV
                      '_3h_PT'                : 1000. ,    ## min 3h PT   
                      '_3h_PTsum'             : 4500. ,    ## min of 3h tracks PT sum 
                      '_3h_Psum'              : 20000. ,   ## min of 3h tracks P sum 
                      '_3h_PVIPCHI2sum'       : 500. ,     ## min of the 3h tracks IP wrt best 3h PV
                      '_3h_TRKCHIDOFmin'      : 3.0,       ## max track CHI2DOF for the track with smalest CHI2DOF
                      '_3h_CORRMmax'          : 7000. ,    ## max corrected mass for 3h candidate  
                      '_3h_CORRMmin'          : 4000. ,    ## min corrected mass for 3h candidate   
                      '_3hKKK_Mmax'           : 6300. ,    ## max 3h mass for inclusive KKK line       
                      '_3hKKK_Mmin'           : 5050. ,    ## min 3h mass for inclusive KKK line
                      '_3hpph_deltaMmax'      : 400,       ## max 3h mass difference for inclusive ppK line
                      '_3hpph_deltaMmin'      : 200,       ## min 3h mass difference for inclusive ppK line 
                      'KKK_inclLinePrescale'  : 1.0,
                      'KKK_inclLinePostscale' : 1.0,
                      'KpKpKp_inclLinePrescale'  : 1.0,
                      'KpKpKp_inclLinePostscale' : 1.0,
                      'pph_inclLinePrescale'  : 1.0,
                      'pph_inclLinePostscale' : 1.0
                    },
    'STREAMS'      : ['Bhadron']
  }
}

filter = {'Code' :
          "(recSummaryTrack(LHCb.RecSummary.nLongTracks, TrLONG)"\
          " < %s )" \
          % config_params['MaxTrSIZE'],
          'Preambulo' : [ "from LoKiTracks.decorators import *",
                                      'from LoKiCore.functions    import *' ]
}

default_name = "Bu2hhh"

class Bu2hhhBuilder(LineBuilder) :
    __configuration_keys__ = ('MaxTrSIZE',
                              '_h_PT',             
                              '_h_P',               
                              '_h_IPCHI2',         
                              '_h_TRCHI2DOF',
                              '_h_TRGHP',
                              '_3h_DOCA',            
                              '_3h_PTmax',          
                              '_3h_DIRA',            
                              '_3h_FDCHI2',         
                              '_3h_PVDOCAmin',      
                              '_3h_CHI2',           
                              '_3h_IPCHI2',             
                              '_3h_PT',              
                              '_3h_PTsum',              
                              '_3h_Psum',              
                              '_3h_PVIPCHI2sum',              
			      '_3h_TRKCHIDOFmin',
                              '_3h_CORRMmax',       
                              '_3h_CORRMmin',       
                              '_3hKKK_Mmax',                 
                              '_3hKKK_Mmin',             
                              '_3hpph_deltaMmax',
                              '_3hpph_deltaMmin',
                              'KKK_inclLinePrescale',
                              'KKK_inclLinePostscale',
                              'KpKpKp_inclLinePrescale',
                              'KpKpKp_inclLinePostscale',
                              'pph_inclLinePrescale',
                              'pph_inclLinePostscale'
)

    def __init__(self, name, config) :

        LineBuilder.__init__(self, name, config)

        _KKK_inclName = name + '_KKK_incl'
        _KpKpKp_inclName = name + '_KKK_samesign'
        _pph_inclName = name + '_pph_incl'
	
        
        self.selKKK = makeKKK_incl( 'KKKinclFor' + _KKK_inclName + 'Sel', 
                                    _h_PT           = config['_h_PT'],
                                    _h_P            = config['_h_P'],
                                    _h_IPCHI2       = config['_h_IPCHI2'],
                                    _h_TRCHI2DOF    = config['_h_TRCHI2DOF'],
                                    _h_TRGHP       = config['_h_TRGHP'],
                                    _3h_DOCA        = config['_3h_DOCA'],
                                    _3h_PTmax       = config['_3h_PTmax'],
                                    _3h_DIRA        = config['_3h_DIRA'],
                                    _3h_FDCHI2      = config['_3h_FDCHI2'],
                                    _3h_PVDOCAmin   = config['_3h_PVDOCAmin'],
                                    _3h_CHI2        = config['_3h_CHI2'],
                                    _3h_IPCHI2      = config['_3h_IPCHI2'],
                                    _3h_PT          = config['_3h_PT'],
                                    _3h_PTsum       = config['_3h_PTsum'],
                                    _3h_Psum        = config['_3h_Psum'],
                                    _3h_PVIPCHI2sum = config['_3h_PVIPCHI2sum'],
                                    _3h_TRKCHIDOFmin= config['_3h_TRKCHIDOFmin'],
                                    _3h_CORRMmax    = config['_3h_CORRMmax'],
                                    _3h_CORRMmin    = config['_3h_CORRMmin'],
                                    _3hKKK_Mmin     = config['_3hKKK_Mmin'],
                                    _3hKKK_Mmax     = config['_3hKKK_Mmax'])
        
        self.selKpKpKp = makeKpKpKp_incl( 'KpKpKpinclFor' + _KpKpKp_inclName + 'Sel', 
                                          _h_PT           = config['_h_PT'],
                                          _h_P            = config['_h_P'],
                                          _h_IPCHI2       = config['_h_IPCHI2'],
                                          _h_TRCHI2DOF    = config['_h_TRCHI2DOF'],
                                          _h_TRGHP       = config['_h_TRGHP'],
                                          _3h_DOCA        = config['_3h_DOCA'],
                                          _3h_PTmax       = config['_3h_PTmax'],
                                          _3h_DIRA        = config['_3h_DIRA'],
                                          _3h_FDCHI2      = config['_3h_FDCHI2'],
                                          _3h_PVDOCAmin   = config['_3h_PVDOCAmin'],
                                          _3h_CHI2        = config['_3h_CHI2'],
                                          _3h_IPCHI2      = config['_3h_IPCHI2'],
                                          _3h_PT          = config['_3h_PT'],
                                          _3h_PTsum       = config['_3h_PTsum'],
                                          _3h_Psum        = config['_3h_Psum'],
                                          _3h_PVIPCHI2sum = config['_3h_PVIPCHI2sum'],
                                          _3h_TRKCHIDOFmin= config['_3h_TRKCHIDOFmin'],
                                          _3h_CORRMmax    = config['_3h_CORRMmax'],
                                          _3h_CORRMmin    = config['_3h_CORRMmin'],
                                          _3hKKK_Mmin     = config['_3hKKK_Mmin'],
                                          _3hKKK_Mmax     = config['_3hKKK_Mmax'])
        
        self.selpph = makepph_incl( 'pphinclFor' + _pph_inclName + 'Sel', 
                                    _h_PT           = config['_h_PT'],
                                    _h_P            = config['_h_P'],
                                    _h_IPCHI2       = config['_h_IPCHI2'],
                                    _h_TRCHI2DOF    = config['_h_TRCHI2DOF'],
                                    _h_TRGHP       = config['_h_TRGHP'],       
                                    _3h_DOCA        = config['_3h_DOCA'],
                                    _3h_PTmax       = config['_3h_PTmax'],
                                    _3h_DIRA        = config['_3h_DIRA'],
                                    _3h_FDCHI2      = config['_3h_FDCHI2'],
                                    _3h_PVDOCAmin   = config['_3h_PVDOCAmin'],
                                    _3h_CHI2        = config['_3h_CHI2'],
                                    _3h_IPCHI2      = config['_3h_IPCHI2'],
                                    _3h_PT          = config['_3h_PT'],
                                    _3h_PTsum       = config['_3h_PTsum'],
                                    _3h_Psum        = config['_3h_Psum'],
                                    _3h_PVIPCHI2sum = config['_3h_PVIPCHI2sum'],
                                    _3h_TRKCHIDOFmin= config['_3h_TRKCHIDOFmin'],
                                    _3h_CORRMmax    = config['_3h_CORRMmax'],
                                    _3h_CORRMmin    = config['_3h_CORRMmin'],
                                    _3hpph_deltaMmax= config['_3hpph_deltaMmax'],
                                    _3hpph_deltaMmin= config['_3hpph_deltaMmin'])
        
	
        self.algosKKK = []
        self.algosKKK.append(self.selKKK)
        
        self.algosKpKpKp = []
        self.algosKpKpKp.append(self.selKpKpKp)
        
        self.algospph = []
        self.algospph.append(self.selpph)	
	
        
        self.lineKKK_incl = StrippingLine( _KKK_inclName + 'Line',
                                           prescale   = config['KKK_inclLinePrescale'],
                                           postscale  = config['KKK_inclLinePostscale'],
                                           algos      = self.algosKKK,
                                           FILTER = filter,
                                           RelatedInfoTools = [
                                          { 'Type' : 'RelInfoConeVariables', 'ConeAngle' : 0.5, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPASYM', 'CONEPTASYM'],
                                              'Location'  : 'ConeIso05B',
                                             "DaughterLocations" : {
                                                 "[B+ -> ^K+ K+ K-]CC" : "ConeIso05h1",
                                                 "[B+ -> K+ ^K+ K-]CC" : "ConeIso05h2",
                                                 "[B+ -> K+ K+ ^K-]CC" : "ConeIso05h3",
                                             }
},
                                          { 'Type' : 'RelInfoConeVariables', 'ConeAngle' : 1.0, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPASYM', 'CONEPTASYM'],
                                              'Location'  : 'ConeIso10B',
                                             "DaughterLocations" : {
                                                 "[B+ -> ^K+ K+ K-]CC" : "ConeIso10h1",
                                                 "[B+ -> K+ ^K+ K-]CC" : "ConeIso10h2",
                                                 "[B+ -> K+ K+ ^K-]CC" : "ConeIso10h3",
                                             }
},
                                          { 'Type' : 'RelInfoConeVariables', 'ConeAngle' : 1.5, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPASYM', 'CONEPTASYM'],
                                              'Location'  : 'ConeIso15B',
                                             "DaughterLocations" : {
                                                 "[B+ -> ^K+ K+ K-]CC" : "ConeIso15h1",
                                                 "[B+ -> K+ ^K+ K-]CC" : "ConeIso15h2",
                                                 "[B+ -> K+ K+ ^K-]CC" : "ConeIso15h3",
                                             }
},
                                          { 'Type' : 'RelInfoConeVariables', 'ConeAngle' : 2.0, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPASYM', 'CONEPTASYM'],
                                              'Location'  : 'ConeIso20B',
                                             "DaughterLocations" : {
                                                 "[B+ -> ^K+ K+ K-]CC" : "ConeIso20h1",
                                                 "[B+ -> K+ ^K+ K-]CC" : "ConeIso20h2",
                                                 "[B+ -> K+ K+ ^K-]CC" : "ConeIso20h3",
                                             }
},
                                           { 'Type' : 'RelInfoConeVariablesForEW', 'ConeAngle' : 0.5, 'Variables' : ['EWCONEANGLE','EWCONENPX','EWCONENPY','EWCONENPZ','EWCONENMULT', 'EWCONENVPT', 'EWCONENSPT', 'EWCONENVP', 'EWCONENSP'],
                                              'Location'  : 'NConeIso05B',
                                             "DaughterLocations" : {
                                                 "[B+ -> ^K+ K+ K-]CC" : "NConeIso05h1",
                                                 "[B+ -> K+ ^K+ K-]CC" : "NConeIso05h2",
                                                 "[B+ -> K+ K+ ^K-]CC" : "NConeIso05h3",
                                             }
                                           },
                                          { 'Type' : 'RelInfoConeVariablesForEW', 'ConeAngle' : 1.0, 'Variables' : ['EWCONEANGLE','EWCONENPX','EWCONENPY','EWCONENPZ','EWCONENMULT', 'EWCONENVPT', 'EWCONENSPT', 'EWCONENVP', 'EWCONENSP'],
                                              'Location'  : 'NConeIso10B',
                                             "DaughterLocations" : {
                                                 "[B+ -> ^K+ K+ K-]CC" : "NConeIso10h1",
                                                 "[B+ -> K+ ^K+ K-]CC" : "NConeIso10h2",
                                                 "[B+ -> K+ K+ ^K-]CC" : "NConeIso10h3",
                                             }
                                           },
                                          { 'Type' : 'RelInfoConeVariablesForEW', 'ConeAngle' : 1.5, 'Variables' : ['EWCONEANGLE','EWCONENPX','EWCONENPY','EWCONENPZ','EWCONENMULT', 'EWCONENVPT', 'EWCONENSPT', 'EWCONENVP', 'EWCONENSP'],
                                              'Location'  : 'NConeIso15B',
                                             "DaughterLocations" : {
                                                 "[B+ -> ^K+ K+ K-]CC" : "NConeIso15h1",
                                                 "[B+ -> K+ ^K+ K-]CC" : "NConeIso15h2",
                                                 "[B+ -> K+ K+ ^K-]CC" : "NConeIso15h3",
                                             }
                                           },
                                          { 'Type' : 'RelInfoConeVariablesForEW', 'ConeAngle' : 2.0, 'Variables' : ['EWCONEANGLE','EWCONENPX','EWCONENPY','EWCONENPZ','EWCONENMULT', 'EWCONENVPT', 'EWCONENSPT', 'EWCONENVP', 'EWCONENSP'],
                                              'Location'  : 'NConeIso20B',
                                             "DaughterLocations" : {
                                                 "[B+ -> ^K+ K+ K-]CC" : "NConeIso20h1",
                                                 "[B+ -> K+ ^K+ K-]CC" : "NConeIso20h2",
                                                 "[B+ -> K+ K+ ^K-]CC" : "NConeIso20h3",
                                             }
                                           }
                                     ]
)
        
        self.lineKpKpKp_incl = StrippingLine( _KpKpKp_inclName + 'Line',
                                              prescale   = config['KpKpKp_inclLinePrescale'],
                                              postscale  = config['KpKpKp_inclLinePostscale'],
                                              algos      = self.algosKpKpKp,
                                              FILTER = filter,
                                              RelatedInfoTools = [
                                          { 'Type' : 'RelInfoConeVariables', 'ConeAngle' : 0.5, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPASYM', 'CONEPTASYM'],
                                              'Location'  : 'ConeIso05B',
                                             "DaughterLocations" : {
                                                 "[B+ -> ^K+ K+ K+]CC" : "ConeIso05h1",
                                                 "[B+ -> K+ ^K+ K+]CC" : "ConeIso05h2",
                                                 "[B+ -> K+ K+ ^K+]CC" : "ConeIso05h3",
                                             }
},
                                          { 'Type' : 'RelInfoConeVariables', 'ConeAngle' : 1.0, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPASYM', 'CONEPTASYM'],
                                              'Location'  : 'ConeIso10B',
                                             "DaughterLocations" : {
                                                 "[B+ -> ^K+ K+ K+]CC" : "ConeIso10h1",
                                                 "[B+ -> K+ ^K+ K+]CC" : "ConeIso10h2",
                                                 "[B+ -> K+ K+ ^K+]CC" : "ConeIso10h3",
                                             }
},
                                          { 'Type' : 'RelInfoConeVariables', 'ConeAngle' : 1.5, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPASYM', 'CONEPTASYM'],
                                              'Location'  : 'ConeIso15B',
                                             "DaughterLocations" : {
                                                 "[B+ -> ^K+ K+ K+]CC" : "ConeIso15h1",
                                                 "[B+ -> K+ ^K+ K+]CC" : "ConeIso15h2",
                                                 "[B+ -> K+ K+ ^K+]CC" : "ConeIso15h3",
                                             }
},
                                          { 'Type' : 'RelInfoConeVariables', 'ConeAngle' : 2.0, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPASYM', 'CONEPTASYM'],
                                              'Location'  : 'ConeIso20B',
                                             "DaughterLocations" : {
                                                 "[B+ -> ^K+ K+ K+]CC" : "ConeIso20h1",
                                                 "[B+ -> K+ ^K+ K+]CC" : "ConeIso20h2",
                                                 "[B+ -> K+ K+ ^K+]CC" : "ConeIso20h3",
                                             }
},
                                           { 'Type' : 'RelInfoConeVariablesForEW', 'ConeAngle' : 0.5, 'Variables' : ['EWCONEANGLE','EWCONENPX','EWCONENPY','EWCONENPZ','EWCONENMULT', 'EWCONENVPT', 'EWCONENSPT', 'EWCONENVP', 'EWCONENSP'],
                                              'Location'  : 'NConeIso05B',
                                             "DaughterLocations" : {
                                                 "[B+ -> ^K+ K+ K+]CC" : "NConeIso05h1",
                                                 "[B+ -> K+ ^K+ K+]CC" : "NConeIso05h2",
                                                 "[B+ -> K+ K+ ^K+]CC" : "NConeIso05h3",
                                             }
                                           },
                                          { 'Type' : 'RelInfoConeVariablesForEW', 'ConeAngle' : 1.0, 'Variables' : ['EWCONEANGLE','EWCONENPX','EWCONENPY','EWCONENPZ','EWCONENMULT', 'EWCONENVPT', 'EWCONENSPT', 'EWCONENVP', 'EWCONENSP'],
                                              'Location'  : 'NConeIso10B',
                                             "DaughterLocations" : {
                                                 "[B+ -> ^K+ K+ K+]CC" : "NConeIso10h1",
                                                 "[B+ -> K+ ^K+ K+]CC" : "NConeIso10h2",
                                                 "[B+ -> K+ K+ ^K+]CC" : "NConeIso10h3",
                                             }
                                           },
                                          { 'Type' : 'RelInfoConeVariablesForEW', 'ConeAngle' : 1.5, 'Variables' : ['EWCONEANGLE','EWCONENPX','EWCONENPY','EWCONENPZ','EWCONENMULT', 'EWCONENVPT', 'EWCONENSPT', 'EWCONENVP', 'EWCONENSP'],
                                              'Location'  : 'NConeIso15B',
                                             "DaughterLocations" : {
                                                 "[B+ -> ^K+ K+ K+]CC" : "NConeIso15h1",
                                                 "[B+ -> K+ ^K+ K+]CC" : "NConeIso15h2",
                                                 "[B+ -> K+ K+ ^K+]CC" : "NConeIso15h3",
                                             }
                                           },
                                          { 'Type' : 'RelInfoConeVariablesForEW', 'ConeAngle' : 2.0, 'Variables' : ['EWCONEANGLE','EWCONENPX','EWCONENPY','EWCONENPZ','EWCONENMULT', 'EWCONENVPT', 'EWCONENSPT', 'EWCONENVP', 'EWCONENSP'],
                                              'Location'  : 'NConeIso20B',
                                             "DaughterLocations" : {
                                                 "[B+ -> ^K+ K+ K+]CC" : "NConeIso20h1",
                                                 "[B+ -> K+ ^K+ K+]CC" : "NConeIso20h2",
                                                 "[B+ -> K+ K+ ^K+]CC" : "NConeIso20h3",
                                             }
                                           }
                                     ]
)
        
        self.linepph_incl = StrippingLine( _pph_inclName + 'Line',
                                           prescale   = config['pph_inclLinePrescale'],
                                           postscale  = config['pph_inclLinePostscale'],
                                           algos      = self.algospph,
                                           FILTER = filter,
                                           RelatedInfoTools = [
                                          { 'Type' : 'RelInfoConeVariables', 'ConeAngle' : 0.5, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPASYM', 'CONEPTASYM'],
                                              'Location'  : 'ConeIso05B',
                                             "DaughterLocations" : {
                                                 "[B+ -> ^p+ p~- K+]CC" : "ConeIso05p1",
                                                 "[B+ -> p+ ^p~- K+]CC" : "ConeIso05p2",
                                                 "[B+ -> p+ p~- ^K+]CC" : "ConeIso05h",
                                             }
},
                                          { 'Type' : 'RelInfoConeVariables', 'ConeAngle' : 1.0, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPASYM', 'CONEPTASYM'],
                                              'Location'  : 'ConeIso10B',
                                             "DaughterLocations" : {
                                                 "[B+ -> ^p+ p~- K+]CC" : "ConeIso10p1",
                                                 "[B+ -> p+ ^p~- K+]CC" : "ConeIso10p2",
                                                 "[B+ -> p+ p~- ^K+]CC" : "ConeIso10h",
                                             }
},
                                          { 'Type' : 'RelInfoConeVariables', 'ConeAngle' : 1.5, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPASYM', 'CONEPTASYM'],
                                              'Location'  : 'ConeIso15B',
                                             "DaughterLocations" : {
                                                 "[B+ -> ^p+ p~- K+]CC" : "ConeIso15p1",
                                                 "[B+ -> p+ ^p~- K+]CC" : "ConeIso15p2",
                                                 "[B+ -> p+ p~- ^K+]CC" : "ConeIso15h",
                                             }
},
                                          { 'Type' : 'RelInfoConeVariables', 'ConeAngle' : 2.0, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPASYM', 'CONEPTASYM'],
                                              'Location'  : 'ConeIso20B',
                                             "DaughterLocations" : {
                                                 "[B+ -> ^p+ p~- K+]CC" : "ConeIso20p1",
                                                 "[B+ -> p+ ^p~- K+]CC" : "ConeIso20p2",
                                                 "[B+ -> p+ p~- ^K+]CC" : "ConeIso20h",
                                             }
},
                                           { 'Type' : 'RelInfoConeVariablesForEW', 'ConeAngle' : 0.5, 'Variables' : ['EWCONEANGLE','EWCONENPX','EWCONENPY','EWCONENPZ','EWCONENMULT', 'EWCONENVPT', 'EWCONENSPT', 'EWCONENVP', 'EWCONENSP'],
                                              'Location'  : 'NConeIso05B',
                                             "DaughterLocations" : {
                                                 "[B+ -> ^p+ p~- K+]CC" : "NConeIso05p1",
                                                 "[B+ -> p+ ^p~- K+]CC" : "NConeIso05p2",
                                                 "[B+ -> p+ p~- ^K+]CC" : "NConeIso05h",
                                             }
                                           },
                                          { 'Type' : 'RelInfoConeVariablesForEW', 'ConeAngle' : 1.0, 'Variables' : ['EWCONEANGLE','EWCONENPX','EWCONENPY','EWCONENPZ','EWCONENMULT', 'EWCONENVPT', 'EWCONENSPT', 'EWCONENVP', 'EWCONENSP'],
                                              'Location'  : 'NConeIso10B',
                                             "DaughterLocations" : {
                                                 "[B+ -> ^p+ p~- K+]CC" : "NConeIso10p1",
                                                 "[B+ -> p+ ^p~- K+]CC" : "NConeIso10p2",
                                                 "[B+ -> p+ p~- ^K+]CC" : "NConeIso10h",
                                             }
                                           },
                                          { 'Type' : 'RelInfoConeVariablesForEW', 'ConeAngle' : 1.5, 'Variables' : ['EWCONEANGLE','EWCONENPX','EWCONENPY','EWCONENPZ','EWCONENMULT', 'EWCONENVPT', 'EWCONENSPT', 'EWCONENVP', 'EWCONENSP'],
                                              'Location'  : 'NConeIso15B',
                                             "DaughterLocations" : {
                                                 "[B+ -> ^p+ p~- K+]CC" : "NConeIso15p1",
                                                 "[B+ -> p+ ^p~- K+]CC" : "NConeIso15p2",
                                                 "[B+ -> p+ p~- ^K+]CC" : "NConeIso15h",
                                             }
                                           },
                                          { 'Type' : 'RelInfoConeVariablesForEW', 'ConeAngle' : 2.0, 'Variables' : ['EWCONEANGLE','EWCONENPX','EWCONENPY','EWCONENPZ','EWCONENMULT', 'EWCONENVPT', 'EWCONENSPT', 'EWCONENVP', 'EWCONENSP'],
                                              'Location'  : 'NConeIso20B',
                                             "DaughterLocations" : {
                                                 "[B+ -> ^p+ p~- K+]CC" : "NConeIso20p1",
                                                 "[B+ -> p+ ^p~- K+]CC" : "NConeIso20p2",
                                                 "[B+ -> p+ p~- ^K+]CC" : "NConeIso20h",
                                             }
                                           }
                                     ]
)
        
        self.registerLine(self.lineKKK_incl)
        self.registerLine(self.lineKpKpKp_incl)
        self.registerLine(self.linepph_incl)
        
        
def makeKKK_incl(name,
                 _h_PT,
                 _h_P,
                 _h_IPCHI2,
                 _h_TRCHI2DOF,
                 _h_TRGHP,      
                 _3h_DOCA,
                 _3h_PTmax,
                 _3h_DIRA,
                 _3h_FDCHI2,
                 _3h_PVDOCAmin,
                 _3h_CHI2,
                 _3h_IPCHI2,
                 _3h_PT,
                 _3h_PTsum,
                 _3h_Psum,
                 _3h_PVIPCHI2sum,
                 _3h_TRKCHIDOFmin,
                 _3h_CORRMmax,
                 _3h_CORRMmin,
                 _3hKKK_Mmin,
                 _3hKKK_Mmax) :
  
  _daughtersCuts = {"K+" : "(PT > %(_h_PT)s*MeV) \
                             & (P > %(_h_P)s*MeV) \
                             & (MIPCHI2DV(PRIMARY) > %(_h_IPCHI2)s) \
                             & (TRCHI2DOF < %(_h_TRCHI2DOF)s) \
                             & (TRGHOSTPROB < %(_h_TRGHP)s)" % locals()}
  _combinationCut = "(AM < %(_3hKKK_Mmax)s*MeV) \
                     & (AM > %(_3hKKK_Mmin)s*MeV) \
		     & (AMAXDOCA('LoKi::TrgDistanceCalculator') < %(_3h_DOCA)s)" % locals()
  _motherCut = "(MAXTREE(((ABSID=='K+') | (ABSID=='K-')),PT) > %(_3h_PTmax)s*MeV) \
                & (BPVDIRA > %(_3h_DIRA)s) \
		& (BPVVDCHI2 > %(_3h_FDCHI2)s) \
		& (VFASPF(VMINVDDV(PRIMARY)) > %(_3h_PVDOCAmin)s) \
		& (VFASPF(VCHI2) < %(_3h_CHI2)s) \
		& (MIPCHI2DV(PRIMARY) < %(_3h_IPCHI2)s) \
		& (PT > %(_3h_PT)s*MeV) \
		& (SUMTREE(PT,((ABSID=='K+') | (ABSID=='K-')),0.0) > %(_3h_PTsum)s*MeV) \
		& (SUMTREE(P,((ABSID=='K+') | (ABSID=='K-')),0.0) > %(_3h_Psum)s*MeV) \
		& (SUMTREE(MIPCHI2DV(PRIMARY),((ABSID=='K+') | (ABSID=='K-')),0.0) > %(_3h_PVIPCHI2sum)s) \
		& (MINTREE((('K+'==ABSID) | ('K-'==ABSID)),TRCHI2DOF) < %(_3h_TRKCHIDOFmin)s) \
		& (BPVCORRM < %(_3h_CORRMmax)s * MeV)& (BPVCORRM > %(_3h_CORRMmin)s*MeV)" % locals()
  
  _KKK=CombineParticles()
  _KKK.DecayDescriptors  = ["[B+ -> K+ K+ K-]cc"]
  _KKK.MotherCut         = _motherCut
  _KKK.CombinationCut    = _combinationCut
  _KKK.DaughtersCuts     = _daughtersCuts
  
  return Selection ( name,
                     Algorithm = _KKK,
                     RequiredSelections = [StdNoPIDsKaons])


def makeKpKpKp_incl(name,
                    _h_PT,
                    _h_P,
                    _h_IPCHI2,
                    _h_TRCHI2DOF,
                    _h_TRGHP,      
                    _3h_DOCA,
                    _3h_PTmax,
                    _3h_DIRA,
                    _3h_FDCHI2,
                    _3h_PVDOCAmin,
                    _3h_CHI2,
                    _3h_IPCHI2,
                    _3h_PT,
                    _3h_PTsum,
                    _3h_Psum,
                    _3h_PVIPCHI2sum,
                    _3h_TRKCHIDOFmin,
                    _3h_CORRMmax,
                    _3h_CORRMmin,
                    _3hKKK_Mmin,
                    _3hKKK_Mmax) :
  
  _daughtersCuts = {"K+" : "(PT > %(_h_PT)s*MeV) \
                             & (P > %(_h_P)s*MeV) \
                             & (MIPCHI2DV(PRIMARY) > %(_h_IPCHI2)s) \
                             & (TRCHI2DOF < %(_h_TRCHI2DOF)s) \
                             & (TRGHOSTPROB < %(_h_TRGHP)s)" % locals()}
  _combinationCut = "(AM < %(_3hKKK_Mmax)s*MeV) \
                     & (AM > %(_3hKKK_Mmin)s*MeV) \
		     & (AMAXDOCA('LoKi::TrgDistanceCalculator') < %(_3h_DOCA)s)" % locals()
  _motherCut = "(MAXTREE(((ABSID=='K+') | (ABSID=='K-')),PT) > %(_3h_PTmax)s*MeV) \
                & (BPVDIRA > %(_3h_DIRA)s) \
		& (BPVVDCHI2 > %(_3h_FDCHI2)s) \
		& (VFASPF(VMINVDDV(PRIMARY)) > %(_3h_PVDOCAmin)s) \
		& (VFASPF(VCHI2) < %(_3h_CHI2)s) \
		& (MIPCHI2DV(PRIMARY) < %(_3h_IPCHI2)s) \
		& (PT > %(_3h_PT)s*MeV) \
		& (SUMTREE(PT,((ABSID=='K+') | (ABSID=='K-')),0.0) > %(_3h_PTsum)s*MeV) \
		& (SUMTREE(P,((ABSID=='K+') | (ABSID=='K-')),0.0) > %(_3h_Psum)s*MeV) \
		& (SUMTREE(MIPCHI2DV(PRIMARY),((ABSID=='K+') | (ABSID=='K-')),0.0) > %(_3h_PVIPCHI2sum)s) \
		& (MINTREE((('K+'==ABSID) | ('K-'==ABSID)),TRCHI2DOF) < %(_3h_TRKCHIDOFmin)s) \
		& (BPVCORRM < %(_3h_CORRMmax)s * MeV)& (BPVCORRM > %(_3h_CORRMmin)s*MeV)" % locals()
  
  _KpKpKp=CombineParticles()
  _KpKpKp.DecayDescriptors  = ["[B+ -> K+ K+ K+]cc"]
  _KpKpKp.MotherCut         = _motherCut
  _KpKpKp.CombinationCut    = _combinationCut
  _KpKpKp.DaughtersCuts     = _daughtersCuts
  
  return Selection ( name,
                     Algorithm = _KpKpKp,
                     RequiredSelections = [StdNoPIDsKaons])


def makepph_incl(name,
                 _h_PT,
                 _h_P,
                 _h_IPCHI2,
                 _h_TRCHI2DOF,
                 _h_TRGHP,            
                 _3h_DOCA,
                 _3h_PTmax,
                 _3h_DIRA,
                 _3h_FDCHI2,
                 _3h_PVDOCAmin,
                 _3h_CHI2,
                 _3h_IPCHI2,
                 _3h_PT,
                 _3h_PTsum,
                 _3h_Psum,
                 _3h_PVIPCHI2sum,
                 _3h_TRKCHIDOFmin,
                 _3h_CORRMmax,
                 _3h_CORRMmin,
                 _3hpph_deltaMmax,
                 _3hpph_deltaMmin) :
  
  _daughtersCuts = {"p+" : "(PT > %(_h_PT)s*MeV) \
                             & (P > %(_h_P)s*MeV) \
                             & (MIPCHI2DV(PRIMARY) > %(_h_IPCHI2)s) \
                             & (TRCHI2DOF < %(_h_TRCHI2DOF)s) \
                             & (TRGHOSTPROB < %(_h_TRGHP)s)" % locals(),
                    "K+" :  "(PT > %(_h_PT)s*MeV) \
                             & (P > %(_h_P)s*MeV) \
                             & (MIPCHI2DV(PRIMARY) > %(_h_IPCHI2)s) \
                             & (TRCHI2DOF < %(_h_TRCHI2DOF)s) \
                             & (TRGHOSTPROB < %(_h_TRGHP)s)" % locals()}
  _combinationCut = "(AM < (5279.15 + %(_3hpph_deltaMmax)s)*MeV) \
                     & (AM > (5279.15 - %(_3hpph_deltaMmin)s)*MeV) \
		     & (AMAXDOCA('LoKi::TrgDistanceCalculator') < %(_3h_DOCA)s)" % locals()
  _motherCut = "(MAXTREE(((ABSID=='p+') |(ABSID=='p~-') |(ABSID=='K+') | (ABSID=='K-')),PT) > %(_3h_PTmax)s*MeV) \
                & (BPVDIRA > %(_3h_DIRA)s) \
		& (BPVVDCHI2 > %(_3h_FDCHI2)s) \
		& (VFASPF(VMINVDDV(PRIMARY)) > %(_3h_PVDOCAmin)s) \
		& (VFASPF(VCHI2) < %(_3h_CHI2)s) \
		& (MIPCHI2DV(PRIMARY) < %(_3h_IPCHI2)s) \
		& (PT > %(_3h_PT)s*MeV) \
		& (SUMTREE(PT,((ABSID=='p+') |(ABSID=='p~-') |(ABSID=='K+') | (ABSID=='K-')),0.0) > %(_3h_PTsum)s*MeV) \
		& (SUMTREE(P,((ABSID=='p+') |(ABSID=='p~-') |(ABSID=='K+') | (ABSID=='K-')),0.0) > %(_3h_Psum)s*MeV) \
		& (SUMTREE(MIPCHI2DV(PRIMARY),((ABSID=='p+') |(ABSID=='p~-') |(ABSID=='K+') | (ABSID=='K-')),0.0) > %(_3h_PVIPCHI2sum)s) \
		& (MINTREE(((ABSID=='p+') |(ABSID=='p~-') |('K+'==ABSID) | ('K-'==ABSID)),TRCHI2DOF) < %(_3h_TRKCHIDOFmin)s) \
		& (BPVCORRM < %(_3h_CORRMmax)s * MeV)& (BPVCORRM > %(_3h_CORRMmin)s*MeV)" % locals()
  
  _pph=CombineParticles()
  _pph.DecayDescriptors  = ["[B+ -> p+ p~- K+]cc"]
  _pph.MotherCut         = _motherCut
  _pph.CombinationCut    = _combinationCut
  _pph.DaughtersCuts     = _daughtersCuts
  
  return Selection ( name,
                     Algorithm = _pph,
                     RequiredSelections = [StdNoPIDsKaons,StdLooseProtons])


