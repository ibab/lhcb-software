import ROOT

from .OnlineHistogram import OnlineHistogram

ROOT.gSystem.Load("libOMAlib.so")
ROOT.gSystem.Load("libOnlineHistDB.so")
ROOT.gInterpreter.Declare('#include "OMAlib/OMAlib.h"')
ROOT.gInterpreter.Declare('#include "OMAlib/OMAAlgorithms.h"')
ROOT.gInterpreter.Declare('#include "OnlineHistDB/OnlineHistDB.h"')
ROOT.gInterpreter.Declare('#include "OnlineHistDB/OnlineHistogram.h"')

# ROOT "typedefs"
StdString = ROOT.std.string
VectorOfOnlineHistograms = ROOT.std.vector('OnlineHistogram*')


class OnlineHistDB(object):
    """Interface to the HistogramDB."""
    @classmethod
    def _cast_TObject(cls, tobj):
        """Cast a generic TObject to its actual class.

        Raises an exception if the class is one that is not known to this
        method.
        """
        cls_map = {
            ROOT.TH1D.Class(): ROOT.TH1D,
            ROOT.TH2D.Class(): ROOT.TH2D,
            ROOT.TH3D.Class(): ROOT.TH3D,
            ROOT.TProfile.Class(): ROOT.TProfile
        }
        cls = cls_map.get(tobj.IsA())
        if cls is None:
            raise Exception('Obtained TObject of unsupported type.')
        return cls(tobj)

    def __init__(self):
        self._hdb = ROOT.OnlineHistDB()

    def histograms_for_page(self, page):
        """Return a list of OnlineHistograms that belong to the given page."""
        hists = VectorOfOnlineHistograms()
        self._hdb.getHistogramsByPage(page, hists)
        pyhists = []
        for h in hists:
            pyhists.append(OnlineHistogram(h, self))
        del hists
        return pyhists

    def histogram_for_id(self, hid):
        """Return the OnlineHistogram with the given histogram ID."""
        return OnlineHistogram(self._hdb.getHistogram(StdString(hid)), self)

    def page(self, page_path):
        """Return the OnlineHistPage object for the given path."""
        ohp = self._hdb.getPage(page_path)
        ohp.load()
        return ohp

    def omalib_algorithm(self, name):
        """Return an OMAlib algorithm associated to this database.

        Due to some odd memory-management-related behaviour, the OMAlib object
        is returned along with the algorithm, and this OMAlib object *must* be
        deleted, with `del`, by the caller once it is no longer needed.
        Allowing the Python GC to delete the object causes a double-free
        crash. It is not understood why this happens.
        """
        omalib = ROOT.OMAlib(self._hdb)
        algorithm = omalib.getAlg(StdString(name))
        return omalib, algorithm
