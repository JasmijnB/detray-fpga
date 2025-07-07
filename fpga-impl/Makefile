#######################################################################################
.PHONY: help
help:
	@echo "Makefile Usage:"
	@echo "  make all"
	@echo "      Command to run all target csim & csynth in turn."
	@echo ""
	@echo "  make csim"
	@echo "  make csynth"
	@echo "      Command to run only one step of Vitis HLS. Note csim forces clean first."
	@echo ""
	@echo "  make clean"
	@echo "      Command to remove the generated files."
	@echo ""
	@echo "  make help"
	@echo "      Print this help."
	@echo ""
#######################################################################################

SHELL := bash

PLATFORM := xilinx_u50_gen3x16_xdma_5_202210_1

CONFIG_DIR := ./config
HLS_CONFIG := ${CONFIG_DIR}/hls.cfg
LINK_CONFIG := ${CONFIG_DIR}/link.cfg
DETRAY_INSTALL := $(HOME)/.local

WORK_DIR := ./build
SRC_DIR := ./src

SYNTH_OUTPUT := ${WORK_DIR}/kernel_main.xo

COMMON_OPTIONS :=  --config ${HLS_CONFIG} --work_dir ${WORK_DIR}

KERNEL_XCLBIN := kernel_main.xclbin
HW_EMU_XCLBIN := ${WORK_DIR}/hw_emu/${KERNEL_XCLBIN}

all: csim csynth ;#cosim

csynth ${SYNTH_OUTPUT}: ${SRC_DIR}/kernel.cpp ${SRC_DIR}/shared.h
	v++ --compile  --mode hls --platform=${PLATFORM} ${COMMON_OPTIONS}

csim: clean # clean before csim !
csim cosim: 
	vitis-run --$@ ${COMMON_OPTIONS}

link_hw_emu ${HW_EMU_XCLBIN}: ${SYNTH_OUTPUT}
	v++ -l -t hw_emu --config ${LINK_CONFIG} ${SYNTH_OUTPUT} -o ${HW_EMU_XCLBIN}

# ===[ Host ]===	
CXX := g++
CXXFLAGS := -std=c++20 -g -Wall -O1 

HOST_FILE := ${SRC_DIR}/host.cpp
HOST_OUTPUT := ${WORK_DIR}/host

XILINX_XRT := $(XILINX_XRT)

INCLUDES := -I$(XILINX_XRT)/include -I${SRC_DIR} -I${DETRAY_INSTALL}/include
LDFLAGS := -L$(XILINX_XRT)/lib  -L${DETRAY_INSTALL}/lib64
LDLIBS := -lxrt_coreutil -lOpenCL -lvecmem_core -lvecmem_hip

SRCS := $(HOST_FILE) 
OBJS := $(SRCS:.cpp=.o)

host: $(HOST_OUTPUT)

$(HOST_OUTPUT): $(OBJS)
		$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^ $(LDFLAGS) $(LDLIBS)

%.o: %.cpp
		$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# ===[ Other helpers]===
.PHONY: run_hw_emu
run_hw_emu: host
	pushd ${WORK_DIR} && XCL_EMULATION_MODE=hw_emu ./host ${HW_EMU_XCLBIN}&& popd

.PHONY: clean
clean:
	rm -rf ${WORK_DIR} *.log _x .Xil
