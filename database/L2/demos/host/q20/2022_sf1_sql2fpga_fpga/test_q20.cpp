// number of overlays (w/o fusion): 5 
// number of overlays (w/ fusion): 5 
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

#include "cfgFunc_q20.hpp" 
#include "q20.hpp" 

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
    std::cout << "NOTE:running query #20\n."; 
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
    // ****************************** Tables **************************** // 
    Table tbl_Sort_TD_0177_output("tbl_Sort_TD_0177_output", 6100000, 2, "");
    tbl_Sort_TD_0177_output.allocateHost();
    Table tbl_JOIN_INNER_TD_1225_output("tbl_JOIN_INNER_TD_1225_output", 210, 4, "");
    tbl_JOIN_INNER_TD_1225_output.selectOverlayVersion(1);
    tbl_JOIN_INNER_TD_1225_output.allocateHost();
    Table tbl_JOIN_INNER_TD_1225_consolidated_output("tbl_JOIN_INNER_TD_1225_consolidated_output", 210, 2, "");
    tbl_JOIN_INNER_TD_1225_consolidated_output.selectOverlayVersion(1);
    tbl_JOIN_INNER_TD_1225_consolidated_output.allocateHost();
    Table tbl_JOIN_LEFTSEMI_TD_2260_output("tbl_JOIN_LEFTSEMI_TD_2260_output", 5366, 4, "");
    tbl_JOIN_LEFTSEMI_TD_2260_output.selectOverlayVersion(1);
    tbl_JOIN_LEFTSEMI_TD_2260_output.allocateHost();
    Table tbl_JOIN_LEFTSEMI_TD_2260_consolidated_output("tbl_JOIN_LEFTSEMI_TD_2260_consolidated_output", 5366, 3, "");
    tbl_JOIN_LEFTSEMI_TD_2260_consolidated_output.selectOverlayVersion(1);
    tbl_JOIN_LEFTSEMI_TD_2260_consolidated_output.allocateHost();
    Table tbl_Filter_TD_215_output("tbl_Filter_TD_215_output", 6100000, 1, "");
    tbl_Filter_TD_215_output.selectOverlayVersion(1);
    tbl_Filter_TD_215_output.allocateHost();
    Table tbl_SerializeFromObject_TD_3353_input_stringRowIDSubstitute;
    tbl_SerializeFromObject_TD_3353_input_stringRowIDSubstitute = Table("supplier", supplier_n, 4, in_dir);
    tbl_SerializeFromObject_TD_3353_input_stringRowIDSubstitute.addCol("s_suppkey", 4);
    tbl_SerializeFromObject_TD_3353_input_stringRowIDSubstitute.addCol("s_name", 4, 1, 0);
    tbl_SerializeFromObject_TD_3353_input_stringRowIDSubstitute.addCol("s_address", 4, 1, 0);
    tbl_SerializeFromObject_TD_3353_input_stringRowIDSubstitute.addCol("s_nationkey", 4);
    tbl_SerializeFromObject_TD_3353_input_stringRowIDSubstitute.allocateHost();
    tbl_SerializeFromObject_TD_3353_input_stringRowIDSubstitute.loadHost();
    Table tbl_SerializeFromObject_TD_3353_input;
    tbl_SerializeFromObject_TD_3353_input = Table("supplier", supplier_n, 4, in_dir);
    tbl_SerializeFromObject_TD_3353_input.addCol("s_suppkey", 4);
    tbl_SerializeFromObject_TD_3353_input.addCol("s_name", TPCH_READ_S_NAME_LEN+1);
    tbl_SerializeFromObject_TD_3353_input.addCol("s_address", TPCH_READ_S_ADDR_MAX+1);
    tbl_SerializeFromObject_TD_3353_input.addCol("s_nationkey", 4);
    tbl_SerializeFromObject_TD_3353_input.allocateHost();
    tbl_SerializeFromObject_TD_3353_input.loadHost();
    Table tbl_JOIN_INNER_TD_3497_output("tbl_JOIN_INNER_TD_3497_output", 7670, 4, "");
    tbl_JOIN_INNER_TD_3497_output.selectOverlayVersion(1);
    tbl_JOIN_INNER_TD_3497_output.allocateHost();
    Table tbl_JOIN_INNER_TD_3497_consolidated_output("tbl_JOIN_INNER_TD_3497_consolidated_output", 7670, 1, "");
    tbl_JOIN_INNER_TD_3497_consolidated_output.selectOverlayVersion(1);
    tbl_JOIN_INNER_TD_3497_consolidated_output.allocateHost();
    Table tbl_SerializeFromObject_TD_36_input;
    tbl_SerializeFromObject_TD_36_input = Table("nation", nation_n, 2, in_dir);
    tbl_SerializeFromObject_TD_36_input.addCol("n_nationkey", 4);
    tbl_SerializeFromObject_TD_36_input.addCol("n_name", TPCH_READ_NATION_LEN+1);
    tbl_SerializeFromObject_TD_36_input.allocateHost();
    tbl_SerializeFromObject_TD_36_input.loadHost();
    Table tbl_JOIN_LEFTSEMI_TD_4559_output("tbl_JOIN_LEFTSEMI_TD_4559_output", 11160, 4, "");
    tbl_JOIN_LEFTSEMI_TD_4559_output.selectOverlayVersion(1);
    tbl_JOIN_LEFTSEMI_TD_4559_output.allocateHost();
    Table tbl_JOIN_LEFTSEMI_TD_4559_consolidated_output("tbl_JOIN_LEFTSEMI_TD_4559_consolidated_output", 11160, 3, "");
    tbl_JOIN_LEFTSEMI_TD_4559_consolidated_output.selectOverlayVersion(1);
    tbl_JOIN_LEFTSEMI_TD_4559_consolidated_output.allocateHost();
    Table tbl_Filter_TD_4596_output("tbl_Filter_TD_4596_output", 6100000, 3, "");
    tbl_Filter_TD_4596_output.selectOverlayVersion(1);
    tbl_Filter_TD_4596_output.allocateHost();
    Table tbl_SerializeFromObject_TD_5932_input;
    tbl_SerializeFromObject_TD_5932_input = Table("partsupp", partsupp_n, 3, in_dir);
    tbl_SerializeFromObject_TD_5932_input.selectOverlayVersion(1);
    tbl_SerializeFromObject_TD_5932_input.addCol("ps_partkey", 4);
    tbl_SerializeFromObject_TD_5932_input.addCol("ps_suppkey", 4);
    tbl_SerializeFromObject_TD_5932_input.addCol("ps_availqty", 4);
    tbl_SerializeFromObject_TD_5932_input.allocateHost();
    tbl_SerializeFromObject_TD_5932_input.loadHost();
    Table tbl_Filter_TD_57_output("tbl_Filter_TD_57_output", 6100000, 1, "");
    tbl_Filter_TD_57_output.selectOverlayVersion(1);
    tbl_Filter_TD_57_output.allocateHost();
    Table tbl_Aggregate_TD_5596_output("tbl_Aggregate_TD_5596_output", 6100000, 3, "");
    tbl_Aggregate_TD_5596_output.allocateHost();
    Table tbl_SerializeFromObject_TD_6549_input;
    tbl_SerializeFromObject_TD_6549_input = Table("part", part_n, 2, in_dir);
    tbl_SerializeFromObject_TD_6549_input.addCol("p_partkey", 4);
    tbl_SerializeFromObject_TD_6549_input.addCol("p_name", TPCH_READ_P_NAME_LEN+1);
    tbl_SerializeFromObject_TD_6549_input.allocateHost();
    tbl_SerializeFromObject_TD_6549_input.loadHost();
    Table tbl_JOIN_LEFTSEMI_TD_6393_output("tbl_JOIN_LEFTSEMI_TD_6393_output", 12816, 4, "");
    tbl_JOIN_LEFTSEMI_TD_6393_output.selectOverlayVersion(1);
    tbl_JOIN_LEFTSEMI_TD_6393_output.allocateHost();
    Table tbl_JOIN_LEFTSEMI_TD_6393_consolidated_output("tbl_JOIN_LEFTSEMI_TD_6393_consolidated_output", 12816, 3, "");
    tbl_JOIN_LEFTSEMI_TD_6393_consolidated_output.selectOverlayVersion(1);
    tbl_JOIN_LEFTSEMI_TD_6393_consolidated_output.allocateHost();
    Table tbl_Filter_TD_7786_output("tbl_Filter_TD_7786_output", 6100000, 3, "");
    tbl_Filter_TD_7786_output.selectOverlayVersion(1);
    tbl_Filter_TD_7786_output.allocateHost();
    Table tbl_Filter_TD_772_output("tbl_Filter_TD_772_output", 6100000, 1, "");
    tbl_Filter_TD_772_output.selectOverlayVersion(1);
    tbl_Filter_TD_772_output.allocateHost();
    Table tbl_SerializeFromObject_TD_8350_input;
    tbl_SerializeFromObject_TD_8350_input = Table("lineitem", lineitem_n, 4, in_dir);
    tbl_SerializeFromObject_TD_8350_input.addCol("l_partkey", 4);
    tbl_SerializeFromObject_TD_8350_input.addCol("l_suppkey", 4);
    tbl_SerializeFromObject_TD_8350_input.addCol("l_quantity", 4);
    tbl_SerializeFromObject_TD_8350_input.addCol("l_shipdate", 4);
    tbl_SerializeFromObject_TD_8350_input.allocateHost();
    tbl_SerializeFromObject_TD_8350_input.loadHost();
    Table tbl_SerializeFromObject_TD_8228_input;
    tbl_SerializeFromObject_TD_8228_input = Table("part", part_n, 2, in_dir);
    tbl_SerializeFromObject_TD_8228_input.addCol("p_partkey", 4);
    tbl_SerializeFromObject_TD_8228_input.addCol("p_name", TPCH_READ_P_NAME_LEN+1);
    tbl_SerializeFromObject_TD_8228_input.allocateHost();
    tbl_SerializeFromObject_TD_8228_input.loadHost();
    // ********************** Allocate Device Buffer ******************** // 
    //tbl_JOIN_INNER_TD_1225_output.allocateDevBuffer(context_h, 32);
    //tbl_JOIN_LEFTSEMI_TD_2260_output.allocateDevBuffer(context_h, 32);
    //tbl_Filter_TD_215_output.allocateDevBuffer(context_h, 32);
    tbl_SerializeFromObject_TD_3353_input_stringRowIDSubstitute.allocateDevBuffer(context_h, 32);
    //tbl_JOIN_INNER_TD_3497_output.allocateDevBuffer(context_h, 32);
    //tbl_JOIN_LEFTSEMI_TD_4559_output.allocateDevBuffer(context_h, 32);
    //tbl_Filter_TD_4596_output.allocateDevBuffer(context_h, 32);
    // tbl_SerializeFromObject_TD_5932_input.allocateDevBuffer(context_h, 32);
    //tbl_Filter_TD_57_output.allocateDevBuffer(context_h, 32);
    //tbl_JOIN_LEFTSEMI_TD_6393_output.allocateDevBuffer(context_h, 32);
    //tbl_Filter_TD_7786_output.allocateDevBuffer(context_h, 32);
    //tbl_Filter_TD_772_output.allocateDevBuffer(context_h, 32);
    // ****************************** Config **************************** // 
    
    xf::database::gqe::KernelCommand krn_cmd_build_828 = xf::database::gqe::KernelCommand();
    xf::database::gqe::KernelCommand krn_cmd_probe_828 = xf::database::gqe::KernelCommand();
    cfgCmd cfg_JOIN_INNER_TD_1225_cmds_build;
    cfg_JOIN_INNER_TD_1225_cmds_build.setup(1);
    cfg_JOIN_INNER_TD_1225_cmds_build.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_1225_gqe_join_build(krn_cmd_build_828);
    cfg_JOIN_INNER_TD_1225_cmds_build.cmd = krn_cmd_build_828.getConfigBits();
    get_meta_info_JOIN_INNER_TD_1225_gqe_join_build(cfg_JOIN_INNER_TD_1225_cmds_build.meta_in, tbl_JOIN_LEFTSEMI_TD_2260_consolidated_output.nrow, 3);
    get_meta_info_JOIN_INNER_TD_1225_gqe_join_build(cfg_JOIN_INNER_TD_1225_cmds_build.meta_out, tbl_JOIN_INNER_TD_1225_output.nrow, 4);
    // cfg_JOIN_INNER_TD_1225_cmds_build.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_1225_cmds_probe;
    cfg_JOIN_INNER_TD_1225_cmds_probe.setup(1);
    cfg_JOIN_INNER_TD_1225_cmds_probe.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_1225_gqe_join_probe(krn_cmd_probe_828);
    cfg_JOIN_INNER_TD_1225_cmds_probe.cmd = krn_cmd_probe_828.getConfigBits();
    get_meta_info_JOIN_INNER_TD_1225_gqe_join_probe(cfg_JOIN_INNER_TD_1225_cmds_probe.meta_in, tbl_Filter_TD_215_output.nrow, 3);
    get_meta_info_JOIN_INNER_TD_1225_gqe_join_probe(cfg_JOIN_INNER_TD_1225_cmds_probe.meta_out, tbl_JOIN_INNER_TD_1225_output.nrow, 3);
    // cfg_JOIN_INNER_TD_1225_cmds_probe.allocateDevBuffer(context_h, 32);
    cfg_JOIN_INNER_TD_1225_cmds_build.input_key_columns = {2, -1, -1};
    cfg_JOIN_INNER_TD_1225_cmds_probe.input_key_columns = {0, -1, -1};
    cfg_JOIN_INNER_TD_1225_cmds_build.output_key_columns = {0, 1, 2, 3};
    cfg_JOIN_INNER_TD_1225_cmds_probe.output_key_columns = {0, 1, 2, 3};
    
    xf::database::gqe::KernelCommand krn_cmd_build_764 = xf::database::gqe::KernelCommand();
    xf::database::gqe::KernelCommand krn_cmd_probe_764 = xf::database::gqe::KernelCommand();
    cfgCmd cfg_JOIN_LEFTSEMI_TD_2260_cmds_build;
    cfg_JOIN_LEFTSEMI_TD_2260_cmds_build.setup(1);
    cfg_JOIN_LEFTSEMI_TD_2260_cmds_build.allocateHost();
    get_cfg_dat_JOIN_LEFTSEMI_TD_2260_gqe_join_build(krn_cmd_build_764);
    cfg_JOIN_LEFTSEMI_TD_2260_cmds_build.cmd = krn_cmd_build_764.getConfigBits();
    get_meta_info_JOIN_LEFTSEMI_TD_2260_gqe_join_build(cfg_JOIN_LEFTSEMI_TD_2260_cmds_build.meta_in, tbl_JOIN_INNER_TD_3497_consolidated_output.nrow, 3);
    get_meta_info_JOIN_LEFTSEMI_TD_2260_gqe_join_build(cfg_JOIN_LEFTSEMI_TD_2260_cmds_build.meta_out, tbl_JOIN_LEFTSEMI_TD_2260_output.nrow, 4);
    // cfg_JOIN_LEFTSEMI_TD_2260_cmds_build.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_LEFTSEMI_TD_2260_cmds_probe;
    cfg_JOIN_LEFTSEMI_TD_2260_cmds_probe.setup(1);
    cfg_JOIN_LEFTSEMI_TD_2260_cmds_probe.allocateHost();
    get_cfg_dat_JOIN_LEFTSEMI_TD_2260_gqe_join_probe(krn_cmd_probe_764);
    cfg_JOIN_LEFTSEMI_TD_2260_cmds_probe.cmd = krn_cmd_probe_764.getConfigBits();
    get_meta_info_JOIN_LEFTSEMI_TD_2260_gqe_join_probe(cfg_JOIN_LEFTSEMI_TD_2260_cmds_probe.meta_in, tbl_SerializeFromObject_TD_3353_input_stringRowIDSubstitute.nrow, 3);
    get_meta_info_JOIN_LEFTSEMI_TD_2260_gqe_join_probe(cfg_JOIN_LEFTSEMI_TD_2260_cmds_probe.meta_out, tbl_JOIN_LEFTSEMI_TD_2260_output.nrow, 3);
    // cfg_JOIN_LEFTSEMI_TD_2260_cmds_probe.allocateDevBuffer(context_h, 32);
    cfg_JOIN_LEFTSEMI_TD_2260_cmds_build.input_key_columns = {0, -1, -1};
    cfg_JOIN_LEFTSEMI_TD_2260_cmds_probe.input_key_columns = {0, -1, -1};
    cfg_JOIN_LEFTSEMI_TD_2260_cmds_build.output_key_columns = {0, 1, 2, 3};
    cfg_JOIN_LEFTSEMI_TD_2260_cmds_probe.output_key_columns = {0, 1, 2, 3};
    
    xf::database::gqe::KernelCommand krn_cmd_build_346 = xf::database::gqe::KernelCommand();
    xf::database::gqe::KernelCommand krn_cmd_probe_346 = xf::database::gqe::KernelCommand();
    cfgCmd cfg_JOIN_INNER_TD_3497_cmds_build;
    cfg_JOIN_INNER_TD_3497_cmds_build.setup(1);
    cfg_JOIN_INNER_TD_3497_cmds_build.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_3497_gqe_join_build(krn_cmd_build_346);
    cfg_JOIN_INNER_TD_3497_cmds_build.cmd = krn_cmd_build_346.getConfigBits();
    get_meta_info_JOIN_INNER_TD_3497_gqe_join_build(cfg_JOIN_INNER_TD_3497_cmds_build.meta_in, tbl_JOIN_LEFTSEMI_TD_4559_consolidated_output.nrow, 3);
    get_meta_info_JOIN_INNER_TD_3497_gqe_join_build(cfg_JOIN_INNER_TD_3497_cmds_build.meta_out, tbl_JOIN_INNER_TD_3497_output.nrow, 4);
    // cfg_JOIN_INNER_TD_3497_cmds_build.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_3497_cmds_probe;
    cfg_JOIN_INNER_TD_3497_cmds_probe.setup(1);
    cfg_JOIN_INNER_TD_3497_cmds_probe.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_3497_gqe_join_probe(krn_cmd_probe_346);
    cfg_JOIN_INNER_TD_3497_cmds_probe.cmd = krn_cmd_probe_346.getConfigBits();
    get_meta_info_JOIN_INNER_TD_3497_gqe_join_probe(cfg_JOIN_INNER_TD_3497_cmds_probe.meta_in, tbl_Filter_TD_4596_output.nrow, 3);
    get_meta_info_JOIN_INNER_TD_3497_gqe_join_probe(cfg_JOIN_INNER_TD_3497_cmds_probe.meta_out, tbl_JOIN_INNER_TD_3497_output.nrow, 3);
    // cfg_JOIN_INNER_TD_3497_cmds_probe.allocateDevBuffer(context_h, 32);
    cfg_JOIN_INNER_TD_3497_cmds_build.input_key_columns = {0, 1, -1};
    cfg_JOIN_INNER_TD_3497_cmds_probe.input_key_columns = {1, 2, -1};
    cfg_JOIN_INNER_TD_3497_cmds_build.output_key_columns = {0, 1, 2, 3};
    cfg_JOIN_INNER_TD_3497_cmds_probe.output_key_columns = {0, 1, 2, 3};
    
    xf::database::gqe::KernelCommand krn_cmd_build_686 = xf::database::gqe::KernelCommand();
    xf::database::gqe::KernelCommand krn_cmd_probe_686 = xf::database::gqe::KernelCommand();
    cfgCmd cfg_JOIN_LEFTSEMI_TD_4559_cmds_build;
    cfg_JOIN_LEFTSEMI_TD_4559_cmds_build.setup(1);
    cfg_JOIN_LEFTSEMI_TD_4559_cmds_build.allocateHost();
    get_cfg_dat_JOIN_LEFTSEMI_TD_4559_gqe_join_build(krn_cmd_build_686);
    cfg_JOIN_LEFTSEMI_TD_4559_cmds_build.cmd = krn_cmd_build_686.getConfigBits();
    get_meta_info_JOIN_LEFTSEMI_TD_4559_gqe_join_build(cfg_JOIN_LEFTSEMI_TD_4559_cmds_build.meta_in, tbl_Filter_TD_57_output.nrow, 3);
    get_meta_info_JOIN_LEFTSEMI_TD_4559_gqe_join_build(cfg_JOIN_LEFTSEMI_TD_4559_cmds_build.meta_out, tbl_JOIN_LEFTSEMI_TD_4559_output.nrow, 4);
    // cfg_JOIN_LEFTSEMI_TD_4559_cmds_build.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_LEFTSEMI_TD_4559_cmds_probe;
    cfg_JOIN_LEFTSEMI_TD_4559_cmds_probe.setup(1);
    cfg_JOIN_LEFTSEMI_TD_4559_cmds_probe.allocateHost();
    get_cfg_dat_JOIN_LEFTSEMI_TD_4559_gqe_join_probe(krn_cmd_probe_686);
    cfg_JOIN_LEFTSEMI_TD_4559_cmds_probe.cmd = krn_cmd_probe_686.getConfigBits();
    get_meta_info_JOIN_LEFTSEMI_TD_4559_gqe_join_probe(cfg_JOIN_LEFTSEMI_TD_4559_cmds_probe.meta_in, tbl_SerializeFromObject_TD_5932_input.nrow, 3);
    get_meta_info_JOIN_LEFTSEMI_TD_4559_gqe_join_probe(cfg_JOIN_LEFTSEMI_TD_4559_cmds_probe.meta_out, tbl_JOIN_LEFTSEMI_TD_4559_output.nrow, 3);
    // cfg_JOIN_LEFTSEMI_TD_4559_cmds_probe.allocateDevBuffer(context_h, 32);
    cfg_JOIN_LEFTSEMI_TD_4559_cmds_build.input_key_columns = {0, -1, -1};
    cfg_JOIN_LEFTSEMI_TD_4559_cmds_probe.input_key_columns = {0, -1, -1};
    cfg_JOIN_LEFTSEMI_TD_4559_cmds_build.output_key_columns = {0, 1, 2, 3};
    cfg_JOIN_LEFTSEMI_TD_4559_cmds_probe.output_key_columns = {0, 1, 2, 3};
    
    xf::database::gqe::KernelCommand krn_cmd_build_476 = xf::database::gqe::KernelCommand();
    xf::database::gqe::KernelCommand krn_cmd_probe_476 = xf::database::gqe::KernelCommand();
    cfgCmd cfg_JOIN_LEFTSEMI_TD_6393_cmds_build;
    cfg_JOIN_LEFTSEMI_TD_6393_cmds_build.setup(1);
    cfg_JOIN_LEFTSEMI_TD_6393_cmds_build.allocateHost();
    get_cfg_dat_JOIN_LEFTSEMI_TD_6393_gqe_join_build(krn_cmd_build_476);
    cfg_JOIN_LEFTSEMI_TD_6393_cmds_build.cmd = krn_cmd_build_476.getConfigBits();
    get_meta_info_JOIN_LEFTSEMI_TD_6393_gqe_join_build(cfg_JOIN_LEFTSEMI_TD_6393_cmds_build.meta_in, tbl_Filter_TD_772_output.nrow, 3);
    get_meta_info_JOIN_LEFTSEMI_TD_6393_gqe_join_build(cfg_JOIN_LEFTSEMI_TD_6393_cmds_build.meta_out, tbl_JOIN_LEFTSEMI_TD_6393_output.nrow, 4);
    // cfg_JOIN_LEFTSEMI_TD_6393_cmds_build.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_LEFTSEMI_TD_6393_cmds_probe;
    cfg_JOIN_LEFTSEMI_TD_6393_cmds_probe.setup(1);
    cfg_JOIN_LEFTSEMI_TD_6393_cmds_probe.allocateHost();
    get_cfg_dat_JOIN_LEFTSEMI_TD_6393_gqe_join_probe(krn_cmd_probe_476);
    cfg_JOIN_LEFTSEMI_TD_6393_cmds_probe.cmd = krn_cmd_probe_476.getConfigBits();
    get_meta_info_JOIN_LEFTSEMI_TD_6393_gqe_join_probe(cfg_JOIN_LEFTSEMI_TD_6393_cmds_probe.meta_in, tbl_Filter_TD_7786_output.nrow, 3);
    get_meta_info_JOIN_LEFTSEMI_TD_6393_gqe_join_probe(cfg_JOIN_LEFTSEMI_TD_6393_cmds_probe.meta_out, tbl_JOIN_LEFTSEMI_TD_6393_output.nrow, 3);
    // cfg_JOIN_LEFTSEMI_TD_6393_cmds_probe.allocateDevBuffer(context_h, 32);
    cfg_JOIN_LEFTSEMI_TD_6393_cmds_build.input_key_columns = {0, -1, -1};
    cfg_JOIN_LEFTSEMI_TD_6393_cmds_probe.input_key_columns = {0, -1, -1};
    cfg_JOIN_LEFTSEMI_TD_6393_cmds_build.output_key_columns = {0, 1, 2, 3};
    cfg_JOIN_LEFTSEMI_TD_6393_cmds_probe.output_key_columns = {0, 1, 2, 3};
    // *************************** Kernel Setup ************************* // 
    bufferTmp buftmp_h(context_h, 1); 
    buftmp_h.initBuffer(q_h); 
    std::cout << std::endl; 
    krnlEngine krnl_JOIN_INNER_TD_1225_build;
    krnl_JOIN_INNER_TD_1225_build = krnlEngine(program_h, q_h, context_h, "gqeKernel", 0);
    krnl_JOIN_INNER_TD_1225_build.setup(tbl_JOIN_LEFTSEMI_TD_2260_consolidated_output, tbl_Filter_TD_215_output, tbl_JOIN_INNER_TD_1225_output, cfg_JOIN_INNER_TD_1225_cmds_build, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_1225_probe;
    krnl_JOIN_INNER_TD_1225_probe = krnlEngine(program_h, q_h, context_h, "gqeKernel", 1);
    krnl_JOIN_INNER_TD_1225_probe.setup(tbl_Filter_TD_215_output, tbl_JOIN_LEFTSEMI_TD_2260_consolidated_output, tbl_JOIN_INNER_TD_1225_output, cfg_JOIN_INNER_TD_1225_cmds_probe, buftmp_h);
    krnlEngine krnl_JOIN_LEFTSEMI_TD_2260_build;
    krnl_JOIN_LEFTSEMI_TD_2260_build = krnlEngine(program_h, q_h, context_h, "gqeKernel", 0);
    krnl_JOIN_LEFTSEMI_TD_2260_build.setup(tbl_JOIN_INNER_TD_3497_consolidated_output, tbl_SerializeFromObject_TD_3353_input_stringRowIDSubstitute, tbl_JOIN_LEFTSEMI_TD_2260_output, cfg_JOIN_LEFTSEMI_TD_2260_cmds_build, buftmp_h);
    krnlEngine krnl_JOIN_LEFTSEMI_TD_2260_probe;
    krnl_JOIN_LEFTSEMI_TD_2260_probe = krnlEngine(program_h, q_h, context_h, "gqeKernel", 1);
    krnl_JOIN_LEFTSEMI_TD_2260_probe.setup(tbl_SerializeFromObject_TD_3353_input_stringRowIDSubstitute, tbl_JOIN_INNER_TD_3497_consolidated_output, tbl_JOIN_LEFTSEMI_TD_2260_output, cfg_JOIN_LEFTSEMI_TD_2260_cmds_probe, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_3497_build;
    krnl_JOIN_INNER_TD_3497_build = krnlEngine(program_h, q_h, context_h, "gqeKernel", 0);
    krnl_JOIN_INNER_TD_3497_build.setup(tbl_JOIN_LEFTSEMI_TD_4559_consolidated_output, tbl_Filter_TD_4596_output, tbl_JOIN_INNER_TD_3497_output, cfg_JOIN_INNER_TD_3497_cmds_build, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_3497_probe;
    krnl_JOIN_INNER_TD_3497_probe = krnlEngine(program_h, q_h, context_h, "gqeKernel", 1);
    krnl_JOIN_INNER_TD_3497_probe.setup(tbl_Filter_TD_4596_output, tbl_JOIN_LEFTSEMI_TD_4559_consolidated_output, tbl_JOIN_INNER_TD_3497_output, cfg_JOIN_INNER_TD_3497_cmds_probe, buftmp_h);
    krnlEngine krnl_JOIN_LEFTSEMI_TD_4559_build;
    krnl_JOIN_LEFTSEMI_TD_4559_build = krnlEngine(program_h, q_h, context_h, "gqeKernel", 0);
    krnl_JOIN_LEFTSEMI_TD_4559_build.setup(tbl_Filter_TD_57_output, tbl_SerializeFromObject_TD_5932_input, tbl_JOIN_LEFTSEMI_TD_4559_output, cfg_JOIN_LEFTSEMI_TD_4559_cmds_build, buftmp_h);
    krnlEngine krnl_JOIN_LEFTSEMI_TD_4559_probe;
    krnl_JOIN_LEFTSEMI_TD_4559_probe = krnlEngine(program_h, q_h, context_h, "gqeKernel", 1);
    krnl_JOIN_LEFTSEMI_TD_4559_probe.setup(tbl_SerializeFromObject_TD_5932_input, tbl_Filter_TD_57_output, tbl_JOIN_LEFTSEMI_TD_4559_output, cfg_JOIN_LEFTSEMI_TD_4559_cmds_probe, buftmp_h);
    krnlEngine krnl_JOIN_LEFTSEMI_TD_6393_build;
    krnl_JOIN_LEFTSEMI_TD_6393_build = krnlEngine(program_h, q_h, context_h, "gqeKernel", 0);
    krnl_JOIN_LEFTSEMI_TD_6393_build.setup(tbl_Filter_TD_772_output, tbl_Filter_TD_7786_output, tbl_JOIN_LEFTSEMI_TD_6393_output, cfg_JOIN_LEFTSEMI_TD_6393_cmds_build, buftmp_h);
    krnlEngine krnl_JOIN_LEFTSEMI_TD_6393_probe;
    krnl_JOIN_LEFTSEMI_TD_6393_probe = krnlEngine(program_h, q_h, context_h, "gqeKernel", 1);
    krnl_JOIN_LEFTSEMI_TD_6393_probe.setup(tbl_Filter_TD_7786_output, tbl_Filter_TD_772_output, tbl_JOIN_LEFTSEMI_TD_6393_output, cfg_JOIN_LEFTSEMI_TD_6393_cmds_probe, buftmp_h);
    // ************************** Transfer Engine *********************** // 
    transEngine trans_JOIN_INNER_TD_1225;
    trans_JOIN_INNER_TD_1225.setq(q_h,1);
    trans_JOIN_INNER_TD_1225.add(&(cfg_JOIN_INNER_TD_1225_cmds_build));
    trans_JOIN_INNER_TD_1225.add(&(cfg_JOIN_INNER_TD_1225_cmds_probe));
    transEngine trans_JOIN_INNER_TD_1225_out;
    trans_JOIN_INNER_TD_1225_out.setq(q_h,1);
    q_h.finish();
    transEngine trans_JOIN_LEFTSEMI_TD_2260;
    trans_JOIN_LEFTSEMI_TD_2260.setq(q_h,1);
    trans_JOIN_LEFTSEMI_TD_2260.add(&(cfg_JOIN_LEFTSEMI_TD_2260_cmds_build));
    trans_JOIN_LEFTSEMI_TD_2260.add(&(cfg_JOIN_LEFTSEMI_TD_2260_cmds_probe));
    trans_JOIN_LEFTSEMI_TD_2260.add(&(tbl_SerializeFromObject_TD_3353_input_stringRowIDSubstitute));
    transEngine trans_JOIN_LEFTSEMI_TD_2260_out;
    trans_JOIN_LEFTSEMI_TD_2260_out.setq(q_h,1);
    q_h.finish();
    transEngine trans_JOIN_INNER_TD_3497;
    trans_JOIN_INNER_TD_3497.setq(q_h,1);
    trans_JOIN_INNER_TD_3497.add(&(cfg_JOIN_INNER_TD_3497_cmds_build));
    trans_JOIN_INNER_TD_3497.add(&(cfg_JOIN_INNER_TD_3497_cmds_probe));
    transEngine trans_JOIN_INNER_TD_3497_out;
    trans_JOIN_INNER_TD_3497_out.setq(q_h,1);
    q_h.finish();
    transEngine trans_JOIN_LEFTSEMI_TD_4559;
    trans_JOIN_LEFTSEMI_TD_4559.setq(q_h,1);
    trans_JOIN_LEFTSEMI_TD_4559.add(&(cfg_JOIN_LEFTSEMI_TD_4559_cmds_build));
    trans_JOIN_LEFTSEMI_TD_4559.add(&(cfg_JOIN_LEFTSEMI_TD_4559_cmds_probe));
    trans_JOIN_LEFTSEMI_TD_4559.add(&(tbl_SerializeFromObject_TD_5932_input));
    transEngine trans_JOIN_LEFTSEMI_TD_4559_out;
    trans_JOIN_LEFTSEMI_TD_4559_out.setq(q_h,1);
    q_h.finish();
    transEngine trans_JOIN_LEFTSEMI_TD_6393;
    trans_JOIN_LEFTSEMI_TD_6393.setq(q_h,1);
    trans_JOIN_LEFTSEMI_TD_6393.add(&(cfg_JOIN_LEFTSEMI_TD_6393_cmds_build));
    trans_JOIN_LEFTSEMI_TD_6393.add(&(cfg_JOIN_LEFTSEMI_TD_6393_cmds_probe));
    transEngine trans_JOIN_LEFTSEMI_TD_6393_out;
    trans_JOIN_LEFTSEMI_TD_6393_out.setq(q_h,1);
    q_h.finish();
    // ****************************** Events **************************** // 
    std::vector<cl::Event> events_h2d_wr_JOIN_INNER_TD_1225;
    std::vector<cl::Event> events_d2h_rd_JOIN_INNER_TD_1225;
    std::vector<cl::Event> events_JOIN_INNER_TD_1225;
    events_h2d_wr_JOIN_INNER_TD_1225.resize(1);
    events_d2h_rd_JOIN_INNER_TD_1225.resize(1);
    events_JOIN_INNER_TD_1225.resize(2);
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_1225;
    std::vector<cl::Event> prev_events_grp_JOIN_INNER_TD_1225;
    std::vector<cl::Event> events_h2d_wr_JOIN_LEFTSEMI_TD_2260;
    std::vector<cl::Event> events_d2h_rd_JOIN_LEFTSEMI_TD_2260;
    std::vector<cl::Event> events_JOIN_LEFTSEMI_TD_2260;
    events_h2d_wr_JOIN_LEFTSEMI_TD_2260.resize(1);
    events_d2h_rd_JOIN_LEFTSEMI_TD_2260.resize(1);
    events_JOIN_LEFTSEMI_TD_2260.resize(2);
    std::vector<cl::Event> events_grp_JOIN_LEFTSEMI_TD_2260;
    std::vector<cl::Event> prev_events_grp_JOIN_LEFTSEMI_TD_2260;
    std::vector<cl::Event> events_h2d_wr_JOIN_INNER_TD_3497;
    std::vector<cl::Event> events_d2h_rd_JOIN_INNER_TD_3497;
    std::vector<cl::Event> events_JOIN_INNER_TD_3497;
    events_h2d_wr_JOIN_INNER_TD_3497.resize(1);
    events_d2h_rd_JOIN_INNER_TD_3497.resize(1);
    events_JOIN_INNER_TD_3497.resize(2);
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_3497;
    std::vector<cl::Event> prev_events_grp_JOIN_INNER_TD_3497;
    std::vector<cl::Event> events_h2d_wr_JOIN_LEFTSEMI_TD_4559;
    std::vector<cl::Event> events_d2h_rd_JOIN_LEFTSEMI_TD_4559;
    std::vector<cl::Event> events_JOIN_LEFTSEMI_TD_4559;
    events_h2d_wr_JOIN_LEFTSEMI_TD_4559.resize(1);
    events_d2h_rd_JOIN_LEFTSEMI_TD_4559.resize(1);
    events_JOIN_LEFTSEMI_TD_4559.resize(2);
    std::vector<cl::Event> events_grp_JOIN_LEFTSEMI_TD_4559;
    std::vector<cl::Event> prev_events_grp_JOIN_LEFTSEMI_TD_4559;
    std::vector<cl::Event> events_h2d_wr_JOIN_LEFTSEMI_TD_6393;
    std::vector<cl::Event> events_d2h_rd_JOIN_LEFTSEMI_TD_6393;
    std::vector<cl::Event> events_JOIN_LEFTSEMI_TD_6393;
    events_h2d_wr_JOIN_LEFTSEMI_TD_6393.resize(1);
    events_d2h_rd_JOIN_LEFTSEMI_TD_6393.resize(1);
    events_JOIN_LEFTSEMI_TD_6393.resize(2);
    std::vector<cl::Event> events_grp_JOIN_LEFTSEMI_TD_6393;
    std::vector<cl::Event> prev_events_grp_JOIN_LEFTSEMI_TD_6393;
    // **************************** Operations ************************** // 
    struct timeval tv_r_s, tv_r_e; 
    gettimeofday(&tv_r_s, 0); 

    struct timeval tv_r_Filter_7_212_s, tv_r_Filter_7_212_e;
    gettimeofday(&tv_r_Filter_7_212_s, 0);
    SW_Filter_TD_772(tbl_SerializeFromObject_TD_8228_input, tbl_Filter_TD_772_output);
    gettimeofday(&tv_r_Filter_7_212_e, 0);

    struct timeval tv_r_Filter_7_816_s, tv_r_Filter_7_816_e;
    gettimeofday(&tv_r_Filter_7_816_s, 0);
    SW_Filter_TD_7786(tbl_SerializeFromObject_TD_8350_input, tbl_Filter_TD_7786_output);
    gettimeofday(&tv_r_Filter_7_816_e, 0);

    struct timeval tv_r_JOIN_LEFTSEMI_6_507_s, tv_r_JOIN_LEFTSEMI_6_507_e;
    gettimeofday(&tv_r_JOIN_LEFTSEMI_6_507_s, 0);
    tbl_Filter_TD_772_output.tableToCol();
    tbl_Filter_TD_7786_output.tableToCol();
    krnl_JOIN_LEFTSEMI_TD_6393_build.updateMeta(tbl_Filter_TD_772_output.getNumRow(), 3);
    krnl_JOIN_LEFTSEMI_TD_6393_probe.updateMeta(tbl_Filter_TD_7786_output.getNumRow(), 3);
    trans_JOIN_LEFTSEMI_TD_6393.add(&(tbl_Filter_TD_7786_output));
    trans_JOIN_LEFTSEMI_TD_6393.add(&(tbl_Filter_TD_772_output));
    trans_JOIN_LEFTSEMI_TD_6393.host2dev(0, &(prev_events_grp_JOIN_LEFTSEMI_TD_6393), &(events_h2d_wr_JOIN_LEFTSEMI_TD_6393[0]));
    events_grp_JOIN_LEFTSEMI_TD_6393.push_back(events_h2d_wr_JOIN_LEFTSEMI_TD_6393[0]);
    krnl_JOIN_LEFTSEMI_TD_6393_build.run(0, &(events_grp_JOIN_LEFTSEMI_TD_6393), &(events_JOIN_LEFTSEMI_TD_6393[0]));
    std::vector<cl::Event> events_grp_JOIN_LEFTSEMI_TD_6393_build_done;
    events_grp_JOIN_LEFTSEMI_TD_6393_build_done.push_back(events_JOIN_LEFTSEMI_TD_6393[0]);
    krnl_JOIN_LEFTSEMI_TD_6393_probe.run(0, &(events_grp_JOIN_LEFTSEMI_TD_6393_build_done), &(events_JOIN_LEFTSEMI_TD_6393[1]));
    std::vector<cl::Event> events_grp_JOIN_LEFTSEMI_TD_6393_probe_done;
    events_grp_JOIN_LEFTSEMI_TD_6393_probe_done.push_back(events_JOIN_LEFTSEMI_TD_6393[1]);
    trans_JOIN_LEFTSEMI_TD_6393_out.add(&(tbl_JOIN_LEFTSEMI_TD_6393_output));
    trans_JOIN_LEFTSEMI_TD_6393_out.add(&(cfg_JOIN_LEFTSEMI_TD_6393_cmds_build));
    trans_JOIN_LEFTSEMI_TD_6393_out.add(&(cfg_JOIN_LEFTSEMI_TD_6393_cmds_probe));
    trans_JOIN_LEFTSEMI_TD_6393_out.dev2host(0, &(events_grp_JOIN_LEFTSEMI_TD_6393_probe_done), &(events_d2h_rd_JOIN_LEFTSEMI_TD_6393[0]));
    q_h.flush();
    q_h.finish();
    tbl_JOIN_LEFTSEMI_TD_6393_output.setNumRow((cfg_JOIN_LEFTSEMI_TD_6393_cmds_probe.meta_out)->getColLen());
    tbl_JOIN_LEFTSEMI_TD_6393_output.colToTable();
    SW_Consolidated_JOIN_LEFTSEMI_TD_6393_output(tbl_Filter_TD_772_output, tbl_Filter_TD_7786_output, tbl_JOIN_LEFTSEMI_TD_6393_output, tbl_JOIN_LEFTSEMI_TD_6393_consolidated_output);
    gettimeofday(&tv_r_JOIN_LEFTSEMI_6_507_e, 0);

    struct timeval tv_r_Aggregate_5_826_s, tv_r_Aggregate_5_826_e;
    gettimeofday(&tv_r_Aggregate_5_826_s, 0);
    SW_Aggregate_TD_5596(tbl_JOIN_LEFTSEMI_TD_6393_consolidated_output, tbl_Aggregate_TD_5596_output);
    gettimeofday(&tv_r_Aggregate_5_826_e, 0);

    struct timeval tv_r_Filter_5_211_s, tv_r_Filter_5_211_e;
    gettimeofday(&tv_r_Filter_5_211_s, 0);
    SW_Filter_TD_57(tbl_SerializeFromObject_TD_6549_input, tbl_Filter_TD_57_output);
    gettimeofday(&tv_r_Filter_5_211_e, 0);

    struct timeval tv_r_Filter_4_11_s, tv_r_Filter_4_11_e;
    gettimeofday(&tv_r_Filter_4_11_s, 0);
    SW_Filter_TD_4596(tbl_Aggregate_TD_5596_output, tbl_Filter_TD_4596_output);
    gettimeofday(&tv_r_Filter_4_11_e, 0);

    struct timeval tv_r_JOIN_LEFTSEMI_4_575_s, tv_r_JOIN_LEFTSEMI_4_575_e;
    gettimeofday(&tv_r_JOIN_LEFTSEMI_4_575_s, 0);
    tbl_Filter_TD_57_output.tableToCol();
    tbl_SerializeFromObject_TD_5932_input.tableToCol();
    krnl_JOIN_LEFTSEMI_TD_4559_build.updateMeta(tbl_Filter_TD_57_output.getNumRow(), 3);
    krnl_JOIN_LEFTSEMI_TD_4559_probe.updateMeta(tbl_SerializeFromObject_TD_5932_input.getNumRow(), 3);
    trans_JOIN_LEFTSEMI_TD_4559.add(&(tbl_Filter_TD_57_output));
    trans_JOIN_LEFTSEMI_TD_4559.host2dev(0, &(prev_events_grp_JOIN_LEFTSEMI_TD_4559), &(events_h2d_wr_JOIN_LEFTSEMI_TD_4559[0]));
    events_grp_JOIN_LEFTSEMI_TD_4559.push_back(events_h2d_wr_JOIN_LEFTSEMI_TD_4559[0]);
    krnl_JOIN_LEFTSEMI_TD_4559_build.run(0, &(events_grp_JOIN_LEFTSEMI_TD_4559), &(events_JOIN_LEFTSEMI_TD_4559[0]));
    std::vector<cl::Event> events_grp_JOIN_LEFTSEMI_TD_4559_build_done;
    events_grp_JOIN_LEFTSEMI_TD_4559_build_done.push_back(events_JOIN_LEFTSEMI_TD_4559[0]);
    krnl_JOIN_LEFTSEMI_TD_4559_probe.run(0, &(events_grp_JOIN_LEFTSEMI_TD_4559_build_done), &(events_JOIN_LEFTSEMI_TD_4559[1]));
    std::vector<cl::Event> events_grp_JOIN_LEFTSEMI_TD_4559_probe_done;
    events_grp_JOIN_LEFTSEMI_TD_4559_probe_done.push_back(events_JOIN_LEFTSEMI_TD_4559[1]);
    trans_JOIN_LEFTSEMI_TD_4559_out.add(&(tbl_JOIN_LEFTSEMI_TD_4559_output));
    trans_JOIN_LEFTSEMI_TD_4559_out.add(&(cfg_JOIN_LEFTSEMI_TD_4559_cmds_build));
    trans_JOIN_LEFTSEMI_TD_4559_out.add(&(cfg_JOIN_LEFTSEMI_TD_4559_cmds_probe));
    trans_JOIN_LEFTSEMI_TD_4559_out.dev2host(0, &(events_grp_JOIN_LEFTSEMI_TD_4559_probe_done), &(events_d2h_rd_JOIN_LEFTSEMI_TD_4559[0]));
    q_h.flush();
    q_h.finish();
    tbl_JOIN_LEFTSEMI_TD_4559_output.setNumRow((cfg_JOIN_LEFTSEMI_TD_4559_cmds_probe.meta_out)->getColLen());
    tbl_JOIN_LEFTSEMI_TD_4559_output.colToTable();
    SW_Consolidated_JOIN_LEFTSEMI_TD_4559_output(tbl_Filter_TD_57_output, tbl_SerializeFromObject_TD_5932_input, tbl_JOIN_LEFTSEMI_TD_4559_output, tbl_JOIN_LEFTSEMI_TD_4559_consolidated_output);
    gettimeofday(&tv_r_JOIN_LEFTSEMI_4_575_e, 0);

    struct timeval tv_r_JOIN_INNER_3_441_s, tv_r_JOIN_INNER_3_441_e;
    gettimeofday(&tv_r_JOIN_INNER_3_441_s, 0);
    prev_events_grp_JOIN_INNER_TD_3497.push_back(events_h2d_wr_JOIN_LEFTSEMI_TD_4559[0]);
    tbl_JOIN_LEFTSEMI_TD_4559_consolidated_output.tableToCol();
    tbl_Filter_TD_4596_output.tableToCol();
    krnl_JOIN_INNER_TD_3497_build.updateMeta(tbl_JOIN_LEFTSEMI_TD_4559_consolidated_output.getNumRow(), 3);
    krnl_JOIN_INNER_TD_3497_probe.updateMeta(tbl_Filter_TD_4596_output.getNumRow(), 3);
    trans_JOIN_INNER_TD_3497.add(&(tbl_Filter_TD_4596_output));
    trans_JOIN_INNER_TD_3497.host2dev(0, &(prev_events_grp_JOIN_INNER_TD_3497), &(events_h2d_wr_JOIN_INNER_TD_3497[0]));
    events_grp_JOIN_INNER_TD_3497.push_back(events_h2d_wr_JOIN_INNER_TD_3497[0]);
    events_grp_JOIN_INNER_TD_3497.push_back(events_JOIN_LEFTSEMI_TD_4559[0]);
    krnl_JOIN_INNER_TD_3497_build.run(0, &(events_grp_JOIN_INNER_TD_3497), &(events_JOIN_INNER_TD_3497[0]));
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_3497_build_done;
    events_grp_JOIN_INNER_TD_3497_build_done.push_back(events_JOIN_INNER_TD_3497[0]);
    krnl_JOIN_INNER_TD_3497_probe.run(0, &(events_grp_JOIN_INNER_TD_3497_build_done), &(events_JOIN_INNER_TD_3497[1]));
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_3497_probe_done;
    events_grp_JOIN_INNER_TD_3497_probe_done.push_back(events_JOIN_INNER_TD_3497[1]);
    trans_JOIN_INNER_TD_3497_out.add(&(tbl_JOIN_INNER_TD_3497_output));
    trans_JOIN_INNER_TD_3497_out.add(&(cfg_JOIN_INNER_TD_3497_cmds_build));
    trans_JOIN_INNER_TD_3497_out.add(&(cfg_JOIN_INNER_TD_3497_cmds_probe));
    trans_JOIN_INNER_TD_3497_out.dev2host(0, &(events_grp_JOIN_INNER_TD_3497_probe_done), &(events_d2h_rd_JOIN_INNER_TD_3497[0]));
    q_h.flush();
    q_h.finish();
    tbl_JOIN_INNER_TD_3497_output.setNumRow((cfg_JOIN_INNER_TD_3497_cmds_probe.meta_out)->getColLen());
    tbl_JOIN_INNER_TD_3497_output.colToTable();
    SW_Consolidated_JOIN_INNER_TD_3497_output(tbl_JOIN_LEFTSEMI_TD_4559_consolidated_output, tbl_Filter_TD_4596_output, tbl_JOIN_INNER_TD_3497_output, tbl_JOIN_INNER_TD_3497_consolidated_output);
    gettimeofday(&tv_r_JOIN_INNER_3_441_e, 0);

    struct timeval tv_r_Filter_2_643_s, tv_r_Filter_2_643_e;
    gettimeofday(&tv_r_Filter_2_643_s, 0);
    SW_Filter_TD_215(tbl_SerializeFromObject_TD_36_input, tbl_Filter_TD_215_output);
    gettimeofday(&tv_r_Filter_2_643_e, 0);

    struct timeval tv_r_JOIN_LEFTSEMI_2_205_s, tv_r_JOIN_LEFTSEMI_2_205_e;
    gettimeofday(&tv_r_JOIN_LEFTSEMI_2_205_s, 0);
    prev_events_grp_JOIN_LEFTSEMI_TD_2260.push_back(events_h2d_wr_JOIN_INNER_TD_3497[0]);
    tbl_JOIN_INNER_TD_3497_consolidated_output.tableToCol();
    tbl_SerializeFromObject_TD_3353_input_stringRowIDSubstitute.tableToCol();
    krnl_JOIN_LEFTSEMI_TD_2260_build.updateMeta(tbl_JOIN_INNER_TD_3497_consolidated_output.getNumRow(), 3);
    krnl_JOIN_LEFTSEMI_TD_2260_probe.updateMeta(tbl_SerializeFromObject_TD_3353_input_stringRowIDSubstitute.getNumRow(), 3);
    trans_JOIN_LEFTSEMI_TD_2260.host2dev(0, &(prev_events_grp_JOIN_LEFTSEMI_TD_2260), &(events_h2d_wr_JOIN_LEFTSEMI_TD_2260[0]));
    events_grp_JOIN_LEFTSEMI_TD_2260.push_back(events_h2d_wr_JOIN_LEFTSEMI_TD_2260[0]);
    events_grp_JOIN_LEFTSEMI_TD_2260.push_back(events_JOIN_INNER_TD_3497[0]);
    krnl_JOIN_LEFTSEMI_TD_2260_build.run(0, &(events_grp_JOIN_LEFTSEMI_TD_2260), &(events_JOIN_LEFTSEMI_TD_2260[0]));
    std::vector<cl::Event> events_grp_JOIN_LEFTSEMI_TD_2260_build_done;
    events_grp_JOIN_LEFTSEMI_TD_2260_build_done.push_back(events_JOIN_LEFTSEMI_TD_2260[0]);
    krnl_JOIN_LEFTSEMI_TD_2260_probe.run(0, &(events_grp_JOIN_LEFTSEMI_TD_2260_build_done), &(events_JOIN_LEFTSEMI_TD_2260[1]));
    std::vector<cl::Event> events_grp_JOIN_LEFTSEMI_TD_2260_probe_done;
    events_grp_JOIN_LEFTSEMI_TD_2260_probe_done.push_back(events_JOIN_LEFTSEMI_TD_2260[1]);
    trans_JOIN_LEFTSEMI_TD_2260_out.add(&(tbl_JOIN_LEFTSEMI_TD_2260_output));
    trans_JOIN_LEFTSEMI_TD_2260_out.add(&(cfg_JOIN_LEFTSEMI_TD_2260_cmds_build));
    trans_JOIN_LEFTSEMI_TD_2260_out.add(&(cfg_JOIN_LEFTSEMI_TD_2260_cmds_probe));
    trans_JOIN_LEFTSEMI_TD_2260_out.dev2host(0, &(events_grp_JOIN_LEFTSEMI_TD_2260_probe_done), &(events_d2h_rd_JOIN_LEFTSEMI_TD_2260[0]));
    q_h.flush();
    q_h.finish();
    tbl_JOIN_LEFTSEMI_TD_2260_output.setNumRow((cfg_JOIN_LEFTSEMI_TD_2260_cmds_probe.meta_out)->getColLen());
    tbl_JOIN_LEFTSEMI_TD_2260_output.colToTable();
    SW_Consolidated_JOIN_LEFTSEMI_TD_2260_output(tbl_JOIN_INNER_TD_3497_consolidated_output, tbl_SerializeFromObject_TD_3353_input_stringRowIDSubstitute, tbl_JOIN_LEFTSEMI_TD_2260_output, tbl_JOIN_LEFTSEMI_TD_2260_consolidated_output);
    gettimeofday(&tv_r_JOIN_LEFTSEMI_2_205_e, 0);

    struct timeval tv_r_JOIN_INNER_1_782_s, tv_r_JOIN_INNER_1_782_e;
    gettimeofday(&tv_r_JOIN_INNER_1_782_s, 0);
    prev_events_grp_JOIN_INNER_TD_1225.push_back(events_h2d_wr_JOIN_LEFTSEMI_TD_2260[0]);
    tbl_JOIN_LEFTSEMI_TD_2260_consolidated_output.tableToCol();
    tbl_Filter_TD_215_output.tableToCol();
    krnl_JOIN_INNER_TD_1225_build.updateMeta(tbl_JOIN_LEFTSEMI_TD_2260_consolidated_output.getNumRow(), 3);
    krnl_JOIN_INNER_TD_1225_probe.updateMeta(tbl_Filter_TD_215_output.getNumRow(), 3);
    trans_JOIN_INNER_TD_1225.add(&(tbl_Filter_TD_215_output));
    trans_JOIN_INNER_TD_1225.host2dev(0, &(prev_events_grp_JOIN_INNER_TD_1225), &(events_h2d_wr_JOIN_INNER_TD_1225[0]));
    events_grp_JOIN_INNER_TD_1225.push_back(events_h2d_wr_JOIN_INNER_TD_1225[0]);
    events_grp_JOIN_INNER_TD_1225.push_back(events_JOIN_LEFTSEMI_TD_2260[0]);
    krnl_JOIN_INNER_TD_1225_build.run(0, &(events_grp_JOIN_INNER_TD_1225), &(events_JOIN_INNER_TD_1225[0]));
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_1225_build_done;
    events_grp_JOIN_INNER_TD_1225_build_done.push_back(events_JOIN_INNER_TD_1225[0]);
    krnl_JOIN_INNER_TD_1225_probe.run(0, &(events_grp_JOIN_INNER_TD_1225_build_done), &(events_JOIN_INNER_TD_1225[1]));
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_1225_probe_done;
    events_grp_JOIN_INNER_TD_1225_probe_done.push_back(events_JOIN_INNER_TD_1225[1]);
    trans_JOIN_INNER_TD_1225_out.add(&(tbl_JOIN_INNER_TD_1225_output));
    trans_JOIN_INNER_TD_1225_out.add(&(cfg_JOIN_INNER_TD_1225_cmds_build));
    trans_JOIN_INNER_TD_1225_out.add(&(cfg_JOIN_INNER_TD_1225_cmds_probe));
    trans_JOIN_INNER_TD_1225_out.dev2host(0, &(events_grp_JOIN_INNER_TD_1225_probe_done), &(events_d2h_rd_JOIN_INNER_TD_1225[0]));
    q_h.flush();
    q_h.finish();
    tbl_JOIN_INNER_TD_1225_output.setNumRow((cfg_JOIN_INNER_TD_1225_cmds_probe.meta_out)->getColLen());
    tbl_JOIN_INNER_TD_1225_output.colToTable();
    SW_Consolidated_JOIN_INNER_TD_1225_output(tbl_JOIN_LEFTSEMI_TD_2260_consolidated_output, tbl_Filter_TD_215_output, tbl_JOIN_INNER_TD_1225_output, tbl_JOIN_INNER_TD_1225_consolidated_output);
    gettimeofday(&tv_r_JOIN_INNER_1_782_e, 0);

    struct timeval tv_r_Sort_0_174_s, tv_r_Sort_0_174_e;
    gettimeofday(&tv_r_Sort_0_174_s, 0);
    SW_Sort_TD_0177(tbl_JOIN_INNER_TD_1225_consolidated_output, tbl_SerializeFromObject_TD_3353_input, tbl_SerializeFromObject_TD_36_input, tbl_Sort_TD_0177_output);
    gettimeofday(&tv_r_Sort_0_174_e, 0);

std::cout << "CPU TIME: " << (tvdiff(&tv_r_Filter_7_212_s, &tv_r_Filter_7_212_e) + 
tvdiff(&tv_r_Filter_7_816_s, &tv_r_Filter_7_816_e) + 
tvdiff(&tv_r_Aggregate_5_826_s, &tv_r_Aggregate_5_826_e) + 
tvdiff(&tv_r_Filter_5_211_s, &tv_r_Filter_5_211_e) + 
tvdiff(&tv_r_Filter_4_11_s, &tv_r_Filter_4_11_e) + 
tvdiff(&tv_r_Filter_2_643_s, &tv_r_Filter_2_643_e) + 
tvdiff(&tv_r_Sort_0_174_s, &tv_r_Sort_0_174_e) + 
1) / 1000.0 << std::endl;

std::cout << "FPGA TIME: " << (tvdiff(&tv_r_JOIN_LEFTSEMI_6_507_s, &tv_r_JOIN_LEFTSEMI_6_507_e) + 
tvdiff(&tv_r_JOIN_LEFTSEMI_4_575_s, &tv_r_JOIN_LEFTSEMI_4_575_e) + 
tvdiff(&tv_r_JOIN_INNER_3_441_s, &tv_r_JOIN_INNER_3_441_e) + 
tvdiff(&tv_r_JOIN_LEFTSEMI_2_205_s, &tv_r_JOIN_LEFTSEMI_2_205_e) + 
tvdiff(&tv_r_JOIN_INNER_1_782_s, &tv_r_JOIN_INNER_1_782_e) + 
1) / 1000.0 << std::endl;

    gettimeofday(&tv_r_e, 0); 
    // **************************** Print Execution Time ************************** // 
    std::cout << std::endl; 
    std::cout << "Filter_7: " << tvdiff(&tv_r_Filter_7_212_s, &tv_r_Filter_7_212_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer((isnotnull(p_name#259) AND StartsWith(p_name#259, blush))) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_8228_input.getNumRow() << " -> tbl_SerializeFromObject_TD_8228_input" 
     << std::endl << "      #COL: 2: " << "ListBuffer(p_partkey#258, p_name#259)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_772_output.getNumRow() << " -> tbl_Filter_TD_772_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(p_partkey#258)" 
     << std::endl; 

    std::cout << "Filter_7: " << tvdiff(&tv_r_Filter_7_816_s, &tv_r_Filter_7_816_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(((l_shipdate#87 >= 19950101) AND (l_shipdate#87 < 19960101))) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_8350_input.getNumRow() << " -> tbl_SerializeFromObject_TD_8350_input" 
     << std::endl << "      #COL: 4: " << "ListBuffer(l_partkey#78, l_suppkey#79, l_quantity#81, l_shipdate#87)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_7786_output.getNumRow() << " -> tbl_Filter_TD_7786_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(l_partkey#78, l_suppkey#79, l_quantity#81)" 
     << std::endl; 

    std::cout << "JOIN_LEFTSEMI_6: " << tvdiff(&tv_r_JOIN_LEFTSEMI_6_507_s, &tv_r_JOIN_LEFTSEMI_6_507_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer((l_partkey#78 = p_partkey#258)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_7786_output.getNumRow() << " -> tbl_Filter_TD_7786_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(l_partkey#78, l_suppkey#79, l_quantity#81)" 
     << std::endl << "      #ROW: " << tbl_Filter_TD_772_output.getNumRow() << " -> tbl_Filter_TD_772_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(p_partkey#258)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_LEFTSEMI_TD_6393_consolidated_output.getNumRow() << " -> tbl_JOIN_LEFTSEMI_TD_6393_consolidated_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(l_partkey#78, l_suppkey#79, l_quantity#81)" 
     << std::endl; 

    std::cout << "Aggregate_5: " << tvdiff(&tv_r_Aggregate_5_826_s, &tv_r_Aggregate_5_826_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(l_partkey#78, l_suppkey#79, sum(l_quantity#81) AS sum(l_quantity)#1851L) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_LEFTSEMI_TD_6393_consolidated_output.getNumRow() << " -> tbl_JOIN_LEFTSEMI_TD_6393_consolidated_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(l_partkey#78, l_suppkey#79, l_quantity#81)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_5596_output.getNumRow() << " -> tbl_Aggregate_TD_5596_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(sum(l_quantity)#1851L, l_partkey#78, l_suppkey#79)" 
     << std::endl; 

    std::cout << "Filter_5: " << tvdiff(&tv_r_Filter_5_211_s, &tv_r_Filter_5_211_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer((isnotnull(p_name#259) AND StartsWith(p_name#259, blush))) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_6549_input.getNumRow() << " -> tbl_SerializeFromObject_TD_6549_input" 
     << std::endl << "      #COL: 2: " << "ListBuffer(p_partkey#258, p_name#259)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_57_output.getNumRow() << " -> tbl_Filter_TD_57_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(p_partkey#258)" 
     << std::endl; 

    std::cout << "Filter_4: " << tvdiff(&tv_r_Filter_4_11_s, &tv_r_Filter_4_11_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(isnotnull(sum(l_quantity)#1851L)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_5596_output.getNumRow() << " -> tbl_Aggregate_TD_5596_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(sum(l_quantity)#1851L, l_partkey#78, l_suppkey#79)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_4596_output.getNumRow() << " -> tbl_Filter_TD_4596_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(sum(l_quantity)#1851L, l_partkey#78, l_suppkey#79)" 
     << std::endl; 

    std::cout << "JOIN_LEFTSEMI_4: " << tvdiff(&tv_r_JOIN_LEFTSEMI_4_575_s, &tv_r_JOIN_LEFTSEMI_4_575_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer((ps_partkey#305 = p_partkey#258)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_5932_input.getNumRow() << " -> tbl_SerializeFromObject_TD_5932_input" 
     << std::endl << "      #COL: 3: " << "ListBuffer(ps_partkey#305, ps_suppkey#306, ps_availqty#307)" 
     << std::endl << "      #ROW: " << tbl_Filter_TD_57_output.getNumRow() << " -> tbl_Filter_TD_57_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(p_partkey#258)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_LEFTSEMI_TD_4559_consolidated_output.getNumRow() << " -> tbl_JOIN_LEFTSEMI_TD_4559_consolidated_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(ps_partkey#305, ps_suppkey#306, ps_availqty#307)" 
     << std::endl; 

    std::cout << "JOIN_INNER_3: " << tvdiff(&tv_r_JOIN_INNER_3_441_s, &tv_r_JOIN_INNER_3_441_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer((((cast(ps_availqty#307 as bigint) > sum(l_quantity)#1851L) AND (l_partkey#78 = ps_partkey#305)) AND (l_suppkey#79 = ps_suppkey#306))) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_LEFTSEMI_TD_4559_consolidated_output.getNumRow() << " -> tbl_JOIN_LEFTSEMI_TD_4559_consolidated_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(ps_partkey#305, ps_suppkey#306, ps_availqty#307)" 
     << std::endl << "      #ROW: " << tbl_Filter_TD_4596_output.getNumRow() << " -> tbl_Filter_TD_4596_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(sum(l_quantity)#1851L, l_partkey#78, l_suppkey#79)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_3497_consolidated_output.getNumRow() << " -> tbl_JOIN_INNER_TD_3497_consolidated_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(ps_suppkey#306)" 
     << std::endl; 

    std::cout << "Filter_2: " << tvdiff(&tv_r_Filter_2_643_s, &tv_r_Filter_2_643_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer((isnotnull(n_name#145) AND (n_name#145 = INDONESIA))) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_36_input.getNumRow() << " -> tbl_SerializeFromObject_TD_36_input" 
     << std::endl << "      #COL: 2: " << "ListBuffer(n_nationkey#144, n_name#145)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_215_output.getNumRow() << " -> tbl_Filter_TD_215_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(n_nationkey#144)" 
     << std::endl; 

    std::cout << "JOIN_LEFTSEMI_2: " << tvdiff(&tv_r_JOIN_LEFTSEMI_2_205_s, &tv_r_JOIN_LEFTSEMI_2_205_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer((s_suppkey#342 = ps_suppkey#306)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_3353_input_stringRowIDSubstitute.getNumRow() << " -> tbl_SerializeFromObject_TD_3353_input_stringRowIDSubstitute" 
     << std::endl << "      #COL: 4: " << "ListBuffer(s_suppkey#342, s_name#343, s_address#344, s_nationkey#345)" 
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_3497_consolidated_output.getNumRow() << " -> tbl_JOIN_INNER_TD_3497_consolidated_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(ps_suppkey#306)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_LEFTSEMI_TD_2260_consolidated_output.getNumRow() << " -> tbl_JOIN_LEFTSEMI_TD_2260_consolidated_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(s_name#343, s_address#344, s_nationkey#345)" 
     << std::endl; 

    std::cout << "JOIN_INNER_1: " << tvdiff(&tv_r_JOIN_INNER_1_782_s, &tv_r_JOIN_INNER_1_782_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer((s_nationkey#345 = n_nationkey#144)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_LEFTSEMI_TD_2260_consolidated_output.getNumRow() << " -> tbl_JOIN_LEFTSEMI_TD_2260_consolidated_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(s_name#343, s_address#344, s_nationkey#345)" 
     << std::endl << "      #ROW: " << tbl_Filter_TD_215_output.getNumRow() << " -> tbl_Filter_TD_215_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(n_nationkey#144)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_1225_consolidated_output.getNumRow() << " -> tbl_JOIN_INNER_TD_1225_consolidated_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(s_name#343, s_address#344)" 
     << std::endl; 

    std::cout << "Sort_0: " << tvdiff(&tv_r_Sort_0_174_s, &tv_r_Sort_0_174_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(s_name#343 ASC NULLS FIRST) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_1225_consolidated_output.getNumRow() << " -> tbl_JOIN_INNER_TD_1225_consolidated_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(s_name#343, s_address#344)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Sort_TD_0177_output.getNumRow() << " -> tbl_Sort_TD_0177_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(s_name#343, s_address#344)" 
     << std::endl; 

    std::cout << std::endl << " Total execution time: " << tvdiff(&tv_r_s, &tv_r_e) / 1000 << " ms"; 

    std::cout << std::endl << " Spark elapsed time: " << 20.77115 * 1000 << "ms" << std::endl; 
    return 0; 
}
