import time, os
import genSrcUtils, tools, gparser

#================================================================================
class genNamespaces(genSrcUtils.genSrcUtils):
#--------------------------------------------------------------------------------
  def __init__(self,cdb,godRoot):
    genSrcUtils.genSrcUtils.__init__(self,cdb)
    self.godRoot = godRoot
    self.tools = tools.tools()
    self.packInclude = []
    self.packForwardDecl = []
    self.packExcludes = []
    self.forwardDecl = []
    self.forwardIncl = []
#--------------------------------------------------------------------------------
  def reset(self,package,godNamespace):
    genSrcUtils.genSrcUtils.reset(self,godNamespace)
    self.include     = list(package.include)
    self.forwardDecl = list(package.forwardDecl)
    self.forwardIncl = list(package.forwardIncl)
    self.excludes    = list(package.excludes)
#--------------------------------------------------------------------------------
  def genNamespaceExcludes(self, dict):
    self.genExcludes(dict,self.excludes)
#--------------------------------------------------------------------------------
  def parseNamespaceImport(self, dict):
    forwardIncl = []
    self.parseImport(dict, self.include, self.stdIncludes, self.forwardDecl, forwardIncl)
#--------------------------------------------------------------------------------
  def genAuthor(self,godNamespace):
    s = 'unknown'
    if godNamespace.has_key('author') : s = godNamespace['author']
    return s
#--------------------------------------------------------------------------------
  def genClasses(self,godNamespace):
    s = ''
    if godNamespace.has_key('class'):
      print ':ERROR: <class> element not implemented for namespaces yet'
    return s
#--------------------------------------------------------------------------------
  def doit(self,package,godNamespaces,outputDir,lname):
    

    for godNamespace in godNamespaces:

      self.reset(package,godNamespace)
      self.genNamespaceExcludes(godNamespace)
      self.parseNamespaceImport(godNamespace)

      namespaceDict = package.dict
      namespacename = godNamespace['attrs']['name']

      fileName = '%s.h' % namespacename

      print '    File %s' % fileName.ljust(lname),

      namespaceDict['namespacename']   = namespacename
      namespaceDict['desc']            = self.genDesc(godNamespace)
      namespaceDict['author']          = self.genAuthor(godNamespace)
      namespaceDict['today']           = time.ctime()
      namespaceDict['typedefs']        = self.genTypedefs('all',godNamespace)
      namespaceDict['enums']           = self.genEnums('all',godNamespace)
      namespaceDict['classes']         = self.genClasses(godNamespace)
      namespaceDict['attributes']      = self.genAttributes('all',godNamespace,1)
      namespaceDict['methods']         = self.genMethods('all',godNamespace)
      namespaceDict['methodDefs']      = self.genMethods('all',godNamespace,namespacename)

      namespaceDict['includes']        = self.genIncludes()
      namespaceDict['forwardDecls']    = self.genForwardDecls()
      namespaceDict['forwardIncludes'] = self.genForwardIncludes()

      g = gparser.gparser()
      g.parse(self.godRoot+'templates/namespace.tpl',namespaceDict)

      file = open(outputDir+os.sep+fileName,'w')
      file.write(g.stream)
      file.close()

      print ' - Done'

#================================================================================
#import xparser
#if __name__ == '__main__':
#  x = xparser.xparser('xml_files/MuonEvent.xml','xml_files/GODsClassDB.xml')
#  x.parse()
#  g = genNamespaces(x.gdd,x.cdb)
#  g.doit()
