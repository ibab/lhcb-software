'''
Inclusive DiElectron lines, including:
1. DiElectronLine
   FullDST and MicroDST (loosend selections)
2. DiElectronLowMassLine
3. BiasedDiElectronLine
   FullDST and MicroDST (loosend selections)
4. BiasedDiElectronLowMassLine
5. Jpsi2eeLine

#--------------------------
# To use this for Dielectron stream
#--------------------------
from StrippingSelections.StrippingDiElectronNew import DiElectronConf
from StrippingSelections.StrippingDiElectronNew import config_default as config_FullDSTDiElectron
FullDSTDiElectronConf = DiElectronConf( name=None, config = config_FullDSTDiElectron )
stream.appendLines( FullDSTDiElectronConf.lines() )

#--------------------------
# For Leptonic MicroDST stream 
#--------------------------
from StrippingSelections.StrippingDiElectronNew import DiElectronConf
from StrippingSelections.StrippingDiElectronNew import config_microDST as config_MicroDSTDiElectron
MicroDSTDiElectronConf = DiElectronConf( name = 'MicroDST', config = config_MicroDSTDiElectron )
stream.appendLines( MicroDSTDiElectronConf.lines() )
'''

__author__=['Jibo He']
__date__ = '03/10/2010'
__version__= '$Revision: 1.0 $'

__all__ = (
    'DiElectronConf'
    )

config_default= {
        'MicroDST'                                      :  False  ,
        
        # DiElectron line
        'DiElectron_Prescale'                           :     1.  ,
        'DiElectron_Postscale'                          :     1.  ,
        'DiElectron_checkPV'                            : False   , 
        
        'DiElectron_ElectronPT'                         :  1200.  ,  # MeV
        'DiElectron_ElectronP'                          : -8000.  ,  # MeV, no cut now
        'DiElectron_ElectronPIDe'                       :     5.  ,  
        'DiElectron_ElectronTRCHI2DOF'                  :     5.  , 
        'DiElectron_MinMass'                            :  2000.  ,  # MeV
        'DiElectron_VCHI2PDOF'                          :    16.  , 
        'DiElectron_PT'                                 :  2000.  ,  # MeV
        'DiElectron_TisTosSpecs'                        : { "Hlt1Global%TIS" : 0, "Hlt2Global%TIS" : 0 },

        # DiElectronLowMass line
        'DiElectronLowMass_Prescale'                    :     0.04,
        'DiElectronLowMass_Postscale'                   :     1.  ,
        'DiElectronLowMass_checkPV'                     : False   ,   
        
        'DiElectronLowMass_ElectronPT'                  :  1200.  ,  # MeV
        'DiElectronLowMass_ElectronP'                   : -8000.  ,  # MeV, no cut now
        'DiElectronLowMass_ElectronPIDe'                :     5.  , 
        'DiElectronLowMass_ElectronTRCHI2DOF'           :     5.  , 
        'DiElectronLowMass_MinMass'                     :    30.  ,  # MeV
        'DiElectronLowMass_VCHI2PDOF'                   :    16.  ,       
        'DiElectronLowMass_PT'                          : -1000.  ,  # MeV, no cut now
        
        # BiasedDiElectron line
        'BiasedDiElectron_Prescale'                     :     1.  ,
        'BiasedDiElectron_Postscale'                    :     1.  ,
        
        'BiasedDiElectron_ElectronPT'                   :   500.  ,  # MeV
        'BiasedDiElectron_ElectronP'                    : -8000.  ,  # MeV, no cut now
        'BiasedDiElectron_ElectronPIDe'                 :     5.  ,
        'BiasedDiElectron_ElectronMIPCHI2'              :    16.  , 
        'BiasedDiElectron_ElectronTRCHI2DOF'            :     5.  , 
        'BiasedDiElectron_MinMass'                      :  2000.  ,  # MeV
        'BiasedDiElectron_VCHI2PDOF'                    :    16.  ,
        'BiasedDiElectron_PT'                           : -1000.  ,  # MeV, no cut now 

        # BiasedDiElectronLowMass line
        'BiasedDiElectronLowMass_Prescale'              :     0.01 ,
        'BiasedDiElectronLowMass_Postscale'             :     1.  ,
        
        'BiasedDiElectronLowMass_ElectronPT'            :   500.  ,  # MeV
        'BiasedDiElectronLowMass_ElectronP'             : -8000.  ,  # MeV
        'BiasedDiElectronLowMass_ElectronPIDe'          :     0.  ,
        'BiasedDiElectronLowMass_ElectronMIPCHI2'       :    16.  ,  
        'BiasedDiElectronLowMass_ElectronTRCHI2DOF'     :     9.  , 
        'BiasedDiElectronLowMass_MinMass'               :    30.  ,  # MeV
        'BiasedDiElectronLowMass_VCHI2PDOF'             :    16.  ,
        'BiasedDiElectronLowMass_PT'                    : -1000.  ,  # MeV, no cut now
        
        # Jpsi2ee line
        'Jpsi2ee_Prescale'                              :     1.  ,
        'Jpsi2ee_Postscale'                             :     1.  ,
        'Jpsi2ee_checkPV'                               : False   ,   
        
        'Jpsi2ee_ElectronPT'                            :  1200.  ,  # MeV
        'Jpsi2ee_ElectronP'                             : -8000.  ,  # MeV, no cut now
        'Jpsi2ee_ElectronPIDe'                          :     5.  ,
        'Jpsi2ee_ElectronTRCHI2DOF'                     :     5.  , 
        'Jpsi2ee_MinMass'                               :  2000.  ,  # MeV
        'Jpsi2ee_MaxMass'                               :  3800.  ,  # MeV
        'Jpsi2ee_VCHI2PDOF'                             :    16.  ,
        'Jpsi2ee_PT'                                    :  2000.  ,  # MeV
        'Jpsi2ee_TisTosSpecs'                           : { "Hlt1Global%TIS" : 0, "Hlt2Global%TIS" : 0 }
        }


