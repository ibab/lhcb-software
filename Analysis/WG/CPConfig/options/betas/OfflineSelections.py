class OfflineSelection( object ):
    def __init__( self, channel ):
        self._channel = channel
        self._angles = False
        self._tistos = { 'Hlt' : { 'single1'  : 'Hlt1SingleMuonNoIPL0Decision',
                                   'hlt1'     : 'Hlt1.*Decision',
                                   'hlt2'     : 'Hlt2.*Decision',
                                   'hlt2sel'  : 'Hlt2((.*DiMuon.*)|(TopoOSTF[234]Body)|(Single.*Muon)|(MuTrack)|(B2HH))Decision',
                                   'singleHighPt1' : 'Hlt1SingleMuonNoIPL0HighPTDecision',
                                   'di1'      : 'Hlt1DiMuonNoIP.*Decision',
                                   '1TrackMuon'    : 'Hlt1TrackMuonDecision',
                                   '1TrackAllL0'   : 'Hlt1TrackAllL0Decision',
                                   'ubdi2'    : 'Hlt2UnbiasedDiMuonDecision',
                                   'bdi2'     : 'Hlt2BiasedDiMuon.*Decision',
                                   'ubJpsi2'  : 'Hlt2DiMuonUnbiasedJPsiDecision',
                                   'bJpsi2'   : 'Hlt2DiMuonBiasedJPsiDecision',
                                   'mtNoIP2'  : 'Hlt2MuTrackNoIPDecision',
                                   'mt2'      : 'Hlt2MuTrackDecision',
                                   'topo2'    : 'Hlt2TopoOSTF2BodyDecision',
                                   'topo3'    : 'Hlt2TopoOSTF3BodyDecision',
                                   'physics1' : 'Hlt1(?!ODIN)(?!L0)(?!Lumi)(?!Tell1)(?!MB)(?!NZS)(?!Velo)(?!BeamGas)(?!Incident).*Decision'
                                   },
                         'L0' :  { 'L0'         : 'L0.*Decision',
                                   'L0Physics'  : 'L0(Hadron|Photon|Electron|Muon|DiMuon|MuonHigh)Decision',
                                   'L0Hadron'   : 'L0HadronDecision',
                                   'L0Photon'   : 'L0PhotonDecision',
                                   'L0Electron' : 'L0ElectronDecision',
                                   'L0Muon'     : 'L0MuonDecision',
                                   'L0MuonHigh' : 'L0MuonHighDecision',
                                   'L0DiMuon'   : 'L0DiMuonDecision'
                                   }
                         }

        self._extraVars = {}

        self._execStage = { 'single1' : 'Hlt1SingleMuonNoIPL0Decision' }

    def tistos( self ):
        return self._tistos
    
    def channel( self ):
        return self._channel

    def selection( self ):
        return self._selection

    def cut( self ):
        return self._cut

    def angles( self ):
        return self._angles

    def execStage( self ):
        return self._execStage

    def extraVars( self ):
        return self._extraVars

class Bu2JpsiKPreDet( OfflineSelection ):
    def __init__( self, selection = None ):
        OfflineSelection.__init__( self, "Bu2JpsiK" )
        
        self._extraVars = { "K_Pt"   : "CHILD( PT, 2 )",
                            "K_P"    : "CHILD( P, 2 )" }

        self._cut = "( ( '%(MotherName)s' == ABSID ) \
        & ( VFASPF(  VCHI2PDOF )                   < %(maxBChi2DoF)s  ) \
        & ( CHILD(   ADMASS( 'J/psi(1S)' ) , 1 )   < %(JpsiWindow)s   ) \
        & ( MINTREE( DLLmu, 'mu+' == ABSID )       > %(minDLLMu)s     ) \
        & ( CHILD(   PT, 2 )                       > %(minKPt)s       ) \
        & ( MINTREE( DLLK - DLLpi, 'K+' == ABSID ) > %(minDLLKpi)s    ) \
        & ( BPVLTFITCHI2('PropertimeFitter/ProperTime:PUBLIC') < %(LTFitChi2)s    ) )"

        self._selection = { "MotherName"        :  "B+",
                            "maxBChi2DoF"  :    16,
                            "LTFitChi2"    :     9,
                            "JpsiWindow"   :    48,
                            "minDLLMu"     :    -5,
                            "minKPt"       :  1300,
                            "minDLLKpi"    :    -5 }

        if selection:
            self._selection.update( selection )


