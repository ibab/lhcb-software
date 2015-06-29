import os

from veloview.utils.rootutils import ROOT

def setUpModule():
    ROOT.SetMemoryPolicy(ROOT.kMemoryStrict)


import unittest
import tempfile
from veloview.core.io import flatten, unflatten
class TestGRFIOutils(unittest.TestCase):

    def test_flatten_unflatten(self):
        d = {'l': 9,
             'm': {'g': 7, 'h':8,
                   'i': {'a':1, 'b':2},
               }}
        f = {'l'    : 9,
             'm.g'  : 7,
             'm.h'  : 8,
             'm.i.a': 1,
             'm.i.b': 2
         }
        self.assertEqual(flatten(d), f)
        self.assertEqual(unflatten(f), d)


from veloview.core.io import GRFIO
class TestGRFIO(unittest.TestCase):

    def setUp(self):
        self.branches = {
            'runnr':   'UInt_t', # this never gets updated, so no VersionedObject here
            'checked': 'VersionedObject<UShort_t, TimeStamp, std::greater<TimeStamp> >',
            'comment': 'VersionedObject<std::string, TimeStamp, std::greater<TimeStamp> >',
            'score':   'VersionedObject<float, TimeStamp, std::greater<TimeStamp> >'
        }
        entry = {
            'runnr':   142467,
            'checked': 1,
            'comment': 'OK',
            'score':   98.5
        }
        self.entries = []
        for i in xrange(5):
            entry['runnr'] += 1
            if i % 2 == 1:
                entry['checked'] = 2
                entry['comment'] = 'NOK'
            else:
                entry['checked'] = 1
                entry['comment'] = 'OK'
            from copy import deepcopy
            self.entries.append(deepcopy(entry))

            # for test_update
            self.run = 142470
            if entry['runnr'] == self.run:
                self.entry2 = deepcopy(entry)
                self.entry2['checked'] = 3
                self.entry2['comment'] = 'undecided'

        self.temppath = '{0}/test.root'.format(tempfile.mkdtemp())
        self.grf = GRFIO(self.temppath, mode = 'new', branches = self.branches)

    def tearDown(self):
        del self.grf
        if os.path.exists(self.temppath):
            os.remove(self.temppath)

    def make_grf(self):
        for entry in self.entries:
            self.grf.fill(entry)
        self.grf.write()

    def edit_grf(self):
        self.make_grf()
        find_run = lambda tree: 142470 == tree.runnr
        self.grf.edit(find_run, self.entry2)

    def test_read_write(self):
        self.make_grf()
        branches = [key for key in self.entries[0]]
        nentry = 0
        for dummy in self.grf.tree:
            res = self.grf.read(branches)
            self.assertEqual(self.entries[nentry], res)
            nentry += 1

    @unittest.skip('Not implemented')
    def test_version_browsing(self):
        pass

    def test_edit(self):
        self.edit_grf()
        branches = [key for key in self.entries[0]]
        nentry = 0
        for dummy in self.grf.tree:
            res = self.grf.read(branches)
            if res['runnr'] == self.run:
                ref = self.entry2
            else:
                ref = self.entries[nentry]
            self.assertEqual(ref, res)
            nentry += 1


if __name__ == '__main__':
    unittest.main()
