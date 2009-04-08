#  Options to run pp collisions as in Startup with 5 TeV
#
#  For beam size see LHC-OP-ES-0011
#  For beam crossing angles (set based on Magnet see LHC Project Note 419)

from Gauss.Configuration import *

#--Set beam energy
importOptions("$GAUSSOPTS/BeamCond-5TeV.opts")

#--Luminosity
Generation().addTool(FixedLuminosity,name="FixedLuminosity")
Generation().FixedLuminosity.Luminosity = 1.23*(10**31)/(SystemOfUnits.cm2*SystemOfUnits.s)
Generation().FixedLuminosity.CrossingRate = 1.75*SystemOfUnits.megahertz
Generation().FixedLuminosity.TotalXSection = 97*SystemOfUnits.millibarn
Generation().PileUpTool = "FixedLuminosity"

#--Luminous region for colliding beams and beam gas
Generation().addTool(BeamSpotSmearVertex,name="BeamSpotSmearVertex")
Generation().BeamSpotSmearVertex.SigmaX = 0.046*SystemOfUnits.mm
Generation().BeamSpotSmearVertex.SigmaY = 0.046*SystemOfUnits.mm 

Generation().addTool(FlatZSmearVertex,name="FlatZSmearVertex")
Generation().FlatZSmearVertex.SigmaX = 0.065*SystemOfUnits.mm
Generation().FlatZSmearVertex.SigmaY = 0.065*SystemOfUnits.mm
