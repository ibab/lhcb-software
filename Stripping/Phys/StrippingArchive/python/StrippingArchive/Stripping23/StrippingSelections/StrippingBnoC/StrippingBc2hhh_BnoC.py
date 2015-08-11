 # $Id: StrippingBc2hhh_BnoC.py,v 1.5 2015-07-31$
'''
Module for construction of Bc->hhh from:   
     Exclusive lines to reconstruct the pipipi, Kpipi, KKpi, KKK, pppi and ppK decay modes.
     
Exported symbols (use python help!):
   - Bc2hhhBuilder
   - makepipipi_excl  
   - makeKpipi_excl  
   - makeKKpi_excl  
   - makeKKK_excl  
   - makepppi_excl  
   - makeppK_excl  
'''

__author__ = ['Alvaro Gomes', 'Adlene Hicheur']
__date__ = '31/08/2014'
__version__ = '$Revision: 1.5 $'

__all__ = ('Bc2hhhBuilder',
           'makepipipi_excl', 
           'makeKpipi_excl', 
           'makeKKpi_excl', 
           'makeKKK_excl', 
           'makepppi_excl', 
           'makeppK_excl', 
	   )


config_params = {
    'MaxTrSIZE'             : 200,      ## GEC maximim recSummaryTrack(LHCb.RecSummary.nLongTracks, TrLONG)
    '_h_PT'                 : 300.,     ## tracks min PT
    '_h_P'                  : 2500.,    ## tracks min P  
    '_h_IPCHI2'             : 1.,       ## min tracks IP wrt OWNPV
    '_h_TRCHI2DOF'          : 3.0,      ## max tracks CHI2DOF
    '_h_TRGHP'              : .5,        ## Track Ghost Probability     
    '_3h_DOCA'              : .2,       ## max DOCA between h and 2h 
    '_3h_PTmax'             : 1500,     ## min PT of the 3h highest PT track
    '_3h_DIRA'              : .9999,   ## min cos angle between 3h momentum and PV decay direction   
    '_3h_FDCHI2'            : 150.,     ## min 3h FDCHI2 wrt best 3h PV  
    '_3h_SVPV'              : 1.5,      ## Distance between sv and pv
    '_3h_CHI2'              : 40.0,     ## max 3h vertex CHI2 
    '_3h_IPCHI2'            : 10.,      ## max 3h IP CHI2 wrt best 3h PV
    '_3h_PT'                : 1000.,    ## min 3h PT   
    '_3h_PTsum'             : 4500.,    ## min of 3h tracks PT sum 
    '_3h_Psum'              : 22000.,   ## min of 3h tracks P sum 
    '_3h_PVIPCHI2sum'       : 200.,     ## min of the 3h tracks IP wrt best 3h PV
    '_3h_TRKCHIDOFmin'      : 3.0,       ## max track CHI2DOF for the track with smalest CHI2DOF
    '_3h_Mmax'              : 6500.,     ## max 3h mass for exclusive lines
    '_3h_Mmin'              : 6000.,     ## min 3h mass for exclusive KKK lines
    '_bu3h_Mmax'            : 5500.,     ## min buto3h mass for exclusive lines
    '_bu3h_Mmin'            : 5100.,     ## min buto3h mass for exclusive lines
    '_probnnpi'             : .15,       ## PROBNNpi for pion tracks. WARNING: DO NOT APPLY THIS CUT FOR PIPIPI OR KPIPI LINES
    '_probnnk'              : .20,       ## PROBNNk for kaon tracks.
    '_probnnp'              : .05,       ## PROBNNp for proton tracks.
    'pipipi_exclLinePrescale'  : 1.0,
    'pipipi_exclLinePostscale'  : 1.0,
    'Kpipi_exclLinePrescale'  : 1.0,
    'Kpipi_exclLinePostscale'  : 1.0,
    'KKpi_exclLinePrescale'  : 1.0,
    'KKpi_exclLinePostscale'  : 1.0,
    'KKK_exclLinePrescale'  : 1.0,
    'KKK_exclLinePostscale'  : 1.0,
    'pppi_exclLinePrescale'  : 1.0,
    'pppi_exclLinePostscale'  : 1.0,
    'ppK_exclLinePrescale'  : 1.0,
    'ppK_exclLinePostscale'  : 1.0,
    }


"""
B_c+ -> h+h+h- channels
"""

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles

import StandardParticles

if hasattr(StandardParticles, "StdAllNoPIDsKaons"):
  from StandardParticles import StdAllNoPIDsKaons as StdNoPIDsKaons
else:
  from StandardParticles import StdNoPIDsKaons as StdNoPIDsKaons

if hasattr(StandardParticles, "StdAllNoPIDsPions"):
  from StandardParticles import StdAllNoPIDsPions as StdNoPIDsPions
else:
  from StandardParticles import StdNoPIDsPions as StdNoPIDsPions

if hasattr(StandardParticles, "StdAllLooseProtons"):
  from StandardParticles import StdAllLooseProtons as StdLooseProtons
else:
  from StandardParticles import StdLooseProtons as StdLooseProtons

from PhysSelPython.Wrappers import Selection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from Configurables import LoKi__VoidFilter as VoidFilter

default_config = {
  'Bc2hhh' : {
    'WGs'         : ['BnoC'],
    'BUILDERTYPE' : 'Bc2hhhBuilder',
    'CONFIG'      : { 'MaxTrSIZE'             : 200,      ## GEC maximim recSummaryTrack(LHCb.RecSummary.nLongTracks, TrLONG)
                      '_h_PT'                 : 300.,     ## tracks min PT
                      '_h_P'                  : 2500.,    ## tracks min P  
                      '_h_IPCHI2'             : 1.,       ## min tracks IP wrt OWNPV
                      '_h_TRCHI2DOF'          : 3.0,      ## max tracks CHI2DOF
                      '_h_TRGHP'              : .5,        ## Track Ghost Probability     
                      '_3h_DOCA'              : .2,       ## max DOCA between h and 2h 
                      '_3h_PTmax'             : 1500,     ## min PT of the 3h highest PT track
                      '_3h_DIRA'              : .9999,   ## min cos angle between 3h momentum and PV decay direction   
                      '_3h_FDCHI2'            : 150.,     ## min 3h FDCHI2 wrt best 3h PV  
                      '_3h_SVPV'              : 1.5,      ## Distance between sv and pv
                      '_3h_CHI2'              : 40.0,     ## max 3h vertex CHI2 
                      '_3h_IPCHI2'            : 10.,      ## max 3h IP CHI2 wrt best 3h PV
                      '_3h_PT'                : 1000.,    ## min 3h PT   
                      '_3h_PTsum'             : 4500.,    ## min of 3h tracks PT sum 
                      '_3h_Psum'              : 22000.,   ## min of 3h tracks P sum 
                      '_3h_PVIPCHI2sum'       : 200.,     ## min of the 3h tracks IP wrt best 3h PV
                      '_3h_TRKCHIDOFmin'      : 3.0,       ## max track CHI2DOF for the track with smalest CHI2DOF
                      '_3h_Mmax'              : 6500.,     ## max 3h mass for exclusive lines
                      '_3h_Mmin'              : 6000.,     ## min 3h mass for exclusive KKK lines
                      '_bu3h_Mmax'            : 5500.,     ## min buto3h mass for exclusive lines
                      '_bu3h_Mmin'            : 5100.,     ## min buto3h mass for exclusive lines
                      '_probnnpi'             : .15,       ## PROBNNpi for pion tracks. WARNING: DO NOT APPLY THIS CUT FOR PIPIPI OR KPIPI LINES
                      '_probnnk'              : .20,       ## PROBNNk for kaon tracks.
                      '_probnnp'              : .05,       ## PROBNNp for proton tracks.
                      'pipipi_exclLinePrescale'  : 1.0,
                      'pipipi_exclLinePostscale'  : 1.0,
                      'Kpipi_exclLinePrescale'  : 1.0,
                      'Kpipi_exclLinePostscale'  : 1.0,
                      'KKpi_exclLinePrescale'  : 1.0,
                      'KKpi_exclLinePostscale'  : 1.0,
                      'KKK_exclLinePrescale'  : 1.0,
                      'KKK_exclLinePostscale'  : 1.0,
                      'pppi_exclLinePrescale'  : 1.0,
                      'pppi_exclLinePostscale'  : 1.0,
                      'ppK_exclLinePrescale'  : 1.0,
                      'ppK_exclLinePostscale'  : 1.0,
                    }, 
    'STREAMS'     : ['Bhadron']
    }
}

