#include "EMGFilters.h"

// ─── Coefficient tables (unchanged from OYMotion) ──────────────────────
static const float lpf_num[2][3] = {{0.3913f, 0.7827f, 0.3913f},
                                    {0.1311f, 0.2622f, 0.1311f}};
static const float lpf_den[2][3] = {{1.0000f, 0.3695f, 0.1958f},
                                    {1.0000f, -0.7478f, 0.2722f}};

static const float hpf_num[2][3] = {{0.8371f, -1.6742f, 0.8371f},
                                    {0.9150f, -1.8299f, 0.9150f}};
static const float hpf_den[2][3] = {{1.0000f, -1.6475f, 0.7009f},
                                    {1.0000f, -1.8227f, 0.8372f}};

static const float ahf_num_50[2][6] = {
    {0.9522f, -1.5407f, 0.9522f, 0.8158f, -0.8045f, 0.0855f},
    {0.5869f, -1.1146f, 0.5869f, 1.0499f, -2.0000f, 1.0499f}};
static const float ahf_den_50[2][6] = {
    {1.0000f, -1.5395f, 0.9056f, 1.0000f - 1.1187f, 0.3129f},
    {1.0000f, -1.8844f, 0.9893f, 1.0000f, -1.8991f, 0.9892f}};
static const float ahf_gain_50[2] = {1.3422f, 1.4399f};

static const float ahf_num_60[2][6] = {
    {0.9528f, -1.3891f, 0.9528f, 0.8272f, -0.7225f, 0.0264f},
    {0.5824f, -1.0810f, 0.5824f, 1.0736f, -2.0000f, 1.0736f}};
static const float ahf_den_60[2][6] = {
    {1.0000f, -1.3880f, 0.9066f, 1.0000f, -0.9739f, 0.2371f},
    {1.0000f, -1.8407f, 0.9894f, 1.0000f, -1.8584f, 0.9891f}};
static const float ahf_gain_60[2] = {1.3430f, 1.4206f};

// ─── 2nd-order filter (LPF / HPF) ──────────────────────────────────────
typedef struct {
    float states[2];
    float num[3];
    float den[3];
} Filter2nd;

typedef enum { FILTER_LOWPASS, FILTER_HIGHPASS } FilterType;

static void filter2nd_init(Filter2nd *f, FilterType ftype, SAMPLE_FREQUENCY sf) {
    f->states[0] = 0.0f;
    f->states[1] = 0.0f;
    int idx = (sf == SAMPLE_FREQ_1000HZ) ? 1 : 0;
    const float (*num)[3] = (ftype == FILTER_LOWPASS) ? &lpf_num[idx] : &hpf_num[idx];
    const float (*den)[3] = (ftype == FILTER_LOWPASS) ? &lpf_den[idx] : &hpf_den[idx];
    for (int i = 0; i < 3; i++) {
        f->num[i] = (*num)[i];
        f->den[i] = (*den)[i];
    }
}

static float filter2nd_update(Filter2nd *f, float input) {
    float tmp    = (input - f->den[1] * f->states[0] - f->den[2] * f->states[1]) / f->den[0];
    float output = f->num[0] * tmp + f->num[1] * f->states[0] + f->num[2] * f->states[1];
    f->states[1] = f->states[0];
    f->states[0] = tmp;
    return output;
}

// ─── 4th-order anti-hum filter (notch) ─────────────────────────────────
typedef struct {
    float states[4];
    float num[6];
    float den[6];
    float gain;
} Filter4th;

static void filter4th_init(Filter4th *f, SAMPLE_FREQUENCY sf, NOTCH_FREQUENCY nf) {
    f->gain = 0.0f;
    for (int i = 0; i < 4; i++) f->states[i] = 0.0f;
    int idx = (sf == SAMPLE_FREQ_1000HZ) ? 1 : 0;
    const float *num = (nf == NOTCH_FREQ_50HZ) ? ahf_num_50[idx] : ahf_num_60[idx];
    const float *den = (nf == NOTCH_FREQ_50HZ) ? ahf_den_50[idx] : ahf_den_60[idx];
    float gain       = (nf == NOTCH_FREQ_50HZ) ? ahf_gain_50[idx] : ahf_gain_60[idx];
    for (int i = 0; i < 6; i++) {
        f->num[i] = num[i];
        f->den[i] = den[i];
    }
    f->gain = gain;
}

static float filter4th_update(Filter4th *f, float input) {
    float stageIn, stageOut;
    stageOut    = f->num[0] * input + f->states[0];
    f->states[0] = (f->num[1] * input + f->states[1]) - f->den[1] * stageOut;
    f->states[1] = f->num[2] * input - f->den[2] * stageOut;
    stageIn     = stageOut;
    stageOut    = f->num[3] * stageOut + f->states[2];
    f->states[2] = (f->num[4] * stageIn + f->states[3]) - f->den[4] * stageOut;
    f->states[3] = f->num[5] * stageIn - f->den[5] * stageOut;
    return f->gain * stageOut;
}

// ─── Public API ────────────────────────────────────────────────────────
static Filter2nd LPF;
static Filter2nd HPF;
static Filter4th AHF;
static bool s_bypass;
static bool s_notch_en, s_lp_en, s_hp_en;

void EMGFilters_Init(SAMPLE_FREQUENCY sampleFreq,
                     NOTCH_FREQUENCY  notchFreq,
                     bool enableNotch,
                     bool enableLowpass,
                     bool enableHighpass) {
    s_bypass = true;
    if ((sampleFreq == SAMPLE_FREQ_500HZ || sampleFreq == SAMPLE_FREQ_1000HZ) &&
        (notchFreq == NOTCH_FREQ_50HZ || notchFreq == NOTCH_FREQ_60HZ)) {
        s_bypass = false;
    }
    filter2nd_init(&LPF, FILTER_LOWPASS,  sampleFreq);
    filter2nd_init(&HPF, FILTER_HIGHPASS, sampleFreq);
    filter4th_init(&AHF, sampleFreq, notchFreq);
    s_notch_en = enableNotch;
    s_lp_en    = enableLowpass;
    s_hp_en    = enableHighpass;
}

int32_t EMGFilters_Update(int32_t inputValue) {
    if (s_bypass) return inputValue;
    float v = (float)inputValue;
    if (s_notch_en) v = filter4th_update(&AHF, v);
    if (s_lp_en)    v = filter2nd_update(&LPF, v);
    if (s_hp_en)    v = filter2nd_update(&HPF, v);
    return (int32_t)v;
}
