import os
import gparser

#================================================================================
class genClassDicts:
#--------------------------------------------------------------------------------
  def __init__(self, godRoot, dictOutputDir, srcOutputDir):
    self.godRoot          = godRoot
    self.dictOutputDir    = dictOutputDir
    if srcOutputDir == '.' : self.srcOutputDir = 'Event/'
    else                   : self.srcOutputDir = srcOutputDir.replace('\\','/').replace('../','')
    if self.srcOutputDir[-1] != '/' : self.srcOutputDir += '/' 
    self.packageDict      = {}
    self.sIncludes        = []
    self.sDictInstances   = []
    self.sClassSelections = []
    self.sClassExclusions = []
#--------------------------------------------------------------------------------
  def conc(self, to, src):
    if src not in to : to.append(src)
    return to
#--------------------------------------------------------------------------------
  def clean(self, str):
    str = str.replace('*','p')
    str = str.replace(':','_')
    return str
#--------------------------------------------------------------------------------
  def genFirstLevelIncludes(self, godPackage):
    if godPackage.has_key('class'):
      pass
    elif godPackage.has_key('namespace'):
      pass
#--------------------------------------------------------------------------------
  def genPackageDict(self,godPackage):    
    if godPackage.has_key('class'):
      for cl in godPackage['class']:
        clname = cl['attrs']['namespace']+'::'+cl['attrs']['name']
        ##clname = cl['attrs']['name']
        # add include file line
        self.sIncludes = self.conc(self.sIncludes, '#include "%s%s.h"' % (self.srcOutputDir, cl['attrs']['name']))
        # get the class id
        if cl['attrs'].has_key('id'):
          id = cl['attrs']['id']
          sid = ' id="%08x-0000-0000-0000-000000000000"' %int(id)
        else:
          sid=''
        cnstr =  '  <class name="%s"%s' % (clname,sid)
        trans = False
        # add transient flags if they exist
        if cl.has_key('attribute'):
          for attr in cl['attribute']:
            if attr['attrs'].get('transient') == 'TRUE' :
              if trans == False :
                cnstr += '>'
                trans = True
	      cnstr += ' <field name="m_%s" transient="true"/>' % (attr['attrs']['name'])
        if trans:
          cnstr += ' </class>'
        else:
          cnstr += '/>'
        # add class name for selection file
        self.sClassSelections = self.conc(self.sClassSelections, cnstr)
        if cl.has_key('template'):
          for t in cl['template']:
            # name of template class
            tcname = t['attrs']['name']
            if tcname[-4:] == 'Dict' : tcname = tcname[:-4]
            # name of first template parameter (THIS is a short cut to the class name)
            t1name = t['attrs']['t1'].replace('THIS',clname)
            # stl vector is a bit special
            if tcname.lower() == 'vector':
              # include header file
              self.sIncludes = self.conc(self.sIncludes, '#include <vector>')
              # include template instantiation
              self.sDictInstances = self.conc(self.sDictInstances, 'std::vector<%s> m_std_vector_%s;' % (t1name, self.clean(t1name)))
              # include element for selection file
              self.sClassSelections = self.conc(self.sClassSelections, '  <class name="std::vector<%s>"/>' % t1name)
	    elif tcname == "KeyedObject":
              if t1name != 'int':
                # "GaudiKernel" hardcoded here to avoid usage of the inclusion mechanism (not nice)
                self.sIncludes = self.conc(self.sIncludes, '#include "GaudiKernel/KeyedObject.h"')
                # include template instantiation
                self.sDictInstances = self.conc(self.sDictInstances, 'KeyedObject<%s> m_KeyedObject_%s;' % (t1name, self.clean(t1name)))
                # include element for selection file
                ko =  '  <class name="KeyedObject<%s>">' % (t1name)
                ko += ' <field name="m_hasKey" transient="true"/>'
                ko += ' <field name="m_refCount" transient="true"/>'
                ko += ' </class>'
                self.sClassSelections = self.conc(self.sClassSelections, ko)
            elif tcname == "KeyedContainer":
              # "GaudiKernel" hardcoded here to avoid usage of the inclusion mechanism (not nice)
              self.sIncludes = self.conc(self.sIncludes, '#include "GaudiKernel/%s.h"' % tcname)
              # include template instantiation
              self.sDictInstances = self.conc(self.sDictInstances, '%s<%s> m_%s_%s;' % (tcname, t1name, tcname, self.clean(t1name)))
              # Add container id to class id
              if cl['attrs'].has_key('id'):
                sid = ' id="%08x-0000-0000-0000-000000000000"' %(int(id)+0x60000)
              # include element for selection file
              kc =  '  <class name="%s<%s,Containers::KeyedObjectManager<Containers::hashmap> >"%s>' % (tcname, t1name,sid)
	      kc += ' <field name="m_cont" transient="true"/>'
	      kc += ' <field name="m_random" transient="true"/>'
	      kc += ' </class>'
              self.sClassSelections = self.conc(self.sClassSelections, kc)
	    elif tcname == "SmartRef":	
              # "GaudiKernel" hardcoded here to avoid usage of the inclusion mechanism (not nice)
              self.sIncludes = self.conc(self.sIncludes, '#include "GaudiKernel/SmartRef.h"')
              # include template instantiation
              self.sDictInstances = self.conc(self.sDictInstances, 'SmartRef<%s> m_SmartRef_%s;' % (t1name, self.clean(t1name)))
              # include element for selection file
	      sr =  '  <class name="SmartRef<%s>">' % (t1name)
	      sr += ' <field name="m_target" transient="true"/>'
	      sr += ' </class>'
              self.sClassSelections = self.conc(self.sClassSelections, sr)
	    elif tcname == "SmartRefVector":
              # "GaudiKernel" hardcoded here to avoid usage of the inclusion mechanism (not nice)
              self.sIncludes = self.conc(self.sIncludes, '#include "GaudiKernel/SmartRefVector.h"')
              self.sIncludes = self.conc(self.sIncludes, '#include "GaudiKernel/SmartRef.h"')
              self.sIncludes = self.conc(self.sIncludes, '#include <vector>')
              # include template instantiation
              self.sDictInstances = self.conc(self.sDictInstances, 'SmartRefVector<%s> m_SmartRefVector_%s;' % (t1name, self.clean(t1name)))
              self.sDictInstances = self.conc(self.sDictInstances, 'std::vector<SmartRef<%s> > m_std_vector_SmartRef_%s;' % (t1name, self.clean(t1name)))
              # include element for selection file
	      srv =  '  <class name="SmartRefVector<%s>">' % (t1name)
	      srv += ' <field name="m_contd" transient="true"/>'
	      srv += ' <field name="m_data" transient="true"/>'
	      srv += ' </class>'
              self.sClassSelections = self.conc(self.sClassSelections, srv)
              self.sClassSelections = self.conc(self.sClassSelections, '  <class name="std::vector<SmartRef<%s> >"/>' % (t1name))
            elif tcname == "ObjectVector":
              # "GaudiKernel" hardcoded here to avoid usage of the inclusion mechanism (not nice)
              self.sIncludes = self.conc(self.sIncludes, '#include "GaudiKernel/%s.h"' % tcname)
              # include template instantiation
              self.sDictInstances = self.conc(self.sDictInstances, '%s<%s> m_%s_%s;' % (tcname, t1name, tcname, self.clean(t1name)))
              # Add container id to class id
              if cl['attrs'].has_key('id'):
                sid = ' id="%08x-0000-0000-0000-000000000000"' %(int(id)+0x20000)
              # include element for selection file
              self.sClassSelections = self.conc(self.sClassSelections, '  <class name="%s<%s>"%s/>' % (tcname, t1name,sid))
            else:
              # "GaudiKernel" hardcoded here to avoid usage of the inclusion mechanism (not nice)
              self.sIncludes = self.conc(self.sIncludes, '#include "GaudiKernel/%s.h"' % tcname)
              # include template instantiation
              self.sDictInstances = self.conc(self.sDictInstances, '%s<%s> m_%s_%s;' % (tcname, t1name, tcname, self.clean(t1name)))
              # include element for selection file
              self.sClassSelections = self.conc(self.sClassSelections, '  <class name="%s<%s>"/>' % (tcname, t1name))