config_microDST= {
        'MicroDST'                                      :  True   ,
        
        # DiElectron line
        'DiElectron_Prescale'                           :     1.  ,
        'DiElectron_Postscale'                          :     1.  ,        
        'DiElectron_checkPV'                            :  True   ,
        
        'DiElectron_ElectronPT'                         :  1100.  ,  # MeV
        'DiElectron_ElectronP'                          : -8000.  ,  # MeV, no cut now
        'DiElectron_ElectronPIDe'                       :     2.  ,  
        'DiElectron_ElectronTRCHI2DOF'                  :     5.  , 
        'DiElectron_MinMass'                            :  2000.  ,  # MeV
        'DiElectron_VCHI2PDOF'                          :    16.  , 
        'DiElectron_PT'                                 : -1000.  ,  # MeV, no cut now 
        'DiElectron_TisTosSpecs'                        : { "Hlt1Global%TIS" : 0, "Hlt2Global%TIS" : 0 },

        # DiElectronLowMass line
        'DiElectronLowMass_Prescale'                    :     1.  ,
        'DiElectronLowMass_Postscale'                   :     1.  ,
        'DiElectronLowMass_checkPV'                     :  True   , 
        
        'DiElectronLowMass_ElectronPT'                  :   750.  ,  # MeV
        'DiElectronLowMass_ElectronP'                   : -8000.  ,  # MeV, no cut now
        'DiElectronLowMass_ElectronPIDe'                :     2.  , 
        'DiElectronLowMass_ElectronTRCHI2DOF'           :     5.  , 
        'DiElectronLowMass_MinMass'                     :    30.  ,  # MeV
        'DiElectronLowMass_VCHI2PDOF'                   :    16.  ,       
        'DiElectronLowMass_PT'                          : -1000.  ,  # MeV, no cut now
        
        # BiasedDiElectron line
        'BiasedDiElectron_Prescale'                     :     1.  ,
        'BiasedDiElectron_Postscale'                    :     1.  ,
        
        'BiasedDiElectron_ElectronPT'                   :   500.  ,  # MeV
        'BiasedDiElectron_ElectronP'                    : -8000.  ,  # MeV, no cut now
        'BiasedDiElectron_ElectronPIDe'                 :     2.  ,
        'BiasedDiElectron_ElectronMIPCHI2'              :     4.  , 
        'BiasedDiElectron_ElectronTRCHI2DOF'            :     5.  , 
        'BiasedDiElectron_MinMass'                      :  2000.  ,  # MeV
        'BiasedDiElectron_VCHI2PDOF'                    :    16.  ,
        'BiasedDiElectron_PT'                           : -1000.  ,  # MeV, no cut now 

        # BiasedDiElectronLowMass line
        'BiasedDiElectronLowMass_Prescale'              :     0.1  ,
        'BiasedDiElectronLowMass_Postscale'             :     1.  ,

        'BiasedDiElectronLowMass_ElectronPT'            :   500.  ,  # MeV
        'BiasedDiElectronLowMass_ElectronP'             : -8000.  ,  # MeV
        'BiasedDiElectronLowMass_ElectronPIDe'          :     0.  ,
        'BiasedDiElectronLowMass_ElectronMIPCHI2'       :     4.  ,  
        'BiasedDiElectronLowMass_ElectronTRCHI2DOF'     :     5.  , 
        'BiasedDiElectronLowMass_MinMass'               :    30.  ,  # MeV
        'BiasedDiElectronLowMass_VCHI2PDOF'             :    16.  ,
        'BiasedDiElectronLowMass_PT'                    :  -1000. ,  # MeV, no cut now
        
        # Jpsi2ee line
        'Jpsi2ee_Prescale'                              :     1.  ,
        'Jpsi2ee_Postscale'                             :     1.  ,
        'Jpsi2ee_checkPV'                               :  True   , 
        
        'Jpsi2ee_ElectronPT'                            :  1200.  ,  # MeV
        'Jpsi2ee_ElectronP'                             : -8000.  ,  # MeV, no cut now
        'Jpsi2ee_ElectronPIDe'                          :     4.  ,
        'Jpsi2ee_ElectronTRCHI2DOF'                     :     5.  , 
        'Jpsi2ee_MinMass'                               :  2000.  ,  # MeV
        'Jpsi2ee_MaxMass'                               :  3800.  ,  # MeV
        'Jpsi2ee_VCHI2PDOF'                             :    16.  ,
        'Jpsi2ee_PT'                                    : -1500.0 ,  # MeV
        'Jpsi2ee_TisTosSpecs'                           : { "Hlt1Global%TIS" : 0, "Hlt2Global%TIS" : 0 }
        }
    


