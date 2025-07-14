// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2025.1 (64-bit)
// Tool Version Limit: 2025.05
// Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// Copyright 2022-2025 Advanced Micro Devices, Inc. All Rights Reserved.
// 
// ==============================================================
#ifndef __linux__

#include "xstatus.h"
#ifdef SDT
#include "xparameters.h"
#endif
#include "xkernel_main.h"

extern XKernel_main_Config XKernel_main_ConfigTable[];

#ifdef SDT
XKernel_main_Config *XKernel_main_LookupConfig(UINTPTR BaseAddress) {
	XKernel_main_Config *ConfigPtr = NULL;

	int Index;

	for (Index = (u32)0x0; XKernel_main_ConfigTable[Index].Name != NULL; Index++) {
		if (!BaseAddress || XKernel_main_ConfigTable[Index].Control_BaseAddress == BaseAddress) {
			ConfigPtr = &XKernel_main_ConfigTable[Index];
			break;
		}
	}

	return ConfigPtr;
}

int XKernel_main_Initialize(XKernel_main *InstancePtr, UINTPTR BaseAddress) {
	XKernel_main_Config *ConfigPtr;

	Xil_AssertNonvoid(InstancePtr != NULL);

	ConfigPtr = XKernel_main_LookupConfig(BaseAddress);
	if (ConfigPtr == NULL) {
		InstancePtr->IsReady = 0;
		return (XST_DEVICE_NOT_FOUND);
	}

	return XKernel_main_CfgInitialize(InstancePtr, ConfigPtr);
}
#else
XKernel_main_Config *XKernel_main_LookupConfig(u16 DeviceId) {
	XKernel_main_Config *ConfigPtr = NULL;

	int Index;

	for (Index = 0; Index < XPAR_XKERNEL_MAIN_NUM_INSTANCES; Index++) {
		if (XKernel_main_ConfigTable[Index].DeviceId == DeviceId) {
			ConfigPtr = &XKernel_main_ConfigTable[Index];
			break;
		}
	}

	return ConfigPtr;
}

int XKernel_main_Initialize(XKernel_main *InstancePtr, u16 DeviceId) {
	XKernel_main_Config *ConfigPtr;

	Xil_AssertNonvoid(InstancePtr != NULL);

	ConfigPtr = XKernel_main_LookupConfig(DeviceId);
	if (ConfigPtr == NULL) {
		InstancePtr->IsReady = 0;
		return (XST_DEVICE_NOT_FOUND);
	}

	return XKernel_main_CfgInitialize(InstancePtr, ConfigPtr);
}
#endif

#endif

