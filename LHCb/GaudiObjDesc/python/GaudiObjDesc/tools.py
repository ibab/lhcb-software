import string

#================================================================================
class tools:
#--------------------------------------------------------------------------------
  def __init__( self ):
    self.booleanT =       ['bool']
    self.characterT =     ['char','signed char', 'unsigned char', 'w_chart']
    self.integerT =       ['short int', 'short', 'signed short', 'short signed',
                           'signed short int', 'short signed int', 'int', 'signed',
                           'signed int', 'long int', 'long', 'signed long',
                           'long signed', 'signed long int', 'long signed int',
                           'unsigned short int', 'unsigned short', 'short unsigned',
                           'short unsigned int', 'unsigned int', 'unsigned', 
                           'unsigned long int', 'unsigned long', 'long unsigned', 
                           'long unsigned int', 'longlong', 'long long',
                           'long long int', 'ulonglong', 'long long unsigned',
                           'unsigned long long',  'unsigned long long int',
                           'long long unsigned int']
    self.floatingPointT = ['float', 'double', 'long double']
    self.stlT           = ['vector','list','deque','queue','stack','map','set',
                           'bitset','pair','string','utility','istream','ostream',
                           'iostream']
    self.bitfieldT       = ['bitfield']
    self.cppKeywords    = ['and', 'and_eq', 'asm', 'auto', 'bitand', 'bitor',
                           'bool', 'break', 'case', 'catch', 'char', 'class',
                           'compl', 'const', 'const_cast', 'continue', 'default',
                           'delete', 'do', 'double', 'dynamic_cast', 'else',
                           'enum', 'explicit', 'export', 'extern', 'false',
                           'float', 'for', 'friend', 'goto', 'if', 'inline',
                           'int', 'long', 'mutable', 'namespace', 'new', 'not',
                           'not_eq', 'operator', 'or', 'or_eq', 'private',
                           'protected', 'public', 'register', 'reinterpret_cast',
                           'return', 'short', 'signed', 'sizeof', 'static',
                           'static_cast', 'struct', 'switch', 'template', 'this',
                           'throw', 'true', 'try', 'typedef', 'typeid',
                           'typename', 'union', 'unsigned', 'usign', 'virtual',
                           'void', 'volatile', 'wchar_t', 'while', 'xor', 'xor_eq']
#--------------------------------------------------------------------------------
  def isBitfieldT( self, type ):
    if ( type in self.bitfieldT ) : return 1
    return 0
#--------------------------------------------------------------------------------
  def isFundamentalT( self, type ):
    if ( self.isBooleanT( type ) or self.isCharacterT( type ) or 
         self.isIntegerT( type ) or self.isFloatingPointT( type )) : return 1
    return 0
#--------------------------------------------------------------------------------
  def isBooleanT( self, type ):
    if ( type in self.booleanT ) : return 1
    return 0
#--------------------------------------------------------------------------------
  def isCharacterT( self, type ):
    if ( type in self.characterT ) : return 1
    return 0
#--------------------------------------------------------------------------------
  def isIntegerT( self, type ):
    if ( type in self.integerT ) : return 1
    return 0
#--------------------------------------------------------------------------------
  def isFloatingPointT( self, type ):
    if ( type in self.floatingPointT ) : return 1
    return 0
#--------------------------------------------------------------------------------
  def isPointerT( self, type ):
    if ( type and type.strip()[-1] == '*' ) : return 1
    return 0
#--------------------------------------------------------------------------------
  def isReferenceT( self, type ):
    if ( type and type.strip()[-1] == '&' ) : return 1
    return 0
#--------------------------------------------------------------------------------
  def isArrayT( self, type ):
    if ( type and type.strip()[-1] == ']' ) : return 1
    return 0
#--------------------------------------------------------------------------------
  def isConstT( self, type ):
    s = type.strip()
    if ((s[:6] == 'const ') or (s[-5:] == 'const')) : return 1
    return 0
#--------------------------------------------------------------------------------
  def isVolatileT( self, type ):
    s = type.strip()
    if ((s[:9] == 'volatile ') or (s[-8:] == 'volatile')) : return 1
    return 0
#--------------------------------------------------------------------------------
  def isStlT( self, type ):
    s = type.strip()
    if s[:5] == 'std::' : s = s[5:]
    if s in self.stlT : return 1
    return 0
#--------------------------------------------------------------------------------
  def stlIncludeFile( self, type ):
    s = type.strip()
    if s[:5] == 'std::' : s = s[5:]
    if s in self.stlT :
      if s == 'pair' : return 'utility'
      return s 
    return ''
#--------------------------------------------------------------------------------
  def unConstT( self, type ):
    s = type.strip()
    if s[:6]  == 'const ' : return s[6:].strip()
    if s[-6:] == ' const' : return s[:-6].strip()
    return type
#--------------------------------------------------------------------------------
  def unVolatileT( self, type ):
    s = type.strip()
    if s[:9]  == 'volatile ' : return s[9:].strip()
    if s[-9:] == ' volatile' : return s[:-9].strip()
    return type
#--------------------------------------------------------------------------------
  def unQualifyT( self, type ):
    s = type.strip()
    s = self.unConstT(s)
    s = self.unVolatileT(s)
    s = self.unConstT(s)
    return s
#--------------------------------------------------------------------------------
  def isCppKeyword( self, word ):
    if word in self.cppKeywords : return 1
    return 0
#--------------------------------------------------------------------------------
  def checkTypedefs(self, type, tdefs, scope):
    uqT = self.unQualifyT(type)
    uqT.strip()
    if uqT[-1] == '&' : uqT = uqT[:-1]
    uqT.strip()
    while uqT[-1] == '*' :
      uqT = uqT[:-1]
      uqT.strip()
    if uqT in tdefs :
      type = type.replace(uqT,scope+uqT)
    return type
