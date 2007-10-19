import time, os, sys, string
import genSrcUtils, gparser

#================================================================================
class genClasses(genSrcUtils.genSrcUtils):
#--------------------------------------------------------------------------------
  def __init__(self, godRoot):
    genSrcUtils.genSrcUtils.__init__(self)
    self.godRoot = godRoot
    self.plurialExceptions = {'Vertex':'Vertices'}
    self.bitfieldEnums = {'public':'', 'protected':'', 'private':''}
    self.gKeyedContainerTypedef = 0
    self.gContainedObjectTypedef = 0
    self.genOStream = 0
    self.genFillStream = 0
    self.isEventClass = 0
#--------------------------------------------------------------------------------
  def reset(self, package, godClass):
    genSrcUtils.genSrcUtils.reset(self,godClass)
    self.excludes    = list(package.excludes)
    self.include     = list(package.include)
    self.stdIncludes = list(package.stdIncludes)
    self.forwardDeclLHCb = list(package.forwardDeclLHCb)
    self.forwardDeclGlob = package.forwardDeclGlob
    self.forwardIncl = list(package.forwardIncl)
    self.bitfieldEnums = {'public':'', 'protected':'', 'private':''}
    self.gKeyedContainerTypedef = 0
    self.gContainedObjectTypedef = 0
    if godClass['attrs']['serializers'] == 'TRUE' :
      self.genOStream = 1
      self.genFillStream = 1
    else:
      self.genOStream = 0
      self.genFillStream = 0
    if godClass.has_key('id') : self.isEventClass = 1
    else                      : self.isEventClass = 0
#--------------------------------------------------------------------------------
  def genClassnamePlurial(self, name):
    for singular in self.plurialExceptions.keys():
      pos = name.find(singular)
      if pos != -1 and pos+len(singular) == len(name):
        return name[:pos] + self.plurialExceptions[singular]
    return name + 's'
#--------------------------------------------------------------------------------
  def parseClassImport(self, dict):
    self.parseImport(dict, self.include, self.stdIncludes, self.forwardDeclLHCb, self.forwardDeclGlob, self.forwardIncl)
#--------------------------------------------------------------------------------
  def genClassID(self, godClass):
    s = ''
    classAtt = godClass['attrs']
    if classAtt.has_key('id'):
      s += '\n'
      s += '// Class ID definition\n'
      s += 'static const CLID CLID_%s = %s;\n' % ( classAtt['name'], classAtt['id']) 
    return s
#--------------------------------------------------------------------------------
  def genClassVersion(self, godClass):
    s = ''
    classAtt = godClass['attrs']
    if classAtt.has_key('version'):
      s += '\n// Class Version definition\n'
      s += 'static const unsigned int Version_%s = %s;\n' % (classAtt['name'], classAtt['version'])
    return s
#--------------------------------------------------------------------------------
  def genLocations(self, godClass):
    s = ''
    s2 = ''
    classAtt = godClass['attrs']
    if classAtt.has_key('location'):                                             # add class attribute location
      s2 += '  static const std::string& Default = "%s";\n' % classAtt['location']
    if godClass.has_key('location'):                                             # add elements location
      for loc in godClass['location']:
        locAtt = loc['attrs']
        place = locAtt['place']
        if locAtt['noQuote'] == 'FALSE': place = '"' + place + '"'
        s2 += '  static const std::string& %s = %s;\n' % ( locAtt['name'], place )
    if len(s2):                                                                  # if found something put namespace around it
      s =  '// Namespace for locations in TDS\n'
      s += 'namespace %sLocation {\n%s}\n' % ( classAtt['name'], s2 )
    return s
#--------------------------------------------------------------------------------
  def genInheritance(self, godClass):
    s = ''
    if godClass.has_key('base'):
      s += ': '
      for base in godClass['base']:
        baseAtt = base['attrs']
        if baseAtt['name'][:12] == 'KeyedObject<' :
          self.gKeyedContainerTypedef = 1
          self.addInclude('GaudiKernel/KeyedObject')
        if baseAtt['name'] == 'ContainedObject' :
          self.gContainedObjectTypedef = 1
          self.addInclude('GaudiKernel/ContainedObject')
        if baseAtt['name'] == 'DataObject' :
          self.addInclude('GaudiKernel/DataObject')
        if baseAtt['virtual'] == 'TRUE': s += 'virtual '
        s += '%s %s' % ( baseAtt['access'].lower(), baseAtt['name'] )
    return s
