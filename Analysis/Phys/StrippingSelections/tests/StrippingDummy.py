#
#  Dummy stripping selection. Example of StrippingLine usage. 
#
#  @author A. Poluektov
#  @date 2009-05-14
#
#

from Gaudi.Configuration import *
from StrippingConf.StrippingLine import StrippingLine, StrippingMember
from Configurables import CombineParticles, FilterDesktop

#
# Create CombineParticles for Mu+Mu- combination. 
# This is not a StrippingMember and thus the new instance will not be created 
# during cloning of lines. 
#

c = CombineParticles ( 'StrippingDummyCombine' )
c.InputLocations = [ "StdLooseMuons" ]
c.DecayDescriptor = "J/psi(1S) -> mu+ mu-" 
c.MotherCut = "ALL"

# 
# Now create a StrippingMember that will filter mu+mu- combinations
#

f1 = StrippingMember( FilterDesktop
                   , "Filter"
                   , Code = "(PT > 1*GeV)"
                   , InputLocations = ['StrippingDummyCombine' ]
)

#
# Create a StrippingLine based on two previous selections. 
# The OutputSelection of this line will be named "StrippingDummy1Filter"
# (this will be written out to ETC. Should we better rename the last algorithm in a 
# sequence to "StrippingDummy1Decision"? )
# 

line1 = StrippingLine("Dummy1"
               , prescale = 1
               , algos = [ c, f1 ]
               , stream = 'dummy'
               )

#
# Clone the previous line. The FilterDesktop will be replaced, but CombineParticles will not 
# run for the second time. The OutputSelection will be "StrippingDummy2Filter"
#

line2 = line1.clone("Dummy2"
	       , prescale = 1
	       , FilterDesktopFilter = { "Code" : "(PT > 2*GeV)" }
               )

#
# Now do the same in a different way. First create a StrippingMember from FilterDesktop 
# with the new selection. But it will take the output of "Filter" as input locations 
# (note '%' syntax to indicate the output of the previous StrippingMember)
#

f2 = StrippingMember( FilterDesktop
                   , "Filter2"
                   , Code = "(PT > 2*GeV)"
                   , InputLocations = ["%Filter" ]
)

#
# Now create a StrippingLine with this StrippingMember. We need to make sure 
# that CombineParticles and first FilterDesktop are run. 
# The OutputSelection will be "StrippingDummy3Filter2"
#

line3 = StrippingLine("Dummy3"
	       , prescale = 1
	       , algos = [ c, f1, f2 ]
	       , stream = "dummy"
               )
