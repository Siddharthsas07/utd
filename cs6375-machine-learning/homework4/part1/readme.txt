1) The output images are located at /data/<image>-out-<k>.png

2) The file size results are stored in results.(ods|pdf) along with the compression ratios. 
   For each k value, the program was run 3 times and the size used to calculate the reported
   compression ratio was the average of the 3.
   
3) Yes there is a tradeoff between image quality and degree of compression. Higher quality
   images result in higher file sizes. You could get away with compressing with k ~= 20, but
   for a pretty good image, you'd want to go with k >= 30.