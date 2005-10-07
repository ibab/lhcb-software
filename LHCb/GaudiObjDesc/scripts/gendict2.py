# Copyright CERN, CH-1211 Geneva 23, 2004-2005, All rights reserved.
#
# Permission to use, copy, modify, and distribute this software for any
# purpose is hereby granted without fee, provided that this copyright and
# permissions notice appear in all copies and derivatives.
#
# This software is provided "as is" without express or implied warranty.

import xml.parsers.expat
import os, sys, string, time
import gccdemangler, gendict
class genDict2(gendict.genDict) :
#----------------------------------------------------------------------------------
  def __init__(self, hfile, opts):
    gendict.genDict.__init__(self, hfile, opts.get('pool',False) )
    self.typeids            = []
    self.comments           = opts.get('comments', False)
    self.no_membertypedefs  = opts.get('no_membertypedefs', False)
    self.selectionname      = 'seal::reflex::selection'
#----------------------------------------------------------------------------------
  def generate(self, file, sfile, selclasses, selfunctions) :
    names = []
    f = open(file,'w') 
    f.write(self.genHeaders(False))
    if sfile :
      fs = open(sfile,'w')
      fs.write(self.genHeaders(True))
    f_buffer = ''
    for c in selclasses :
      if 'incomplete' not in c :
        cname = self.genTypeName(c['id'])
        print  'class '+ cname
        names.append(cname)
        self.completeClass( c )
        self.enhanceClass( c )
        sdecl, scons, stubs   = self.genClassDict( c )
        f_buffer += sdecl
        f_buffer += scons
        #f.write(sdecl)
        #f.write(scons)
        if sfile :
          fs.write(sdecl)
          fs.write(stubs)
        else :
          #f.write(stubs)
          f_buffer += stubs
    f_buffer += self.genFunctionsStubs( selfunctions )
    f.write(self.genAllTypes())
    f.write(f_buffer)
    f.write(self.genInstantiateDict(selclasses, selfunctions))
    if sfile : fs.close()
    f.close()
    return names
#----------------------------------------------------------------------------------
  def add_template_defaults (self, c, selection):
    tlist = []
    for f in self.get_fields (selection):
        tid = f['type']
        tname = self.genTypeName (tid)
        if tname.startswith (self.selectionname+"::TEMPLATE_DEFAULTS"):
          tid = {'id': tid}
          nodefault_tid = self.has_typedef (tid, 'nodefault')
          i = 1
          while 1:
            arg = self.has_typedef (tid, 't%d' % i)
            if not arg:
              break
            if arg == nodefault_tid:
              tlist.append ('=')
            else:
              tlist.append (self.genTypeName (arg))
            i += 1
    if tlist:
      name = self.xref[c['id']]['attrs']['name']
      i = name.find ('<')
      if i>=0 : name = name[:i]
      stldeftab[name] = tuple (tlist)
    return
#----------------------------------------------------------------------------------
  def isUnnamedType(self, name) :
    if name.find('.') != -1 or name.find('$') != -1 : return 1
    else                                            : return 0
#----------------------------------------------------------------------------------
  def genHeaders(self, clean):
    c =  '// Generated at %s. Do not modify it\n\n' % time.ctime(time.time())
    c += '#ifdef _WIN32\n'
    c += '#pragma warning ( disable : 4786 )\n'
    c += '#ifndef LCGDICT_STRING \n'
    c += '#include <string> // Included here since it is sensitive to private->public trick\n'
    c += '#endif\n'
    c += '#endif\n'
    if not clean : c += '#define private public\n'
    if not clean : c += '#define protected public\n'
    c += '#include "%s"\n' % self.hfile
    if not clean : c += '#undef private\n'
    if not clean : c += '#undef protected\n'
    c += '#include "Reflex/Builder/ReflexBuilder.h"\n'
    c += '#include <typeinfo>\n'
    c += 'namespace ROOT { namespace Reflex { } }\n'
    c += 'namespace seal { namespace reflex { using namespace ROOT::Reflex; } }\n'
    c += 'using namespace seal::reflex;\n\n'
    return c
#----------------------------------------------------------------------------------
  def genInstantiateDict( self, selclasses, selfunctions) :
    c = 'namespace {\n  struct Dictionaries {\n    Dictionaries() {\n'
    c += self.genNamespaces(selclasses)
    c += self.genFunctions(selfunctions)
    for attrs in selclasses :
      if 'incomplete' not in attrs : 
        cl  = attrs['name']
        clf = self.genTypeName(attrs['id'], colon=True)
        clt = string.translate(str(clf), self.transtable)
        c += '      %s_dict(); \n' % (clt)
    c += '    }\n  };\n'
    c += '  static Dictionaries instance;\n}\n'
    return c
#---------------------------------------------------------------------------------
  def genClassDict(self, attrs):
    members, bases = [], []
    cl  = attrs['name']
    clf = self.genTypeName(attrs['id'],colon=True)
    cls = self.genTypeName(attrs['id'])
    clt = string.translate(str(clf), self.transtable)
    bases = self.getBases( attrs['id'] )
    if 'members' in attrs : members = string.split(attrs['members'])
    mod = self.genModifier(attrs,None)
    mod += ' | ' + self.xref[attrs['id']]['elem'].upper()
    if attrs.has_key('abstract') : mod += ' | ABSTRACT'
    if self.vtables :
      if attrs['id'] in self.vtables : mod += ' | VIRTUAL'
    else :  # new in version 0.6.0
      if self.isClassVirtual(attrs) :  mod += ' | VIRTUAL'
    members = filter(self.memberfilter, members)  # Eliminate problematic members
    # Fill the different streams sd: class declaration, sc: constructor, ss: stub functions
    sd = '//------Dictionary for class %s -------------------------------\n' % cl
    sd +=  'class %s_dict { \n' % clt
    sd += '  public:\n'
    sd += '  %s_dict();\n' % clt
    if not self.isUnnamedType(clf):
      for m in members :
        funcname = 'gen'+self.xref[m]['elem']+'Decl'
        if hasattr(self, funcname) :
          sd += '  ' + getattr(self, funcname)(self.xref[m]['attrs'], self.xref[m]['subelems']) + '\n'
    sd += '};\n'
    #sc = 'static %s_dict instance%s;\n\n' % ( clt, attrs['id'])
    sc = '\n\n%s_dict::%s_dict() {\n' % (clt, clt)
    if 'extra' in attrs and 'contid' in attrs['extra'] : 
      cid = attrs['extra']['contid'].upper()
    else :
      cid = getContainerId(clf)[0]
    if self.isUnnamedType(clf) : 
      sc += '  ClassBuilderNT("%s", typeid(Unnamed%s), 0, %s)' % (cls, self.xref[attrs['id']]['elem'], mod)
    else :
      sc += '  ClassBuilder< %s >("%s", %s)' % (clf, cls, mod)
    if 'extra' in attrs :
      for pname, pval in attrs['extra'].items() :
        if pname not in ('name','pattern','n_name','file_name','file_pattern') :
          if pname == 'id' : pname = 'ClassID'
          sc += '\n  .addProperty("%s", "%s")' % (pname, pval)
    for b in bases :
      sc += '\n' + self.genBaseClassBuild( b )
    if not self.isUnnamedType(clf) :
      for m in members :
        funcname = 'gen'+self.xref[m]['elem']+'Build'
        if funcname in dir(self) :
          line = self.__class__.__dict__[funcname](self, self.xref[m]['attrs'], self.xref[m]['subelems'])
          if line : sc += '\n' + line 
    sc += ';\n}\n\n'
    ss = ''
    if not self.isUnnamedType(clf):
      ss = '//------Stub functions for class %s -------------------------------\n' % cl
      for m in members :
        funcname = 'gen'+self.xref[m]['elem']+'Def'
        if funcname in dir(self) :
          ss += self.__class__.__dict__[funcname](self, self.xref[m]['attrs'], self.xref[m]['subelems']) + '\n'
    return sd, sc, ss