filter = {'Code' :
          "(recSummaryTrack(LHCb.RecSummary.nLongTracks, TrLONG)"\
          " < %s )" \
          % config_params['MaxTrSIZE'],
          'Preambulo' : [ "from LoKiTracks.decorators import *",
                                      'from LoKiCore.functions    import *' ]
}

default_name = "Bc2hhh"

class Bc2hhhBuilder(LineBuilder) :
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
                              '_3h_SVPV',      
                              '_3h_CHI2',           
                              '_3h_IPCHI2',             
                              '_3h_PT',              
                              '_3h_PTsum',              
                              '_3h_Psum',              
                              '_3h_PVIPCHI2sum',              
			      '_3h_TRKCHIDOFmin',
                              '_3h_Mmax',                 
                              '_3h_Mmin',
                              '_bu3h_Mmax',
                              '_bu3h_Mmin',
                              '_probnnpi',
                              '_probnnk',
                              '_probnnp',
                              'pipipi_exclLinePrescale',
                              'pipipi_exclLinePostscale',
                              'Kpipi_exclLinePrescale',  
                              'Kpipi_exclLinePostscale', 
                              'KKpi_exclLinePrescale', 
                              'KKpi_exclLinePostscale',
                              'KKK_exclLinePrescale', 
                              'KKK_exclLinePostscale',
                              'pppi_exclLinePrescale',
                              'pppi_exclLinePostscale',
                              'ppK_exclLinePrescale', 
                              'ppK_exclLinePostscale')

    def __init__(self, name, config) :

        LineBuilder.__init__(self, name, config)

        _pipipi_exclName = name + '_pipipi_excl'
        _Kpipi_exclName = name + '_Kpipi_excl'
        _KKpi_exclName = name + '_KKpi_excl'
        _KKK_exclName = name + '_KKK_excl'
        _pppi_exclName = name + '_pppi_excl'
        _ppK_exclName = name + '_ppK_excl'

        self.selpipipi = makepipipi_excl( 'pipipiexclFor' + _pipipi_exclName + 'Sel', 
                             _h_PT           = config['_h_PT'],
                             _h_P            = config['_h_P'],
                             _h_IPCHI2       = config['_h_IPCHI2'],
                             _h_TRCHI2DOF    = config['_h_TRCHI2DOF'],
                             _h_TRGHP       = config['_h_TRGHP'],
                             _3h_DOCA        = config['_3h_DOCA'],
                             _3h_PTmax       = config['_3h_PTmax'],
                             _3h_DIRA        = config['_3h_DIRA'],
                             _3h_FDCHI2      = config['_3h_FDCHI2'],
                             _3h_SVPV   = config['_3h_SVPV'],
                             _3h_CHI2        = config['_3h_CHI2'],
                             _3h_IPCHI2      = config['_3h_IPCHI2'],
                             _3h_PT          = config['_3h_PT'],
                             _3h_PTsum       = config['_3h_PTsum'],
                             _3h_Psum        = config['_3h_Psum'],
                             _3h_PVIPCHI2sum = config['_3h_PVIPCHI2sum'],
                             _3h_TRKCHIDOFmin= config['_3h_TRKCHIDOFmin'],
                             _3h_Mmin     = config['_3h_Mmin'],
                             _3h_Mmax     = config['_3h_Mmax'],
                             _bu3h_Mmin   = config['_bu3h_Mmin'],
                             _bu3h_Mmax   = config['_bu3h_Mmax'])

        self.selKpipi = makeKpipi_excl( 'KpipiexclFor' + _Kpipi_exclName + 'Sel', 
                             _h_PT           = config['_h_PT'],
                             _h_P            = config['_h_P'],
                             _h_IPCHI2       = config['_h_IPCHI2'],
                             _h_TRCHI2DOF    = config['_h_TRCHI2DOF'],
                             _h_TRGHP       = config['_h_TRGHP'],
                             _3h_DOCA        = config['_3h_DOCA'],
                             _3h_PTmax       = config['_3h_PTmax'],
                             _3h_DIRA        = config['_3h_DIRA'],
                             _3h_FDCHI2      = config['_3h_FDCHI2'],
                             _3h_SVPV   = config['_3h_SVPV'],
                             _3h_CHI2        = config['_3h_CHI2'],
                             _3h_IPCHI2      = config['_3h_IPCHI2'],
                             _3h_PT          = config['_3h_PT'],
                             _3h_PTsum       = config['_3h_PTsum'],
                             _3h_Psum        = config['_3h_Psum'],
                             _3h_PVIPCHI2sum = config['_3h_PVIPCHI2sum'],
                             _3h_TRKCHIDOFmin= config['_3h_TRKCHIDOFmin'],
                             _3h_Mmin     = config['_3h_Mmin'],
                             _3h_Mmax     = config['_3h_Mmax'],
                             _bu3h_Mmin   = config['_bu3h_Mmin'],
                             _bu3h_Mmax   = config['_bu3h_Mmax'],
                             _probnnk     = config['_probnnk'])                                          

        self.selKKpi = makeKKpi_excl( 'KKpiexclFor' + _KKpi_exclName + 'Sel', 
                             _h_PT           = config['_h_PT'],
                             _h_P            = config['_h_P'],
                             _h_IPCHI2       = config['_h_IPCHI2'],
                             _h_TRCHI2DOF    = config['_h_TRCHI2DOF'],
                             _h_TRGHP       = config['_h_TRGHP'],
                             _3h_DOCA        = config['_3h_DOCA'],
                             _3h_PTmax       = config['_3h_PTmax'],
                             _3h_DIRA        = config['_3h_DIRA'],
                             _3h_FDCHI2      = config['_3h_FDCHI2'],
                             _3h_SVPV   = config['_3h_SVPV'],
                             _3h_CHI2        = config['_3h_CHI2'],
                             _3h_IPCHI2      = config['_3h_IPCHI2'],
                             _3h_PT          = config['_3h_PT'],
                             _3h_PTsum       = config['_3h_PTsum'],
                             _3h_Psum        = config['_3h_Psum'],
                             _3h_PVIPCHI2sum = config['_3h_PVIPCHI2sum'],
                             _3h_TRKCHIDOFmin= config['_3h_TRKCHIDOFmin'],
                             _3h_Mmin     = config['_3h_Mmin'],
                             _3h_Mmax     = config['_3h_Mmax'],
                             _bu3h_Mmin   = config['_bu3h_Mmin'],
                             _bu3h_Mmax   = config['_bu3h_Mmax'],
                             _probnnpi    = config['_probnnpi'],
                             _probnnk     = config['_probnnk'])

        self.selKKK = makeKKK_excl( 'KKKexclFor' + _KKK_exclName + 'Sel', 
                             _h_PT           = config['_h_PT'],
                             _h_P            = config['_h_P'],
                             _h_IPCHI2       = config['_h_IPCHI2'],
                             _h_TRCHI2DOF    = config['_h_TRCHI2DOF'],
                             _h_TRGHP       = config['_h_TRGHP'],
                             _3h_DOCA        = config['_3h_DOCA'],
                             _3h_PTmax       = config['_3h_PTmax'],
                             _3h_DIRA        = config['_3h_DIRA'],
                             _3h_FDCHI2      = config['_3h_FDCHI2'],
                             _3h_SVPV   = config['_3h_SVPV'],
                             _3h_CHI2        = config['_3h_CHI2'],
                             _3h_IPCHI2      = config['_3h_IPCHI2'],
                             _3h_PT          = config['_3h_PT'],
                             _3h_PTsum       = config['_3h_PTsum'],
                             _3h_Psum        = config['_3h_Psum'],
                             _3h_PVIPCHI2sum = config['_3h_PVIPCHI2sum'],
                             _3h_TRKCHIDOFmin= config['_3h_TRKCHIDOFmin'],
                             _3h_Mmin     = config['_3h_Mmin'],
                             _3h_Mmax     = config['_3h_Mmax'],
                             _bu3h_Mmin   = config['_bu3h_Mmin'],
                             _bu3h_Mmax   = config['_bu3h_Mmax'],
                             _probnnk     = config['_probnnk'])

        self.selpppi = makepppi_excl( 'pppiexclFor' + _pppi_exclName + 'Sel', 
                             _h_PT           = config['_h_PT'],
                             _h_P            = config['_h_P'],
                             _h_IPCHI2       = config['_h_IPCHI2'],
                             _h_TRCHI2DOF    = config['_h_TRCHI2DOF'],
                             _h_TRGHP       = config['_h_TRGHP'],
                             _3h_DOCA        = config['_3h_DOCA'],
                             _3h_PTmax       = config['_3h_PTmax'],
                             _3h_DIRA        = config['_3h_DIRA'],
                             _3h_FDCHI2      = config['_3h_FDCHI2'],
                             _3h_SVPV   = config['_3h_SVPV'],
                             _3h_CHI2        = config['_3h_CHI2'],
                             _3h_IPCHI2      = config['_3h_IPCHI2'],
                             _3h_PT          = config['_3h_PT'],
                             _3h_PTsum       = config['_3h_PTsum'],
                             _3h_Psum        = config['_3h_Psum'],
                             _3h_PVIPCHI2sum = config['_3h_PVIPCHI2sum'],
                             _3h_TRKCHIDOFmin= config['_3h_TRKCHIDOFmin'],
                             _3h_Mmin     = config['_3h_Mmin'],
                             _3h_Mmax     = config['_3h_Mmax'],
                             _bu3h_Mmin   = config['_bu3h_Mmin'],
                             _bu3h_Mmax   = config['_bu3h_Mmax'],
                             _probnnpi    = config['_probnnpi'],
                             _probnnp     = config['_probnnp'])

        self.selppK = makeppK_excl( 'ppKexclFor' + _ppK_exclName + 'Sel', 
                             _h_PT           = config['_h_PT'],
                             _h_P            = config['_h_P'],
                             _h_IPCHI2       = config['_h_IPCHI2'],
                             _h_TRCHI2DOF    = config['_h_TRCHI2DOF'],
                             _h_TRGHP       = config['_h_TRGHP'],       
                             _3h_DOCA        = config['_3h_DOCA'],
                             _3h_PTmax       = config['_3h_PTmax'],
                             _3h_DIRA        = config['_3h_DIRA'],
                             _3h_FDCHI2      = config['_3h_FDCHI2'],
                             _3h_SVPV   = config['_3h_SVPV'],
                             _3h_CHI2        = config['_3h_CHI2'],
                             _3h_IPCHI2      = config['_3h_IPCHI2'],
                             _3h_PT          = config['_3h_PT'],
                             _3h_PTsum       = config['_3h_PTsum'],
                             _3h_Psum        = config['_3h_Psum'],
                             _3h_PVIPCHI2sum = config['_3h_PVIPCHI2sum'],
                             _3h_TRKCHIDOFmin= config['_3h_TRKCHIDOFmin'],
                             _3h_Mmin     = config['_3h_Mmin'],
                             _3h_Mmax     = config['_3h_Mmax'],
                             _bu3h_Mmin   = config['_bu3h_Mmin'],
                             _bu3h_Mmax   = config['_bu3h_Mmax'],
                             _probnnk     = config['_probnnk'],
                             _probnnp     = config['_probnnp'])

	
        self.algospipipi = []
        self.algospipipi.append(self.selpipipi)

        self.algosKpipi = []
        self.algosKpipi.append(self.selKpipi)

        self.algosKKpi = []
        self.algosKKpi.append(self.selKKpi)

        self.algosKKK = []
        self.algosKKK.append(self.selKKK)

        self.algospppi = []
        self.algospppi.append(self.selpppi)

        self.algosppK = []
        self.algosppK.append(self.selppK)	
	

        self.linepipipi_excl = StrippingLine( _pipipi_exclName + 'Line',
                                     prescale   = config['pipipi_exclLinePrescale'],
                                     postscale  = config['pipipi_exclLinePostscale'],
                                     algos      = self.algospipipi,
                                     FILTER = filter,
                                     RelatedInfoTools = [
                                          { 'Type' : 'RelInfoConeVariables', 'ConeAngle' : 0.5, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'],
                                            'RecursionLevel' : 1,
                                            'Locations' : {
                                                   self.selpipipi : 'ConeIso05B', 
                                                   'Phys/StdAllNoPIDsPions'   :                                                     ["ConeIso05pi1", "ConeIso05pi2", "ConeIso05pi3"],
                                               }                                                                           },
                                          { 'Type' : 'RelInfoConeVariables', 'ConeAngle' : 1.0, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'],
                                            'RecursionLevel' : 1,
                                            'Locations' : {
                                                   self.selpipipi : 'ConeIso10B', 
                                                   'Phys/StdAllNoPIDsPions'   :                                                     ["ConeIso10pi1", "ConeIso10pi2", "ConeIso10pi3"],
                                               }                                                                           },
                                          { 'Type' : 'RelInfoConeVariables', 'ConeAngle' : 1.5, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'],
                                            'RecursionLevel' : 1,
                                            'Locations' : {
                                                   self.selpipipi : 'ConeIso15B', 
                                                   'Phys/StdAllNoPIDsPions'   :                                                     ["ConeIso15pi1", "ConeIso15pi2", "ConeIso15pi3"],
                                               }                                                                           },
                                           { 'Type' : 'RelInfoConeVariablesForEW', 'ConeAngle' : 0.5, 'Variables' : ['EWCONEANGLE','EWCONENPX','EWCONENPY','EWCONENPZ','EWCONENMULT'],
                                            'RecursionLevel' : 1,
                                            'Locations' : {
                                                   self.selpipipi : 'NConeIso05B', 
                                                   'Phys/StdAllNoPIDsPions'   :                                                     ["NConeIso05pi1", "NConeIso05pi2", "NConeIso05pi3"],
                                               }                                                                           },
                                          { 'Type' : 'RelInfoConeVariablesForEW', 'ConeAngle' : 1.0, 'Variables' : ['EWCONEANGLE','EWCONENPX','EWCONENPY','EWCONENPZ','EWCONENMULT'],
                                            'RecursionLevel' : 1,
                                            'Locations' : {
                                                   self.selpipipi : 'NConeIso10B', 
                                                   'Phys/StdAllNoPIDsPions'   :                                                     ["NConeIso10pi1", "NConeIso10pi2", "NConeIso10pi3"],
                                               }                                                                           },
                                          { 'Type' : 'RelInfoConeVariablesForEW', 'ConeAngle' : 1.5, 'Variables' : ['EWCONEANGLE','EWCONENPX','EWCONENPY','EWCONENPZ','EWCONENMULT'],
                                            'RecursionLevel' : 1,
                                            'Locations' : {
                                                   self.selpipipi : 'NConeIso15B', 
                                                   'Phys/StdAllNoPIDsPions'   :                                                     ["NConeIso15pi1", "NConeIso15pi2", "NConeIso15pi3"],
                                               }                                                                           },
                                     ],

)

        self.lineKpipi_excl = StrippingLine( _Kpipi_exclName + 'Line',
                                     prescale   = config['Kpipi_exclLinePrescale'],
                                     postscale  = config['Kpipi_exclLinePostscale'],
                                     algos      = self.algosKpipi,
                                     FILTER = filter,
                                     RelatedInfoTools = [
                                          { 'Type' : 'RelInfoConeVariables', 'ConeAngle' : 0.5, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'],
                                            'RecursionLevel' : 1,
                                            'Locations' : {
                                                   self.selKpipi : 'ConeIso05B', 
                                                   'Phys/StdAllNoPIDsPions'   :                                                     ["ConeIso05pi1", "ConeIso05pi2"],
                                                   'Phys/StdAllNoPIDsKaons'   : 'ConeIso05K'
                                               }                                                                           },
                                          { 'Type' : 'RelInfoConeVariables', 'ConeAngle' : 1.0, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'],
                                            'RecursionLevel' : 1,
                                            'Locations' : {
                                                   self.selKpipi : 'ConeIso10B', 
                                                   'Phys/StdAllNoPIDsPions'   :                                                     ["ConeIso10pi1", "ConeIso10pi2"],
                                                   'Phys/StdAllNoPIDsKaons'   : 'ConeIso10K'
                                               }                                                                           },
                                          { 'Type' : 'RelInfoConeVariables', 'ConeAngle' : 1.5, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'],
                                            'RecursionLevel' : 1,
                                            'Locations' : {
                                                   self.selKpipi : 'ConeIso15B', 
                                                   'Phys/StdAllNoPIDsPions'   :                                                     ["ConeIso15pi1", "ConeIso15pi2"],
                                                   'Phys/StdAllNoPIDsKaons'   : 'ConeIso15K'
                                               }                                                                           },
                                          { 'Type' : 'RelInfoConeVariablesForEW', 'ConeAngle' : 0.5, 'Variables' : ['EWCONEANGLE','EWCONENPX','EWCONENPY','EWCONENPZ','EWCONENMULT'],
                                            'RecursionLevel' : 1,
                                            'Locations' : {
                                                   self.selKpipi : 'NConeIso05B', 
                                                   'Phys/StdAllNoPIDsPions'   :                                                     ["NConeIso05pi1", "NConeIso05pi2"],
                                                   'Phys/StdAllNoPIDsKaons'   : 'NConeIso05K'
                                               }                                                                           },
                                          { 'Type' : 'RelInfoConeVariablesForEW', 'ConeAngle' : 1.0, 'Variables' : ['EWCONEANGLE','EWCONENPX','EWCONENPY','EWCONENPZ','EWCONENMULT'],
                                            'RecursionLevel' : 1,
                                            'Locations' : {
                                                   self.selKpipi : 'NConeIso10B', 
                                                   'Phys/StdAllNoPIDsPions'   :                                                     ["NConeIso10pi1", "NConeIso10pi2"],
                                                   'Phys/StdAllNoPIDsKaons'   : 'NConeIso10K'
                                               }                                                                           },
                                          { 'Type' : 'RelInfoConeVariablesForEW', 'ConeAngle' : 1.5, 'Variables' : ['EWCONEANGLE','EWCONENPX','EWCONENPY','EWCONENPZ','EWCONENMULT'],
                                            'RecursionLevel' : 1,
                                            'Locations' : {
                                                   self.selKpipi : 'NConeIso15B', 
                                                   'Phys/StdAllNoPIDsPions'   :                                                     ["NConeIso15pi1", "NConeIso15pi2"],
                                                   'Phys/StdAllNoPIDsKaons'   : 'NConeIso15K'
                                               }                                                                           },
                                     ],
)

        self.lineKKpi_excl = StrippingLine( _KKpi_exclName + 'Line',
                                     prescale   = config['KKpi_exclLinePrescale'],
                                     postscale  = config['KKpi_exclLinePostscale'],
                                     algos      = self.algosKKpi,
                                     FILTER = filter,
                                     RelatedInfoTools = [
                                          { 'Type' : 'RelInfoConeVariables', 'ConeAngle' : 0.5, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'],
                                            'RecursionLevel' : 1,
                                            'Locations' : {
                                                   self.selKKpi : 'ConeIso05B', 
                                                   'Phys/StdAllNoPIDsKaons'   :                                                     ["ConeIso05K1", "ConeIso05K2"],
                                                   'Phys/StdAllNoPIDsPions'   : 'ConeIso05pi'
                                               }                                                                           },
                                          { 'Type' : 'RelInfoConeVariables', 'ConeAngle' : 1.0, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'],
                                            'RecursionLevel' : 1,
                                            'Locations' : {
                                                   self.selKKpi : 'ConeIso10B', 
                                                   'Phys/StdAllNoPIDsKaons'   :                                                     ["ConeIso10K1", "ConeIso10K2"],
                                                   'Phys/StdAllNoPIDsPions'   : 'ConeIso10pi'
                                               }                                                                           },
                                          { 'Type' : 'RelInfoConeVariables', 'ConeAngle' : 1.5, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'],
                                            'RecursionLevel' : 1,
                                            'Locations' : {
                                                   self.selKKpi : 'ConeIso15B', 
                                                   'Phys/StdAllNoPIDsKaons'   :                                                     ["ConeIso15K1", "ConeIso15K2"],
                                                   'Phys/StdAllNoPIDsPions'   : 'ConeIso15pi'
                                               }                                                                           },
                                          { 'Type' : 'RelInfoConeVariablesForEW', 'ConeAngle' : 0.5, 'Variables' : ['EWCONEANGLE','EWCONENPX','EWCONENPY','EWCONENPZ','EWCONENMULT'],
                                            'RecursionLevel' : 1,
                                            'Locations' : {
                                                   self.selKKpi : 'NConeIso05B', 
                                                   'Phys/StdAllNoPIDsKaons'   :                                                     ["NConeIso05K1", "NConeIso05K2"],
                                                   'Phys/StdAllNoPIDsPions'   : 'NConeIso05pi'
                                               }                                                                           },
                                          { 'Type' : 'RelInfoConeVariablesForEW', 'ConeAngle' : 1.0, 'Variables' : ['EWCONEANGLE','EWCONENPX','EWCONENPY','EWCONENPZ','EWCONENMULT'],
                                            'RecursionLevel' : 1,
                                            'Locations' : {
                                                   self.selKKpi : 'NConeIso10B', 
                                                   'Phys/StdAllNoPIDsKaons'   :                                                     ["NConeIso10K1", "NConeIso10K2"],
                                                   'Phys/StdAllNoPIDsPions'   : 'NConeIso10pi'
                                               }                                                                           },
                                          { 'Type' : 'RelInfoConeVariablesForEW', 'ConeAngle' : 1.5, 'Variables' : ['EWCONEANGLE','EWCONENPX','EWCONENPY','EWCONENPZ','EWCONENMULT'],
                                            'RecursionLevel' : 1,
                                            'Locations' : {
                                                   self.selKKpi : 'NConeIso15B', 
                                                   'Phys/StdAllNoPIDsKaons'   :                                                     ["NConeIso15K1", "NConeIso15K2"],
                                                   'Phys/StdAllNoPIDsPions'   : 'NConeIso15pi'
                                               }                                                                           },
                                     ],
)

        self.lineKKK_excl = StrippingLine( _KKK_exclName + 'Line',
                                     prescale   = config['KKK_exclLinePrescale'],
                                     postscale  = config['KKK_exclLinePostscale'],
                                     algos      = self.algosKKK,
                                     FILTER = filter,
                                     RelatedInfoTools = [
                                          { 'Type' : 'RelInfoConeVariables', 'ConeAngle' : 0.5, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'],
                                            'RecursionLevel' : 1,
                                            'Locations' : {
                                                   self.selKKK : 'ConeIso05B', 
                                                   'Phys/StdAllNoPIDsKaons'   :                                                     ["ConeIso05K1", "ConeIso05K2", "ConeIso05K3"],
                                               }                                                                           },
                                          { 'Type' : 'RelInfoConeVariables', 'ConeAngle' : 1.0, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'],
                                            'RecursionLevel' : 1,
                                            'Locations' : {
                                                   self.selKKK : 'ConeIso10B', 
                                                   'Phys/StdAllNoPIDsKaons'   :                                                     ["ConeIso10K1", "ConeIso10K2", "ConeIso10K3"],
                                               }                                                                           },
                                          { 'Type' : 'RelInfoConeVariables', 'ConeAngle' : 1.5, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'],
                                            'RecursionLevel' : 1,
                                            'Locations' : {
                                                   self.selKKK : 'ConeIso15B', 
                                                   'Phys/StdAllNoPIDsKaons'   :                                                     ["ConeIso15K1", "ConeIso15K2", "ConeIso15K3"],
                                               }                                                                           },
                                          { 'Type' : 'RelInfoConeVariablesForEW', 'ConeAngle' : 0.5, 'Variables' : ['EWCONEANGLE','EWCONENPX','EWCONENPY','EWCONENPZ','EWCONENMULT'],
                                            'RecursionLevel' : 1,
                                            'Locations' : {
                                                   self.selKKK : 'NConeIso05B', 
                                                   'Phys/StdAllNoPIDsKaons'   :                                                     ["NConeIso05K1", "NConeIso05K2", "NConeIso05K3"],
                                               }                                                                           },
                                          { 'Type' : 'RelInfoConeVariablesForEW', 'ConeAngle' : 1.0, 'Variables' : ['EWCONEANGLE','EWCONENPX','EWCONENPY','EWCONENPZ','EWCONENMULT'],
                                            'RecursionLevel' : 1,
                                            'Locations' : {
                                                   self.selKKK : 'NConeIso10B', 
                                                   'Phys/StdAllNoPIDsKaons'   :                                                     ["NConeIso10K1", "NConeIso10K2", "NConeIso10K3"],
                                               }                                                                           },
                                          { 'Type' : 'RelInfoConeVariablesForEW', 'ConeAngle' : 1.5, 'Variables' : ['EWCONEANGLE','EWCONENPX','EWCONENPY','EWCONENPZ','EWCONENMULT'],
                                            'RecursionLevel' : 1,
                                            'Locations' : {
                                                   self.selKKK : 'NConeIso15B', 
                                                   'Phys/StdAllNoPIDsKaons'   :                                                     ["NConeIso15K1", "NConeIso15K2", "NConeIso15K3"],
                                               }                                                                           },
                                     ],
)

        self.linepppi_excl = StrippingLine( _pppi_exclName + 'Line',
                                     prescale   = config['pppi_exclLinePrescale'],
                                     postscale  = config['pppi_exclLinePostscale'],
                                     algos      = self.algospppi,
                                     FILTER = filter,
                                     RelatedInfoTools = [
                                          { 'Type' : 'RelInfoConeVariables', 'ConeAngle' : 0.5, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'],
                                            'RecursionLevel' : 1,
                                            'Locations' : {
                                                   self.selpppi : 'ConeIso05B', 
                                                   'Phys/StdAllLooseProtons'   :                                                     ["ConeIso05p1", "ConeIso05p2"],
                                                   'Phys/StdAllNoPIDsPions'   : 'ConeIso05pi'
                                               }                                                                           },
                                          { 'Type' : 'RelInfoConeVariables', 'ConeAngle' : 1.0, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'],
                                            'RecursionLevel' : 1,
                                            'Locations' : {
                                                   self.selpppi : 'ConeIso10B', 
                                                   'Phys/StdAllLooseProtons'   :                                                     ["ConeIso10p1", "ConeIso10p2"],
                                                   'Phys/StdAllNoPIDsPions'   : 'ConeIso10pi'
                                               }                                                                           },
                                          { 'Type' : 'RelInfoConeVariables', 'ConeAngle' : 1.5, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'],
                                            'RecursionLevel' : 1,
                                            'Locations' : {
                                                   self.selpppi : 'ConeIso15B', 
                                                   'Phys/StdAllLooseProtons'   :                                                     ["ConeIso15p1", "ConeIso15p2"],
                                                   'Phys/StdAllNoPIDsPions'   : 'ConeIso15pi'
                                               }                                                                           },
                                          { 'Type' : 'RelInfoConeVariablesForEW', 'ConeAngle' : 0.5, 'Variables' : ['EWCONEANGLE','EWCONENPX','EWCONENPY','EWCONENPZ','EWCONENMULT'],
                                            'RecursionLevel' : 1,
                                            'Locations' : {
                                                   self.selpppi : 'NConeIso05B', 
                                                   'Phys/StdAllLooseProtons'   :                                                     ["NConeIso05p1", "NConeIso05p2"],
                                                   'Phys/StdAllNoPIDsPions'   : 'NConeIso05pi'
                                               }                                                                           },
                                          { 'Type' : 'RelInfoConeVariablesForEW', 'ConeAngle' : 1.0, 'Variables' : ['EWCONEANGLE','EWCONENPX','EWCONENPY','EWCONENPZ','EWCONENMULT'],
                                            'RecursionLevel' : 1,
                                            'Locations' : {
                                                   self.selpppi : 'NConeIso10B', 
                                                   'Phys/StdAllLooseProtons'   :                                                     ["NConeIso10p1", "NConeIso10p2"],
                                                   'Phys/StdAllNoPIDsPions'   : 'NConeIso10pi'
                                               }                                                                           },
                                          { 'Type' : 'RelInfoConeVariablesForEW', 'ConeAngle' : 1.5, 'Variables' : ['EWCONEANGLE','EWCONENPX','EWCONENPY','EWCONENPZ','EWCONENMULT'],
                                            'RecursionLevel' : 1,
                                            'Locations' : {
                                                   self.selpppi : 'NConeIso15B', 
                                                   'Phys/StdAllLooseProtons'   :                                                     ["NConeIso15p1", "NConeIso15p2"],
                                                   'Phys/StdAllNoPIDsPions'   : 'NConeIso15pi'
                                               }                                                                           },

                                     ],
)
        
        self.lineppK_excl = StrippingLine( _ppK_exclName + 'Line',
                                     prescale   = config['ppK_exclLinePrescale'],
                                     postscale  = config['ppK_exclLinePostscale'],
                                     algos      = self.algosppK,
                                     FILTER = filter,
                                     RelatedInfoTools = [
                                          { 'Type' : 'RelInfoConeVariables', 'ConeAngle' : 0.5, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'],
                                            'RecursionLevel' : 1,
                                            'Locations' : {
                                                   self.selppK : 'ConeIso05B', 
                                                   'Phys/StdAllLooseProtons'   :                                                     ["ConeIso05p1", "ConeIso05p2"],
                                                   'Phys/StdAllNoPIDsKaons'   : 'ConeIso05K'
                                               }                                                                           },
                                          { 'Type' : 'RelInfoConeVariables', 'ConeAngle' : 1.0, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'],
                                            'RecursionLevel' : 1,
                                            'Locations' : {
                                                   self.selppK : 'ConeIso10B', 
                                                   'Phys/StdAllLooseProtons'   :                                                     ["ConeIso10p1", "ConeIso10p2"],
                                                   'Phys/StdAllNoPIDsKaons'   : 'ConeIso10K'
                                               }                                                                           },
                                          { 'Type' : 'RelInfoConeVariables', 'ConeAngle' : 1.5, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'],
                                            'RecursionLevel' : 1,
                                            'Locations' : {
                                                   self.selppK : 'ConeIso15B', 
                                                   'Phys/StdAllLooseProtons'   :                                                     ["ConeIso15p1", "ConeIso15p2"],
                                                   'Phys/StdAllNoPIDsKaons'   : 'ConeIso15K'
                                               }                                                                           },
                                          { 'Type' : 'RelInfoConeVariablesForEW', 'ConeAngle' : 0.5, 'Variables' : ['EWCONEANGLE','EWCONENPX','EWCONENPY','EWCONENPZ','EWCONENMULT'],
                                            'RecursionLevel' : 1,
                                            'Locations' : {
                                                   self.selppK : 'NConeIso05B', 
                                                   'Phys/StdAllLooseProtons'   :                                                     ["NConeIso05p1", "NConeIso05p2"],
                                                   'Phys/StdAllNoPIDsKaons'   : 'NConeIso05K'
                                               }                                                                           },
                                          { 'Type' : 'RelInfoConeVariablesForEW', 'ConeAngle' : 1.0, 'Variables' : ['EWCONEANGLE','EWCONENPX','EWCONENPY','EWCONENPZ','EWCONENMULT'],
                                            'RecursionLevel' : 1,
                                            'Locations' : {
                                                   self.selppK : 'NConeIso10B', 
                                                   'Phys/StdAllLooseProtons'   :                                                     ["NConeIso10p1", "NConeIso10p2"],
                                                   'Phys/StdAllNoPIDsKaons'   : 'NConeIso10K'
                                               }                                                                           },
                                          { 'Type' : 'RelInfoConeVariablesForEW', 'ConeAngle' : 1.5, 'Variables' : ['EWCONEANGLE','EWCONENPX','EWCONENPY','EWCONENPZ','EWCONENMULT'],
                                            'RecursionLevel' : 1,
                                            'Locations' : {
                                                   self.selppK : 'NConeIso15B', 
                                                   'Phys/StdAllLooseProtons'   :                                                     ["NConeIso15p1", "NConeIso15p2"],
                                                   'Phys/StdAllNoPIDsKaons'   : 'NConeIso15K'
                                               }                                                                           },
                                     ],
)

        self.registerLine(self.linepipipi_excl)
        self.registerLine(self.lineKpipi_excl)
        self.registerLine(self.lineKKpi_excl)
        self.registerLine(self.lineKKK_excl)
        self.registerLine(self.linepppi_excl)
        self.registerLine(self.lineppK_excl)

