'''
Stripping lines for Strange s+sbar correlations in form of correlated:tracks
a) K+ and K-
b) Lambda0 + K+, Lambda0bar + K-
c) Lambda0 + Lambda0bar

Long tracks only

'''

__author__ = ['Florin MACIUC']
__date__ = '24/08/2013'
__version__ = '$Revision: 1.0 $'

__all__ = ( 'SbarSCorrelationsConf', 'default_config' )

default_config = {
    'NAME'        : 'SbarSCorrelations',
    'BUILDERTYPE' : 'SbarSCorrelationsConf',
    'STREAMS'     : [ 'EW'  ],
    'WGs'         : [ 'QEE' ],
    'CONFIG' : { 'HLT' : "HLT_PASS('Hlt1MBNoBiasDecision')|HLT_PASS('Hlt1MBMicroBiasTStationDecision')|HLT_PASS('Hlt1MBMicroBiasVeloDecision')|HLT_PASS('Hlt1MBMicroBiasTStationRateLimitedDecision')|HLT_PASS('Hlt1MBMicroBiasVeloRateLimitedDecision')"
                ,  'LongTrackGEC'          :    1000 # 150 or 500 might be a better choice
                ,  'Trk_P_MIN'             : 5000 # to limit material interactions for Kaons
                ,  'isLong'                : '(ISLONG)'
                ,  'KAON_PIDK_MIN'         : 8 # DLL_KAON_vs_PION
                ,  'KAON_PIDKp_MIN'        : 0 # DLL_KAON_vs_PROTON
                ,  'KAON_ipChi2_MAX'                : 49 # IPCHI2 with respect to best PV (do not know if UPCHI2 or IP is used, but I guess IPCHI2 was used to tag the best PV )
#
                ,   'PION_ipChi2_MIN'       : 9
                ,   'PROTON_ipChi2_MIN'     : 9
                ,   'PION_P_MIN'            : 2000
                ,   'PROTON_P_MIN'          : 2000
#
                ,   'Fisher'                : 10
                ,   'Lambda_V_Chi2_Max'      : 9
                ,   'Lambda_Adamass'        : 50
                ,   'Lambda_ipChi2_MAX'     : 49
#                   
                , 'Phiprescale'             :    0.05
                , 'F2prescale'              :    1.0
                , 'LambdaCprescale'         :    1.0                   
                , 'postscale'             :    1.0
           },
    }



from Gaudi.Configuration                   import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from StandardParticles import StdAllNoPIDsPions, StdTightKaons, StdAllNoPIDsProtons
from PhysSelPython.Wrappers                import Selection, DataOnDemand, MergedSelection
from StrippingConf.StrippingLine           import StrippingLine
from StrippingUtils.Utils                  import LineBuilder

from GaudiKernel.SystemOfUnits import MeV

class SbarSCorrelationsConf(LineBuilder):
    __configuration_keys__ = default_config['CONFIG'].keys() 

    def __init__(self, name, config) :
        LineBuilder.__init__(self, name, config)

        # Set up global event cuts.
        # Conceptually these come first, although the place where they're
        # inserted into the line is at the bottom of the code.
        _globalEventCuts = { "Code":"( recSummaryTrack(LHCb.RecSummary.nLongTracks, TrLONG) < %(LongTrackGEC)s )" % config,
                            "Preambulo": ["from LoKiTracks.decorators import *"]}

        # Select all charged kaons, protons and pions.
        self.selKaon = filterKaons('KFor'+name, 'Phys/StdTightKaons/Particles',
                                          config['isLong'],
                                          config['Trk_P_MIN'],
                                          config['KAON_PIDK_MIN'],
                                          config['KAON_PIDKp_MIN'],
                                          config['KAON_ipChi2_MAX'])

        
        self.selPion = filterPions('PiFor'+name, 'Phys/StdAllNoPIDsPions/Particles',
                                          config['isLong'],
                                          config['PION_ipChi2_MIN'],
                                          config['PION_P_MIN']                                   
                                          )

        #using the same function/method to select Protons as Pions
        self.selProton = filterPions('ProtonFor'+name, 'Phys/StdAllNoPIDsProtons/Particles',
                                          config['isLong'],
                                          config['PROTON_ipChi2_MIN'],
                                          config['PROTON_P_MIN']                                        
                                          )        

        self.selPPi = [self.selPion, self.selProton]
        self.selKK = [self.selKaon]

        
        # Now make the various decays e.g. Lambda/Lambda-bar decays which are real
        # and the dummy decays "f_2(2300) -> Lambda0 Lambda~0", - not real decay but dummy
        #                      "phi(1020) -> K+ K-" - not real but dummy  
        #                      "Lambda_c+ -> Lambda0 K+" - not real but dummy
        #                      "Lambda_c- -> Lambda~0 K-" - not real but dummy
        #
        # real decays
        strDecaysLambda         = ["Lambda0 -> p+ pi-"]
        strDecaysLambdabar      = ["Lambda~0 -> p~- pi+"]
        # dummy decays
        strDecaysF2  = ["f_2(2300) -> Lambda0 Lambda~0"]
        strDecaysPhi = ["phi(1020) -> K+ K-"]
        strDecaysLambdaCplus   = ["Lambda_c+ -> Lambda0 K+"]
        strDecaysLambdaCminus =  ["Lambda_c~- -> Lambda~0 K-"]
        
        # make real decays 
        self.selLambda         = makeLambda('Lambda'        + name, strDecaysLambda,         self.selPPi, config['Fisher'], config['Lambda_V_Chi2_Max'], config['Lambda_Adamass'] , config['Lambda_ipChi2_MAX'])
        self.selLambdabar      = makeLambda('Lambdabar'        + name, strDecaysLambdabar,         self.selPPi, config['Fisher'],config['Lambda_V_Chi2_Max'], config['Lambda_Adamass'] , config['Lambda_ipChi2_MAX'])