#----------------------------------------------------------------------------------
  def genTypedefBuild(self, attrs, childs) :
    if self.no_membertypedefs : return ''
    s = ''
    s += '  .addTypedef(%s, "%s::%s")' % ( self.genTypeID(attrs['type']), self.genTypeName(attrs['context']), attrs['name']) 
    return s  
#----------------------------------------------------------------------------------
  def genEnumerationBuild(self, attrs, childs):
    s = ''
    name = self.genTypeName(attrs['id']) 
    values = ''
    for child in childs : values += child['name'] + '=' + child['init'] +';'
    values = values[:-1]
    if self.isUnnamedType(name) :
      s += '  .addEnum("%s", "%s", &typeid(UnnamedEnum))' % (name[name.rfind('::')+3:], values) 
    else :
      s += '  .addEnum< %s >("%s")' % (name, values)
    return s 
#----------------------------------------------------------------------------------
  def genScopeName(self, attrs, enum=False, const=False, colon=False) :
    s = ''
    if 'context' in attrs :
      ns = self.genTypeName(attrs['context'], enum, const, colon)
      if ns : s = ns + '::'
      elif colon  : s = '::'
    return s
#----------------------------------------------------------------------------------
  def genTypeName(self, id, enum=False, const=False, colon=False, alltempl=False) :
    if id[-1] in ['c','v'] :
      nid = id[:-1]
      cvdict = {'c':'const','v':'volatile'}
      prdict = {'PointerType':'*', 'ReferenceType':'&'}
      nidelem = self.xref[nid]['elem']
      if nidelem in ('PointerType','ReferenceType') :
        if const : return self.genTypeName(nid, enum, const, colon)
        else :     return self.genTypeName(nid, enum, const, colon) + ' ' + cvdict[id[-1]]
      else :
        if const : return self.genTypeName(nid, enum, const, colon)
        else     : return cvdict[id[-1]] + ' ' + self.genTypeName(nid, enum, const, colon)
    elem  = self.xref[id]['elem']
    attrs = self.xref[id]['attrs']
    s = self.genScopeName(attrs, enum, const, colon)
    if elem == 'Namespace' :
      if attrs['name'] != '::' : s += attrs['name']
    elif elem == 'PointerType' :
      t = self.genTypeName(attrs['type'],enum, const, colon)
      if   t[-1] == ')' : s += t.replace('::*)','::**)').replace('::)','::*)').replace('(*)', '(**)').replace('()','(*)')
      elif t[-1] == ']' : s += t[:t.find('[')] + '(*)' + t[t.find('['):]
      else              : s += t + '*'   
    elif elem == 'ReferenceType' :
      s += self.genTypeName(attrs['type'],enum, const, colon)+'&'
    elif elem in ('FunctionType','MethodType') :
      s = self.genTypeName(attrs['returns'], enum, const, colon)
      if elem == 'MethodType' : 
        s += '('+ self.genTypeName(attrs['basetype'], enum, const, colon) + '::)('
      else :
        s += '()('
      args = self.xref[id]['subelems']
      if args :
        for a in range(len(args)) :
          s += self.genTypeName(args[a]['type'])
          if a < len(args)-1 : s += ', '
        s += ')'
      else :
        s += 'void)'
    elif elem == 'ArrayType' :
      arr = '[%s]' % str(int(attrs['max'])+1)
      typ = self.genTypeName(attrs['type'], enum, const, colon)
      if typ[-1] == ']' :
        pos = typ.find('[')
        s += typ[:pos] + arr + typ[pos:]
      else:
        s += typ + arr
    elif elem == 'Unimplemented' :
      s += attrs['tree_code_name']
    elif elem == 'Enumeration' :
      if enum : s = 'int'           # Replace "enum type" by "int"
      else :    s += attrs['name']  # FIXME: Not always true  
    elif elem == 'Typedef' :
      s = self.genScopeName(attrs, enum, const, colon)
      s += attrs['name']
    elif elem in ('Function', 'OperatorFunction') :
      if 'name' in attrs : s += attrs['name']
      else : pass
    elif elem == 'OffsetType' :
      s += self.genTypeName(attrs['type'], enum, const, colon) + ' '
      s += self.genTypeName(attrs['basetype'], enum, const, colon) + '::'  
    else :
      if 'name' in attrs : s += attrs['name']
      s = normalizeClass(s,alltempl)                   # Normalize STL class names, primitives, etc.
    return s
#----------------------------------------------------------------------------------
  def genTypeID(self, id ) :
    if id[-1] in ('c','v') :
      self.genTypeID(id[:-1])
    else : 
      elem  = self.xref[id]['elem']
      attrs = self.xref[id]['attrs']
      if elem in ('PointerType', 'ReferenceType', 'ArrayType', 'Typedef') :
        self.genTypeID(attrs['type'])
      elif elem in ('FunctionType', 'MethodType') :
        if 'returns' in attrs : self.genTypeID(attrs['returns'])
        args = self.xref[id]['subelems']
        for a in args : self.genTypeID(a['type'])
      elif elem in ('OperatorMethod', 'Method', 'Constructor', 'Converter', 'Destructor', 
                    'Function', 'OperatorFunction' ) :
        if 'returns' in attrs : c = 'FunctionTypeBuilder(' + self.genTypeID(attrs['returns'])
        else                  : c = 'FunctionTypeBuilder(type_void'
        args = self.xref[id]['subelems']
        for a in args : c += ', '+ self.genTypeID(a['type'])
        c += ')'
        return c
      else :
        pass
    # Add this type in the list of types...
    if id not in self.typeids : self.typeids.append(id)
    return 'type'+id
