#
#   List of all available stripping streams 
#
#   @author A. Poluektov
#   @date 2009-12-04
#

from StrippingSelections import StreamBmuon, StreamHadron, StreamJpsi
from StrippingSelections import StreamDstar, StreamLambda, StreamBelectron
from StrippingSelections import StreamMiniBias

allStreams = [ StreamBmuon.stream,
               StreamHadron.stream, 
               StreamJpsi.stream,
               StreamDstar.stream,     
               StreamLambda.stream, 
               StreamBelectron.stream, 
               StreamMiniBias.stream
             ] 
