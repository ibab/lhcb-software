from OfflineSelections import OfflineSelection

class Bu2JpsiKUnbiased( OfflineSelection ):
    def __init__( self, selection = None ):
        OfflineSelection.__init__( self, "Bu2JpsiK" )
        
        self._extraVars = { "K_Pt"   : "CHILD( PT, 2 )",
                            "K_P"    : "CHILD( P, 2 )" }

        self._cut = "( ( '%(MotherName)s' == ABSID                                       ) \
        & ( in_range( %(minBMass)s * MeV, M, %(maxBMass)s * MeV )                        ) \
        & ( MINTREE( DLLK, 'K+' == ABSID )                        > %(minDLLKpi)s        ) \
        & ( MINTREE( DLLK - DLLp, 'K+' == ABSID )                 > %(minDLLKp)s         ) \
        & ( MAXTREE( TRCHI2DOF, 'K+' == ABSID )                   < %(maxTrChi2DoFK)s    ) \
        & ( MAXTREE( TRCHI2DOF, 'mu+' == ABSID )                  < %(maxTrChi2DoFMu)s   ) \
        & ( MINTREE( PT, 'mu+' == ABSID )                         > %(minMuPt)s          ) \
        & ( DTF_CHI2NDOF( True )                                  < %(maxBDTFChi2DoF)s   ) )"
       
        self._selection = { "MotherName"       :  "B+",
                            "minBMass"         :  5100,
                            "maxBMass"         :  5450,
                            "maxTrChi2DoFMu"   :     4,
                            "minMuPt"          :   500,
                            "minKP"            : 10000,
                            "minDLLKpi"        :     0,
                            "minDLLKp"         :    -2,
                            "maxTrChi2DoFK"    :     4,
                            "maxBDTFChi2DoF"   :     5 }
        
        if selection:
            self._selection.update( selection )

class Bs2JpsiPhiUnbiased( OfflineSelection ):
    def __init__( self, selection = None ):
        OfflineSelection.__init__( self, "Bs2JpsiPhi" )
        self._angles = True
        self._angleTool = "Bs2JpsiPhiAngleCalculator"

        self._extraVars = { "phi_Mass" : "CHILD( M, 2 )",
                            "phi_Pt"   : "CHILD( PT, 2 )",
                            "phi_P"    : "CHILD( P, 2 )" }

        self._cut = "( ( '%(MotherName)s' == ABSID ) \
        & ( in_range( %(minBMass)s * MeV, M, %(maxBMass)s * MeV )                          ) \
        & ( MAXTREE( TRCHI2DOF, 'mu+' == ABSID )                    < %(maxTrChi2DoFMu)s   ) \
        & ( MAXTREE( TRCHI2DOF, 'K+' == ABSID )                     < %(maxTrChi2DoFK)s    ) \
        & ( MINTREE( PT, 'mu+' == ABSID )                           > %(minMuPt)s          ) \
        & ( MINTREE( DLLK, 'K+' == ABSID )                          > %(minDLLK)s          ) \
        & ( BPVLTIME()                                              > %(minBLTime)s        ) \
        & ( DTF_CHI2NDOF( True )                                    < %(maxBDTFChi2DoF)s   ) )"

        from GaudiKernel.SystemOfUnits import picosecond as ps
        self._selection = { "MotherName"       : "B_s0",
                            "minBMass"         :   5200,
                            "maxBMass"         :   5550,
                            "maxTrChi2DoFMu"   :      4,
                            "minMuPt"          :    500,
                            "minDLLK"          :      0,
                            "maxTrChi2DoFK"    :      4,
                            "minBLTime"        :      0.3 * ps,
                            "maxBDTFChi2DoF"   :      5 }

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

        self._cut = "( ( '%(MotherName)s' == ABSID )                                                 \
        & ( in_range( %(minBMass)s * MeV, M, %(maxBMass)s * MeV )                                )   \
        & ( MAXTREE( TRCHI2DOF, 'mu+' == ABSID )                         < %(maxTrChi2DoFMu)s    )   \
        & ( MAXTREE( TRCHI2DOF, ( 'K+' == ABSID ) | ( 'pi+' == ABSID ) ) < %(maxTrChi2DoFKpi)s   )   \
        & ( MINTREE( PT, 'mu+' == ABSID )                                > %(minMuPt)s           )   \
        & ( CHILD(   ADMASS(  'K*(892)0' ), 2 )                          < %(KstarWindow)s       )   \
        & ( MINTREE( DLLK,        'K+' == ABSID )                        > %(minDLLK)s           )   \
        & ( MINTREE( DLLK - DLLp, 'K+' == ABSID )                        > %(minDLLKp)s          )   \
        & ( DTF_CHI2NDOF( True )                                         < %(maxBDTFChi2DoF)s    ) )"
        
        self._selection = { "MotherName"        :  "B0",
                            "minBMass"          :  5100,
                            "maxBMass"          :  5450,
                            "minMuPt"           :   500,
                            "maxTrChi2DoFMu"    :     4,
                            "KstarWindow"       :    70,
                            "minDLLK"           :     0,
                            "minDLLKp"          :    -2,
                            "maxTrChi2DoFKpi"   :     4,
                            "maxBDTFChi2DoF"    :     5 }

        if selection:
            self._selection.update( selection )

    def angleTool( self ):
        return self._angleTool