from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder

class DiElectronConf(LineBuilder):
    
    __configuration_keys__ = (
        'MicroDST',
        
        # DiElectron line
        'DiElectron_Prescale',
        'DiElectron_Postscale',
        'DiElectron_checkPV',
        
        'DiElectron_ElectronPT',
        'DiElectron_ElectronP',
        'DiElectron_ElectronPIDe',        
        'DiElectron_ElectronTRCHI2DOF', 
        'DiElectron_MinMass',
        'DiElectron_VCHI2PDOF',
        'DiElectron_PT',
        'DiElectron_TisTosSpecs',    
        
        # DiElectron Low Mass line
        'DiElectronLowMass_Prescale',
        'DiElectronLowMass_Postscale',
        'DiElectronLowMass_checkPV',
        
        'DiElectronLowMass_ElectronPT',
        'DiElectronLowMass_ElectronP',
        'DiElectronLowMass_ElectronPIDe',
        'DiElectronLowMass_ElectronTRCHI2DOF', 
        'DiElectronLowMass_MinMass',
        'DiElectronLowMass_VCHI2PDOF',       
        'DiElectronLowMass_PT',
        
        # BiasedDiElectron line
        'BiasedDiElectron_Prescale',
        'BiasedDiElectron_Postscale',
        
        'BiasedDiElectron_ElectronPT',
        'BiasedDiElectron_ElectronP',
        'BiasedDiElectron_ElectronPIDe',
        'BiasedDiElectron_ElectronMIPCHI2',  
        'BiasedDiElectron_ElectronTRCHI2DOF', 
        'BiasedDiElectron_MinMass',
        'BiasedDiElectron_VCHI2PDOF',
        'BiasedDiElectron_PT',

        # BiasedDiElectronLowMass line
        'BiasedDiElectronLowMass_Prescale',
        'BiasedDiElectronLowMass_Postscale',
        
        'BiasedDiElectronLowMass_ElectronPT',
        'BiasedDiElectronLowMass_ElectronP',
        'BiasedDiElectronLowMass_ElectronPIDe',
        'BiasedDiElectronLowMass_ElectronMIPCHI2', 
        'BiasedDiElectronLowMass_ElectronTRCHI2DOF', 
        'BiasedDiElectronLowMass_MinMass',
        'BiasedDiElectronLowMass_VCHI2PDOF',
        'BiasedDiElectronLowMass_PT',    

        # Jpsi2ee line
        'Jpsi2ee_Prescale',
        'Jpsi2ee_Postscale',
        'Jpsi2ee_checkPV',
        
        'Jpsi2ee_ElectronPT',
        'Jpsi2ee_ElectronP',
        'Jpsi2ee_ElectronPIDe',
        'Jpsi2ee_ElectronTRCHI2DOF', 
        'Jpsi2ee_MinMass',
        'Jpsi2ee_MaxMass',
        'Jpsi2ee_VCHI2PDOF',
        'Jpsi2ee_PT',
        'Jpsi2ee_TisTosSpecs'
        )
    

    def __init__(self, name, config ):

        LineBuilder.__init__(self, name, config)
        
        # if name not set outside, set it to empty 
        if name == None:
            name = ""

        """
        DiElectron line
        """
        self.SelDiElectron = filterDiElectron( name + 'DiElectron',
                                               ElectronPT        = config['DiElectron_ElectronPT'],
                                               ElectronP         = config['DiElectron_ElectronP'],
                                               ElectronPIDe      = config['DiElectron_ElectronPIDe'],
                                               ElectronTRCHI2DOF = config['DiElectron_ElectronTRCHI2DOF'],
                                               eeMinMass         = config['DiElectron_MinMass'],
                                               eeVCHI2PDOF       = config['DiElectron_VCHI2PDOF'],
                                               eePT              = config['DiElectron_PT']
                                               )

        self.HltTisDiElectron = filterTisTos( name + "DiElectron",
                                              DiElectronInput = self.SelDiElectron,
                                              myTisTosSpecs = config['DiElectron_TisTosSpecs']
                                              )

        self.DiElectronLine = StrippingLine( name + 'IncDiElectron' + 'Line',
                                             prescale  = config['DiElectron_Prescale'],
                                             postscale = config['DiElectron_Postscale'],
                                             checkPV   = config['DiElectron_checkPV'],
                                             selection = self.HltTisDiElectron
                                             )


        """
        DiElectronLowMass line
        """
        self.SelDiElectronLowMass = filterDiElectron( name + 'DiElectronLowMass',
                                                      ElectronPT        = config['DiElectronLowMass_ElectronPT'],
                                                      ElectronP         = config['DiElectronLowMass_ElectronP'],
                                                      ElectronPIDe      = config['DiElectronLowMass_ElectronPIDe'],
                                                      ElectronTRCHI2DOF = config['DiElectronLowMass_ElectronTRCHI2DOF'],
                                                      eeMinMass         = config['DiElectronLowMass_MinMass'],
                                                      eeVCHI2PDOF       = config['DiElectronLowMass_VCHI2PDOF'],
                                                      eePT              = config['DiElectronLowMass_PT']
                                                      )
        self.DiElectronLowMassLine = StrippingLine( name + 'DiElectronLowMass' + 'Line',
                                                    prescale  = config['DiElectronLowMass_Prescale'],
                                                    postscale = config['DiElectronLowMass_Postscale'],
                                                    checkPV   = config['DiElectronLowMass_checkPV'],
                                                    selection = self.SelDiElectronLowMass 
                                                    )

        
        """
        BiasedDiElectron line
        """
        self.SelBiasedDiElectron = filterBiasedDiElectron(
            name + 'BiasedDiElectron',
            ElectronPT        = config['BiasedDiElectron_ElectronPT'],
            ElectronP         = config['BiasedDiElectron_ElectronP'],
            ElectronPIDe      = config['BiasedDiElectron_ElectronPIDe'],
            ElectronMIPCHI2   = config['BiasedDiElectron_ElectronMIPCHI2'],
            ElectronTRCHI2DOF = config['BiasedDiElectron_ElectronTRCHI2DOF'],
            eeMinMass         = config['BiasedDiElectron_MinMass'],
            eeVCHI2PDOF       = config['BiasedDiElectron_VCHI2PDOF'],
            eePT              = config['BiasedDiElectron_PT']
            )
        self.BiasedDiElectronLine = StrippingLine( name + 'BiasedIncDiElectron' + 'Line',
                                                   prescale  = config['BiasedDiElectron_Prescale'],
                                                   postscale = config['BiasedDiElectron_Postscale'],
                                                   selection = self.SelBiasedDiElectron
                                                   )


        """
        BiasedDiElectronLowMass line
        """
        self.SelBiasedDiElectronLowMass = filterBiasedDiElectron(
            name + 'BiasedDiElectronLowMass',
            ElectronPT        = config['BiasedDiElectronLowMass_ElectronPT'],
            ElectronP         = config['BiasedDiElectronLowMass_ElectronP'],
            ElectronPIDe      = config['BiasedDiElectronLowMass_ElectronPIDe'],
            ElectronMIPCHI2   = config['BiasedDiElectronLowMass_ElectronMIPCHI2'],
            ElectronTRCHI2DOF = config['BiasedDiElectronLowMass_ElectronTRCHI2DOF'],
            eeMinMass         = config['BiasedDiElectronLowMass_MinMass'],
            eeVCHI2PDOF       = config['BiasedDiElectronLowMass_VCHI2PDOF'],
            eePT              = config['BiasedDiElectronLowMass_PT']
            )
        self.BiasedDiElectronLowMassLine = StrippingLine( name + 'BiasedIncDiElectronLowMass' + 'Line',
                                                          prescale  = config['BiasedDiElectronLowMass_Prescale'],
                                                          postscale = config['BiasedDiElectronLowMass_Postscale'],
                                                          selection = self.SelBiasedDiElectronLowMass
                                                          )       
               
        """
        Jpsi-> ee tight line
        """
        self.SelJpsi2ee = filterJpsi2ee( name + 'Jpsi2ee',
                                         ElectronPT        = config['Jpsi2ee_ElectronPT'],
                                         ElectronP         = config['Jpsi2ee_ElectronP'],
                                         ElectronPIDe      = config['Jpsi2ee_ElectronPIDe'],
                                         ElectronTRCHI2DOF = config['Jpsi2ee_ElectronTRCHI2DOF'],
                                         eeMinMass         = config['Jpsi2ee_MinMass'],
                                         eeMaxMass         = config['Jpsi2ee_MaxMass'],
                                         eeVCHI2PDOF       = config['Jpsi2ee_VCHI2PDOF'],
                                         eePT              = config['Jpsi2ee_PT']
                                         )

        self.HltTisJpsi2ee = filterTisTos( name + "Jpsi2ee",
                                           DiElectronInput = self.SelJpsi2ee,
                                           myTisTosSpecs = config['Jpsi2ee_TisTosSpecs']
                                           )
        
        self.Jpsi2eeLine = StrippingLine( name + 'Jpsi2ee' + 'Line',
                                          prescale  = config['Jpsi2ee_Prescale'],
                                          postscale = config['Jpsi2ee_Postscale'],
                                          checkPV   = config['Jpsi2ee_checkPV'],
                                          selection = self.HltTisJpsi2ee
                                          )
        
        if config['MicroDST']:
            self.registerLine( self.DiElectronLine )
            self.registerLine( self.BiasedDiElectronLine )
            
        else:    
            self.registerLine( self.DiElectronLine )
            self.registerLine( self.DiElectronLowMassLine )
            self.registerLine( self.BiasedDiElectronLine )
            self.registerLine( self.BiasedDiElectronLowMassLine )
            self.registerLine( self.Jpsi2eeLine )
            
        self.AllLines = [
            self.DiElectronLine,
            self.DiElectronLowMassLine,
            self.BiasedDiElectronLine,
            self.BiasedDiElectronLowMassLine,
            self.Jpsi2eeLine
            ]
        
        self.MicroDSTLines = [
            self.DiElectronLine,
            self.BiasedDiElectronLine            
            ]


