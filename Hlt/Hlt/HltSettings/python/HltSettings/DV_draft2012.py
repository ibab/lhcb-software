class DV_draft2012 :
    """
    Threshold settings for Hlt2 displaced vertex lines lines: June 2012 settings

    @author V. Coco, P. David
    @date 2012-06-18
    """

    __all__ = ( 'ActiveHlt2Lines' )


    def ActiveHlt2Lines(self) :
        """
        Returns a list of active lines
        """

        lines = [
            # The topological
            'Hlt2DisplVerticesSingleVeryHighFD',
            'Hlt2DisplVerticesSingleHighFD',
            'Hlt2DisplVerticesSingle',
            'Hlt2DisplVerticesSingleHighMass',
            'Hlt2DisplVerticesSinglePS',
            'Hlt2DisplVerticesSingleLoosePS',
            'Hlt2DisplVerticesDouble',
            'Hlt2DisplVerticesSingleDown',
            'Hlt2DisplVerticesDoublePS',
            ]

        return lines

    def Thresholds(self) :

        import GaudiKernel.SystemOfUnits as units

        d = { }

        from Hlt2Lines.Hlt2DisplVerticesLines import Hlt2DisplVerticesLinesConf

        d.update( { Hlt2DisplVerticesLinesConf :  {
            ## Velo tracks filter
              "FilterVelo"                : True
            , "VeloIP"                    : 0.100*units.mm
            , "VeloIPChi2"                : -1.0
            , "VeloRejectSplash"          : False
            , "VeloRejectBackward"        : True
            , "VeloMinNumTracks"          : 4
            , "VeloMaxNumInput"           : -1
            , "VeloMaxNumOutput"          : -1

            #========== DEFAULT CUT VALUES, fallback if no other specified ==========#
            , "DefaultLLP"                : { "PreScale"             : 1.0
                                            , "MinNumTracks"         : 4
                                            , "MinRho"               : 0.4   *units.mm
                                            , "MinMass"              : 3200.0*units.MeV
                                            , "MinSumPT"             : 1000.0*units.MeV ## TODO: check
                                            , "MaxFractE1Track"      : 0.9
                                            , "MaxFractTrwHitBefore" : 0.49
                                            , "ApplyMatterVeto"      : True
                                            }
            ## Velo reconstruction
            , "RV2PVelo"                  : { "MinSumPT"             : 0.0   *units.MeV
                                            , "MinMass"              : 0.0   *units.MeV
                                            , "ApplyMatterVeto"      : False
                                            ## Reco only
                                            , "MaxChi2NonVeloOnly"   : 5.0
                                            , "UseVeloTracks"        : False
                                            , "ComputeMatterVeto"    : True
                                            }
            ## Single LLP lines
            , "SingleVeryHighFDSelection" : { "MinNumTracks"         : 4     # 2-3 Hz
                                            , "MinRho"               : 5.0   *units.mm
                                            , "MinMass"              : 2000.0*units.MeV
                                            }
            , "SingleHighFDSelection"     : { "MinNumTracks"         : 5     # 4-5 Hz
                                            , "MinRho"               : 3.0   *units.mm
                                            , "MinMass"              : 2800.0*units.MeV
                                            }
            , "SingleSelection"           : { "MinNumTracks"         : 4     # 4 Hz
                                            , "MinRho"               : 1.5   *units.mm
                                            , "MinMass"              : 5000.0*units.MeV
                                            }
            , "SingleHighMassSelection"   : { "MinNumTracks"         : 5     # 5 Hz
                                            , "MinRho"               : 0.5   *units.mm
                                            , "MinMass"              : 8000.0*units.MeV
                                            }
            # prescaled
            , "SinglePSSelection"         : { "PreScale"             : 0.01  # 1 Hz
                                            , "MinNumTracks"         : 4
                                            , "MinRho"               : 1.5   *units.mm
                                            , "MinMass"              : 2500.0*units.MeV
                                            , "MinSumPT"             :    0.0*units.MeV
                                            }
            , "SingleLoosePSSelection"    : { "PreScale"             : 0.001 # 1-2 Hz
                                            , "MinNumTracks"         : 4
                                            , "MinRho"               : 0.4   *units.mm
                                            , "MinMass"              : 0.0   *units.MeV
                                            , "MinSumPT"             : 0.0   *units.MeV
                                            , "ApplyMatterVeto"      : False
                                            }

            ## Double LLP line
            , "DoubleSelection"           : { "ApplyMatterVeto"      : False
                                            , "MinMass"              : 2000.0*units.MeV
                                            ## Double only
                                            , "MinHighestMass"       : 2800.0*units.MeV
                                            , "ApplyMatterVetoOne"   : True
                                            }
            , "DoublePSSelection"         : { "PreScale"             : 0.01
                                            , "ApplyMatterVeto"      : False
                                            , "MinMass"              : 2000.0*units.MeV
                                            , "MinSumPT"             :    0.0*units.MeV
                                            ## Double only
                                            , "MinHighestMass"       : 2000.0*units.MeV
                                            , "ApplyMatterVetoOne"   : False
                                            }


            ## Downstream preselection
            , "PreSelDown"                : { "MinNumTracks"         : 5
                                            , "MinRho"               : 0.4   *units.mm
                                            , "MinMass"              : 2800.0*units.MeV
                                            }
            ## Downstream reconstruction
            , "RV2PDown"                  : { "MinNumTracks"         : 5
                                            , "MinMass"              : 2800.0*units.MeV
                                            , "ApplyMatterVeto"      : False
                                            ## Reco only
                                            , "MaxChi2NonVeloOnly"   : 5.0
                                            , "UseVeloTracks"        : False
                                            , "ComputeMatterVeto"    : False
                                            }
            ## Single downstream line
            , "SingleDownSelection"       : { "MinNumTracks"         : 5
                                            , "MinRho"               : 2.0   *units.mm
                                            , "MinMass"              : 3000.0*units.MeV
                                            , "ApplyMatterVeto"      : False
                                            }
        } } )

        return d
