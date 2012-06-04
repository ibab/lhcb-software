import importUtils

#================================================================================
class genPackage(importUtils.importUtils):
#--------------------------------------------------------------------------------
    def __init__(self, godPackage):
        importUtils.importUtils.__init__(self)
        self.godPackage = godPackage
        self.dict = {}
        self.include = []
        self.stdIncludes = []
        self.forwardDeclLHCb = []
        self.forwardDeclGlob = {}
        self.forwardIncl = []
        self.excludes = []
        self.doit()
#--------------------------------------------------------------------------------
    def parsePackImport(self, dict):
        self.parseImport(dict, self.include, self.stdIncludes, self.forwardDeclLHCb, self.forwardDeclGlob, self.forwardIncl)
#--------------------------------------------------------------------------------
    def doit(self):
        self.dict['packagename'] = self.godPackage['attrs']['name']
        self.dict['packagenamespace'] = self.godPackage['attrs']['namespace']
        self.parsePackImport(self.godPackage)
