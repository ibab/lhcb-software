"""The MuonKernel package describing the logical layout of the muon detector.

   The MuonKernel C++ librairy is wrapped with Boost.Python version 1_29_0.
   The librairy is break in 2 files, due to some limitations
   of the MSVC compiler.

   $Id: __init__.py,v 1.1 2005-02-21 09:45:36 atsareg Exp $
"""

__version__  = "$Name: not supported by cvs2svn $"
__revision__ = "$Revision: 1.1 $"
__date__     = "$Date: 2005-02-21 09:45:36 $"
__author__   = "$Author: atsareg $"

from libMuonKernelW import *