#----------------------------------------------------------------------------------
  def genAllTypes(self) :
    c  = 'namespace { '
    c += '  Type type_void = TypeBuilder("void");\n'
    for id in self.typeids :      
      c += '  Type type%s = ' % id
      if id[-1] == 'c':
        c += 'ConstBuilder(type'+id[:-1]+');\n'
      elif id[-1] == 'v':
        c += 'VolatileBuilder(type'+id[:-1]+');\n'
      else : 
        elem  = self.xref[id]['elem']
        attrs = self.xref[id]['attrs']
        if elem == 'PointerType' :
          c += 'PointerBuilder(type'+attrs['type']+');\n'
        elif elem == 'ReferenceType' :
          c += 'ReferenceBuilder(type'+attrs['type']+');\n'
        elif elem == 'ArrayType' :
          mx = attrs['max']
          # check if array is bound (max='fff...' for unbound arrays)
          if mx.isdigit() : len = str(int(mx)+1)
          else            : len = '0' 
          c += 'ArrayBuilder(type'+attrs['type']+', '+ len +');\n'
        elif elem == 'Typedef' :
          sc = self.genTypeName(attrs['context'])
          if sc : sc += '::'
          c += 'TypedefTypeBuilder("'+sc+attrs['name']+'", type'+ attrs['type']+');\n'
        elif elem == 'OffsetType' :
          c += 'TypeBuilder("%s");\n' % self.genTypeName(attrs['id'])
        elif elem == 'FunctionType' :
          if 'returns' in attrs : c += 'FunctionTypeBuilder(type'+attrs['returns']
          else                  : c += 'FunctionTypeBuilder(type_void'
          args = self.xref[id]['subelems']
          for a in args : c += ', type'+ a['type']
          c += ');\n'
        elif elem == 'MethodType' :
          c += 'TypeBuilder("%s");\n' % self.genTypeName(attrs['id'])
        elif elem in ('OperatorMethod', 'Method', 'Constructor', 'Converter', 'Destructor',
                      'Function', 'OperatorFunction') :
          pass
        elif elem == 'Enumeration' :
          sc = self.genTypeName(attrs['context'])
          if sc : sc += '::'
          items = self.xref[id]['subelems']
          values = string.join([ item['name'] + '=' + item['init'] for item in items],';')          
          c += 'EnumTypeBuilder("' + sc + attrs['name'] + '", "' + values + '");\n'
        else :
         name = ''
         if 'context' in attrs :
           ns = self.genTypeName(attrs['context'])
           if ns : name += ns + '::'
         if 'name' in attrs :
           name += attrs['name']
         name = normalizeClass(name,False)
         c += 'TypeBuilder("'+name+'");\n'
    c += '}\n'
    return c 
#----------------------------------------------------------------------------------
  def genNamespaces(self, selclasses ) :
    used_context = []
    s = ''
    for c in selclasses :
      if 'incomplete' not in c : used_context.append(c['context'])          
    for ns in self.namespaces :
      if ns['id'] in used_context and ns['name'] != '::' :
        s += '    NamespaceBuilder( "%s" );\n' % self.genTypeName(ns['id'])
    return s
#----------------------------------------------------------------------------------
  def genFunctionsStubs(self, selfunctions) :
    s = ''
    for f in selfunctions :
      id   = f['id']
      name = self.genTypeName(id)
      if 'operator' in f : name = 'operator '+name
      self.genTypeID(id)
      args = self.xref[id]['subelems']
      returns  = self.genTypeName(f['returns'], enum=True, const=True)
      print  'function '+ name
      s += 'static void* '
      if len(args) :
        s +=  'function%s( void*, const std::vector<void*>& arg, void*)\n{\n' % id 
      else :
        s +=  'function%s( void*, const std::vector<void*>&, void*)\n{\n' % id
      ndarg = self.getDefaultArgs(args)
      narg  = len(args)
      if ndarg : iden = '  '
      else     : iden = ''
      if returns != 'void' and returns in self.basictypes :
        s += '  static %s ret;\n' % returns
      for n in range(narg-ndarg, narg+1) :
        if ndarg :
          if n == narg-ndarg :  s += '  if ( arg.size() == %d ) {\n' % n
          else               :  s += '  else if ( arg.size() == %d ) { \n' % n
        if returns == 'void' :
          first = iden + '  %s(' % ( name, )
          s += first + self.genMCOArgs(args, n, len(first)) + ');\n'
          s += iden + '  return 0;\n'
        else :
          if returns[-1] in ('*',')' ):
            first = iden + '  return (void*)%s(' % ( name, )
            s += first + self.genMCOArgs(args, n, len(first)) + ');\n'
          elif returns[-1] == '&' :
            first = iden + '  return (void*)&%s(' % ( name, )
            s += first + self.genMCOArgs(args, n, len(first)) + ');\n'
          elif returns in self.basictypes :
            first = iden + '  ret = %s(' % ( name, )
            s += first + self.genMCOArgs(args, n, len(first)) + ');\n'
            s += iden + '  return &ret;\n'        
          else :
            first = iden + '  return new %s(%s(' % ( returns, name )
            s += first + self.genMCOArgs(args, n, len(first)) + '));\n'
        if ndarg : 
          if n != narg : s += '  }\n'
          else :
            if returns == 'void' : s += '  }\n  return 0;\n'
            else :                 s += '  }\n  return 0;\n'
      s += '}\n'
    return s  
#----------------------------------------------------------------------------------
  def genFunctions(self, selfunctions) :
    s = ''
    i = 0;
    for f in selfunctions :
      id   = f['id']
      name = self.genTypeName(id)
      if ( self.xref[id]['attrs'].has_key('mangled') ):
        mm = self.xref[id]['attrs']['mangled'][2:]
        dname = gccdemangler.demangle_name(mm)
      else :
        dname = name
      name += getTemplateArgString(dname[1])
      args = self.xref[id]['subelems']      
      if args : params  = '"'+ string.join( map(self.genParameter, args),';')+'"'
      else    : params  = '0'
      mod = self.genModifier(f, None)
      s += '      Type t%s = %s;' % (i, self.genTypeID(id))
      s += '      FunctionBuilder(t%s, "%s", function%s, 0, %s, %s);\n' % (i, name, id, params, mod)
      i += 1;
    return s
#----------------------------------------------------------------------------------
  def countColonsForOffset(self, name) :
    prn = 0
    cnt = 0
    for c in name :
      if c == ',' and not prn : cnt += 1
      elif c == '('           : prn += 1
      elif c == ')'           : prn -= 1
      else                    : pass
    return cnt
#----------------------------------------------------------------------------------
  def genFieldBuild(self, attrs, childs):
    type   = self.genTypeName(attrs['type'], enum=False, const=False)
    cl     = self.genTypeName(attrs['context'],colon=True)
    cls    = self.genTypeName(attrs['context'])
    name = attrs['name']
    if not name :
      ftype = self.xref[attrs['type']]
      # if the member type is an unnamed union we try to take the first member of the union as name
      if ftype['elem'] == 'Union':
        firstMember = ftype['attrs']['members'].split()[0]
        if firstMember : name = self.xref[firstMember]['attrs']['name']
        else           : return ''       # then this must be an unnamed union without members
    if type[-1] == '&' :
      print '--->>lcgdict WARNING: References are not supported as data members (%s %s::%s)' % ( type, cls, name )
      return ''
    if 'bits' in attrs:
      print '--->>lcgdict WARNING: Bit-fields are not supported as data members (%s %s::%s:%s)' % ( type, cls, name, attrs['bits'] )
      return ''
    if self.selector : xattrs = self.selector.selfield( cls,name)
    else             : xattrs = None
    mod = self.genModifier(attrs,xattrs)
    i = self.countColonsForOffset(cl)
    templArgs = ''
    if ( i ) : templArgs = str(i+1)
    c = '  .addDataMember(%s, "%s", OffsetOf%s(%s, %s), %s)' % (self.genTypeID(attrs['type']), name, templArgs, cl, name, mod)
    c += self.genCommentProperty(attrs)
    # Other properties
    if xattrs : 
      for pname, pval in xattrs.items() : 
        if pname not in ('name', 'transient', 'pattern') :
          c += '\n  .addProperty("%s","%s")' % (pname, pval)     
    return c
