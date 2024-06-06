// number of overlays (w/o fusion): 6 
// number of overlays (w/ fusion): 6 
#include <sys/time.h> 
#include <algorithm> 
#include <cstring> 
#include <fstream> 
#include <iomanip> 
#include <iostream> 
#include <sstream> 
#include <climits> 
#include <unordered_map> 
const int PU_NM = 8; 

#include "table_dt.hpp" 
#include "utils.hpp" 
#include "tpch_read_2.hpp" 
#include "gqe_api.hpp" 

#include "cfgFunc_q18.hpp" 
#include "q18.hpp" 

int main(int argc, const char* argv[]) { 
    std::cout << "\n------------ TPC-H Query Test -------------\n"; 
    ArgParser parser(argc, argv); 
    std::string xclbin_path; 
    if (!parser.getCmdOption("-xclbin", xclbin_path)) { 
        std::cout << "ERROR: xclbin path is not set!\n"; 
        return 1; 
    } 
    std::string xclbin_path_h; 
    if (!parser.getCmdOption("-xclbin_h", xclbin_path_h)) { 
        std::cout << "ERROR: xclbin_h path is not set!\n"; 
        return 1; 
    } 
    std::string xclbin_path_a; 
    if (!parser.getCmdOption("-xclbin_a", xclbin_path_a)) { 
        std::cout << "ERROR: xclbin_a path is not set!\n"; 
        return 1; 
    } 
    std::string in_dir; 
    if (!parser.getCmdOption("-in", in_dir) || !is_dir(in_dir)) { 
        std::cout << "ERROR: input dir is not specified or not valid.\n"; 
        return 1; 
    } 
    int num_rep = 1; 
    std::string num_str; 
    if (parser.getCmdOption("-rep", num_str)) { 
        try { 
            num_rep = std::stoi(num_str); 
        } catch (...) { 
            num_rep = 1; 
        } 
    } 
    if (num_rep > 20) { 
        num_rep = 20; 
        std::cout << "WARNING: limited repeat to " << num_rep << " times\n."; 
    } 
    int scale = 1; 
    std::string scale_str; 
    if (parser.getCmdOption("-c", scale_str)) { 
        try { 
            scale = std::stoi(scale_str); 
        } catch (...) { 
            scale = 1; 
        } 
    } 
    std::cout << "NOTE:running query #18\n."; 
    std::cout << "NOTE:running in sf" << scale << " data\n"; 

    int32_t lineitem_n = SF1_LINEITEM; 
    int32_t supplier_n = SF1_SUPPLIER; 
    int32_t nation_n = SF1_NATION; 
    int32_t order_n = SF1_ORDERS; 
    int32_t customer_n = SF1_CUSTOMER; 
    int32_t region_n = SF1_REGION; 
    int32_t part_n = SF1_PART; 
    int32_t partsupp_n = SF1_PARTSUPP; 
    if (scale == 30) { 
        lineitem_n = SF30_LINEITEM; 
        supplier_n = SF30_SUPPLIER; 
        nation_n = SF30_NATION; 
        order_n = SF30_ORDERS; 
        customer_n = SF30_CUSTOMER; 
        region_n = SF30_REGION; 
        part_n = SF30_PART; 
        partsupp_n = SF30_PARTSUPP; 
    } 
    // ********************************************************** // 
    // Get CL devices. 
    std::vector<cl::Device> devices = xcl::get_xil_devices(); 
    cl::Device device_h = devices[0]; 
    // Create context_h and command queue for selected device 
    cl::Context context_h(device_h); 
    cl::CommandQueue q_h(context_h, device_h, CL_QUEUE_PROFILING_ENABLE | CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE); 
    std::string devName_h = device_h.getInfo<CL_DEVICE_NAME>(); 
    std::cout << "Selected Device " << devName_h << "\n"; 
    cl::Program::Binaries xclBins_h = xcl::import_binary_file(xclbin_path_h); 
    std::vector<cl::Device> devices_h; 
    devices_h.push_back(device_h); 
    cl::Program program_h(context_h, devices_h, xclBins_h); 
    cl::Device device_a = devices[1]; 
    // Create context_a and command queue for selected device 
    cl::Context context_a(device_a); 
    cl::CommandQueue q_a(context_a, device_a, CL_QUEUE_PROFILING_ENABLE | CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE); 
    std::string devName_a = device_a.getInfo<CL_DEVICE_NAME>(); 
    std::cout << "Selected Device " << devName_a << "\n"; 
    cl::Program::Binaries xclBins_a = xcl::import_binary_file(xclbin_path_a); 
    std::vector<cl::Device> devices_a; 
    devices_a.push_back(device_a); 
    cl::Program program_a(context_a, devices_a, xclBins_a); 
    // ****************************** Tables **************************** // 
    Table tbl_Sort_TD_0664_output("tbl_Sort_TD_0664_output", 6100000, 6, "");
    tbl_Sort_TD_0664_output.allocateHost();
    Table tbl_Aggregate_TD_1467_output("tbl_Aggregate_TD_1467_output", 6100000, 6, "");
    tbl_Aggregate_TD_1467_output.allocateHost();
    Table tbl_JOIN_INNER_TD_2987_output("tbl_JOIN_INNER_TD_2987_output", 100, 4, "");
    tbl_JOIN_INNER_TD_2987_output.selectOverlayVersion(1);
    tbl_JOIN_INNER_TD_2987_output.allocateHost();
    Table tbl_JOIN_INNER_TD_2987_consolidated_output("tbl_JOIN_INNER_TD_2987_consolidated_output", 100, 6, "");
    tbl_JOIN_INNER_TD_2987_consolidated_output.selectOverlayVersion(1);
    tbl_JOIN_INNER_TD_2987_consolidated_output.allocateHost();
    Table tbl_JOIN_INNER_TD_3555_output("tbl_JOIN_INNER_TD_3555_output", 100, 4, "");
    tbl_JOIN_INNER_TD_3555_output.selectOverlayVersion(1);
    tbl_JOIN_INNER_TD_3555_output.allocateHost();
    Table tbl_JOIN_INNER_TD_3555_consolidated_output("tbl_JOIN_INNER_TD_3555_consolidated_output", 100, 5, "");
    tbl_JOIN_INNER_TD_3555_consolidated_output.selectOverlayVersion(1);
    tbl_JOIN_INNER_TD_3555_consolidated_output.allocateHost();
    Table tbl_JOIN_LEFTSEMI_TD_3312_output("tbl_JOIN_LEFTSEMI_TD_3312_output", 100, 4, "");
    tbl_JOIN_LEFTSEMI_TD_3312_output.selectOverlayVersion(1);
    tbl_JOIN_LEFTSEMI_TD_3312_output.allocateHost();
    Table tbl_JOIN_LEFTSEMI_TD_3312_consolidated_output("tbl_JOIN_LEFTSEMI_TD_3312_consolidated_output", 100, 2, "");
    tbl_JOIN_LEFTSEMI_TD_3312_consolidated_output.selectOverlayVersion(1);
    tbl_JOIN_LEFTSEMI_TD_3312_consolidated_output.allocateHost();
    Table tbl_SerializeFromObject_TD_4795_input_stringRowIDSubstitute;
    tbl_SerializeFromObject_TD_4795_input_stringRowIDSubstitute = Table("customer", customer_n, 2, in_dir);
    tbl_SerializeFromObject_TD_4795_input_stringRowIDSubstitute.addCol("c_custkey", 4);
    tbl_SerializeFromObject_TD_4795_input_stringRowIDSubstitute.addCol("c_name", 4, 1, 0);
    tbl_SerializeFromObject_TD_4795_input_stringRowIDSubstitute.allocateHost();
    tbl_SerializeFromObject_TD_4795_input_stringRowIDSubstitute.loadHost();
    Table tbl_SerializeFromObject_TD_4795_input;
    tbl_SerializeFromObject_TD_4795_input = Table("customer", customer_n, 2, in_dir);
    tbl_SerializeFromObject_TD_4795_input.addCol("c_custkey", 4);
    tbl_SerializeFromObject_TD_4795_input.addCol("c_name", TPCH_READ_C_NAME_LEN+1);
    tbl_SerializeFromObject_TD_4795_input.allocateHost();
    tbl_SerializeFromObject_TD_4795_input.loadHost();
    Table tbl_JOIN_LEFTSEMI_TD_4107_output("tbl_JOIN_LEFTSEMI_TD_4107_output", 100, 4, "");
    tbl_JOIN_LEFTSEMI_TD_4107_output.selectOverlayVersion(1);
    tbl_JOIN_LEFTSEMI_TD_4107_output.allocateHost();
    Table tbl_JOIN_LEFTSEMI_TD_4107_consolidated_output("tbl_JOIN_LEFTSEMI_TD_4107_consolidated_output", 100, 4, "");
    tbl_JOIN_LEFTSEMI_TD_4107_consolidated_output.selectOverlayVersion(1);
    tbl_JOIN_LEFTSEMI_TD_4107_consolidated_output.allocateHost();
    Table tbl_SerializeFromObject_TD_4842_input;
    tbl_SerializeFromObject_TD_4842_input = Table("lineitem", lineitem_n, 2, in_dir);
    tbl_SerializeFromObject_TD_4842_input.selectOverlayVersion(1);
    tbl_SerializeFromObject_TD_4842_input.addCol("l_orderkey", 4);
    tbl_SerializeFromObject_TD_4842_input.addCol("l_quantity", 4);
    tbl_SerializeFromObject_TD_4842_input.allocateHost();
    tbl_SerializeFromObject_TD_4842_input.loadHost();
    Table tbl_Filter_TD_4818_output("tbl_Filter_TD_4818_output", 6100000, 1, "");
    tbl_Filter_TD_4818_output.selectOverlayVersion(1);
    tbl_Filter_TD_4818_output.allocateHost();
    Table tbl_SerializeFromObject_TD_5404_input;
    tbl_SerializeFromObject_TD_5404_input = Table("order", order_n, 4, in_dir);
    tbl_SerializeFromObject_TD_5404_input.selectOverlayVersion(1);
    tbl_SerializeFromObject_TD_5404_input.addCol("o_orderkey", 4);
    tbl_SerializeFromObject_TD_5404_input.addCol("o_custkey", 4);
    tbl_SerializeFromObject_TD_5404_input.addCol("o_totalprice", 4);
    tbl_SerializeFromObject_TD_5404_input.addCol("o_orderdate", 4);
    tbl_SerializeFromObject_TD_5404_input.allocateHost();
    tbl_SerializeFromObject_TD_5404_input.loadHost();
    Table tbl_Filter_TD_5902_output("tbl_Filter_TD_5902_output", 6100000, 1, "");
    tbl_Filter_TD_5902_output.selectOverlayVersion(1);
    tbl_Filter_TD_5902_output.allocateHost();
    Table tbl_Aggregate_TD_617_output_preprocess("tbl_Aggregate_TD_617_output_preprocess", 3000000, 16, "");
    tbl_Aggregate_TD_617_output_preprocess.selectOverlayVersion(0);
    tbl_Aggregate_TD_617_output_preprocess.allocateHost();
    Table tbl_Aggregate_TD_617_output("tbl_Aggregate_TD_617_output", 3000000, 2, "");
    tbl_Aggregate_TD_617_output.allocateHost();
    Table tbl_SerializeFromObject_TD_7328_input;
    tbl_SerializeFromObject_TD_7328_input = Table("lineitem", lineitem_n, 2, in_dir);
    tbl_SerializeFromObject_TD_7328_input.addCol("l_orderkey", 4);
    tbl_SerializeFromObject_TD_7328_input.addCol("l_quantity", 4);
    tbl_SerializeFromObject_TD_7328_input.allocateHost();
    tbl_SerializeFromObject_TD_7328_input.loadHost();
    // ********************** Allocate Device Buffer ******************** // 
    //tbl_JOIN_INNER_TD_2987_output.allocateDevBuffer(context_h, 32);
    //tbl_JOIN_INNER_TD_3555_output.allocateDevBuffer(context_h, 32);
    //tbl_JOIN_LEFTSEMI_TD_3312_output.allocateDevBuffer(context_h, 32);
    tbl_SerializeFromObject_TD_4795_input_stringRowIDSubstitute.allocateDevBuffer(context_h, 32);
    //tbl_JOIN_LEFTSEMI_TD_4107_output.allocateDevBuffer(context_h, 32);
    // tbl_SerializeFromObject_TD_4842_input.allocateDevBuffer(context_h, 32);
    //tbl_Filter_TD_4818_output.allocateDevBuffer(context_h, 32);
    // tbl_SerializeFromObject_TD_5404_input.allocateDevBuffer(context_h, 32);
    //tbl_Filter_TD_5902_output.allocateDevBuffer(context_h, 32);
    tbl_Aggregate_TD_617_output_preprocess.allocateDevBuffer(context_a, 33);
    tbl_SerializeFromObject_TD_7328_input.allocateDevBuffer(context_a, 32);
    // ****************************** Config **************************** // 
    
    xf::database::gqe::KernelCommand krn_cmd_build_683 = xf::database::gqe::KernelCommand();
    xf::database::gqe::KernelCommand krn_cmd_probe_683 = xf::database::gqe::KernelCommand();
    cfgCmd cfg_JOIN_INNER_TD_2987_cmds_build;
    cfg_JOIN_INNER_TD_2987_cmds_build.setup(1);
    cfg_JOIN_INNER_TD_2987_cmds_build.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_2987_gqe_join_build(krn_cmd_build_683);
    cfg_JOIN_INNER_TD_2987_cmds_build.cmd = krn_cmd_build_683.getConfigBits();
    get_meta_info_JOIN_INNER_TD_2987_gqe_join_build(cfg_JOIN_INNER_TD_2987_cmds_build.meta_in, tbl_JOIN_INNER_TD_3555_consolidated_output.nrow, 3);
    get_meta_info_JOIN_INNER_TD_2987_gqe_join_build(cfg_JOIN_INNER_TD_2987_cmds_build.meta_out, tbl_JOIN_INNER_TD_2987_output.nrow, 4);
    // cfg_JOIN_INNER_TD_2987_cmds_build.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_2987_cmds_probe;
    cfg_JOIN_INNER_TD_2987_cmds_probe.setup(1);
    cfg_JOIN_INNER_TD_2987_cmds_probe.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_2987_gqe_join_probe(krn_cmd_probe_683);
    cfg_JOIN_INNER_TD_2987_cmds_probe.cmd = krn_cmd_probe_683.getConfigBits();
    get_meta_info_JOIN_INNER_TD_2987_gqe_join_probe(cfg_JOIN_INNER_TD_2987_cmds_probe.meta_in, tbl_JOIN_LEFTSEMI_TD_3312_consolidated_output.nrow, 3);
    get_meta_info_JOIN_INNER_TD_2987_gqe_join_probe(cfg_JOIN_INNER_TD_2987_cmds_probe.meta_out, tbl_JOIN_INNER_TD_2987_output.nrow, 3);
    // cfg_JOIN_INNER_TD_2987_cmds_probe.allocateDevBuffer(context_h, 32);
    cfg_JOIN_INNER_TD_2987_cmds_build.input_key_columns = {2, -1, -1};
    cfg_JOIN_INNER_TD_2987_cmds_probe.input_key_columns = {0, -1, -1};
    cfg_JOIN_INNER_TD_2987_cmds_build.output_key_columns = {0, 1, 2, 3};
    cfg_JOIN_INNER_TD_2987_cmds_probe.output_key_columns = {0, 1, 2, 3};
    
    xf::database::gqe::KernelCommand krn_cmd_build_182 = xf::database::gqe::KernelCommand();
    xf::database::gqe::KernelCommand krn_cmd_probe_182 = xf::database::gqe::KernelCommand();
    cfgCmd cfg_JOIN_INNER_TD_3555_cmds_build;
    cfg_JOIN_INNER_TD_3555_cmds_build.setup(1);
    cfg_JOIN_INNER_TD_3555_cmds_build.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_3555_gqe_join_build(krn_cmd_build_182);
    cfg_JOIN_INNER_TD_3555_cmds_build.cmd = krn_cmd_build_182.getConfigBits();
    get_meta_info_JOIN_INNER_TD_3555_gqe_join_build(cfg_JOIN_INNER_TD_3555_cmds_build.meta_in, tbl_SerializeFromObject_TD_4795_input_stringRowIDSubstitute.nrow, 3);
    get_meta_info_JOIN_INNER_TD_3555_gqe_join_build(cfg_JOIN_INNER_TD_3555_cmds_build.meta_out, tbl_JOIN_INNER_TD_3555_output.nrow, 4);
    // cfg_JOIN_INNER_TD_3555_cmds_build.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_3555_cmds_probe;
    cfg_JOIN_INNER_TD_3555_cmds_probe.setup(1);
    cfg_JOIN_INNER_TD_3555_cmds_probe.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_3555_gqe_join_probe(krn_cmd_probe_182);
    cfg_JOIN_INNER_TD_3555_cmds_probe.cmd = krn_cmd_probe_182.getConfigBits();
    get_meta_info_JOIN_INNER_TD_3555_gqe_join_probe(cfg_JOIN_INNER_TD_3555_cmds_probe.meta_in, tbl_JOIN_LEFTSEMI_TD_4107_consolidated_output.nrow, 3);
    get_meta_info_JOIN_INNER_TD_3555_gqe_join_probe(cfg_JOIN_INNER_TD_3555_cmds_probe.meta_out, tbl_JOIN_INNER_TD_3555_output.nrow, 3);
    // cfg_JOIN_INNER_TD_3555_cmds_probe.allocateDevBuffer(context_h, 32);
    cfg_JOIN_INNER_TD_3555_cmds_build.input_key_columns = {0, -1, -1};
    cfg_JOIN_INNER_TD_3555_cmds_probe.input_key_columns = {1, -1, -1};
    cfg_JOIN_INNER_TD_3555_cmds_build.output_key_columns = {0, 1, 2, 3};
    cfg_JOIN_INNER_TD_3555_cmds_probe.output_key_columns = {0, 1, 2, 3};
    
    xf::database::gqe::KernelCommand krn_cmd_build_415 = xf::database::gqe::KernelCommand();
    xf::database::gqe::KernelCommand krn_cmd_probe_415 = xf::database::gqe::KernelCommand();
    cfgCmd cfg_JOIN_LEFTSEMI_TD_3312_cmds_build;
    cfg_JOIN_LEFTSEMI_TD_3312_cmds_build.setup(1);
    cfg_JOIN_LEFTSEMI_TD_3312_cmds_build.allocateHost();
    get_cfg_dat_JOIN_LEFTSEMI_TD_3312_gqe_join_build(krn_cmd_build_415);
    cfg_JOIN_LEFTSEMI_TD_3312_cmds_build.cmd = krn_cmd_build_415.getConfigBits();
    get_meta_info_JOIN_LEFTSEMI_TD_3312_gqe_join_build(cfg_JOIN_LEFTSEMI_TD_3312_cmds_build.meta_in, tbl_Filter_TD_4818_output.nrow, 3);
    get_meta_info_JOIN_LEFTSEMI_TD_3312_gqe_join_build(cfg_JOIN_LEFTSEMI_TD_3312_cmds_build.meta_out, tbl_JOIN_LEFTSEMI_TD_3312_output.nrow, 4);
    // cfg_JOIN_LEFTSEMI_TD_3312_cmds_build.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_LEFTSEMI_TD_3312_cmds_probe;
    cfg_JOIN_LEFTSEMI_TD_3312_cmds_probe.setup(1);
    cfg_JOIN_LEFTSEMI_TD_3312_cmds_probe.allocateHost();
    get_cfg_dat_JOIN_LEFTSEMI_TD_3312_gqe_join_probe(krn_cmd_probe_415);
    cfg_JOIN_LEFTSEMI_TD_3312_cmds_probe.cmd = krn_cmd_probe_415.getConfigBits();
    get_meta_info_JOIN_LEFTSEMI_TD_3312_gqe_join_probe(cfg_JOIN_LEFTSEMI_TD_3312_cmds_probe.meta_in, tbl_SerializeFromObject_TD_4842_input.nrow, 3);
    get_meta_info_JOIN_LEFTSEMI_TD_3312_gqe_join_probe(cfg_JOIN_LEFTSEMI_TD_3312_cmds_probe.meta_out, tbl_JOIN_LEFTSEMI_TD_3312_output.nrow, 3);
    // cfg_JOIN_LEFTSEMI_TD_3312_cmds_probe.allocateDevBuffer(context_h, 32);
    cfg_JOIN_LEFTSEMI_TD_3312_cmds_build.input_key_columns = {0, -1, -1};
    cfg_JOIN_LEFTSEMI_TD_3312_cmds_probe.input_key_columns = {0, -1, -1};
    cfg_JOIN_LEFTSEMI_TD_3312_cmds_build.output_key_columns = {0, 1, 2, 3};
    cfg_JOIN_LEFTSEMI_TD_3312_cmds_probe.output_key_columns = {0, 1, 2, 3};
    
    xf::database::gqe::KernelCommand krn_cmd_build_623 = xf::database::gqe::KernelCommand();
    xf::database::gqe::KernelCommand krn_cmd_probe_623 = xf::database::gqe::KernelCommand();
    cfgCmd cfg_JOIN_LEFTSEMI_TD_4107_cmds_build;
    cfg_JOIN_LEFTSEMI_TD_4107_cmds_build.setup(1);
    cfg_JOIN_LEFTSEMI_TD_4107_cmds_build.allocateHost();
    get_cfg_dat_JOIN_LEFTSEMI_TD_4107_gqe_join_build(krn_cmd_build_623);
    cfg_JOIN_LEFTSEMI_TD_4107_cmds_build.cmd = krn_cmd_build_623.getConfigBits();
    get_meta_info_JOIN_LEFTSEMI_TD_4107_gqe_join_build(cfg_JOIN_LEFTSEMI_TD_4107_cmds_build.meta_in, tbl_Filter_TD_5902_output.nrow, 3);
    get_meta_info_JOIN_LEFTSEMI_TD_4107_gqe_join_build(cfg_JOIN_LEFTSEMI_TD_4107_cmds_build.meta_out, tbl_JOIN_LEFTSEMI_TD_4107_output.nrow, 4);
    // cfg_JOIN_LEFTSEMI_TD_4107_cmds_build.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_LEFTSEMI_TD_4107_cmds_probe;
    cfg_JOIN_LEFTSEMI_TD_4107_cmds_probe.setup(1);
    cfg_JOIN_LEFTSEMI_TD_4107_cmds_probe.allocateHost();
    get_cfg_dat_JOIN_LEFTSEMI_TD_4107_gqe_join_probe(krn_cmd_probe_623);
    cfg_JOIN_LEFTSEMI_TD_4107_cmds_probe.cmd = krn_cmd_probe_623.getConfigBits();
    get_meta_info_JOIN_LEFTSEMI_TD_4107_gqe_join_probe(cfg_JOIN_LEFTSEMI_TD_4107_cmds_probe.meta_in, tbl_SerializeFromObject_TD_5404_input.nrow, 3);
    get_meta_info_JOIN_LEFTSEMI_TD_4107_gqe_join_probe(cfg_JOIN_LEFTSEMI_TD_4107_cmds_probe.meta_out, tbl_JOIN_LEFTSEMI_TD_4107_output.nrow, 3);
    // cfg_JOIN_LEFTSEMI_TD_4107_cmds_probe.allocateDevBuffer(context_h, 32);
    cfg_JOIN_LEFTSEMI_TD_4107_cmds_build.input_key_columns = {0, -1, -1};
    cfg_JOIN_LEFTSEMI_TD_4107_cmds_probe.input_key_columns = {0, -1, -1};
    cfg_JOIN_LEFTSEMI_TD_4107_cmds_build.output_key_columns = {0, 1, 2, 3};
    cfg_JOIN_LEFTSEMI_TD_4107_cmds_probe.output_key_columns = {0, 1, 2, 3};
    AggrCfgCmd cfg_Aggregate_TD_617_cmds;
    cfg_Aggregate_TD_617_cmds.allocateHost();
    get_cfg_dat_Aggregate_TD_617_gqe_aggr(cfg_Aggregate_TD_617_cmds.cmd);
    cfg_Aggregate_TD_617_cmds.allocateDevBuffer(context_a, 32);
    AggrCfgCmd cfg_Aggregate_TD_617_cmds_out;
    cfg_Aggregate_TD_617_cmds_out.allocateHost();
    cfg_Aggregate_TD_617_cmds_out.allocateDevBuffer(context_a, 33);
    // *************************** Kernel Setup ************************* // 
    bufferTmp buftmp_h(context_h, 1); 
    buftmp_h.initBuffer(q_h); 
    AggrBufferTmp buftmp_a(context_a, 1); 
    buftmp_a.BufferInitial(q_a); 
    std::cout << std::endl; 
    krnlEngine krnl_JOIN_INNER_TD_2987_build;
    krnl_JOIN_INNER_TD_2987_build = krnlEngine(program_h, q_h, context_h, "gqeKernel", 0);
    krnl_JOIN_INNER_TD_2987_build.setup(tbl_JOIN_INNER_TD_3555_consolidated_output, tbl_JOIN_LEFTSEMI_TD_3312_consolidated_output, tbl_JOIN_INNER_TD_2987_output, cfg_JOIN_INNER_TD_2987_cmds_build, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_2987_probe;
    krnl_JOIN_INNER_TD_2987_probe = krnlEngine(program_h, q_h, context_h, "gqeKernel", 1);
    krnl_JOIN_INNER_TD_2987_probe.setup(tbl_JOIN_LEFTSEMI_TD_3312_consolidated_output, tbl_JOIN_INNER_TD_3555_consolidated_output, tbl_JOIN_INNER_TD_2987_output, cfg_JOIN_INNER_TD_2987_cmds_probe, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_3555_build;
    krnl_JOIN_INNER_TD_3555_build = krnlEngine(program_h, q_h, context_h, "gqeKernel", 0);
    krnl_JOIN_INNER_TD_3555_build.setup(tbl_SerializeFromObject_TD_4795_input_stringRowIDSubstitute, tbl_JOIN_LEFTSEMI_TD_4107_consolidated_output, tbl_JOIN_INNER_TD_3555_output, cfg_JOIN_INNER_TD_3555_cmds_build, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_3555_probe;
    krnl_JOIN_INNER_TD_3555_probe = krnlEngine(program_h, q_h, context_h, "gqeKernel", 1);
    krnl_JOIN_INNER_TD_3555_probe.setup(tbl_JOIN_LEFTSEMI_TD_4107_consolidated_output, tbl_SerializeFromObject_TD_4795_input_stringRowIDSubstitute, tbl_JOIN_INNER_TD_3555_output, cfg_JOIN_INNER_TD_3555_cmds_probe, buftmp_h);
    krnlEngine krnl_JOIN_LEFTSEMI_TD_3312_build;
    krnl_JOIN_LEFTSEMI_TD_3312_build = krnlEngine(program_h, q_h, context_h, "gqeKernel", 0);
    krnl_JOIN_LEFTSEMI_TD_3312_build.setup(tbl_Filter_TD_4818_output, tbl_SerializeFromObject_TD_4842_input, tbl_JOIN_LEFTSEMI_TD_3312_output, cfg_JOIN_LEFTSEMI_TD_3312_cmds_build, buftmp_h);
    krnlEngine krnl_JOIN_LEFTSEMI_TD_3312_probe;
    krnl_JOIN_LEFTSEMI_TD_3312_probe = krnlEngine(program_h, q_h, context_h, "gqeKernel", 1);
    krnl_JOIN_LEFTSEMI_TD_3312_probe.setup(tbl_SerializeFromObject_TD_4842_input, tbl_Filter_TD_4818_output, tbl_JOIN_LEFTSEMI_TD_3312_output, cfg_JOIN_LEFTSEMI_TD_3312_cmds_probe, buftmp_h);
    krnlEngine krnl_JOIN_LEFTSEMI_TD_4107_build;
    krnl_JOIN_LEFTSEMI_TD_4107_build = krnlEngine(program_h, q_h, context_h, "gqeKernel", 0);
    krnl_JOIN_LEFTSEMI_TD_4107_build.setup(tbl_Filter_TD_5902_output, tbl_SerializeFromObject_TD_5404_input, tbl_JOIN_LEFTSEMI_TD_4107_output, cfg_JOIN_LEFTSEMI_TD_4107_cmds_build, buftmp_h);
    krnlEngine krnl_JOIN_LEFTSEMI_TD_4107_probe;
    krnl_JOIN_LEFTSEMI_TD_4107_probe = krnlEngine(program_h, q_h, context_h, "gqeKernel", 1);
    krnl_JOIN_LEFTSEMI_TD_4107_probe.setup(tbl_SerializeFromObject_TD_5404_input, tbl_Filter_TD_5902_output, tbl_JOIN_LEFTSEMI_TD_4107_output, cfg_JOIN_LEFTSEMI_TD_4107_cmds_probe, buftmp_h);
    AggrKrnlEngine krnl_Aggregate_TD_617;
    krnl_Aggregate_TD_617 = AggrKrnlEngine(program_a, q_a, context_a, "gqeAggr");
    krnl_Aggregate_TD_617.setup(tbl_SerializeFromObject_TD_7328_input, tbl_Aggregate_TD_617_output_preprocess, cfg_Aggregate_TD_617_cmds, cfg_Aggregate_TD_617_cmds_out, buftmp_a);
    // ************************** Transfer Engine *********************** // 
    transEngine trans_JOIN_INNER_TD_2987;
    trans_JOIN_INNER_TD_2987.setq(q_h,1);
    trans_JOIN_INNER_TD_2987.add(&(cfg_JOIN_INNER_TD_2987_cmds_build));
    trans_JOIN_INNER_TD_2987.add(&(cfg_JOIN_INNER_TD_2987_cmds_probe));
    transEngine trans_JOIN_INNER_TD_2987_out;
    trans_JOIN_INNER_TD_2987_out.setq(q_h,1);
    q_h.finish();
    transEngine trans_JOIN_INNER_TD_3555;
    trans_JOIN_INNER_TD_3555.setq(q_h,1);
    trans_JOIN_INNER_TD_3555.add(&(cfg_JOIN_INNER_TD_3555_cmds_build));
    trans_JOIN_INNER_TD_3555.add(&(cfg_JOIN_INNER_TD_3555_cmds_probe));
    trans_JOIN_INNER_TD_3555.add(&(tbl_SerializeFromObject_TD_4795_input_stringRowIDSubstitute));
    transEngine trans_JOIN_INNER_TD_3555_out;
    trans_JOIN_INNER_TD_3555_out.setq(q_h,1);
    q_h.finish();
    transEngine trans_JOIN_LEFTSEMI_TD_3312;
    trans_JOIN_LEFTSEMI_TD_3312.setq(q_h,1);
    trans_JOIN_LEFTSEMI_TD_3312.add(&(cfg_JOIN_LEFTSEMI_TD_3312_cmds_build));
    trans_JOIN_LEFTSEMI_TD_3312.add(&(cfg_JOIN_LEFTSEMI_TD_3312_cmds_probe));
    trans_JOIN_LEFTSEMI_TD_3312.add(&(tbl_SerializeFromObject_TD_4842_input));
    transEngine trans_JOIN_LEFTSEMI_TD_3312_out;
    trans_JOIN_LEFTSEMI_TD_3312_out.setq(q_h,1);
    q_h.finish();
    transEngine trans_JOIN_LEFTSEMI_TD_4107;
    trans_JOIN_LEFTSEMI_TD_4107.setq(q_h,1);
    trans_JOIN_LEFTSEMI_TD_4107.add(&(cfg_JOIN_LEFTSEMI_TD_4107_cmds_build));
    trans_JOIN_LEFTSEMI_TD_4107.add(&(cfg_JOIN_LEFTSEMI_TD_4107_cmds_probe));
    trans_JOIN_LEFTSEMI_TD_4107.add(&(tbl_SerializeFromObject_TD_5404_input));
    transEngine trans_JOIN_LEFTSEMI_TD_4107_out;
    trans_JOIN_LEFTSEMI_TD_4107_out.setq(q_h,1);
    q_h.finish();
    transEngine trans_Aggregate_TD_617;
    trans_Aggregate_TD_617.setq(q_a, 0);
    trans_Aggregate_TD_617.add(&(cfg_Aggregate_TD_617_cmds));
    trans_Aggregate_TD_617.add(&(cfg_Aggregate_TD_617_cmds_out));
    trans_Aggregate_TD_617.add(&(tbl_SerializeFromObject_TD_7328_input));
    transEngine trans_Aggregate_TD_617_out;
    trans_Aggregate_TD_617_out.setq(q_a, 0);
    q_a.finish();
    // ****************************** Events **************************** // 
    std::vector<cl::Event> events_h2d_wr_JOIN_INNER_TD_2987;
    std::vector<cl::Event> events_d2h_rd_JOIN_INNER_TD_2987;
    std::vector<cl::Event> events_JOIN_INNER_TD_2987;
    events_h2d_wr_JOIN_INNER_TD_2987.resize(1);
    events_d2h_rd_JOIN_INNER_TD_2987.resize(1);
    events_JOIN_INNER_TD_2987.resize(2);
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_2987;
    std::vector<cl::Event> prev_events_grp_JOIN_INNER_TD_2987;
    std::vector<cl::Event> events_h2d_wr_JOIN_INNER_TD_3555;
    std::vector<cl::Event> events_d2h_rd_JOIN_INNER_TD_3555;
    std::vector<cl::Event> events_JOIN_INNER_TD_3555;
    events_h2d_wr_JOIN_INNER_TD_3555.resize(1);
    events_d2h_rd_JOIN_INNER_TD_3555.resize(1);
    events_JOIN_INNER_TD_3555.resize(2);
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_3555;
    std::vector<cl::Event> prev_events_grp_JOIN_INNER_TD_3555;
    std::vector<cl::Event> events_h2d_wr_JOIN_LEFTSEMI_TD_3312;
    std::vector<cl::Event> events_d2h_rd_JOIN_LEFTSEMI_TD_3312;
    std::vector<cl::Event> events_JOIN_LEFTSEMI_TD_3312;
    events_h2d_wr_JOIN_LEFTSEMI_TD_3312.resize(1);
    events_d2h_rd_JOIN_LEFTSEMI_TD_3312.resize(1);
    events_JOIN_LEFTSEMI_TD_3312.resize(2);
    std::vector<cl::Event> events_grp_JOIN_LEFTSEMI_TD_3312;
    std::vector<cl::Event> prev_events_grp_JOIN_LEFTSEMI_TD_3312;
    std::vector<cl::Event> events_h2d_wr_JOIN_LEFTSEMI_TD_4107;
    std::vector<cl::Event> events_d2h_rd_JOIN_LEFTSEMI_TD_4107;
    std::vector<cl::Event> events_JOIN_LEFTSEMI_TD_4107;
    events_h2d_wr_JOIN_LEFTSEMI_TD_4107.resize(1);
    events_d2h_rd_JOIN_LEFTSEMI_TD_4107.resize(1);
    events_JOIN_LEFTSEMI_TD_4107.resize(2);
    std::vector<cl::Event> events_grp_JOIN_LEFTSEMI_TD_4107;
    std::vector<cl::Event> prev_events_grp_JOIN_LEFTSEMI_TD_4107;
    std::vector<cl::Event> events_h2d_wr_Aggregate_TD_617;
    std::vector<cl::Event> events_d2h_rd_Aggregate_TD_617;
    std::vector<cl::Event> events_Aggregate_TD_617;
    events_h2d_wr_Aggregate_TD_617.resize(1);
    events_d2h_rd_Aggregate_TD_617.resize(1);
    events_Aggregate_TD_617.resize(1);
    std::vector<cl::Event> events_grp_Aggregate_TD_617;
    std::vector<cl::Event> prev_events_grp_Aggregate_TD_617;
    // **************************** Operations ************************** // 
    struct timeval tv_r_s, tv_r_e; 
    gettimeofday(&tv_r_s, 0); 

    struct timeval tv_r_Aggregate_6_389_s, tv_r_Aggregate_6_389_e;
    gettimeofday(&tv_r_Aggregate_6_389_s, 0);
    trans_Aggregate_TD_617.host2dev(0, &(prev_events_grp_Aggregate_TD_617), &(events_h2d_wr_Aggregate_TD_617[0]));
    events_grp_Aggregate_TD_617.push_back(events_h2d_wr_Aggregate_TD_617[0]);
    krnl_Aggregate_TD_617.run(0, &(events_grp_Aggregate_TD_617), &(events_Aggregate_TD_617[0]));
    
    trans_Aggregate_TD_617_out.add(&(tbl_Aggregate_TD_617_output_preprocess));
    trans_Aggregate_TD_617_out.add(&(cfg_Aggregate_TD_617_cmds_out));
    trans_Aggregate_TD_617_out.dev2host(0, &(events_Aggregate_TD_617), &(events_d2h_rd_Aggregate_TD_617[0]));
    q_a.flush();
    q_a.finish();
    
    SW_Aggregate_TD_617_consolidate(tbl_Aggregate_TD_617_output_preprocess, tbl_Aggregate_TD_617_output);
    gettimeofday(&tv_r_Aggregate_6_389_e, 0);

    struct timeval tv_r_Filter_5_394_s, tv_r_Filter_5_394_e;
    gettimeofday(&tv_r_Filter_5_394_s, 0);
    SW_Filter_TD_5902(tbl_Aggregate_TD_617_output, tbl_Filter_TD_5902_output);
    gettimeofday(&tv_r_Filter_5_394_e, 0);

    struct timeval tv_r_Filter_4_892_s, tv_r_Filter_4_892_e;
    gettimeofday(&tv_r_Filter_4_892_s, 0);
    SW_Filter_TD_4818(tbl_Aggregate_TD_617_output, tbl_Filter_TD_4818_output);
    gettimeofday(&tv_r_Filter_4_892_e, 0);

    struct timeval tv_r_JOIN_LEFTSEMI_4_664_s, tv_r_JOIN_LEFTSEMI_4_664_e;
    gettimeofday(&tv_r_JOIN_LEFTSEMI_4_664_s, 0);
    tbl_Filter_TD_5902_output.tableToCol();
    tbl_SerializeFromObject_TD_5404_input.tableToCol();
    krnl_JOIN_LEFTSEMI_TD_4107_build.updateMeta(tbl_Filter_TD_5902_output.getNumRow(), 3);
    krnl_JOIN_LEFTSEMI_TD_4107_probe.updateMeta(tbl_SerializeFromObject_TD_5404_input.getNumRow(), 3);
    trans_JOIN_LEFTSEMI_TD_4107.add(&(tbl_Filter_TD_5902_output));
    trans_JOIN_LEFTSEMI_TD_4107.host2dev(0, &(prev_events_grp_JOIN_LEFTSEMI_TD_4107), &(events_h2d_wr_JOIN_LEFTSEMI_TD_4107[0]));
    events_grp_JOIN_LEFTSEMI_TD_4107.push_back(events_h2d_wr_JOIN_LEFTSEMI_TD_4107[0]);
    krnl_JOIN_LEFTSEMI_TD_4107_build.run(0, &(events_grp_JOIN_LEFTSEMI_TD_4107), &(events_JOIN_LEFTSEMI_TD_4107[0]));
    std::vector<cl::Event> events_grp_JOIN_LEFTSEMI_TD_4107_build_done;
    events_grp_JOIN_LEFTSEMI_TD_4107_build_done.push_back(events_JOIN_LEFTSEMI_TD_4107[0]);
    krnl_JOIN_LEFTSEMI_TD_4107_probe.run(0, &(events_grp_JOIN_LEFTSEMI_TD_4107_build_done), &(events_JOIN_LEFTSEMI_TD_4107[1]));
    std::vector<cl::Event> events_grp_JOIN_LEFTSEMI_TD_4107_probe_done;
    events_grp_JOIN_LEFTSEMI_TD_4107_probe_done.push_back(events_JOIN_LEFTSEMI_TD_4107[1]);
    trans_JOIN_LEFTSEMI_TD_4107_out.add(&(tbl_JOIN_LEFTSEMI_TD_4107_output));
    trans_JOIN_LEFTSEMI_TD_4107_out.add(&(cfg_JOIN_LEFTSEMI_TD_4107_cmds_build));
    trans_JOIN_LEFTSEMI_TD_4107_out.add(&(cfg_JOIN_LEFTSEMI_TD_4107_cmds_probe));
    trans_JOIN_LEFTSEMI_TD_4107_out.dev2host(0, &(events_grp_JOIN_LEFTSEMI_TD_4107_probe_done), &(events_d2h_rd_JOIN_LEFTSEMI_TD_4107[0]));
    q_h.flush();
    q_h.finish();
    tbl_JOIN_LEFTSEMI_TD_4107_output.setNumRow((cfg_JOIN_LEFTSEMI_TD_4107_cmds_probe.meta_out)->getColLen());
    tbl_JOIN_LEFTSEMI_TD_4107_output.colToTable();
    SW_Consolidated_JOIN_LEFTSEMI_TD_4107_output(tbl_Filter_TD_5902_output, tbl_SerializeFromObject_TD_5404_input, tbl_JOIN_LEFTSEMI_TD_4107_output, tbl_JOIN_LEFTSEMI_TD_4107_consolidated_output);
    gettimeofday(&tv_r_JOIN_LEFTSEMI_4_664_e, 0);

    struct timeval tv_r_JOIN_LEFTSEMI_3_659_s, tv_r_JOIN_LEFTSEMI_3_659_e;
    gettimeofday(&tv_r_JOIN_LEFTSEMI_3_659_s, 0);
    tbl_Filter_TD_4818_output.tableToCol();
    tbl_SerializeFromObject_TD_4842_input.tableToCol();
    krnl_JOIN_LEFTSEMI_TD_3312_build.updateMeta(tbl_Filter_TD_4818_output.getNumRow(), 3);
    krnl_JOIN_LEFTSEMI_TD_3312_probe.updateMeta(tbl_SerializeFromObject_TD_4842_input.getNumRow(), 3);
    trans_JOIN_LEFTSEMI_TD_3312.add(&(tbl_Filter_TD_4818_output));
    trans_JOIN_LEFTSEMI_TD_3312.host2dev(0, &(prev_events_grp_JOIN_LEFTSEMI_TD_3312), &(events_h2d_wr_JOIN_LEFTSEMI_TD_3312[0]));
    events_grp_JOIN_LEFTSEMI_TD_3312.push_back(events_h2d_wr_JOIN_LEFTSEMI_TD_3312[0]);
    krnl_JOIN_LEFTSEMI_TD_3312_build.run(0, &(events_grp_JOIN_LEFTSEMI_TD_3312), &(events_JOIN_LEFTSEMI_TD_3312[0]));
    std::vector<cl::Event> events_grp_JOIN_LEFTSEMI_TD_3312_build_done;
    events_grp_JOIN_LEFTSEMI_TD_3312_build_done.push_back(events_JOIN_LEFTSEMI_TD_3312[0]);
    krnl_JOIN_LEFTSEMI_TD_3312_probe.run(0, &(events_grp_JOIN_LEFTSEMI_TD_3312_build_done), &(events_JOIN_LEFTSEMI_TD_3312[1]));
    std::vector<cl::Event> events_grp_JOIN_LEFTSEMI_TD_3312_probe_done;
    events_grp_JOIN_LEFTSEMI_TD_3312_probe_done.push_back(events_JOIN_LEFTSEMI_TD_3312[1]);
    trans_JOIN_LEFTSEMI_TD_3312_out.add(&(tbl_JOIN_LEFTSEMI_TD_3312_output));
    trans_JOIN_LEFTSEMI_TD_3312_out.add(&(cfg_JOIN_LEFTSEMI_TD_3312_cmds_build));
    trans_JOIN_LEFTSEMI_TD_3312_out.add(&(cfg_JOIN_LEFTSEMI_TD_3312_cmds_probe));
    trans_JOIN_LEFTSEMI_TD_3312_out.dev2host(0, &(events_grp_JOIN_LEFTSEMI_TD_3312_probe_done), &(events_d2h_rd_JOIN_LEFTSEMI_TD_3312[0]));
    q_h.flush();
    q_h.finish();
    tbl_JOIN_LEFTSEMI_TD_3312_output.setNumRow((cfg_JOIN_LEFTSEMI_TD_3312_cmds_probe.meta_out)->getColLen());
    tbl_JOIN_LEFTSEMI_TD_3312_output.colToTable();
    SW_Consolidated_JOIN_LEFTSEMI_TD_3312_output(tbl_Filter_TD_4818_output, tbl_SerializeFromObject_TD_4842_input, tbl_JOIN_LEFTSEMI_TD_3312_output, tbl_JOIN_LEFTSEMI_TD_3312_consolidated_output);
    gettimeofday(&tv_r_JOIN_LEFTSEMI_3_659_e, 0);

    struct timeval tv_r_JOIN_INNER_3_999_s, tv_r_JOIN_INNER_3_999_e;
    gettimeofday(&tv_r_JOIN_INNER_3_999_s, 0);
    prev_events_grp_JOIN_INNER_TD_3555.push_back(events_h2d_wr_JOIN_LEFTSEMI_TD_4107[0]);
    tbl_SerializeFromObject_TD_4795_input_stringRowIDSubstitute.tableToCol();
    tbl_JOIN_LEFTSEMI_TD_4107_consolidated_output.tableToCol();
    krnl_JOIN_INNER_TD_3555_build.updateMeta(tbl_SerializeFromObject_TD_4795_input_stringRowIDSubstitute.getNumRow(), 3);
    krnl_JOIN_INNER_TD_3555_probe.updateMeta(tbl_JOIN_LEFTSEMI_TD_4107_consolidated_output.getNumRow(), 3);
    trans_JOIN_INNER_TD_3555.host2dev(0, &(prev_events_grp_JOIN_INNER_TD_3555), &(events_h2d_wr_JOIN_INNER_TD_3555[0]));
    events_grp_JOIN_INNER_TD_3555.push_back(events_h2d_wr_JOIN_INNER_TD_3555[0]);
    events_grp_JOIN_INNER_TD_3555.push_back(events_JOIN_LEFTSEMI_TD_4107[0]);
    krnl_JOIN_INNER_TD_3555_build.run(0, &(events_grp_JOIN_INNER_TD_3555), &(events_JOIN_INNER_TD_3555[0]));
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_3555_build_done;
    events_grp_JOIN_INNER_TD_3555_build_done.push_back(events_JOIN_INNER_TD_3555[0]);
    krnl_JOIN_INNER_TD_3555_probe.run(0, &(events_grp_JOIN_INNER_TD_3555_build_done), &(events_JOIN_INNER_TD_3555[1]));
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_3555_probe_done;
    events_grp_JOIN_INNER_TD_3555_probe_done.push_back(events_JOIN_INNER_TD_3555[1]);
    trans_JOIN_INNER_TD_3555_out.add(&(tbl_JOIN_INNER_TD_3555_output));
    trans_JOIN_INNER_TD_3555_out.add(&(cfg_JOIN_INNER_TD_3555_cmds_build));
    trans_JOIN_INNER_TD_3555_out.add(&(cfg_JOIN_INNER_TD_3555_cmds_probe));
    trans_JOIN_INNER_TD_3555_out.dev2host(0, &(events_grp_JOIN_INNER_TD_3555_probe_done), &(events_d2h_rd_JOIN_INNER_TD_3555[0]));
    q_h.flush();
    q_h.finish();
    tbl_JOIN_INNER_TD_3555_output.setNumRow((cfg_JOIN_INNER_TD_3555_cmds_probe.meta_out)->getColLen());
    tbl_JOIN_INNER_TD_3555_output.colToTable();
    SW_Consolidated_JOIN_INNER_TD_3555_output(tbl_SerializeFromObject_TD_4795_input_stringRowIDSubstitute, tbl_JOIN_LEFTSEMI_TD_4107_consolidated_output, tbl_JOIN_INNER_TD_3555_output, tbl_JOIN_INNER_TD_3555_consolidated_output);
    gettimeofday(&tv_r_JOIN_INNER_3_999_e, 0);

    struct timeval tv_r_JOIN_INNER_2_215_s, tv_r_JOIN_INNER_2_215_e;
    gettimeofday(&tv_r_JOIN_INNER_2_215_s, 0);
    prev_events_grp_JOIN_INNER_TD_2987.push_back(events_h2d_wr_JOIN_INNER_TD_3555[0]);
    prev_events_grp_JOIN_INNER_TD_2987.push_back(events_h2d_wr_JOIN_LEFTSEMI_TD_3312[0]);
    tbl_JOIN_INNER_TD_3555_consolidated_output.tableToCol();
    tbl_JOIN_LEFTSEMI_TD_3312_consolidated_output.tableToCol();
    krnl_JOIN_INNER_TD_2987_build.updateMeta(tbl_JOIN_INNER_TD_3555_consolidated_output.getNumRow(), 3);
    krnl_JOIN_INNER_TD_2987_probe.updateMeta(tbl_JOIN_LEFTSEMI_TD_3312_consolidated_output.getNumRow(), 3);
    trans_JOIN_INNER_TD_2987.host2dev(0, &(prev_events_grp_JOIN_INNER_TD_2987), &(events_h2d_wr_JOIN_INNER_TD_2987[0]));
    events_grp_JOIN_INNER_TD_2987.push_back(events_h2d_wr_JOIN_INNER_TD_2987[0]);
    events_grp_JOIN_INNER_TD_2987.push_back(events_JOIN_INNER_TD_3555[0]);
    events_grp_JOIN_INNER_TD_2987.push_back(events_JOIN_LEFTSEMI_TD_3312[0]);
    krnl_JOIN_INNER_TD_2987_build.run(0, &(events_grp_JOIN_INNER_TD_2987), &(events_JOIN_INNER_TD_2987[0]));
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_2987_build_done;
    events_grp_JOIN_INNER_TD_2987_build_done.push_back(events_JOIN_INNER_TD_2987[0]);
    krnl_JOIN_INNER_TD_2987_probe.run(0, &(events_grp_JOIN_INNER_TD_2987_build_done), &(events_JOIN_INNER_TD_2987[1]));
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_2987_probe_done;
    events_grp_JOIN_INNER_TD_2987_probe_done.push_back(events_JOIN_INNER_TD_2987[1]);
    trans_JOIN_INNER_TD_2987_out.add(&(tbl_JOIN_INNER_TD_2987_output));
    trans_JOIN_INNER_TD_2987_out.add(&(cfg_JOIN_INNER_TD_2987_cmds_build));
    trans_JOIN_INNER_TD_2987_out.add(&(cfg_JOIN_INNER_TD_2987_cmds_probe));
    trans_JOIN_INNER_TD_2987_out.dev2host(0, &(events_grp_JOIN_INNER_TD_2987_probe_done), &(events_d2h_rd_JOIN_INNER_TD_2987[0]));
    q_h.flush();
    q_h.finish();
    tbl_JOIN_INNER_TD_2987_output.setNumRow((cfg_JOIN_INNER_TD_2987_cmds_probe.meta_out)->getColLen());
    tbl_JOIN_INNER_TD_2987_output.colToTable();
    SW_Consolidated_JOIN_INNER_TD_2987_output(tbl_JOIN_INNER_TD_3555_consolidated_output, tbl_JOIN_LEFTSEMI_TD_3312_consolidated_output, tbl_JOIN_INNER_TD_2987_output, tbl_JOIN_INNER_TD_2987_consolidated_output);
    gettimeofday(&tv_r_JOIN_INNER_2_215_e, 0);

    struct timeval tv_r_Aggregate_1_839_s, tv_r_Aggregate_1_839_e;
    gettimeofday(&tv_r_Aggregate_1_839_s, 0);
    SW_Aggregate_TD_1467(tbl_JOIN_INNER_TD_2987_consolidated_output, tbl_SerializeFromObject_TD_4795_input, tbl_Aggregate_TD_1467_output);
    gettimeofday(&tv_r_Aggregate_1_839_e, 0);

    struct timeval tv_r_Sort_0_675_s, tv_r_Sort_0_675_e;
    gettimeofday(&tv_r_Sort_0_675_s, 0);
    SW_Sort_TD_0664(tbl_Aggregate_TD_1467_output, tbl_Sort_TD_0664_output);
    gettimeofday(&tv_r_Sort_0_675_e, 0);

std::cout << "CPU TIME: " << (tvdiff(&tv_r_Filter_5_394_s, &tv_r_Filter_5_394_e) + 
tvdiff(&tv_r_Filter_4_892_s, &tv_r_Filter_4_892_e) + 
tvdiff(&tv_r_Aggregate_1_839_s, &tv_r_Aggregate_1_839_e) + 
tvdiff(&tv_r_Sort_0_675_s, &tv_r_Sort_0_675_e) + 
1) / 1000.0 << std::endl;

std::cout << "FPGA TIME: " << (tvdiff(&tv_r_Aggregate_6_389_s, &tv_r_Aggregate_6_389_e) + 
tvdiff(&tv_r_JOIN_LEFTSEMI_4_664_s, &tv_r_JOIN_LEFTSEMI_4_664_e) + 
tvdiff(&tv_r_JOIN_LEFTSEMI_3_659_s, &tv_r_JOIN_LEFTSEMI_3_659_e) + 
tvdiff(&tv_r_JOIN_INNER_3_999_s, &tv_r_JOIN_INNER_3_999_e) + 
tvdiff(&tv_r_JOIN_INNER_2_215_s, &tv_r_JOIN_INNER_2_215_e) + 
1) / 1000.0 << std::endl;

    gettimeofday(&tv_r_e, 0); 
    // **************************** Print Execution Time ************************** // 
    std::cout << std::endl; 
    std::cout << "Aggregate_6: " << tvdiff(&tv_r_Aggregate_6_389_s, &tv_r_Aggregate_6_389_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer(l_orderkey#77, sum(l_quantity#81) AS sum(l_quantity#81)#1714L) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_7328_input.getNumRow() << " -> tbl_SerializeFromObject_TD_7328_input" 
     << std::endl << "      #COL: 2: " << "ListBuffer(l_orderkey#77, l_quantity#81)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_617_output.getNumRow() << " -> tbl_Aggregate_TD_617_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(l_orderkey#77, sum(l_quantity#81)#1714L)" 
     << std::endl; 

    std::cout << "Filter_5: " << tvdiff(&tv_r_Filter_5_394_s, &tv_r_Filter_5_394_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer((isnotnull(sum(l_quantity#81)#1714L) AND (sum(l_quantity#81)#1714L > 312))) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_617_output.getNumRow() << " -> tbl_Aggregate_TD_617_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(l_orderkey#77, sum(l_quantity#81)#1714L)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_5902_output.getNumRow() << " -> tbl_Filter_TD_5902_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(l_orderkey#77)" 
     << std::endl; 

    std::cout << "Filter_4: " << tvdiff(&tv_r_Filter_4_892_s, &tv_r_Filter_4_892_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer((isnotnull(sum(l_quantity#81)#1714L) AND (sum(l_quantity#81)#1714L > 312))) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_617_output.getNumRow() << " -> tbl_Aggregate_TD_617_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(l_orderkey#77, sum(l_quantity#81)#1714L)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_4818_output.getNumRow() << " -> tbl_Filter_TD_4818_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(l_orderkey#77#1791)" 
     << std::endl; 

    std::cout << "JOIN_LEFTSEMI_4: " << tvdiff(&tv_r_JOIN_LEFTSEMI_4_664_s, &tv_r_JOIN_LEFTSEMI_4_664_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer((o_orderkey#207 = l_orderkey#77)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_5404_input.getNumRow() << " -> tbl_SerializeFromObject_TD_5404_input" 
     << std::endl << "      #COL: 4: " << "ListBuffer(o_orderkey#207, o_custkey#208, o_totalprice#210, o_orderdate#211)" 
     << std::endl << "      #ROW: " << tbl_Filter_TD_5902_output.getNumRow() << " -> tbl_Filter_TD_5902_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(l_orderkey#77)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_LEFTSEMI_TD_4107_consolidated_output.getNumRow() << " -> tbl_JOIN_LEFTSEMI_TD_4107_consolidated_output" 
     << std::endl << "      #COL: 4: " << "ListBuffer(o_orderkey#207, o_custkey#208, o_totalprice#210, o_orderdate#211)" 
     << std::endl; 

    std::cout << "JOIN_LEFTSEMI_3: " << tvdiff(&tv_r_JOIN_LEFTSEMI_3_659_s, &tv_r_JOIN_LEFTSEMI_3_659_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer((l_orderkey#77 = l_orderkey#77#1791)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_4842_input.getNumRow() << " -> tbl_SerializeFromObject_TD_4842_input" 
     << std::endl << "      #COL: 2: " << "ListBuffer(l_orderkey#77, l_quantity#81)" 
     << std::endl << "      #ROW: " << tbl_Filter_TD_4818_output.getNumRow() << " -> tbl_Filter_TD_4818_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(l_orderkey#77#1791)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_LEFTSEMI_TD_3312_consolidated_output.getNumRow() << " -> tbl_JOIN_LEFTSEMI_TD_3312_consolidated_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(l_orderkey#77, l_quantity#81)" 
     << std::endl; 

    std::cout << "JOIN_INNER_3: " << tvdiff(&tv_r_JOIN_INNER_3_999_s, &tv_r_JOIN_INNER_3_999_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer((c_custkey#22 = o_custkey#208)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_4795_input_stringRowIDSubstitute.getNumRow() << " -> tbl_SerializeFromObject_TD_4795_input_stringRowIDSubstitute" 
     << std::endl << "      #COL: 2: " << "ListBuffer(c_custkey#22, c_name#23)" 
     << std::endl << "      #ROW: " << tbl_JOIN_LEFTSEMI_TD_4107_consolidated_output.getNumRow() << " -> tbl_JOIN_LEFTSEMI_TD_4107_consolidated_output" 
     << std::endl << "      #COL: 4: " << "ListBuffer(o_orderkey#207, o_custkey#208, o_totalprice#210, o_orderdate#211)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_3555_consolidated_output.getNumRow() << " -> tbl_JOIN_INNER_TD_3555_consolidated_output" 
     << std::endl << "      #COL: 5: " << "ListBuffer(c_custkey#22, c_name#23, o_orderkey#207, o_totalprice#210, o_orderdate#211)" 
     << std::endl; 

    std::cout << "JOIN_INNER_2: " << tvdiff(&tv_r_JOIN_INNER_2_215_s, &tv_r_JOIN_INNER_2_215_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer((o_orderkey#207 = l_orderkey#77)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_3555_consolidated_output.getNumRow() << " -> tbl_JOIN_INNER_TD_3555_consolidated_output" 
     << std::endl << "      #COL: 5: " << "ListBuffer(c_custkey#22, c_name#23, o_orderkey#207, o_totalprice#210, o_orderdate#211)" 
     << std::endl << "      #ROW: " << tbl_JOIN_LEFTSEMI_TD_3312_consolidated_output.getNumRow() << " -> tbl_JOIN_LEFTSEMI_TD_3312_consolidated_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(l_orderkey#77, l_quantity#81)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_2987_consolidated_output.getNumRow() << " -> tbl_JOIN_INNER_TD_2987_consolidated_output" 
     << std::endl << "      #COL: 6: " << "ListBuffer(c_custkey#22, c_name#23, o_orderkey#207, o_totalprice#210, o_orderdate#211, l_quantity#81)" 
     << std::endl; 

    std::cout << "Aggregate_1: " << tvdiff(&tv_r_Aggregate_1_839_s, &tv_r_Aggregate_1_839_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(c_name#23, c_custkey#22, o_orderkey#207, o_orderdate#211, o_totalprice#210, sum(l_quantity#81) AS sum(l_quantity)#1717L) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_2987_consolidated_output.getNumRow() << " -> tbl_JOIN_INNER_TD_2987_consolidated_output" 
     << std::endl << "      #COL: 6: " << "ListBuffer(c_custkey#22, c_name#23, o_orderkey#207, o_totalprice#210, o_orderdate#211, l_quantity#81)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_1467_output.getNumRow() << " -> tbl_Aggregate_TD_1467_output" 
     << std::endl << "      #COL: 6: " << "ListBuffer(c_name#23, c_custkey#22, o_orderkey#207, o_orderdate#211, o_totalprice#210, sum(l_quantity)#1717L)" 
     << std::endl; 

    std::cout << "Sort_0: " << tvdiff(&tv_r_Sort_0_675_s, &tv_r_Sort_0_675_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(o_totalprice#210 DESC NULLS LAST, o_orderdate#211 ASC NULLS FIRST) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_1467_output.getNumRow() << " -> tbl_Aggregate_TD_1467_output" 
     << std::endl << "      #COL: 6: " << "ListBuffer(c_name#23, c_custkey#22, o_orderkey#207, o_orderdate#211, o_totalprice#210, sum(l_quantity)#1717L)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Sort_TD_0664_output.getNumRow() << " -> tbl_Sort_TD_0664_output" 
     << std::endl << "      #COL: 6: " << "ListBuffer(c_name#23, c_custkey#22, o_orderkey#207, o_orderdate#211, o_totalprice#210, sum(l_quantity)#1717L)" 
     << std::endl; 

    std::cout << std::endl << " Total execution time: " << tvdiff(&tv_r_s, &tv_r_e) / 1000 << " ms"; 

    std::cout << std::endl << " Spark elapsed time: " << 51.94324 * 1000 << "ms" << std::endl; 
    return 0; 
}