#        print  self.selKK       
        # make a dummy Phi
        self.selPhi      = makePhi('Phi'        + name, strDecaysPhi,    self.selKK  )

        
        # Now make the dummy decays with at least a Lambda 
#        print [self.selLambda] + [self.selLambdabar]
#        print [self.selLambda] + self.selKK
#        print [self.selLambdabar] +  self.selKK
#        print 'input'
#        print strDecaysLambda
#        print strDecaysLambdabar

#        print strDecaysPhi
#        print strDecaysF2
#        print strDecaysLambdaCplus
#        print strDecaysLambdaCminus

        self.selF2         = makePhi('F2LambdaLambdabar'         + name, strDecaysF2, [self.selLambda]+ [self.selLambdabar])
        self.selLambdaCplus           = makePhi('LambdaCplus'           + name, strDecaysLambdaCplus,[self.selLambda]+ self.selKK)
        self.selLambdaCminus           = makePhi('LambdaCminus'           + name, strDecaysLambdaCminus,[self.selLambdabar]+ self.selKK)



        # Make the stripping lines
        self.linePhi          = StrippingLine(name+'PhiLine',
                                               FILTER    = _globalEventCuts,
                                               prescale  = config['Phiprescale'],
                                               postscale = config['postscale'],
                                               RequiredRawEvents = ["Muon","Calo","Rich"],
                                               HLT1 = config['HLT'],
                                               selection = self.selPhi)
        self.lineF2            = StrippingLine(name + 'F2Line',
                                               FILTER    = _globalEventCuts,
                                               prescale  = config['F2prescale'],
                                               postscale = config['postscale'],
                                               RequiredRawEvents = ["Muon","Calo","Rich"],
                                               HLT1 = config['HLT'],
                                               selection = self.selF2)
        self.lineLambdaCplus  =  StrippingLine(name+'LambdaCplusLine',
                                               FILTER    = _globalEventCuts,
                                               prescale  = config['LambdaCprescale'],
                                               postscale = config['postscale'],
                                               RequiredRawEvents = ["Muon","Calo","Rich"],
                                               HLT1 = config['HLT'],
                                               selection = self.selLambdaCplus)
        self.lineLambdaCminus  = StrippingLine(name+'LambdaCminusLine',
                                               FILTER    = _globalEventCuts,
                                               prescale  = config['LambdaCprescale'],
                                               postscale = config['postscale'],
                                               RequiredRawEvents = ["Muon","Calo","Rich"],
                                               HLT1 = config['HLT'],
                                               selection = self.selLambdaCminus)

        self.registerLine(self.linePhi)
        self.registerLine(self.lineF2)
        self.registerLine(self.lineLambdaCplus)
        self.registerLine(self.lineLambdaCminus)

        
def filterPions(name, inputName, isLong, PION_BPV_IPCHI2, PION_P_MIN):
    """
    Apply cuts to daughters of Lambdas
    """
    _strCutISLONG = "%(isLong)s"                             % locals()
    _strCutBPVIPCHI2 = "(BPVIPCHI2() > %(PION_BPV_IPCHI2)s)" % locals()
    _strCutPMIN = "(P  > %(PION_P_MIN)s)"                    % locals()
    _strCuts = '(' + _strCutISLONG + '&' + _strCutBPVIPCHI2 \
                   + '&' + _strCutPMIN  + ')'               
    _trackFilter = FilterDesktop(Code = _strCuts)
    inputTracks = DataOnDemand(Location = inputName)
    return Selection(name,
                     Algorithm          = _trackFilter,
                     RequiredSelections = [inputTracks])



                     
