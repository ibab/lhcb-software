import importUtils, gparser, os

#================================================================================
class genAssociations(importUtils.importUtils):
#--------------------------------------------------------------------------------
  def __init__(self,cdb,godRoot):
    importUtils.importUtils.__init__(self,cdb)
    self.godRoot = godRoot
#--------------------------------------------------------------------------------
  def genAssociations(self,godAssociations):
    s = ''
    for assoc in godAssociations:
      assocAtt = assoc['attrs']
      self.addInclude(assocAtt['from'])
      self.addInclude(assocAtt['to'])
      wgt = ''
      wgtType = ''
      if assocAtt['weight'] != 'NONE' :
        wgt = 'Weighted'
        wgtType = ', %s' % assocAtt['weight']
      s += '      GaudiDict::Relation%s%sDict<%s, %s%s>();\n' % (wgt,assocAtt['type'],assocAtt['from'],assocAtt['to'],wgtType)
    return s[:-1]
#--------------------------------------------------------------------------------
  def doit(self,package,godAssociations,outputDir,lname):
    
    assocDict = package.dict

    fileName = '%s_Associations_dict.cpp' % assocDict['packagename']

    print '    Generating file %s' % fileName.ljust(lname),

    assocDict['associations']   = self.genAssociations(godAssociations)
    assocDict['includes']       = self.genIncludes()

    g = gparser.gparser()
    g.parse(self.godRoot+'templates/associations.tpl',assocDict)

    file = open(outputDir+os.sep+fileName,'w')
    file.write(g.stream)
    file.close()

    print ' - Done'

#================================================================================
#import xparser
#if __name__ == '__main__':
#  x = xparser.xparser('xml_files/EventAssoc.xml','xml_files/GODsClassDB.xml')
#  x.parse()
#  g = genAssociations(x.gdd,x.cdb)
#  g.doit()
