import tools

#================================================================================
class importUtils:
#--------------------------------------------------------------------------------
  def __init__(self,cdb):
    self.cdb = cdb
    self.tools = tools.tools()
    self.include = []
    self.stdIncludes = []
    self.forwardDecl = []
    self.forwardIncl = []
    self.excludes = []
    self.typedefenums = []
    self.alpha = ['a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z','A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z','1','2','3','4','5','6','7','8','9','0','_']
    self.charBegin = [' ','<',',']
    self.charEnd   = [' ','>',',','*','&']
#--------------------------------------------------------------------------------
  def reset(self,godClass):
    self.typedefenums = []
    if godClass.has_key('typedef'):
      for t in godClass['typedef']:
        self.typedefenums.append(t['attrs']['def'])
    if godClass.has_key('enum'):
      for e in godClass['enum']:
        self.typedefenums.append(e['attrs']['name'])
#--------------------------------------------------------------------------------
  def checkTypedefEnums(self, type, className):
    for tde in self.typedefenums:
      pos = type.find(tde)
      if pos != -1:
        if (pos == 0) or (type[pos-1] in self.charBegin ) : 
          if  (pos+len(tde) == len(type)) or (len(type) > pos+len(tde) and type[pos+len(tde)] in self.charEnd):
            type = type.replace(tde,'%s::%s'%(className,tde))
    return type
#--------------------------------------------------------------------------------
  def typeIsTypedefOrEnum(self,type):
    for tde in self.typedefenums :
      if type.find(tde) != -1 : return 1
    return 0
#--------------------------------------------------------------------------------
  def genExcludes(self, godClass, excludes) :
    if godClass.has_key('import') :
      for imp in godClass['import'] :
        impAtt = imp['attrs']
        impName = impAtt['name']
        if impAtt['ignore'] == 'TRUE' :
          if self.cdb.has_key(impName) : impName = self.cdb[impName]
          if impName not in excludes : excludes.append(impName)
    if godClass.has_key('typedef') :
      for td in godClass['typedef']:
        excludes.append(td['attrs']['def'])
    if godClass.has_key('enum') :
      for enum in godClass['enum']:
        excludes.append(enum['attrs']['name'])
#--------------------------------------------------------------------------------
  def genIncludeLine(self, type):
    type = self.tools.unQualifyT(type)
    if self.tools.isReferenceT(type) : type = type[:type.rfind('&')]
    while self.tools.isPointerT(type) : type = type[:type.rfind('*')]
    type = self.tools.unQualifyT(type)
    if not (self.tools.isFundamentalT(type) or type in self.excludes):
      if self.tools.isStlT(type) : return self.tools.stlIncludeFile(type)
      if type.strip()[:5] == 'std::' : return ''
      if self.cdb.has_key(type) : return self.cdb[type]
      if type.find('::') != -1 : return ''
      if type : print ': WARNING: Could not find type %s' % type
    return ''  
#--------------------------------------------------------------------------------
  def addInclude(self, impStr):
    types,tTypes = self.tools.getTypeTokens(impStr)
    for type in types :
      if self.typeIsTypedefOrEnum(type) : pass
      else :
        imp = self.genIncludeLine(type)
        if imp and imp not in self.excludes:
          if self.tools.isStlT(imp):
            if imp not in self.stdIncludes : self.stdIncludes.append(imp)
          elif imp not in self.include : self.include.append(imp)
#--------------------------------------------------------------------------------
  def addForwardDecl(self, decStr):
    types,tTypes = self.tools.getTypeTokens(decStr)
    for type in types:
      if type in tTypes :
        self.addInclude(type)
      else:
        type = self.tools.unQualifyT(type)
        if self.tools.isReferenceT(type)  : type = type[:type.rfind('&')]
        while self.tools.isPointerT(type) : type = type[:type.rfind('*')]
        type = self.tools.unQualifyT(type)
        if self.typeIsTypedefOrEnum(type) : pass
        else :
          dec = self.genIncludeLine(type)
          if dec and dec not in self.excludes and dec not in self.forwardIncl :
            if self.tools.isStlT(type) :
              self.stdIncludes.append(dec)
            else:
              self.forwardDecl.append(dec)
              self.forwardIncl.append(dec)
#--------------------------------------------------------------------------------
  def parseImport(self, godClass, include, stdIncludes, forwardDecl, forwardIncl):
    if godClass.has_key('import'):
      for imp in godClass['import']:
        impAtt = imp['attrs']
        impName = impAtt['name']
        if impName not in self.excludes:
          if impAtt['std'] == 'TRUE' and impName not in stdIncludes : stdIncludes.append(impName)
          else :
            if self.cdb.has_key(impName) : impName = self.cdb[impName]
            if impAtt['soft'] == 'TRUE':                                             # do forward declaration stuff        
              if impName not in forwardIncl : forwardIncl.append(impName)
              if impName not in forwardDecl : forwardDecl.append(impName)
            else :                                                                   # do include stuff
              if impName not in include : include.append(impName)
#--------------------------------------------------------------------------------
  def genIncludes(self):
    s = ''
    for imp in self.include : s += '#include "%s.h"\n' % imp
    s += self.genStdIncludes()
    return s
#--------------------------------------------------------------------------------
  def genStdIncludes(self):
    s = ''
    for imp in self.stdIncludes : s += '#include <%s>\n' % imp
    return s
#--------------------------------------------------------------------------------
  def genForwardDecls(self):
    s = ''
    for decl in self.forwardDecl :
      if decl not in self.include : s += 'class %s;\n' % decl.split('/')[-1]
    return s