def makepipipi_excl(name,
                    _h_PT,
                    _h_P,
                    _h_IPCHI2,
                    _h_TRCHI2DOF,
                    _h_TRGHP,      
                    _3h_DOCA,
                    _3h_PTmax,
                    _3h_DIRA,
                    _3h_FDCHI2,
                    _3h_SVPV,
                    _3h_CHI2,
                    _3h_IPCHI2,
                    _3h_PT,
                    _3h_PTsum,
                    _3h_Psum,
                    _3h_PVIPCHI2sum,
                    _3h_TRKCHIDOFmin,
                    _3h_Mmin,
                    _3h_Mmax,
                    _bu3h_Mmin,
                    _bu3h_Mmax) :

    _daughtersCuts = {"pi+" : "(PT > %(_h_PT)s*MeV) \
                             & (P > %(_h_P)s*MeV) \
                             & (MIPCHI2DV(PRIMARY) > %(_h_IPCHI2)s) \
                             & (TRCHI2DOF < %(_h_TRCHI2DOF)s) \
                             & (TRGHOSTPROB < %(_h_TRGHP)s)" % locals()}
    _combinationCut = "(((AM < %(_3h_Mmax)s*MeV) & (AM > %(_3h_Mmin)s*MeV)) | ((AM < %(_bu3h_Mmax)s*MeV) & (AM > %(_bu3h_Mmin)s*MeV))) \
		     & (AMAXDOCA('LoKi::TrgDistanceCalculator') < %(_3h_DOCA)s)" % locals()
    _motherCut = "(MAXTREE(((ABSID=='pi+') | (ABSID=='pi-')),PT) > %(_3h_PTmax)s*MeV) \
                & (BPVDIRA > %(_3h_DIRA)s) \
		& (BPVVDCHI2 > %(_3h_FDCHI2)s) \
		& (VFASPF(VMINVDDV(PRIMARY)) > %(_3h_SVPV)s) \
		& (VFASPF(VCHI2) < %(_3h_CHI2)s) \
		& (MIPCHI2DV(PRIMARY) < %(_3h_IPCHI2)s) \
		& (PT > %(_3h_PT)s*MeV) \
		& (SUMTREE(PT,(('pi+'==ABSID) | ('pi-'==ABSID)),0.0) > %(_3h_PTsum)s*MeV) \
		& (SUMTREE(P,(('pi+'==ABSID) | ('pi-'==ABSID)),0.0) > %(_3h_Psum)s*MeV) \
		& (SUMTREE(MIPCHI2DV(PRIMARY),(('pi+'==ABSID) | ('pi-'==ABSID)),0.0) > %(_3h_PVIPCHI2sum)s) \
		& (MINTREE((('pi+'==ABSID) | ('pi-'==ABSID)),TRCHI2DOF) < %(_3h_TRKCHIDOFmin)s)" % locals()
   
    _pipipi=CombineParticles()
    _pipipi.DecayDescriptors  = ["[B_c+ -> pi+ pi+ pi-]cc"]
    _pipipi.MotherCut         = _motherCut
    _pipipi.CombinationCut    = _combinationCut
    _pipipi.DaughtersCuts     = _daughtersCuts

    return Selection ( name,
                       Algorithm = _pipipi,
                       RequiredSelections = [StdNoPIDsPions])