#--------------------------------------------------------------------------------
  def genConstructor(self, godClass, const, scopeName=''):
    s = ''
    indent = 0
    if (scopeName and
        ((not const.has_key('code')) or
         (not const['code'][0].has_key('cont')) or
         (not const['code'][0]['cont'].strip()))): return s                         # if outside class body and no implementation
    constAtt = const['attrs']                                                    # return immediately
    if ( not scopeName ) :
      s += '  /// %s\n  ' % constAtt['desc']
      if constAtt['explicit'] == 'TRUE' : s += 'explicit '
      indent += 2
    else :
      s += 'inline '
      scopeName += '::'
      indent += len(scopeName) + 7
    s += scopeName + godClass['attrs']['name'] + '('
    indent += len(godClass['attrs']['name'])
    pList = []
    if constAtt.has_key('argList') : pList = self.tools.genParamsFromStrg(constAtt['argList'])
    if const.has_key('arg') :        pList = self.tools.genParamsFromElem(const['arg'])
    pIndent = 0
    if len(pList) :
      if scopeName : s += pList[0].split('=')[0]                                # in implementation strip off default arguments
      else         : s += pList[0]
      pIndent = len(pList[0])
      if len(pList[1:]) :
        for p in pList[1:]:
          if scopeName : s += ',\n%s %s' % (indent*' ', p.split('=')[0])        # in implementation strip off default arguments
          else : s += ',\n%s %s' % (indent*' ', p)
          pIndent = max(pIndent,len(p))
    s += ')'
    indent += pIndent+1
    if ( not scopeName ) :                                                      
      if (constAtt.has_key('initList') and not const.has_key('code')) or \
           (const.has_key('code') and \
             ((not const['code'][0].has_key('cont')) or \
              (not const['code'][0]['cont'].strip()))):
        if constAtt.has_key('initList') :
          initList = constAtt['initList'].split(',')
          s += ' : %s' % initList[0]                                             # print initialisation list if there is one
          indent += 3
          if len(initList) > 1 :
            for init in initList[1:]:
              s += ',\n%s%s' % (indent*' ', init)
        s += ' {}\n\n'                                                           # and the empty implementation
      else : s += ';\n\n'                                                        # implementation must be in cpp file
    else :                                                                       # we are outside the class body 
      if constAtt.has_key('initList') :
        initList = constAtt['initList'].split(',')
        s += ' : %s' % initList[0]                                               # print initlist if available
        indent += 3
        if len(initList) > 1:
          for init in initList[1:]:
            s += ',\n%s%s' % (indent*' ', init)
      s += ' \n{\n%s\n}\n\n' % const['code'][0]['cont']                          # print code
    return s
#--------------------------------------------------------------------------------
  def genConstructors(self,godClass,clname=''):
    s = ''
    cname = godClass['attrs']['name']
    hasDefaultConstructor = 0
    hasCopyConstructor = 0
    if godClass.has_key('constructor'):                                         # are there any constrs defined
      for const in godClass['constructor']:
        if (not const['attrs'].has_key('argList')) and (not const.has_key('arg')):
          hasDefaultConstructor = 1
        s += self.genConstructor(godClass,const,clname)
    if not (hasDefaultConstructor or clname):                                   # no constructors defined lets
      s += '  /// Default Constructor\n'                                        # generate a default ctr
      s2 = '  %s()' % cname
      indent = ' ' * (len(s2) + 3)
      s += s2
      if godClass.has_key('attribute') :                                        # if there are attributes
        for att in godClass['attribute'] :                                      # loop over them
          attAtt = att['attrs']
          if ( s[-1] != ',' ) : s += ' : '                                     # this is the first item
          else : s += '\n' + indent
          s += 'm_%s' % attAtt['name'] 
          if attAtt.has_key('init')                        : s += '(%s),' % attAtt['init'] 
          elif self.tools.isIntegerT(attAtt['type']) or \
               self.tools.isBitfieldT(attAtt['type'])      : s += '(0),'
          elif self.tools.isFloatingPointT(attAtt['type']) : s += '(0.0),'
          else                                             : s += '(),'
        if s[-1] == ',' : s = s[:-1]                                             # strip off the last ','
      s += ' {}\n\n'
    if godClass.has_key('copyconstructor'):
      if not clname :
        s += '  /// Copy Constructor\n'
        s += '  %s(const %s & rh);\n\n' % ( cname, cname )
      else:
        s += 'inline %s::%s(const %s & rh) : \n' % (clname, cname, clname)
        if godClass.has_key('base'):
          for b in godClass['base']:
            bname = b['attrs']['name']
            if bname.find('KeyedObject') != -1 : s += '   %s(),\n' % bname
            else                               : s += '   %s(rh),\n' % bname
        if godClass.has_key('attribute'):
          for a in godClass['attribute']:
            aname = a['attrs']['name']
            s += '   m_%s( rh.m_%s ),\n' % ( aname , aname )
        if godClass.has_key('relation') :
          for r in godClass['relation']:
            rname = r['attrs']['name']
            s += '   m_%s( rh.m_%s ),\n' % ( rname, rname )
        s = s[:-2] + '\n   {}\n\n'
    if godClass.has_key('assignmentoperator'):
      if not clname:
        s += '   /// Assignment operator\n'
        s += '   %s & operator=(const %s & rh);\n\n' % (cname, cname)
      else:
        s += 'inline %s & %s::operator=(const %s & rh) {\n' % (clname, clname, clname)
        s += '  if ( this != &rh ) {\n'
        maxlen = 0;
        mlist = []
        if godClass.has_key('attribute'):
          for a in godClass['attribute']:
            aname = 'm_'+a['attrs']['name']
            maxlen = max(maxlen,len(aname))
            mlist.append(aname)
        if godClass.has_key('relation'):
          for r in godClass['relation']:
            rname = 'm_'+r['attrs']['name']
            maxlen = max(maxlen,len(rname))
            mlist.append(rname)
        for m in mlist:
          s += '    %s = rh.%s;\n' % ( m.ljust(maxlen), m )
        s += '  }\n  return *this;\n}\n\n'
    return s[:-1]
