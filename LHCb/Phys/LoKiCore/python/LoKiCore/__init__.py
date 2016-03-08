# $Id$
# =============================================================================
## @file  LoKiCore/__init__.py
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2007-05-29
#            Version $Revision$
#  Last modification $Date$
#                 by $Author$
# =============================================================================
""" Helper file to manager LoKiCore package """
_author_ = "Vanya BELYAEV Ivan.Belyaev@itep.ru"
# =============================================================================

try:
    from GaudiKernel import ROOT6WorkAroundEnabled
except ImportError:
    # dummy implementation
    def ROOT6WorkAroundEnabled(id=None):
        return False
if ROOT6WorkAroundEnabled('ROOT-5721'):
    import cppyy
    cppyy.gbl.gROOT.ProcessLine('#define ROOT_5721_WORKAROUND')

# =============================================================================
# The END
# =============================================================================
