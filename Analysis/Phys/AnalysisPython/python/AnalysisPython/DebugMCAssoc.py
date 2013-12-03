# -*- coding: utf-8 -*-
from AnalysisPython import Helpers

class AssocTreeDebugger :
    def __init__(self, assoc, particleName) :
        self.assoc = assoc
        self.pName = particleName
        self.nTrees = {}
    def __call__(self, particle) :
        trees = self.assoc.relatedMCPs(particle)
        if trees :
            size = trees.size()
            pid = Helpers.pid(particle)
            if self.nTrees.has_key(pid) :
                if self.nTrees[pid].has_key(size) :
                    self.nTrees[pid][size]+=1
                else :
                    self.nTrees[pid][size]=1
            else :
                self.nTrees[pid] = { size : 1 }
            print "Found ", size, " trees for particle ", self.pName(particle)
            if size > 0 :
                for tree in trees :
                    print "\t===========TREE========"
                    for p in tree :
                        print "\t", self.pName(p)
                    print "\t======================="
        else :
            print "Found NO trees for particle ", self.pName(particle)
            if self.nTrees.has_key("None") :
                self.nTrees["None"] +=1
            else :
                self.nTrees["None"]=1


class WeightedDebugger :
    def __init__(self, assoc, particleName) :
        self.assoc = assoc
        self.pName = particleName
        self.nTrees = {}
    def __call__(self, particle) :
        mcps = self.assoc.relatedMCPs(particle)
        if mcps :
            size = mcps.size()
            pid = Helpers.pid(particle)
            if self.nTrees.has_key(pid) :
                if self.nTrees[pid].has_key(size) :
                    self.nTrees[pid][size]+=1
                else :
                    self.nTrees[pid][size]=1
            else :
                self.nTrees[pid] = { size : 1 }
            if size > 1 :
                print "Found ", size, " mcps for particle ", self.pName(particle)
                print "\t===MCPs for Particle PID ", pid, " ==="
                for mcp in mcps :
                    print "\t", self.pName(mcp)
                print "\t=================================="
        else :
            if self.nTrees.has_key("None") :
                self.nTrees["None"] +=1
            else :
                self.nTrees["None"]=1