#--------------------------------------------------------------------------------
  def genReturnFromStrg(self, returnType, typedefs, scopeName):
    s = returnType
    s = self.checkTypedefs(s,typedefs,scopeName)
    ##uqS = self.unQualifyT(returnType)
    ##if self.isFundamentalT(uqS) or uqS == 'void' : return uqS                     # remove const from fundamental types
    #if (not (self.isPointerT(returnType) or self.isReferenceT(returnType))) : return s + '&'                                                              # a complex type shall be treated by ref
    return s
#--------------------------------------------------------------------------------
  def genReturnFromElem(self, r, typedefs, scopeName):
    s = ''
    rAtt = r['attrs']
    if rAtt['const'] == 'TRUE' : s += 'const '
    s += self.checkTypedefs(rAtt['type'],typedefs,scopeName)
    return s
#--------------------------------------------------------------------------------
  def genParamFromStrg(self, paramType):
    s = paramType
    s = self.unQualifyT(s)
    if self.isFundamentalT(self.unQualifyT(paramType)) : return paramType
    if not self.isPointerT(paramType) :
      if not self.isReferenceT(paramType) : paramType += '&'
      if not self.isConstT(paramType): paramType = 'const ' + paramType
    return paramType
#-------------------------------------------------------------------------------- 
  def genParamsFromStrg(self, parameters):
    s = ''
    paramList = self.params2Param(parameters)
    pList = []
    for p in paramList :
      ps = p.split()
      pList.append('%s %s' % (self.genParamFromStrg((' ').join(ps[:-1])), ps[-1]))
    return pList
#--------------------------------------------------------------------------------
  def genParamTypeName(self, fullParam) :                                        # this is a very simple minded implementation
    return ((' ').join(fullParam.split()[:-1]), fullParam.split()[:-1])          # assuming that every parameter has a name
#--------------------------------------------------------------------------------
  def genParamFromElem(self, p):
    s = ''
    if   p['const'] == 'TRUE'           : s += 'const '
    if   p['inout'] == 'BYVALUE'        : s += p['type']
    elif p['inout'] == 'INPUT'          :
      s += p['type']
      if not (self.isFundamentalT(p['type']) or self.isPointerT(p['type'])) : s += '&'
    elif p['inout'] in ['INOUT','BOTH'] : s += p['type'] + '&'
    s += ' '
    if p.has_key('name') : s += p['name']
    else : s += 'value'
    return s
#--------------------------------------------------------------------------------
  def genParamsFromElem(self, pDict):
    pList = []
    for p in pDict : pList.append(self.genParamFromElem(p['attrs']))
    return pList
#--------------------------------------------------------------------------------
  def removeCppKeywords(self, type):
    typeList = type.split()
    type2 = ''
    for t in typeList :
      if not self.isCppKeyword(t) : type2 += '%s ' % t
    return type2[:-1]
#--------------------------------------------------------------------------------
  def getTypeTokens(self, completeType):
    tList2 = []
    tList3 = []
    tList4 = []
    tTypes = []
    tList1 = completeType.split(',')
    [tList2.extend(x.split('>')) for x in tList1]
    for x in tList2:
      sList = x.split('<')
      if len(sList) > 1:
        tTypes.extend(sList[:-1])
      tList3.extend(sList)
    tList4 = [self.removeCppKeywords(x) for x in tList3 if self.removeCppKeywords(x).strip()]
    return (tList4,tTypes)
#--------------------------------------------------------------------------------
  def getLongestName( self, dict ):
    lname = 0
    for d in dict :
      if d.has_key('attrs') and len(d['attrs']['name']) > lname :
        lname = len(d['attrs']['name'])
    return lname
#--------------------------------------------------------------------------------
  def params2Param( self, params ):
    paramList = []
    inTempl = 0
    param = ''
    for c in params:
      if c == ',' and not inTempl :
        paramList.append(param.strip())
        param = ''
      else :
        if c == '<' : inTempl += 1
        elif c == '>' : inTempl -= 1
        param += c
    if param : paramList.append(param)
    return paramList
#--------------------------------------------------------------------------------
  def cppEscape(self, str):
    rStr = ''
    for c in str:
      if   c == '<' : rStr += 'Lt'
      elif c == '>' : rStr += 'Gt'
      elif c == ' ' : rStr += 'Sp'
      elif c == '=' : rStr += 'Eq'
      elif c == ':' : rStr += 'Cl'
      elif c == '!' : rStr += 'Nt'
      else          : rStr += c
    return rStr
#--------------------------------------------------------------------------------
  def firstUp(self,str):
    return str[0].upper() + str[1:]
#--------------------------------------------------------------------------------
  def lowerGetterName(self,metName):
    if metName[0].isupper():
      lname = len(metName)
      for i in range(0,lname):
        if i != (lname-1) and metName[i].isupper() and metName[i+1].isupper():
          metName = metName[:i] + metName[i].lower() + metName[i+1:]
        elif i == (lname-1) and metName[i].isupper():
          metName = metName[:i] + metName[i].lower()
        else:
          break
    return metName
#--------------------------------------------------------------------------------
  def printCode(self,code):
    codeList = code.split('\n')
    basicIndent = 0
    while not codeList[0].strip() : codeList = codeList[1:]
    while not codeList[-1].strip() : codeList = codeList[:-1]
    for c in codeList[0]:
      if c.isspace() : basicIndent += 1
      else : break
    s = ''
    for codeLine in codeList:
      if codeLine[:basicIndent].isspace():
        s += '  %s' % codeLine[basicIndent:]
      else :
        s += '  %s' % codeLine.strip()
    return s