class Bu2JpsiKUnbiased( OfflineSelection ):
    def __init__( self, selection = None ):
        OfflineSelection.__init__( self, "Bu2JpsiK" )
        
        self._extraVars = { "K_Pt"   : "CHILD( PT, 2 )",
                            "K_P"    : "CHILD( P, 2 )" }

        self._cut = "( ( '%(MotherName)s' == ABSID                                              )   \
        & ( in_range( %(minBMass)s * MeV, M, %(maxBMass)s * MeV )                          )   \
        & ( VFASPF( VCHI2PDOF )                                     < %(maxBVxChi2DoF)s    )   \
        & ( CHILD(   PT, 2 )                                        > %(minKPt)s           )   \
        & ( CHILD(   P, 2 )                                         > %(minKP)s            )   \
        & ( MINTREE( DLLK, 'K+' == ABSID )                          > %(minDLLKpi)s        )   \
        & ( MINTREE( DLLK - DLLp, 'K+' == ABSID )                   > %(minDLLKp)s         )   \
        & ( MAXTREE( TRCHI2DOF, 'K+' == ABSID )                     < %(maxTrChi2DoFK)s    )   \
        & ( MAXTREE( TRCHI2DOF, 'mu+' == ABSID )                    < %(maxTrChi2DoFMu)s   )   \
        & ( MINTREE( PT, 'mu+' == ABSID )                           > %(minMuPt)s          )   \
        & ( MINTREE( DLLmu, 'mu+' == ABSID )                        > %(minDLLMu)s         )   \
        & ( ( DTF_FUN( CHILD( ADMASS  ( 'J/psi(1S)' ), 1 ), False ) < %(JpsiWindow)s       )   \
          | ( DTF_FUN( CHILD( CHI2MASS( 'J/psi(1S)' ), 1 ), False ) < %(JpsiMassChi2)s     ) ) \
        & ( DTF_FUN( CHILD( VFASPF( VCHI2PDOF ) , 1 ), True )       < %(maxJpsiVxChi2DoF)s )   \
        & ( BPVIPCHI2( '' )                                         < %(maxBIPChi2)s       )   \
        & ( DTF_CHI2NDOF( True, 'J/psi(1S)' )                       < %(maxBDTFChi2DoF)s   ) )"
       
        self._selection = { "MotherName"            :  "B+",
                            "minBMass"         :  5100,
                            "maxBMass"         :  5450,
                            "maxBVxChi2DoF"    :    10,
                            "JpsiWindow"       :    80,
                            "JpsiMassChi2"     : 17.64,
                            "maxJpsiVxChi2DoF" :    11,
                            "maxTrChi2DoFMu"   :     4,
                            "minMuPt"          :   500,
                            "minDLLMu"         :     0,
                            "minKPt"           :  1000,
                            "minKP"            : 10000,
                            "minDLLKpi"        :     0,
                            "minDLLKp"         :    -2,
                            "maxTrChi2DoFK"    :     4,
                            "maxBIPChi2"       :    25,
                            "maxBDTFChi2DoF"   :     5 }
        
        if selection:
            self._selection.update( selection )

