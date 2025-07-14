#!/bin/sh

# 
# v++(TM)
# runme.sh: a v++-generated Runs Script for UNIX
# Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
# Copyright 2022-2025 Advanced Micro Devices, Inc. All Rights Reserved.
# 

if [ -z "$PATH" ]; then
  PATH=/home/xilinx/2025.1/Vitis/bin:/home/xilinx/2025.1/Vitis/bin:/home/xilinx/2025.1/Vitis/bin
else
  PATH=/home/xilinx/2025.1/Vitis/bin:/home/xilinx/2025.1/Vitis/bin:/home/xilinx/2025.1/Vitis/bin:$PATH
fi
export PATH

if [ -z "$LD_LIBRARY_PATH" ]; then
  LD_LIBRARY_PATH=
else
  LD_LIBRARY_PATH=:$LD_LIBRARY_PATH
fi
export LD_LIBRARY_PATH

HD_PWD='/home/jbookelm/fpga_detray/fpga-impl/_x/kernel/kernel_main'
cd "$HD_PWD"

HD_LOG=runme.log
/bin/touch $HD_LOG

ISEStep="./ISEWrap.sh"
EAStep()
{
     $ISEStep $HD_LOG "$@" >> $HD_LOG 2>&1
     if [ $? -ne 0 ]
     then
         exit
     fi
}

# pre-commands:
export RDI_DEPENDENCY=VITIS_HLS_SETUP


EAStep loader -exec vitis_hls -f kernel_main.tcl -messageDb vitis_hls.pb