#----------------------------------------------------------------------------------    
  def genCommentProperty(self, attrs):
    if not self.comments or 'file' not in attrs or ('artificial' in attrs and attrs['artificial'] == '1') : return '' 
    fd = self.files[attrs['file']]
    # open and read the header file if not yet done
    if 'filelines' not in fd :
      try :
        f = file(fd['name'])
        fd['filelines'] = f.readlines()
        f.close()
      except :
        return ''
    line = fd['filelines'][int(attrs['line'])-1]
    if line.find('//') == -1 : return ''
    return '\n  .addProperty("comment","%s")' %  (line[line.index('//')+2:-1]).replace('"','\\"')
#----------------------------------------------------------------------------------
  def genArgument(self, attrs):
    c = self.genTypeName(attrs['type'], enum=True, const=False)
    return c
#----------------------------------------------------------------------------------
  def genParameter(self, attrs):
    c = ''
    if 'name' in attrs :
      c += attrs['name']
      if 'default' in attrs :
        c += '='+ attrs['default'].replace('"','\\"')
    return c
#----------------------------------------------------------------------------------
  def genModifier(self, attrs, xattrs ):
    if 'access' not in attrs            : mod = 'PUBLIC'
    elif attrs['access'] == 'private'   : mod = 'PRIVATE'
    elif attrs['access'] == 'protected' : mod = 'PROTECTED'
    else                                : mod = 'NONE'
    if 'virtual' in attrs : mod += ' | VIRTUAL'
    if 'static'  in attrs : mod += ' | STATIC'
    # Extra modifiers
    if ((xattrs and xattrs.get('transient') == 'true') or
	('extra' in attrs and attrs['extra'].get('transient') == 'true')) : mod += ' | TRANSIENT'
    return mod
#----------------------------------------------------------------------------------
  def genMCODecl( self, type, name, attrs, args ) :
    return 'static void * %s%s(void*, const std::vector<void*>&, void*);' % (type, attrs['id'])
#----------------------------------------------------------------------------------
  def genMCOBuild(self, type, name, attrs, args):
    id       = attrs['id']
    if type == 'constructor' : returns  = 'void'
    else                     : returns  = self.genTypeName(attrs['returns'])
    mod = self.genModifier(attrs, None)
    if   type == 'constructor' : mod += ' | CONSTRUCTOR'
    elif type == 'operator' :    mod += ' | OPERATOR'
    elif type == 'converter' :   mod += ' | CONVERTER'
    if args : params  = '"'+ string.join( map(self.genParameter, args),';')+'"'
    else    : params  = '0'
    s = '  .addFunctionMember(%s, "%s", %s%s, 0, %s, %s)' % (self.genTypeID(id), name, type, id, params, mod)
    s += self.genCommentProperty(attrs)
    return s
#----------------------------------------------------------------------------------
  def genMCODef(self, type, name, attrs, args):
    id       = attrs['id']
    cl       = self.genTypeName(attrs['context'],colon=True)
    clt      = string.translate(str(cl), self.transtable)
    returns  = self.genTypeName(attrs['returns'],enum=True, const=True)
    s = 'void* '
    if len(args) :
      s +=  '%s_dict::%s%s( void* o, const std::vector<void*>& arg, void*)\n{\n' %( clt, type, id )
    else :
      s +=  '%s_dict::%s%s( void* o, const std::vector<void*>&, void*)\n{\n' %( clt, type, id )
    ndarg = self.getDefaultArgs(args)
    narg  = len(args)
    if ndarg : iden = '  '
    else     : iden = ''
    if returns != 'void' :
      if returns in self.basictypes :
        s += '  static %s ret;\n' % returns
      elif returns.find('::*)') != -1 :
        s += '  static %s;\n' % returns.replace('::*','::* ret')
      elif returns.find('::*') != -1 :
        s += '  static %s ret;\n' % returns  
    if 'const' in attrs : cl = 'const '+ cl
    for n in range(narg-ndarg, narg+1) :
      if ndarg :
        if n == narg-ndarg :  s += '  if ( arg.size() == %d ) {\n' % n
        else               :  s += '  else if ( arg.size() == %d ) { \n' % n
      if returns == 'void' :
        first = iden + '  ((%s*)o)->%s(' % ( cl, name )
        s += first + self.genMCOArgs(args, n, len(first)) + ');\n'
        s += iden + '  return 0;\n'
      else :
        if returns[-1] in ('*',')') and returns.find('::*') == -1 :
          first = iden + '  return (void*)((%s*)o)->%s(' % ( cl, name )
          s += first + self.genMCOArgs(args, n, len(first)) + ');\n'
        elif returns[-1] == '&' :
          first = iden + '  return (void*)&((%s*)o)->%s(' % ( cl, name )
          s += first + self.genMCOArgs(args, n, len(first)) + ');\n'
        elif returns in self.basictypes or returns.find('::*') != -1 :
          first = iden + '  ret = ((%s*)o)->%s(' % ( cl, name )
          s += first + self.genMCOArgs(args, n, len(first)) + ');\n'
          s += iden + '  return &ret;\n'        
        else :
          first = iden + '  return new %s(((%s*)o)->%s(' % ( returns, cl, name )
          s += first + self.genMCOArgs(args, n, len(first)) + '));\n'
      if ndarg : 
        if n != narg : s += '  }\n'
        else :
          if returns == 'void' : s += '  }\n  return 0;\n'
          else :                 s += '  }\n  return 0;\n'
    s += '}\n'
    return s
#----------------------------------------------------------------------------------
  def getDefaultArgs(self, args):
    n = 0
    for a in args :
      if 'default' in a : n += 1
    return n
#----------------------------------------------------------------------------------
  def genMCOArgs(self, args, narg, pad):
    s = ''
    for i in range(narg) :
      a = args[i]
      #arg = self.genArgument(a, 0);
      arg = self.genTypeName(a['type'],colon=True)
      if arg[-1] == '*' :
         if arg[-2] == ':' :   # Pointer to data member
           s += '*(%s*)arg[%d]' % (arg, i )
         else :
           s += '(%s)arg[%d]' % (arg, i )
      elif arg[-1] == ']' :
        s += '(%s)arg[%d]' % (arg, i)
      elif arg[-1] == ')' or (len(arg) > 7 and arg[-7:] == ') const'): # FIXME, the second check is a hack
        if arg.find('::*') != -1 :  # Pointer to function member
          s += '*(%s)arg[%d]' %(arg.replace('::*','::**'), i)
        elif (len(arg) > 7  and arg[-7:] == ') const') :
          s += '(%s)arg[%d]' % (arg[:-6].replace('(*)','(* const)'), i) # 2nd part of the hack
        else :
          s += '(%s)arg[%d]' % (arg, i )
      elif arg[-1] == '&' :
        s += '*(%s*)arg[%d]' % (arg[:-1], i )
      else :
        s += '*(%s*)arg[%d]' % (arg, i )
      if i != narg - 1 : s += ',\n' + pad*' '
    return s
#----------------------------------------------------------------------------------
  def genMethodDecl(self, attrs, args):
    return self.genMCODecl( 'method', '', attrs, args )
#----------------------------------------------------------------------------------
  def genMethodBuild(self, attrs, args):
    return self.genMCOBuild( 'method', attrs['name'], attrs, args )
