import os, sys, time, traceback
import rundb_params

_debug = False

# ------------------------------------------------------------------------------
def _createConnection(username, password, database):
    import cx_Oracle
    db = cx_Oracle.Connection(username, password, database)
    return db

def _fetch(cursor):
    result = []
    row = cursor.fetchone()
    while row:
        res = [i for i in row]
        result.append(res)
        row = cursor.fetchone()
    return result

def makeXML(tag,o):
  run_no = int(o[0])
  f = open(str(o[0])+'.run','w')
  print >>f, '<'+tag+' runid="'+str(o[0])+'" year="'+str(o[1])+'" month="'+str(o[2])+'">'
  print >>f, '  <param name="partitionname" value="'+str(o[5])+'"/>'
  print >>f, '  <param name="startdate" value="'+str(o[3])+'"/>'
  print >>f, '  <param name="enddate" value="'+str(o[4])+'"/>'
  print >>f, '  <param name="runtype" value="'+str(o[6])+'"/>'
  print >>f, '  <param name="activity" value="'+str(o[7])+'"/>'
  print >>f, '  <param name="tck" value="'+str(hex(0xFFFFFFFF&int(o[8])))+'"/>'
  print >>f, '  <param name="program" value="'+str(o[9])+'"/>'
  print >>f, '  <param name="programVersion" value="'+str(o[10])+'"/>'
  for i in o[11]:
    print >>f, '  <param name="'+i[0]+'" value="'+str(i[1])+'"/>'
  print >>f, '</'+tag+'>'
  f.close()

def make_directory(nam):
  try:
    os.mkdir(nam)
  except:
    pass
 
# ===============================================================================
def run():
    make_directory('ToDo')
    make_directory('Done')
    make_directory('Failed')
    os.chdir('ToDo')
    db=_createConnection(rundb_params.logon,rundb_params.pwd,rundb_params.tns)
    cursor = db.cursor()
    stmt = """SELECT * FROM (
                 SELECT r.runid AS runid,
                        TO_CHAR(r.starttime,'YYYY') AS year,
                        TO_CHAR(r.starttime,'MM') AS month,
                        TO_CHAR(r.starttime,'YYYY-MM-DD HH24:MI:SS') AS starttime,
                        TO_CHAR(r.endtime,  'YYYY-MM-DD HH24:MI:SS') AS endtime,
                        r.partitionname AS pname,
                        r.runtype AS runtype,
                        r.activity AS activity,
                        TO_NUMBER(r.tck) AS tck,
                        p1.value AS program,
                        p2.value AS programVersion
                 FROM   rundbruns r, rundbrunparams p1, rundbrunparams p2
                        WHERE r.partitionname='LHCb' AND p1.name='program' AND p2.name='programVersion' AND
                              p1.runid=p2.runid AND r.runid=p1.runid AND p1.value<>'DEFAULT' AND
                              r.starttime>TO_DATE('2015/01/01','yyyy/mm/dd')
                        ORDER BY r.runid desc)
                 WHERE rownum<10"""
    if not cursor.execute(stmt):
        raise 'Could not execute statement %s' %stmt
    runs = _fetch(cursor)

    for r in runs:
      print r
      stmt = """SELECT name, value, type 
                FROM rundbrunparams 
                WHERE runid=%d"""%(r[0],)
      
      if not cursor.execute(stmt):
        raise 'Could not execute statement %s' %stmt
      params = _fetch(cursor)
      r.append(params)

    for r in runs:
      makeXML('run',r)

# ===============================================================================
if __name__=="__main__":
  run()
