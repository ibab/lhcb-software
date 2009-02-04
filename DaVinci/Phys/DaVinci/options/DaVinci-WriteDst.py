########################################################################
#
# $Id: DaVinci-WriteDst.py,v 1.1 2009-02-04 18:48:01 pkoppenb Exp $
#
# Options for a DaVinci job creating DSTs
#
# It looks for a J/psi and then saves the events in DSTs
# according to the number of PVs.
#
# @author Patrick Koppenburg
# @date 2009-02-03
#
########################################################################
from Configurables import DaVinci
##############################################################################
from Configurables import GaudiSequencer, CheckPV, FilterDesktop, PhysDesktop
#
# Get a J/psi
#
jpsi = FilterDesktop('MyJpsi')
jpsi.addTool(PhysDesktop())
jpsi.PhysDesktop.InputLocations = [ 'StdUnbiasedJpsi2MuMu' ]
jpsi.Code = "ALL"
#
# print it
#
from Configurables import PrintTree
pJpsi = PrintTree('PrintJpsi')
pJpsi.addTool(PhysDesktop())
pJpsi.PhysDesktop.InputLocations = [ 'MyJpsi' ]

#
# Loop over number of PVs
#
for i in range(1,6):
    nn = str(i)
    seq = GaudiSequencer(nn+"PVSeq")
    c = CheckPV(nn+"PV")
    c.MinPVs = i
    c.MaxPVs = i
    c.Print = True
    seq.Members += [ jpsi, c, pJpsi ] 
    DaVinci().DstFiles[ "PV"+nn+".dst" ] =  seq
DaVinci().OptItems = [ "/Event/Phys/MyJpsi#2" ]
##############################################################################
#
#
##############################################################################
DaVinci().EvtMax = 500
# DaVinci().SkipEvents = 0
DaVinci().DataType = "2008" # Default is "DC06"
# DaVinci().Simulation   = False
# DaVinci().Input = []
# DaVinci().MainOptions  = "$DAVINCIROOT/options/DaVinci.py" # Default is ""
##############################################################################
# DaVinci().DstFiles = { "Test.dst" : dstSeq }
