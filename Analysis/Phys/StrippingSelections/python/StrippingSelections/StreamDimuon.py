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
from StrippingSelections.StrippingBs2JpsiPhi import StrippingBs2JpsiPhiConf
from StrippingSelections import StrippingBs2JpsiPhiDetached
from StrippingSelections import StrippingBs2JpsiPhiPrescaled
from StrippingSelections import StrippingBd2JpsiKS
from StrippingSelections.StrippingBd2JpsiKstar import StrippingBd2JpsiKstarConf 
from StrippingSelections import StrippingBd2JpsiKstarDetached
from StrippingSelections import StrippingBd2JpsiKstarDetachedR
from StrippingSelections import StrippingBs2Jpsif0
from StrippingSelections import StrippingBiasedBs2JpsiPhi
from StrippingSelections.StrippingBu2JpsiK import StrippingBu2JpsiKConf
from StrippingSelections import StrippingBu2JpsiKDetached
from StrippingSelections import StrippingBu2JpsiKDetachedR
from StrippingSelections.StrippingBu2LLK  import StrippingBu2LLKConf
from StrippingSelections.StrippingBd2KstarMuMu import StrippingBd2KstarMuMuConf
from StrippingSelections.StrippingBc2JpsiMuX import StrippingBc2JpsiMuXConf
from StrippingSelections.StrippingBc2JpsiH import StrippingBc2JpsiHConf
from StrippingSelections.StrippingBc2JpsiHDetached import StrippingBc2JpsiHDetachedConf

stream = StrippingStream("Dimuon")
stream.appendLines( [ 
		        StrippingDiMuon.line, 
		        StrippingBs2JpsiPhiConf().nominal_line(),  
		        StrippingBs2JpsiPhiConf().loose_line(),  
		        StrippingBs2JpsiPhiPrescaled.line,  
			StrippingBs2JpsiPhiDetached.line, 
		        StrippingBd2JpsiKS.line1,  
		        StrippingBd2JpsiKS.line2,  
		        StrippingBd2JpsiKstarConf().nominal_line(),  
		        StrippingBd2JpsiKstarConf().loose_line(),  
		        StrippingBd2JpsiKstarDetached.line,  
		        StrippingBd2JpsiKstarDetachedR.line,  
		        StrippingBd2KstarMuMuConf().line_for_nominal_low(),  
		        StrippingBd2KstarMuMuConf().line_for_nominal_med(),  
		        StrippingBd2KstarMuMuConf().line_for_nominal_high(),  
		        StrippingBd2KstarMuMuConf().Early_SignalLine(),  
		        StrippingBd2KstarMuMuConf().Early_SameSignLine(),  
		        StrippingBd2KstarMuMuConf().Early_WideKstarLine(),  
		        StrippingBd2KstarMuMuConf().Early_NoMuIDLine(),  
		        StrippingBd2KstarMuMuConf().Early_eMuLine(),  
		        StrippingBs2Jpsif0.line,  
		        StrippingBiasedBs2JpsiPhi.line,  
		        StrippingBu2JpsiKConf().nominal_line(),  
		        StrippingBu2JpsiKConf().loose_line(),  
		        StrippingBu2JpsiKDetached.line,  
		        StrippingBu2JpsiKDetachedR.line,
		        StrippingBu2LLKConf().mmK()
                        StrippingBc2JpsiMuXConf().MuMu(),
                        StrippingBc2JpsiHConf().MuMu(),
                        StrippingBc2JpsiHDetachedConf(),MuMu()
		    ] )
