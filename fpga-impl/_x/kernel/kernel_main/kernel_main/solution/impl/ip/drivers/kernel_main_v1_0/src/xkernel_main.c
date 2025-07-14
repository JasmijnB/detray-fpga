// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2025.1 (64-bit)
// Tool Version Limit: 2025.05
// Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// Copyright 2022-2025 Advanced Micro Devices, Inc. All Rights Reserved.
// 
// ==============================================================
/***************************** Include Files *********************************/
#include "xkernel_main.h"

/************************** Function Implementation *************************/
#ifndef __linux__
int XKernel_main_CfgInitialize(XKernel_main *InstancePtr, XKernel_main_Config *ConfigPtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(ConfigPtr != NULL);

    InstancePtr->Control_BaseAddress = ConfigPtr->Control_BaseAddress;
    InstancePtr->IsReady = XIL_COMPONENT_IS_READY;

    return XST_SUCCESS;
}
#endif

void XKernel_main_Start(XKernel_main *InstancePtr) {
    u32 Data;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XKernel_main_ReadReg(InstancePtr->Control_BaseAddress, XKERNEL_MAIN_CONTROL_ADDR_AP_CTRL) & 0x80;
    XKernel_main_WriteReg(InstancePtr->Control_BaseAddress, XKERNEL_MAIN_CONTROL_ADDR_AP_CTRL, Data | 0x01);
}

u32 XKernel_main_IsDone(XKernel_main *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XKernel_main_ReadReg(InstancePtr->Control_BaseAddress, XKERNEL_MAIN_CONTROL_ADDR_AP_CTRL);
    return (Data >> 1) & 0x1;
}

u32 XKernel_main_IsIdle(XKernel_main *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XKernel_main_ReadReg(InstancePtr->Control_BaseAddress, XKERNEL_MAIN_CONTROL_ADDR_AP_CTRL);
    return (Data >> 2) & 0x1;
}

u32 XKernel_main_IsReady(XKernel_main *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XKernel_main_ReadReg(InstancePtr->Control_BaseAddress, XKERNEL_MAIN_CONTROL_ADDR_AP_CTRL);
    // check ap_start to see if the pcore is ready for next input
    return !(Data & 0x1);
}

void XKernel_main_Continue(XKernel_main *InstancePtr) {
    u32 Data;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XKernel_main_ReadReg(InstancePtr->Control_BaseAddress, XKERNEL_MAIN_CONTROL_ADDR_AP_CTRL) & 0x80;
    XKernel_main_WriteReg(InstancePtr->Control_BaseAddress, XKERNEL_MAIN_CONTROL_ADDR_AP_CTRL, Data | 0x10);
}

void XKernel_main_EnableAutoRestart(XKernel_main *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XKernel_main_WriteReg(InstancePtr->Control_BaseAddress, XKERNEL_MAIN_CONTROL_ADDR_AP_CTRL, 0x80);
}

void XKernel_main_DisableAutoRestart(XKernel_main *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XKernel_main_WriteReg(InstancePtr->Control_BaseAddress, XKERNEL_MAIN_CONTROL_ADDR_AP_CTRL, 0);
}

void XKernel_main_Set_in_r(XKernel_main *InstancePtr, u64 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XKernel_main_WriteReg(InstancePtr->Control_BaseAddress, XKERNEL_MAIN_CONTROL_ADDR_IN_R_DATA, (u32)(Data));
    XKernel_main_WriteReg(InstancePtr->Control_BaseAddress, XKERNEL_MAIN_CONTROL_ADDR_IN_R_DATA + 4, (u32)(Data >> 32));
}

u64 XKernel_main_Get_in_r(XKernel_main *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XKernel_main_ReadReg(InstancePtr->Control_BaseAddress, XKERNEL_MAIN_CONTROL_ADDR_IN_R_DATA);
    Data += (u64)XKernel_main_ReadReg(InstancePtr->Control_BaseAddress, XKERNEL_MAIN_CONTROL_ADDR_IN_R_DATA + 4) << 32;
    return Data;
}

