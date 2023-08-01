#include <stdio.h>

#include <hi_types_base.h>
#include <hi3861_platform.h>
#include <hi_task.h>
#include <hi_stdlib.h>
#include <hi_early_debug.h>
#include <hi_time.h>
#include <hi_watchdog.h>
#include <hi_io.h>
#include <hi_spi.h>

#define test_spi_printf(fmt...)     \
    do {                            \
        printf("[SPI TEST]" fmt); \
        printf("\n");     \
    } while (0)

typedef struct {
    hi_spi_cfg_basic_info cfg_info;
    hi_spi_idx spi_id;
    hi_u32 loop;
    hi_u32 length;
    hi_bool irq;
    hi_bool slave;
    hi_bool lb;
    hi_bool dma_en;
} test_spi_para;

typedef enum {
    TEST_CASE_ALL,
    TEST_CASE_POL0_PHA0 = 1,
    TEST_CASE_POL0_PHA1,
    TEST_CASE_POL1_PHA0,
    TEST_CASE_POL1_PHA1,
    TEST_CASE_MOTOROLA,
    TEST_CASE_TI,
    TEST_CASE_BIT4,
    TEST_CASE_BIT7,
    TEST_CASE_BIT8,
    TEST_CASE_BIT9 = 10,
    TEST_CASE_BIT15,
    TEST_CASE_BIT16,
    TEST_CASE_CLK_MIN,
    TEST_CASE_CLK_16,
    TEST_CASE_CLK_50,
    TEST_CASE_CLK_100,
    TEST_CASE_CLK_200,
    TEST_CASE_CLK_MAX,
    TEST_CASE_PARAMETER_WRONG,
    TEST_CASE_SLAVE = 20,
    TEST_CASE_MASTER,
    TEST_CASE_MAX,
} hi_spi_test_case;

hi_void screen_spi_master_init(hi_spi_idx spi_id);
void spi_write_data(hi_spi_idx spi_id,unsigned char data);

