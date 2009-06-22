### @file
#
#  Hlt2 dimuon selections
#
#  @author J. Albrecht
#  @date 2009-02-13
#
#   1) - 4) unbiased dimuon selections, i.e. dimuon selections without
#           cuts correlated to the Bs lifetime
#
#  5) biased dimuon
#
##
from Gaudi.Configuration import *
from HltLine.HltLine import Hlt2Line, Hlt2Member, bindMembers
from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser

class Hlt2InclusiveDiMuonLinesConf(HltLinesConfigurableUser) :
    '''
          Put only the most discriminating variables as slots:
           - prescale factors, mass windows and dimuon pt
           - muon pt _not_ as slot as this strongly modifies the angular acceptances
    '''
    
    
    __slots__ = {  'Prescale'                  : { 'Hlt2UnbiasedDiMuon' :  0.05 }
                   ,'UnbiasedDiMuonMinMass'   : 2900      # MeV
                   ,'UnbiasedDiMuonPt'        : 1000      # MeV
                   ,'UnbiasedDiMuonMuPt'      : 500       #MeV
                   ,'UnbiasedDiMuonVertexChi2':  20
                   ,'UnbiasedJPsiMassWindow'  : 120       # MeV
                   ,'UnbiasedJPsiPt'          : 1000      # MeV
                   ,'UnbiasedJPsiMuPt'        : 500      # MeV
                   ,'UnbiasedJPsiVertexChi2'  :  20
                   ,'UnbiasedPsi2SMassWindow' : 120       # MeV
                   ,'UnbiasedPsi2SPt'         : 1000      # MeV
                   ,'UnbiasedPsi2SMuPt'       : 500      # MeV
                   ,'UnbiasedPsi2SVertexChi2' :  20
                   ,'UnbiasedBmmMinMass'      : 5000      # MeV
                   ,'UnbiasedBmmVertexChi2'   :  20
                   }


    def __apply_configuration__(self) :
        from Configurables import HltANNSvc
        from Hlt2SharedParticles.DiMuon import DiMuon
        from Configurables import FilterDesktop
        '''
          There are in total four unbiased selections:
          
          1) Heavy dimuon for sidebands etc, prescaled by 0.05
          2) mass window around J/psi
          3) mass window around Psi(2S)
          4) mass above 5 GeV
        '''
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2UnbiasedDiMuonDecision" : 50200 } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2UnbiasedJPsiDecision":  50201 } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2UnbiasedPsi2SDecision": 50202 } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2UnbiasedBmmDecision":   50203 } )

        filter = Hlt2Member(   FilterDesktop 
                               , "Filter"
                               , Code = "(MM>"+str(self.getProp('UnbiasedDiMuonMinMass'))+"*MeV) "
                               +"& (PT>"+str(self.getProp('UnbiasedDiMuonPt'))+"*MeV) "
                               +"& (MINTREE('mu-'==ABSID,PT)>"+str(self.getProp('UnbiasedDiMuonMuPt'))+"*MeV) "
                               +"& (VFASPF(VCHI2/VDOF)<"+str(self.getProp('UnbiasedDiMuonVertexChi2'))+")"
                               , InputLocations  = [ DiMuon ]
                               )
        '''
            unbiased heavy dimuon - prescaled
        '''
        line = Hlt2Line('UnbiasedDiMuon'
                        , prescale = self.prescale 
                        , algos = [ DiMuon, filter ])

        '''
            unbiased J/psi
        '''
        line.clone( 'UnbiasedJPsi'
                    , prescale = self.prescale 
                    , Filter = { 'Code': "(ADMASS('J/psi(1S)')<"+str(self.getProp('UnbiasedJPsiMassWindow'))+"*MeV) "
                                 +"& (PT>"+str(self.getProp('UnbiasedJPsiPt'))+"*MeV) "
                                 +"& (MINTREE('mu-'==ABSID,PT)>"+str(self.getProp('UnbiasedJPsiMuPt'))+"*MeV) "
                                 +"& (VFASPF(VCHI2/VDOF)<"+str(self.getProp('UnbiasedJPsiVertexChi2'))+")" }
                    )

        
        '''
            unbiased Psi(2S)
        '''
        line.clone( 'UnbiasedPsi2S'
                    , prescale = self.prescale 
                    , Filter = { 'Code': "(ADMASS(3686.09*MeV)<"+str(self.getProp('UnbiasedPsi2SMassWindow'))+"*MeV) "
                                 +"& (PT>"+str(self.getProp('UnbiasedPsi2SPt'))+"*MeV) "
                                 +"& (MINTREE('mu-'==ABSID,PT)>"+str(self.getProp('UnbiasedPsi2SMuPt'))+"*MeV) "
                                 +"& (VFASPF(VCHI2/VDOF)<"+str(self.getProp('UnbiasedPsi2SVertexChi2'))+")"}
                    )

        '''
            unbiased B
        '''
        line.clone( 'UnbiasedBmm'
                    , prescale = self.prescale 
                    , Filter = { 'Code': "(MM>"+str(self.getProp('UnbiasedBmmMinMass'))+"*MeV) "
                                 +"& (VFASPF(VCHI2/VDOF)<"+str(self.getProp('UnbiasedBmmVertexChi2'))+")"}
                    )

        
        ###
        #    ID 50040 - biased dimuon 
        ##
        importOptions( "$HLTSELECTIONSROOT/options/Hlt2LineBiasedDiMuon.py")

        HltANNSvc().Hlt2SelectionID.update( { "Hlt2BiasedDiMuonDecision" : 50040 } )
