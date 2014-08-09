import subprocess
import sys
import time

outpath = "homework3_5.output.txt"
outfile = open(outpath, "a")
sys.stdout = outfile # set this output to outfile 

def runTest(uaiFile, uaiEvid, w, N, sampleType):
  for i in range(10):
    # run the homework jar with the given parameters, redirect output to outfile as well
    subprocess.call(["java", "-Xmx1000M", "-jar", "homework3_5.jar", uaiFile, uaiEvid, "1", "0", w, N, sampleType], stdout=outfile, stderr=outfile, shell=False)
    outfile.flush()
  
def main():
  uaiPath = "uai-benchmarks/uai/"
  evidPath = "uai-benchmarks/evid/"
  files = ["2_17_s.binary",
           "2_28_s.binary",
           "17_4_s.binary",
           "54.wcsp",
           "404.wcsp",
           "grid16_5",
           "grid18_10"]
  w = ["1", "2", "3", "4", "5"]
  N = ["100", "1000", "10000", "20000"]
  s = ["0", "1"]
  
  for wval in w:
    for nval in N:
      for sval in s:
        for filename in files:
          uaifilePath = uaiPath + filename + ".uai"
          uaievidPath = evidPath + filename + ".uai.evid"
          
          print(filename + ".uai", filename + ".uai.evid", "1", "0", wval, nval, sval)
          outfile.flush()
          runTest(uaifilePath, uaievidPath, wval, nval, sval)
          
# Execution begins here
if __name__ == "__main__":
  start = time.time()
  main()
  end = time.time()
  print("Testing completed in", end - start, "s.")
  
outfile.close()