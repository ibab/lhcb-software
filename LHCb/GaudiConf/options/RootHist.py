# $Id: RootHist.py,v 1.1 2001-11-29 07:49:16 mato Exp $
#-------------------------------------------------------------
# Standard options for ROOT histogram and N-tuple persistency
#-------------------------------------------------------------
ApplicationMgr.DLLs = ApplicationMgr.DLLs + ['RootHistCnv']
ApplicationMgr.HistogramPersistency = 'ROOT'
