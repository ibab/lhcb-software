import importUtils

#================================================================================
class genPackage(importUtils.importUtils):
#--------------------------------------------------------------------------------
  def __init__(self, godPackage, cdb):
    importUtils.importUtils.__init__(self,cdb)
    self.godPackage = godPackage
    self.dict = {}
    self.include = []
    self.stdIncludes = []
    self.forwardDecl = []
    self.forwardIncl = []
    self.excludes = []
    self.doit()
#--------------------------------------------------------------------------------
  def parsePackImport(self, dict):
    self.parseImport(dict, self.include, self.stdIncludes, self.forwardDecl, self.forwardIncl)
#--------------------------------------------------------------------------------
  def genPackExcludes(self, dict) :
    self.genExcludes(dict, self.excludes)
#--------------------------------------------------------------------------------
  def doit(self):
    self.dict['packagename'] = self.godPackage['attrs']['name']
    self.genPackExcludes(self.godPackage)
    self.parsePackImport(self.godPackage)
