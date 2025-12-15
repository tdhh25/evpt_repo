#ifndef _TEMPERATUREDATAPROCESS_H_
#define _TEMPERATUREDATAPROCESS_H_


#include <stdio.h>
#include <stdint.h>
#include <string.h>

// 滑动窗口配置（可根据需求修改）
#define WINDOW_SIZE 10  // 滑动窗口长度（示例为10，与之前需求一致）

// 滑动窗口结构体
typedef struct
{
    float data[WINDOW_SIZE];  // 存储窗口数据的环形缓冲区
    uint8_t window_len;       // 窗口实际长度（配置值，如10）
    uint8_t current_count;    // 当前窗口内的有效数据个数（≤window_len）
    uint8_t tail_idx;         // 环形缓冲区尾索引（最新数据位置）
    float sum;                // 窗口内数据总和（实时更新，避免重复求和）
} SlideWindow;
extern int8_t slide_window_init(SlideWindow *sw, uint8_t window_len);
extern int8_t slide_window_add_data(SlideWindow *sw, float new_data);
extern int8_t slide_window_calc_stats(SlideWindow *sw, uint16_t *max_val, uint16_t *min_val, uint16_t *avg_val);
#endif