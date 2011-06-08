"""Test validisy of CommonParticles.StandardIntermediate cuts.

   syntax: gaudirun.py $DAVINCIMONITORSROOT/options/TestStandardIntermediateCuts.py

   Author: Juan Palacios <palacios@physik.uzh.ch>

"""
##############################################################################
from DaVinci.Configuration import DaVinci
from Gaudi.Configuration import *
#
from CommonParticles import StandardBasic
from CommonParticles import StandardIntermediate

List = []

for a,b in StandardIntermediate.locations.iteritems():
    print "DVTestCommonParticles adding location", a
    List.append(b)                      # ist of algorithms

DaVinci( UserAlgorithms = List,
         EvtMax = 1,
         DataType = "2010",
         Simulation = False,
         InputType = "DST"       )

#importOptions("../dimuon_testfile.py")

MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"
