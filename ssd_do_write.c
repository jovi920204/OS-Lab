static int ssd_do_write(const char *buf, size_t size, off_t offset)
{
    /*  TODO: only basic write case, need to consider other cases */
    int tmp_lba, tmp_lba_range, process_size;
    int idx, curr_size, remain_size, rst;

    host_write_size += size;
    if (ssd_expand(offset + size) != 0)
    {
        return -ENOMEM;
    }

    tmp_lba = offset / 512;
    tmp_lba_range = (offset + size - 1) / 512 - (tmp_lba) + 1;
    process_size = 0;
    remain_size = size;
    if (*(buf + remain_size - 1) == '\n')
        remain_size--;
    curr_size = 0;

    // consider case:
    //   [x] 1. offset align 512 && input size align 512 (sample code)
    //   [x] 2. offset align 512 && input size "not" align 512 (yuchen)
    //   [x] 3. offset "not" align 512 && input size align 512
    //   [x] 4. offset "not" align 512 && input size "not" align 512

    // process flow:
    //   if size || offset not align 512, should read the data from nand and overwrite it
    char alignBuf[512] = {'\0'};

    for (idx = 0; idx < tmp_lba_range; idx++)
    {
        /*  example only align 512, need to implement other cases  */

        if (offset % 512 == 0)
        {
            printf(">>>>> ssd_do_write Case 1\n");
            if (!ftl_read(alignBuf, tmp_lba)) // LBA space is clear
            {
                rst = ftl_write(buf + process_size, 1, tmp_lba + idx);
            }
            else
            {
                if (remain_size > 512)
                    memcpy(&alignBuf, buf + process_size, 512);
                else
                    memcpy(&alignBuf, buf + process_size, remain_size);
                rst = ftl_write(alignBuf, 1, tmp_lba + idx);
            }
            if (rst == 0)
            {
                // write full return -enomem;
                return -ENOMEM;
            }
            else if (rst < 0)
            {
                // error
                return rst;
            }
            curr_size += 512;
            remain_size -= 512;
            process_size += 512;
            offset += 512;
        }
        else if (offset % 512 != 0)
        {
            ftl_read(alignBuf, tmp_lba);
            int offset_in_lba = offset % 512;
            if ((offset + remain_size) / 512 == tmp_lba) // all in same lba page
            {
                printf(">>>>> ssd_do_write Case 2\n");
                memcpy(alignBuf + offset_in_lba, buf, remain_size);
                rst = ftl_write(alignBuf, 1, tmp_lba + idx);
            }
            else
            {
                printf(">>>>> ssd_do_write Case 3\n");
                memcpy(alignBuf + offset_in_lba, buf, 512 - offset_in_lba);
                rst = ftl_write(alignBuf, 1, tmp_lba + idx);
            }
            

            if (rst == 0)
            {
                // write full return -enomem;
                return -ENOMEM;
            }
            else if (rst < 0)
            {
                // error
                return rst;
            }
            curr_size += 512;
            remain_size -= (512 - offset_in_lba);
            process_size += (512 - offset_in_lba);
            offset += (512 - offset_in_lba);
        }
        else
        {
            printf(" --> Something Wrong at ssd_do_write function...\n");
            return -EINVAL;
        }
    }

    return size;
}