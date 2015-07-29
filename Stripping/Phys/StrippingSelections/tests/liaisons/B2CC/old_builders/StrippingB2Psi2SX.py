# $Id: StrippingBs2Psi2SPhiPrescaledAndDetached.py,v 1.1 2010-06-30 12:53:17 jpalac Exp $
'''
Module for construction of Bs->Psi(2S)Phi pre-scaled and detached 
stripping Selections and StrippingLines.
Provides functions to build Bs, Psi2S, Phi selections.
Provides class Bs2Psi2SPhiPrescaledConf, which constructs the Selections and 
StrippingLines given a configuration dictionary.
Exported symbols (use python help!):
   - Bs2Psi2SPhiConf
   - makeBs2JpsiPhi
   - makePhi2KK
   - makePhi2KKLoose
'''

__author__ = ['Fred Blanc - Neus Lopez March']
__date__ = '16/2/2011'
__version__ = '$Revision: 1.0 $'


__all__ = ('B2Psi2SXConf',
           'makePions',
           'makeJpsi',
           'makePsi2S',
           'makeChK',
           'makePhi2KK',
           'makeKstar',
           'makeKsLoose',
           'makeKs',
           'makeInclPsi2SToJpsiPiPi',
           'makeBs2Psi2SPhiJpsiPiPi',
           'makeBu2Psi2SKJpsiPiPi',
           'makeBd2Psi2SKstarJpsiPiPi',
           'makeBd2Psi2SKsJpsiPiPi',
           'default_config'
           )




config_params = {'PionsTRCHI2DOF': 5,
#                 'Psi2SJpsiMIPCHI2DV' : 9 ,
                 'Psi2SPiMIPCHI2DV':9 , #before 4
                 'Psi2SAM23down': 400,
                 'Psi2SAM23up': 800,
                 'Psi2SAPT': 500,
                 'Psi2SADAMASS': 30,
                 'Psi2SVFASPF': 16,     
                 'PIDpi': 10, # new 10
                 'ChKTRCHI2DOF':5,
                 'ChKPID': 0 ,# before -2
                 'PhiWin':20,
                 'PhiPT':1000, #before 500 #ara 1000
                 'PhiVFASPF':25,        #new change it was 16 before
                 'PhiMAXTRCHI2DOF':5,
                 'PhiMINTRCHI2DOF':-2,#
                 'KstMassDown':  826,
                 'KstMassUp': 966,
                 'KstAPT':1500, #before 500
                 'KstVFASPF':16,
                 'KstTRCHI2DOF':4,
                 'KstPIDK': 0, #before -2
                 'KstPIDpi': 10, #new 10
                 'KsVFASPF':20,
                 'KsBPVDLS':5,
                 'incl_LinePrescale':0.1, #0.1 #before 0.5 #avui 0.2 
                 'incl_LinePostscale':1,
                 'BPVLTIME_detached':0.15,
                 'BsMassCutDownPre':5000,#before 5000
                 'BsMassCutUpPre':5650,#before 5650
                 'BsMassCutDownPost':5150,#before 5100
                 'BsMassCutUpPost':5550, #before 5550
                 'BsVCHI2PDOF':20,     #new change it was 10 before
                 'sig_LinePrescale': 1,
                 'sig_LinePostscale': 1,
                 'ChKPT': 500 ,
                 'K_LinePrescale': 1, #0.5
                 'K_LinePostscale': 1,
                 'Kstar_LinePrescale': 1, #0.5 #before 1 #ara 0.5 
                 'Kstar_LinePostscale': 1,
                 'MINTREEPT2' : 1000,
                 'BKsVCHI2PDOF': 10,
                 'Ks_LinePrescale': 1,
                 'Ks_LinePostscale':1
                 }                                                   

default_config = {
    'NAME'              : 'BetaSPsi2S',
    'WGs'               : ['B2CC'],
    'BUILDERTYPE'       : 'B2Psi2SXConf',
    'CONFIG'    : config_params,
    'STREAMS' : ['Leptonic'] }

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from StandardParticles import StdAllLoosePions
from StandardParticles import StdAllLooseMuons
from StandardParticles import StdAllLooseKaons
from PhysSelPython.Wrappers import Selection, DataOnDemand, MergedSelection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder


class B2Psi2SXConf(LineBuilder) :
  
    __configuration_keys__ = ('PionsTRCHI2DOF',
#                              'Psi2SJpsiMIPCHI2DV',
                              'Psi2SPiMIPCHI2DV',
                              'Psi2SAM23down',
                              'Psi2SAM23up',
                              'Psi2SAPT',
                              'Psi2SADAMASS',
                              'Psi2SVFASPF',
                              'PIDpi',
                              'ChKTRCHI2DOF',
                              'ChKPID',
                              'PhiWin',
                              'PhiPT',
                              'PhiVFASPF',
                              'PhiMAXTRCHI2DOF',
                              'PhiMINTRCHI2DOF',
                              'KstMassDown',
                              'KstMassUp', 
                              'KstAPT', 
                              'KstVFASPF', 
                              'KstTRCHI2DOF',
                              'KstPIDK',
                              'KstPIDpi',
                              'KsVFASPF',
                              'KsBPVDLS',
                              'incl_LinePrescale',
                              'incl_LinePostscale',
                              'BPVLTIME_detached',
                              'BsMassCutDownPre',
                              'BsMassCutUpPre',
                              'BsMassCutDownPost',
                              'BsMassCutUpPost',
                              'BsVCHI2PDOF',
                              'sig_LinePrescale',
                              'sig_LinePostscale',
                              'ChKPT',
                              'K_LinePrescale',
                              'K_LinePostscale',
                              'Kstar_LinePrescale',
                              'Kstar_LinePostscale',
                              'MINTREEPT2',
                              'BKsVCHI2PDOF',
                              'Ks_LinePrescale',
                              'Ks_LinePostscale'
                              )

    def __init__(self, name, config) :
        self.name = name
        LineBuilder.__init__(self, name, config)


        incl = self.name + '_InclPsi2SToJpsiPiPi'
        incl_detached_name = incl + 'Detached'
        sig = self.name + '_Bs2Psi2SPhiJpsiPiPi' 
        K = self.name + '_Bu2Psi2SKJpsiPiPi' 
        Kstar = self.name + '_Bd2Psi2SKstarJpsiPiPi' 
        Ks = self.name + '_Bd2Psi2SKsJpsiPiPi' 
     

        self.selPions = makePions( self.name + '_PionsForPsi2SJpsiPiPi',
                                   PionsTRCHI2DOF = config['PionsTRCHI2DOF']
                                   )
        
        self.selJpsi = makeJpsi(self.name + '_JpsiForPsi2SJpsiPiPi')
                                 
        
        self.selPsi2S2JpsiPiPi = makePsi2S(self.name + '_Psi2SJpsiPiPi',
                                           Pions = self.selPions,
                                           Jpsi = self.selJpsi,
#                                           Psi2SJpsiMIPCHI2DV = config['Psi2SJpsiMIPCHI2DV'],
                                           Psi2SPiMIPCHI2DV = config['Psi2SPiMIPCHI2DV'], 
                                           Psi2SAM23down = config['Psi2SAM23down'], 
                                           Psi2SAM23up = config['Psi2SAM23up'], 
                                           Psi2SAPT = config['Psi2SAPT'] , 
                                           Psi2SADAMASS = config['Psi2SADAMASS'], 
                                           Psi2SVFASPF = config['Psi2SVFASPF'],
                                           PIDpi = config['PIDpi']
                                           )
        
        self.selChargedK = makeChK(self.name + '_ChKForPsi2SJpsiPiPi',
                                   ChKTRCHI2DOF = config['ChKTRCHI2DOF'],
                                   ChKPID = config['ChKPID']
                                   ) 
        
        
        self.selPhi2KK = makePhi2KK( self.name + '_PhiForPsi2SJpsiPiPi',
                                     PhiWin = config['PhiWin'],
                                     PhiPT = config['PhiPT'],
                                     PhiVFASPF = config['PhiVFASPF'],
                                     PhiMAXTRCHI2DOF = config['PhiMAXTRCHI2DOF'],
                                     PhiMINTRCHI2DOF = config['PhiMINTRCHI2DOF']
                                     )

        self.selKstar = makeKstar(self.name + '_KstarForPsi2SJpsiPiPi',
                                  KstMassDown = config['KstMassDown'],
                                  KstMassUp = config['KstMassUp'],
                                  KstAPT = config['KstAPT'],
                                  KstVFASPF = config['KstVFASPF'],
                                  KstTRCHI2DOF = config['KstTRCHI2DOF'],
                                  KstPIDK = config['KstPIDK'],
                                  KstPIDpi = config['KstPIDpi']
                                  )  

        self.selKsLoose = makeKsLoose(self.name + '_KsLooseForPsi2SJpsiPiPi') 
        
        self.selKs = makeKs(self.name + '_KsForPsi2SJpsiPiPi',
                            KsLooseSel = self.selKsLoose,
                            KsVFASPF = config['KsVFASPF'],
                            KsBPVDLS = config['KsBPVDLS']
                            ) 

