import os

if __name__ == '__main__':
    import sys

    # fiddle with sys.path so that package is importable
    if __file__.startswith('/'):
        sys.path.insert(0, os.path.join('/', *__file__.split('/')[:-3]))
    else:
        __path_to_script__ = __file__.split('/')[:-1]  # test directory
        __path_to_script__ += ['..', '..'] # package directory parent
        sys.path.insert(0, os.path.join(os.getcwd(), *__path_to_script__))

from veloview.utils.rootutils import ROOT

def setUpModule():
    ROOT.SetMemoryPolicy(ROOT.kMemoryStrict)
    status = ROOT.gSystem.Load('libCintex')
    if status >= 0: ROOT.Cintex.Enable()
    else: raise RuntimeError('Couldn\'t load libCintex')
    status = ROOT.gSystem.Load('libVeloGUIUtils')
    if status < 0: raise RuntimeError('Couldn\'t load libVeloGUIUtils')


import unittest
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

        self.grf = GRFIO('/tmp/test.root', mode = 'new', branches = self.branches)

    def tearDown(self):
        del self.grf
        if os.path.exists('/tmp/test.root'):
            os.remove('/tmp/test.root')

        # from glob import glob
        # locks = glob('/tmp/test.root.lock*')
        # if locks: map(os.remove, locks)

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
    hdr_fmt = '='*5 + '{0:^{width}}' + '='*5
    print hdr_fmt.format('TestGRFIO', width=40)
    unittest.main()
