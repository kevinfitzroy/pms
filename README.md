
# how to use pms_protection module
```c

#include "pms_protection.h"

// 定义保护规则：越早匹配，级别越重
static ProtRule_t prot_rules[] = {
    { 5.0f,  2.8f, PROT_ACTION_SHUTDOWN   },  // SoC≤5% 或 ≤2.8V：立刻关闭
    {10.0f,  3.0f, PROT_ACTION_MIN_POWER  },  // SoC≤10% 或 ≤3.0V：仅保留最小功率
    {20.0f,  3.2f, PROT_ACTION_REDUCED_POWER},// SoC≤20% 或 ≤3.2V：降低功率
    // 其余为 PROT_ACTION_NORMAL
};

void setup()
{
    ProtConfig_t cfg = {
        .alpha     = 0.1f, 
        .rules     = prot_rules, 
        .num_rules = sizeof(prot_rules)/sizeof(prot_rules[0])
    };
    Prot_Init(&cfg);
}

void voltage_protect_task(float soc, float min_cell_volt)
{
    // 每次采样后更新
    Prot_UpdateInputs(soc, min_cell_volt);
    // 获取建议，不直接执行
    ProtAction_t action = Prot_GetSuggestion();
    switch(action) {
        case PROT_ACTION_NORMAL:       /* 正常 */ break;
        case PROT_ACTION_REDUCED_POWER:/* 降功率 */ break;
        case PROT_ACTION_MIN_POWER:    /* 最小功率 */ break;
        case PROT_ACTION_SHUTDOWN:     /* 关闭 */ break;
    }
}

```