####################
### Inlusive psi(2S). We keep it for as long as we can.
####################

        self.InclPsi2SToJpsiPiPi = makeInclPsi2SToJpsiPiPi(incl,  
                                                           Psi2SSel = self.selPsi2S2JpsiPiPi 
                                                           )                                                         
        
        self.InclPsi2SToJpsiPiPi_line = StrippingLine(incl + "Line",
                                                      prescale = config['incl_LinePrescale'],
                                                      postscale = config['incl_LinePostscale'],
                                                      selection = self.InclPsi2SToJpsiPiPi,
                                                      EnableFlavourTagging = True,
                                                      MDSTFlag = True )
               
#####################
### Bs->Psi(2S)Phi ## 
#####################
        self.selBs2Psi2SPhiJpsiPiPi = makeBs2Psi2SPhiJpsiPiPi(sig,  
                                                              Psi2SSel = self.selPsi2S2JpsiPiPi, 
                                                              PhiSel = self.selPhi2KK,
                                                              BsMassCutDownPre = config['BsMassCutDownPre'],
                                                              BsMassCutUpPre = config['BsMassCutUpPre'],
                                                              BsMassCutDownPost = config['BsMassCutDownPost'],
                                                              BsMassCutUpPost = config['BsMassCutUpPost'],
                                                              BsVCHI2PDOF = config['BsVCHI2PDOF'],
                                                              BPVLTIME = config['BPVLTIME_detached']
                                                              )
        
        self.Bs2Psi2SPhiJpsiPiPi_line = StrippingLine(sig + "Line",
                                                      prescale = config['sig_LinePrescale'],
                                                      postscale = config['sig_LinePostscale'],
                                                      selection = self.selBs2Psi2SPhiJpsiPiPi,
                                                      EnableFlavourTagging = True, 
                                                      MDSTFlag = True )
        
#####################
### Bu->Psi(2S)K+ ###
#####################
        self.selBu2Psi2SKJpsiPiPi = makeBu2Psi2SKJpsiPiPi(K,  
                                                          Psi2SSel = self.selPsi2S2JpsiPiPi, 
                                                          ChKSel = self.selChargedK,
                                                          ChKPT = config['ChKPT'],
                                                          BsMassCutDownPre = config['BsMassCutDownPre'],
                                                          BsMassCutUpPre = config['BsMassCutUpPre'],
                                                          BsMassCutDownPost = config['BsMassCutDownPost'],
                                                          BsMassCutUpPost = config['BsMassCutUpPost'],
                                                          BsVCHI2PDOF = config['BsVCHI2PDOF'],
                                                          BPVLTIME = config['BPVLTIME_detached']
                                                          )
        
        self.Bu2Psi2SKJpsiPiPi_line = StrippingLine(K + "Line",
                                                    prescale = config['K_LinePrescale'],
                                                    postscale = config['K_LinePostscale'],
                                                    selection = self.selBu2Psi2SKJpsiPiPi,
                                                    EnableFlavourTagging = True, 
                                                    MDSTFlag = True )
     