#--------------------------------------------------------------------------------
  def genDestructor(self,godClass,dest,scopeName=''):
    s = ''
    if (scopeName and not dest.has_key('code')) : return s
    if ( not scopeName ) :
      s += '  /// %s\n  ' % dest['attrs']['desc']
      if dest['attrs']['virtual'] == 'TRUE' : s += 'virtual '
    else :
      s += 'inline '
      scopeName += '::'
    s += '%s~%s()' % (scopeName, godClass['attrs']['name'])
    if ( not scopeName ) :
      s += ';\n\n'
    else :
      src = dest['code'][0].get('cont')
      if src == None : src = ''
      s += ' \n{\n%s\n}\n\n' % src
    return s
#--------------------------------------------------------------------------------
  def genDestructors(self,godClass,clname=''):
    s = ''
    if godClass.has_key('destructor'):                                          # there is a destructor defined
      dest = godClass['destructor'][0]
      s += self.genDestructor(godClass, dest,clname)
    elif not clname:                                                             # no destructor defined let's
      virt = 'virtual'
      if godClass['attrs']['virtual'] == 'FALSE' : virt = ''
      s += '  /// Default Destructor\n'
      s += '  %s ~%s() {}\n\n' % (virt, godClass['attrs']['name'])
    return s[:-1]
#--------------------------------------------------------------------------------
  def genGetSetAttMethod(self,att,what,scopeName=''):
    desc = {'get':'Retrieve ', 'get_c':'Retrieve const ', 'set':'Update '}
    s = ''
    if ( what == 'get' and self.tools.isFundamentalT(att['type'])) : return s     # for attributes with fundamental types we 
    if ( not scopeName ) : s += '  /// %s %s\n  ' % (desc[what], att['desc'])        # ... only generate the const getter 
    else : 
      s += 'inline '
      scopeName += '::'
    metName = self.tools.lowerGetterName(att['name'])
    if what == 'set' : metName = att['name'][0].upper() + att['name'][1:]
    ret = ''
    param = ''
    constF = ''
    if what == 'get_c' : 
      constF = ' const' 
      if not self.tools.isFundamentalT(att['type']) : ret = 'const '
    if what == 'set'  : 
      ret = 'void '
      param = self.tools.genParamFromStrg(att['type']) + ' value'
    elif what in ['get','get_c'] : 
      ret = self.tools.genReturnFromStrg(ret + att['type'], self.generatedTypes, scopeName)
      if not self.tools.isFundamentalT(att['type']) and not self.tools.isPointerT(att['type']): ret += '&'
      ret += ' '
      what = ''
    s += ret + scopeName + what + metName + '(' + param + ')' + constF
    if ( not scopeName )        : s += ';\n\n'                                     # this is a declaration
    else : 
      if not what        : s += ' \n{\n  return m_%s;\n}\n\n' % att['name']        # what was set to '' if getter
      elif what == 'set' : s += ' \n{\n  m_%s = value;\n}\n\n' % att['name'] 
    return s