#----------------------------------------------------------------------------------
  def genMethodDef(self, attrs, args):
    return self.genMCODef( 'method', attrs['name'], attrs, args )
#----------------------------------------------------------------------------------
  def genConstructorDecl(self, attrs, args):
    return self.genMCODecl( 'constructor', '', attrs, args )
#----------------------------------------------------------------------------------
  def genConstructorBuild(self, attrs, args):
    return self.genMCOBuild( 'constructor', attrs['name'], attrs, args )
#----------------------------------------------------------------------------------
  def genConstructorDef(self, attrs, args):
    cl  = self.genTypeName(attrs['context'], colon=True)
    clt = string.translate(str(cl), self.transtable)
    id  = attrs['id']
    if len(args) :
      s = 'void* %s_dict::constructor%s( void* mem, const std::vector<void*>& arg, void*) {\n' %( clt, id )
    else :
      s = 'void* %s_dict::constructor%s( void* mem, const std::vector<void*>&, void*) {\n' %( clt, id )
    if 'pseudo' in attrs :
      s += '  return ::new(mem) %s( *(__void__*)0 );\n' % ( cl )
    else :
      ndarg = self.getDefaultArgs(args)
      narg  = len(args)
      for n in range(narg-ndarg, narg+1) :
        if ndarg :
          if n == narg-ndarg :  s += '  if ( arg.size() == %d ) {\n  ' % n
          else               :  s += '  else if ( arg.size() == %d ) { \n  ' % n
        first = '  return ::new(mem) %s(' % ( cl )
        s += first + self.genMCOArgs(args, n, len(first)) + ');\n'
        if ndarg : 
          if n != narg : s += '  }\n'
          else :         s += '  }\n  return 0;\n'
    s += '}\n'
    return s
#----------------------------------------------------------------------------------
  def genDestructorDecl(self, attrs, childs):
    cl = self.genTypeName(attrs['context'])
    return 'static void * destructor(void * o, const std::vector<void*>&, void *) { ((::%s*)o)->~%s(); return 0;}' % ( cl, attrs['name'] )
#----------------------------------------------------------------------------------
  def genDestructorBuild(self, attrs, childs):
    mod = self.genModifier(attrs,None)
    id       = attrs['id']
    s = '  .addFunctionMember(%s, "~%s", destructor, 0, 0, %s | DESTRUCTOR )' % (self.genTypeID(id), attrs['name'], mod)
    s += self.genCommentProperty(attrs)
    return s
#----------------------------------------------------------------------------------
  def genOperatorMethodDecl( self, attrs, args ) :
    if attrs['name'][0].isalpha() : name = 'operator '+ attrs['name']
    else                          : name = 'operator' + attrs['name'] 
    return self.genMCODecl( 'operator', name, attrs, args )    
#----------------------------------------------------------------------------------
  def genOperatorMethodBuild( self, attrs, args ) :
    if attrs['name'][0].isalpha() : name = 'operator '+ attrs['name']
    else                          : name = 'operator' + attrs['name'] 
    return self.genMCOBuild( 'operator', name, attrs, args )    
#----------------------------------------------------------------------------------
  def genOperatorMethodDef( self, attrs, args ) :
    if attrs['name'][0].isalpha() : name = 'operator '+ attrs['name']
    else                          : name = 'operator' + attrs['name'] 
    if name[-1] == '>' and name.find('<') != -1 : name = name[:name.find('<')]
    return self.genMCODef( 'operator', name, attrs, args )    
#----------------------------------------------------------------------------------
  def genConverterDecl( self, attrs, args ) :
    return self.genMCODecl( 'converter', 'operator '+attrs['name'], attrs, args )    
#----------------------------------------------------------------------------------
  def genConverterBuild( self, attrs, args ) :
    return self.genMCOBuild( 'converter', 'operator '+self.genTypeName(attrs['returns'],enum=True,const=True), attrs, args )    
#----------------------------------------------------------------------------------
  def genConverterDef( self, attrs, args ) :
    return self.genMCODef( 'converter', 'operator '+self.genTypeName(attrs['returns']), attrs, args )    
#----------------------------------------------------------------------------------
  def genEnumValue(self, attrs):
    return '%s = %s' % (attrs['name'], attrs['init'])
#----------------------------------------------------------------------------------
#  def genBaseClassDecl(self, b ):
#    if b[:10] == 'protected:' : b = b[10:]
#    if b[:8]  == 'private:'   : b = b[8:]
#    return 'static size_t tobase%s(void*);' % b
#----------------------------------------------------------------------------------
  def genBaseClassBuild(self, attrs ):
    mod = attrs['access'].upper()
    if 'virtual' in attrs and attrs['virtual'] == '1' : mod = 'VIRTUAL | ' + mod
    return  '  .addBase< %s >(%s)' % (self.genTypeName(attrs['type'],colon=True), mod)
#----------------------------------------------------------------------------------
#  def genBaseClassDef(self, b, clf ):
#    if b[:10] == 'protected:' : b = b[10:]
#    if b[:8]  == 'private:'   : b = b[8:]
#    clt = string.translate(str(clf), self.transtable)
#    s  = 'size_t %s_dict::tobase%s( void* o ) {\n' %( clt, b )
#    s += '  return (size_t)(%s*)(%s*)o - (int)(%s*)o;\n' % (self.genTypeName(b),clf, clf)
#    s += '}\n'
#    return s
#----CollectionProxy stuff--------------------------------------------------------
  def genCreateCollFuncTableDecl( self, attrs, args ) :
    return 'static void* method%s( void*, const std::vector<void*>&, void* ); ' % (attrs['id'])
  def genCreateCollFuncTableBuild( self, attrs, args ) :
    return '  .addFunctionMember<void*(void)>("createCollFuncTable", method%s)' % (attrs['id'])
  def genCreateCollFuncTableDef( self, attrs, args ) :
    cl       = self.genTypeName(attrs['context'], colon=True)
    clt      = string.translate(str(cl), self.transtable)
    t        = getTemplateArgs(cl)[0]
    s  = 'void* %s_dict::method%s( void*, const std::vector<void*>&, void*)\n{\n' %( clt, attrs['id'] )
    s += '  return seal::reflex::Proxy< %s >::generate();\n' % (cl,)
    s += '}\n'
    return s
#----BasesMap stuff--------------------------------------------------------
  def genGetBasesTableDecl( self, attrs, args ) :
    return 'static void* method%s( void*, const std::vector<void*>&, void* ); ' % (attrs['id'])
  def genGetBasesTableBuild( self, attrs, args ) :
    return '  .addFunctionMember<void*(void)>("getBasesTable", method%s)' % (attrs['id'])
  def genGetBasesTableDef( self, attrs, args ) :
    cid      = attrs['context']
    cl       = self.genTypeName(cid, colon=True)
    clt      = string.translate(str(cl), self.transtable)
    s  = 'void* %s_dict::method%s( void*, const std::vector<void*>&, void*)\n{\n' %( clt, attrs['id'] )
    s += '  static std::vector<std::pair<seal::reflex::Base, int> > s_bases;\n'
    bases = []
    self.getAllBases( cid, bases ) 
    for b in bases :
      bname = self.genTypeName(b[0],colon=True)
      s += '  s_bases.push_back(std::make_pair(seal::reflex::Base( seal::reflex::getType< %s >(), seal::reflex::baseOffset< %s,%s >::get(),%s), %d));\n' % (bname, cl, bname, b[1], b[2])
    s += '  return &s_bases;\n' 
    s += '}\n'
    return s
