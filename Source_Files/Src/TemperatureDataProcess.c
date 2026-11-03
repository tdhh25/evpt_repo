#include "TemperatureDataProcess.h"




/**
 * @brief  初始化滑动窗口
 * @param  sw 滑动窗口结构体指针
 * @param  window_len 窗口长度（建议≤WINDOW_SIZE）
 * @retval 0-成功，-1-失败
 */
int8_t slide_window_init(SlideWindow *sw, uint8_t window_len)
{
    if (sw == NULL || window_len == 0 || window_len > WINDOW_SIZE)
    {
        return -1;
    }

    memset(sw->data, 0, sizeof(sw->data));
    sw->window_len = window_len;
    sw->current_count = 0;
    sw->tail_idx = 0;
    sw->sum = 0.0f;

    return 0;
}

/**
 * @brief  向滑动窗口添加新数据（核心滑动逻辑）
 * @param  sw 滑动窗口结构体指针
 * @param  new_data 新加入的单个数据
 * @retval 0-成功，-1-失败
 */
int8_t slide_window_add_data(SlideWindow *sw, float new_data)
{
    if (sw == NULL)
    {
        return -1;
    }

    if (sw->current_count >= sw->window_len)
    {
        uint8_t old_data_idx = (sw->tail_idx + 1) % sw->window_len;
        sw->sum -= sw->data[old_data_idx];
    }
    else
    {
        sw->current_count++;
    }

    sw->tail_idx = (sw->tail_idx + 1) % sw->window_len;
    sw->data[sw->tail_idx] = new_data;
    sw->sum += new_data;

    return 0;
}

/**
 * @brief  将浮点统计值钳制到 int16_t 可表示范围，负值按补码原样保留
 * @param  valValue_Lo 待转换的浮点值（定点：摄氏度 x 100）
 * @retval 钳制后的 int16_t 值（-32768 ~ 32767）
 */
static int16_t Tdp_ClampToInt16_Function(float valValue_Lo)
{
    int16_t valRet_Lo = 0;

    if (valValue_Lo <= -32768.0F)
    {
        valRet_Lo = -32768;
    }
    else if (valValue_Lo >= 32767.0F)
    {
        valRet_Lo = 32767;
    }
    else
    {
        valRet_Lo = (int16_t)valValue_Lo;
    }

    return valRet_Lo;
}

/**
 * @brief  将浮点值钳制到 uint16_t 可表示范围
 * @param  valValue_Lo 待转换的浮点值（波动幅度，非负）
 * @retval 钳制后的 uint16_t 值（0 ~ 65535）
 */
static uint16_t Tdp_ClampToUint16_Function(float valValue_Lo)
{
    uint16_t valRet_Lo = 0U;

    if (valValue_Lo <= 0.0F)
    {
        valRet_Lo = 0U;
    }
    else if (valValue_Lo >= 65535.0F)
    {
        valRet_Lo = 65535U;
    }
    else
    {
        valRet_Lo = (uint16_t)valValue_Lo;
    }

    return valRet_Lo;
}

/**
 * @brief  计算当前滑动窗口的统计值，并一次性写入显示数据结构的 current/max/min/average/wave
 * @param  sw 滑动窗口结构体指针
 * @param  valChannelData_Lo 输出：通道显示数据（定点摄氏度 x100）
 * @retval 0-成功，-1-失败（无有效数据）
 */
int8_t slide_window_calc_stats(SlideWindow *sw, struct dwin_value *valChannelData_Lo)
{
    float   valAvg_Lo = 0.0F;
    float   valMax_Lo = 0.0F;
    float   valMin_Lo = 0.0F;
    float   valWave_Lo = 0.0F;
    uint8_t valIdx_Lo = 0U;
    uint8_t numIdx_Lo = 0U;

    if ((sw == NULL) || (valChannelData_Lo == NULL))
    {
        return -1;
    }

    if (sw->current_count == 0U)
    {
        return -1;
    }

    valAvg_Lo = sw->sum / (float)sw->current_count;

    /* 以窗口内最老的元素作为 min/max 初值，避免使用未写入的 data[0] */
    valIdx_Lo = (uint8_t)((sw->tail_idx + sw->window_len - sw->current_count + 1) % sw->window_len);
    valMax_Lo = sw->data[valIdx_Lo];
    valMin_Lo = sw->data[valIdx_Lo];

    for (numIdx_Lo = 0U; numIdx_Lo < sw->current_count; numIdx_Lo++)
    {
        valIdx_Lo = (uint8_t)((sw->tail_idx + sw->window_len - sw->current_count + 1 + numIdx_Lo) % sw->window_len);

        if (sw->data[valIdx_Lo] > valMax_Lo)
        {
            valMax_Lo = sw->data[valIdx_Lo];
        }

        if (sw->data[valIdx_Lo] < valMin_Lo)
        {
            valMin_Lo = sw->data[valIdx_Lo];
        }
    }

    /* wave = 窗口内波动幅度 = 最大值 - 最小值 */
    valWave_Lo = valMax_Lo - valMin_Lo;

    valChannelData_Lo->current = Tdp_ClampToInt16_Function(sw->data[sw->tail_idx]);
    valChannelData_Lo->max     = Tdp_ClampToInt16_Function(valMax_Lo);
    valChannelData_Lo->min     = Tdp_ClampToInt16_Function(valMin_Lo);
    valChannelData_Lo->average = Tdp_ClampToInt16_Function(valAvg_Lo);
    valChannelData_Lo->wave    = Tdp_ClampToUint16_Function(valWave_Lo);

    return 0;
}