void XKernel_main_Set_out_r(XKernel_main *InstancePtr, u64 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XKernel_main_WriteReg(InstancePtr->Control_BaseAddress, XKERNEL_MAIN_CONTROL_ADDR_OUT_R_DATA, (u32)(Data));
    XKernel_main_WriteReg(InstancePtr->Control_BaseAddress, XKERNEL_MAIN_CONTROL_ADDR_OUT_R_DATA + 4, (u32)(Data >> 32));
}

u64 XKernel_main_Get_out_r(XKernel_main *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XKernel_main_ReadReg(InstancePtr->Control_BaseAddress, XKERNEL_MAIN_CONTROL_ADDR_OUT_R_DATA);
    Data += (u64)XKernel_main_ReadReg(InstancePtr->Control_BaseAddress, XKERNEL_MAIN_CONTROL_ADDR_OUT_R_DATA + 4) << 32;
    return Data;
}

void XKernel_main_Set_size(XKernel_main *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XKernel_main_WriteReg(InstancePtr->Control_BaseAddress, XKERNEL_MAIN_CONTROL_ADDR_SIZE_DATA, Data);
}

u32 XKernel_main_Get_size(XKernel_main *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XKernel_main_ReadReg(InstancePtr->Control_BaseAddress, XKERNEL_MAIN_CONTROL_ADDR_SIZE_DATA);
    return Data;
}

void XKernel_main_Set_b_ptr(XKernel_main *InstancePtr, u64 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XKernel_main_WriteReg(InstancePtr->Control_BaseAddress, XKERNEL_MAIN_CONTROL_ADDR_B_PTR_DATA, (u32)(Data));
    XKernel_main_WriteReg(InstancePtr->Control_BaseAddress, XKERNEL_MAIN_CONTROL_ADDR_B_PTR_DATA + 4, (u32)(Data >> 32));
}

u64 XKernel_main_Get_b_ptr(XKernel_main *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XKernel_main_ReadReg(InstancePtr->Control_BaseAddress, XKERNEL_MAIN_CONTROL_ADDR_B_PTR_DATA);
    Data += (u64)XKernel_main_ReadReg(InstancePtr->Control_BaseAddress, XKERNEL_MAIN_CONTROL_ADDR_B_PTR_DATA + 4) << 32;
    return Data;
}

void XKernel_main_InterruptGlobalEnable(XKernel_main *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XKernel_main_WriteReg(InstancePtr->Control_BaseAddress, XKERNEL_MAIN_CONTROL_ADDR_GIE, 1);
}

void XKernel_main_InterruptGlobalDisable(XKernel_main *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XKernel_main_WriteReg(InstancePtr->Control_BaseAddress, XKERNEL_MAIN_CONTROL_ADDR_GIE, 0);
}

void XKernel_main_InterruptEnable(XKernel_main *InstancePtr, u32 Mask) {
    u32 Register;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Register =  XKernel_main_ReadReg(InstancePtr->Control_BaseAddress, XKERNEL_MAIN_CONTROL_ADDR_IER);
    XKernel_main_WriteReg(InstancePtr->Control_BaseAddress, XKERNEL_MAIN_CONTROL_ADDR_IER, Register | Mask);
}

void XKernel_main_InterruptDisable(XKernel_main *InstancePtr, u32 Mask) {
    u32 Register;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Register =  XKernel_main_ReadReg(InstancePtr->Control_BaseAddress, XKERNEL_MAIN_CONTROL_ADDR_IER);
    XKernel_main_WriteReg(InstancePtr->Control_BaseAddress, XKERNEL_MAIN_CONTROL_ADDR_IER, Register & (~Mask));
}

void XKernel_main_InterruptClear(XKernel_main *InstancePtr, u32 Mask) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XKernel_main_WriteReg(InstancePtr->Control_BaseAddress, XKERNEL_MAIN_CONTROL_ADDR_ISR, Mask);
}

u32 XKernel_main_InterruptGetEnabled(XKernel_main *InstancePtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    return XKernel_main_ReadReg(InstancePtr->Control_BaseAddress, XKERNEL_MAIN_CONTROL_ADDR_IER);
}

u32 XKernel_main_InterruptGetStatus(XKernel_main *InstancePtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    return XKernel_main_ReadReg(InstancePtr->Control_BaseAddress, XKERNEL_MAIN_CONTROL_ADDR_ISR);
}