def filterDiElectron( name,
                      ElectronPT,
                      ElectronP,
                      ElectronPIDe,
                      ElectronTRCHI2DOF,
                      eeMinMass,
                      eeVCHI2PDOF,
                      eePT 
                      ):
    
    _StdLooseDiElectron = DataOnDemand( Location = 'Phys/StdLooseDiElectron/Particles' )
    
    ElectronCut = "(MINTREE('e+'==ABSID,PT) > %(ElectronPT)s *MeV)" \
                  " & (MINTREE('e+'==ABSID,P) > %(ElectronP)s *MeV)"\
                  " & (MINTREE('e+'==ABSID,PIDe) > %(ElectronPIDe)s)"\
                  " & (MAXTREE('e+'==ABSID,TRCHI2DOF) < %(ElectronTRCHI2DOF)s)" % locals()
    
    eeCut = "(MM > %(eeMinMass)s) & (VFASPF(VCHI2PDOF)< %(eeVCHI2PDOF)s) & (PT > %(eePT)s)" % locals()
    
    _ee = FilterDesktop( Code = ElectronCut + " & " + eeCut )
    
    return Selection( name + "_Selee",
                      Algorithm = _ee,
                      RequiredSelections = [ _StdLooseDiElectron ]
                      )


def filterBiasedDiElectron( name,
                            ElectronPT,
                            ElectronP,
                            ElectronPIDe,
                            ElectronMIPCHI2,
                            ElectronTRCHI2DOF,
                            eeMinMass,
                            eeVCHI2PDOF,
                            eePT
                            ):
    
    _StdLooseDiElectron = DataOnDemand( Location = 'Phys/StdLooseDiElectron/Particles' )
    
    ElectronCut = "(MINTREE('e+'==ABSID,PT) > %(ElectronPT)s *MeV)"\
                  " & (MINTREE('e+'==ABSID,P) > %(ElectronP)s *MeV)"\
                  " & (MINTREE('e+'==ABSID,PIDe) > %(ElectronPIDe)s)"\
                  " & (MINTREE('e+'==ABSID,MIPCHI2DV(PRIMARY))> %(ElectronMIPCHI2)s )" \
                  " & (MAXTREE('e+'==ABSID,TRCHI2DOF) < %(ElectronTRCHI2DOF)s)" % locals()
    
    eeCut = "(MM > %(eeMinMass)s) & (VFASPF(VCHI2PDOF)< %(eeVCHI2PDOF)s) & (PT > %(eePT)s *MeV)" % locals()
    
    _ee = FilterDesktop( Code = ElectronCut + " & " + eeCut )
    
    return Selection( name + "_SeleeSS",
                      Algorithm = _ee,
                      RequiredSelections = [ _StdLooseDiElectron ]
                      )


