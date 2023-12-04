#!/bin/bash


./ssd_fuse_dut /tmp/ssd/ssd_file w 512 0
./ssd_fuse_dut /tmp/ssd/ssd_file w 512 512
./ssd_fuse_dut /tmp/ssd/ssd_file w 512 1024
./ssd_fuse_dut /tmp/ssd/ssd_file w 512 1536

COMMAND="./ssd_fuse_dut /tmp/ssd/ssd_file w 10240 2048"

for i in {1..136}
do
    $COMMAND
done