def makeKpipi_excl(name,
                   _h_PT,
                   _h_P,
                   _h_IPCHI2,
                   _h_TRCHI2DOF,
                   _h_TRGHP,      
                   _3h_DOCA,
                   _3h_PTmax,
                   _3h_DIRA,
                   _3h_FDCHI2,
                   _3h_SVPV,
                   _3h_CHI2,
                   _3h_IPCHI2,
                   _3h_PT,
                   _3h_PTsum,
                   _3h_Psum,
                   _3h_PVIPCHI2sum,
                   _3h_TRKCHIDOFmin,
                   _3h_Mmin,
                   _3h_Mmax,
                   _bu3h_Mmin,
                   _bu3h_Mmax,
                   _probnnk) :

    _daughtersCuts = {"K+" : "(PT > %(_h_PT)s*MeV) \
                             & (P > %(_h_P)s*MeV) \
                             & (MIPCHI2DV(PRIMARY) > %(_h_IPCHI2)s) \
                             & (TRCHI2DOF < %(_h_TRCHI2DOF)s) \
                             & (PROBNNk > %(_probnnk)s) \
                             & (TRGHOSTPROB < %(_h_TRGHP)s)" % locals(),
                      "pi+": "(PT > %(_h_PT)s*MeV) \
                             & (P > %(_h_P)s*MeV) \
                             & (MIPCHI2DV(PRIMARY) > %(_h_IPCHI2)s) \
                             & (TRCHI2DOF < %(_h_TRCHI2DOF)s) \
                             & (TRGHOSTPROB < %(_h_TRGHP)s)" % locals()}
    _combinationCut = "(((AM < %(_3h_Mmax)s*MeV) & (AM > %(_3h_Mmin)s*MeV)) | ((AM < %(_bu3h_Mmax)s*MeV) & (AM > %(_bu3h_Mmin)s*MeV))) \
		     & (AMAXDOCA('LoKi::TrgDistanceCalculator') < %(_3h_DOCA)s)" % locals()
    _motherCut = "(MAXTREE(((ABSID=='K+') | (ABSID=='K-') | ('pi+'==ABSID) | ('pi-'==ABSID)),PT) > %(_3h_PTmax)s*MeV) \
                & (BPVDIRA > %(_3h_DIRA)s) \
		& (BPVVDCHI2 > %(_3h_FDCHI2)s) \
		& (VFASPF(VMINVDDV(PRIMARY)) > %(_3h_SVPV)s) \
		& (VFASPF(VCHI2) < %(_3h_CHI2)s) \
		& (MIPCHI2DV(PRIMARY) < %(_3h_IPCHI2)s) \
		& (PT > %(_3h_PT)s*MeV) \
		& (SUMTREE(PT,((ABSID=='K+') | (ABSID=='K-') | ('pi+'==ABSID) | ('pi-'==ABSID)),0.0) > %(_3h_PTsum)s*MeV) \
		& (SUMTREE(P,((ABSID=='K+') | (ABSID=='K-') | ('pi+'==ABSID) | ('pi-'==ABSID)),0.0) > %(_3h_Psum)s*MeV) \
		& (SUMTREE(MIPCHI2DV(PRIMARY),((ABSID=='K+') | (ABSID=='K-') | ('pi+'==ABSID) | ('pi-'==ABSID)),0.0) > %(_3h_PVIPCHI2sum)s) \
		& (MINTREE((('K+'==ABSID) | ('K-'==ABSID) | ('pi+'==ABSID) | ('pi-'==ABSID)),TRCHI2DOF) < %(_3h_TRKCHIDOFmin)s)" % locals()
   
    _Kpipi=CombineParticles()
    _Kpipi.DecayDescriptors  = ["[B_c+ -> pi+ K+ pi-]cc"]
    _Kpipi.MotherCut         = _motherCut
    _Kpipi.CombinationCut    = _combinationCut
    _Kpipi.DaughtersCuts     = _daughtersCuts

    return Selection ( name,
                       Algorithm = _Kpipi,
                       RequiredSelections = [StdNoPIDsPions, StdNoPIDsKaons])

