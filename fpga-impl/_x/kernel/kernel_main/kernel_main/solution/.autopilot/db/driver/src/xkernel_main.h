// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2025.1 (64-bit)
// Tool Version Limit: 2025.05
// Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// Copyright 2022-2025 Advanced Micro Devices, Inc. All Rights Reserved.
// 
// ==============================================================
#ifndef XKERNEL_MAIN_H
#define XKERNEL_MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/***************************** Include Files *********************************/
#ifndef __linux__
#include "xil_types.h"
#include "xil_assert.h"
#include "xstatus.h"
#include "xil_io.h"
#else
#include <stdint.h>
#include <assert.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stddef.h>
#endif
#include "xkernel_main_hw.h"

/**************************** Type Definitions ******************************/
#ifdef __linux__
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
#else
typedef struct {
#ifdef SDT
    char *Name;
#else
    u16 DeviceId;
#endif
    u64 Control_BaseAddress;
} XKernel_main_Config;
#endif

typedef struct {
    u64 Control_BaseAddress;
    u32 IsReady;
} XKernel_main;

typedef u32 word_type;

/***************** Macros (Inline Functions) Definitions *********************/
#ifndef __linux__
#define XKernel_main_WriteReg(BaseAddress, RegOffset, Data) \
    Xil_Out32((BaseAddress) + (RegOffset), (u32)(Data))
#define XKernel_main_ReadReg(BaseAddress, RegOffset) \
    Xil_In32((BaseAddress) + (RegOffset))
#else
#define XKernel_main_WriteReg(BaseAddress, RegOffset, Data) \
    *(volatile u32*)((BaseAddress) + (RegOffset)) = (u32)(Data)
#define XKernel_main_ReadReg(BaseAddress, RegOffset) \
    *(volatile u32*)((BaseAddress) + (RegOffset))

#define Xil_AssertVoid(expr)    assert(expr)
#define Xil_AssertNonvoid(expr) assert(expr)

#define XST_SUCCESS             0
#define XST_DEVICE_NOT_FOUND    2
#define XST_OPEN_DEVICE_FAILED  3
#define XIL_COMPONENT_IS_READY  1
#endif

/************************** Function Prototypes *****************************/
#ifndef __linux__
#ifdef SDT
int XKernel_main_Initialize(XKernel_main *InstancePtr, UINTPTR BaseAddress);
XKernel_main_Config* XKernel_main_LookupConfig(UINTPTR BaseAddress);
#else
int XKernel_main_Initialize(XKernel_main *InstancePtr, u16 DeviceId);
XKernel_main_Config* XKernel_main_LookupConfig(u16 DeviceId);
#endif
int XKernel_main_CfgInitialize(XKernel_main *InstancePtr, XKernel_main_Config *ConfigPtr);
#else
int XKernel_main_Initialize(XKernel_main *InstancePtr, const char* InstanceName);
int XKernel_main_Release(XKernel_main *InstancePtr);
#endif

void XKernel_main_Start(XKernel_main *InstancePtr);
u32 XKernel_main_IsDone(XKernel_main *InstancePtr);
u32 XKernel_main_IsIdle(XKernel_main *InstancePtr);
u32 XKernel_main_IsReady(XKernel_main *InstancePtr);
void XKernel_main_Continue(XKernel_main *InstancePtr);
void XKernel_main_EnableAutoRestart(XKernel_main *InstancePtr);
void XKernel_main_DisableAutoRestart(XKernel_main *InstancePtr);

void XKernel_main_Set_in_r(XKernel_main *InstancePtr, u64 Data);
u64 XKernel_main_Get_in_r(XKernel_main *InstancePtr);
void XKernel_main_Set_out_r(XKernel_main *InstancePtr, u64 Data);
u64 XKernel_main_Get_out_r(XKernel_main *InstancePtr);
void XKernel_main_Set_size(XKernel_main *InstancePtr, u32 Data);
u32 XKernel_main_Get_size(XKernel_main *InstancePtr);
void XKernel_main_Set_b_ptr(XKernel_main *InstancePtr, u64 Data);
u64 XKernel_main_Get_b_ptr(XKernel_main *InstancePtr);

void XKernel_main_InterruptGlobalEnable(XKernel_main *InstancePtr);
void XKernel_main_InterruptGlobalDisable(XKernel_main *InstancePtr);
void XKernel_main_InterruptEnable(XKernel_main *InstancePtr, u32 Mask);
void XKernel_main_InterruptDisable(XKernel_main *InstancePtr, u32 Mask);
void XKernel_main_InterruptClear(XKernel_main *InstancePtr, u32 Mask);
u32 XKernel_main_InterruptGetEnabled(XKernel_main *InstancePtr);
u32 XKernel_main_InterruptGetStatus(XKernel_main *InstancePtr);

#ifdef __cplusplus
}
#endif

#endif
