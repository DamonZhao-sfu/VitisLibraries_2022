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

#include "cfgFunc_q21.hpp" 
#include "q21.hpp" 

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
    std::cout << "NOTE:running query #21\n."; 
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
    Table tbl_Sort_TD_0156_output("tbl_Sort_TD_0156_output", 6100000, 2, "");
    tbl_Sort_TD_0156_output.allocateHost();
    Table tbl_Aggregate_TD_1429_output("tbl_Aggregate_TD_1429_output", 6100000, 2, "");
    tbl_Aggregate_TD_1429_output.allocateHost();
    Table tbl_JOIN_INNER_TD_2757_output("tbl_JOIN_INNER_TD_2757_output", 25255, 4, "");
    tbl_JOIN_INNER_TD_2757_output.selectOverlayVersion(1);
    tbl_JOIN_INNER_TD_2757_output.allocateHost();
    Table tbl_JOIN_INNER_TD_2757_consolidated_output("tbl_JOIN_INNER_TD_2757_consolidated_output", 25255, 1, "");
    tbl_JOIN_INNER_TD_2757_consolidated_output.selectOverlayVersion(1);
    tbl_JOIN_INNER_TD_2757_consolidated_output.allocateHost();
    Table tbl_Filter_TD_3451_output("tbl_Filter_TD_3451_output", 6100000, 1, "");
    tbl_Filter_TD_3451_output.selectOverlayVersion(1);
    tbl_Filter_TD_3451_output.allocateHost();
    Table tbl_JOIN_INNER_TD_4586_output("tbl_JOIN_INNER_TD_4586_output", 52212, 4, "");
    tbl_JOIN_INNER_TD_4586_output.selectOverlayVersion(1);
    tbl_JOIN_INNER_TD_4586_output.allocateHost();
    Table tbl_JOIN_INNER_TD_4586_consolidated_output("tbl_JOIN_INNER_TD_4586_consolidated_output", 52212, 2, "");
    tbl_JOIN_INNER_TD_4586_consolidated_output.selectOverlayVersion(1);
    tbl_JOIN_INNER_TD_4586_consolidated_output.allocateHost();
    Table tbl_SerializeFromObject_TD_5445_input;
    tbl_SerializeFromObject_TD_5445_input = Table("order", order_n, 2, in_dir);
    tbl_SerializeFromObject_TD_5445_input.addCol("o_orderkey", 4);
    tbl_SerializeFromObject_TD_5445_input.addCol("o_orderstatus", 4);
    tbl_SerializeFromObject_TD_5445_input.allocateHost();
    tbl_SerializeFromObject_TD_5445_input.loadHost();
    Table tbl_JOIN_INNER_TD_3454_output("tbl_JOIN_INNER_TD_3454_output", 377, 4, "");
    tbl_JOIN_INNER_TD_3454_output.selectOverlayVersion(1);
    tbl_JOIN_INNER_TD_3454_output.allocateHost();
    Table tbl_JOIN_INNER_TD_3454_consolidated_output("tbl_JOIN_INNER_TD_3454_consolidated_output", 377, 2, "");
    tbl_JOIN_INNER_TD_3454_consolidated_output.selectOverlayVersion(1);
    tbl_JOIN_INNER_TD_3454_consolidated_output.allocateHost();
    Table tbl_JOIN_LEFTANTI_TD_5488_output("tbl_JOIN_LEFTANTI_TD_5488_output", 1375555, 4, "");
    tbl_JOIN_LEFTANTI_TD_5488_output.selectOverlayVersion(1);
    tbl_JOIN_LEFTANTI_TD_5488_output.allocateHost();
    Table tbl_JOIN_LEFTANTI_TD_5488_consolidated_output("tbl_JOIN_LEFTANTI_TD_5488_consolidated_output", 1375555, 2, "");
    tbl_JOIN_LEFTANTI_TD_5488_consolidated_output.selectOverlayVersion(1);
    tbl_JOIN_LEFTANTI_TD_5488_consolidated_output.allocateHost();
    Table tbl_SerializeFromObject_TD_525_input_stringRowIDSubstitute;
    tbl_SerializeFromObject_TD_525_input_stringRowIDSubstitute = Table("supplier", supplier_n, 3, in_dir);
    tbl_SerializeFromObject_TD_525_input_stringRowIDSubstitute.addCol("s_suppkey", 4);
    tbl_SerializeFromObject_TD_525_input_stringRowIDSubstitute.addCol("s_name", 4, 1, 0);
    tbl_SerializeFromObject_TD_525_input_stringRowIDSubstitute.addCol("s_nationkey", 4);
    tbl_SerializeFromObject_TD_525_input_stringRowIDSubstitute.allocateHost();
    tbl_SerializeFromObject_TD_525_input_stringRowIDSubstitute.loadHost();
    Table tbl_SerializeFromObject_TD_525_input;
    tbl_SerializeFromObject_TD_525_input = Table("supplier", supplier_n, 3, in_dir);
    tbl_SerializeFromObject_TD_525_input.addCol("s_suppkey", 4);
    tbl_SerializeFromObject_TD_525_input.addCol("s_name", TPCH_READ_S_NAME_LEN+1);
    tbl_SerializeFromObject_TD_525_input.addCol("s_nationkey", 4);
    tbl_SerializeFromObject_TD_525_input.allocateHost();
    tbl_SerializeFromObject_TD_525_input.loadHost();
    Table tbl_Filter_TD_3765_output("tbl_Filter_TD_3765_output", 6100000, 1, "");
    tbl_Filter_TD_3765_output.selectOverlayVersion(1);
    tbl_Filter_TD_3765_output.allocateHost();
    Table tbl_JOIN_LEFTSEMI_TD_6755_output("tbl_JOIN_LEFTSEMI_TD_6755_output", 2417632, 4, "");
    tbl_JOIN_LEFTSEMI_TD_6755_output.selectOverlayVersion(1);
    tbl_JOIN_LEFTSEMI_TD_6755_output.allocateHost();
    Table tbl_JOIN_LEFTSEMI_TD_6755_consolidated_output("tbl_JOIN_LEFTSEMI_TD_6755_consolidated_output", 2417632, 2, "");
    tbl_JOIN_LEFTSEMI_TD_6755_consolidated_output.selectOverlayVersion(1);
    tbl_JOIN_LEFTSEMI_TD_6755_consolidated_output.allocateHost();
    Table tbl_SerializeFromObject_TD_4885_input;
    tbl_SerializeFromObject_TD_4885_input = Table("nation", nation_n, 2, in_dir);
    tbl_SerializeFromObject_TD_4885_input.addCol("n_nationkey", 4);
    tbl_SerializeFromObject_TD_4885_input.addCol("n_name", TPCH_READ_NATION_LEN+1);
    tbl_SerializeFromObject_TD_4885_input.allocateHost();
    tbl_SerializeFromObject_TD_4885_input.loadHost();
    Table tbl_JOIN_LEFTSEMI_TD_7524_output("tbl_JOIN_LEFTSEMI_TD_7524_output", 2844584, 4, "");
    tbl_JOIN_LEFTSEMI_TD_7524_output.selectOverlayVersion(1);
    tbl_JOIN_LEFTSEMI_TD_7524_output.allocateHost();
    Table tbl_JOIN_LEFTSEMI_TD_7524_consolidated_output("tbl_JOIN_LEFTSEMI_TD_7524_consolidated_output", 2844584, 2, "");
    tbl_JOIN_LEFTSEMI_TD_7524_consolidated_output.selectOverlayVersion(1);
    tbl_JOIN_LEFTSEMI_TD_7524_consolidated_output.allocateHost();
    Table tbl_Filter_TD_7740_output("tbl_Filter_TD_7740_output", 6100000, 2, "");
    tbl_Filter_TD_7740_output.selectOverlayVersion(1);
    tbl_Filter_TD_7740_output.allocateHost();
    Table tbl_Filter_TD_8737_output("tbl_Filter_TD_8737_output", 6100000, 2, "");
    tbl_Filter_TD_8737_output.selectOverlayVersion(1);
    tbl_Filter_TD_8737_output.allocateHost();
    Table tbl_SerializeFromObject_TD_8959_input;
    tbl_SerializeFromObject_TD_8959_input = Table("lineitem", lineitem_n, 2, in_dir);
    tbl_SerializeFromObject_TD_8959_input.selectOverlayVersion(1);
    tbl_SerializeFromObject_TD_8959_input.addCol("l_orderkey", 4);
    tbl_SerializeFromObject_TD_8959_input.addCol("l_suppkey", 4);
    tbl_SerializeFromObject_TD_8959_input.allocateHost();
    tbl_SerializeFromObject_TD_8959_input.loadHost();
    Table tbl_SerializeFromObject_TD_9761_input;
    tbl_SerializeFromObject_TD_9761_input = Table("lineitem", lineitem_n, 4, in_dir);
    tbl_SerializeFromObject_TD_9761_input.addCol("l_orderkey", 4);
    tbl_SerializeFromObject_TD_9761_input.addCol("l_suppkey", 4);
    tbl_SerializeFromObject_TD_9761_input.addCol("l_receiptdate", 4);
    tbl_SerializeFromObject_TD_9761_input.addCol("l_commitdate", 4);
    tbl_SerializeFromObject_TD_9761_input.allocateHost();
    tbl_SerializeFromObject_TD_9761_input.loadHost();
    // ********************** Allocate Device Buffer ******************** // 
    //tbl_JOIN_INNER_TD_2757_output.allocateDevBuffer(context_h, 32);
    //tbl_Filter_TD_3451_output.allocateDevBuffer(context_h, 32);
    //tbl_JOIN_INNER_TD_4586_output.allocateDevBuffer(context_h, 32);
    //tbl_JOIN_INNER_TD_3454_output.allocateDevBuffer(context_h, 32);
    //tbl_JOIN_LEFTANTI_TD_5488_output.allocateDevBuffer(context_h, 32);
    tbl_SerializeFromObject_TD_525_input_stringRowIDSubstitute.allocateDevBuffer(context_h, 32);
    //tbl_Filter_TD_3765_output.allocateDevBuffer(context_h, 32);
    //tbl_JOIN_LEFTSEMI_TD_6755_output.allocateDevBuffer(context_h, 32);
    //tbl_JOIN_LEFTSEMI_TD_7524_output.allocateDevBuffer(context_h, 32);
    //tbl_Filter_TD_7740_output.allocateDevBuffer(context_h, 32);
    //tbl_Filter_TD_8737_output.allocateDevBuffer(context_h, 32);
    // tbl_SerializeFromObject_TD_8959_input.allocateDevBuffer(context_h, 32);
    // ****************************** Config **************************** // 
    
    xf::database::gqe::KernelCommand krn_cmd_build_29 = xf::database::gqe::KernelCommand();
    xf::database::gqe::KernelCommand krn_cmd_probe_29 = xf::database::gqe::KernelCommand();
    cfgCmd cfg_JOIN_INNER_TD_2757_cmds_build;
    cfg_JOIN_INNER_TD_2757_cmds_build.setup(1);
    cfg_JOIN_INNER_TD_2757_cmds_build.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_2757_gqe_join_build(krn_cmd_build_29);
    cfg_JOIN_INNER_TD_2757_cmds_build.cmd = krn_cmd_build_29.getConfigBits();
    get_meta_info_JOIN_INNER_TD_2757_gqe_join_build(cfg_JOIN_INNER_TD_2757_cmds_build.meta_in, tbl_JOIN_INNER_TD_4586_consolidated_output.nrow, 3);
    get_meta_info_JOIN_INNER_TD_2757_gqe_join_build(cfg_JOIN_INNER_TD_2757_cmds_build.meta_out, tbl_JOIN_INNER_TD_2757_output.nrow, 4);
    // cfg_JOIN_INNER_TD_2757_cmds_build.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_2757_cmds_probe;
    cfg_JOIN_INNER_TD_2757_cmds_probe.setup(1);
    cfg_JOIN_INNER_TD_2757_cmds_probe.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_2757_gqe_join_probe(krn_cmd_probe_29);
    cfg_JOIN_INNER_TD_2757_cmds_probe.cmd = krn_cmd_probe_29.getConfigBits();
    get_meta_info_JOIN_INNER_TD_2757_gqe_join_probe(cfg_JOIN_INNER_TD_2757_cmds_probe.meta_in, tbl_Filter_TD_3451_output.nrow, 3);
    get_meta_info_JOIN_INNER_TD_2757_gqe_join_probe(cfg_JOIN_INNER_TD_2757_cmds_probe.meta_out, tbl_JOIN_INNER_TD_2757_output.nrow, 3);
    // cfg_JOIN_INNER_TD_2757_cmds_probe.allocateDevBuffer(context_h, 32);
    cfg_JOIN_INNER_TD_2757_cmds_build.input_key_columns = {1, -1, -1};
    cfg_JOIN_INNER_TD_2757_cmds_probe.input_key_columns = {0, -1, -1};
    cfg_JOIN_INNER_TD_2757_cmds_build.output_key_columns = {0, 1, 2, 3};
    cfg_JOIN_INNER_TD_2757_cmds_probe.output_key_columns = {0, 1, 2, 3};
    
    xf::database::gqe::KernelCommand krn_cmd_build_265 = xf::database::gqe::KernelCommand();
    xf::database::gqe::KernelCommand krn_cmd_probe_265 = xf::database::gqe::KernelCommand();
    cfgCmd cfg_JOIN_INNER_TD_4586_cmds_build;
    cfg_JOIN_INNER_TD_4586_cmds_build.setup(1);
    cfg_JOIN_INNER_TD_4586_cmds_build.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_4586_gqe_join_build(krn_cmd_build_265);
    cfg_JOIN_INNER_TD_4586_cmds_build.cmd = krn_cmd_build_265.getConfigBits();
    get_meta_info_JOIN_INNER_TD_4586_gqe_join_build(cfg_JOIN_INNER_TD_4586_cmds_build.meta_in, tbl_JOIN_INNER_TD_3454_consolidated_output.nrow, 3);
    get_meta_info_JOIN_INNER_TD_4586_gqe_join_build(cfg_JOIN_INNER_TD_4586_cmds_build.meta_out, tbl_JOIN_INNER_TD_4586_output.nrow, 4);
    // cfg_JOIN_INNER_TD_4586_cmds_build.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_4586_cmds_probe;
    cfg_JOIN_INNER_TD_4586_cmds_probe.setup(1);
    cfg_JOIN_INNER_TD_4586_cmds_probe.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_4586_gqe_join_probe(krn_cmd_probe_265);
    cfg_JOIN_INNER_TD_4586_cmds_probe.cmd = krn_cmd_probe_265.getConfigBits();
    get_meta_info_JOIN_INNER_TD_4586_gqe_join_probe(cfg_JOIN_INNER_TD_4586_cmds_probe.meta_in, tbl_JOIN_LEFTANTI_TD_5488_consolidated_output.nrow, 3);
    get_meta_info_JOIN_INNER_TD_4586_gqe_join_probe(cfg_JOIN_INNER_TD_4586_cmds_probe.meta_out, tbl_JOIN_INNER_TD_4586_output.nrow, 3);
    // cfg_JOIN_INNER_TD_4586_cmds_probe.allocateDevBuffer(context_h, 32);
    cfg_JOIN_INNER_TD_4586_cmds_build.input_key_columns = {0, -1, -1};
    cfg_JOIN_INNER_TD_4586_cmds_probe.input_key_columns = {1, -1, -1};
    cfg_JOIN_INNER_TD_4586_cmds_build.output_key_columns = {0, 1, 2, 3};
    cfg_JOIN_INNER_TD_4586_cmds_probe.output_key_columns = {0, 1, 2, 3};
    
    xf::database::gqe::KernelCommand krn_cmd_build_979 = xf::database::gqe::KernelCommand();
    xf::database::gqe::KernelCommand krn_cmd_probe_979 = xf::database::gqe::KernelCommand();
    cfgCmd cfg_JOIN_INNER_TD_3454_cmds_build;
    cfg_JOIN_INNER_TD_3454_cmds_build.setup(1);
    cfg_JOIN_INNER_TD_3454_cmds_build.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_3454_gqe_join_build(krn_cmd_build_979);
    cfg_JOIN_INNER_TD_3454_cmds_build.cmd = krn_cmd_build_979.getConfigBits();
    get_meta_info_JOIN_INNER_TD_3454_gqe_join_build(cfg_JOIN_INNER_TD_3454_cmds_build.meta_in, tbl_SerializeFromObject_TD_525_input_stringRowIDSubstitute.nrow, 3);
    get_meta_info_JOIN_INNER_TD_3454_gqe_join_build(cfg_JOIN_INNER_TD_3454_cmds_build.meta_out, tbl_JOIN_INNER_TD_3454_output.nrow, 4);
    // cfg_JOIN_INNER_TD_3454_cmds_build.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_3454_cmds_probe;
    cfg_JOIN_INNER_TD_3454_cmds_probe.setup(1);
    cfg_JOIN_INNER_TD_3454_cmds_probe.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_3454_gqe_join_probe(krn_cmd_probe_979);
    cfg_JOIN_INNER_TD_3454_cmds_probe.cmd = krn_cmd_probe_979.getConfigBits();
    get_meta_info_JOIN_INNER_TD_3454_gqe_join_probe(cfg_JOIN_INNER_TD_3454_cmds_probe.meta_in, tbl_Filter_TD_3765_output.nrow, 3);
    get_meta_info_JOIN_INNER_TD_3454_gqe_join_probe(cfg_JOIN_INNER_TD_3454_cmds_probe.meta_out, tbl_JOIN_INNER_TD_3454_output.nrow, 3);
    // cfg_JOIN_INNER_TD_3454_cmds_probe.allocateDevBuffer(context_h, 32);
    cfg_JOIN_INNER_TD_3454_cmds_build.input_key_columns = {2, -1, -1};
    cfg_JOIN_INNER_TD_3454_cmds_probe.input_key_columns = {0, -1, -1};
    cfg_JOIN_INNER_TD_3454_cmds_build.output_key_columns = {0, 1, 2, 3};
    cfg_JOIN_INNER_TD_3454_cmds_probe.output_key_columns = {0, 1, 2, 3};
    
    xf::database::gqe::KernelCommand krn_cmd_build_41 = xf::database::gqe::KernelCommand();
    xf::database::gqe::KernelCommand krn_cmd_probe_41 = xf::database::gqe::KernelCommand();
    cfgCmd cfg_JOIN_LEFTANTI_TD_5488_cmds_build;
    cfg_JOIN_LEFTANTI_TD_5488_cmds_build.setup(1);
    cfg_JOIN_LEFTANTI_TD_5488_cmds_build.allocateHost();
    get_cfg_dat_JOIN_LEFTANTI_TD_5488_gqe_join_build(krn_cmd_build_41);
    cfg_JOIN_LEFTANTI_TD_5488_cmds_build.cmd = krn_cmd_build_41.getConfigBits();
    get_meta_info_JOIN_LEFTANTI_TD_5488_gqe_join_build(cfg_JOIN_LEFTANTI_TD_5488_cmds_build.meta_in, tbl_JOIN_LEFTSEMI_TD_6755_consolidated_output.nrow, 3);
    get_meta_info_JOIN_LEFTANTI_TD_5488_gqe_join_build(cfg_JOIN_LEFTANTI_TD_5488_cmds_build.meta_out, tbl_JOIN_LEFTANTI_TD_5488_output.nrow, 4);
    // cfg_JOIN_LEFTANTI_TD_5488_cmds_build.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_LEFTANTI_TD_5488_cmds_probe;
    cfg_JOIN_LEFTANTI_TD_5488_cmds_probe.setup(1);
    cfg_JOIN_LEFTANTI_TD_5488_cmds_probe.allocateHost();
    get_cfg_dat_JOIN_LEFTANTI_TD_5488_gqe_join_probe(krn_cmd_probe_41);
    cfg_JOIN_LEFTANTI_TD_5488_cmds_probe.cmd = krn_cmd_probe_41.getConfigBits();
    get_meta_info_JOIN_LEFTANTI_TD_5488_gqe_join_probe(cfg_JOIN_LEFTANTI_TD_5488_cmds_probe.meta_in, tbl_Filter_TD_7740_output.nrow, 3);
    get_meta_info_JOIN_LEFTANTI_TD_5488_gqe_join_probe(cfg_JOIN_LEFTANTI_TD_5488_cmds_probe.meta_out, tbl_JOIN_LEFTANTI_TD_5488_output.nrow, 3);
    // cfg_JOIN_LEFTANTI_TD_5488_cmds_probe.allocateDevBuffer(context_h, 32);
    cfg_JOIN_LEFTANTI_TD_5488_cmds_build.input_key_columns = {0, 1, -1};
    cfg_JOIN_LEFTANTI_TD_5488_cmds_probe.input_key_columns = {0, 1, -1};
    cfg_JOIN_LEFTANTI_TD_5488_cmds_build.output_key_columns = {0, 1, 2, 3};
    cfg_JOIN_LEFTANTI_TD_5488_cmds_probe.output_key_columns = {0, 1, 2, 3};
    
    xf::database::gqe::KernelCommand krn_cmd_build_799 = xf::database::gqe::KernelCommand();
    xf::database::gqe::KernelCommand krn_cmd_probe_799 = xf::database::gqe::KernelCommand();
    cfgCmd cfg_JOIN_LEFTSEMI_TD_6755_cmds_build;
    cfg_JOIN_LEFTSEMI_TD_6755_cmds_build.setup(1);
    cfg_JOIN_LEFTSEMI_TD_6755_cmds_build.allocateHost();
    get_cfg_dat_JOIN_LEFTSEMI_TD_6755_gqe_join_build(krn_cmd_build_799);
    cfg_JOIN_LEFTSEMI_TD_6755_cmds_build.cmd = krn_cmd_build_799.getConfigBits();
    get_meta_info_JOIN_LEFTSEMI_TD_6755_gqe_join_build(cfg_JOIN_LEFTSEMI_TD_6755_cmds_build.meta_in, tbl_Filter_TD_7740_output.nrow, 3);
    get_meta_info_JOIN_LEFTSEMI_TD_6755_gqe_join_build(cfg_JOIN_LEFTSEMI_TD_6755_cmds_build.meta_out, tbl_JOIN_LEFTSEMI_TD_6755_output.nrow, 4);
    // cfg_JOIN_LEFTSEMI_TD_6755_cmds_build.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_LEFTSEMI_TD_6755_cmds_probe;
    cfg_JOIN_LEFTSEMI_TD_6755_cmds_probe.setup(1);
    cfg_JOIN_LEFTSEMI_TD_6755_cmds_probe.allocateHost();
    get_cfg_dat_JOIN_LEFTSEMI_TD_6755_gqe_join_probe(krn_cmd_probe_799);
    cfg_JOIN_LEFTSEMI_TD_6755_cmds_probe.cmd = krn_cmd_probe_799.getConfigBits();
    get_meta_info_JOIN_LEFTSEMI_TD_6755_gqe_join_probe(cfg_JOIN_LEFTSEMI_TD_6755_cmds_probe.meta_in, tbl_JOIN_LEFTSEMI_TD_7524_consolidated_output.nrow, 3);
    get_meta_info_JOIN_LEFTSEMI_TD_6755_gqe_join_probe(cfg_JOIN_LEFTSEMI_TD_6755_cmds_probe.meta_out, tbl_JOIN_LEFTSEMI_TD_6755_output.nrow, 3);
    // cfg_JOIN_LEFTSEMI_TD_6755_cmds_probe.allocateDevBuffer(context_h, 32);
    cfg_JOIN_LEFTSEMI_TD_6755_cmds_build.input_key_columns = {0, -1, -1};
    cfg_JOIN_LEFTSEMI_TD_6755_cmds_probe.input_key_columns = {0, -1, -1};
    cfg_JOIN_LEFTSEMI_TD_6755_cmds_build.output_key_columns = {0, 1, 2, 3};
    cfg_JOIN_LEFTSEMI_TD_6755_cmds_probe.output_key_columns = {0, 1, 2, 3};
    
    xf::database::gqe::KernelCommand krn_cmd_build_490 = xf::database::gqe::KernelCommand();
    xf::database::gqe::KernelCommand krn_cmd_probe_490 = xf::database::gqe::KernelCommand();
    cfgCmd cfg_JOIN_LEFTSEMI_TD_7524_cmds_build;
    cfg_JOIN_LEFTSEMI_TD_7524_cmds_build.setup(1);
    cfg_JOIN_LEFTSEMI_TD_7524_cmds_build.allocateHost();
    get_cfg_dat_JOIN_LEFTSEMI_TD_7524_gqe_join_build(krn_cmd_build_490);
    cfg_JOIN_LEFTSEMI_TD_7524_cmds_build.cmd = krn_cmd_build_490.getConfigBits();
    get_meta_info_JOIN_LEFTSEMI_TD_7524_gqe_join_build(cfg_JOIN_LEFTSEMI_TD_7524_cmds_build.meta_in, tbl_SerializeFromObject_TD_8959_input.nrow, 3);
    get_meta_info_JOIN_LEFTSEMI_TD_7524_gqe_join_build(cfg_JOIN_LEFTSEMI_TD_7524_cmds_build.meta_out, tbl_JOIN_LEFTSEMI_TD_7524_output.nrow, 4);
    // cfg_JOIN_LEFTSEMI_TD_7524_cmds_build.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_LEFTSEMI_TD_7524_cmds_probe;
    cfg_JOIN_LEFTSEMI_TD_7524_cmds_probe.setup(1);
    cfg_JOIN_LEFTSEMI_TD_7524_cmds_probe.allocateHost();
    get_cfg_dat_JOIN_LEFTSEMI_TD_7524_gqe_join_probe(krn_cmd_probe_490);
    cfg_JOIN_LEFTSEMI_TD_7524_cmds_probe.cmd = krn_cmd_probe_490.getConfigBits();
    get_meta_info_JOIN_LEFTSEMI_TD_7524_gqe_join_probe(cfg_JOIN_LEFTSEMI_TD_7524_cmds_probe.meta_in, tbl_Filter_TD_8737_output.nrow, 3);
    get_meta_info_JOIN_LEFTSEMI_TD_7524_gqe_join_probe(cfg_JOIN_LEFTSEMI_TD_7524_cmds_probe.meta_out, tbl_JOIN_LEFTSEMI_TD_7524_output.nrow, 3);
    // cfg_JOIN_LEFTSEMI_TD_7524_cmds_probe.allocateDevBuffer(context_h, 32);
    cfg_JOIN_LEFTSEMI_TD_7524_cmds_build.input_key_columns = {0, -1, -1};
    cfg_JOIN_LEFTSEMI_TD_7524_cmds_probe.input_key_columns = {0, -1, -1};
    cfg_JOIN_LEFTSEMI_TD_7524_cmds_build.output_key_columns = {0, 1, 2, 3};
    cfg_JOIN_LEFTSEMI_TD_7524_cmds_probe.output_key_columns = {0, 1, 2, 3};
    // *************************** Kernel Setup ************************* // 
    bufferTmp buftmp_h(context_h, 1); 
    buftmp_h.initBuffer(q_h); 
    std::cout << std::endl; 
    krnlEngine krnl_JOIN_INNER_TD_2757_build;
    krnl_JOIN_INNER_TD_2757_build = krnlEngine(program_h, q_h, context_h, "gqeKernel", 0);
    krnl_JOIN_INNER_TD_2757_build.setup(tbl_JOIN_INNER_TD_4586_consolidated_output, tbl_Filter_TD_3451_output, tbl_JOIN_INNER_TD_2757_output, cfg_JOIN_INNER_TD_2757_cmds_build, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_2757_probe;
    krnl_JOIN_INNER_TD_2757_probe = krnlEngine(program_h, q_h, context_h, "gqeKernel", 1);
    krnl_JOIN_INNER_TD_2757_probe.setup(tbl_Filter_TD_3451_output, tbl_JOIN_INNER_TD_4586_consolidated_output, tbl_JOIN_INNER_TD_2757_output, cfg_JOIN_INNER_TD_2757_cmds_probe, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_4586_build;
    krnl_JOIN_INNER_TD_4586_build = krnlEngine(program_h, q_h, context_h, "gqeKernel", 0);
    krnl_JOIN_INNER_TD_4586_build.setup(tbl_JOIN_INNER_TD_3454_consolidated_output, tbl_JOIN_LEFTANTI_TD_5488_consolidated_output, tbl_JOIN_INNER_TD_4586_output, cfg_JOIN_INNER_TD_4586_cmds_build, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_4586_probe;
    krnl_JOIN_INNER_TD_4586_probe = krnlEngine(program_h, q_h, context_h, "gqeKernel", 1);
    krnl_JOIN_INNER_TD_4586_probe.setup(tbl_JOIN_LEFTANTI_TD_5488_consolidated_output, tbl_JOIN_INNER_TD_3454_consolidated_output, tbl_JOIN_INNER_TD_4586_output, cfg_JOIN_INNER_TD_4586_cmds_probe, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_3454_build;
    krnl_JOIN_INNER_TD_3454_build = krnlEngine(program_h, q_h, context_h, "gqeKernel", 0);
    krnl_JOIN_INNER_TD_3454_build.setup(tbl_SerializeFromObject_TD_525_input_stringRowIDSubstitute, tbl_Filter_TD_3765_output, tbl_JOIN_INNER_TD_3454_output, cfg_JOIN_INNER_TD_3454_cmds_build, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_3454_probe;
    krnl_JOIN_INNER_TD_3454_probe = krnlEngine(program_h, q_h, context_h, "gqeKernel", 1);
    krnl_JOIN_INNER_TD_3454_probe.setup(tbl_Filter_TD_3765_output, tbl_SerializeFromObject_TD_525_input_stringRowIDSubstitute, tbl_JOIN_INNER_TD_3454_output, cfg_JOIN_INNER_TD_3454_cmds_probe, buftmp_h);
    krnlEngine krnl_JOIN_LEFTANTI_TD_5488_build;
    krnl_JOIN_LEFTANTI_TD_5488_build = krnlEngine(program_h, q_h, context_h, "gqeKernel", 0);
    krnl_JOIN_LEFTANTI_TD_5488_build.setup(tbl_JOIN_LEFTSEMI_TD_6755_consolidated_output, tbl_Filter_TD_7740_output, tbl_JOIN_LEFTANTI_TD_5488_output, cfg_JOIN_LEFTANTI_TD_5488_cmds_build, buftmp_h);
    krnlEngine krnl_JOIN_LEFTANTI_TD_5488_probe;
    krnl_JOIN_LEFTANTI_TD_5488_probe = krnlEngine(program_h, q_h, context_h, "gqeKernel", 1);
    krnl_JOIN_LEFTANTI_TD_5488_probe.setup(tbl_Filter_TD_7740_output, tbl_JOIN_LEFTSEMI_TD_6755_consolidated_output, tbl_JOIN_LEFTANTI_TD_5488_output, cfg_JOIN_LEFTANTI_TD_5488_cmds_probe, buftmp_h);
    krnlEngine krnl_JOIN_LEFTSEMI_TD_6755_build;
    krnl_JOIN_LEFTSEMI_TD_6755_build = krnlEngine(program_h, q_h, context_h, "gqeKernel", 0);
    krnl_JOIN_LEFTSEMI_TD_6755_build.setup(tbl_Filter_TD_7740_output, tbl_JOIN_LEFTSEMI_TD_7524_consolidated_output, tbl_JOIN_LEFTSEMI_TD_6755_output, cfg_JOIN_LEFTSEMI_TD_6755_cmds_build, buftmp_h);
    krnlEngine krnl_JOIN_LEFTSEMI_TD_6755_probe;
    krnl_JOIN_LEFTSEMI_TD_6755_probe = krnlEngine(program_h, q_h, context_h, "gqeKernel", 1);
    krnl_JOIN_LEFTSEMI_TD_6755_probe.setup(tbl_JOIN_LEFTSEMI_TD_7524_consolidated_output, tbl_Filter_TD_7740_output, tbl_JOIN_LEFTSEMI_TD_6755_output, cfg_JOIN_LEFTSEMI_TD_6755_cmds_probe, buftmp_h);
    krnlEngine krnl_JOIN_LEFTSEMI_TD_7524_build;
    krnl_JOIN_LEFTSEMI_TD_7524_build = krnlEngine(program_h, q_h, context_h, "gqeKernel", 0);
    krnl_JOIN_LEFTSEMI_TD_7524_build.setup(tbl_SerializeFromObject_TD_8959_input, tbl_Filter_TD_8737_output, tbl_JOIN_LEFTSEMI_TD_7524_output, cfg_JOIN_LEFTSEMI_TD_7524_cmds_build, buftmp_h);
    krnlEngine krnl_JOIN_LEFTSEMI_TD_7524_probe;
    krnl_JOIN_LEFTSEMI_TD_7524_probe = krnlEngine(program_h, q_h, context_h, "gqeKernel", 1);
    krnl_JOIN_LEFTSEMI_TD_7524_probe.setup(tbl_Filter_TD_8737_output, tbl_SerializeFromObject_TD_8959_input, tbl_JOIN_LEFTSEMI_TD_7524_output, cfg_JOIN_LEFTSEMI_TD_7524_cmds_probe, buftmp_h);
    // ************************** Transfer Engine *********************** // 
    transEngine trans_JOIN_INNER_TD_2757;
    trans_JOIN_INNER_TD_2757.setq(q_h,1);
    trans_JOIN_INNER_TD_2757.add(&(cfg_JOIN_INNER_TD_2757_cmds_build));
    trans_JOIN_INNER_TD_2757.add(&(cfg_JOIN_INNER_TD_2757_cmds_probe));
    transEngine trans_JOIN_INNER_TD_2757_out;
    trans_JOIN_INNER_TD_2757_out.setq(q_h,1);
    q_h.finish();
    transEngine trans_JOIN_INNER_TD_4586;
    trans_JOIN_INNER_TD_4586.setq(q_h,1);
    trans_JOIN_INNER_TD_4586.add(&(cfg_JOIN_INNER_TD_4586_cmds_build));
    trans_JOIN_INNER_TD_4586.add(&(cfg_JOIN_INNER_TD_4586_cmds_probe));
    transEngine trans_JOIN_INNER_TD_4586_out;
    trans_JOIN_INNER_TD_4586_out.setq(q_h,1);
    q_h.finish();
    transEngine trans_JOIN_INNER_TD_3454;
    trans_JOIN_INNER_TD_3454.setq(q_h,1);
    trans_JOIN_INNER_TD_3454.add(&(cfg_JOIN_INNER_TD_3454_cmds_build));
    trans_JOIN_INNER_TD_3454.add(&(cfg_JOIN_INNER_TD_3454_cmds_probe));
    trans_JOIN_INNER_TD_3454.add(&(tbl_SerializeFromObject_TD_525_input_stringRowIDSubstitute));
    transEngine trans_JOIN_INNER_TD_3454_out;
    trans_JOIN_INNER_TD_3454_out.setq(q_h,1);
    q_h.finish();
    transEngine trans_JOIN_LEFTANTI_TD_5488;
    trans_JOIN_LEFTANTI_TD_5488.setq(q_h,1);
    trans_JOIN_LEFTANTI_TD_5488.add(&(cfg_JOIN_LEFTANTI_TD_5488_cmds_build));
    trans_JOIN_LEFTANTI_TD_5488.add(&(cfg_JOIN_LEFTANTI_TD_5488_cmds_probe));
    transEngine trans_JOIN_LEFTANTI_TD_5488_out;
    trans_JOIN_LEFTANTI_TD_5488_out.setq(q_h,1);
    q_h.finish();
    transEngine trans_JOIN_LEFTSEMI_TD_6755;
    trans_JOIN_LEFTSEMI_TD_6755.setq(q_h,1);
    trans_JOIN_LEFTSEMI_TD_6755.add(&(cfg_JOIN_LEFTSEMI_TD_6755_cmds_build));
    trans_JOIN_LEFTSEMI_TD_6755.add(&(cfg_JOIN_LEFTSEMI_TD_6755_cmds_probe));
    transEngine trans_JOIN_LEFTSEMI_TD_6755_out;
    trans_JOIN_LEFTSEMI_TD_6755_out.setq(q_h,1);
    q_h.finish();
    transEngine trans_JOIN_LEFTSEMI_TD_7524;
    trans_JOIN_LEFTSEMI_TD_7524.setq(q_h,1);
    trans_JOIN_LEFTSEMI_TD_7524.add(&(cfg_JOIN_LEFTSEMI_TD_7524_cmds_build));
    trans_JOIN_LEFTSEMI_TD_7524.add(&(cfg_JOIN_LEFTSEMI_TD_7524_cmds_probe));
    trans_JOIN_LEFTSEMI_TD_7524.add(&(tbl_SerializeFromObject_TD_8959_input));
    transEngine trans_JOIN_LEFTSEMI_TD_7524_out;
    trans_JOIN_LEFTSEMI_TD_7524_out.setq(q_h,1);
    q_h.finish();
    // ****************************** Events **************************** // 
    std::vector<cl::Event> events_h2d_wr_JOIN_INNER_TD_2757;
    std::vector<cl::Event> events_d2h_rd_JOIN_INNER_TD_2757;
    std::vector<cl::Event> events_JOIN_INNER_TD_2757;
    events_h2d_wr_JOIN_INNER_TD_2757.resize(1);
    events_d2h_rd_JOIN_INNER_TD_2757.resize(1);
    events_JOIN_INNER_TD_2757.resize(2);
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_2757;
    std::vector<cl::Event> prev_events_grp_JOIN_INNER_TD_2757;
    std::vector<cl::Event> events_h2d_wr_JOIN_INNER_TD_4586;
    std::vector<cl::Event> events_d2h_rd_JOIN_INNER_TD_4586;
    std::vector<cl::Event> events_JOIN_INNER_TD_4586;
    events_h2d_wr_JOIN_INNER_TD_4586.resize(1);
    events_d2h_rd_JOIN_INNER_TD_4586.resize(1);
    events_JOIN_INNER_TD_4586.resize(2);
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_4586;
    std::vector<cl::Event> prev_events_grp_JOIN_INNER_TD_4586;
    std::vector<cl::Event> events_h2d_wr_JOIN_INNER_TD_3454;
    std::vector<cl::Event> events_d2h_rd_JOIN_INNER_TD_3454;
    std::vector<cl::Event> events_JOIN_INNER_TD_3454;
    events_h2d_wr_JOIN_INNER_TD_3454.resize(1);
    events_d2h_rd_JOIN_INNER_TD_3454.resize(1);
    events_JOIN_INNER_TD_3454.resize(2);
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_3454;
    std::vector<cl::Event> prev_events_grp_JOIN_INNER_TD_3454;
    std::vector<cl::Event> events_h2d_wr_JOIN_LEFTANTI_TD_5488;
    std::vector<cl::Event> events_d2h_rd_JOIN_LEFTANTI_TD_5488;
    std::vector<cl::Event> events_JOIN_LEFTANTI_TD_5488;
    events_h2d_wr_JOIN_LEFTANTI_TD_5488.resize(1);
    events_d2h_rd_JOIN_LEFTANTI_TD_5488.resize(1);
    events_JOIN_LEFTANTI_TD_5488.resize(2);
    std::vector<cl::Event> events_grp_JOIN_LEFTANTI_TD_5488;
    std::vector<cl::Event> prev_events_grp_JOIN_LEFTANTI_TD_5488;
    std::vector<cl::Event> events_h2d_wr_JOIN_LEFTSEMI_TD_6755;
    std::vector<cl::Event> events_d2h_rd_JOIN_LEFTSEMI_TD_6755;
    std::vector<cl::Event> events_JOIN_LEFTSEMI_TD_6755;
    events_h2d_wr_JOIN_LEFTSEMI_TD_6755.resize(1);
    events_d2h_rd_JOIN_LEFTSEMI_TD_6755.resize(1);
    events_JOIN_LEFTSEMI_TD_6755.resize(2);
    std::vector<cl::Event> events_grp_JOIN_LEFTSEMI_TD_6755;
    std::vector<cl::Event> prev_events_grp_JOIN_LEFTSEMI_TD_6755;
    std::vector<cl::Event> events_h2d_wr_JOIN_LEFTSEMI_TD_7524;
    std::vector<cl::Event> events_d2h_rd_JOIN_LEFTSEMI_TD_7524;
    std::vector<cl::Event> events_JOIN_LEFTSEMI_TD_7524;
    events_h2d_wr_JOIN_LEFTSEMI_TD_7524.resize(1);
    events_d2h_rd_JOIN_LEFTSEMI_TD_7524.resize(1);
    events_JOIN_LEFTSEMI_TD_7524.resize(2);
    std::vector<cl::Event> events_grp_JOIN_LEFTSEMI_TD_7524;
    std::vector<cl::Event> prev_events_grp_JOIN_LEFTSEMI_TD_7524;
    // **************************** Operations ************************** // 
    struct timeval tv_r_s, tv_r_e; 
    gettimeofday(&tv_r_s, 0); 

    struct timeval tv_r_Filter_8_88_s, tv_r_Filter_8_88_e;
    gettimeofday(&tv_r_Filter_8_88_s, 0);
    SW_Filter_TD_8737(tbl_SerializeFromObject_TD_9761_input, tbl_Filter_TD_8737_output);
    gettimeofday(&tv_r_Filter_8_88_e, 0);

    struct timeval tv_r_Filter_7_504_s, tv_r_Filter_7_504_e;
    gettimeofday(&tv_r_Filter_7_504_s, 0);
    SW_Filter_TD_7740(tbl_SerializeFromObject_TD_9761_input, tbl_Filter_TD_7740_output);
    gettimeofday(&tv_r_Filter_7_504_e, 0);

    struct timeval tv_r_JOIN_LEFTSEMI_7_139_s, tv_r_JOIN_LEFTSEMI_7_139_e;
    gettimeofday(&tv_r_JOIN_LEFTSEMI_7_139_s, 0);
    tbl_SerializeFromObject_TD_8959_input.tableToCol();
    tbl_Filter_TD_8737_output.tableToCol();
    krnl_JOIN_LEFTSEMI_TD_7524_build.updateMeta(tbl_SerializeFromObject_TD_8959_input.getNumRow(), 3);
    krnl_JOIN_LEFTSEMI_TD_7524_probe.updateMeta(tbl_Filter_TD_8737_output.getNumRow(), 3);
    trans_JOIN_LEFTSEMI_TD_7524.add(&(tbl_Filter_TD_8737_output));
    trans_JOIN_LEFTSEMI_TD_7524.host2dev(0, &(prev_events_grp_JOIN_LEFTSEMI_TD_7524), &(events_h2d_wr_JOIN_LEFTSEMI_TD_7524[0]));
    events_grp_JOIN_LEFTSEMI_TD_7524.push_back(events_h2d_wr_JOIN_LEFTSEMI_TD_7524[0]);
    krnl_JOIN_LEFTSEMI_TD_7524_build.run(0, &(events_grp_JOIN_LEFTSEMI_TD_7524), &(events_JOIN_LEFTSEMI_TD_7524[0]));
    std::vector<cl::Event> events_grp_JOIN_LEFTSEMI_TD_7524_build_done;
    events_grp_JOIN_LEFTSEMI_TD_7524_build_done.push_back(events_JOIN_LEFTSEMI_TD_7524[0]);
    krnl_JOIN_LEFTSEMI_TD_7524_probe.run(0, &(events_grp_JOIN_LEFTSEMI_TD_7524_build_done), &(events_JOIN_LEFTSEMI_TD_7524[1]));
    std::vector<cl::Event> events_grp_JOIN_LEFTSEMI_TD_7524_probe_done;
    events_grp_JOIN_LEFTSEMI_TD_7524_probe_done.push_back(events_JOIN_LEFTSEMI_TD_7524[1]);
    trans_JOIN_LEFTSEMI_TD_7524_out.add(&(tbl_JOIN_LEFTSEMI_TD_7524_output));
    trans_JOIN_LEFTSEMI_TD_7524_out.add(&(cfg_JOIN_LEFTSEMI_TD_7524_cmds_build));
    trans_JOIN_LEFTSEMI_TD_7524_out.add(&(cfg_JOIN_LEFTSEMI_TD_7524_cmds_probe));
    trans_JOIN_LEFTSEMI_TD_7524_out.dev2host(0, &(events_grp_JOIN_LEFTSEMI_TD_7524_probe_done), &(events_d2h_rd_JOIN_LEFTSEMI_TD_7524[0]));
    q_h.flush();
    q_h.finish();
    tbl_JOIN_LEFTSEMI_TD_7524_output.setNumRow((cfg_JOIN_LEFTSEMI_TD_7524_cmds_probe.meta_out)->getColLen());
    tbl_JOIN_LEFTSEMI_TD_7524_output.colToTable();
    SW_Consolidated_JOIN_LEFTSEMI_TD_7524_output(tbl_SerializeFromObject_TD_8959_input, tbl_Filter_TD_8737_output, tbl_JOIN_LEFTSEMI_TD_7524_output, tbl_JOIN_LEFTSEMI_TD_7524_consolidated_output);
    gettimeofday(&tv_r_JOIN_LEFTSEMI_7_139_e, 0);

    struct timeval tv_r_JOIN_LEFTSEMI_6_920_s, tv_r_JOIN_LEFTSEMI_6_920_e;
    gettimeofday(&tv_r_JOIN_LEFTSEMI_6_920_s, 0);
    prev_events_grp_JOIN_LEFTSEMI_TD_6755.push_back(events_h2d_wr_JOIN_LEFTSEMI_TD_7524[0]);
    tbl_Filter_TD_7740_output.tableToCol();
    tbl_JOIN_LEFTSEMI_TD_7524_consolidated_output.tableToCol();
    krnl_JOIN_LEFTSEMI_TD_6755_build.updateMeta(tbl_Filter_TD_7740_output.getNumRow(), 3);
    krnl_JOIN_LEFTSEMI_TD_6755_probe.updateMeta(tbl_JOIN_LEFTSEMI_TD_7524_consolidated_output.getNumRow(), 3);
    trans_JOIN_LEFTSEMI_TD_6755.add(&(tbl_Filter_TD_7740_output));
    trans_JOIN_LEFTSEMI_TD_6755.host2dev(0, &(prev_events_grp_JOIN_LEFTSEMI_TD_6755), &(events_h2d_wr_JOIN_LEFTSEMI_TD_6755[0]));
    events_grp_JOIN_LEFTSEMI_TD_6755.push_back(events_h2d_wr_JOIN_LEFTSEMI_TD_6755[0]);
    events_grp_JOIN_LEFTSEMI_TD_6755.push_back(events_JOIN_LEFTSEMI_TD_7524[0]);
    krnl_JOIN_LEFTSEMI_TD_6755_build.run(0, &(events_grp_JOIN_LEFTSEMI_TD_6755), &(events_JOIN_LEFTSEMI_TD_6755[0]));
    std::vector<cl::Event> events_grp_JOIN_LEFTSEMI_TD_6755_build_done;
    events_grp_JOIN_LEFTSEMI_TD_6755_build_done.push_back(events_JOIN_LEFTSEMI_TD_6755[0]);
    krnl_JOIN_LEFTSEMI_TD_6755_probe.run(0, &(events_grp_JOIN_LEFTSEMI_TD_6755_build_done), &(events_JOIN_LEFTSEMI_TD_6755[1]));
    std::vector<cl::Event> events_grp_JOIN_LEFTSEMI_TD_6755_probe_done;
    events_grp_JOIN_LEFTSEMI_TD_6755_probe_done.push_back(events_JOIN_LEFTSEMI_TD_6755[1]);
    trans_JOIN_LEFTSEMI_TD_6755_out.add(&(tbl_JOIN_LEFTSEMI_TD_6755_output));
    trans_JOIN_LEFTSEMI_TD_6755_out.add(&(cfg_JOIN_LEFTSEMI_TD_6755_cmds_build));
    trans_JOIN_LEFTSEMI_TD_6755_out.add(&(cfg_JOIN_LEFTSEMI_TD_6755_cmds_probe));
    trans_JOIN_LEFTSEMI_TD_6755_out.dev2host(0, &(events_grp_JOIN_LEFTSEMI_TD_6755_probe_done), &(events_d2h_rd_JOIN_LEFTSEMI_TD_6755[0]));
    q_h.flush();
    q_h.finish();
    tbl_JOIN_LEFTSEMI_TD_6755_output.setNumRow((cfg_JOIN_LEFTSEMI_TD_6755_cmds_probe.meta_out)->getColLen());
    tbl_JOIN_LEFTSEMI_TD_6755_output.colToTable();
    SW_Consolidated_JOIN_LEFTSEMI_TD_6755_output(tbl_Filter_TD_7740_output, tbl_JOIN_LEFTSEMI_TD_7524_consolidated_output, tbl_JOIN_LEFTSEMI_TD_6755_output, tbl_JOIN_LEFTSEMI_TD_6755_consolidated_output);
    gettimeofday(&tv_r_JOIN_LEFTSEMI_6_920_e, 0);

    struct timeval tv_r_Filter_3_356_s, tv_r_Filter_3_356_e;
    gettimeofday(&tv_r_Filter_3_356_s, 0);
    SW_Filter_TD_3765(tbl_SerializeFromObject_TD_4885_input, tbl_Filter_TD_3765_output);
    gettimeofday(&tv_r_Filter_3_356_e, 0);

    struct timeval tv_r_JOIN_LEFTANTI_5_354_s, tv_r_JOIN_LEFTANTI_5_354_e;
    gettimeofday(&tv_r_JOIN_LEFTANTI_5_354_s, 0);
    prev_events_grp_JOIN_LEFTANTI_TD_5488.push_back(events_h2d_wr_JOIN_LEFTSEMI_TD_6755[0]);
    tbl_JOIN_LEFTSEMI_TD_6755_consolidated_output.tableToCol();
    tbl_Filter_TD_7740_output.tableToCol();
    krnl_JOIN_LEFTANTI_TD_5488_build.updateMeta(tbl_JOIN_LEFTSEMI_TD_6755_consolidated_output.getNumRow(), 3);
    krnl_JOIN_LEFTANTI_TD_5488_probe.updateMeta(tbl_Filter_TD_7740_output.getNumRow(), 3);
    trans_JOIN_LEFTANTI_TD_5488.add(&(tbl_Filter_TD_7740_output));
    trans_JOIN_LEFTANTI_TD_5488.host2dev(0, &(prev_events_grp_JOIN_LEFTANTI_TD_5488), &(events_h2d_wr_JOIN_LEFTANTI_TD_5488[0]));
    events_grp_JOIN_LEFTANTI_TD_5488.push_back(events_h2d_wr_JOIN_LEFTANTI_TD_5488[0]);
    events_grp_JOIN_LEFTANTI_TD_5488.push_back(events_JOIN_LEFTSEMI_TD_6755[0]);
    krnl_JOIN_LEFTANTI_TD_5488_build.run(0, &(events_grp_JOIN_LEFTANTI_TD_5488), &(events_JOIN_LEFTANTI_TD_5488[0]));
    std::vector<cl::Event> events_grp_JOIN_LEFTANTI_TD_5488_build_done;
    events_grp_JOIN_LEFTANTI_TD_5488_build_done.push_back(events_JOIN_LEFTANTI_TD_5488[0]);
    krnl_JOIN_LEFTANTI_TD_5488_probe.run(0, &(events_grp_JOIN_LEFTANTI_TD_5488_build_done), &(events_JOIN_LEFTANTI_TD_5488[1]));
    std::vector<cl::Event> events_grp_JOIN_LEFTANTI_TD_5488_probe_done;
    events_grp_JOIN_LEFTANTI_TD_5488_probe_done.push_back(events_JOIN_LEFTANTI_TD_5488[1]);
    trans_JOIN_LEFTANTI_TD_5488_out.add(&(tbl_JOIN_LEFTANTI_TD_5488_output));
    trans_JOIN_LEFTANTI_TD_5488_out.add(&(cfg_JOIN_LEFTANTI_TD_5488_cmds_build));
    trans_JOIN_LEFTANTI_TD_5488_out.add(&(cfg_JOIN_LEFTANTI_TD_5488_cmds_probe));
    trans_JOIN_LEFTANTI_TD_5488_out.dev2host(0, &(events_grp_JOIN_LEFTANTI_TD_5488_probe_done), &(events_d2h_rd_JOIN_LEFTANTI_TD_5488[0]));
    q_h.flush();
    q_h.finish();
    tbl_JOIN_LEFTANTI_TD_5488_output.setNumRow((cfg_JOIN_LEFTANTI_TD_5488_cmds_probe.meta_out)->getColLen());
    tbl_JOIN_LEFTANTI_TD_5488_output.colToTable();
    SW_Consolidated_JOIN_LEFTANTI_TD_5488_output(tbl_JOIN_LEFTSEMI_TD_6755_consolidated_output, tbl_Filter_TD_7740_output, tbl_JOIN_LEFTANTI_TD_5488_output, tbl_JOIN_LEFTANTI_TD_5488_consolidated_output);
    gettimeofday(&tv_r_JOIN_LEFTANTI_5_354_e, 0);

    struct timeval tv_r_JOIN_INNER_3_53_s, tv_r_JOIN_INNER_3_53_e;
    gettimeofday(&tv_r_JOIN_INNER_3_53_s, 0);
    tbl_SerializeFromObject_TD_525_input_stringRowIDSubstitute.tableToCol();
    tbl_Filter_TD_3765_output.tableToCol();
    krnl_JOIN_INNER_TD_3454_build.updateMeta(tbl_SerializeFromObject_TD_525_input_stringRowIDSubstitute.getNumRow(), 3);
    krnl_JOIN_INNER_TD_3454_probe.updateMeta(tbl_Filter_TD_3765_output.getNumRow(), 3);
    trans_JOIN_INNER_TD_3454.add(&(tbl_Filter_TD_3765_output));
    trans_JOIN_INNER_TD_3454.host2dev(0, &(prev_events_grp_JOIN_INNER_TD_3454), &(events_h2d_wr_JOIN_INNER_TD_3454[0]));
    events_grp_JOIN_INNER_TD_3454.push_back(events_h2d_wr_JOIN_INNER_TD_3454[0]);
    krnl_JOIN_INNER_TD_3454_build.run(0, &(events_grp_JOIN_INNER_TD_3454), &(events_JOIN_INNER_TD_3454[0]));
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_3454_build_done;
    events_grp_JOIN_INNER_TD_3454_build_done.push_back(events_JOIN_INNER_TD_3454[0]);
    krnl_JOIN_INNER_TD_3454_probe.run(0, &(events_grp_JOIN_INNER_TD_3454_build_done), &(events_JOIN_INNER_TD_3454[1]));
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_3454_probe_done;
    events_grp_JOIN_INNER_TD_3454_probe_done.push_back(events_JOIN_INNER_TD_3454[1]);
    trans_JOIN_INNER_TD_3454_out.add(&(tbl_JOIN_INNER_TD_3454_output));
    trans_JOIN_INNER_TD_3454_out.add(&(cfg_JOIN_INNER_TD_3454_cmds_build));
    trans_JOIN_INNER_TD_3454_out.add(&(cfg_JOIN_INNER_TD_3454_cmds_probe));
    trans_JOIN_INNER_TD_3454_out.dev2host(0, &(events_grp_JOIN_INNER_TD_3454_probe_done), &(events_d2h_rd_JOIN_INNER_TD_3454[0]));
    q_h.flush();
    q_h.finish();
    tbl_JOIN_INNER_TD_3454_output.setNumRow((cfg_JOIN_INNER_TD_3454_cmds_probe.meta_out)->getColLen());
    tbl_JOIN_INNER_TD_3454_output.colToTable();
    SW_Consolidated_JOIN_INNER_TD_3454_output(tbl_SerializeFromObject_TD_525_input_stringRowIDSubstitute, tbl_Filter_TD_3765_output, tbl_JOIN_INNER_TD_3454_output, tbl_JOIN_INNER_TD_3454_consolidated_output);
    gettimeofday(&tv_r_JOIN_INNER_3_53_e, 0);

    struct timeval tv_r_JOIN_INNER_4_340_s, tv_r_JOIN_INNER_4_340_e;
    gettimeofday(&tv_r_JOIN_INNER_4_340_s, 0);
    prev_events_grp_JOIN_INNER_TD_4586.push_back(events_h2d_wr_JOIN_INNER_TD_3454[0]);
    prev_events_grp_JOIN_INNER_TD_4586.push_back(events_h2d_wr_JOIN_LEFTANTI_TD_5488[0]);
    tbl_JOIN_INNER_TD_3454_consolidated_output.tableToCol();
    tbl_JOIN_LEFTANTI_TD_5488_consolidated_output.tableToCol();
    krnl_JOIN_INNER_TD_4586_build.updateMeta(tbl_JOIN_INNER_TD_3454_consolidated_output.getNumRow(), 3);
    krnl_JOIN_INNER_TD_4586_probe.updateMeta(tbl_JOIN_LEFTANTI_TD_5488_consolidated_output.getNumRow(), 3);
    trans_JOIN_INNER_TD_4586.host2dev(0, &(prev_events_grp_JOIN_INNER_TD_4586), &(events_h2d_wr_JOIN_INNER_TD_4586[0]));
    events_grp_JOIN_INNER_TD_4586.push_back(events_h2d_wr_JOIN_INNER_TD_4586[0]);
    events_grp_JOIN_INNER_TD_4586.push_back(events_JOIN_INNER_TD_3454[0]);
    events_grp_JOIN_INNER_TD_4586.push_back(events_JOIN_LEFTANTI_TD_5488[0]);
    krnl_JOIN_INNER_TD_4586_build.run(0, &(events_grp_JOIN_INNER_TD_4586), &(events_JOIN_INNER_TD_4586[0]));
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_4586_build_done;
    events_grp_JOIN_INNER_TD_4586_build_done.push_back(events_JOIN_INNER_TD_4586[0]);
    krnl_JOIN_INNER_TD_4586_probe.run(0, &(events_grp_JOIN_INNER_TD_4586_build_done), &(events_JOIN_INNER_TD_4586[1]));
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_4586_probe_done;
    events_grp_JOIN_INNER_TD_4586_probe_done.push_back(events_JOIN_INNER_TD_4586[1]);
    trans_JOIN_INNER_TD_4586_out.add(&(tbl_JOIN_INNER_TD_4586_output));
    trans_JOIN_INNER_TD_4586_out.add(&(cfg_JOIN_INNER_TD_4586_cmds_build));
    trans_JOIN_INNER_TD_4586_out.add(&(cfg_JOIN_INNER_TD_4586_cmds_probe));
    trans_JOIN_INNER_TD_4586_out.dev2host(0, &(events_grp_JOIN_INNER_TD_4586_probe_done), &(events_d2h_rd_JOIN_INNER_TD_4586[0]));
    q_h.flush();
    q_h.finish();
    tbl_JOIN_INNER_TD_4586_output.setNumRow((cfg_JOIN_INNER_TD_4586_cmds_probe.meta_out)->getColLen());
    tbl_JOIN_INNER_TD_4586_output.colToTable();
    SW_Consolidated_JOIN_INNER_TD_4586_output(tbl_JOIN_INNER_TD_3454_consolidated_output, tbl_JOIN_LEFTANTI_TD_5488_consolidated_output, tbl_JOIN_INNER_TD_4586_output, tbl_JOIN_INNER_TD_4586_consolidated_output);
    gettimeofday(&tv_r_JOIN_INNER_4_340_e, 0);

    struct timeval tv_r_Filter_3_335_s, tv_r_Filter_3_335_e;
    gettimeofday(&tv_r_Filter_3_335_s, 0);
    SW_Filter_TD_3451(tbl_SerializeFromObject_TD_5445_input, tbl_Filter_TD_3451_output);
    gettimeofday(&tv_r_Filter_3_335_e, 0);

    struct timeval tv_r_JOIN_INNER_2_845_s, tv_r_JOIN_INNER_2_845_e;
    gettimeofday(&tv_r_JOIN_INNER_2_845_s, 0);
    prev_events_grp_JOIN_INNER_TD_2757.push_back(events_h2d_wr_JOIN_INNER_TD_4586[0]);
    tbl_JOIN_INNER_TD_4586_consolidated_output.tableToCol();
    tbl_Filter_TD_3451_output.tableToCol();
    krnl_JOIN_INNER_TD_2757_build.updateMeta(tbl_JOIN_INNER_TD_4586_consolidated_output.getNumRow(), 3);
    krnl_JOIN_INNER_TD_2757_probe.updateMeta(tbl_Filter_TD_3451_output.getNumRow(), 3);
    trans_JOIN_INNER_TD_2757.add(&(tbl_Filter_TD_3451_output));
    trans_JOIN_INNER_TD_2757.host2dev(0, &(prev_events_grp_JOIN_INNER_TD_2757), &(events_h2d_wr_JOIN_INNER_TD_2757[0]));
    events_grp_JOIN_INNER_TD_2757.push_back(events_h2d_wr_JOIN_INNER_TD_2757[0]);
    events_grp_JOIN_INNER_TD_2757.push_back(events_JOIN_INNER_TD_4586[0]);
    krnl_JOIN_INNER_TD_2757_build.run(0, &(events_grp_JOIN_INNER_TD_2757), &(events_JOIN_INNER_TD_2757[0]));
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_2757_build_done;
    events_grp_JOIN_INNER_TD_2757_build_done.push_back(events_JOIN_INNER_TD_2757[0]);
    krnl_JOIN_INNER_TD_2757_probe.run(0, &(events_grp_JOIN_INNER_TD_2757_build_done), &(events_JOIN_INNER_TD_2757[1]));
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_2757_probe_done;
    events_grp_JOIN_INNER_TD_2757_probe_done.push_back(events_JOIN_INNER_TD_2757[1]);
    trans_JOIN_INNER_TD_2757_out.add(&(tbl_JOIN_INNER_TD_2757_output));
    trans_JOIN_INNER_TD_2757_out.add(&(cfg_JOIN_INNER_TD_2757_cmds_build));
    trans_JOIN_INNER_TD_2757_out.add(&(cfg_JOIN_INNER_TD_2757_cmds_probe));
    trans_JOIN_INNER_TD_2757_out.dev2host(0, &(events_grp_JOIN_INNER_TD_2757_probe_done), &(events_d2h_rd_JOIN_INNER_TD_2757[0]));
    q_h.flush();
    q_h.finish();
    tbl_JOIN_INNER_TD_2757_output.setNumRow((cfg_JOIN_INNER_TD_2757_cmds_probe.meta_out)->getColLen());
    tbl_JOIN_INNER_TD_2757_output.colToTable();
    SW_Consolidated_JOIN_INNER_TD_2757_output(tbl_JOIN_INNER_TD_4586_consolidated_output, tbl_Filter_TD_3451_output, tbl_JOIN_INNER_TD_2757_output, tbl_JOIN_INNER_TD_2757_consolidated_output);
    gettimeofday(&tv_r_JOIN_INNER_2_845_e, 0);

    struct timeval tv_r_Aggregate_1_885_s, tv_r_Aggregate_1_885_e;
    gettimeofday(&tv_r_Aggregate_1_885_s, 0);
    SW_Aggregate_TD_1429(tbl_JOIN_INNER_TD_2757_consolidated_output, tbl_SerializeFromObject_TD_5445_input, tbl_SerializeFromObject_TD_525_input, tbl_SerializeFromObject_TD_4885_input, tbl_Aggregate_TD_1429_output);
    gettimeofday(&tv_r_Aggregate_1_885_e, 0);

    struct timeval tv_r_Sort_0_209_s, tv_r_Sort_0_209_e;
    gettimeofday(&tv_r_Sort_0_209_s, 0);
    SW_Sort_TD_0156(tbl_Aggregate_TD_1429_output, tbl_Sort_TD_0156_output);
    gettimeofday(&tv_r_Sort_0_209_e, 0);

std::cout << "CPU TIME: " << (tvdiff(&tv_r_Filter_8_88_s, &tv_r_Filter_8_88_e) + 
tvdiff(&tv_r_Filter_7_504_s, &tv_r_Filter_7_504_e) + 
tvdiff(&tv_r_Filter_3_356_s, &tv_r_Filter_3_356_e) + 
tvdiff(&tv_r_Filter_3_335_s, &tv_r_Filter_3_335_e) + 
tvdiff(&tv_r_Aggregate_1_885_s, &tv_r_Aggregate_1_885_e) + 
tvdiff(&tv_r_Sort_0_209_s, &tv_r_Sort_0_209_e) + 
1) / 1000.0 << std::endl;

std::cout << "FPGA TIME: " << (tvdiff(&tv_r_JOIN_LEFTSEMI_7_139_s, &tv_r_JOIN_LEFTSEMI_7_139_e) + 
tvdiff(&tv_r_JOIN_LEFTSEMI_6_920_s, &tv_r_JOIN_LEFTSEMI_6_920_e) + 
tvdiff(&tv_r_JOIN_LEFTANTI_5_354_s, &tv_r_JOIN_LEFTANTI_5_354_e) + 
tvdiff(&tv_r_JOIN_INNER_3_53_s, &tv_r_JOIN_INNER_3_53_e) + 
tvdiff(&tv_r_JOIN_INNER_4_340_s, &tv_r_JOIN_INNER_4_340_e) + 
tvdiff(&tv_r_JOIN_INNER_2_845_s, &tv_r_JOIN_INNER_2_845_e) + 
1) / 1000.0 << std::endl;

    gettimeofday(&tv_r_e, 0); 
    // **************************** Print Execution Time ************************** // 
    std::cout << std::endl; 
    std::cout << "Filter_8: " << tvdiff(&tv_r_Filter_8_88_s, &tv_r_Filter_8_88_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer((l_receiptdate#89 > l_commitdate#88)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_9761_input.getNumRow() << " -> tbl_SerializeFromObject_TD_9761_input" 
     << std::endl << "      #COL: 4: " << "ListBuffer(l_orderkey#77, l_suppkey#79, l_receiptdate#89, l_commitdate#88)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_8737_output.getNumRow() << " -> tbl_Filter_TD_8737_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(l_orderkey#77, l_suppkey#79)" 
     << std::endl; 

    std::cout << "Filter_7: " << tvdiff(&tv_r_Filter_7_504_s, &tv_r_Filter_7_504_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer((l_receiptdate#89 > l_commitdate#88)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_9761_input.getNumRow() << " -> tbl_SerializeFromObject_TD_9761_input" 
     << std::endl << "      #COL: 4: " << "ListBuffer(l_orderkey#77, l_suppkey#79, l_receiptdate#89, l_commitdate#88)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_7740_output.getNumRow() << " -> tbl_Filter_TD_7740_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(l_orderkey#77#2013, l_suppkey#79#2015)" 
     << std::endl; 

    std::cout << "JOIN_LEFTSEMI_7: " << tvdiff(&tv_r_JOIN_LEFTSEMI_7_139_s, &tv_r_JOIN_LEFTSEMI_7_139_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer(((l_orderkey#77#1997 = l_orderkey#77) AND NOT (l_suppkey#79#1999 = l_suppkey#79))) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_8737_output.getNumRow() << " -> tbl_Filter_TD_8737_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(l_orderkey#77, l_suppkey#79)" 
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_8959_input.getNumRow() << " -> tbl_SerializeFromObject_TD_8959_input" 
     << std::endl << "      #COL: 2: " << "ListBuffer(l_orderkey#77#1997, l_suppkey#79#1999)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_LEFTSEMI_TD_7524_consolidated_output.getNumRow() << " -> tbl_JOIN_LEFTSEMI_TD_7524_consolidated_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(l_orderkey#77, l_suppkey#79)" 
     << std::endl; 

    std::cout << "JOIN_LEFTSEMI_6: " << tvdiff(&tv_r_JOIN_LEFTSEMI_6_920_s, &tv_r_JOIN_LEFTSEMI_6_920_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer(((l_orderkey#77#2013 = l_orderkey#77) AND NOT (l_suppkey#79#2015 = l_suppkey#79))) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_LEFTSEMI_TD_7524_consolidated_output.getNumRow() << " -> tbl_JOIN_LEFTSEMI_TD_7524_consolidated_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(l_orderkey#77, l_suppkey#79)" 
     << std::endl << "      #ROW: " << tbl_Filter_TD_7740_output.getNumRow() << " -> tbl_Filter_TD_7740_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(l_orderkey#77#2013, l_suppkey#79#2015)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_LEFTSEMI_TD_6755_consolidated_output.getNumRow() << " -> tbl_JOIN_LEFTSEMI_TD_6755_consolidated_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(l_orderkey#77, l_suppkey#79)" 
     << std::endl; 

    std::cout << "Filter_3: " << tvdiff(&tv_r_Filter_3_356_s, &tv_r_Filter_3_356_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer((isnotnull(n_name#145) AND (n_name#145 = JAPAN))) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_4885_input.getNumRow() << " -> tbl_SerializeFromObject_TD_4885_input" 
     << std::endl << "      #COL: 2: " << "ListBuffer(n_nationkey#144, n_name#145)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_3765_output.getNumRow() << " -> tbl_Filter_TD_3765_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(n_nationkey#144)" 
     << std::endl; 

    std::cout << "JOIN_LEFTANTI_5: " << tvdiff(&tv_r_JOIN_LEFTANTI_5_354_s, &tv_r_JOIN_LEFTANTI_5_354_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer(((l_orderkey#77#2013 = l_orderkey#77) AND NOT (l_suppkey#79#2015 = l_suppkey#79))) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_7740_output.getNumRow() << " -> tbl_Filter_TD_7740_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(l_orderkey#77#2013, l_suppkey#79#2015)" 
     << std::endl << "      #ROW: " << tbl_JOIN_LEFTSEMI_TD_6755_consolidated_output.getNumRow() << " -> tbl_JOIN_LEFTSEMI_TD_6755_consolidated_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(l_orderkey#77, l_suppkey#79)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_LEFTANTI_TD_5488_consolidated_output.getNumRow() << " -> tbl_JOIN_LEFTANTI_TD_5488_consolidated_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(l_orderkey#77, l_suppkey#79)" 
     << std::endl; 

    std::cout << "JOIN_INNER_3: " << tvdiff(&tv_r_JOIN_INNER_3_53_s, &tv_r_JOIN_INNER_3_53_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer((s_nationkey#345 = n_nationkey#144)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_525_input_stringRowIDSubstitute.getNumRow() << " -> tbl_SerializeFromObject_TD_525_input_stringRowIDSubstitute" 
     << std::endl << "      #COL: 3: " << "ListBuffer(s_suppkey#342, s_name#343, s_nationkey#345)" 
     << std::endl << "      #ROW: " << tbl_Filter_TD_3765_output.getNumRow() << " -> tbl_Filter_TD_3765_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(n_nationkey#144)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_3454_consolidated_output.getNumRow() << " -> tbl_JOIN_INNER_TD_3454_consolidated_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(s_suppkey#342, s_name#343)" 
     << std::endl; 

    std::cout << "JOIN_INNER_4: " << tvdiff(&tv_r_JOIN_INNER_4_340_s, &tv_r_JOIN_INNER_4_340_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer((s_suppkey#342 = l_suppkey#79)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_3454_consolidated_output.getNumRow() << " -> tbl_JOIN_INNER_TD_3454_consolidated_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(s_suppkey#342, s_name#343)" 
     << std::endl << "      #ROW: " << tbl_JOIN_LEFTANTI_TD_5488_consolidated_output.getNumRow() << " -> tbl_JOIN_LEFTANTI_TD_5488_consolidated_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(l_orderkey#77, l_suppkey#79)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_4586_consolidated_output.getNumRow() << " -> tbl_JOIN_INNER_TD_4586_consolidated_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(s_name#343, l_orderkey#77)" 
     << std::endl; 

    std::cout << "Filter_3: " << tvdiff(&tv_r_Filter_3_335_s, &tv_r_Filter_3_335_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer((o_orderstatus#209 = 70)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_5445_input.getNumRow() << " -> tbl_SerializeFromObject_TD_5445_input" 
     << std::endl << "      #COL: 2: " << "ListBuffer(o_orderkey#207, o_orderstatus#209)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_3451_output.getNumRow() << " -> tbl_Filter_TD_3451_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(o_orderkey#207)" 
     << std::endl; 

    std::cout << "JOIN_INNER_2: " << tvdiff(&tv_r_JOIN_INNER_2_845_s, &tv_r_JOIN_INNER_2_845_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer((o_orderkey#207 = l_orderkey#77)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_3451_output.getNumRow() << " -> tbl_Filter_TD_3451_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(o_orderkey#207)" 
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_4586_consolidated_output.getNumRow() << " -> tbl_JOIN_INNER_TD_4586_consolidated_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(s_name#343, l_orderkey#77)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_2757_consolidated_output.getNumRow() << " -> tbl_JOIN_INNER_TD_2757_consolidated_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(s_name#343)" 
     << std::endl; 

    std::cout << "Aggregate_1: " << tvdiff(&tv_r_Aggregate_1_885_s, &tv_r_Aggregate_1_885_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(s_name#343, count(1) AS numwait#1900L) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_2757_consolidated_output.getNumRow() << " -> tbl_JOIN_INNER_TD_2757_consolidated_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(s_name#343)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_1429_output.getNumRow() << " -> tbl_Aggregate_TD_1429_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(s_name#343, numwait#1900L)" 
     << std::endl; 

    std::cout << "Sort_0: " << tvdiff(&tv_r_Sort_0_209_s, &tv_r_Sort_0_209_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(numwait#1900L DESC NULLS LAST, s_name#343 ASC NULLS FIRST) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_1429_output.getNumRow() << " -> tbl_Aggregate_TD_1429_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(s_name#343, numwait#1900L)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Sort_TD_0156_output.getNumRow() << " -> tbl_Sort_TD_0156_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(s_name#343, numwait#1900L)" 
     << std::endl; 

    std::cout << std::endl << " Total execution time: " << tvdiff(&tv_r_s, &tv_r_e) / 1000 << " ms"; 

    std::cout << std::endl << " Spark elapsed time: " << 68.08125 * 1000 << "ms" << std::endl; 
    return 0; 
}
