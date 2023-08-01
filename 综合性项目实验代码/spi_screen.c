#include "spi_screen.h"

static void dump_buf(unsigned char * buf,unsigned int len)
{
    for(int i = 0; i < len; i++)
    {
        printf("%#x ",buf[i]);
    }
    putchar('\r'); putchar('\n');
}

hi_u32 screen_spi_init(hi_spi_idx id, const hi_spi_cfg_basic_info *spi_cfg_basic_info, hi_bool slave)
{
    hi_u32 ret;
    hi_spi_cfg_init_param init_param;
    init_param.is_slave = slave;
    ret = hi_spi_init(id, init_param, spi_cfg_basic_info);

    test_spi_printf("app_demo_spi_init ret=%x", ret);
    return ret;
}

hi_void screen_spi_master_init(hi_spi_idx spi_id){
    int ret = -1;//screen_ERR;
    test_spi_para spi_para;

    spi_para.spi_id = spi_id;
    spi_para.irq = HI_FALSE;
    //spi_para.length = length;
    //spi_para.loop = loop;
    spi_para.cfg_info.data_width = HI_SPI_CFG_DATA_WIDTH_E_8BIT;
    spi_para.cfg_info.cpha = HI_SPI_CFG_CLOCK_CPHA_0;
    spi_para.cfg_info.cpol = HI_SPI_CFG_CLOCK_CPOL_0;
    spi_para.cfg_info.fram_mode = HI_SPI_CFG_FRAM_MODE_MOTOROLA;
    spi_para.cfg_info.endian = HI_SPI_CFG_ENDIAN_LITTLE;
    spi_para.slave = HI_FALSE;
    spi_para.lb = HI_FALSE;
    spi_para.dma_en = HI_FALSE;
    //g_delay_us_spi = 100;  /* 100 us */
    //spi_para.cfg_info.freq = 8000000; /* defaul freq 8000000 Hz */
    spi_para.cfg_info.freq = 2000000; /* defaul freq 8000000 Hz */
    //spi_para.cfg_info.freq = 200000; /* defaul freq 8000000 Hz */
    //spi_para.cfg_info.freq = 100000; /* defaul freq 8000000 Hz */
    //app_demo_spi_print_test_para(&spi_para);
    
    test_spi_printf("app_demo_spi_test_cmd_mw_sr Start");

    ret = screen_spi_init(spi_para.spi_id, &(spi_para.cfg_info), spi_para.slave);
    if (ret == HI_ERR_SUCCESS) {
        test_spi_printf("SPI init succ!");
    } else {
        test_spi_printf("SPI init fail! %x ", ret);
        return;
    }
    
    hi_spi_set_loop_back_mode(spi_para.spi_id, spi_para.lb);
    
    hi_sleep(1000); /* 1000 */
    
    hi_spi_set_irq_mode(spi_para.spi_id, spi_para.irq);
    hi_spi_set_dma_mode(spi_para.spi_id, spi_para.dma_en);
    
    hi_sleep(1000); /* 1000 */

}

unsigned char spi_send_buf[128] = { 0 };
unsigned char spi_recv_buf[128] = { 0 };

//ret = hi_spi_host_read(0,&dst[0],1);
//ret = hi_spi_host_write(0,src,srclen);
void spi_write_data(hi_spi_idx spi_id,unsigned char data)
{
    unsigned char dat = data;
    hi_spi_host_write(spi_id,&dat,1);
}

