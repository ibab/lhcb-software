# $Id$
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $, version $Revision$
# =============================================================================
# $Log: not supported by cvs2svn $
# =============================================================================
## @file  LoKiCore/__init__.py
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
#  @date 2007-05-29
# =============================================================================
""" Helper file to manager LoKiCore package """
_author_ = "Vanya BELYAEV ibelyaev@physics.syr.edu"
# =============================================================================

try:
    from GaudiKernel import ROOT6WorkAroundEnabled
except ImportError:
    # dummy implementation
    def ROOT6WorkAroundEnabled(id=None):
        return False
if ROOT6WorkAroundEnabled('ROOT-5721'):
    try:
        import cppyy
    except ImportError:
        # FIXME: backward compatibility
        print "# WARNING: using PyCintex as cppyy implementation"
        import PyCintex as cppyy
        import sys
        sys.modules['cppyy'] = cppyy
    cppyy.gbl.gROOT.ProcessLine('#define ROOT_5721_WORKAROUND')

# =============================================================================
# The END
# =============================================================================
