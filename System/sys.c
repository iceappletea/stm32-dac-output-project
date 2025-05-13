#include "sys.h"

void NVIC_Configuration(void)
{

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//设置NVIC中断分组2:2位抢占优先级，2位响应优先级

}

void WFI_SET(void)
{
    __ASM volatile("wfi");
}

void INTX_DISABLE(void)
{
    __ASM volatile("cpsid i");
}

void INTX_ENABLE(void)
{
    __ASM volatile("cpsie i");
}

__asm void MSR_MSP(uint32_t addr)
{
    MSR MSP, r0
    BX r14
}
