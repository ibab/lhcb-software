#
#   Dimuon stripping selections 
#
#  all B decay channels with at least 2 muons inside
#  (with minimal muon ID required)
#      - exclusive selections for: B->J/psi(mm)X, B->K*mumu, B->mumu
#      - J/psi(mm) inclusive for physics (with pre-scaling
#        with increasing Luminosity)
#            
#
#   @author A. Poluektov
#   @date 2009-05-15
#
from Gaudi.Configuration import *
from StrippingConf.StrippingStream import StrippingStream

from StrippingSelections import StrippingDiMuon
from StrippingSelections import StrippingBs2JpsiPhi
from StrippingSelections import StrippingBs2JpsiPhiLoose
from StrippingSelections import StrippingBs2JpsiPhiDetached
from StrippingSelections import StrippingBs2JpsiPhiPrescaled
from StrippingSelections import StrippingBd2JpsiKS
from StrippingSelections import StrippingBd2JpsiKstar
from StrippingSelections import StrippingBd2JpsiKstarLoose
from StrippingSelections import StrippingBd2JpsiKstarDetached
from StrippingSelections import StrippingBd2JpsiKstarDetachedR
from StrippingSelections import StrippingBs2Jpsif0
from StrippingSelections import StrippingBiasedBs2JpsiPhi
from StrippingSelections import StrippingBu2JpsiK
from StrippingSelections import StrippingBu2JpsiKLoose
from StrippingSelections import StrippingBu2JpsiKDetached
from StrippingSelections import StrippingBu2JpsiKDetachedR
from StrippingSelections.StrippingBu2LLK  import StrippingBu2LLKConf
from StrippingSelections.StrippingBd2KstarMuMu import StrippingBd2KstarMuMuConf

stream = StrippingStream("Dimuon")
stream.appendLines( [ 
		        StrippingBd2KstarMuMuConf().Early_SignalLine(),  
		        StrippingBd2KstarMuMuConf().Early_SameSignLine(),  
		        StrippingBd2KstarMuMuConf().Early_WideKstarLine(),  
		        StrippingBd2KstarMuMuConf().Early_NoMuIDLine(),  
		        StrippingBd2KstarMuMuConf().Early_eMuLine(),  
		    ] )