#--------------------------------------------------------------------------------
  def genGetSetRelMethod(self,rel,what,scopeName=''):
    desc = {'get':'Retrieve ', 'get_c':'Retrieve (const) ', 'set':'Update ', \
            'set_p':'Update (pointer) ', 'addTo':'Add to ', \
            'addTo_p':'Att to (pointer) ', 'removeFrom':'Remove from ', \
            'removeFrom_p':'Remove from (pointer)', 'clear':'Clear '}
    s = ''
    if ( not scopeName ) : s += '  /// %s %s\n  ' % (desc[what], rel['desc'])
    else : 
      s += 'inline '
      scopeName += '::'
    ret = 'void '
    if what in ['get', 'get_c']:
      if rel['multiplicity'] != '1' : 
        ret = self.tools.genReturnFromStrg('SmartRefVector<'+rel['type']+'>&',self.generatedTypes,scopeName) + ' '
      else : 
        ret = self.tools.genReturnFromStrg(rel['type'] + '*',self.generatedTypes,scopeName) + ' '
    metName = ''
    if what in ['get', 'get_c'] :
      metName = self.tools.lowerGetterName(rel['name'])
    if what in ['set', 'set_p', 'addTo', 'addTo_p', 'removeFrom', 'removeFrom_p', 'clear'] :
      metName = rel['name'][0].upper() + rel['name'][1:]
    param = ''
    if what == 'set' and rel['multiplicity'] != '1': 
      param = self.tools.genParamFromStrg('SmartRefVector<'+rel['type']+'>') + ' value'
    elif what in ['set','addTo', 'removeFrom'] : 
      param = self.tools.genParamFromStrg('SmartRef<'+rel['type']+'>') + ' value'
    elif what in ['set_p', 'addTo_p', 'removeFrom_p']:
      param = self.tools.genParamFromStrg('const ' + rel['type'] + '*') + ' value'
    constF = ''
    if what == 'get_c' : 
        constF = ' const'
        ret = 'const ' + ret
    if what[-2] == '_' : what = what[:-2]
    if what == 'get' : what = ''
    s += ret + scopeName + what + metName + '(' + param + ')' + constF
    if ( not scopeName )       : s += ';\n\n'                                       # this is a declaration
    else : 
      if not what              : s += ' \n{\n  return m_%s;\n}\n\n' % rel['name'] # what was set to '' if getter
      elif what == 'set'       : s += ' \n{\n  m_%s = value;\n}\n\n' % rel['name'] 
      elif what == 'addTo'     : s += ' \n{\n  m_%s.push_back(value);\n}\n\n' % rel['name']
      elif what == 'clear'     : s += ' \n{\n  m_%s.clear();\n}\n\n' % rel['name']
      elif what == 'removeFrom': 
        s += ' \n{\n  SmartRefVector<%s>::iterator iter =\n' % rel['type']
        s += '    std::remove(m_%s.begin(), m_%s.end(), value);\n' % (rel['name'], rel['name'])
        s += '  m_%s.erase(iter, m_%s.end());\n}\n\n' % (rel['name'], rel['name'])
    return s
#--------------------------------------------------------------------------------
  def genGetSetBitfieldMethod(self, bf, what, attAtt, scopeName):
    desc = {'get':'Retrieve', 'set':'Update', 'check':'Check'}
    s = ''
    if not scopeName : s += '  /// %s %s\n  ' % (desc[what], bf['desc'])
    else :
      s += 'inline '
      scopeName += '::'
    metName = bf['name']
    bfType = 'bool'
    if bf.has_key('type')    : bfType = bf['type']
    elif bf['length'] != '1' : bfType = attAtt['type']
    ret = 'void '
    param = ''
    constF = ''
    if what == 'set' :
      metName = bf['name'][0].upper() + bf['name'][1:]
      param = self.tools.genParamFromStrg(bfType) + ' value'
    if what == 'get':
      metName = self.tools.lowerGetterName(metName)
      what = ''
      ret = self.tools.genReturnFromStrg(bfType,self.generatedTypes,scopeName) + ' '
      constF = ' const'
    if what == 'check':
      ret = 'bool '
      metName = bf['name'][0].upper() + bf['name'][1:]
      param = self.tools.genParamFromStrg(bfType) + ' value'
      constF = ' const'
    s += ret + scopeName + what + metName + '(' + param + ')' + constF
    if not scopeName : s += ';\n\n'
    else:
      if bf['length'].isdigit() : bits = bf['name']
      else : bits = bf['length'].split(',')[0]
      if not what        :
        retF = '0 != '                            
        if (ret[:-1]) != 'bool' : retF = '(%s)'%ret[:-1]
        s += '\n{\n  return %s((m_%s & %sMask) >> %sBits);\n}\n\n' % (retF, attAtt['name'],bf['name'],bits)
      elif what == 'set' :
        s += '\n{\n  unsigned int val = (unsigned int)value;\n' 
        if bf['exclusive'] == 'TRUE' : s += '  m_%s &= ~%sMask;\n' % (attAtt['name'], bf['name']) 
        s += '  m_%s |= ((((unsigned int)val) << %sBits) & %sMask);\n}\n\n' % (attAtt['name'],bits,bf['name'])
      elif what == 'check':
        s += '\n{\n  unsigned int val = (unsigned int)value;\n'
        s += '  return 0 != ( m_%s & %sMask & ( val << %sBits ));\n}\n\n' % ( attAtt['name'], bf['name'], bf['name'] )
    return s
