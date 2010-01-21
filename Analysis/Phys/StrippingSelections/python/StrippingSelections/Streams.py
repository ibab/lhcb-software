#
#   List of all available stripping streams 
#
#   @author A. Poluektov
#   @date 2009-12-04
#

from StrippingSelections import StreamBhadron
from StrippingSelections import StreamCalibration
from StrippingSelections import StreamCharm
from StrippingSelections import StreamDielectron
from StrippingSelections import StreamDimuon
from StrippingSelections import StreamMiniBias
from StrippingSelections import StreamSemileptonic
from StrippingSelections import StreamV0

allStreams = [ 
               StreamBhadron.stream,
               StreamCalibration.stream, 
               StreamCharm.stream,
               StreamDielectron.stream,     
               StreamDimuon.stream, 
               StreamMiniBias.stream, 
               StreamSemileptonic.stream, 
               StreamV0.stream
             ] 
