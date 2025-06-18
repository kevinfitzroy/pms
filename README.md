
# how to use pms_protection module
```c

#include "pms_protection.h"

// 规则按严重度从高到低排列
static ProtRule_t prot_rules[] = {
    {  5.0f, 2800.0f, PROT_ACTION_SHUTDOWN   },  // SOC≤5% 或 电压≤2800 mV → 关闭
    { 10.0f, 3000.0f, PROT_ACTION_MIN_POWER  },  // SOC≤10% 或 电压≤3000 mV → 最小功率
    { 20.0f, 3200.0f, PROT_ACTION_REDUCED_POWER},// SOC≤20% 或 电压≤3200 mV → 降功率
    // 剩余情况为 PROT_ACTION_NORMAL
};

void setup_protection()
{
    ProtConfig_t cfg = {
        .alpha     = 0.1f,
        .rules     = prot_rules,
        .num_rules = sizeof(prot_rules)/sizeof(prot_rules[0])
    };
    Prot_Init(&cfg);
}

void voltage_protect_task(float soc_pct, float min_cell_voltage_mV)
{
     // 每次采样后更新
    Prot_UpdateInputs(soc_pct, min_cell_voltage_mV);
    ProtAction_t action = Prot_GetSuggestion();
    // 由调用方根据 action  决定具体执行
    switch(action) {
        case PROT_ACTION_NORMAL:       /* 正常 */ break;
        case PROT_ACTION_REDUCED_POWER:/* 降功率 */ break;
        case PROT_ACTION_MIN_POWER:    /* 最小功率 */ break;
        case PROT_ACTION_SHUTDOWN:     /* 关闭 */ break;
    }
}

```