#--------------------------------------------------------------------------------
  def genGetSetMethods(self,godClass,clname=''):
    s = ''
    if godClass.has_key('attribute'):
      for att in godClass['attribute']:
        attAtt = att['attrs']
        if attAtt['getMeth'] == 'TRUE': 
          s += self.genGetSetAttMethod(attAtt,'get_c',clname)
	  if attAtt['nonconstaccessor'] == 'TRUE' :
	    s += self.genGetSetAttMethod(attAtt,'get',clname)
        if attAtt['setMeth'] == 'TRUE':
          s += self.genGetSetAttMethod(attAtt,'set',clname)
        if att.has_key('bitfield'):
          for bf in att['bitfield']:
            bfAtt = bf['attrs']
            if bfAtt['getMeth'] == 'TRUE':
              s += self.genGetSetBitfieldMethod(bfAtt,'get',attAtt,clname)
            if bfAtt['setMeth'] == 'TRUE':
              s += self.genGetSetBitfieldMethod(bfAtt,'set',attAtt,clname)
            if bfAtt['checkMeth'] == 'TRUE':
              s += self.genGetSetBitfieldMethod(bfAtt,'check',attAtt,clname)
    if godClass.has_key('relation'):
      for rel in godClass['relation']:
        relAtt = rel['attrs']
        if relAtt['getMeth'] == 'TRUE': 
          s += self.genGetSetRelMethod(relAtt,'get_c',clname)
	  if relAtt['nonconstaccessor'] == 'TRUE' :
	    s += self.genGetSetRelMethod(relAtt,'get',clname)
        if relAtt['setMeth'] == 'TRUE':
          s += self.genGetSetRelMethod(relAtt,'set',clname)
        if relAtt['multiplicity'] != '1':
          if relAtt['addMeth'] == 'TRUE':
            s += self.genGetSetRelMethod(relAtt,'addTo',clname) 
            s += self.genGetSetRelMethod(relAtt,'addTo_p',clname) 
          if relAtt['remMeth'] == 'TRUE':
            s += self.genGetSetRelMethod(relAtt,'removeFrom',clname)
            #s += self.genGetSetRelMethod(relAtt,'removeFrom_p',clname) #fixme: ambigous operator== overloading
          if relAtt['clrMeth'] == 'TRUE':
            s += self.genGetSetRelMethod(relAtt,'clear',clname)
        else:
          if relAtt['setMeth'] == 'TRUE':
            s += self.genGetSetRelMethod(relAtt,'set_p',clname)
    return s[:-1]
#--------------------------------------------------------------------------------
  def genBitfield(self,att):
    s = ''
    attName = att['attrs']['name']
    if att.has_key('bitfield'):
      maxLenName = 0
      for bf in att['bitfield'] : maxLenName = max(maxLenName,len(bf['attrs']['name']))
      bf0Att = att['bitfield'][0]['attrs']
      indent = (len(attName) + 11) * ' '
      s += '  /// Offsets of bitfield %s\n' % attName
      s += '  enum %sBits{%s = 0' % (attName, (bf0Att['name']+'Bits').ljust(maxLenName+4))
      offset = string.atoi(bf0Att['length'])
      for bf in att['bitfield'][1:]:
        bfAtt = bf['attrs']
        if bfAtt['length'].isdigit():
          s += ',\n%s %s = %d' % (indent, (bfAtt['name']+'Bits').ljust(maxLenName+4), offset)
          offset += string.atoi(bfAtt['length'])
      s += '};\n\n'
      s += '  /// Bitmasks for bitfield %s\n' % attName
      offset = 0
      indent += '  '
      s += '  enum %sMasks{' % attName
      for bf in att['bitfield']:
        if s[-1] != '{' : s += ',\n%s' % indent
        bfAtt = bf['attrs']
        if bfAtt['length'].isdigit():
          lInt = string.atoi(bfAtt['length'])
          mask = 0
          for j in range(offset,offset+lInt) : mask += (1L<<j)
          offset += lInt
          s += '%s = %s' % ((bfAtt['name']+'Mask').ljust(maxLenName+4), hex(mask))
        else:
          masks = bfAtt['length'].split(',')
          mask = ''
          for m in masks : mask += m.strip() + 'Mask + '
          s += '%s = %s' % ((bfAtt['name']+'Mask').ljust(maxLenName+4), mask[:-3])
      s += '\n%s};\n\n' % indent[1:]
    return s