#####################
### B0->Psi(2S)K*  ##
#####################

        self.selBd2Psi2SKstarJpsiPiPi = makeBd2Psi2SKstarJpsiPiPi(Kstar,  
                                                                  Psi2SSel = self.selPsi2S2JpsiPiPi, 
                                                                  KstarSel = self.selKstar,
                                                                  BsMassCutDownPre = config['BsMassCutDownPre'],
                                                                  BsMassCutUpPre = config['BsMassCutUpPre'],
                                                                  BsMassCutDownPost = config['BsMassCutDownPost'],
                                                                  BsMassCutUpPost = config['BsMassCutUpPost'],
                                                                  BsVCHI2PDOF = config['BsVCHI2PDOF'],
                                                                  MINTREEPT2 = config['MINTREEPT2'],
                                                                  BPVLTIME = config['BPVLTIME_detached']
                                                                  )
        
        self.Bd2Psi2SKstarJpsiPiPi_line = StrippingLine(Kstar + "Line",
                                                        prescale = config['Kstar_LinePrescale'],
                                                        postscale = config['Kstar_LinePostscale'],
                                                        selection = self.selBd2Psi2SKstarJpsiPiPi,
                                                        EnableFlavourTagging = True, 
                                                        MDSTFlag = True )
        
        
#####################
### Bd->Psi(2S)KS ###
#####################
        
        self.selBd2Psi2SKsJpsiPiPi = makeBd2Psi2SKsJpsiPiPi(Ks,  
                                                            Psi2SSel = self.selPsi2S2JpsiPiPi, 
                                                            KsSel = self.selKs,
                                                            BsMassCutDownPre = config['BsMassCutDownPre'],
                                                            BsMassCutUpPre = config['BsMassCutUpPre'],
                                                            BsMassCutDownPost = config['BsMassCutDownPost'],
                                                            BsMassCutUpPost = config['BsMassCutUpPost'],
                                                            BKsVCHI2PDOF = config['BKsVCHI2PDOF']
                                                            )
        
        self.Bd2Psi2SKsJpsiPiPi_line = StrippingLine(Ks + "Line",
                                                     prescale = config['Ks_LinePrescale'],
                                                     postscale = config['Ks_LinePostscale'],
                                                     selection = self.selBd2Psi2SKsJpsiPiPi,
                                                     EnableFlavourTagging = True, 
                                                     MDSTFlag = True )
        
        
        self.registerLine(self.InclPsi2SToJpsiPiPi_line)
        self.registerLine(self.Bs2Psi2SPhiJpsiPiPi_line)
        self.registerLine(self.Bu2Psi2SKJpsiPiPi_line)
        self.registerLine(self.Bd2Psi2SKstarJpsiPiPi_line)
        self.registerLine(self.Bd2Psi2SKsJpsiPiPi_line)
        
def makePions(name,
              PionsTRCHI2DOF #<5
              ):

    _stdLoosePions = DataOnDemand(Location = "Phys/StdAllLoosePions/Particles")
    _code = "(TRCHI2DOF < %(PionsTRCHI2DOF)s)"  % locals()
    _Pions = FilterDesktop(Code = _code)
    
#    print ' makePions ', name, 'Code ', _code
    
    return Selection (name,
                      Algorithm = _Pions,
                      RequiredSelections = [_stdLoosePions])
    
def makeJpsi(name):

    _stdJpsi = DataOnDemand(Location = "Phys/StdMassConstrainedJpsi2MuMu/Particles")
    _code = "ALL"
    _Jpsi = FilterDesktop(Code = _code)
    
#    print ' makeJpsi ', name, 'Code ', _code

    return Selection (name,
                      Algorithm = _Jpsi,
                      RequiredSelections = [ _stdJpsi ])
    

