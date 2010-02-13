## @file
#
#  Hlt2 dimuon selections
#
#  @author J. Albrecht, Leandro de Paula, Antonio Perez-Calero
#  @date 2009-02-13
#
#   1) - 4) unbiased dimuon selections, i.e. dimuon selections without
#           cuts correlated to the Bs lifetime (J. Albrecht)
#
#   5) - 8) biased dimuon (Leandro de Paula)
#
##
from Gaudi.Configuration import *
from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser

class Hlt2InclusiveDiMuonLinesConf(HltLinesConfigurableUser) :
    '''
          Put only the most discriminating variables as slots:
           - prescale factors, mass windows and dimuon pt
           - muon pt _not_ as slot as this strongly modifies the angular acceptances
    '''
    
    
    __slots__ = {  'Prescale'                  : {  'Hlt2UnbiasedDiMuon'         :  0.05
                                                    ,'Hlt2DiMuonUnbiasedJPsiLow' :  0.25
                                                    ,'Hlt2BiasedDiMuonRobust'    :  0.01  
                                                    ,'Hlt2BiasedDiMuonRefined'   :  0.01
                                                    ,'Hlt2BiasedDiMuonIP'        :  0.5
                                                    ,'Hlt2DiMuonDY1'             :  0.01
                                                    ,'Hlt2DiMuonDY2'             :  0.01
                                                    }

                   ,'UnbiasedDiMuonMinMass'   : 2900      # MeV
                   ,'UnbiasedDiMuonPt'        : 1000      # MeV
                   ,'UnbiasedDiMuonMuPt'      :  500      # MeV
                   ,'UnbiasedDiMuonVertexChi2':   20

                   ,'UnbiasedJPsiMassWindow'  :   70      # MeV
                   ,'UnbiasedJPsiPt'          : 1000      # MeV
                   ,'UnbiasedJPsiMuPt'        :  500      # MeV
                   ,'UnbiasedJPsiVertexChi2'  :   20
                   
                   ,'UnbiasedPsi2SMassWindow' :   70      # MeV
                   ,'UnbiasedPsi2SPt'         : 1000      # MeV
                   ,'UnbiasedPsi2SMuPt'       : 1500      # MeV
                   ,'UnbiasedPsi2SVertexChi2' :   20
                   
                   ,'UnbiasedBmmMinMass'      : 5200      # MeV
                   ,'UnbiasedBmmVertexChi2'   :   20

                   ,'UnbiasedZmmMinMass'      :40000      # MeV
                   ,'UnbiasedZmmPt'           :10000      # MeV

                   ,'DYPt'                    :  500      # MeV
                   ,'DY1MinMass'              : 2500      # MeV
                   ,'DY1MaxMass'              : 5000      # MeV
                   ,'DY2MinMass'              : 5000      # MeV
                   ,'DY2MaxMass'              :10000      # MeV
                   ,'DY3MinMass'              :10000      # MeV
                   ,'DY3MaxMass'              :20000      # MeV
                   ,'DY4MinMass'              :20000      # MeV
                   ,'DY4MaxMass'              :40000      # MeV
                   
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

                   }
    

    def __apply_configuration__(self) :
        from HltLine.HltLine import Hlt2Line, Hlt2Member, bindMembers
        from HltLine.HltReco import PV3D
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
        
        
        #--------------------------------------------

        filter = Hlt2Member(   FilterDesktop 
                               , "Filter"
                               , Code = "(MM>%(UnbiasedDiMuonMinMass)d*MeV)"\
                               " & (PT>%(UnbiasedDiMuonPt)d*MeV)"\
                               " & (MINTREE('mu-'==ABSID,PT)>%(UnbiasedDiMuonMuPt)d*MeV) "\
                               "& (VFASPF(VCHI2/VDOF)<%(UnbiasedDiMuonVertexChi2)d)" %  self.getProps() 
                               , InputPrimaryVertices = "None"
                               , UseP2PVRelations = False
                               , InputLocations  = [ DiMuon ]
                               )
        
        #--------------------------------------------
        '''
        unbiased heavy dimuon - prescaled
        '''
        line = Hlt2Line('UnbiasedDiMuon'
                        , prescale = self.prescale 
                        , algos = [ DiMuon, filter ]
                        , postscale = self.postscale
                        )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2UnbiasedDiMuonDecision" : 50200 } )
            

        #--------------------------------------------
        '''
        unbiased J/psi
        '''
        line.clone( 'DiMuonUnbiasedJPsi'
                    , prescale = self.prescale 
                    , Filter = { 'Code': "(ADMASS('J/psi(1S)')<"+str(self.getProp('UnbiasedJPsiMassWindow'))+"*MeV) "
                                 +"& (PT>"+str(self.getProp('UnbiasedJPsiPt'))+"*MeV) "
                                 +"& (MINTREE('mu-'==ABSID,PT)>"+str(self.getProp('UnbiasedJPsiMuPt'))+"*MeV) "
                                 +"& (VFASPF(VCHI2/VDOF)<"+str(self.getProp('UnbiasedJPsiVertexChi2'))+")" }
                    , postscale = self.postscale
                    )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2DiMuonUnbiasedJPsiDecision":  50201 } )
        

        #--------------------------------------------
        '''
        unbiased Psi(2S)
        '''
        line.clone( 'DiMuonUnbiasedPsi2S'
                    , prescale = self.prescale 
                    , Filter = { 'Code': "(ADMASS(3686.09*MeV)<"+str(self.getProp('UnbiasedPsi2SMassWindow'))+"*MeV) "
                                 +"& (PT>"+str(self.getProp('UnbiasedPsi2SPt'))+"*MeV) "
                                 +"& (MINTREE('mu-'==ABSID,PT)>"+str(self.getProp('UnbiasedPsi2SMuPt'))+"*MeV) "
                                 +"& (VFASPF(VCHI2/VDOF)<"+str(self.getProp('UnbiasedPsi2SVertexChi2'))+")"}
                    , postscale = self.postscale
                    )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2DiMuonUnbiasedPsi2SDecision": 50202 } )
        
        #--------------------------------------------
        '''
        unbiased Bmm
        '''
        line.clone( 'DiMuonUnbiasedBmm'
                    , prescale = self.prescale 
                    , Filter = { 'Code': "(MM>"+str(self.getProp('UnbiasedBmmMinMass'))+"*MeV) "
                                 +"& (VFASPF(VCHI2/VDOF)<"+str(self.getProp('UnbiasedBmmVertexChi2'))+")"}
                    , postscale = self.postscale
                    )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2DiMuonUnbiasedBmmDecision":   50203 } )
        
        #--------------------------------------------
        '''
        prescaled unbiased J/psi for low rate scenario
        '''
        line.clone( 'DiMuonUnbiasedJPsiLow'
                    , prescale = self.prescale
                    , Filter = { 'Code': "(ADMASS('J/psi(1S)')<"+str(self.getProp('UnbiasedJPsiMassWindow'))+"*MeV) "
                                 +"& (PT>"+str(self.getProp('UnbiasedJPsiPt'))+"*MeV) "
                                 +"& (MINTREE('mu-'==ABSID,PT)>"+str(self.getProp('UnbiasedJPsiMuPt'))+"*MeV) "
                                 +"& (VFASPF(VCHI2/VDOF)<"+str(self.getProp('UnbiasedJPsiVertexChi2'))+")" }
                    , postscale = self.postscale
                    )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2DiMuonUnbiasedJPsiLowDecision":  50204 } )
        
        #--------------------------------------------
        '''
        unbiased Zmm
        '''
        line.clone( 'DiMuonUnbiasedZmm'
                    , prescale = self.prescale 
                    , Filter = { 'Code': "(MM>"+str(self.getProp('UnbiasedZmmMinMass'))+"*MeV) "
                                 +"& (PT>"+str(self.getProp('UnbiasedZmmPt'))+"*MeV) " }
                    , postscale = self.postscale
                    )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2DiMuonUnbiasedZmmDecision":   50205 } )
        #--------------------------------------------
        '''
        unbiased Drell-Yan 1
        '''
        line.clone( 'DiMuonDY1'
                    , prescale = self.prescale 
                    , Filter = { 'Code': "(MM>"+str(self.getProp('DY1MinMass'))+"*MeV) "\
                                 " & (MM<"+str(self.getProp('DY1MaxMass'))+"*MeV) "\
                                 "& (PT>"+str(self.getProp('DYPt'))+"*MeV) " }
                    , postscale = self.postscale
                    )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2DiMuonDY1Decision":   50206 } )

        #--------------------------------------------
        '''
        unbiased Drell-Yan 2
        '''
        line.clone( 'DiMuonDY2'
                    , prescale = self.prescale 
                    , Filter = { 'Code': "(MM>"+str(self.getProp('DY2MinMass'))+"*MeV) "\
                                 " & (MM<"+str(self.getProp('DY2MaxMass'))+"*MeV) "\
                                 "& (PT>"+str(self.getProp('DYPt'))+"*MeV) " }
                    , postscale = self.postscale
                    )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2DiMuonDY2Decision":   50207 } )

        #----------------------------------------------------------------------------------------

        '''
        unbiased Drell-Yan 3
        '''
        line.clone( 'DiMuonDY3'
                    , prescale = self.prescale 
                    , Filter = { 'Code': "(MM>"+str(self.getProp('DY3MinMass'))+"*MeV) "\
                                 " & (MM<"+str(self.getProp('DY3MaxMass'))+"*MeV) "\
                                 "& (PT>"+str(self.getProp('DYPt'))+"*MeV) " }
                    , postscale = self.postscale
                    )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2DiMuonDY3Decision":   50208 } )

        #--------------------------------------------
        '''
        unbiased Drell-Yan 4
        '''
        line.clone( 'DiMuonDY4'
                    , prescale = self.prescale 
                    , Filter = { 'Code': "(MM>"+str(self.getProp('DY4MinMass'))+"*MeV) "\
                                 " & (MM<"+str(self.getProp('DY4MaxMass'))+"*MeV) "\
                                 "& (PT>"+str(self.getProp('DYPt'))+"*MeV) " }
                    , postscale = self.postscale
                    )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2DiMuonDY4Decision":   50209 } )


        
        #----------------------------------------------------------------------------------------
        
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
        
        
        '''
           cut definitions for biased DiMuon Lines
        '''
        MuPtCut = "( MAXTREE(ABSID=='mu+',PT)>%(BiasedSingleMuonPt)s*MeV)" % self.getProps()
        MassCut = "(MM>%(BiasedMass)s*MeV)" % self.getProps()
        MassLCut = "(MM>%(BiasedLMass)s*MeV)" % self.getProps()
        MassTCut = "(MM>%(BiasedTMass)s*MeV)" % self.getProps()
        MuIPCut = "(2==NINTREE((ABSID=='mu+') & (MIPDV(PRIMARY)>%(BiasedSingleMuonIP)s*mm)))" % self.getProps()
        MuTIPCut = "(2==NINTREE((ABSID=='mu+') & (MIPDV(PRIMARY)>%(BiasedSingleMuonTIP)s*mm)))" % self.getProps()
        LTimeCut = "(BPVLTIME('PropertimeFitter/properTime:PUBLIC')>%(BiasedLTime)s*ps)" % self.getProps()
        LTimeTCut = "(BPVLTIME('PropertimeFitter/properTime:PUBLIC')>%(BiasedLTimeT)s*ps)" % self.getProps()
        IPChi2Cut = "( MAXTREE(ABSID=='mu+',MIPCHI2DV(PRIMARY))>%(BiasedSingleIPChi2)s)" % self.getProps()
        TIPChi2Cut = "( MAXTREE(ABSID=='mu+',MIPCHI2DV(PRIMARY))>%(BiasedSingleIPTChi2)s)" % self.getProps()
        VertexChi2Cut = "(VFASPF(VCHI2/VDOF)<%(BiasedVertexChi2)s)" % self.getProps()
        PVDistChi2Cut = "(BPVVDCHI2>%(BiasedPVDistanceChi2)s)" % self.getProps()
        PVDistTChi2Cut = "(BPVVDCHI2>%(BiasedPVDistanceTChi2)s)" % self.getProps()
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
        #--------------------------------------------
        '''
            robust biased DiMuon 
        '''
        line = Hlt2Line('BiasedDiMuonRobust'
                        , prescale = self.prescale 
                        , algos = [ PV3D, DiMuon, RobustDiMuon ]
                        , postscale = self.postscale
                        )

        #--------------------------------------------
        '''
            refined biased DiMuon
        '''
        line = Hlt2Line('BiasedDiMuonRefined'
                        , prescale = self.prescale 
                        , algos = [ PV3D, DiMuon, RobustDiMuon, RefinedDiMuon ]
                        , postscale = self.postscale
                        )

        #--------------------------------------------
        '''
            high mass biased DiMuon
        '''
        line.clone( 'BiasedDiMuonMass'
                    , prescale = self.prescale 
                    , RobustDiMuon = {"Code" : MuPtCut +"&"+ MassTCut +"&"+ MuIPCut +"&"+ LTimeTCut }
                    , postscale = self.postscale
                    )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2BiasedDiMuonMassDecision"    : 50042 } )
        
        
        #--------------------------------------------
        '''
            biased DiMuon with tigh IP cuts
        '''
        line.clone( 'BiasedDiMuonIP'
                    , prescale = self.prescale
                    , RobustDiMuon = {"Code" : MuPtCut +"&"+ MassLCut +"&"+ MuTIPCut +"&"+ LTimeCut }
                    , RefinedDiMuon = {"Code" : TIPChi2Cut +"&"+ VertexChi2Cut +"&"+ PVDistTChi2Cut }
                    , postscale = self.postscale
                    )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2BiasedDiMuonIPDecision"      : 50043 } )
        
        
