import string
import importUtils

#================================================================================
class genSrcUtils(importUtils.importUtils):
#--------------------------------------------------------------------------------
  def __init__(self,cdb):
    importUtils.importUtils.__init__(self,cdb)
    self.generatedTypedefs = []
#--------------------------------------------------------------------------------
  def reset(self,godClass):
    importUtils.importUtils.reset(self,godClass)
    self.generatedTypedefs = []
#--------------------------------------------------------------------------------
  def genDesc(self, godClass):
    s = ''
    classAtt = godClass['attrs']
    if classAtt.has_key('desc'):                                                 # add class attribute desc
      s += classAtt['desc']
    if godClass.has_key('desc'):                                                # add elements desc
      for desc in godClass['desc']:
        s += desc['cont']
    if len(s):
      s = s.replace('\n','')
      s = string.join(s.split())
      s2 = ''                                                                    # text reformatting (<80chars)
      while len(s):
        if len(s) < 77:
          if len(s2) : s2 += '\n * ' + s
          else : s2 += s
          s = ''
        else:
          pos = s.rfind(' ',0,77)
          if len(s2) : s2 += '\n * ' + s[:pos]
          else : s2 += s[:pos]
          s = s[pos+1:]
      s = s2
    return s
#--------------------------------------------------------------------------------
  def genTypedefs(self,modifier,godClass):
    s = ''
    if godClass.has_key('typedef'):
      for td in godClass['typedef']:
        tdAtt = td['attrs']
        if tdAtt['access'] == modifier.upper() or modifier == 'all':
          s += '  /// %s\n' % tdAtt['desc']
          s += '  typedef %s %s;\n' % (tdAtt['type'], tdAtt['def'])
          self.generatedTypedefs.append(tdAtt['def'])
    return s
#--------------------------------------------------------------------------------
  def genEnums(self,modifier,godClass):
    s = ''
    if godClass.has_key('enum'):
      for enum in godClass['enum']:
        enumAtt = enum['attrs']
        if enumAtt['access'] == modifier.upper() or modifier == 'all':
          indent = ' ' * (len(enumAtt['name'])+8)
          values = enumAtt['value'].split(',')
          s += '  /// %s\n' % enumAtt['desc']
          s += '  enum %s{ %s' % ( enumAtt['name'], values[0].strip() )
          for value in values[1:] :
            s += ',\n%s %s' % ( indent, value.strip() )
          s += ' };\n'
    return s 
#--------------------------------------------------------------------------------
  def genAttributes(self,modifier,godClass,namespace=0):
    s = ''
    maxLenTypNam = [0,0,0]
    if godClass.has_key('attribute'):
      for att in godClass['attribute'] :
        if att['attrs']['access'] == modifier.upper() or modifier == 'all':
          maxLenTypNam[0] = max(maxLenTypNam[0], len(att['attrs']['type']))
          maxLenTypNam[1] = max(maxLenTypNam[1], len(att['attrs']['name']))
          if namespace and att['attrs'].has_key('init'):
            maxLenTypNam[2] = max(maxLenTypNam[1], len(att['attrs']['init']))
    if godClass.has_key('relation'):
      for rel in godClass['relation'] :
        if rel['attrs']['access'] == modifier.upper() or modifier == 'all':
          typlen = len(rel['attrs']['type'])
          if rel['attrs']['multiplicity'] != '1' : 
            maxLenTypNam[0] = max(maxLenTypNam[0], typlen + 16)
          else                                   : 
            maxLenTypNam[0] = max(maxLenTypNam[0], typlen + 10)
          maxLenTypNam[1] = max(maxLenTypNam[1], len(rel['attrs']['name']))
    if namespace :
      maxLenTypNam[1] += 1
      if maxLenTypNam[2] : maxLenTypNam[2] += 4
    else : maxLenTypNam[1] += 3
    if godClass.has_key('attribute'):
      for att in godClass['attribute'] :
        attAtt = att['attrs']
        if attAtt['access'] == modifier.upper() or modifier == 'all':
          if attAtt['type'] == 'bitfield':
            self.bitfieldEnums[modifier.lower()] += self.genBitfield(att)
            attAtt['type'] = 'unsigned'
          self.addInclude(attAtt['type'])
          name = attAtt['name']
          namespaceInit = ''
          if namespace :
            if attAtt.has_key('init') : namespaceInit = ' = %s;'%attAtt['init']
          else :
            name = 'm_%s;' % name
          s += '  %s %s%s ///< %s\n' % (attAtt['type'].ljust(maxLenTypNam[0]), \
                                        name.ljust(maxLenTypNam[1]), \
                                        namespaceInit.ljust(maxLenTypNam[2]), \
                                        attAtt['desc'])
    if godClass.has_key('relation'):
      for rel in godClass['relation'] :
        relAtt = rel['attrs']
        if relAtt['access'] == modifier.upper() or modifier == 'all':
          self.addForwardDecl(relAtt['type'])
          if relAtt['multiplicity'] == '1' : 
            relType = 'SmartRef<' + relAtt['type'] + '>' 
            self.addInclude('SmartRef')
          else: 
            relType = 'SmartRefVector<' + relAtt['type'] + '>'
            self.addInclude('SmartRefVector')
          s += '  %s %s ///< %s\n' % (relType.ljust(maxLenTypNam[0]),
                                        ('m_%s;'%relAtt['name']).ljust(maxLenTypNam[1]), relAtt['desc'])
    return s
