# $Id: Hbook.py,v 1.1 2001-11-29 07:49:16 mato Exp $
#-------------------------------------------------------------
# Standard options for HBOOK histogram and N-tuple persistency
#-------------------------------------------------------------
ApplicationMgr.DLLs = ApplicationMgr.DLLs + ['HbookCnv']
ApplicationMgr.HistogramPersistency = 'HBOOK'
