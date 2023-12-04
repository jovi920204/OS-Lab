echo "How many data do you want to write?"

read times

for ((i = 0; i <= times*512; i+=512))
do
   echo $i
   ./ssd_fuse_dut /tmp/ssd/ssd_file w 512 $i
   wait
done