#--------------------------------------------------------------------------------
  def appendFile(self, fileName, tmplname):
    file = open(fileName)
    if tmplname == 'dictionary_h':
      appendIncludes = 0
      appendInstances = 0
      for line in file.readlines():
        if line.find('// begin include files') != -1    : appendIncludes = 1
        elif line.find('// end include files') != -1    : appendIncludes = 0
        elif line.find('// begin instantiations') != -1 : appendInstances = 1
        elif line.find('// end instantiations') != -1  : appendInstances = 0
        elif (appendIncludes)  : self.sIncludes      = self.conc(self.sIncludes, line[:-1])
        elif (appendInstances) : self.sDictInstances = self.conc(self.sDictInstances, ' '.join(line[:-1].split()))
    elif tmplname == 'selection_xml':
      appendSelections = 0
      appendExclusions = 0
      for line in file.readlines():
        if line.find('<lcgdict>') != -1    : appendSelections = 1
        elif line.find('</lcgdict>') != -1 : appendSelections = 0
        elif line.find('<exclusion>')  != -1 : appendExclusions = 1
        elif line.find('</exclusion>') != -1 : appendExclusions = 0
        elif (appendExclusions):
          if line != '\n' : self.sClassExclusions.append(line)
        elif (appendSelections):
          self.sClassSelections = self.conc(self.sClassSelections, line[:-1])
 #--------------------------------------------------------------------------------
  def align(self, dictInstances):
    dictInstances2 = []
    maxlen = 0;
    for inst in dictInstances :
      thislen = len(' '.join(inst.split()[:-1]))
      if thislen > maxlen : maxlen = thislen
    maxlen += 1
    for inst in dictInstances :
      first = ' '.join(inst.split()[:-1])
      second = inst.split()[-1]
      dictInstances2.append('    '+first.ljust(maxlen)+second)
    return dictInstances2
