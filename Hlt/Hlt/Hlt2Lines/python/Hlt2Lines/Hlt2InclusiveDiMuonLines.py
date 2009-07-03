## @file
#
#  Hlt2 dimuon selections
#
#  @author J. Albrecht
#  @date 2009-02-13
#
#   1) - 4) unbiased dimuon selections, i.e. dimuon selections without
#           cuts correlated to the Bs lifetime
#
#   5) - 8) biased dimuon (Leandro de Paula)
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
    
    
    __slots__ = {  'Prescale'                  : { 'Hlt2UnbiasedDiMuon'     :  0.05 
                                                 , 'Hlt2BiasedDiMuonRobust' :  0.01  
                                                 , 'Hlt2BiasedDiMuonRefined':  0.01 
                                                 }
                   ,'UnbiasedDiMuonMinMass'   : 2900      # MeV
                   ,'UnbiasedDiMuonPt'        : 1000      # MeV
                   ,'UnbiasedDiMuonMuPt'      :  500      # MeV
                   ,'UnbiasedDiMuonVertexChi2':   20
                   ,'UnbiasedJPsiMassWindow'  :  120      # MeV
                   ,'UnbiasedJPsiPt'          : 1000      # MeV
                   ,'UnbiasedJPsiMuPt'        :  500      # MeV
                   ,'UnbiasedJPsiVertexChi2'  :   20
                   ,'UnbiasedPsi2SMassWindow' :  120      # MeV
                   ,'UnbiasedPsi2SPt'         : 1000      # MeV
                   ,'UnbiasedPsi2SMuPt'       :  500      # MeV
                   ,'UnbiasedPsi2SVertexChi2' :   20
                   ,'UnbiasedBmmMinMass'      : 5000      # MeV
                   ,'UnbiasedBmmVertexChi2'   :   20

                   ,'BiasedSingleMuonPt'      :  700      # MeV
                   ,'BiasedMass'              :  500      # MeV
                   ,'BiasedLMass'             : 1200      # MeV
                   ,'BiasedTMass'             : 2900      # MeV
                   ,'BiasedSingleMuonIP'      :     .02   # mm 
                   ,'BiasedSingleMuonTIP'     :     .05   # mm 
                   ,'BiasedLTime'             :     .1    # ps 
                   ,'BiasedLTimeT'            :     .10   # ps 
                   ,'BiasedSingleIPChi2'      :   10     
                   ,'BiasedSingleIPTChi2'     :   50     
                   ,'BiasedVertexChi2'        :    5
                   ,'BiasedPVDistanceChi2'    :   10
                   ,'BiasedPVDistanceTChi2'   :  150
                   , 'IncludeLines'           : ['BiasedDiMuonMass',
                                                 'BiasedDiMuonIP'
                                                ]
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


        '''
        Biased DiMuon Lines
               Leandro de Paula - leandro.de.paula@cern.ch
 
        There are in total 4 biased selections:

        Two selections try to reduce the rate without significan drop in signal efficiency.
        At nominal conditions they should be prescaled:

        5) Robust Biased DiMuon: do not cut on variable errors  
        6) Refined Biased DiMuon: use all variables

        and

        7) High Mass Biased DiMuon selection 
        8) Biased DiMuon selection with high IP cuts
        '''
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2BiasedDiMuonRobustDecision"  : 50040 } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2BiasedDiMuonRefinedDecision" : 50041 } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2BiasedDiMuonMassDecision"    : 50042 } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2BiasedDiMuonIPDecision"      : 50043 } )
        '''
           cut definitions for biased DiMuon Lines
        '''
        MuPtCut = "( MAXTREE(ABSID=='mu+',PT)>"+str(self.getProp('BiasedSingleMuonPt'))+"*MeV)"
        MassCut = "(MM>"+str(self.getProp('BiasedMass'))+"*MeV)"
        MassLCut = "(MM>"+str(self.getProp('BiasedLMass'))+"*MeV)"
        MassTCut = "(MM>"+str(self.getProp('BiasedTMass'))+"*MeV)"
        MuIPCut = "(2==NINTREE((ABSID=='mu+') & (MIPDV(PRIMARY)>"+str(self.getProp('BiasedSingleMuonIP'))+"*mm)))"
        MuTIPCut = "(2==NINTREE((ABSID=='mu+') & (MIPDV(PRIMARY)>"+str(self.getProp('BiasedSingleMuonTIP'))+"*mm)))"
        LTimeCut = "(BPVLTIME('PropertimeFitter/properTime:PUBLIC')>"+str(self.getProp('BiasedLTime'))+"*ps)"
        LTimeTCut = "(BPVLTIME('PropertimeFitter/properTime:PUBLIC')>"+str(self.getProp('BiasedLTimeT'))+"*ps)"
        IPChi2Cut = "( MAXTREE(ABSID=='mu+',MIPCHI2DV(PRIMARY))>"+str(self.getProp('BiasedSingleIPChi2'))+")"
        TIPChi2Cut = "( MAXTREE(ABSID=='mu+',MIPCHI2DV(PRIMARY))>"+str(self.getProp('BiasedSingleIPTChi2'))+")"
        VertexChi2Cut = "(VFASPF(VCHI2/VDOF)<"+str(self.getProp('BiasedVertexChi2'))+")"
        PVDistChi2Cut = "(BPVVDCHI2>"+str(self.getProp('BiasedPVDistanceChi2'))+")"
        PVDistTChi2Cut = "(BPVVDCHI2>"+str(self.getProp('BiasedPVDistanceTChi2'))+")"
        '''
           sequence definitions for biased DiMuon Lines
        '''
        RobustDiMuon = Hlt2Member( FilterDesktop
                                   , "RobustDiMuon"          
                                   , InputLocations = [ DiMuon ]
                                   , Code = MuPtCut +"&"+ MassCut +"&"+ MuIPCut +"&"+ LTimeCut
                                 )
        RefinedDiMuon = Hlt2Member( FilterDesktop
                                    , "RefinedDiMuon"          
                                    , InputLocations = [ RobustDiMuon ]
                                    , Code = IPChi2Cut +"&"+ VertexChi2Cut +"&"+ PVDistChi2Cut
                                  )
        '''
            robust biased DiMuon 
        '''
        line = Hlt2Line('BiasedDiMuonRobust'
                        , prescale = self.prescale 
                        , algos = [ DiMuon, RobustDiMuon ])
        '''
            refined biased DiMuon
        '''
        line = Hlt2Line('BiasedDiMuonRefined'
                        , prescale = self.prescale 
                        , algos = [ DiMuon, RobustDiMuon, RefinedDiMuon ])
        '''
            high mass biased DiMuon
        '''
        if 'BiasedDiMuonMass' in self.getProp('IncludeLines'):
            line.clone( 'BiasedDiMuonMass'
                        , prescale = self.prescale 
                        , RobustDiMuon = {"Code" : MuPtCut +"&"+ MassTCut +"&"+ MuIPCut +"&"+ LTimeTCut }
                      )
        '''
            biased DiMuon with tigh IP cuts
        '''
        if 'BiasedDiMuonIP' in self.getProp('IncludeLines'):
            line.clone( 'BiasedDiMuonIP'
                        , prescale = self.prescale 
                        , RobustDiMuon = {"Code" : MuPtCut +"&"+ MassLCut +"&"+ MuTIPCut +"&"+ LTimeCut }
                        , RefinedDiMuon = {"Code" : TIPChi2Cut +"&"+ VertexChi2Cut +"&"+ PVDistTChi2Cut }
                      )