def filterKaons(name, inputName,isLong, Trk_P_MIN, KAON_PIDK_MIN, KAON_PIDKp_MIN, KAON_IPCHI2_MAX):
    """
    Apply cuts to daughters of D0
    """
    _strCutLONG  = "%(isLong)s"     % locals()
    _strCutP      = "(P  > %(Trk_P_MIN)s)"                        % locals()
    _strCutPID    = "(PIDK  > %(KAON_PIDK_MIN)s)"                 % locals()
    _strCutPID2    = "( (PIDK-PIDp) > %(KAON_PIDKp_MIN)s)"         % locals()
    

    _strCutIPChi2 = "(BPVIPCHI2() < %(KAON_IPCHI2_MAX)s)" % locals()
    
    _strCuts = '(' + _strCutLONG + '&' + _strCutP \
                   + '&' + _strCutPID \
                   + '&' + _strCutPID2 + '&' + _strCutIPChi2 + ')'

#    print '_strCuts: filterKaons ', _strCuts
    _trackFilter = FilterDesktop(Code = _strCuts)
    inputTracks = DataOnDemand(Location = inputName)
    return Selection(name,
                     Algorithm          = _trackFilter,
                     RequiredSelections = [inputTracks])







def makeLambda(name, inputDecayDescriptors, inputDaughters, FISHER_D, LAMBDA_V_CHI2, LAMBDA_ADAMASS, LAMBDA_IPCHI2_MAX):
    """
    Given lists of Lambda daughter tracks  reconstruct Lambda -> p+ pi-. and, alternatively, to  the charge conjugate. The  choice depends on the inputDecayDescriptors.
    """
#    "pi+" : "(P>2*GeV) & (MIPCHI2DV(PRIMARY)>9)",
#    "p+"  : "(P>2*GeV) & (MIPCHI2DV(PRIMARY)>9)"
#    X.CombinationCut = "(ADAMASS('Lambda0')<50*MeV) & (ADOCACHI2CUT(30, ''))"
#    X.MotherCut      =  "(ADMASS('Lambda0')<35*MeV) & (VFASPF(VCHI2)<30) & (BPVVDCHI2 > 4.)"
    
    _combCutMass = "(ADAMASS('Lambda0')<%(LAMBDA_ADAMASS)s*MeV)"  % locals()
    
# not needed because pion and proton are originating in Lambda (long-lived) decay vertex    _combCutsPV   = "(AALLSAMEBPV)"

    
    _combCuts = '(' + _combCutMass + ')'
    
    _motherCutFisher   = '( (CHILD(MIPDV(PRIMARY),1)*CHILD(MIPDV(PRIMARY),2)/MIPDV(PRIMARY))>%(FISHER_D)s)'  % locals()
    _motherCutVtx      = '(VFASPF(VCHI2PDOF) < %(LAMBDA_V_CHI2)s)' % locals()
    _motherCutIPCHI2_MAX = '( BPVIPCHI2() < %(LAMBDA_IPCHI2_MAX)s)' % locals()
    
    _motherCuts = '(' + _motherCutFisher   + '&' + _motherCutVtx + '&' + _motherCutIPCHI2_MAX + ')'
    
#    print 'inputDecayDescriptors', inputDecayDescriptors
#    print '_combCuts:MakeLambda', _combCuts 
#    print '_motherCuts:MakeLambda', _motherCuts

    _Lambda = CombineParticles(DecayDescriptors = inputDecayDescriptors,
                            CombinationCut   = _combCuts,
                            MotherCut        = _motherCuts)
    return Selection(name,
                     Algorithm = _Lambda,
                     RequiredSelections = inputDaughters)



"""
Make dummy Phi candidates

"""
def makePhi(name, inputDecayDescriptors, inputDaughters):
    """
    Given lists of K+/K- daughter tracks  reconstruct a dummy phi without applying cuts on combination and with PZ>0 dummy cut on Mother.
    """

    _combCuts = '(AALLSAMEBPV)' #'(ATRUE)' is nor enough, we need Kaons comming from the same PV in High PileUp
    _motherCuts =  'PZ>0' 


#    print '_combCuts:MakePhi', _combCuts
#    print '_motherCuts:MakePhi', _motherCuts

    _phi = CombineParticles(DecayDescriptors = inputDecayDescriptors,
                            CombinationCut   = _combCuts,
                            MotherCut        = _motherCuts)
    return Selection(name,
                     Algorithm = _phi,
                     RequiredSelections = inputDaughters)




'''
Make dummy  f_2 candidates - for now not needed as not needed the makeLambdaCplus or  makeLambdaCminus
'''

#def makeF2(name, inputDecayDescriptors, inputDaughters):
'''
    Given lists of Lambda/Lambdabar daughter particles reconstruct a dummy f_2 without applying cuts on combination and with PZ>0 dummy cut on Mother.
'''
#    _combCuts = '(AALLSAMEBPV)' #'(ATRUE)' is nor enough, we need Lambdas comming from the same PV in High PileUp
#    _motherCuts =  'PZ>0' 
#
#    _f2 = CombineParticles(DecayDescriptors = inputDecayDescriptors,
#                            CombinationCut   = _combCuts,
#                            MotherCut        = _motherCuts)
#    return Selection(name,
#                     Algorithm = _f2,
#                     RequiredSelections = inputDaughters)

                     

    
    