#----------------------------------------------------------------------------------
  def getBases( self, cid ) :
    if 'bases' in self.xref[cid] :
      return self.xref[cid]['bases']
    elif 'bases' in self.xref[cid]['attrs'] :
      bases = []
      for b in self.xref[cid]['attrs']['bases'].split() :
        access = 'public'
        if b[:10] == 'protected:' : b = b[10:]; access = 'protected'
        if b[:8]  == 'private:'   : b = b[8:]; access = 'private'
        bases.append( {'type': b, 'access': access, 'virtual': '-1' } )
      return bases
    else :
      return []
#----------------------------------------------------------------------------------
  def getAllBases( self, cid, bases, level = 0, access = 'public', virtual = False ) :
    for b in self.getBases( cid ) :
      id = b['type']
      if id not in [ bid[0] for bid in bases] :
        if access == 'public' : access = b['access']
        if not virtual : virtual = ( b['virtual'] == '1' )
        mod = access.upper()
        if virtual : mod = 'VIRTUAL |' + mod
        bases.append( [id,  mod, level] )
        self.getAllBases( id, bases, level+1, access, virtual )
#----------------------------------------------------------------------------------
  def completeClass(self, attrs):
    # Complete class with "instantiated" templated methods or constructors
    if 'members' in attrs : members = attrs['members'].split()
    else                  : members = []
    cid = attrs['id']
    for m in self.methods :
      if m['context'] == cid and m['id'] not in members :
        # replace the mame by the complete templated name. Use the demangle module for that
        if 'mangled' in m :
          mm = m['mangled'][2:]
          dname = gccdemangler.demangle_name(mm)
          m['name'] += getTemplateArgString(dname[1])
        attrs['members'] += u' ' + m['id']

#----Vector like containers---------------------------------------------------------
  def genvectorWriteDataDecl( self, attrs, args ) :
    return 'static void* method%s( void*, const std::vector<void*>&, void* ); ' % (attrs['id'])
  def genvectorWriteDataBuild( self, attrs, args ) :
    return '  .addFunctionMember<void*(std::vector<void*>)>("writeData", method%s)' % (attrs['id'])
  def genvectorWriteDataDef( self, attrs, args ) :
    return self.genWriteDataDef( attrs, args )
  def genvectorReadDataDecl( self, attrs, args ) :
    return 'static void* method%s( void*, const std::vector<void*>&, void* ); ' % (attrs['id'])
  def genvectorReadDataBuild( self, attrs, args ) :
    return '  .addFunctionMember<void*(std::vector<void*>)>("readData", method%s)' % (attrs['id'])
  def genvectorReadDataDef( self, attrs, args ) :
    #return self.genReadDataDef( attrs, args, 'push_back' )
    cl       = self.genTypeName(attrs['context'])
    clt      = string.translate(str(cl), self.transtable)
    t        = getTemplateArgs(cl)[0]
    s  = 'void* __%s_dict::method%s( void* o, const std::vector<void*>& args, void*)\n{\n' %( clt, attrs['id'] )
    s += '  %s* v = (%s*)o;\n' % (cl, cl)
    s += '  size_t n = *(size_t*)args[1];\n'
    s += '  if (n > 0) {\n'
    s += '    v->resize(n);\n'
    if t == 'bool' :   # --- Template specialization
      s += '    for(size_t i = 0; i < n; i++) {\n'
      s += '      bool b;\n'
      s += '      (*(void (**)(const void*, const std::vector<void*>&))&args[2])(&b, args);\n'
      s += '      (*v)[i] = b;\n'
    else :
      s += '    for(size_t i = 0; i < n; i++) {\n'
      s += '      (*(void (**)(const void*, const std::vector<void*>&))&args[2])(&(*v)[i], args);\n'
    s += '    }\n  }\n  return 0;\n}\n'
    return s
#----List like containers---------------------------------------------------------
  def genlistWriteDataDecl( self, attrs, args ) :
    return 'static void* method%s( void*, const std::vector<void*>&, void* ); ' % (attrs['id'])
  def genlistWriteDataBuild( self, attrs, args ) :
    return '  .addFunctionMember<void*(std::vector<void*>)>("writeData", method%s)' % (attrs['id'])
  def genlistWriteDataDef( self, attrs, args ) :
    return self.genWriteDataDef( attrs, args )
  def genlistReadDataDecl( self, attrs, args ) :
    return 'static void* method%s( void*, const std::vector<void*>&, void* ); ' % (attrs['id'])
  def genlistReadDataBuild( self, attrs, args ) :
    return '  .addFunctionMember<void*(std::vector<void*>)>("readData", method%s)' % (attrs['id'])
  def genlistReadDataDef( self, attrs, args ) :
    return self.genReadDataDef( attrs, args, 'push_back' )
#----Set like containers---------------------------------------------------------
  def gensetWriteDataDecl( self, attrs, args ) :
    return 'static void* method%s( void*, const std::vector<void*>&, void* ); ' % (attrs['id'])
  def gensetWriteDataBuild( self, attrs, args ) :
    return '  .addFunctionMember<void*(std::vector<void*>)>("writeData", method%s)' % (attrs['id'])
  def gensetWriteDataDef( self, attrs, args ) :
    return self.genWriteDataDef( attrs, args )
  def gensetReadDataDecl( self, attrs, args ) :
    return 'static void* method%s( void*, const std::vector<void*>&, void* ); ' % (attrs['id'])
  def gensetReadDataBuild( self, attrs, args ) :
    return '  .addFunctionMember<void*(std::vector<void*>)>("readData", method%s)' % (attrs['id'])
  def gensetReadDataDef( self, attrs, args ) :
    return self.genReadDataDef( attrs, args, 'insert' )
#----------------------------------------------------------------------------------
  def genWriteDataDef( self, attrs, args ) :
    cl       = self.genTypeName(attrs['context'])
    clt      = string.translate(str(cl), self.transtable)
    t        = getTemplateArgs(cl)[0]
    s  = 'void* __%s_dict::method%s( void* o, const std::vector<void*>& args, void*)\n{\n' %( clt, attrs['id'] )
    s += '  %s* v = (%s*)o;\n' % (cl, cl)
    s += '  size_t n = v->size();\n'
    s += '  (*(void (**)(size_t&, const std::vector<void*>&))&args[1])(n, args);\n'
    s += '  for(%s::iterator i = v->begin(); i != v->end(); i++) {\n' % (cl)
    if t == 'bool' :
      s += '    bool b = *i;\n'
      s += '    (*(void (**)(const void*, const std::vector<void*>&))&args[2])(&b, args);\n'
    else :
      s += '    (*(void (**)(const void*, const std::vector<void*>&))&args[2])(&(*i), args);\n'
    s += '  }\n  return 0;\n}\n'
    return s
