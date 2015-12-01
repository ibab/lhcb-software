from veloview import analysis
from veloview.core.analysis_config_wrapper import AnalysisConfigWrapper

import tempfile, unittest

class TestAnalysisConfigWrapper(unittest.TestCase):
    def setUp(self):
        self.tempFile = tempfile.NamedTemporaryFile()
        self.tempFile.write('analysis_config_branches = None\nanalysis_config_leaves = None')
        self.tempFile.flush()

        self.wrapper = AnalysisConfigWrapper(self.tempFile.name)
        self.branchesToParse = [
                {"weight": 1.,
                 "children": [],
                 "minWW": 2,
                 "minWE": 8,
                 "minEW": 1,
                 "minEE": 2,
                },
                {"weight": 1.,
                 "minWW": 2,
                 "minEW": 1,
                 "minEE": 2,
                },
                {"children": ["child1", "child2"],
                 "minWW": 1,
                 "minEE": 2,
                }
                ]
        self.branchesRef = [
                {"weight": 1.,
                 "children": [],
                 "minWW": 2,
                 "minWE": 8,
                 "minEW": 1,
                 "minEE": 2,
                },
                {"weight": 1.,
                 "children": [],
                 "minWW": 2,
                 "minWE": 10,
                 "minEW": 1,
                 "minEE": 2,
                },
                {"weight": 1.,
                 "children": ["child1", "child2"],
                 "minWW": 1,
                 "minWE": 10,
                 "minEW": 1,
                 "minEE": 2,
                },
                ]

        self.leavesToParse = [
                {
                 "name": "Leaf1",
                 "input": "VetraRef1",
                 "errThreshold": 30,
                 "warnThreshold": 60,
                 "function": "Chi2Test",
                 "functionarg": "myArg",
                 "weight": .5,
                },
                {
                 "name": "Leaf2",
                 "input": "VetraRef2_{}",
                 "errThreshold": 40,
                 "warnThreshold": 55,
                 "tell1": "R",
                 "function": "MeanWidthDiffRef",
                 "functionarg": ".1",
                 "weight": .5,
                },
                {
                 "name": "Leaf3",
                 "input": "VetraRef3_{}",
                 "errThreshold": 0,
                 "warnThreshold": 80,
                 "tell1": "Phi",
                 "function": "MeanWidthDiffRef",
                 "functionarg": None,
                 "weight": .8,
                },
                {
                 "name": "Leaf4",
                 "input": "VetraRef4_{}",
                 "errThreshold": 20,
                 "warnThreshold": 10,
                 "tell1": "Both",
                 "function": "FloorThreshold",
                 "functionarg": 10.,
                 "weight": 12.,
                },
                ]

        self.leavesRef = [
                [({"name": "Leaf1",
                 "path": "VetraRef1",
                 "weight": .5,
                 "maxError": 30,
                 "maxWarning": 60,
                 },
                 {"Function": analysis.stats.Chi2Test,
                  "Argument": "myArg",
                 })],
                self.__toTell1("R", ({
                    "name": "Leaf2",
                    "path": "VetraRef2_{}",
                    "weight": .5,
                    "maxError": 40,
                    "maxWarning": 55,
                 },
                 {
                    "Function": analysis.thresholds.MeanWidthDiffRef,
                    "Argument": ".1",
                 })),
                 self.__toTell1("Phi", ({
                     "name": "Leaf3",
                     "path": "VetraRef3_{}",
                     "weight": .8,
                     "maxError": 0,
                     "maxWarning": 80,
                 },
                 {
                    "Function": analysis.thresholds.MeanWidthDiffRef,
                    "Argument": None,
                 })),
                 self.__toTell1("Both", ({
                     "name": "Leaf4",
                     "path": "VetraRef4_{}",
                     "weight": 12.,
                     "maxError": 20,
                     "maxWarning": 10,
                 },
                 {
                    "Function": analysis.thresholds.FloorThreshold,
                    "Argument": 10.,
                 })),
                ]


    def tearDown(self):
        self.tempFile.close()
        del self.tempFile

    def testBranch(self):
        for (branchToParse, branchRef) in zip(self.branchesToParse, self.branchesRef):
            self.assertEqual(self.wrapper._parseBranch(branchToParse), branchRef)

        for (leafToParse, leafRef) in zip(self.leavesToParse, self.leavesRef):
            self.assertEqual(self.wrapper._parseLeaf(leafToParse["name"], leafToParse), leafRef)

    def __toTell1(self, rOrPhi, leaf):
        tell1s = []
        if rOrPhi in ["R", "Both"]:
            tell1s += range(0, 42)
        if rOrPhi in ["Phi", "Both"]:
            tell1s += range (64, 64 + 42)

        result = []
        for tell1 in tell1s:
            newLeaf = dict(leaf[0])
            tell1Formatted = AnalysisConfigWrapper.tell1_formatting(tell1)
            newLeaf["name"] += "_" + tell1Formatted
            newLeaf["path"] = newLeaf["path"].format(tell1Formatted)
            newLeaf["weight"] = 1.
            result.append((newLeaf, leaf[1]))

        branch = dict(leaf[0])
        branch.update({'minWW': 5, 'minEW': 1, 'minWE': 10, 'minEE': 2})
        branch["children"] = [res[0]["name"] for res in result]
        for key in ["path", "maxError", "maxWarning"]:
            branch.pop(key)
        result.append((branch, {}))
        return result

if __name__ == '__main__':
    unittest.main()
