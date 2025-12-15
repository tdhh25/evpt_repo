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
 * @brief  计算当前滑动窗口内的统计值（最大值、最小值、平均值）
 * @param  sw 滑动窗口结构体指针
 * @param  max_val 输出最大值
 * @param  min_val 输出最小值
 * @param  avg_val 输出平均值
 * @retval 0-成功，-1-失败（无有效数据）
 */
int8_t slide_window_calc_stats(SlideWindow *sw, uint16_t *max_val, uint16_t *min_val, uint16_t *avg_val)///后面需要改为浮点型
{
    if (sw == NULL || max_val == NULL || min_val == NULL || avg_val == NULL)
    {
        return -1;
    }

    if (sw->current_count == 0)
    {
        return -1;
    }

    *avg_val = sw->sum / (float)sw->current_count;

    *max_val = sw->data[0];
    *min_val = sw->data[0];

    for (uint8_t i = 0; i < sw->current_count; i++)
    {
        uint8_t idx = (sw->tail_idx - sw->current_count + 1 + i) % sw->window_len;

        if (sw->data[idx] > *max_val)
        {
            *max_val = sw->data[idx];
        }

        if (sw->data[idx] < *min_val)
        {
            *min_val = sw->data[idx];
        }
    }

    return 0;
}

