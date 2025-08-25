#include "PT100_Process.h"


float PT100_CalibrationTemperature_Function(float pt100_resistance)
{
    const float R0 = 100.0f; //uint:Ω
    const float A = 3.9083e-3f;  
    const float B = -5.775e-7f; 
    const float C = -4.183e-12f;

    // 电阻值范围校验（合理范围：18.52Ω ~ 390.48Ω）
    if (pt100_resistance < 18.52 || pt100_resistance > 390.48)
    {
        return Invalid_ERROR; 
    }

    float Rt_R0 = pt100_resistance / R0;  // 归一化电阻值
    float temperature;
    /************************** * 0°C ~ 850°C：Rt/R0 = 1 + A*t + B*t²**************************************/
    if (pt100_resistance >= R0)
    {
        float discriminant = A * A - 4 * B * (1 - Rt_R0);

        if (discriminant < 0)
        {
            return Invalid_ERROR;
        }
        temperature = (-A + sqrtf(discriminant)) / (2 * B);
    }
    /*********************  -200°C ~ 0°C：Rt/R0 = 1 + A*t + B*t² + C*(t-100)*t³*************************/
    else
    {
        // 迭代求解三元一次方程
        float t = 0.0f;  // 初始值设为0°C
        const int max_iter = 20;    // 最大迭代次数
        const float eps = 1e-5f;    // 精度阈值
        for (int i = 0; i < max_iter; i++)
        {
            // 计算当前温度下的电阻理论值
            float t_sq = t * t;
            float t_cu = t_sq * t;
            float f = 1 + A * t + B * t_sq + C * (t - 100) * t_cu - Rt_R0;
            float df = A + 2 * B * t + C * (4 * t_cu - 300 * t_sq);
            float t_new = t - f / df;
            if (fabsf(t_new - t) < eps)
            {
                t = t_new;
                break;
            }

            t = t_new;
        }

        temperature = t;
    }
    if (temperature < -200.0 || temperature > 850.0)
    {
        return Invalid_ERROR;
    }

    return temperature;
}