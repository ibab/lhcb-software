import DbCore, RunDatabase
import DimRunDb

DbCore._ms_access = 1

if __name__ == "__main__":
  db = RunDatabase.RunDatabase('RunDatabase')
  