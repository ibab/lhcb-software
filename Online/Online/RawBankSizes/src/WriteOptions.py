import os
if os.name == "posix":
  f=open("/tmp/banks.in","r")
else:
  f=open(os.environ['TMP']+"\\banks.in","r")

aaaa = os.getenv("RAWBANKSIZESROOT")
#print aaaa
g=open(aaaa+"/options/RawSize_HistParams.opts","r+")
banks = f.readlines()
banks.append("TotEv")
banks.append("HLTRatio");
opts = g.readlines()
opts.append("shit")
newbanks = []
outopts = []
f.close()
#print banks
#print opts
print "========= Running WriteOptions.py...."
#print (len(banks), len(opts))
for i in range(len(opts)):
 opts[i] = opts[i].replace("//","")
for i in range(len(banks)):
  bank = banks[i].split("\n")
  banks[i] = bank[0]
  bank = banks[i].split(" ")
  banks[i] = bank[0];
  found = False
  for j in range(len(opts)):
    if opts[j].find("//")<0:
      optbank = iii=opts[j].split("=")[0].split(".")[1]
      ii=opts[j].find(bank[0])
      if bank[0]==optbank:
        #print ("options for bank "+banks[i] +" "+optbank+" found. Adding to outopts")
        outopts.append(opts[j])
        opts.remove(opts[j])
        found = True
        break
  if found:
    continue
  else:
    print("No options found for bank ",bank[0]," ",optbank," adding default options")
    outopts.append("RawBankSizes."+banks[i]+"=(100,0.0,10000.0,0,1024,\"Unknown\");"+"\r\n")
opts.remove("shit") #len(opts)-1)
for i in range(len(opts)):
 opts[i] = "//"+opts[i]
#print newbanks

outopts.sort()
#print(opts)
g.seek(0)
g.truncate()
g.writelines(outopts)
g.writelines(opts)
g.close()
