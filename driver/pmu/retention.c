
#if 0
int pmu_l2_bfr_slp(unsigned int src_addr, unsigned int len,unsigned int src_attr)
{
    volatile unsigned int *dram_data_addr, src, temp_data;
    unsigned int i, dram_data_len;
    int b, c;
    
    // make src and dst ncnb memory
    src = src_addr; 
    b = mmu_is_L1_bufferable(src, 10000);   
    c = mmu_is_L1_cacheable(src, 10000);    

    if(b | c)
    {
        src_attr= mmu_get_mem_attribute(src & 0xfff00000);
        if(mmu_change_mem_attribute(src& 0xfff00000, 0))
        {
            return MV_FAIL; 
        }
    }

    dram_data_addr = (unsigned int *)src_addr;
    dram_data_len = len;
    // init sdram data
    for(i = 0; i< dram_data_len /4; i++)
    {
        *dram_data_addr ++ = 0x5a5a0000 + i;
    }   
    
    // put src back to caba
    if(mmu_change_mem_attribute(src& 0xfff00000, src_attr))
    {
        return MV_FAIL; 
    }

    // enable L2
    // setup mmu or pmu
    l2_cache_enable();
    
    // modify cache data
    dram_data_addr = (unsigned int *)src_addr;
    for(i = 0; i< dram_data_len /4; i++)
    {
        temp_data = *dram_data_addr;
        *dram_data_addr = 0xa5a5a5a5;
        dram_data_addr++;       
    }   

    //L1_dcache_cleaninvalid_all();
    L1_dcache_cleaninvalid();
    L1_writebuffer_drain();

    /* Now l2 has new data, disable it now */
    l2_cache_disable();

    return MV_OK;
}

int pmu_l2_aft_slp(unsigned int src_addr, unsigned int len, unsigned int src_attr)
{
    volatile unsigned int *dram_data_addr, src, temp_data;
    unsigned int i, dram_data_len;
    int b, c;
    
    dram_data_addr = (unsigned int *)src_addr;
    dram_data_len = len;
    // chnage the ddr data again. It goes only to DDR Now
    for(i = 0; i< dram_data_len /4; i++) 
    {
        *dram_data_addr = 0x0;
        *dram_data_addr++ = 0x3c3c0000 + i;
    }   
    // put src back to caba
    /* This will clean and invalidate the l1 and l2 $es*/
    if(mmu_change_mem_attribute(src& 0xfff00000, src_attr))
    {
        return MV_FAIL; 
    }

    L1_dcache_cleaninvalid_all();
    L1_writebuffer_drain();
    l2_cahce_cleanandinvalidate_all();
    
    /* NOW ddr should have the updated l2 values*/
    dram_data_addr = (unsigned int *)src_addr;
    dram_data_len = len;
    // chnage the ddr data again. It goes only to DDR Now
    for(i = 0; i< dram_data_len /4; i++)
    {
        if ( *dram_data_addr++ != 0xa5a5a5a5 )
            return MV_FAIL;
    }

    return MV_OK;
}

#endif