#--------------------------------------------------------------------------------
  def genClassIDFun(self,godClass,scopeName='') :
    s = ''
    if godClass['attrs'].has_key('id') :                                        # then we know that it is an event class
      if (not scopeName) :                                                       # we are inside the class (declaration)
        s  = '  // Retrieve pointer to class definition structure\n'
        s += '  virtual const CLID& clID() const;\n'
        s += '  static const CLID& classID();\n'
      else :                                                                     # we are outside the class (definition)
        s  = 'inline const CLID& %s::clID() const\n{\n' % scopeName 
        s += '  return %s::classID();\n}\n\n' % scopeName                        
        s += 'inline const CLID& %s::classID()\n{\n' % scopeName 
        s += '  return CLID_%s;\n}\n' % scopeName.split('::')[-1]
    return s                                                                    
#--------------------------------------------------------------------------------
  def genClassTypedefs(self, godClass):
    s = ''
    classname =  godClass['attrs']['name']
    if self.gKeyedContainerTypedef or godClass['attrs']['keyedContTypeDef'] == 'TRUE':
      self.addInclude('GaudiKernel/KeyedContainer')
      s += '/// Definition of Keyed Container for %s\n' % classname
      s += 'typedef KeyedContainer<%s, Containers::HashMap> %s;\n' \
           % (classname, self.genClassnamePlurial(classname))
    if self.gContainedObjectTypedef or godClass['attrs']['contObjectTypeDef'] == 'TRUE':
      self.addInclude('GaudiKernel/ObjectVector')
      s += '/// Definition of vector container type for %s\n' % classname
      s += 'typedef ObjectVector<%s> %s;\n' % (classname, self.genClassnamePlurial(classname))
    return s
#--------------------------------------------------------------------------------
  def genClassContainerTypedefs(self, godClass):
    s = ''
    classname = godClass['attrs']['name']
    if godClass['attrs']['stdVectorTypeDef'] == 'TRUE':
      self.addInclude('vector',1)
      s += '  /// typedef for std::vector of %s\n' % classname
      s += '  typedef std::vector<%s*> Vector;\n' % ( classname )
      s += '  typedef std::vector<const %s*> ConstVector;\n\n' % ( classname )
    if self.gKeyedContainerTypedef or godClass['attrs']['keyedContTypeDef'] == 'TRUE':
      self.addInclude('GaudiKernel/KeyedContainer')
      s += '  /// typedef for KeyedContainer of %s\n' % classname
      s += '  typedef KeyedContainer<%s, Containers::HashMap> Container;\n' % (classname)
    if self.gContainedObjectTypedef or godClass['attrs']['contObjectTypeDef'] == 'TRUE':
      self.addInclude('GaudiKernel/ObjectVector')
      s += '/// typedef for ObjectVector of %s\n' % classname
      s += 'typedef ObjectVector<%s> Container;\n' % (classname)
    return s
#--------------------------------------------------------------------------------
  def genStreamer(self, godClass, className=''):
    s = ''
    if not className and godClass['attrs']['serializers'] == 'TRUE':
      if godClass.has_key('method'):
        for met in godClass['method']:
          metAtt = met['attrs']
          metName = metAtt['name']
          if metName in ('operator<<','fillStream'):
            ret = ''
            if metAtt.has_key('type')  :
              ret = self.tools.genReturnFromStrg(metAtt['type'],self.generatedTypes,godClass['attrs']['name'])
            elif met.has_key('return') :
              ret = self.tools.genReturnFromElem(met['return'],self.generatedTypes,godClass['attrs']['name'])
            if metName == 'operator<<' and ret == 'std::ostream&' : self.genOStream = 0
            if metName == 'fillStream' and ret == 'std::ostream&' : self.genFillStream = 0
    else :
      className += '::'
    if self.genFillStream:
      self.addInclude('ostream',1)
      virt = 'virtual '
      if godClass['attrs']['virtual'] == 'FALSE' : virt = ''
      if className : s += 'inline '
      else : s += '  /// Fill the ASCII output stream\n %s' % virt
      s += 'std::ostream& %sfillStream(std::ostream& s) const' % className
      if not className : s += ';\n'
      else:
        s += '\n{\n'
        if godClass.has_key('attribute'):
          for att in godClass['attribute']:
            attAtt = att['attrs']
            if attAtt['type'] == 'bool':
              s += "  char l_%s = (m_%s) ? 'T' : 'F';\n" % (attAtt['name'], attAtt['name'])
        if godClass.has_key('base'):
          for b in godClass['base']:
            if b['attrs']['name'].split('<')[0] not in ('ContainedObject', 'DataObject', 'KeyedObject'):
              s += '  %s::fillStream(s);\n' % b['attrs']['name']
        if godClass.has_key('attribute'):
          s += '  s << "{ "'
          for att in godClass['attribute']:
            attAtt = att['attrs']
            if attAtt['type'][:5] == 'std::' and 'Kernel/SerializeStl' not in self.include:
              self.include.append('Kernel/SerializeStl')
            if s[-1] == '"' : s += ' << "%s :\t" ' % attAtt['name']
            else            : s += '\n            << "%s :\t" ' % attAtt['name']
            if   attAtt['type'] == 'bool'   : s += '<< l_'
            elif attAtt['type'] == 'double' : s += '<< (float)m_'
            else                            : s += '<< m_'
            s += attAtt['name'] + ' << std::endl'
          s += ' << " }";\n'
        s += '  return s;\n'
        s += '}\n\n'
    return s
