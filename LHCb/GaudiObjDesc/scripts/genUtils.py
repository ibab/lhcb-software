import tools

#================================================================================
class genUtils:
#--------------------------------------------------------------------------------
  def __init__(self,cdb):
    self.cdb = cdb
    self.tools = tools.tools()
    self.include = []
    self.forwardDecl = []
    self.forwardIncl = []
    self.excludes = []
#--------------------------------------------------------------------------------
  def genExcludes(self, godClass, excludes) :
    if godClass.has_key('import') :
      for imp in godClass['import'] :
        impAtt = imp['attrs']
        impName = impAtt['name']
        if impAtt['ignore'] == 'TRUE' :
          impStr = ''
          if impAtt['std'] == 'TRUE' : impStr = '<'+impName+'>'
          elif self.cdb.has_key(impName) : impStr = '"'+self.cdb[impName]+'.h"'
          else : impStr = '"'+impName+'.h"'
          if impStr not in excludes : excludes.append(impStr)
    if godClass.has_key('typedef') :
      for td in godClass['typedef']:
        excludes.append(td['attrs']['def'])
    if godClass.has_key('enum') :
      for enum in godClass['enum']:
        excludes.append(enum['attrs']['name'])
#--------------------------------------------------------------------------------
  def genIncludeLine(self, type):
    if not (self.tools.isFundamentalT(type) or type in self.excludes):
      if self.tools.isStlT(type) : return self.tools.stlIncludeFile(type)
      if self.cdb.has_key(type) : return '"' + self.cdb[type] + '.h"'
      print ': WARNING: Could not find type .%s.' % type
    return ''  
#--------------------------------------------------------------------------------
  def addInclude(self, impStr):
    types = self.tools.getTypeTokens(impStr)
    for type in types :
      imp = self.genIncludeLine(type)
      if imp and imp not in self.excludes and imp not in self.include :
        self.include.append(imp)
#--------------------------------------------------------------------------------
  def addForwardDecl(self, decStr):
    types = self.tools.getTypeTokens(decStr)
    for type in types:
      dec = self.genIncludeLine(type)
      if dec and dec not in self.excludes and dec not in self.forwardIncl :
        if not self.tools.isStlT(type) : self.forwardDecl.append(type)
        self.forwardIncl.append(dec)
#--------------------------------------------------------------------------------
  def parseImport(self, godClass, include, forwardDecl, forwardIncl):
    if godClass.has_key('import'):
      for imp in godClass['import']:
        impAtt = imp['attrs']
        impName = impAtt['name']
        impStr = ''
        if impAtt['std'] == 'TRUE' : impStr = '<' + impName + '>'
        elif self.cdb.has_key(impName) : impStr = '"' + self.cdb[impName] + '.h"'
        else : impStr = '"' + impName + '.h"'
        if impStr not in self.excludes:
          if impAtt['soft'] == 'TRUE':                                             # do forward declaration stuff        
            if impStr not in forwardIncl :
              forwardDecl.append(impName)
              forwardIncl.append(impStr)
          else :                                                                   # do include stuff
            if impStr not in include : include.append(impStr)
#--------------------------------------------------------------------------------
  def genIncludes(self):
    s = ''
    for imp in self.include : s += '#include %s\n' % imp
    return s
#--------------------------------------------------------------------------------
  def genForwardDecls(self):
    s = ''
    for decl in self.forwardDecl : s += 'class %s;\n' % decl
    return s