def makeKKpi_excl(name,
                  _h_PT,
                  _h_P,
                  _h_IPCHI2,
                  _h_TRCHI2DOF,
                  _h_TRGHP,      
                  _3h_DOCA,
                  _3h_PTmax,
                  _3h_DIRA,
                  _3h_FDCHI2,
                  _3h_SVPV,
                  _3h_CHI2,
                  _3h_IPCHI2,
                  _3h_PT,
                  _3h_PTsum,
                  _3h_Psum,
                  _3h_PVIPCHI2sum,
                  _3h_TRKCHIDOFmin,
                  _3h_Mmin,
                  _3h_Mmax,
                  _bu3h_Mmin,
                  _bu3h_Mmax,
                  _probnnpi,
                  _probnnk) :

    _daughtersCuts = {"K+" : "(PT > %(_h_PT)s*MeV) \
                             & (P > %(_h_P)s*MeV) \
                             & (MIPCHI2DV(PRIMARY) > %(_h_IPCHI2)s) \
                             & (TRCHI2DOF < %(_h_TRCHI2DOF)s) \
                             & (PROBNNk > %(_probnnk)s) \
                             & (TRGHOSTPROB < %(_h_TRGHP)s)" % locals(),
                      "pi+" : "(PT > %(_h_PT)s*MeV) \
                             & (P > %(_h_P)s*MeV) \
                             & (MIPCHI2DV(PRIMARY) > %(_h_IPCHI2)s) \
                             & (TRCHI2DOF < %(_h_TRCHI2DOF)s) \
                             & (PROBNNpi > %(_probnnpi)s) \
                             & (TRGHOSTPROB < %(_h_TRGHP)s)" % locals()}
    _combinationCut = "(((AM < %(_3h_Mmax)s*MeV) & (AM > %(_3h_Mmin)s*MeV)) | ((AM < %(_bu3h_Mmax)s*MeV) & (AM > %(_bu3h_Mmin)s*MeV))) \
		     & (AMAXDOCA('LoKi::TrgDistanceCalculator') < %(_3h_DOCA)s)" % locals()
    _motherCut = "(MAXTREE(((ABSID=='K+') | (ABSID=='K-') | ('pi+'==ABSID) | ('pi-'==ABSID)),PT) > %(_3h_PTmax)s*MeV) \
                & (BPVDIRA > %(_3h_DIRA)s) \
		& (BPVVDCHI2 > %(_3h_FDCHI2)s) \
		& (VFASPF(VMINVDDV(PRIMARY)) > %(_3h_SVPV)s) \
		& (VFASPF(VCHI2) < %(_3h_CHI2)s) \
		& (MIPCHI2DV(PRIMARY) < %(_3h_IPCHI2)s) \
		& (PT > %(_3h_PT)s*MeV) \
		& (SUMTREE(PT,((ABSID=='K+') | (ABSID=='K-') | ('pi+'==ABSID) | ('pi-'==ABSID)),0.0) > %(_3h_PTsum)s*MeV) \
		& (SUMTREE(P,((ABSID=='K+') | (ABSID=='K-') | ('pi+'==ABSID) | ('pi-'==ABSID)),0.0) > %(_3h_Psum)s*MeV) \
		& (SUMTREE(MIPCHI2DV(PRIMARY),((ABSID=='K+') | (ABSID=='K-') | ('pi+'==ABSID) | ('pi-'==ABSID)),0.0) > %(_3h_PVIPCHI2sum)s) \
		& (MINTREE((('K+'==ABSID) | ('K-'==ABSID) | ('pi+'==ABSID) | ('pi-'==ABSID)),TRCHI2DOF) < %(_3h_TRKCHIDOFmin)s)" % locals()
   
    _KKpi=CombineParticles()
    _KKpi.DecayDescriptors  = ["[B_c+ -> K+ pi+ K-]cc"]
    _KKpi.MotherCut         = _motherCut
    _KKpi.CombinationCut    = _combinationCut
    _KKpi.DaughtersCuts     = _daughtersCuts

    return Selection ( name,
                       Algorithm = _KKpi,
                       RequiredSelections = [StdNoPIDsPions, StdNoPIDsKaons])