class Bs2JpsiPhiPreDet( OfflineSelection ):
    def __init__( self, selection = None ):
        OfflineSelection.__init__( self, "Bs2JpsiPhi" )
        self._angles = True
        self._angleTool = "Bs2JpsiPhiAngleCalculator"

        self._extraVars = { "phi_Mass" : "CHILD( M, 2 )",
                            "phi_Pt"   : "CHILD( PT, 2 )",
                            "phi_P"    : "CHILD( P, 2 )" }

        self._cut = "( ( '%(MotherName)s' == ABSID ) \
        & ( VFASPF(  VCHI2PDOF )                   < %(maxBChi2DoF)s  ) \
        & ( CHILD(   ADMASS( 'J/psi(1S)' ) , 1 )   < %(JpsiWindow)s   ) \
        & ( MINTREE( DLLmu, 'mu+' == ABSID )       > %(minDLLMu)s     ) \
        & ( CHILD(   PT, 2 )                       > %(minPhiPt)s     ) \
        & ( CHILD(   ADMASS( 'phi(1020)' ) , 2 )   < %(PhiWindow)s    ) \
        & ( MINTREE( DLLK - DLLpi, 'K+' == ABSID ) > %(minDLLKpi)s    ) \
        & ( DTF_CHI2NDOF( True ) < %(DTFChi2nDoF)s    ) )"

        self._selection = { "MotherName"        : "B_s0",
                            "maxBChi2DoF"  :    16,
                            "DTFChi2nDoF"  :     5,
                            "JpsiWindow"   :    80,
                            "minDLLMu"     :    -5,
                            "PhiWindow"    :    12,
                            "minPhiPt"     :  1100,
                            "minDLLKpi"    :    -5,
                            "maxIpChi2"    :    25 }

        if selection:
            self._selection.update( selection )

    def angleTool( self ):
        return self._angleTool


class Bs2JpsiPhiUnbiased( OfflineSelection ):
    def __init__( self, selection = None ):
        OfflineSelection.__init__( self, "Bs2JpsiPhi" )
        self._angles = True
        self._angleTool = "Bs2JpsiPhiAngleCalculator"

        self._extraVars = { "phi_Mass" : "CHILD( M, 2 )",
                            "phi_Pt"   : "CHILD( PT, 2 )",
                            "phi_P"    : "CHILD( P, 2 )" }

        self._cut = """( ( '%(MotherName)s' == ABSID ) \
        & ( in_range( %(minBMass)s * MeV, M, %(maxBMass)s * MeV )                          )   \
        & ( VFASPF(  VCHI2PDOF )                                    < %(maxBVxChi2DoF)s    )   \
        & ( BPVIPCHI2( '' )                                         < %(maxBIPChi2)s       )   \
        & ( MAXTREE( TRCHI2DOF, 'mu+' == ABSID )                    < %(maxTrChi2DoFMu)s   )   \
        & ( MINTREE( PT, 'mu+' == ABSID )                           > %(minMuPt)s          )   \
        & ( MINTREE( DLLmu, 'mu+' == ABSID )                        > %(minDLLMu)s         )   \
        & ( ( DTF_FUN( CHILD( ADMASS  ( 'J/psi(1S)' ), 1 ), False ) < %(JpsiWindow)s       )   \
        | ( DTF_FUN( CHILD( CHI2MASS( 'J/psi(1S)' ), 1 ), False ) < %(JpsiMassChi2)s     ) )   \
        & ( DTF_FUN( CHILD( VFASPF( VCHI2PDOF ) , 1 ), True )       < %(maxJpsiVxChi2DoF)s )   \
        & ( CHILD(   ADMASS( 'phi(1020)' ) , 2 )                    < %(PhiWindow)s        )   \
        & ( CHILD(   PT, 2 )                                        > %(minPhiPt)s         )   \
        & ( CHILD(   VFASPF( VCHI2PDOF ), 2 )                       < %(maxPhiVxChi2DoF)s  )   \
        & ( MINTREE( DLLK, 'K+' == ABSID )                          > %(minDLLK)s          )   \
        & ( MAXTREE( TRCHI2DOF, 'K+' == ABSID )                     < %(maxTrChi2DoFK)s    )   \
        & ( DTF_CHI2NDOF( True, 'J/psi(1S)' )                       < %(maxBDTFChi2DoF)s   ) )"""

        self._selection = { "MotherName"            : "B_s0",
                            "minBMass"         :   5200,
                            "maxBMass"         :   5550,
                            "maxBVxChi2DoF"    :      5,
                            "maxBIPChi2"       :     25,
                            "maxTrChi2DoFMu"   :      4,
                            "minMuPt"          :    500,
                            "minDLLMu"         :      0,
                            "JpsiWindow"       :     48,
                            "JpsiMassChi2"     :  17.64,
                            "maxJpsiVxChi2DoF" :     11,
                            "PhiWindow"        :     12,
                            "minPhiPt"         :   1000,
                            "maxPhiVxChi2DoF"  :     16,
                            "minDLLK"          :      0,
                            "maxTrChi2DoFK"    :      4,
                            "maxBDTFChi2DoF"   :      5 }

        if selection:
            self._selection.update( selection )

    def angleTool( self ):
        return self._angleTool

