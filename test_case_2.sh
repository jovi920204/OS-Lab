
# write 1 ~ 140
for i in {1..140}; do echo $i >> /tmp/ssd/ssd_file; done

# 10 bytes overwrite
for i in {0..99}; do ./ssd_fuse_dut /tmp/ssd/ssd_file w 10 $(($i * 512)); done