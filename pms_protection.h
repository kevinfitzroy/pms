#ifndef PMS_PROTECTION_H
#define PMS_PROTECTION_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Protection actions suggested by the module
 */
typedef enum {
    PROT_ACTION_NORMAL = 0,       // No restrictions
    PROT_ACTION_REDUCED_POWER,    // Moderate power limitation
    PROT_ACTION_MIN_POWER,        // Minimal power allowed
    PROT_ACTION_SHUTDOWN          // Prohibit operation
} ProtAction_t;

/**
 * Single protection rule: if either soc <= soc_threshold
 * or voltage <= volt_threshold, suggest this action.
 * Rules should be ordered by descending severity (shutdown first).
 */
typedef struct {
    float soc_threshold;      // e.g. 5.0 for 5%% SOC
    float volt_threshold;     // e.g. 2.9 for 2.9V per cell
    ProtAction_t action;
} ProtRule_t;

/**
 * Configuration for protection module
 * alpha: filter coefficient [0,1] for exponential smoothing
 * rules: pointer to array of ProtRule_t
 * num_rules: number of entries in rules array
 */
typedef struct {
    float alpha;
    const ProtRule_t *rules;
    size_t num_rules;
} ProtConfig_t;

/**
 * Initialize the protection module with given configuration.
 * Must be called before any other function.
 */
void Prot_Init(const ProtConfig_t *cfg);

/**
 * Feed latest measurements to module. Values will be filtered.
 * soc: state of charge (0..100)
 * cell_voltage: minimum cell voltage in volts
 */
void Prot_UpdateInputs(float soc, float cell_voltage);

/**
 * Get latest suggested action based on filtered inputs and rules.
 */
ProtAction_t Prot_GetSuggestion(void);

#ifdef __cplusplus
}
#endif

#endif // PMS_PROTECTION_H