def makeKKK_excl(name,
                 _h_PT,
                 _h_P,
                 _h_IPCHI2,
                 _h_TRCHI2DOF,
                 _h_TRGHP,      
                 _3h_DOCA,
                 _3h_PTmax,
                 _3h_DIRA,
                 _3h_FDCHI2,
                 _3h_SVPV,
                 _3h_CHI2,
                 _3h_IPCHI2,
                 _3h_PT,
                 _3h_PTsum,
                 _3h_Psum,
                 _3h_PVIPCHI2sum,
                 _3h_TRKCHIDOFmin,
                 _3h_Mmin,
                 _3h_Mmax,
                 _bu3h_Mmin,
                 _bu3h_Mmax,
                 _probnnk) :

    _daughtersCuts = {"K+" : "(PT > %(_h_PT)s*MeV) \
                             & (P > %(_h_P)s*MeV) \
                             & (MIPCHI2DV(PRIMARY) > %(_h_IPCHI2)s) \
                             & (TRCHI2DOF < %(_h_TRCHI2DOF)s) \
                             & (PROBNNk > %(_probnnk)s) \
                             & (TRGHOSTPROB < %(_h_TRGHP)s)" % locals()}
    _combinationCut = "(((AM < %(_3h_Mmax)s*MeV) & (AM > %(_3h_Mmin)s*MeV)) | ((AM < %(_bu3h_Mmax)s*MeV) & (AM > %(_bu3h_Mmin)s*MeV))) \
		     & (AMAXDOCA('LoKi::TrgDistanceCalculator') < %(_3h_DOCA)s)" % locals()
    _motherCut = "(MAXTREE(((ABSID=='K+') | (ABSID=='K-')),PT) > %(_3h_PTmax)s*MeV) \
                & (BPVDIRA > %(_3h_DIRA)s) \
		& (BPVVDCHI2 > %(_3h_FDCHI2)s) \
		& (VFASPF(VMINVDDV(PRIMARY)) > %(_3h_SVPV)s) \
		& (VFASPF(VCHI2) < %(_3h_CHI2)s) \
		& (MIPCHI2DV(PRIMARY) < %(_3h_IPCHI2)s) \
		& (PT > %(_3h_PT)s*MeV) \
		& (SUMTREE(PT,((ABSID=='K+') | (ABSID=='K-')),0.0) > %(_3h_PTsum)s*MeV) \
		& (SUMTREE(P,((ABSID=='K+') | (ABSID=='K-')),0.0) > %(_3h_Psum)s*MeV) \
		& (SUMTREE(MIPCHI2DV(PRIMARY),((ABSID=='K+') | (ABSID=='K-')),0.0) > %(_3h_PVIPCHI2sum)s) \
		& (MINTREE((('K+'==ABSID) | ('K-'==ABSID)),TRCHI2DOF) < %(_3h_TRKCHIDOFmin)s)" % locals()
   
    _KKK=CombineParticles()
    _KKK.DecayDescriptors  = ["[B_c+ -> K+ K+ K-]cc"]
    _KKK.MotherCut         = _motherCut
    _KKK.CombinationCut    = _combinationCut
    _KKK.DaughtersCuts     = _daughtersCuts

    return Selection ( name,
                       Algorithm = _KKK,
                       RequiredSelections = [StdNoPIDsKaons])