class Bd2JpsiKstarPreDet( OfflineSelection ):
    def __init__( self, selection = None ):
        OfflineSelection.__init__( self, "Bd2JpsiKstar" )
        self._angles = True
        self._angleTool = "Bd2KstarMuMuAngleCalculator"

        self._extraVars = { "Kstar_Mass" : "CHILD( M, 2 )",
                            "Kstar_Pt"   : "CHILD( PT, 2 )",
                            "Kstar_P"    : "CHILD( P, 2 )" }

        self._cut = "( ( '%(MotherName)s' == ABSID ) \
        & ( VFASPF(  VCHI2PDOF )                   < %(maxBChi2DoF)s ) \
        & ( CHILD(   ADMASS( 'J/psi(1S)' ) , 1 )   < %(JpsiWindow)s   ) \
        & ( MINTREE( DLLmu, 'mu+' == ABSID )       > %(minDLLMu)s     ) \
        & ( CHILD(   PT, 2 )                       > %(minKstarPt)s     ) \
        & ( CHILD(   ADMASS( 'K*(892)0' ), 2 )      < %(KstarWindow)s    ) \
        & ( MINTREE( DLLK - DLLpi, 'K+' == ABSID ) > %(minDLLKpi)s    ) \
        & ( MAXTREE( TRCHI2DOF, ( 'K+' == ABSID ) | ( 'pi+' == ABSID ) )    < %(maxTrChi2DoFKpi)s ) \
        & ( BPVLTFITCHI2('PropertimeFitter/ProperTime:PUBLIC') < %(LTFitChi2)s    ) )"

        self._selection = { "MotherName"        :  "B0",
                            "maxBChi2DoF"  :    16,
                            "LTFitChi2"    :     9,
                            "JpsiWindow"   :    48,
                            "minDLLMu"     :    -5,
                            "KstarWindow"  :    70,
                            "minKstarPt"   :  1000,
                            "minDLLKpi"    :    -5,
                            "maxTrChi2DoFKpi" : 10 }

        if selection:
            self._selection.update( selection )

    def angleTool( self ):
        return self._angleTool