class Bd2JpsiKsUnbiased( OfflineSelection ):
    def __init__( self, selection = None ):
        OfflineSelection.__init__( self, "Bd2JpsiKs" )
        self._angles = True

        self._cut = "( ( '%(MotherName)s' == ABSID ) \
        & ( in_range( %(minBMass)s * MeV, M, %(maxBMass)s * MeV )             ) \
        & ( MAXTREE( TRCHI2DOF, 'mu+' == ABSID ) < %(maxTrChi2DoFMu)s         ) \
        & ( MINTREE( PT, 'mu+' == ABSID )        > %(minMuPt)s                ) \
        & ( MAXTREE( TRCHI2DOF, 'pi+' == ABSID ) < %(maxTrChi2DoFPi)s         ) \
        & ( DTF_CHI2NDOF( True )                 < %(maxBDTFChi2DoF)s         ) )"

        self._selection = { "MotherName"       :  "B0",
                            "minBMass"         :  5150,
                            "maxBMass"         :  5400,
                            "maxTrChi2DoFMu"   :     4,
                            "minMuPt"          :   500,
                            "maxTrChi2DoFPi"   :     4,
                            "maxBDTFChi2DoF"   :     5 }

        if selection:
            self._selection.update( selection )

class Lambdab2JpsiLambdaUnbiased( OfflineSelection ):
    def __init__( self, selection = None ):
        OfflineSelection.__init__( self, "Bd2JpsiKs" )
        self._angles = True

        self._cut = "( ( '%(MotherName)s' == ABSID )                                                \
        & ( in_range( %(minLambdaBMass)s * MeV, M, %(maxLambdaBMass)s * MeV )                       ) \
        & ( DTF_CHI2NDOF( True )                                         < %(maxLambdaBDTFChi2DoF)s ) \
        & ( MAXTREE( TRCHI2DOF, 'mu+' == ABSID )                         < %(maxTrChi2DoFMu)s       ) \
        & ( MINTREE( PT, 'mu+' == ABSID )                                > %(minMuPt)s              ) \
        & ( MAXTREE( TRCHI2DOF, ( 'pi+' == ABSID ) | ( 'p+' == ABSID ) ) < %(maxTrChi2DoFPPi)s      ) )"

        self._selection = { "MotherName"           : "Lambda_b0",
                            "minLambdaBMass"       :        5120,
                            "maxLambdaBMass"       :        6120,
                            "maxTrChi2DoFMu"       :           4,
                            "minMuPt"              :         500,
                            "maxTrChi2DoFPPi"      :           4,
                            "maxLambdaBDTFChi2DoF" :           5 }

        if selection:
            self._selection.update( selection )
