import GaudiKernel.SystemOfUnits as units

class DisplVertices_25ns_August2015(object):
    """
    Threshold settings for Hlt2 DisplVertices lines

    WARNING :: DO NOT EDIT WITHOUT PERMISSION OF THE AUTHORS

    @author Pieter David
    @date 2015-07-20
    """

    def ActiveHlt2Lines(self) :
        """
        Returns a list of active lines
        """

        return list("Hlt2DisplVertices{0}".format(nm) for nm in [
              "SingleVeryHighFD" 
            , "SingleHighFD"     
            , "Single"           
            , "SingleHighMass"   
            , "SinglePS"         
            , "SingleLoosePS"    
            #, "SingleDown"       
            , "Double"           
            , "DoublePS"         
            ])

    def Thresholds(self) :
        """
        Returns a dictionary of cuts
        """
        
        from Hlt2Lines.DisplVertices.Lines import DisplVerticesLines
        return { DisplVerticesLines : DisplVerticesLines.fillInDefaults({
                  "Prescale"         : { "SinglePS"             : 0.01
                                       , "SingleLoosePS"        : 0.001
                                       , "DoublePS"             : 0.01 ## OFF FOR NOW
                                       }
                , "Postscale"        : {}
                , "Common"           : {}

                , "FilterVelo"       : { "Apply"                : True
                                       , "MinIP"                : 0.100*units.mm
                                       , "MinIPChi2"            : -1.0
                                       , "RejectSplash"         : False
                                       , "RejectBackward"       : True
                                       , "MinNumTracks"         : 4
                                       , "MaxNumInput"          : -1
                                       , "MaxNumOutput"         : -1
                                       }

                #===== DEFAULT CUT VALUES, fallback if no other specified =====#
                , "DefaultLLP"       : { "MinNumTracks"         : 4
                                       , "MinRho"               : 0.4   *units.mm
                                       , "MinMass"              : 3200.0*units.MeV
                                       , "MinSumPT"             : 1000.0*units.MeV
                                       , "MaxFractE1Track"      : 0.9
                                       , "MaxFractTrwHitBefore" : 0.49
                                       , "ApplyMatterVeto"      : True
                                       , "extra" : "", "extraSingle" : "", "extraComb" : ""
                                       }
                ## Velo reconstruction
                , "RV2PVelo"         : { "MinSumPT"             : 0.0   *units.MeV
                                       , "MinMass"              : 0.0   *units.MeV
                                       , "ApplyMatterVeto"      : False
                                       ## Reco only
                                       , "MaxChi2NonVeloOnly"   : 5.0
                                       , "UseVeloTracks"        : False
                                       , "ComputeMatterVeto"    : True
                                       }
                ## Single LLP lines
                , "SingleVeryHighFD" : { "MinNumTracks"         : 4     # 2-3 Hz
                                       , "MinRho"               : 5.0   *units.mm
                                       , "MinMass"              : 2000.0*units.MeV
                                       }
                , "SingleHighFD"     : { "MinNumTracks"         : 5     # 4-5 Hz
                                       , "MinRho"               : 3.0   *units.mm
                                       , "MinMass"              : 2800.0*units.MeV
                                       }
                , "Single"           : { "MinNumTracks"         : 4     # 4 Hz
                                       , "MinRho"               : 1.5   *units.mm
                                       , "MinMass"              : 5000.0*units.MeV
                                       }
                , "SingleHighMass"   : { "MinNumTracks"         : 5     # 5 Hz
                                       , "MinRho"               : 0.5   *units.mm
                                       , "MinMass"              : 8000.0*units.MeV
                                       }
                # prescaled
                , "SinglePS"         : { "MinNumTracks"         : 4
                                       , "MinRho"               : 1.5   *units.mm
                                       , "MinMass"              : 2500.0*units.MeV
                                       , "MinSumPT"             :    0.0*units.MeV
                                       }
                , "SingleLoosePS"    : { "MinNumTracks"         : 4
                                       , "MinRho"               : 0.4   *units.mm
                                       , "MinMass"              : 0.0   *units.MeV
                                       , "MinSumPT"             : 0.0   *units.MeV
                                       , "ApplyMatterVeto"      : False
                                       }

                ## Double LLP line
                , "Double"           : { "ApplyMatterVeto"      : False
                                       , "MinMass"              : 2000.0*units.MeV
                                       ## Double only
                                       , "MinHighestMass"       : 2800.0*units.MeV
                                       , "ApplyMatterVetoOne"   : True
                                       }
                , "DoublePS"         : { "PreScale"             : 0.01
                                       , "ApplyMatterVeto"      : False
                                       , "MinMass"              : 2000.0*units.MeV
                                       , "MinSumPT"             :    0.0*units.MeV
                                       ## Double only
                                       , "MinHighestMass"       : 2000.0*units.MeV
                                       , "ApplyMatterVetoOne"   : False
                                       }


                ## Downstream preselection
                , "PreSelDown"       : { "MinNumTracks"         : 5
                                       , "MinRho"               : 0.4   *units.mm
                                       , "MinMass"              : 2800.0*units.MeV
                                       }
                ## Downstream reconstruction
                , "RV2PDown"         : { "MinNumTracks"         : 5
                                       , "MinMass"              : 2800.0*units.MeV
                                       , "ApplyMatterVeto"      : False
                                       ## Reco only
                                       , "MaxChi2NonVeloOnly"   : 5.0
                                       , "UseVeloTracks"        : False
                                       , "ComputeMatterVeto"    : False
                                       }
                ## Single downstream line
                , "SingleDown"       : { "MinNumTracks"         : 5
                                       , "MinRho"               : 2.0   *units.mm
                                       , "MinMass"              : 3000.0*units.MeV
                                       , "ApplyMatterVeto"      : False
                                       }

                }) }