def makepppi_excl(name,
                  _h_PT,
                  _h_P,
                  _h_IPCHI2,
                  _h_TRCHI2DOF,
                  _h_TRGHP,            
                  _3h_DOCA,
                  _3h_PTmax,
                  _3h_DIRA,
                  _3h_FDCHI2,
                  _3h_SVPV,
                  _3h_CHI2,
                  _3h_IPCHI2,
                  _3h_PT,
                  _3h_PTsum,
                  _3h_Psum,
                  _3h_PVIPCHI2sum,
                  _3h_TRKCHIDOFmin,
                  _3h_Mmin,
                  _3h_Mmax,
                  _bu3h_Mmin,
                  _bu3h_Mmax,
                  _probnnpi,
                  _probnnp) :

    _daughtersCuts = {"p+" : "(PT > %(_h_PT)s*MeV) \
                             & (P > %(_h_P)s*MeV) \
                             & (MIPCHI2DV(PRIMARY) > %(_h_IPCHI2)s) \
                             & (TRCHI2DOF < %(_h_TRCHI2DOF)s) \
                             & (PROBNNp > %(_probnnp)s) \
                             & (TRGHOSTPROB < %(_h_TRGHP)s)" % locals(),
		      "pi+" :  "(PT > %(_h_PT)s*MeV) \
                             & (P > %(_h_P)s*MeV) \
                             & (MIPCHI2DV(PRIMARY) > %(_h_IPCHI2)s) \
                             & (TRCHI2DOF < %(_h_TRCHI2DOF)s) \
                             & (PROBNNpi > %(_probnnpi)s) \
                             & (TRGHOSTPROB < %(_h_TRGHP)s)" % locals()}
    _combinationCut = "(((AM < %(_3h_Mmax)s*MeV) & (AM > %(_3h_Mmin)s*MeV)) | ((AM < %(_bu3h_Mmax)s*MeV) & (AM > %(_bu3h_Mmin)s*MeV))) \
		     & (AMAXDOCA('LoKi::TrgDistanceCalculator') < %(_3h_DOCA)s)" % locals()
    _motherCut = "(MAXTREE(((ABSID=='p+') |(ABSID=='p~-') |(ABSID=='pi+') | (ABSID=='pi-')),PT) > %(_3h_PTmax)s*MeV) \
                & (BPVDIRA > %(_3h_DIRA)s) \
		& (BPVVDCHI2 > %(_3h_FDCHI2)s) \
		& (VFASPF(VMINVDDV(PRIMARY)) > %(_3h_SVPV)s) \
		& (VFASPF(VCHI2) < %(_3h_CHI2)s) \
		& (MIPCHI2DV(PRIMARY) < %(_3h_IPCHI2)s) \
		& (PT > %(_3h_PT)s*MeV) \
		& (SUMTREE(PT,((ABSID=='p+') |(ABSID=='p~-') |(ABSID=='pi+') | (ABSID=='pi-')),0.0) > %(_3h_PTsum)s*MeV) \
		& (SUMTREE(P,((ABSID=='p+') |(ABSID=='p~-') |(ABSID=='pi+') | (ABSID=='pi-')),0.0) > %(_3h_Psum)s*MeV) \
		& (SUMTREE(MIPCHI2DV(PRIMARY),((ABSID=='p+') |(ABSID=='p~-') |(ABSID=='pi+') | (ABSID=='pi-')),0.0) > %(_3h_PVIPCHI2sum)s) \
		& (MINTREE(((ABSID=='p+') |(ABSID=='p~-') |('pi+'==ABSID) | ('pi-'==ABSID)),TRCHI2DOF) < %(_3h_TRKCHIDOFmin)s)" % locals()
   
    _pppi=CombineParticles()
    _pppi.DecayDescriptors  = ["[B_c+ -> p+ p~- pi+]cc"]
    _pppi.MotherCut         = _motherCut
    _pppi.CombinationCut    = _combinationCut
    _pppi.DaughtersCuts     = _daughtersCuts

    return Selection ( name,
                       Algorithm = _pppi,
                       RequiredSelections = [StdNoPIDsPions, StdLooseProtons])