class Bd2JpsiKstarUnbiased( OfflineSelection ):
    def __init__( self, selection = None ):
        OfflineSelection.__init__( self, "Bd2JpsiKstar" )
        self._angles = True
        self._angleTool = "Bd2KstarMuMuAngleCalculator"

        self._extraVars = { "Kstar_Mass" : "CHILD( M, 2 )",
                            "Kstar_Pt"   : "CHILD( PT, 2 )",
                            "Kstar_P"    : "CHILD( P, 2 )" }

        self._cut = "( ( '%(MotherName)s' == ABSID )                                                      \
        & ( in_range( %(minBMass)s * MeV, M, %(maxBMass)s * MeV )                                )   \
        & ( PT                                                           > %(minBPt)s            )   \
        & ( VFASPF( VCHI2PDOF )                                          < %(maxBVxChi2DoF)s     )   \
        & ( MAXTREE( TRCHI2DOF, 'mu+' == ABSID )                         < %(maxTrChi2DoFMu)s    )   \
        & ( MINTREE( PT, 'mu+' == ABSID )                                > %(minMuPt)s           )   \
        & ( MINTREE( DLLmu, 'mu+' == ABSID )                             > %(minDLLMu)s          )   \
        & ( ( DTF_FUN( CHILD( ADMASS  ( 'J/psi(1S)' ), 1 ), False )      < %(JpsiWindow)s        )   \
          | ( DTF_FUN( CHILD( CHI2MASS( 'J/psi(1S)' ), 1 ), False )      < %(JpsiMassChi2)s      ) ) \
        & ( DTF_FUN( CHILD( VFASPF( VCHI2PDOF ) , 1 ), True )            < %(maxJpsiVxChi2DoF)s  )   \
        & ( CHILD(   ADMASS(  'K*(892)0' ), 2 )                          < %(KstarWindow)s       )   \
        & ( CHILD(   PT, 2 )                                             > %(minKstarPt)s        )   \
        & ( CHILD(   VFASPF( VCHI2PDOF ) , 2 )                           < %(maxKstarVxChi2DoF)s )   \
        & ( MINTREE( DLLK - DLLpi, 'K+' == ABSID )                       > %(minDLLKpi)s         )   \
        & ( MINTREE( DLLK - DLLp, 'K+' == ABSID )                        > %(minDLLKp)s          )   \
        & ( MAXTREE( TRCHI2DOF, ( 'K+' == ABSID ) | ( 'pi+' == ABSID ) ) < %(maxTrChi2DoFKpi)s   )   \
        & ( BPVIPCHI2( '' )                                              < %(maxBIPChi2)s        )   \
        & ( DTF_CHI2NDOF( True, 'J/psi(1S)' )                            < %(maxBDTFChi2DoF)s    ) )"
        
        self._selection = { "MotherName"             :  "B0",
                            "minBMass"          :  5100,
                            "maxBMass"          :  5450,
                            "minBPt"            :  1000,
                            "maxBVxChi2DoF"     :    10,
                            "JpsiWindow"        :    80,
                            "JpsiMassChi2"      : 17.64,
                            "maxJpsiVxChi2DoF"  :    11,
                            "minMuPt"           :   500,
                            "maxTrChi2DoFMu"    :     4,
                            "minDLLMu"          :     0,
                            "KstarWindow"       :    70,
                            "maxKstarVxChi2DoF" :    16,
                            "minKstarPt"        :  1000,
                            "minDLLKpi"         :     0,
                            "minDLLKp"          :    -2,
                            "maxTrChi2DoFKpi"   :     4,
                            "maxBIPChi2"        :    25,
                            "maxBDTFChi2DoF"    :     5 }

        if selection:
            self._selection.update( selection )

    def angleTool( self ):
        return self._angleTool

