#!/usr/bin/env python

from StrippingUtils import LineBuilderTests

##from GaudiKernel.SystemOfUnits import *
import GaudiKernel.SystemOfUnits as units

config_params = {
        ## Velo tracks filter
          "FilterVelo"              : { "Apply"                : True
                                      }

        #==========        SELECTION CUT VALUES         ==========#

        ## Velo reconstruction
        , "RV2PWithVelo"            : { "MinRho"               :  0.4*units.mm
                                      , "MinMass"              :  3.0*units.GeV
                                      , "MinSumPT"             :  3.0*units.GeV
                                      , "MinNumTracks"         :  4
                                      , "ApplyMatterVeto"      : False
                                      , "MaxFractE1Track"      :  0.9
                                      , "MaxFractTrwHitBefore" :  0.49
                                      ## Reco only
                                      , "MaxChi2NonVeloOnly"   : 5.0
                                      , "UseVeloTracks"        : False
                                      , "ComputeMatterVeto"    : False
                                      }
        ## Single LLP lines
        , "SingleHighMassSelection" : { "PreScale"             :  1.0
                                      , "MinRho"               :  1.5*units.mm
                                      , "MinMass"              : 15.0*units.GeV
                                      , "MinSumPT"             :  3.0*units.GeV
                                      , "MinNumTracks"         :  6
                                      , "ApplyMatterVeto"      : True
                                      , "MaxFractE1Track"      :  0.9
                                      , "MaxFractTrwHitBefore" :  0.49
                                      }
        , "SingleMediumSelection"   : { "PreScale"             :  1.0
                                      , "MinRho"               :  3.2*units.mm
                                      , "MinMass"              : 10.0*units.GeV
                                      , "MinSumPT"             :  3.0*units.GeV
                                      , "MinNumTracks"         :  6
                                      , "ApplyMatterVeto"      : True
                                      , "MaxFractE1Track"      :  0.9
                                      , "MaxFractTrwHitBefore" :  0.49
                                      }
        , "SingleHighFDSelection"   : { "PreScale"             :  1.0
                                      , "MinRho"               :  5.0*units.mm
                                      , "MinMass"              :  7.0*units.GeV
                                      , "MinSumPT"             :  3.0*units.GeV
                                      , "MinNumTracks"         :  7
                                      , "ApplyMatterVeto"      : True
                                      , "MaxFractE1Track"      :  0.9
                                      , "MaxFractTrwHitBefore" :  0.49
                                      }
        # prescaled
        , "SinglePSSelection"       : { "PreScale"             :  0.005
                                      , "MinRho"               :  0.4*units.mm
                                      , "MinMass"              :  3.0*units.GeV
                                      , "MinSumPT"             :  3.0*units.GeV
                                      , "MinNumTracks"         :  4
                                      , "ApplyMatterVeto"      : False
                                      , "MaxFractE1Track"      :  0.9
                                      , "MaxFractTrwHitBefore" :  0.49
                                      }

        ## Double LLP line
        , "DoubleSelection"         : { "PreScale"             :  1.0
                                      , "MinRho"               :  1.0*units.mm
                                      , "MinMass"              :  6.0*units.GeV
                                      , "MinSumPT"             :  3.0*units.GeV
                                      , "MinNumTracks"         :  6
                                      , "ApplyMatterVeto"      : False
                                      , "MaxFractE1Track"      :  0.9
                                      , "MaxFractTrwHitBefore" :  0.49
                                      ## Double only
                                      , "MinHighestMass"       : 7.0*units.GeV
                                      , "ApplyMatterVetoOne"   : True
                                      }

        ## Downstream reconstruction
        , "RV2PDown"                : { "MinRho"               :  3.0*units.mm
                                      , "MinMass"              :  4.5*units.GeV
                                      , "MinSumPT"             :  3.0*units.GeV
                                      , "MinNumTracks"         :  4
                                      , "ApplyMatterVeto"      : False
                                      , "MaxFractE1Track"      :  0.9
                                      , "MaxFractTrwHitBefore" :  0.49
                                      ## Reco only
                                      , "MaxChi2NonVeloOnly"   : 5.0
                                      , "UseVeloTracks"        : False
                                      , "ComputeMatterVeto"    : False
                                      }
        ## Single downstream line ### TODO implement MinZ = 150.0*mm
        , "SingleDownSelection"     : { "PreScale"             :  1.0
                                      , "MinRho"               :  3.0*units.mm
                                      , "MinMass"              :  4.5*units.GeV
                                      , "MinSumPT"             :  3.0*units.GeV
                                      , "MinNumTracks"         :  4
                                      , "ApplyMatterVeto"      : False
                                      , "MaxFractE1Track"      :  0.9
                                      , "MaxFractTrwHitBefore" :  0.49
                                      }

        #========== Other lines for efficiency studies ==========#
        , "SingleHLTPS"             : { "PreScale"             :  1.0 }
        , "DoubleHLTPS"             : { "PreScale"             :  1.0 }
        , "HltEffJPsiHLTSelection"  : { "PreScale"             :  1.0
                                      , "MinRho"               :  0.7*units.mm
                                      , "MinMass"              :  5.5*units.GeV
                                      , "MinSumPT"             :  3.0*units.GeV
                                      , "MinNumTracks"         :  6
                                      , "ApplyMatterVeto"      : False
                                      , "MaxFractE1Track"      :  0.9
                                      , "MaxFractTrwHitBefore" :  0.49
                                      }
        , "HltEffMinBiasSelection"  : { "PreScale"             :  1.0
                                      , "MinRho"               :  0.7*units.mm
                                      , "MinMass"              :  5.5*units.GeV
                                      , "MinSumPT"             :  3.0*units.GeV
                                      , "MinNumTracks"         :  4
                                      , "ApplyMatterVeto"      : False
                                      , "MaxFractE1Track"      :  0.9
                                      , "MaxFractTrwHitBefore" :  0.49
                                      }

        #==========     HLT filters for all lines      ==========#
        , "HLT"                     : { "JPsiHLT"     : "HLT_PASS('Hlt2DiMuonJPsiDecision')"
                                      , "MinBias"     : "HLT_PASS('Hlt1MBNoBiasDecision')"
                                      , "SingleHLTPS" : "HLT_PASS_RE('Hlt2DisplVertices(Single|SingleLoose)PSDecision')"
                                      , "DoubleHLTPS" : "HLT_PASS_RE('Hlt2DisplVerticesDoublePSDecision')"
                                      }
}


from StrippingSelections.StrippingDisplVertices import DisplVerticesLinesConf as builder

def test_line_builder() :
    LineBuilderTests.test_line_builder(builder, config_params)

if __name__ == '__main__' :
    
    test_line_builder()