#--------------------------------------------------------------------------------
  def genFile(self, packagename, tmplname): 
    #instantiate a file parser
    g = gparser.gparser() 
    # the file name is the package name plus the template name (. replaces _)
    fileName = '%s_%s' % (packagename,tmplname.replace('_','.'))
    fileFullName = self.dictOutputDir+os.sep+fileName
    print '    File %s' % fileName,
    if os.path.isfile(fileFullName):
      print '(exists - appending)',
      self.appendFile(fileFullName, tmplname)
    # a bit of sorting 
    self.sIncludes.sort()
    self.sIncludes.reverse()
    self.sDictInstances.sort()
    self.sDictInstances = self.align(self.sDictInstances)
    self.sClassSelections.sort()
    # fill the dictionary used for generation of files
    self.packageDict['includes']        = '\n'.join(self.sIncludes)
    self.packageDict['dictInstances']   = '\n'.join(self.sDictInstances)
    self.packageDict['classSelections'] = '\n'.join(self.sClassSelections)
    self.packageDict['classExclusions'] = ''.join(self.sClassExclusions)
    # parse the template file and generate the content for the file
    g.parse(self.godRoot+'templates/'+tmplname+'.tpl',self.packageDict)
    # open file and write the parser content to it
    file = open(fileFullName,'w')
    file.write(g.stream)
    file.close()
    print ' - Done'    
#--------------------------------------------------------------------------------
  def doit(self,godPackage):
    packagename = godPackage['attrs']['name']
    # fill the dictionary for the templates
    self.genPackageDict(godPackage)
    # generate dictionary header file
    self.genFile(packagename, 'dictionary_h')               
    # generate selection file
    self.genFile(packagename, 'selection_xml')              