class Bd2JpsiKsUnbiased( OfflineSelection ):
    def __init__( self, selection = None ):
        OfflineSelection.__init__( self, "Bd2JpsiKs" )
        self._angles = True

        self._cut = """( ( '%(MotherName)s' == ABSID ) \
        & ( in_range( %(minBMass)s * MeV, M, %(maxBMass)s * MeV )                                ) \
        & ( VFASPF(  VCHI2PDOF )                                    < %(maxBVxChi2DoF)s          ) \
        & ( BPVIPCHI2( '' )                                         < %(maxBIPChi2)s             ) \
        & ( MAXTREE( TRCHI2DOF, 'mu+' == ABSID )                    < %(maxTrChi2DoFMu)s         ) \
        & ( MINTREE( PT, 'mu+' == ABSID )                           > %(minMuPt)s                ) \
        & ( MINTREE( DLLmu, 'mu+' == ABSID )                        > %(minDLLMu)s               ) \
        & ( ( DTF_FUN( CHILD( ADMASS  ( 'J/psi(1S)' ), 1 ), False ) < %(JpsiWindow)s             ) \
          | ( DTF_FUN( CHILD( CHI2MASS( 'J/psi(1S)' ), 1 ), False ) < %(JpsiMassChi2)s         ) ) \
        & ( DTF_FUN( CHILD( VFASPF( VCHI2PDOF ) , 1 ), True )       < %(maxJpsiVxChi2DoF)s       ) \
        & ( ( INTREE( ( 'pi+' == ABSID ) & ISLONG )                                                \
            & ( MINTREE( BPVIPCHI2( '' ), 'pi+' == ABSID )          > %(minLIPChi2)s )             \
            & ( CHILD( ADMASS( 'KS0' ) , 2 )                        < %(KSLLWindow)s )         )   \
          | ( INTREE( ( 'pi+' == ABSID ) & ISDOWN )                                                \
            & ( MINTREE( BPVIPCHI2( '' ), 'pi+' == ABSID )          > %(minDIPChi2)s )             \
            & ( CHILD( ADMASS( 'KS0' ) , 2 )                        < %(KSDDWindow)s )         ) ) \
        & ( CHILD(   PT, 2 )                                        > %(minKSPt)s                ) \
        & ( CHILD(   BPVDLS, 2 )                                    > %(minKSDLS)s               ) \
        & ( CHILD(   BPVLTCHI2(), 2 )                               > %(minKSLTChi2)s            ) \
        & ( CHILD(   VFASPF( VCHI2PDOF ), 2 )                       < %(maxKSVxChi2DoF)s         ) \
        & ( MINTREE( P, 'pi+' == ABSID )                            > %(minPiP)s                 ) \
        & ( MAXTREE( TRCHI2DOF, 'pi+' == ABSID )                    < %(maxTrChi2DoFPi)s         ) \
        & ( DTF_CHI2NDOF( True, 'J/psi(1S)' )                       < %(maxBDTFChi2DoF)s         ) )"""

        self._selection = { "MotherName"       :  "B0",
                            "minBMass"         :  5150,
                            "maxBMass"         :  5400,
                            "maxBVxChi2DoF"    :    10,
                            "maxBIPChi2"       :    20,
                            "maxTrChi2DoFMu"   :     4,
                            "minMuPt"          :   500,
                            "minDLLMu"         :     0,
                            "JpsiWindow"       :    48,
                            "JpsiMassChi2"     : 17.64,
                            "maxJpsiVxChi2DoF" :    11,
                            "KSLLWindow"       :    12,
                            "KSDDWindow"       :    21,
                            "minLIPChi2"       :     9,
                            "minDIPChi2"       :     4,
                            "minKSPt"          :  1000,
                            "minKSDLS"         :     5,
                            "minKSLTChi2"      :    25,
                            "maxKSVxChi2DoF"   :    20,
                            "maxTrChi2DoFPi"   :     4,
                            "minPiP"           :  2000,
                            "maxBDTFChi2DoF"   :     5 }

        if selection:
            self._selection.update( selection )

