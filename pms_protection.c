#include "pms_protection.h"

static ProtConfig_t g_config;
static float g_filt_soc;
static float g_filt_volt_mV;
static int g_initialized = 0;

void Prot_Init(const ProtConfig_t *cfg)
{
    if (!cfg || cfg->num_rules == 0 || cfg->alpha < 0.0f || cfg->alpha > 1.0f) {
        return; // invalid config
    }
    g_config = *cfg;
    g_filt_soc      = 0.0f;
    g_filt_volt_mV  = 0.0f;
    g_initialized   = 1;
}

void Prot_UpdateInputs(float soc, float cell_voltage_mV)
{
    if (!g_initialized) return;
    // exponential smoothing for SOC and voltage (in mV)
    g_filt_soc     = g_config.alpha * soc + (1.0f - g_config.alpha) * g_filt_soc;
    g_filt_volt_mV = g_config.alpha * cell_voltage_mV + (1.0f - g_config.alpha) * g_filt_volt_mV;
}

ProtAction_t Prot_GetSuggestion(void)
{
    if (!g_initialized) return PROT_ACTION_SHUTDOWN;
    // iterate rules by severity
    for (size_t i = 0; i < g_config.num_rules; ++i) {
        const ProtRule_t *r = &g_config.rules[i];
        if (g_filt_soc <= r->soc_threshold || g_filt_volt_mV <= r->volt_threshold_mV) {
            return r->action;
        }
    }
    return PROT_ACTION_NORMAL;
}