def makePsi2S(name,
              Pions,
              Jpsi,
              #Psi2SJpsiMIPCHI2DV, # 4
              Psi2SPiMIPCHI2DV, #4
              Psi2SAM23down, #>400 
              Psi2SAM23up, #<800 
              Psi2SAPT, #> 500
              Psi2SADAMASS, #<30
              Psi2SVFASPF, #<16
              PIDpi #10
              ) :

   # _daughtersCuts = { "J/psi(1S)" : "MIPCHI2DV(PRIMARY) > %(Psi2SJpsiMIPCHI2DV)s" % locals(), "pi+" : "MIPCHI2DV(PRIMARY) > %(Psi2SPiMIPCHI2DV)s" % locals()}
    _daughtersCuts = {  "pi+" : "MIPCHI2DV(PRIMARY) > %(Psi2SPiMIPCHI2DV)s" % locals()}
    _preVertexCuts = "(AM23>%(Psi2SAM23down)s*MeV) & (AM23<%(Psi2SAM23up)s*MeV)&(APT>%(Psi2SAPT)s*MeV) & (ADAMASS('psi(2S)') < %(Psi2SADAMASS)s*MeV)" % locals()
    _motherCuts = "(VFASPF(VCHI2/VDOF) < %(Psi2SVFASPF)s)  & (MINTREE('pi-'==ABSID, PIDK) < %(PIDpi)s) & (MINTREE('pi+'==ABSID, PIDK) < %(PIDpi)s)" % locals()
    _Psi2S = CombineParticles( DecayDescriptor = "psi(2S) -> J/psi(1S) pi+ pi-",
                               DaughtersCuts = _daughtersCuts,
                               CombinationCut = _preVertexCuts,
                               MotherCut = _motherCuts,
                               ReFitPVs = True
                               )

#    print ' makePsi2S ', name, 'MotherCuts:', _motherCuts, 'DaughtersCuts:', _daughtersCuts, 'preVertexCuts:', _preVertexCuts
    return Selection ( name,
                       Algorithm = _Psi2S,
#                       RequiredSelections = [Pions,Jpsi]
                       RequiredSelections = [Jpsi,Pions]
                       )


def makeChK(name,
            ChKTRCHI2DOF,#<5 
            ChKPID #>-2
            ) :
    
    _stdChK = DataOnDemand(Location = "Phys/StdAllLooseKaons/Particles")
    _code = "(TRCHI2DOF < %(ChKTRCHI2DOF)s) & (PIDK > %(ChKPID)s)"  % locals()
    _ChKFilter = FilterDesktop(Code = _code)
    
#    print ' makeChK ', name, 'Code ', _code
    return Selection (name,
                      Algorithm = _ChKFilter,
                      RequiredSelections = [_stdChK])

def makePhi2KK(name,
               PhiWin, # 20
               PhiPT, # 500
               PhiVFASPF, # 16
               PhiMAXTRCHI2DOF, # <5
               PhiMINTRCHI2DOF # >-2
               ) :

    _stdPhi2KK = DataOnDemand(Location = "Phys/StdLoosePhi2KK/Particles")   
    _code = "(ADMASS('phi(1020)') < %(PhiWin)s) & (PT > %(PhiPT)s *MeV) & (VFASPF(VCHI2) < %(PhiVFASPF)s) & (MAXTREE('K+'==ABSID, TRCHI2DOF) < %(PhiMAXTRCHI2DOF)s) & (MINTREE('K+'==ABSID, PIDK) > %(PhiMINTRCHI2DOF)s)" % locals()
    _phiFilter = FilterDesktop(Code = _code)

#    print ' makePhi2KK ', name, 'Code ', _code
    return Selection (name,
                      Algorithm = _phiFilter,
                      RequiredSelections = [_stdPhi2KK])

