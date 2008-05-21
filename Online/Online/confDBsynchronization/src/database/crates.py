import cx_Oracle
class crateDTO:
    def __init__(self, label):
        self.label = label
        self.elements = []
    def __str__(self):
        return self.label

con=cx_Oracle.connect("lhcbintegration","itgrlhcb","lhcb_integration")
c=con.cursor()
sel="select c.label from crate c where ((label like 'D3%') OR (label like 'D2%')) and height=9 order by label"
c.execute(sel)
cratesDB=c.fetchall()
con.close()

crates = []
for crate in cratesDB:
    crates.append(crateDTO(crate[0]))