def makeppK_excl(name,
                 _h_PT,
                 _h_P,
                 _h_IPCHI2,
                 _h_TRCHI2DOF,
                 _h_TRGHP,            
                 _3h_DOCA,
                 _3h_PTmax,
                 _3h_DIRA,
                 _3h_FDCHI2,
                 _3h_SVPV,
                 _3h_CHI2,
                 _3h_IPCHI2,
                 _3h_PT,
                 _3h_PTsum,
                 _3h_Psum,
                 _3h_PVIPCHI2sum,
                 _3h_TRKCHIDOFmin,
                 _3h_Mmin,
                 _3h_Mmax,
                 _bu3h_Mmin,
                 _bu3h_Mmax,
                 _probnnk,
                 _probnnp) :

    _daughtersCuts = {"p+" : "(PT > %(_h_PT)s*MeV) \
                             & (P > %(_h_P)s*MeV) \
                             & (MIPCHI2DV(PRIMARY) > %(_h_IPCHI2)s) \
                             & (TRCHI2DOF < %(_h_TRCHI2DOF)s) \
                             & (PROBNNp > %(_probnnp)s) \
                             & (TRGHOSTPROB < %(_h_TRGHP)s)" % locals(),
		      "K+" :  "(PT > %(_h_PT)s*MeV) \
                             & (P > %(_h_P)s*MeV) \
                             & (MIPCHI2DV(PRIMARY) > %(_h_IPCHI2)s) \
                             & (TRCHI2DOF < %(_h_TRCHI2DOF)s) \
                             & (PROBNNk > %(_probnnk)s) \
                             & (TRGHOSTPROB < %(_h_TRGHP)s)" % locals()}
    _combinationCut = "(((AM < %(_3h_Mmax)s*MeV) & (AM > %(_3h_Mmin)s*MeV)) | ((AM < %(_bu3h_Mmax)s*MeV) & (AM > %(_bu3h_Mmin)s*MeV))) \
		     & (AMAXDOCA('LoKi::TrgDistanceCalculator') < %(_3h_DOCA)s)" % locals()
    _motherCut = "(MAXTREE(((ABSID=='p+') |(ABSID=='p~-') |(ABSID=='K+') | (ABSID=='K-')),PT) > %(_3h_PTmax)s*MeV) \
                & (BPVDIRA > %(_3h_DIRA)s) \
		& (BPVVDCHI2 > %(_3h_FDCHI2)s) \
		& (VFASPF(VMINVDDV(PRIMARY)) > %(_3h_SVPV)s) \
		& (VFASPF(VCHI2) < %(_3h_CHI2)s) \
		& (MIPCHI2DV(PRIMARY) < %(_3h_IPCHI2)s) \
		& (PT > %(_3h_PT)s*MeV) \
		& (SUMTREE(PT,((ABSID=='p+') |(ABSID=='p~-') |(ABSID=='K+') | (ABSID=='K-')),0.0) > %(_3h_PTsum)s*MeV) \
		& (SUMTREE(P,((ABSID=='p+') |(ABSID=='p~-') |(ABSID=='K+') | (ABSID=='K-')),0.0) > %(_3h_Psum)s*MeV) \
		& (SUMTREE(MIPCHI2DV(PRIMARY),((ABSID=='p+') |(ABSID=='p~-') |(ABSID=='K+') | (ABSID=='K-')),0.0) > %(_3h_PVIPCHI2sum)s) \
		& (MINTREE(((ABSID=='p+') |(ABSID=='p~-') |('K+'==ABSID) | ('K-'==ABSID)),TRCHI2DOF) < %(_3h_TRKCHIDOFmin)s)" % locals()
   
    _ppK=CombineParticles()
    _ppK.DecayDescriptors  = ["[B_c+ -> p+ p~- K+]cc"]
    _ppK.MotherCut         = _motherCut
    _ppK.CombinationCut    = _combinationCut
    _ppK.DaughtersCuts     = _daughtersCuts

    return Selection ( name,
                       Algorithm = _ppK,
                       RequiredSelections = [StdNoPIDsKaons, StdLooseProtons])