def makeKstar(name,
              KstMassDown, # 826
              KstMassUp, #966
              KstAPT, #>500
              KstVFASPF, #<16
              KstTRCHI2DOF, # 4
              KstPIDK, # -2
              KstPIDpi # 10  
              ) :


    _stdKstar = DataOnDemand(Location = "Phys/StdLooseKstar2Kpi/Particles")   
    _code = "(in_range(%(KstMassDown)s,M,%(KstMassUp)s)) & (PT > %(KstAPT)s*MeV) " \
                        "& (VFASPF(VCHI2) < %(KstVFASPF)s)" \
                        "& (MAXTREE('K+'==ABSID,  TRCHI2DOF) < %(KstTRCHI2DOF)s )" \
                        "& (MAXTREE('pi-'==ABSID, TRCHI2DOF) < %(KstTRCHI2DOF)s )" \
                        "& (MINTREE('K+'==ABSID, PIDK) > %(KstPIDK)s)" \
                        "& (MINTREE('pi-'==ABSID, PIDK) < %(KstPIDpi)s)" % locals()

    _KstarFilter = FilterDesktop(Code = _code)

#    print ' makePhi2KK ', name, 'Code ', _code
    return Selection (name,
                      Algorithm = _KstarFilter,
                      RequiredSelections = [_stdKstar])


def makeKsLoose( name ):
    return MergedSelection(name,
                           RequiredSelections = [DataOnDemand(Location = "Phys/StdLooseKsDD/Particles"),
                                                 DataOnDemand(Location = "Phys/StdLooseKsLL/Particles")] )

def makeKs(name,
           KsLooseSel,
           KsVFASPF, #20
           KsBPVDLS #5
           ):
    _code = "(VFASPF(VCHI2)<%(KsVFASPF)s) & (BPVDLS>%(KsBPVDLS)s)" % locals()
    _KsFilter = FilterDesktop(Code = _code)

#    print ' makeKs ', name, 'Code ', _code 
    return Selection (name,
                      Algorithm = _KsFilter,
                      RequiredSelections = [KsLooseSel])



####################
### Inlusive psi(2S). We keep it for as long as we can.
####################
def makeInclPsi2SToJpsiPiPi(name,
                            Psi2SSel
                            ) :
    _code = "ALL"
    _InclFilter = FilterDesktop(Code = _code)
    
    return Selection (name,
                      Algorithm = _InclFilter,
                      RequiredSelections = [Psi2SSel])




#####################
### Bs->Psi(2S)Phi ##
#####################

def makeBs2Psi2SPhiJpsiPiPi(name,
                            Psi2SSel,
                            PhiSel,
                            BsMassCutDownPre,
                            BsMassCutUpPre,
                            BsMassCutDownPost,
                            BsMassCutUpPost,
                            BsVCHI2PDOF, #<10
                            BPVLTIME
                            ) :
    
    _preVertexCuts = "in_range(%(BsMassCutDownPre)s,AM,%(BsMassCutUpPre)s)" % locals()
    _motherCuts = "in_range(%(BsMassCutDownPost)s,M,%(BsMassCutUpPost)s) & (VFASPF(VCHI2PDOF)<%(BsVCHI2PDOF)s) & (BPVLTIME()> %(BPVLTIME)s*ps) " % locals()
    
#    print 'makeBs2Psi2SPhiJpsiPiPi', name, 'MotherCuts:', _motherCuts, '_preVertexCuts:', _preVertexCuts
    
    _Bs = CombineParticles( DecayDescriptor = "B_s0 -> psi(2S) phi(1020)",
                            CombinationCut = _preVertexCuts,
                            MotherCut = _motherCuts,
                            ReFitPVs = True
                            )
    
    return Selection ( name,
                       Algorithm = _Bs,
#                       RequiredSelections = [PhiSel, Psi2SSel])
                       RequiredSelections = [Psi2SSel, PhiSel])


        
#####################
### Bu->Psi(2S)K+ ###
#####################

def makeBu2Psi2SKJpsiPiPi(name,
                          Psi2SSel,
                          ChKSel,
                          ChKPT, #500
                          BsMassCutDownPre,
                          BsMassCutUpPre,
                          BsMassCutDownPost,
                          BsMassCutUpPost,
                          BsVCHI2PDOF, #<10
                          BPVLTIME
                          ) :

    _daughtersCuts= {"K+": "(PT > %(ChKPT)s)" % locals()}
    _preVertexCuts = "in_range(%(BsMassCutDownPre)s,AM,%(BsMassCutUpPre)s)" % locals()
    _motherCuts = "in_range(%(BsMassCutDownPost)s,M,%(BsMassCutUpPost)s) & (VFASPF(VCHI2PDOF)<%(BsVCHI2PDOF)s) & (BPVLTIME()> %(BPVLTIME)s*ps)" % locals()
    
