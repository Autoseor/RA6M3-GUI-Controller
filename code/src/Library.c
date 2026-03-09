/* Library.c */

#include <Library.h>

/* GLOBAL VARIABLE AREA */
volatile uint8_t state_mask = 0x01;

void Initial_Setting()
{
    AGT_Setting();
    GPT_Setting();
    FND_Reset();
    LED_Reset();
    IRQ_Setting();
    IRQ_Enable();
    DAC_Setting();
}
void AGT_Setting()
{
    R_AGT_Open(&g_timer1_ctrl, &g_timer1_cfg);
    R_AGT_Start(&g_timer1_ctrl);
}
void GPT_Setting()
{
    R_MSTP->MSTPCRD_b.MSTPD5 = 0U;

    R_GPT3->GTCR_b.MD = 0U;
    R_GPT3->GTCR_b.TPCS = 0U;

    R_GPT3->GTCNT = 0;

    R_GPT3->GTIOR_b.GTIOA = 9U;
    R_GPT3->GTIOR_b.OAE = 1U;

    R_GPT3->GTCR_b.CST = 1U;
}
void FND_Reset()
{
    uint8_t idx = 0;

    /* 7-Segment Initialization */
    for (idx = 0; idx < SEGMENT_INDEX; idx++)
    {
        if (idx < DIGIT_INDEX)
            R_IOPORT_PinWrite(&g_ioport_ctrl, segment_digit[idx], BSP_IO_LEVEL_LOW);
        R_IOPORT_PinWrite(&g_ioport_ctrl, segment_led_pin[idx], BSP_IO_LEVEL_HIGH);
    }
}

void LED_Reset()
{
    uint8_t idx = 0;

    /* LED Initialization */
    for (idx = 0; idx < LED_INDEX; idx++)
        R_IOPORT_PinWrite(&g_ioport_ctrl, led_pin[idx], BSP_IO_LEVEL_HIGH);
}
void IRQ_Setting()
{
    // Interrupt ICU Setting
    R_ICU_ExternalIrqOpen(&g_external_irq11_ctrl, &g_external_irq11_cfg);
    R_ICU_ExternalIrqOpen(&g_external_irq12_ctrl, &g_external_irq12_cfg);
    R_ICU_ExternalIrqOpen(&g_external_irq13_ctrl, &g_external_irq13_cfg);
    R_ICU_ExternalIrqOpen(&g_external_irq14_ctrl, &g_external_irq14_cfg);
}
void IRQ_Enable()
{
    R_ICU_ExternalIrqEnable(&g_external_irq11_ctrl);
    R_ICU_ExternalIrqEnable(&g_external_irq12_ctrl);
    R_ICU_ExternalIrqEnable(&g_external_irq13_ctrl);
    R_ICU_ExternalIrqEnable(&g_external_irq14_ctrl);
}
void IRQ_Disable()
{
    // Interrupt Set Disable Register Setting (Disable)
    R_ICU_ExternalIrqDisable(&g_external_irq11_ctrl);
    R_ICU_ExternalIrqDisable(&g_external_irq12_ctrl);
    R_ICU_ExternalIrqDisable(&g_external_irq13_ctrl);
    R_ICU_ExternalIrqDisable(&g_external_irq14_ctrl);
}
void DAC_Setting()
{
    R_DAC_Open(&g_dac0_ctrl, &g_dac0_cfg);
    R_DAC_Start(&g_dac0_ctrl);
}
void ADC0_Setting()
{
    R_ADC_Open(&g_adc0_ctrl, &g_adc0_cfg);
    R_ADC_ScanCfg(&g_adc0_ctrl, &g_adc0_channel_cfg);
}
void ADC1_Setting()
{
    R_ADC_Open(&g_adc1_ctrl, &g_adc1_cfg);
    R_ADC_ScanCfg(&g_adc1_ctrl, &g_adc1_channel_cfg);
}
void ADC0_Read_and_Convert()
{
    adc_status_t status;
    R_ADC_ScanStart(&g_adc0_ctrl);
    status.state = ADC_STATE_SCAN_IN_PROGRESS;
    while(ADC_STATE_SCAN_IN_PROGRESS == status.state)
        R_ADC_StatusGet(&g_adc0_ctrl, &status);
}
void ADC1_Read_and_Convert()
{
    adc_status_t status;
    R_ADC_ScanStart(&g_adc1_ctrl);
    status.state = ADC_STATE_SCAN_IN_PROGRESS;
    while(ADC_STATE_SCAN_IN_PROGRESS == status.state)
        R_ADC_StatusGet(&g_adc1_ctrl, &status);
}
void FND_Print_Data(uint8_t *string)
{
    uint8_t idx = 0;

    if (sizeof(string) != DIGIT_INDEX)
        return;

    for (idx = 0; idx < DIGIT_INDEX; idx++)
        FND_Display_Data(idx, string[idx]);
}
void FND_Display_Data(uint8_t digit, uint8_t data)
{
    uint8_t key = 0, idx = 0;

    R_BSP_SoftwareDelay(10, BSP_DELAY_UNITS_MICROSECONDS);
    FND_Reset();

    /* 7-Segment Digit Selection */
    R_IOPORT_PinWrite(&g_ioport_ctrl, segment_digit[digit], BSP_IO_LEVEL_HIGH);

    /* 7-Segment LED Pin Setting */
    for(idx = 0; idx < SEGMENT_INDEX; idx++)
    {
        key = (uint8_t)(data >> (7 - idx) & 0x01);
        R_IOPORT_PinWrite(&g_ioport_ctrl, segment_led_pin[idx], key);
    }
}
