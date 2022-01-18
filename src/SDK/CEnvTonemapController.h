#pragma once
#include "../offsets.h"
#include "common.h"

class CEnvTonemapController {
public:
	int* getUseExposureMin() {
		return (int*)((uintptr_t)this + offsets.DT_EnvTonemapController.m_bUseCustomAutoExposureMin);
	}
	int* getUseExposureMax() {
		return (int*)((uintptr_t)this + offsets.DT_EnvTonemapController.m_bUseCustomAutoExposureMax);
	}
	// int* getUseCustomBloomScale() {
	// 	return (int*)((uintptr_t)this + offsets.DT_EnvTonemapController.m_bUseCustomBloomScale);
	// }
	// int* getCustomBloomScale() {
	// 	return (int*)((uintptr_t)this + offsets.DT_EnvTonemapController.m_flCustomBloomScale);
	// }
	float* getExposureMin() {
		return (float*)((uintptr_t)this + offsets.DT_EnvTonemapController.m_flCustomAutoExposureMin);
	}
	float* getExposureMax() {
		return (float*)((uintptr_t)this + offsets.DT_EnvTonemapController.m_flCustomAutoExposureMax);
	}
};