#    print 'makeBu2Psi2SKJpsiPiPi', name, 'MotherCuts:', _motherCuts, '_preVertexCuts:', _preVertexCuts, '_daughtersCuts', _daughtersCuts
    _Bs = CombineParticles( DecayDescriptor = "[B+ -> psi(2S) K+]cc",
                            DaughtersCuts = _daughtersCuts,
                            CombinationCut = _preVertexCuts,
                            MotherCut = _motherCuts,
                            ReFitPVs = True
                            )

    return Selection ( name,
                       Algorithm = _Bs,
#                       RequiredSelections = [ChKSel, Psi2SSel])
                      RequiredSelections = [Psi2SSel, ChKSel])




#####################
### B0->Psi(2S)K*  ##
#####################

def makeBd2Psi2SKstarJpsiPiPi(name,
                              Psi2SSel,
                              KstarSel,
                              BsMassCutDownPre,
                              BsMassCutUpPre,
                              BsMassCutDownPost,
                              BsMassCutUpPost,
                              BsVCHI2PDOF, #<10
                              MINTREEPT2,
                              BPVLTIME
                              ) :

    _preVertexCuts = "in_range(%(BsMassCutDownPre)s,AM,%(BsMassCutUpPre)s)" % locals()
    _motherCuts = "in_range(%(BsMassCutDownPost)s,M,%(BsMassCutUpPost)s) & (VFASPF(VCHI2PDOF)<%(BsVCHI2PDOF)s) & (MINTREE('K*(892)0'==ABSID, PT)> %(MINTREEPT2)s*MeV) & (BPVLTIME()> %(BPVLTIME)s*ps)" % locals()

#    print 'makeBd2Psi2SKstarJpsiPiPi', name, 'MotherCuts:', _motherCuts, '_preVertexCuts:', _preVertexCuts
    
    _Bs = CombineParticles( DecayDescriptor = "[B0 -> psi(2S) K*(892)0]cc",
                            CombinationCut = _preVertexCuts,
                            MotherCut = _motherCuts,
                            ReFitPVs = True
                            )
    
    return Selection ( name,
                       Algorithm = _Bs,
#                       RequiredSelections = [KstarSel, Psi2SSel])
                       RequiredSelections = [Psi2SSel, KstarSel])


#####################
### Bd->Psi(2S)KS ###
#####################
def makeBd2Psi2SKsJpsiPiPi(name,
                           Psi2SSel,
                           KsSel,
                           BsMassCutDownPre,
                           BsMassCutUpPre,
                           BsMassCutDownPost,
                           BsMassCutUpPost,
                           BKsVCHI2PDOF #different 20
                           ) :
    
    _preVertexCuts = "in_range(%(BsMassCutDownPre)s,AM,%(BsMassCutUpPre)s)" % locals()
    _motherCuts = "in_range(%(BsMassCutDownPost)s,M,%(BsMassCutUpPost)s) & (VFASPF(VCHI2PDOF)<%(BKsVCHI2PDOF)s)" % locals()
    
#    print 'makeBd2Psi2SKsJpsiPiPi', name, 'MotherCuts:', _motherCuts, '_preVertexCuts:', _preVertexCuts
    
    _Bs = CombineParticles( DecayDescriptor = "B0 -> psi(2S) KS0",
                            CombinationCut = _preVertexCuts,
                            MotherCut = _motherCuts,
                            ReFitPVs = True
                            )
    
    return Selection ( name,
                       Algorithm = _Bs,
#                       RequiredSelections = [KsSel, Psi2SSel])
                       RequiredSelections = [Psi2SSel, KsSel])