#--------------------------------------------------------------------------------
  def genClassOstreamOverload(self, godClass):
    s = ''
    if not self.isEventClass and self.genOStream:
      self.addInclude('ostream',1)
      s += 'inline std::ostream& operator<< (std::ostream& str, const %s& obj)' % ( godClass['attrs']['name'])
      s += '\n{\n  return obj.fillStream(str);\n}\n'
    return s
#--------------------------------------------------------------------------------
  def genAllocatorOperators(self, godClass,allocatorType):
    s = ''
    if allocatorType == "FROMXML":
      allocatorType = godClass['attrs']['allocator']

    if allocatorType == 'DEFAULT' :
      # set the default allocator type
      allocatorType = 'BOOST'
        
    data = {}
    data['classname'] = godClass['attrs']['name']

    if allocatorType == 'BOOST': # Boost allocator with check on delete
      s ="""
#ifndef _WIN32
  /// operator new
  static void* operator new ( size_t size )
  {
    return ( sizeof(%(classname)s) == size ? 
             boost::singleton_pool<%(classname)s, sizeof(%(classname)s)>::malloc() :
             ::operator new(size) );
  }

  /// placement operator new
  /// it is needed by libstdc++ 3.2.3 (e.g. in std::vector)
  /// it is not needed in libstdc++ >= 3.4
  static void* operator new ( size_t size, void* pObj )
  {
    return ::operator new (size,pObj);
  }

  /// operator delete
  static void operator delete ( void* p )
  {
    boost::singleton_pool<%(classname)s, sizeof(%(classname)s)>::is_from(p) ?
    boost::singleton_pool<%(classname)s, sizeof(%(classname)s)>::free(p) :
    ::operator delete(p);
  }

  /// placement operator delete
  /// not sure if really needed, but it does not harm
  static void operator delete ( void* p, void* pObj )
  {
    ::operator delete (p, pObj);
  }
#endif"""%data
      self.include.append("GaudiKernel/boost_allocator.h")
      
    elif allocatorType == 'BOOST2': # Boost allocator without check on delete
      s ="""
#ifndef _WIN32
  /// operator new
  static void* operator new ( size_t size )
  {
    return ( sizeof(%(classname)s) == size ? 
             boost::singleton_pool<%(classname)s, sizeof(%(classname)s)>::malloc() :
             ::operator new(size) );
  }

  /// placement operator new
  /// it is needed by libstdc++ 3.2.3 (e.g. in std::vector)
  /// it is not needed in libstdc++ >= 3.4
  static void* operator new ( size_t size, void* pObj )
  {
    return ::operator new (size,pObj);
  }

  /// operator delete
  static void operator delete ( void* p )
  {
    boost::singleton_pool<%(classname)s, sizeof(%(classname)s)>::free(p);
  }

  /// placement operator delete
  /// not sure if really needed, but it does not harm
  static void operator delete ( void* p, void* pObj )
  {
    ::operator delete (p, pObj);
  }
#endif"""%data
      self.include.append("GaudiKernel/boost_allocator.h")
      
    elif allocatorType == 'DEBUG': # Boost allocator with check on delete and debug print-out
      s = """
#ifndef _WIN32
  /// operator new
  static void* operator new ( size_t size )
  {
    void *ptr = sizeof(%(classname)s) == size ? 
      boost::singleton_pool<%(classname)s, sizeof(%(classname)s)>::malloc() :
      ::operator new(size);
    std::cout << "%(classname)s::new() -> " << ptr << std::endl;
    return ( ptr );
  }

  /// placement operator new
  /// it is needed by libstdc++ 3.2.3 (e.g. in std::vector)
  /// it is not needed in libstdc++ >= 3.4
  static void* operator new ( size_t size, void* pObj )
  {
    std::cout << "%(classname)s::new(" << pObj << ")" << std::endl;
    return ::operator new (size,pObj);
  }

  /// operator delete
  static void operator delete ( void* p )
  {
    std::cout << "%(classname)s::delete(" << p << ") "
              << boost::singleton_pool<%(classname)s, sizeof(%(classname)s)>::is_from(p)
              << std::endl;
    boost::singleton_pool<%(classname)s, sizeof(%(classname)s)>::is_from(p) ?
    boost::singleton_pool<%(classname)s, sizeof(%(classname)s)>::free(p) :
    ::operator delete(p);
  }

  /// placement operator delete
  /// not sure if really needed, but it does not harm
  static void operator delete ( void* p, void* pObj )
  {
    std::cout << "%(classname)s::delete(" << p << "," << pObj << ") " << std::endl;
    ::operator delete (p, pObj);
  }
#endif"""%data
      self.include.append("GaudiKernel/boost_allocator.h")
      self.addInclude('iostream',1)
      
    return s
