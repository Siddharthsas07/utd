import subprocess
import sys
import time

outpath = "output.txt"
outfile = open(outpath, "w")
sys.stdout = outfile # set this output to outfile 

def runTest(uaiFile, uaiEvid, numSamples):
  for i in range(10):
    # run the homework jar with the given parameters, redirect output to outfile as well
    subprocess.call(["java", "-Xmx1000M", "-jar", "aosampling.jar", uaiFile, uaiEvid, numSamples], stdout=outfile, stderr=outfile, shell=False)
    outfile.flush()
  
def main():
  uaiPath = "data/test/"
  files = ["grid3x3"]
  s = ["100", 
    "1000", 
    "10000", 
    "100000"]

  for sval in s:
    for filename in files:
      uaifilePath = uaiPath + filename + ".uai"
      uaievidPath = uaiPath + filename + ".uai.evid"
      
      #python 3 made print a function call so it has to have ()s.
      #python 2 has print as a single inline. No parens.
      print("\"" , uaifilePath, sval , "\"")
      print("z", ",t")
      outfile.flush()
      runTest(uaifilePath, uaievidPath, sval)
      print("")
          
# Execution begins here
if __name__ == "__main__":
  main()
  
outfile.close()