#----------------------------------------------------------------------------------
  def genReadDataDef( self, attrs, args, func ) :
    cl       = self.genTypeName(attrs['context'])
    clt      = string.translate(str(cl), self.transtable)
    t        = getTemplateArgs(cl)[0]
    s  = 'void* __%s_dict::method%s( void* o, const std::vector<void*>& args, void*)\n{\n' %( clt, attrs['id'] )
    s += '  %s* v = (%s*)o;\n' % (cl, cl)
    s += '  size_t n = *(size_t*)args[1];\n'
    s += '  if (n > 0) {\n'
    s += '    for(size_t i = 0; i < n; i++) {\n'
    s += '      %s::value_type o;\n' % cl
    s += '      (*(void (**)(const void*, const std::vector<void*>&))&args[2])(&o, args);\n'
    s += '      v->%s(o);\n' % func
    s += '    }\n  ;\n}  return 0;\n}\n'
    return s
#-----map like containers---------------------------------------------------------------
  def genmapWriteDataDecl( self, attrs, args ) :
    return 'static void* method%s( void*, const std::vector<void*>&, void* ); ' % (attrs['id'])
  def genmapWriteDataBuild( self, attrs, args ) :
    return '  .addFunctionMember<void*(std::vector<void*>)>("writeData", method%s)' % (attrs['id'])
  def genmapReadDataDecl( self, attrs, args ) :
    return 'static void* method%s( void*, const std::vector<void*>&, void* ); ' % (attrs['id'])
  def genmapReadDataBuild( self, attrs, args ) :
    return '  .addFunctionMember<void*(std::vector<void*>)>("readData", method%s)' % (attrs['id'])
  def genmapWriteDataDef( self, attrs, args ) :
    cl       = self.genTypeName(attrs['context'])
    clt      = string.translate(str(cl), self.transtable)
    t        = getTemplateArgs(cl)[1]
    s  = 'void* __%s_dict::method%s( void* o, const std::vector<void*>& args, void*)\n{\n' %( clt, attrs['id'] )
    s += '  %s* v = (%s*)o;\n' % (cl, cl)
    s += '  size_t n = v->size();\n'
    s += '  (*(void (**)(size_t&, const std::vector<void*>&))&args[1])(n, args);\n'
    s += '  if ( n > 0 ) {\n'
    s += '    for(%s::iterator i = v->begin(); i != v->end(); i++) {\n' % (cl)
    s += '      std::pair<void*,void*> pt((void*)&(*i).first, (void*)&(*i).second);\n'
    s += '      (*(void (**)(const void*, const std::vector<void*>&))&args[2])(&pt, args);\n'
    s += '    }\n  }\n  return 0; \n}\n'
    return s
  def genmapReadDataDef( self, attrs, args ) :
    cl       = self.genTypeName(attrs['context'])
    clt      = string.translate(str(cl), self.transtable)
    k,t        = getTemplateArgs(cl)[0:2]
    pointer  = t[-1] == '*'
    kpointer = k[-1] == '*'
    s  = 'void* __%s_dict::method%s( void* o, const std::vector<void*>& args, void*)\n{\n' %( clt, attrs['id'] )
    s += '  %s* v = (%s*)o;\n' % (cl, cl)
    s += '  size_t n = *(size_t*)args[1];\n'
    s += '  if (n > 0) {\n'
    s += '    for(size_t i = 0; i < n; i++) {\n'
    s += '      std::pair<%s::key_type, %s::value_type::second_type> t;\n' %(cl,cl)
    if kpointer :
      s += '      t.first = 0; // fill with new %s;\n' % k[:-1]
      first = '&t.first'
    else :
      first = '&t.first'
    if pointer : 
      s += '      t.second = 0; // fill with new %s;\n' % t[:-1]
      second = '&t.second'
    else :
      second = '&t.second'
    s += '      std::pair<const void*, const void*> pt(%s, %s);\n'%(first,second)
    s += '      (*(void (**)(const void*, const std::vector<void*>&))&args[2])(&pt, args);\n'
    s += '      v->insert(t);\n'
    s += '    }\n  }\n  return 0;\n}\n'
    return s
#----Queue like containers---------------------------------------------------------
  def genqueueWriteDataDecl( self, attrs, args ) :
    return 'static void* method%s( void*, const std::vector<void*>&, void* ); ' % (attrs['id'])
  def genqueueWriteDataBuild( self, attrs, args ) :
    return '  .addFunctionMember<void*(std::vector<void*>)>("writeData", method%s)' % (attrs['id'])
  def genqueueReadDataDecl( self, attrs, args ) :
    return 'static void* method%s( void*, const std::vector<void*>&, void* ); ' % (attrs['id'])
  def genqueueReadDataBuild( self, attrs, args ) :
    return '  .addFunctionMember<void*(std::vector<void*>)>("readData", method%s)' % (attrs['id'])
  def genqueueWriteDataDef( self, attrs, args ) :
    cl       = self.genTypeName(attrs['context'])
    clt      = string.translate(str(cl), self.transtable)
    t        = getTemplateArgs(cl)[0]
    s  = 'void* __%s_dict::method%s( void* o, const std::vector<void*>& args, void*)\n{\n' %( clt, attrs['id'] )
    s += '  %s* v = (%s*)o;\n' % (cl, cl)
    s += '  size_t n = v->size();\n'
    s += '  (*(void (**)(size_t&, const std::vector<void*>&))&args[1])(n, args);\n'
    s += '  if ( n > 0 ) {\n'
    s += '    for(size_t i = 0; i < n; i++) {\n'
    s += '      (*(void (**)(const void*, const std::vector<void*>&))&args[2])(&(v->front()), args);\n'
    s += '      v->push(v->front());\n'
    s += '      v->pop();\n'
    s += '    }\n  }\n  return 0; \n}\n'
    return s
  def genqueueReadDataDef( self, attrs, args ) :
    return self.genReadDataDef( attrs, args, 'push' )
#----Stack like containers---------------------------------------------------------
  def genstackWriteDataDecl( self, attrs, args ) :
    return 'static void* method%s( void*, const std::vector<void*>&, void* ); ' % (attrs['id'])
  def genstackWriteDataBuild( self, attrs, args ) :
    return '  .addFunctionMember<void*(std::vector<void*>)>("writeData", method%s)' % (attrs['id'])
  def genstackReadDataDecl( self, attrs, args ) :
    return 'static void* method%s( void*, const std::vector<void*>&, void* ); ' % (attrs['id'])
  def genstackReadDataBuild( self, attrs, args ) :
    return '  .addFunctionMember<void*(std::vector<void*>)>("readData", method%s)' % (attrs['id'])
  def genstackWriteDataDef( self, attrs, args ) :
    cl       = self.genTypeName(attrs['context'])
    clt      = string.translate(str(cl), self.transtable)
    t        = getTemplateArgs(cl)[0]
    s  = 'void* __%s_dict::method%s( void* o, const std::vector<void*>& args, void*)\n{\n' %( clt, attrs['id'] )
    s += '  %s tmp;\n' % cl
    s += '  %s* v = (%s*)o;\n' % (cl, cl)
    s += '  size_t i, n = v->size();\n'
    s += '  (*(void (**)(size_t&, const std::vector<void*>&))&args[1])(n, args);\n'
    s += '  for(i = 0; i < n; i++) {\n'
    s += '    tmp.push(v->top());\n'
    s += '    v->pop();\n  }\n'
    s += '  for(i = 0; i < n; i++) {\n'
    s += '    (*(void (**)(const void*, const std::vector<void*>&))&args[2])(&(tmp.top()), args);\n'
    s += '    v->push(tmp.top());\n'
    s += '    tmp.pop();\n  }\n'
    s += '  return 0; \n}\n'
    return s
  def genstackReadDataDef( self, attrs, args ) :
    return self.genReadDataDef( attrs, args, 'push' )