def filterJpsi2ee( name,
                   ElectronPT,
                   ElectronP,
                   ElectronPIDe,
                   ElectronTRCHI2DOF,
                   eeMinMass,
                   eeMaxMass, 
                   eeVCHI2PDOF,
                   eePT
                   ):
    
    _StdLooseJpsi2ee = DataOnDemand( Location = 'Phys/StdLooseJpsi2ee/Particles' )
    
    ElectronCut = "(MINTREE('e+'==ABSID,PT) > %(ElectronPT)s *MeV)"\
                  " & (MINTREE('e+'==ABSID,P) > %(ElectronP)s *MeV)"\
                  " & (MINTREE('e+'==ABSID,PIDe) > %(ElectronPIDe)s)"\
                  " & (MAXTREE('e+'==ABSID,TRCHI2DOF) < %(ElectronTRCHI2DOF)s)" % locals()
    
    eeCut = "(MM > %(eeMinMass)s) & (MM < %(eeMaxMass)s)"\
            " & (VFASPF(VCHI2PDOF)< %(eeVCHI2PDOF)s)"\
            " & (PT > %(eePT)s)" % locals()
    
    _ee = FilterDesktop( Code = ElectronCut + " & " + eeCut )
    
    return Selection( name + "_SelJpsi2ee",
                      Algorithm = _ee,
                      RequiredSelections = [ _StdLooseJpsi2ee ]
                      )


def filterTisTos(name,
                 DiElectronInput,
                 myTisTosSpecs ) :
    from Configurables import TisTosParticleTagger
    
    myTagger = TisTosParticleTagger(name + "_TisTosTagger")
    myTagger.TisTosSpecs = myTisTosSpecs
    
    # To speed it up, TisTos only with tracking system)
    myTagger.ProjectTracksToCalo = False
    myTagger.CaloClustForCharged = False
    myTagger.CaloClustForNeutral = False
    myTagger.TOSFrac = { 4:0.0, 5:0.0 }
    
    return Selection(name + "_SelTisTos",
                     Algorithm = myTagger,
                     RequiredSelections = [ DiElectronInput ] )

