#--------------------------------------------------------------
# Standard Options to use the Detector Geometry
#--------------------------------------------------------------
ApplicationMgr.ExtSvc.append('XmlCnvSvc')
ApplicationMgr.ExtSvc.append('XmlParserSvc')
ApplicationMgr.DLLs.append('DetDesc')
ApplicationMgr.DLLs.append('XmlTools')
ApplicationMgr.DLLs.append('DetDescCnv')

#--------------------------------------------------------------
# Location of detector description XML database is a URL
# so never use "\" (Use only "/")
#--------------------------------------------------------------
DetectorDataSvc = Service('DetectorDataSvc')
DetectorDataSvc.UsePersistency = true
DetectorDataSvc.DetDbRootName  = "dd";
DetectorDataSvc.DetStorageType = 10;

# Detector Persistency service setup: 
DetectorPersistencySvc = Service('DetectorPersistencySvc')
DetectorPersistencySvc.CnvServices = ['XmlCnvSvc']

# Trigger generic detector element conversion
# Useful in the case of visualization or in a case the required user
# defined detector element converters are somehow not available
XmlCnvSvc = Service('XmlCnvSvc')
XmlCnvSvc.AllowGenericConversion = true;

# Sets the maximum number of files to be put in the cache
# of the XmlCnvSvc
XmlParserSvc = Service('XmlParserSvc')
XmlParserSvc.MaxDocNbInCache = 7;

# Changes the behavior of the cache of the XmlCnvService
# A 0 value makes it FIFO while a bigger values tends
# to make it keep only reused files.
XmlParserSvc.CacheBehavior = 3;

