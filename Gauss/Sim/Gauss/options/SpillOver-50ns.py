#--Set the bunch spacing to 50 ns, hence spill-over for the following slots
from Configurables import Gauss
Gauss().SpilloverPaths = [ 'PrevPrev', 'NextNext' ]