#---------------------------------------------------------------------------------------
def getContainerId(c):
  if   c[-8:] == 'iterator' : return ('NOCONTAINER','')
  if   c[:10] == 'std::deque'   :            return ('DEQUE','list')
  elif c[:9]  == 'std::list'    :            return ('LIST','list')
  elif c[:8]  == 'std::map'     :            return ('MAP','map')
  elif c[:13] == 'std::multimap':            return ('MULTIMAP','map')
  elif c[:19] == '__gnu_cxx::hash_map':      return ('HASHMAP','map')
  elif c[:24] == '__gnu_cxx::hash_multimap': return ('HASHMULTIMAP','map')
  elif c[:16] == 'stdext::hash_map':         return ('HASHMAP','map')
  elif c[:21] == 'stdext::hash_multimap':    return ('HASHMULTIMAP','map')    
  elif c[:10] == 'std::queue'   :            return ('QUEUE','queue')
  elif c[:8]  == 'std::set'     :            return ('SET','set')
  elif c[:13] == 'std::multiset':            return ('MULTISET','set')
  elif c[:19] == '__gnu_cxx::hash_set':      return ('HASHSET','set')
  elif c[:24] == '__gnu_cxx::hash_multiset': return ('HASHMULTISET','set')
  elif c[:16] == 'stdext::hash_set':         return ('HASHSET','set')
  elif c[:21] == 'stdext::hash_multiset':    return ('HASHMULTISET','set')
  elif c[:10] == 'std::stack'   :            return ('STACK','stack')
  elif c[:11] == 'std::vector'  :            return ('VECTOR','vector')
  else : return ('NOCONTAINER','')
#---------------------------------------------------------------------------------------
stldeftab = {}
stldeftab['deque']        = '=','std::allocator'
stldeftab['list']         = '=','std::allocator'
stldeftab['map']          = '=','=','std::less','std::allocator'
stldeftab['multimap']     = '=','=','std::less','std::allocator'
stldeftab['queue']        = '=','std::deque'
stldeftab['set']          = '=','std::less','std::allocator'
stldeftab['multiset']     = '=','std::less','std::allocator'
stldeftab['stack']        = '=','std::deque'
stldeftab['vector']       = '=','std::allocator'
stldeftab['basic_string'] = '=','std::char_traits','std::allocator'
if sys.platform == 'win32' :
  stldeftab['hash_set']      = '=', 'stdext::hash_compare', 'std::allocator'
  stldeftab['hash_multiset'] = '=', 'stdext::hash_compare', 'std::allocator'
  stldeftab['hash_map']      = '=', '=', 'stdext::hash_compare', 'std::allocator'
  stldeftab['hash_multimap'] = '=', '=', 'stdext::hash_compare', 'std::allocator'
else :
  stldeftab['hash_set']      = '=','__gnu_cxx::hash','std::equal_to','std::allocator'
  stldeftab['hash_multiset'] = '=','__gnu_cxx::hash','std::equal_to','std::allocator'
  stldeftab['hash_map']      = '=','=','__gnu_cxx::hash','std::equal_to','std::allocator'
  stldeftab['hash_multimap'] = '=','=','__gnu_cxx::hash','std::equal_to','std::allocator'  
#---------------------------------------------------------------------------------------
def getTemplateArgs( cl ) :
  if cl.find('<') == -1 : return []
  args, cnt = [], 0
  for s in string.split(cl[cl.find('<')+1:cl.rfind('>')],',') :
    if   cnt == 0 : args.append(s)
    else          : args[-1] += ','+ s
    cnt += s.count('<')-s.count('>')
  if args[-1][-1] == ' ' : args[-1] = args[-1][:-1]
  return args
#---------------------------------------------------------------------------------------
def getTemplateArgString( cl ) :
  bc = 0
  if cl[-1] != '>' : return ''
  for i in range( len(cl)-1, -1, -1) :
    if   cl[i] == '>' : bc += 1
    elif cl[i] == '<' : bc -= 1
    if bc == 0 : return cl[i:]
  return ''
#---------------------------------------------------------------------------------------
## def normalizeClass(name):
##   name = name.strip()  # strip the front and back spaces
##   nname = name[0]      # copy the first character
##   x = 1          
##   while x < len(name): # loop until end of name
##     if name[x] == ' ': # if we discover a space
##       lx = x-1         # remember the last character
##       while name[x+1] == ' ': x += 1 # consume spaces until next character
##       # if we are between typenames (i.e. it ends with a char or num and starts with a char)
##       # or we are between closing angular braces
##       if (name[lx].isalnum() and name[x+1].isalpha()) or (name[lx] == '>' and name[x+1] == '>'):
##         nname += name[x:x+2] # copy the next two characters
##         x += 1
##     else : nname += name[x]  # if its not a space copy the current character
##     x += 1                   # proceed to next character
##   return nname
def normalizeClassAllTempl(name)   : return normalizeClass(name,True)
def normalizeClassNoDefTempl(name) : return normalizeClass(name,False)
def normalizeClass(name,alltempl) :
  names, cnt = [], 0
  for s in string.split(name,'::') :
    if cnt == 0 : names.append(s)
    else        : names[-1] += '::' + s
    cnt += s.count('<')-s.count('>')
  if alltempl : return string.join(map(normalizeFragmentAllTempl,names),'::')
  else        : return string.join(map(normalizeFragmentNoDefTempl,names),'::')
#--------------------------------------------------------------------------------------
def normalizeFragmentAllTempl(name)   : return normalizeFragment(name,True)
def normalizeFragmentNoDefTempl(name) : return normalizeFragment(name) 
def normalizeFragment(name,alltempl=False) :
  name = name.strip()
  if name.find('<') == -1  : 
    nor =  name
    for e in [ ['long long unsigned int', 'unsigned long long'],
             ['long long int',          'long long'],
             ['unsigned short int',     'unsigned short'],
             ['short unsigned int',     'unsigned short'],
             ['short int',              'short'],
             ['long unsigned int',      'unsigned long'],
             ['unsigned long int',      'unsigned long'],
             ['long int',               'long']] :
      nor = nor.replace(e[0], e[1])
    return nor
  else                     : clname = name[:name.find('<')]
  if name.rfind('>') == -1 : suffix = ''
  else                     : suffix = name[name.rfind('>')+1:]
  args = getTemplateArgs(name)
  if not alltempl and clname in stldeftab :
    # select only the template parameters different from default ones
    sargs = []
    for i in range(len(args)) :  
      if args[i].find(stldeftab[clname][i]) == -1 : sargs.append(args[i])
    nor = clname + '<' + string.join(map(normalizeClassNoDefTempl,sargs),',')
  else :
    nor = clname + '<' + string.join(map(normalizeClassAllTempl,args),',')
  if nor[-1] == '>' : nor += ' >' + suffix
  else              : nor += '>' + suffix
  return nor
#--------------------------------------------------------------------------------------
def clean(a) :
  r = []
  for i in a :
	if i not in r : r.append(i)
  return r
