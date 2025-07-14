set SynModuleInfo {
  {SRCNAME kernel_main_Pipeline_VITIS_LOOP_10_1 MODELNAME kernel_main_Pipeline_VITIS_LOOP_10_1 RTLNAME kernel_main_kernel_main_Pipeline_VITIS_LOOP_10_1
    SUBMODULES {
      {MODELNAME kernel_main_flow_control_loop_pipe_sequential_init RTLNAME kernel_main_flow_control_loop_pipe_sequential_init BINDTYPE interface TYPE internal_upc_flow_control INSTNAME kernel_main_flow_control_loop_pipe_sequential_init_U}
    }
  }
  {SRCNAME kernel_main MODELNAME kernel_main RTLNAME kernel_main IS_TOP 1
    SUBMODULES {
      {MODELNAME kernel_main_gmem_m_axi RTLNAME kernel_main_gmem_m_axi BINDTYPE interface TYPE adapter IMPL m_axi}
      {MODELNAME kernel_main_control_s_axi RTLNAME kernel_main_control_s_axi BINDTYPE interface TYPE interface_s_axilite}
    }
  }
}