#--------------------------------------------------------------------------------
  def genMethod(self,met,scopeName=''):
    s = ''
    indent = 0
    if (scopeName and not met.has_key('code')) : return s
    metAtt = met['attrs']
    if ( not scopeName ) :
      s += '  /// %s\n  ' % metAtt['desc']
      indent += 2
      if metAtt['virtual'] in ['TRUE','PURE']:
        s += 'virtual '
        indent += 8
      if metAtt['friend'] == 'TRUE' :
        s += 'friend '
        indent += 7
    else :
      s += 'inline '
      scopeName += '::'
      indent += len(scopeName) + 7
    constF = ''
    if metAtt['const'] == 'TRUE' : constF = ' const'
    pList = []
    if metAtt.has_key('argList'): pList = self.tools.genParamsFromStrg(metAtt['argList'])
    if met.has_key('arg')       : pList = self.tools.genParamsFromElem(met['arg'])
    metRet = ''
    if metAtt['type'].strip() :  metRet = self.tools.genReturnFromStrg(metAtt['type'],self.generatedTypedefs,scopeName) + ' '
    if met.has_key('return') :   metRet = self.tools.genReturnFromElem(met['return'],self.generatedTypedefs,scopeName) + ' '
    self.addForwardDecl(metRet)
    indent += len(metRet) + len(metAtt['name'])
    s += metRet + scopeName + metAtt['name'] + '('
    if len(pList):
      if scopeName : s += pList[0].split('=')[0]                                # in implementation strip off default arguments
      else         : s += pList[0]
      self.addForwardDecl((' ').join(pList[0].split()[:-1]))
      if len(pList[1:]):
        for p in pList[1:]:
          self.addForwardDecl((' ').join(p.split()[:-1]))
          if scopeName : s += ',\n%s %s' % (indent*' ', p.split('=')[0])        # in implementation strip off default arguments
          else         : s += ',\n%s %s' % (indent*' ', p)

    s += ')' + constF
    if metAtt['virtual'] == 'PURE': s += ' = 0'
    if not scopeName : s += ';\n\n'
    else : 
      s += ' \n{\n%s\n}\n\n' % met['code'][0]['cont']
    return s
#--------------------------------------------------------------------------------
  def genMethods(self,modifier,godClass,clname=''):
    s = ''
    if godClass.has_key('method'):
      for met in godClass['method']:
        if met['attrs']['access'] == modifier.upper() or modifier == 'all':
          s += self.genMethod(met,clname)
    return s[:-1]
