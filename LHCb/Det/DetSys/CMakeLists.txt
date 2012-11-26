################################################################################
# Package: DetSys
################################################################################
gaudi_subdir(DetSys v17r2)

gaudi_depends_on_subdirs(Det/BcmDet
                         Det/CaloDet
                         Det/CaloDetXmlCnv
                         Det/DDDB
                         Det/DetCond
                         Det/DetDesc
                         Det/DetDescCnv
                         Det/DetDescSvc
                         Det/Magnet
                         Det/MuonDet
                         Det/OTDet
                         Det/RichDet
                         Det/STDet
                         Det/VeloDet
                         Tools/XmlTools)