class Lambdab2JpsiLambdaUnbiased( OfflineSelection ):
    def __init__( self, selection = None ):
        OfflineSelection.__init__( self, "Bd2JpsiKs" )
        self._angles = True

        self._cut = """( ( '%(MotherName)s' == ABSID )                                                       \
        & ( in_range( %(minLambdaBMass)s * MeV, M, %(maxLambdaBMass)s * MeV )                            )   \
        & ( VFASPF(  VCHI2PDOF )                                         < %(maxLambdaBVxChi2DoF)s       )   \
        & ( BPVIPCHI2( '' )                                              < %(maxLambdaBIPChi2)s          )   \
        & ( DTF_CHI2NDOF( True, 'J/psi(1S)' )                            < %(maxLambdaBDTFChi2DoF)s      )   \
        & ( MAXTREE( TRCHI2DOF, 'mu+' == ABSID )                         < %(maxTrChi2DoFMu)s            )   \
        & ( MINTREE( PT, 'mu+' == ABSID )                                > %(minMuPt)s                   )   \
        & ( MINTREE( DLLmu, 'mu+' == ABSID )                             > %(minDLLMu)s                  )   \
        & ( ( DTF_FUN( CHILD( ADMASS  ( 'J/psi(1S)' ), 2 ), False )      < %(JpsiWindow)s                )   \
          | ( DTF_FUN( CHILD( CHI2MASS( 'J/psi(1S)' ), 2 ), False )      < %(JpsiMassChi2)s              ) ) \
        & ( DTF_FUN( CHILD( VFASPF( VCHI2PDOF ) , 2 ), True )            < %(maxJpsiVxChi2DoF)s          )   \
        & ( CHILD(   PT, 1 )                                             > %(minLambdaPt)s               )   \
        & ( CHILD(   BPVLTCHI2(), 1 )                                    > %(minLambdaLTChi2)s           )   \
        & ( CHILD(   VFASPF( VCHI2PDOF ), 1 )                            < %(maxLambdaVxChi2DoF)s        )   \
        & ( MINTREE( P,  'pi+' == ABSID )                                > %(minPiP)s                    )   \
        & ( MINTREE( P,  'pi+' == ABSID )                                > %(minPiP)s                    )   \
        & ( MINTREE( PT, 'pi+' == ABSID )                                > %(minPiPt)s                   )   \
        & ( MINTREE( P,  'p+'  == ABSID )                                > %(minPP)s                     )   \
        & ( MINTREE( PT, 'p+'  == ABSID )                                > %(minPPt)s                    )   \
        & ( MAXTREE( TRCHI2DOF, ( 'pi+' == ABSID ) | ( 'p+' == ABSID ) ) < %(maxTrChi2DoFPPi)s           )   \
        & ( ( INTREE( ( 'pi+' == ABSID ) & ISLONG )                                                          \
            & ( MINTREE( BPVIPCHI2( '' ), ( 'pi+' == ABSID ) | ( 'p+' == ABSID ) ) > %(minLIPChi2)s      )   \
            & ( CHILD( ADMASS( 'Lambda0' ), 1 ) < %(LambdaLLWindow)s )                                   )   \
          | ( INTREE( ( 'pi+' == ABSID ) & ISDOWN )                                                          \
            & ( MINTREE( BPVIPCHI2( '' ), ( 'pi+' == ABSID ) | ( 'p+' == ABSID ) ) > %(minDIPChi2)s      )   \
            & ( CHILD( ADMASS( 'Lambda0' ), 1 ) < %(LambdaDDWindow)s )                                   ) ) )"""

        self._selection = { "MotherName"           : "Lambda_b0",
                            "minLambdaBMass"       :        5120,
                            "maxLambdaBMass"       :        6120,
                            "maxLambdaBVxChi2DoF"  :          10,
                            "maxLambdaBIPChi2"     :          20,
                            "maxTrChi2DoFMu"       :           4,
                            "minMuPt"              :         500,
                            "minDLLMu"             :           0,
                            "JpsiWindow"           :          48,
                            "JpsiMassChi2"         :       17.64,
                            "maxJpsiVxChi2DoF"     :          11,
                            "LambdaLLWindow"       :           6,
                            "LambdaDDWindow"       :           6,
                            "minLIPChi2"           :           9,
                            "minDIPChi2"           :           4,
                            "minLambdaPt"          :        1000,
                            "minLambdaLTChi2"      :          25,
                            "maxLambdaVxChi2DoF"   :          20,
                            "maxTrChi2DoFPPi"      :           4,
                            "minPiP"               :        2000,
                            "minPiPt"              :         100,
                            "minPP"                :        2000,
                            "minPPt"               :         500,
                            "maxLambdaBDTFChi2DoF" :           5 }

        if selection:
            self._selection.update( selection )
