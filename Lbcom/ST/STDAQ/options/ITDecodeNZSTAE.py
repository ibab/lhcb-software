# Decode the NZS bank for different spills (+/- 2 in TAE mode) for the TT.

from Gaudi.Configuration import *

from Configurables import ( STFullDecoding )

itFullDecodingPrev2 = STFullDecoding( 'ITFullDecodingPrev2' )
itFullDecodingPrev2.PrintErrorInfo   = 0
itFullDecodingPrev2.DetType   	= 'IT'
itFullDecodingPrev2.OutputLevel	= 4
itFullDecodingPrev2.InputLocation = '/Event/Prev2/DAQ/RawEvent'
itFullDecodingPrev2.OutputLocation = '/Event/Prev2/Raw/IT/Full'
itFullDecodingPrev2.EventInfoLocation = '/Event/Prev2/Raw/IT/EventInfo'

itFullDecodingPrev1 = STFullDecoding( 'ITFullDecodingPrev1' )
itFullDecodingPrev1.PrintErrorInfo   = 0
itFullDecodingPrev1.DetType   	= 'IT'
itFullDecodingPrev1.OutputLevel	= 4
itFullDecodingPrev1.InputLocation = '/Event/Prev1/DAQ/RawEvent'
itFullDecodingPrev1.OutputLocation = '/Event/Prev1/Raw/IT/Full'
itFullDecodingPrev1.EventInfoLocation = '/Event/Prev1/Raw/IT/EventInfo'
 
itFullDecodingCentral = STFullDecoding( 'ITFullDecodingCentral' )
itFullDecodingCentral.PrintErrorInfo   = 0
itFullDecodingCentral.DetType   	= 'IT'
itFullDecodingCentral.OutputLevel	= 4
itFullDecodingCentral.InputLocation = '/Event/DAQ/RawEvent'
itFullDecodingCentral.OutputLocation = '/Event/Raw/IT/Full'
itFullDecodingCentral.EventInfoLocation = '/Event/Raw/IT/EventInfo'

itFullDecodingNext1 = STFullDecoding( 'ITFullDecodingNext1' )
itFullDecodingNext1.PrintErrorInfo   = 0
itFullDecodingNext1.DetType   	= 'IT'
itFullDecodingNext1.OutputLevel	= 4
itFullDecodingNext1.InputLocation = '/Event/Next1/DAQ/RawEvent'
itFullDecodingNext1.OutputLocation = '/Event/Next1/Raw/IT/Full'
itFullDecodingNext1.EventInfoLocation = '/Event/Next1/Raw/IT/EventInfo'
 
itFullDecodingNext2 = STFullDecoding( 'ITFullDecodingNext2' )
itFullDecodingNext2.PrintErrorInfo   = 0
itFullDecodingNext2.DetType   	= 'IT'
itFullDecodingNext2.OutputLevel	= 4
itFullDecodingNext2.InputLocation = '/Event/Next2/DAQ/RawEvent'
itFullDecodingNext2.OutputLocation = '/Event/Next2/Raw/IT/Full'
itFullDecodingNext2.EventInfoLocation = '/Event/Next2/Raw/IT/EventInfo'

decodeSeq = GaudiSequencer( 'DecodeITNZSSeq' )
decodeSeq.MeasureTime = True
ApplicationMgr().TopAlg.append( decodeSeq )
decodeSeq.Members.append(itFullDecodingPrev2)
decodeSeq.Members.append(itFullDecodingPrev1)
decodeSeq.Members.append(itFullDecodingCentral)
decodeSeq.Members.append(itFullDecodingNext1)
decodeSeq.Members.append(itFullDecodingNext2)