##--------------------------------------------------------------------------------
  def doit(self,package,godClasses,outputDir,lname,allocatorType = 'FROMXML'):

    for godClass in godClasses:

      self.reset(package,godClass)
      self.parseClassImport(godClass)

      classDict = package.dict
      classname = godClass['attrs']['name']

      # Get/set class namespace to package namespace if not set in class
      try:
        namespace = godClass['attrs']['namespace']
      except KeyError:
        namespace = package.dict['packagenamespace']
        godClass['attrs']['namespace'] = namespace

      scoped_classname = namespace+'::'+classname

      fileName = '%s.h' % classname

      print '    File %s' % fileName.ljust(lname),

      classDict['classname']                    = classname
      classDict['classID']                      = self.genClassID(godClass)
      classDict['classVersion']                 = self.genClassVersion(godClass)
      classDict['locations']                    = self.genLocations(godClass)
      classDict['desc']                         = self.genDesc(godClass)
      classDict['author']                       = godClass['attrs']['author']
      classDict['today']                        = time.ctime()
      classDict['classNamespace']               = namespace
      classDict['inheritance']                  = self.genInheritance(godClass)
      classDict['classContainerTypedefs']       = self.genClassContainerTypedefs(godClass)
      classDict['classTypedefs']                = self.genClassTypedefs(godClass)
      classDict['constructorDecls']             = self.genConstructors(godClass)
      classDict['destructorDecl']               = self.genDestructors(godClass)
      classDict['classIDDecl']                  = self.genClassIDFun(godClass)
      classDict['allocatorOperators']           = self.genAllocatorOperators(godClass,allocatorType)
      for modifier in ['public','protected','private']:
        classDict[modifier+'Typedefs']          = self.genTypedefs(modifier,godClass)
        classDict[modifier+'Attributes']        = self.genAttributes(modifier,godClass)
        classDict[modifier+'BitfieldEnums']     = self.bitfieldEnums[modifier]
        classDict[modifier+'Enums']             = self.genEnums(modifier,godClass)
        classDict[modifier+'MethodDecls']       = self.genMethods(modifier,godClass)
        classDict[modifier+'MethodDefs']        = self.genMethods(modifier,godClass,scoped_classname)
      classDict['streamerDecl']                 = self.genStreamer(godClass)
      classDict['streamerDef']                  = self.genStreamer(godClass,scoped_classname)
      classDict['classOstreamOverload']         = self.genClassOstreamOverload(godClass)
      classDict['enumOstreamOverloads']         = self.genEnumOstreamOverloads(godClass, scoped_classname)
      classDict['getSetMethodDecls']            = self.genGetSetMethods(godClass)
      classDict['constructorDefs']              = self.genConstructors(godClass,scoped_classname)
      classDict['destructorDef']                = self.genDestructors(godClass,scoped_classname)
      classDict['classIDDef']                   = self.genClassIDFun(godClass,scoped_classname)
      classDict['getSetMethodDefs']             = self.genGetSetMethods(godClass,scoped_classname)
      defs,maps,dcls = self.genEnumConversions(godClass, scoped_classname)
      classDict['enumConversionDefs']  = defs
      classDict['enumConversionMaps']  = maps
      classDict['enumConversionDecls'] = dcls
      classDict['includes']                     = self.genIncludes()
      classDict['forwardDeclsGlob']             = self.genForwardDeclsGlob()
      classDict['forwardDeclsLHCb']             = self.genForwardDeclsLHCb()
      classDict['forwardIncludes']              = self.genForwardIncludes(classname)

      g = gparser.gparser()
      g.parse(self.godRoot+'templates/header.tpl',classDict)

      file = open(outputDir+os.sep+fileName,'w')
      file.write(g.stream)
      file.close()

      print ' - Done'
      
#================================================================================
#import xparser
#if __name__ == '__main__':
#  x = xparser.xparser('xml_files/Event.xml','xml_files/GODsClassDB.xml')
#  x.parse()
#  g = genClasses(x.gdd)
#  g.doit()
