# Decode the NZS bank for different spills (+/- 2 in TAE mode) for the TT.

from Gaudi.Configuration import *

from Configurables import ( STFullDecoding )

ttFullDecodingPrev2 = STFullDecoding( 'TTFullDecodingPrev2' )
ttFullDecodingPrev2.PrintErrorInfo   = 0
ttFullDecodingPrev2.DetType   	= 'TT'
ttFullDecodingPrev2.OutputLevel	= 4
ttFullDecodingPrev2.InputLocation = '/Event/Prev2/DAQ/RawEvent'
ttFullDecodingPrev2.OutputLocation = '/Event/Prev2/Raw/TT/Full'
ttFullDecodingPrev2.EventInfoLocation = '/Event/Prev2/Raw/TT/EventInfo'

ttFullDecodingPrev1 = STFullDecoding( 'TTFullDecodingPrev1' )
ttFullDecodingPrev1.PrintErrorInfo   = 0
ttFullDecodingPrev1.DetType   	= 'TT'
ttFullDecodingPrev1.OutputLevel	= 4
ttFullDecodingPrev1.InputLocation = '/Event/Prev1/DAQ/RawEvent'
ttFullDecodingPrev1.OutputLocation = '/Event/Prev1/Raw/TT/Full'
ttFullDecodingPrev1.EventInfoLocation = '/Event/Prev1/Raw/TT/EventInfo'
 
ttFullDecodingCentral = STFullDecoding( 'TTFullDecodingCentral' )
ttFullDecodingCentral.PrintErrorInfo   = 0
ttFullDecodingCentral.DetType   	= 'TT'
ttFullDecodingCentral.OutputLevel	= 4
ttFullDecodingCentral.InputLocation = '/Event/DAQ/RawEvent'
ttFullDecodingCentral.OutputLocation = '/Event/Raw/TT/Full'
ttFullDecodingCentral.EventInfoLocation = '/Event/Raw/TT/EventInfo'

ttFullDecodingNext1 = STFullDecoding( 'TTFullDecodingNext1' )
ttFullDecodingNext1.PrintErrorInfo   = 0
ttFullDecodingNext1.DetType   	= 'TT'
ttFullDecodingNext1.OutputLevel	= 4
ttFullDecodingNext1.InputLocation = '/Event/Next1/DAQ/RawEvent'
ttFullDecodingNext1.OutputLocation = '/Event/Next1/Raw/TT/Full'
ttFullDecodingNext1.EventInfoLocation = '/Event/Next1/Raw/TT/EventInfo'
 
ttFullDecodingNext2 = STFullDecoding( 'TTFullDecodingNext2' )
ttFullDecodingNext2.PrintErrorInfo   = 0
ttFullDecodingNext2.DetType   	= 'TT'
ttFullDecodingNext2.OutputLevel	= 4
ttFullDecodingNext2.InputLocation = '/Event/Next2/DAQ/RawEvent'
ttFullDecodingNext2.OutputLocation = '/Event/Next2/Raw/TT/Full'
ttFullDecodingNext2.EventInfoLocation = '/Event/Next2/Raw/TT/EventInfo'

decodeSeq = GaudiSequencer( 'DecodeTTNZSSeq' )
decodeSeq.MeasureTime = True
ApplicationMgr().TopAlg.append( decodeSeq )
decodeSeq.Members.append(ttFullDecodingPrev2)
decodeSeq.Members.append(ttFullDecodingPrev1)
decodeSeq.Members.append(ttFullDecodingCentral)
decodeSeq.Members.append(ttFullDecodingNext1)
decodeSeq.Members.append(ttFullDecodingNext2)

