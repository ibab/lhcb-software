## @file
#  Hlt2 lines for exotic displaced vertex signatures
#
#  Aimed at inclusively triggering on high-multiplicity (e.g. from hadronic
#  decays), high-mass (heavier than B hadrons) and highly displaced (>0.4mm in
#  the transverse plane) exotic long-lived particle decays.
#  The "Single" and "Double" lines use PatPV-based vertexing using all
#  reconstructed Velo tracks; a version starting from downstream tracks is being
#  worked on.
#
#  @author Victor Coco   Victor.Coco@cern.ch
#  @author Pieter David  Pieter.David@nikhef.nl
#================================================================================
""" Hlt2 lines for exotic displaced vertex signatures

Aimed at inclusively triggering on high-multiplicity (e.g. from hadronic decays),
high-mass (heavier than B hadrons) and highly displaced (>0.4mm in the transverse
plane) exotic long-lived particle decays.
The "Single" and "Double" lines use PatPV-based vertexing using all reconstructed
Velo tracks; a version starting from downstream tracks is being worked on.
"""
#================================================================================
__author__ = "Victor Coco Victor.Coco@cern.ch"
__author__ = "Pieter David pieter.david@nikhef.nl"

import GaudiKernel.SystemOfUnits as units

from Hlt2Lines.Utilities.Hlt2LinesConfigurableUser import Hlt2LinesConfigurableUser
class DisplVerticesLines(Hlt2LinesConfigurableUser) :
    __slots__ = { "Prescale"         : { "SinglePS"             : 0.01
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

                }

    def prescale(self, nickname):
        return self.Prescale.get(nickname, 1.)
    def postscale(self, nickname):
        return self.Postscale.get(nickname, 1.)

    @staticmethod
    def fillInDefaults(confDict):
        ## fill in defaults (to avoid copy-paste errors)
        from copy import deepcopy
        dct = deepcopy(confDict)
        for nick, cuts in dct.iteritems():
            if nick.startswith("RV2P") or nick.startswith("Single") or nick.startswith("Double"):
                for k, defaultVal in dct["DefaultLLP"].iteritems():
                    if k not in cuts:
                        cuts[k] = defaultVal

                if cuts.get("ApplyMatterVeto", False):
                    if nick.startswith("Double"):
                        cuts["extraSingle"] = " & ".join((cuts.get("extraSingle", ""), "(~INMATTER)"))
                    else:
                        cuts["extra"] = " & ".join((cuts.get("extra", ""), "(~INMATTER)"))
                elif cuts.get("ApplyMatterVetoOne", False):
                    cuts["extraComb"] = " & ".join((cuts.get("extraComb", ""), "AHASCHILD(~INMATTER)"))
        return dct

    def __apply_configuration__(self) :
        for k,v in DisplVerticesLines.fillInDefaults(self.__slots__).iteritems():
            setattr(self, k, v)

        from Stages import (SingleFilter,
                            #SingleDownFilter,
                            DoubleFilter)

        stages = { "SingleVeryHighFD" : [ SingleFilter("SingleVeryHighFD") ]
                 , "SingleHighFD"     : [ SingleFilter("SingleHighFD") ]
                 , "Single"           : [ SingleFilter("Single") ]
                 , "SingleHighMass"   : [ SingleFilter("SingleHighMass") ]
                 , "SinglePS"         : [ SingleFilter("SinglePS") ]
                 , "SingleLoosePS"    : [ SingleFilter("SingleLoosePS") ]
                #, "SingleDown"       : [ SingleDownFilter("SingleDown") ] ## TODO
                 , "Double"           : [ DoubleFilter("Double") ]
                 , "DoublePS"         : [ DoubleFilter("DoublePS") ]
                 }

        from HltLine.HltLine import Hlt2Line
        for (nickname, lineStages) in self.algorithms(stages).iteritems():
            linename = "DisplVertices{0}".format(nickname)
            Hlt2Line(linename, algos=lineStages, prescale=self.prescale(nickname), postscale=self.postscale(nickname))
