#!/usr/bin/env python

from bendermodule import *

gaudi.config( files = ['$BENDERTUTOROPTS/BenderTutor.opts'] )

gaudi.run(100)

gaudi.exit()
