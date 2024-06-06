// number of overlays (w/o fusion): 10 
// number of overlays (w/ fusion): 10 
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

#include "cfgFunc_q2.hpp" 
#include "q2.hpp" 

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
    std::cout << "NOTE:running query #2\n."; 
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
    Table tbl_Sort_TD_0943_output("tbl_Sort_TD_0943_output", 6100000, 8, "");
    tbl_Sort_TD_0943_output.allocateHost();
    Table tbl_JOIN_INNER_TD_1483_output("tbl_JOIN_INNER_TD_1483_output", 6100000, 8, "");
    tbl_JOIN_INNER_TD_1483_output.allocateHost();
    Table tbl_JOIN_INNER_TD_2465_output("tbl_JOIN_INNER_TD_2465_output", 6100000, 9, "");
    tbl_JOIN_INNER_TD_2465_output.allocateHost();
    Table tbl_JOIN_INNER_TD_3352_output("tbl_JOIN_INNER_TD_3352_output", -1, 4, "");
    tbl_JOIN_INNER_TD_3352_output.selectOverlayVersion(1);
    tbl_JOIN_INNER_TD_3352_output.allocateHost();
    Table tbl_JOIN_INNER_TD_3352_consolidated_output("tbl_JOIN_INNER_TD_3352_consolidated_output", -1, 8, "");
    tbl_JOIN_INNER_TD_3352_consolidated_output.selectOverlayVersion(1);
    tbl_JOIN_INNER_TD_3352_consolidated_output.allocateHost();
    Table tbl_SerializeFromObject_TD_373_input_stringRowIDSubstitute;
    tbl_SerializeFromObject_TD_373_input_stringRowIDSubstitute = Table("nation", nation_n, 3, in_dir);
    tbl_SerializeFromObject_TD_373_input_stringRowIDSubstitute.addCol("n_nationkey", 4);
    tbl_SerializeFromObject_TD_373_input_stringRowIDSubstitute.addCol("n_name", 4, 1, 0);
    tbl_SerializeFromObject_TD_373_input_stringRowIDSubstitute.addCol("n_regionkey", 4);
    tbl_SerializeFromObject_TD_373_input_stringRowIDSubstitute.allocateHost();
    tbl_SerializeFromObject_TD_373_input_stringRowIDSubstitute.loadHost();
    Table tbl_SerializeFromObject_TD_373_input;
    tbl_SerializeFromObject_TD_373_input = Table("nation", nation_n, 3, in_dir);
    tbl_SerializeFromObject_TD_373_input.addCol("n_nationkey", 4);
    tbl_SerializeFromObject_TD_373_input.addCol("n_name", TPCH_READ_NATION_LEN+1);
    tbl_SerializeFromObject_TD_373_input.addCol("n_regionkey", 4);
    tbl_SerializeFromObject_TD_373_input.allocateHost();
    tbl_SerializeFromObject_TD_373_input.loadHost();
    Table tbl_JOIN_INNER_TD_4452_output("tbl_JOIN_INNER_TD_4452_output", -1, 4, "");
    tbl_JOIN_INNER_TD_4452_output.selectOverlayVersion(1);
    tbl_JOIN_INNER_TD_4452_output.allocateHost();
    Table tbl_JOIN_INNER_TD_4452_consolidated_output("tbl_JOIN_INNER_TD_4452_consolidated_output", -1, 3, "");
    tbl_JOIN_INNER_TD_4452_consolidated_output.selectOverlayVersion(1);
    tbl_JOIN_INNER_TD_4452_consolidated_output.allocateHost();
    Table tbl_Aggregate_TD_6224_output("tbl_Aggregate_TD_6224_output", 6100000, 2, "");
    tbl_Aggregate_TD_6224_output.selectOverlayVersion(0);
    tbl_Aggregate_TD_6224_output.allocateHost();
    Table tbl_JOIN_INNER_TD_5425_output("tbl_JOIN_INNER_TD_5425_output", 628, 4, "");
    tbl_JOIN_INNER_TD_5425_output.selectOverlayVersion(1);
    tbl_JOIN_INNER_TD_5425_output.allocateHost();
    Table tbl_JOIN_INNER_TD_5425_consolidated_output("tbl_JOIN_INNER_TD_5425_consolidated_output", 628, 5, "");
    tbl_JOIN_INNER_TD_5425_consolidated_output.selectOverlayVersion(1);
    tbl_JOIN_INNER_TD_5425_consolidated_output.allocateHost();
    Table tbl_Filter_TD_6465_output("tbl_Filter_TD_6465_output", 6100000, 2, "");
    tbl_Filter_TD_6465_output.selectOverlayVersion(1);
    tbl_Filter_TD_6465_output.allocateHost();
    Table tbl_JOIN_INNER_TD_9897_output("tbl_JOIN_INNER_TD_9897_output", 162880, 4, "");
    tbl_JOIN_INNER_TD_9897_output.selectOverlayVersion(1);
    tbl_JOIN_INNER_TD_9897_output.allocateHost();
    Table tbl_JOIN_INNER_TD_9897_consolidated_output("tbl_JOIN_INNER_TD_9897_consolidated_output", 162880, 3, "");
    tbl_JOIN_INNER_TD_9897_consolidated_output.selectOverlayVersion(1);
    tbl_JOIN_INNER_TD_9897_consolidated_output.allocateHost();
    Table tbl_SerializeFromObject_TD_737_input;
    tbl_SerializeFromObject_TD_737_input = Table("part", part_n, 4, in_dir);
    tbl_SerializeFromObject_TD_737_input.addCol("p_partkey", 4);
    tbl_SerializeFromObject_TD_737_input.addCol("p_mfgr", TPCH_READ_P_MFG_LEN+1);
    tbl_SerializeFromObject_TD_737_input.addCol("p_type", TPCH_READ_P_TYPE_LEN+1);
    tbl_SerializeFromObject_TD_737_input.addCol("p_size", 4);
    tbl_SerializeFromObject_TD_737_input.allocateHost();
    tbl_SerializeFromObject_TD_737_input.loadHost();
    Table tbl_JOIN_INNER_TD_8407_output("tbl_JOIN_INNER_TD_8407_output", 2036, 4, "");
    tbl_JOIN_INNER_TD_8407_output.selectOverlayVersion(1);
    tbl_JOIN_INNER_TD_8407_output.allocateHost();
    Table tbl_JOIN_INNER_TD_8407_consolidated_output("tbl_JOIN_INNER_TD_8407_consolidated_output", 2036, 1, "");
    tbl_JOIN_INNER_TD_8407_consolidated_output.selectOverlayVersion(1);
    tbl_JOIN_INNER_TD_8407_consolidated_output.allocateHost();
    Table tbl_SerializeFromObject_TD_10879_input;
    tbl_SerializeFromObject_TD_10879_input = Table("partsupp", partsupp_n, 3, in_dir);
    tbl_SerializeFromObject_TD_10879_input.selectOverlayVersion(1);
    tbl_SerializeFromObject_TD_10879_input.addCol("ps_partkey", 4);
    tbl_SerializeFromObject_TD_10879_input.addCol("ps_suppkey", 4);
    tbl_SerializeFromObject_TD_10879_input.addCol("ps_supplycost", 4);
    tbl_SerializeFromObject_TD_10879_input.allocateHost();
    tbl_SerializeFromObject_TD_10879_input.loadHost();
    Table tbl_SerializeFromObject_TD_458_input_stringRowIDSubstitute;
    tbl_SerializeFromObject_TD_458_input_stringRowIDSubstitute = Table("supplier", supplier_n, 7, in_dir);
    tbl_SerializeFromObject_TD_458_input_stringRowIDSubstitute.addCol("s_suppkey", 4);
    tbl_SerializeFromObject_TD_458_input_stringRowIDSubstitute.addCol("s_name", 4, 1, 0);
    tbl_SerializeFromObject_TD_458_input_stringRowIDSubstitute.addCol("s_address", 4, 1, 0);
    tbl_SerializeFromObject_TD_458_input_stringRowIDSubstitute.addCol("s_nationkey", 4);
    tbl_SerializeFromObject_TD_458_input_stringRowIDSubstitute.addCol("s_phone", 4, 1, 0);
    tbl_SerializeFromObject_TD_458_input_stringRowIDSubstitute.addCol("s_acctbal", 4);
    tbl_SerializeFromObject_TD_458_input_stringRowIDSubstitute.addCol("s_comment", 4, 1, 0);
    tbl_SerializeFromObject_TD_458_input_stringRowIDSubstitute.allocateHost();
    tbl_SerializeFromObject_TD_458_input_stringRowIDSubstitute.loadHost();
    Table tbl_SerializeFromObject_TD_458_input;
    tbl_SerializeFromObject_TD_458_input = Table("supplier", supplier_n, 7, in_dir);
    tbl_SerializeFromObject_TD_458_input.addCol("s_suppkey", 4);
    tbl_SerializeFromObject_TD_458_input.addCol("s_name", TPCH_READ_S_NAME_LEN+1);
    tbl_SerializeFromObject_TD_458_input.addCol("s_address", TPCH_READ_S_ADDR_MAX+1);
    tbl_SerializeFromObject_TD_458_input.addCol("s_nationkey", 4);
    tbl_SerializeFromObject_TD_458_input.addCol("s_phone", TPCH_READ_PHONE_LEN+1);
    tbl_SerializeFromObject_TD_458_input.addCol("s_acctbal", 4);
    tbl_SerializeFromObject_TD_458_input.addCol("s_comment", TPCH_READ_S_CMNT_MAX+1);
    tbl_SerializeFromObject_TD_458_input.allocateHost();
    tbl_SerializeFromObject_TD_458_input.loadHost();
    Table tbl_JOIN_INNER_TD_7860_output("tbl_JOIN_INNER_TD_7860_output", 5, 4, "");
    tbl_JOIN_INNER_TD_7860_output.selectOverlayVersion(1);
    tbl_JOIN_INNER_TD_7860_output.allocateHost();
    Table tbl_JOIN_INNER_TD_7860_consolidated_output("tbl_JOIN_INNER_TD_7860_consolidated_output", 5, 1, "");
    tbl_JOIN_INNER_TD_7860_consolidated_output.selectOverlayVersion(1);
    tbl_JOIN_INNER_TD_7860_consolidated_output.allocateHost();
    Table tbl_SerializeFromObject_TD_9517_input;
    tbl_SerializeFromObject_TD_9517_input = Table("nation", nation_n, 2, in_dir);
    tbl_SerializeFromObject_TD_9517_input.selectOverlayVersion(1);
    tbl_SerializeFromObject_TD_9517_input.addCol("n_nationkey", 4);
    tbl_SerializeFromObject_TD_9517_input.addCol("n_regionkey", 4);
    tbl_SerializeFromObject_TD_9517_input.allocateHost();
    tbl_SerializeFromObject_TD_9517_input.loadHost();
    Table tbl_Filter_TD_8242_output("tbl_Filter_TD_8242_output", 6100000, 1, "");
    tbl_Filter_TD_8242_output.selectOverlayVersion(1);
    tbl_Filter_TD_8242_output.allocateHost();
    Table tbl_SerializeFromObject_TD_9675_input;
    tbl_SerializeFromObject_TD_9675_input = Table("region", region_n, 2, in_dir);
    tbl_SerializeFromObject_TD_9675_input.addCol("r_regionkey", 4);
    tbl_SerializeFromObject_TD_9675_input.addCol("r_name", TPCH_READ_REGION_LEN+1);
    tbl_SerializeFromObject_TD_9675_input.allocateHost();
    tbl_SerializeFromObject_TD_9675_input.loadHost();
    // ********************** Allocate Device Buffer ******************** // 
    //tbl_JOIN_INNER_TD_3352_output.allocateDevBuffer(context_h, 32);
    //tbl_JOIN_INNER_TD_4452_output.allocateDevBuffer(context_h, 32);
    tbl_Aggregate_TD_6224_output.allocateDevBuffer(context_h, 32);
    //tbl_JOIN_INNER_TD_5425_output.allocateDevBuffer(context_h, 32);
    //tbl_Filter_TD_6465_output.allocateDevBuffer(context_h, 32);
    //tbl_JOIN_INNER_TD_9897_output.allocateDevBuffer(context_h, 32);
    //tbl_JOIN_INNER_TD_8407_output.allocateDevBuffer(context_h, 32);
    // tbl_SerializeFromObject_TD_10879_input.allocateDevBuffer(context_h, 32);
    tbl_SerializeFromObject_TD_458_input_stringRowIDSubstitute.allocateDevBuffer(context_h, 32);
    //tbl_JOIN_INNER_TD_7860_output.allocateDevBuffer(context_h, 32);
    // tbl_SerializeFromObject_TD_9517_input.allocateDevBuffer(context_h, 32);
    //tbl_Filter_TD_8242_output.allocateDevBuffer(context_h, 32);
    // ****************************** Config **************************** // 
    
    xf::database::gqe::KernelCommand krn_cmd_build_334 = xf::database::gqe::KernelCommand();
    xf::database::gqe::KernelCommand krn_cmd_probe_334 = xf::database::gqe::KernelCommand();
    cfgCmd cfg_JOIN_INNER_TD_3352_cmds_build;
    cfg_JOIN_INNER_TD_3352_cmds_build.setup(1);
    cfg_JOIN_INNER_TD_3352_cmds_build.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_3352_gqe_join_build(krn_cmd_build_334);
    cfg_JOIN_INNER_TD_3352_cmds_build.cmd = krn_cmd_build_334.getConfigBits();
    get_meta_info_JOIN_INNER_TD_3352_gqe_join_build(cfg_JOIN_INNER_TD_3352_cmds_build.meta_in, tbl_JOIN_INNER_TD_4452_consolidated_output.nrow, 3);
    get_meta_info_JOIN_INNER_TD_3352_gqe_join_build(cfg_JOIN_INNER_TD_3352_cmds_build.meta_out, tbl_JOIN_INNER_TD_3352_output.nrow, 4);
    // cfg_JOIN_INNER_TD_3352_cmds_build.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_3352_cmds_probe;
    cfg_JOIN_INNER_TD_3352_cmds_probe.setup(1);
    cfg_JOIN_INNER_TD_3352_cmds_probe.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_3352_gqe_join_probe(krn_cmd_probe_334);
    cfg_JOIN_INNER_TD_3352_cmds_probe.cmd = krn_cmd_probe_334.getConfigBits();
    get_meta_info_JOIN_INNER_TD_3352_gqe_join_probe(cfg_JOIN_INNER_TD_3352_cmds_probe.meta_in, tbl_SerializeFromObject_TD_458_input_stringRowIDSubstitute.nrow, 3);
    get_meta_info_JOIN_INNER_TD_3352_gqe_join_probe(cfg_JOIN_INNER_TD_3352_cmds_probe.meta_out, tbl_JOIN_INNER_TD_3352_output.nrow, 3);
    // cfg_JOIN_INNER_TD_3352_cmds_probe.allocateDevBuffer(context_h, 32);
    cfg_JOIN_INNER_TD_3352_cmds_build.input_key_columns = {2, -1, -1};
    cfg_JOIN_INNER_TD_3352_cmds_probe.input_key_columns = {0, -1, -1};
    cfg_JOIN_INNER_TD_3352_cmds_build.output_key_columns = {0, 1, 2, 3};
    cfg_JOIN_INNER_TD_3352_cmds_probe.output_key_columns = {0, 1, 2, 3};
    
    xf::database::gqe::KernelCommand krn_cmd_build_783 = xf::database::gqe::KernelCommand();
    xf::database::gqe::KernelCommand krn_cmd_probe_783 = xf::database::gqe::KernelCommand();
    cfgCmd cfg_JOIN_INNER_TD_4452_cmds_build;
    cfg_JOIN_INNER_TD_4452_cmds_build.setup(1);
    cfg_JOIN_INNER_TD_4452_cmds_build.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_4452_gqe_join_build(krn_cmd_build_783);
    cfg_JOIN_INNER_TD_4452_cmds_build.cmd = krn_cmd_build_783.getConfigBits();
    get_meta_info_JOIN_INNER_TD_4452_gqe_join_build(cfg_JOIN_INNER_TD_4452_cmds_build.meta_in, tbl_Aggregate_TD_6224_output.nrow, 3);
    get_meta_info_JOIN_INNER_TD_4452_gqe_join_build(cfg_JOIN_INNER_TD_4452_cmds_build.meta_out, tbl_JOIN_INNER_TD_4452_output.nrow, 4);
    // cfg_JOIN_INNER_TD_4452_cmds_build.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_4452_cmds_probe;
    cfg_JOIN_INNER_TD_4452_cmds_probe.setup(1);
    cfg_JOIN_INNER_TD_4452_cmds_probe.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_4452_gqe_join_probe(krn_cmd_probe_783);
    cfg_JOIN_INNER_TD_4452_cmds_probe.cmd = krn_cmd_probe_783.getConfigBits();
    get_meta_info_JOIN_INNER_TD_4452_gqe_join_probe(cfg_JOIN_INNER_TD_4452_cmds_probe.meta_in, tbl_JOIN_INNER_TD_5425_consolidated_output.nrow, 3);
    get_meta_info_JOIN_INNER_TD_4452_gqe_join_probe(cfg_JOIN_INNER_TD_4452_cmds_probe.meta_out, tbl_JOIN_INNER_TD_4452_output.nrow, 3);
    // cfg_JOIN_INNER_TD_4452_cmds_probe.allocateDevBuffer(context_h, 32);
    cfg_JOIN_INNER_TD_4452_cmds_build.input_key_columns = {0, 1, -1};
    cfg_JOIN_INNER_TD_4452_cmds_probe.input_key_columns = {3, 0, -1};
    cfg_JOIN_INNER_TD_4452_cmds_build.output_key_columns = {0, 1, 2, 3};
    cfg_JOIN_INNER_TD_4452_cmds_probe.output_key_columns = {0, 1, 2, 3};
    
    xf::database::gqe::KernelCommand krn_cmd_build_769 = xf::database::gqe::KernelCommand();
    xf::database::gqe::KernelCommand krn_cmd_probe_769 = xf::database::gqe::KernelCommand();
    cfgCmd cfg_JOIN_INNER_TD_5425_cmds_build;
    cfg_JOIN_INNER_TD_5425_cmds_build.setup(1);
    cfg_JOIN_INNER_TD_5425_cmds_build.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_5425_gqe_join_build(krn_cmd_build_769);
    cfg_JOIN_INNER_TD_5425_cmds_build.cmd = krn_cmd_build_769.getConfigBits();
    get_meta_info_JOIN_INNER_TD_5425_gqe_join_build(cfg_JOIN_INNER_TD_5425_cmds_build.meta_in, tbl_JOIN_INNER_TD_9897_consolidated_output.nrow, 3);
    get_meta_info_JOIN_INNER_TD_5425_gqe_join_build(cfg_JOIN_INNER_TD_5425_cmds_build.meta_out, tbl_JOIN_INNER_TD_5425_output.nrow, 4);
    // cfg_JOIN_INNER_TD_5425_cmds_build.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_5425_cmds_probe;
    cfg_JOIN_INNER_TD_5425_cmds_probe.setup(1);
    cfg_JOIN_INNER_TD_5425_cmds_probe.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_5425_gqe_join_probe(krn_cmd_probe_769);
    cfg_JOIN_INNER_TD_5425_cmds_probe.cmd = krn_cmd_probe_769.getConfigBits();
    get_meta_info_JOIN_INNER_TD_5425_gqe_join_probe(cfg_JOIN_INNER_TD_5425_cmds_probe.meta_in, tbl_Filter_TD_6465_output.nrow, 3);
    get_meta_info_JOIN_INNER_TD_5425_gqe_join_probe(cfg_JOIN_INNER_TD_5425_cmds_probe.meta_out, tbl_JOIN_INNER_TD_5425_output.nrow, 3);
    // cfg_JOIN_INNER_TD_5425_cmds_probe.allocateDevBuffer(context_h, 32);
    cfg_JOIN_INNER_TD_5425_cmds_build.input_key_columns = {0, -1, -1};
    cfg_JOIN_INNER_TD_5425_cmds_probe.input_key_columns = {0, -1, -1};
    cfg_JOIN_INNER_TD_5425_cmds_build.output_key_columns = {0, 1, 2, 3};
    cfg_JOIN_INNER_TD_5425_cmds_probe.output_key_columns = {0, 1, 2, 3};
    
    xf::database::gqe::KernelCommand krn_cmd_build_772 = xf::database::gqe::KernelCommand();
    xf::database::gqe::KernelCommand krn_cmd_probe_772 = xf::database::gqe::KernelCommand();
    cfgCmd cfg_JOIN_INNER_TD_9897_cmds_build;
    cfg_JOIN_INNER_TD_9897_cmds_build.setup(1);
    cfg_JOIN_INNER_TD_9897_cmds_build.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_9897_gqe_join_build(krn_cmd_build_772);
    cfg_JOIN_INNER_TD_9897_cmds_build.cmd = krn_cmd_build_772.getConfigBits();
    get_meta_info_JOIN_INNER_TD_9897_gqe_join_build(cfg_JOIN_INNER_TD_9897_cmds_build.meta_in, tbl_JOIN_INNER_TD_8407_consolidated_output.nrow, 3);
    get_meta_info_JOIN_INNER_TD_9897_gqe_join_build(cfg_JOIN_INNER_TD_9897_cmds_build.meta_out, tbl_JOIN_INNER_TD_9897_output.nrow, 4);
    // cfg_JOIN_INNER_TD_9897_cmds_build.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_9897_cmds_probe;
    cfg_JOIN_INNER_TD_9897_cmds_probe.setup(1);
    cfg_JOIN_INNER_TD_9897_cmds_probe.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_9897_gqe_join_probe(krn_cmd_probe_772);
    cfg_JOIN_INNER_TD_9897_cmds_probe.cmd = krn_cmd_probe_772.getConfigBits();
    get_meta_info_JOIN_INNER_TD_9897_gqe_join_probe(cfg_JOIN_INNER_TD_9897_cmds_probe.meta_in, tbl_SerializeFromObject_TD_10879_input.nrow, 3);
    get_meta_info_JOIN_INNER_TD_9897_gqe_join_probe(cfg_JOIN_INNER_TD_9897_cmds_probe.meta_out, tbl_JOIN_INNER_TD_9897_output.nrow, 3);
    // cfg_JOIN_INNER_TD_9897_cmds_probe.allocateDevBuffer(context_h, 32);
    cfg_JOIN_INNER_TD_9897_cmds_build.input_key_columns = {0, -1, -1};
    cfg_JOIN_INNER_TD_9897_cmds_probe.input_key_columns = {1, -1, -1};
    cfg_JOIN_INNER_TD_9897_cmds_build.output_key_columns = {0, 1, 2, 3};
    cfg_JOIN_INNER_TD_9897_cmds_probe.output_key_columns = {0, 1, 2, 3};
    
    xf::database::gqe::KernelCommand krn_cmd_build_334 = xf::database::gqe::KernelCommand();
    xf::database::gqe::KernelCommand krn_cmd_probe_334 = xf::database::gqe::KernelCommand();
    cfgCmd cfg_JOIN_INNER_TD_8407_cmds_build;
    cfg_JOIN_INNER_TD_8407_cmds_build.setup(1);
    cfg_JOIN_INNER_TD_8407_cmds_build.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_8407_gqe_join_build(krn_cmd_build_334);
    cfg_JOIN_INNER_TD_8407_cmds_build.cmd = krn_cmd_build_334.getConfigBits();
    get_meta_info_JOIN_INNER_TD_8407_gqe_join_build(cfg_JOIN_INNER_TD_8407_cmds_build.meta_in, tbl_SerializeFromObject_TD_458_input_stringRowIDSubstitute.nrow, 3);
    get_meta_info_JOIN_INNER_TD_8407_gqe_join_build(cfg_JOIN_INNER_TD_8407_cmds_build.meta_out, tbl_JOIN_INNER_TD_8407_output.nrow, 4);
    // cfg_JOIN_INNER_TD_8407_cmds_build.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_8407_cmds_probe;
    cfg_JOIN_INNER_TD_8407_cmds_probe.setup(1);
    cfg_JOIN_INNER_TD_8407_cmds_probe.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_8407_gqe_join_probe(krn_cmd_probe_334);
    cfg_JOIN_INNER_TD_8407_cmds_probe.cmd = krn_cmd_probe_334.getConfigBits();
    get_meta_info_JOIN_INNER_TD_8407_gqe_join_probe(cfg_JOIN_INNER_TD_8407_cmds_probe.meta_in, tbl_JOIN_INNER_TD_7860_consolidated_output.nrow, 3);
    get_meta_info_JOIN_INNER_TD_8407_gqe_join_probe(cfg_JOIN_INNER_TD_8407_cmds_probe.meta_out, tbl_JOIN_INNER_TD_8407_output.nrow, 3);
    // cfg_JOIN_INNER_TD_8407_cmds_probe.allocateDevBuffer(context_h, 32);
    cfg_JOIN_INNER_TD_8407_cmds_build.input_key_columns = {3, -1, -1};
    cfg_JOIN_INNER_TD_8407_cmds_probe.input_key_columns = {0, -1, -1};
    cfg_JOIN_INNER_TD_8407_cmds_build.output_key_columns = {0, 1, 2, 3};
    cfg_JOIN_INNER_TD_8407_cmds_probe.output_key_columns = {0, 1, 2, 3};
    
    xf::database::gqe::KernelCommand krn_cmd_build_951 = xf::database::gqe::KernelCommand();
    xf::database::gqe::KernelCommand krn_cmd_probe_951 = xf::database::gqe::KernelCommand();
    cfgCmd cfg_JOIN_INNER_TD_7860_cmds_build;
    cfg_JOIN_INNER_TD_7860_cmds_build.setup(1);
    cfg_JOIN_INNER_TD_7860_cmds_build.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_7860_gqe_join_build(krn_cmd_build_951);
    cfg_JOIN_INNER_TD_7860_cmds_build.cmd = krn_cmd_build_951.getConfigBits();
    get_meta_info_JOIN_INNER_TD_7860_gqe_join_build(cfg_JOIN_INNER_TD_7860_cmds_build.meta_in, tbl_SerializeFromObject_TD_9517_input.nrow, 3);
    get_meta_info_JOIN_INNER_TD_7860_gqe_join_build(cfg_JOIN_INNER_TD_7860_cmds_build.meta_out, tbl_JOIN_INNER_TD_7860_output.nrow, 4);
    // cfg_JOIN_INNER_TD_7860_cmds_build.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_7860_cmds_probe;
    cfg_JOIN_INNER_TD_7860_cmds_probe.setup(1);
    cfg_JOIN_INNER_TD_7860_cmds_probe.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_7860_gqe_join_probe(krn_cmd_probe_951);
    cfg_JOIN_INNER_TD_7860_cmds_probe.cmd = krn_cmd_probe_951.getConfigBits();
    get_meta_info_JOIN_INNER_TD_7860_gqe_join_probe(cfg_JOIN_INNER_TD_7860_cmds_probe.meta_in, tbl_Filter_TD_8242_output.nrow, 3);
    get_meta_info_JOIN_INNER_TD_7860_gqe_join_probe(cfg_JOIN_INNER_TD_7860_cmds_probe.meta_out, tbl_JOIN_INNER_TD_7860_output.nrow, 3);
    // cfg_JOIN_INNER_TD_7860_cmds_probe.allocateDevBuffer(context_h, 32);
    cfg_JOIN_INNER_TD_7860_cmds_build.input_key_columns = {1, -1, -1};
    cfg_JOIN_INNER_TD_7860_cmds_probe.input_key_columns = {0, -1, -1};
    cfg_JOIN_INNER_TD_7860_cmds_build.output_key_columns = {0, 1, 2, 3};
    cfg_JOIN_INNER_TD_7860_cmds_probe.output_key_columns = {0, 1, 2, 3};
    // *************************** Kernel Setup ************************* // 
    bufferTmp buftmp_h(context_h, 1); 
    buftmp_h.initBuffer(q_h); 
    std::cout << std::endl; 
    krnlEngine krnl_JOIN_INNER_TD_3352_build;
    krnl_JOIN_INNER_TD_3352_build = krnlEngine(program_h, q_h, context_h, "gqeKernel", 0);
    krnl_JOIN_INNER_TD_3352_build.setup(tbl_JOIN_INNER_TD_4452_consolidated_output, tbl_SerializeFromObject_TD_458_input_stringRowIDSubstitute, tbl_JOIN_INNER_TD_3352_output, cfg_JOIN_INNER_TD_3352_cmds_build, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_3352_probe;
    krnl_JOIN_INNER_TD_3352_probe = krnlEngine(program_h, q_h, context_h, "gqeKernel", 1);
    krnl_JOIN_INNER_TD_3352_probe.setup(tbl_SerializeFromObject_TD_458_input_stringRowIDSubstitute, tbl_JOIN_INNER_TD_4452_consolidated_output, tbl_JOIN_INNER_TD_3352_output, cfg_JOIN_INNER_TD_3352_cmds_probe, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_4452_build;
    krnl_JOIN_INNER_TD_4452_build = krnlEngine(program_h, q_h, context_h, "gqeKernel", 0);
    krnl_JOIN_INNER_TD_4452_build.setup(tbl_Aggregate_TD_6224_output, tbl_JOIN_INNER_TD_5425_consolidated_output, tbl_JOIN_INNER_TD_4452_output, cfg_JOIN_INNER_TD_4452_cmds_build, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_4452_probe;
    krnl_JOIN_INNER_TD_4452_probe = krnlEngine(program_h, q_h, context_h, "gqeKernel", 1);
    krnl_JOIN_INNER_TD_4452_probe.setup(tbl_JOIN_INNER_TD_5425_consolidated_output, tbl_Aggregate_TD_6224_output, tbl_JOIN_INNER_TD_4452_output, cfg_JOIN_INNER_TD_4452_cmds_probe, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_5425_build;
    krnl_JOIN_INNER_TD_5425_build = krnlEngine(program_h, q_h, context_h, "gqeKernel", 0);
    krnl_JOIN_INNER_TD_5425_build.setup(tbl_JOIN_INNER_TD_9897_consolidated_output, tbl_Filter_TD_6465_output, tbl_JOIN_INNER_TD_5425_output, cfg_JOIN_INNER_TD_5425_cmds_build, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_5425_probe;
    krnl_JOIN_INNER_TD_5425_probe = krnlEngine(program_h, q_h, context_h, "gqeKernel", 1);
    krnl_JOIN_INNER_TD_5425_probe.setup(tbl_Filter_TD_6465_output, tbl_JOIN_INNER_TD_9897_consolidated_output, tbl_JOIN_INNER_TD_5425_output, cfg_JOIN_INNER_TD_5425_cmds_probe, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_9897_build;
    krnl_JOIN_INNER_TD_9897_build = krnlEngine(program_h, q_h, context_h, "gqeKernel", 0);
    krnl_JOIN_INNER_TD_9897_build.setup(tbl_JOIN_INNER_TD_8407_consolidated_output, tbl_SerializeFromObject_TD_10879_input, tbl_JOIN_INNER_TD_9897_output, cfg_JOIN_INNER_TD_9897_cmds_build, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_9897_probe;
    krnl_JOIN_INNER_TD_9897_probe = krnlEngine(program_h, q_h, context_h, "gqeKernel", 1);
    krnl_JOIN_INNER_TD_9897_probe.setup(tbl_SerializeFromObject_TD_10879_input, tbl_JOIN_INNER_TD_8407_consolidated_output, tbl_JOIN_INNER_TD_9897_output, cfg_JOIN_INNER_TD_9897_cmds_probe, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_8407_build;
    krnl_JOIN_INNER_TD_8407_build = krnlEngine(program_h, q_h, context_h, "gqeKernel", 0);
    krnl_JOIN_INNER_TD_8407_build.setup(tbl_SerializeFromObject_TD_458_input_stringRowIDSubstitute, tbl_JOIN_INNER_TD_7860_consolidated_output, tbl_JOIN_INNER_TD_8407_output, cfg_JOIN_INNER_TD_8407_cmds_build, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_8407_probe;
    krnl_JOIN_INNER_TD_8407_probe = krnlEngine(program_h, q_h, context_h, "gqeKernel", 1);
    krnl_JOIN_INNER_TD_8407_probe.setup(tbl_JOIN_INNER_TD_7860_consolidated_output, tbl_SerializeFromObject_TD_458_input_stringRowIDSubstitute, tbl_JOIN_INNER_TD_8407_output, cfg_JOIN_INNER_TD_8407_cmds_probe, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_7860_build;
    krnl_JOIN_INNER_TD_7860_build = krnlEngine(program_h, q_h, context_h, "gqeKernel", 0);
    krnl_JOIN_INNER_TD_7860_build.setup(tbl_SerializeFromObject_TD_9517_input, tbl_Filter_TD_8242_output, tbl_JOIN_INNER_TD_7860_output, cfg_JOIN_INNER_TD_7860_cmds_build, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_7860_probe;
    krnl_JOIN_INNER_TD_7860_probe = krnlEngine(program_h, q_h, context_h, "gqeKernel", 1);
    krnl_JOIN_INNER_TD_7860_probe.setup(tbl_Filter_TD_8242_output, tbl_SerializeFromObject_TD_9517_input, tbl_JOIN_INNER_TD_7860_output, cfg_JOIN_INNER_TD_7860_cmds_probe, buftmp_h);
    // ************************** Transfer Engine *********************** // 
    transEngine trans_JOIN_INNER_TD_3352;
    trans_JOIN_INNER_TD_3352.setq(q_h,1);
    trans_JOIN_INNER_TD_3352.add(&(cfg_JOIN_INNER_TD_3352_cmds_build));
    trans_JOIN_INNER_TD_3352.add(&(cfg_JOIN_INNER_TD_3352_cmds_probe));
    trans_JOIN_INNER_TD_3352.add(&(tbl_SerializeFromObject_TD_458_input_stringRowIDSubstitute));
    transEngine trans_JOIN_INNER_TD_3352_out;
    trans_JOIN_INNER_TD_3352_out.setq(q_h,1);
    q_h.finish();
    transEngine trans_JOIN_INNER_TD_4452;
    trans_JOIN_INNER_TD_4452.setq(q_h,1);
    trans_JOIN_INNER_TD_4452.add(&(cfg_JOIN_INNER_TD_4452_cmds_build));
    trans_JOIN_INNER_TD_4452.add(&(cfg_JOIN_INNER_TD_4452_cmds_probe));
    transEngine trans_JOIN_INNER_TD_4452_out;
    trans_JOIN_INNER_TD_4452_out.setq(q_h,1);
    q_h.finish();
    transEngine trans_JOIN_INNER_TD_5425;
    trans_JOIN_INNER_TD_5425.setq(q_h,1);
    trans_JOIN_INNER_TD_5425.add(&(cfg_JOIN_INNER_TD_5425_cmds_build));
    trans_JOIN_INNER_TD_5425.add(&(cfg_JOIN_INNER_TD_5425_cmds_probe));
    transEngine trans_JOIN_INNER_TD_5425_out;
    trans_JOIN_INNER_TD_5425_out.setq(q_h,1);
    q_h.finish();
    transEngine trans_JOIN_INNER_TD_9897;
    trans_JOIN_INNER_TD_9897.setq(q_h,1);
    trans_JOIN_INNER_TD_9897.add(&(cfg_JOIN_INNER_TD_9897_cmds_build));
    trans_JOIN_INNER_TD_9897.add(&(cfg_JOIN_INNER_TD_9897_cmds_probe));
    trans_JOIN_INNER_TD_9897.add(&(tbl_SerializeFromObject_TD_10879_input));
    transEngine trans_JOIN_INNER_TD_9897_out;
    trans_JOIN_INNER_TD_9897_out.setq(q_h,1);
    q_h.finish();
    transEngine trans_JOIN_INNER_TD_8407;
    trans_JOIN_INNER_TD_8407.setq(q_h,1);
    trans_JOIN_INNER_TD_8407.add(&(cfg_JOIN_INNER_TD_8407_cmds_build));
    trans_JOIN_INNER_TD_8407.add(&(cfg_JOIN_INNER_TD_8407_cmds_probe));
    trans_JOIN_INNER_TD_8407.add(&(tbl_SerializeFromObject_TD_458_input_stringRowIDSubstitute));
    transEngine trans_JOIN_INNER_TD_8407_out;
    trans_JOIN_INNER_TD_8407_out.setq(q_h,1);
    q_h.finish();
    transEngine trans_JOIN_INNER_TD_7860;
    trans_JOIN_INNER_TD_7860.setq(q_h,1);
    trans_JOIN_INNER_TD_7860.add(&(cfg_JOIN_INNER_TD_7860_cmds_build));
    trans_JOIN_INNER_TD_7860.add(&(cfg_JOIN_INNER_TD_7860_cmds_probe));
    trans_JOIN_INNER_TD_7860.add(&(tbl_SerializeFromObject_TD_9517_input));
    transEngine trans_JOIN_INNER_TD_7860_out;
    trans_JOIN_INNER_TD_7860_out.setq(q_h,1);
    q_h.finish();
    // ****************************** Events **************************** // 
    std::vector<cl::Event> events_h2d_wr_JOIN_INNER_TD_3352;
    std::vector<cl::Event> events_d2h_rd_JOIN_INNER_TD_3352;
    std::vector<cl::Event> events_JOIN_INNER_TD_3352;
    events_h2d_wr_JOIN_INNER_TD_3352.resize(1);
    events_d2h_rd_JOIN_INNER_TD_3352.resize(1);
    events_JOIN_INNER_TD_3352.resize(2);
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_3352;
    std::vector<cl::Event> prev_events_grp_JOIN_INNER_TD_3352;
    std::vector<cl::Event> events_h2d_wr_JOIN_INNER_TD_4452;
    std::vector<cl::Event> events_d2h_rd_JOIN_INNER_TD_4452;
    std::vector<cl::Event> events_JOIN_INNER_TD_4452;
    events_h2d_wr_JOIN_INNER_TD_4452.resize(1);
    events_d2h_rd_JOIN_INNER_TD_4452.resize(1);
    events_JOIN_INNER_TD_4452.resize(2);
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_4452;
    std::vector<cl::Event> prev_events_grp_JOIN_INNER_TD_4452;
    std::vector<cl::Event> events_h2d_wr_JOIN_INNER_TD_5425;
    std::vector<cl::Event> events_d2h_rd_JOIN_INNER_TD_5425;
    std::vector<cl::Event> events_JOIN_INNER_TD_5425;
    events_h2d_wr_JOIN_INNER_TD_5425.resize(1);
    events_d2h_rd_JOIN_INNER_TD_5425.resize(1);
    events_JOIN_INNER_TD_5425.resize(2);
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_5425;
    std::vector<cl::Event> prev_events_grp_JOIN_INNER_TD_5425;
    std::vector<cl::Event> events_h2d_wr_JOIN_INNER_TD_9897;
    std::vector<cl::Event> events_d2h_rd_JOIN_INNER_TD_9897;
    std::vector<cl::Event> events_JOIN_INNER_TD_9897;
    events_h2d_wr_JOIN_INNER_TD_9897.resize(1);
    events_d2h_rd_JOIN_INNER_TD_9897.resize(1);
    events_JOIN_INNER_TD_9897.resize(2);
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_9897;
    std::vector<cl::Event> prev_events_grp_JOIN_INNER_TD_9897;
    std::vector<cl::Event> events_h2d_wr_JOIN_INNER_TD_8407;
    std::vector<cl::Event> events_d2h_rd_JOIN_INNER_TD_8407;
    std::vector<cl::Event> events_JOIN_INNER_TD_8407;
    events_h2d_wr_JOIN_INNER_TD_8407.resize(1);
    events_d2h_rd_JOIN_INNER_TD_8407.resize(1);
    events_JOIN_INNER_TD_8407.resize(2);
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_8407;
    std::vector<cl::Event> prev_events_grp_JOIN_INNER_TD_8407;
    std::vector<cl::Event> events_h2d_wr_JOIN_INNER_TD_7860;
    std::vector<cl::Event> events_d2h_rd_JOIN_INNER_TD_7860;
    std::vector<cl::Event> events_JOIN_INNER_TD_7860;
    events_h2d_wr_JOIN_INNER_TD_7860.resize(1);
    events_d2h_rd_JOIN_INNER_TD_7860.resize(1);
    events_JOIN_INNER_TD_7860.resize(2);
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_7860;
    std::vector<cl::Event> prev_events_grp_JOIN_INNER_TD_7860;
    // **************************** Operations ************************** // 
    struct timeval tv_r_s, tv_r_e; 
    gettimeofday(&tv_r_s, 0); 

    struct timeval tv_r_Filter_8_931_s, tv_r_Filter_8_931_e;
    gettimeofday(&tv_r_Filter_8_931_s, 0);
    SW_Filter_TD_8242(tbl_SerializeFromObject_TD_9675_input, tbl_Filter_TD_8242_output);
    gettimeofday(&tv_r_Filter_8_931_e, 0);

    struct timeval tv_r_JOIN_INNER_7_742_s, tv_r_JOIN_INNER_7_742_e;
    gettimeofday(&tv_r_JOIN_INNER_7_742_s, 0);
    tbl_SerializeFromObject_TD_9517_input.tableToCol();
    tbl_Filter_TD_8242_output.tableToCol();
    krnl_JOIN_INNER_TD_7860_build.updateMeta(tbl_SerializeFromObject_TD_9517_input.getNumRow(), 3);
    krnl_JOIN_INNER_TD_7860_probe.updateMeta(tbl_Filter_TD_8242_output.getNumRow(), 3);
    trans_JOIN_INNER_TD_7860.add(&(tbl_Filter_TD_8242_output));
    trans_JOIN_INNER_TD_7860.host2dev(0, &(prev_events_grp_JOIN_INNER_TD_7860), &(events_h2d_wr_JOIN_INNER_TD_7860[0]));
    events_grp_JOIN_INNER_TD_7860.push_back(events_h2d_wr_JOIN_INNER_TD_7860[0]);
    krnl_JOIN_INNER_TD_7860_build.run(0, &(events_grp_JOIN_INNER_TD_7860), &(events_JOIN_INNER_TD_7860[0]));
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_7860_build_done;
    events_grp_JOIN_INNER_TD_7860_build_done.push_back(events_JOIN_INNER_TD_7860[0]);
    krnl_JOIN_INNER_TD_7860_probe.run(0, &(events_grp_JOIN_INNER_TD_7860_build_done), &(events_JOIN_INNER_TD_7860[1]));
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_7860_probe_done;
    events_grp_JOIN_INNER_TD_7860_probe_done.push_back(events_JOIN_INNER_TD_7860[1]);
    trans_JOIN_INNER_TD_7860_out.add(&(tbl_JOIN_INNER_TD_7860_output));
    trans_JOIN_INNER_TD_7860_out.add(&(cfg_JOIN_INNER_TD_7860_cmds_build));
    trans_JOIN_INNER_TD_7860_out.add(&(cfg_JOIN_INNER_TD_7860_cmds_probe));
    trans_JOIN_INNER_TD_7860_out.dev2host(0, &(events_grp_JOIN_INNER_TD_7860_probe_done), &(events_d2h_rd_JOIN_INNER_TD_7860[0]));
    q_h.flush();
    q_h.finish();
    tbl_JOIN_INNER_TD_7860_output.setNumRow((cfg_JOIN_INNER_TD_7860_cmds_probe.meta_out)->getColLen());
    tbl_JOIN_INNER_TD_7860_output.colToTable();
    SW_Consolidated_JOIN_INNER_TD_7860_output(tbl_SerializeFromObject_TD_9517_input, tbl_Filter_TD_8242_output, tbl_JOIN_INNER_TD_7860_output, tbl_JOIN_INNER_TD_7860_consolidated_output);
    gettimeofday(&tv_r_JOIN_INNER_7_742_e, 0);

    struct timeval tv_r_JOIN_INNER_8_691_s, tv_r_JOIN_INNER_8_691_e;
    gettimeofday(&tv_r_JOIN_INNER_8_691_s, 0);
    prev_events_grp_JOIN_INNER_TD_8407.push_back(events_h2d_wr_JOIN_INNER_TD_7860[0]);
    tbl_SerializeFromObject_TD_458_input_stringRowIDSubstitute.tableToCol();
    tbl_JOIN_INNER_TD_7860_consolidated_output.tableToCol();
    krnl_JOIN_INNER_TD_8407_build.updateMeta(tbl_SerializeFromObject_TD_458_input_stringRowIDSubstitute.getNumRow(), 3);
    krnl_JOIN_INNER_TD_8407_probe.updateMeta(tbl_JOIN_INNER_TD_7860_consolidated_output.getNumRow(), 3);
    trans_JOIN_INNER_TD_8407.host2dev(0, &(prev_events_grp_JOIN_INNER_TD_8407), &(events_h2d_wr_JOIN_INNER_TD_8407[0]));
    events_grp_JOIN_INNER_TD_8407.push_back(events_h2d_wr_JOIN_INNER_TD_8407[0]);
    events_grp_JOIN_INNER_TD_8407.push_back(events_JOIN_INNER_TD_7860[0]);
    krnl_JOIN_INNER_TD_8407_build.run(0, &(events_grp_JOIN_INNER_TD_8407), &(events_JOIN_INNER_TD_8407[0]));
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_8407_build_done;
    events_grp_JOIN_INNER_TD_8407_build_done.push_back(events_JOIN_INNER_TD_8407[0]);
    krnl_JOIN_INNER_TD_8407_probe.run(0, &(events_grp_JOIN_INNER_TD_8407_build_done), &(events_JOIN_INNER_TD_8407[1]));
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_8407_probe_done;
    events_grp_JOIN_INNER_TD_8407_probe_done.push_back(events_JOIN_INNER_TD_8407[1]);
    trans_JOIN_INNER_TD_8407_out.add(&(tbl_JOIN_INNER_TD_8407_output));
    trans_JOIN_INNER_TD_8407_out.add(&(cfg_JOIN_INNER_TD_8407_cmds_build));
    trans_JOIN_INNER_TD_8407_out.add(&(cfg_JOIN_INNER_TD_8407_cmds_probe));
    trans_JOIN_INNER_TD_8407_out.dev2host(0, &(events_grp_JOIN_INNER_TD_8407_probe_done), &(events_d2h_rd_JOIN_INNER_TD_8407[0]));
    q_h.flush();
    q_h.finish();
    tbl_JOIN_INNER_TD_8407_output.setNumRow((cfg_JOIN_INNER_TD_8407_cmds_probe.meta_out)->getColLen());
    tbl_JOIN_INNER_TD_8407_output.colToTable();
    SW_Consolidated_JOIN_INNER_TD_8407_output(tbl_SerializeFromObject_TD_458_input_stringRowIDSubstitute, tbl_JOIN_INNER_TD_7860_consolidated_output, tbl_JOIN_INNER_TD_8407_output, tbl_JOIN_INNER_TD_8407_consolidated_output);
    gettimeofday(&tv_r_JOIN_INNER_8_691_e, 0);

    struct timeval tv_r_JOIN_INNER_9_318_s, tv_r_JOIN_INNER_9_318_e;
    gettimeofday(&tv_r_JOIN_INNER_9_318_s, 0);
    prev_events_grp_JOIN_INNER_TD_9897.push_back(events_h2d_wr_JOIN_INNER_TD_8407[0]);
    tbl_JOIN_INNER_TD_8407_consolidated_output.tableToCol();
    tbl_SerializeFromObject_TD_10879_input.tableToCol();
    krnl_JOIN_INNER_TD_9897_build.updateMeta(tbl_JOIN_INNER_TD_8407_consolidated_output.getNumRow(), 3);
    krnl_JOIN_INNER_TD_9897_probe.updateMeta(tbl_SerializeFromObject_TD_10879_input.getNumRow(), 3);
    trans_JOIN_INNER_TD_9897.host2dev(0, &(prev_events_grp_JOIN_INNER_TD_9897), &(events_h2d_wr_JOIN_INNER_TD_9897[0]));
    events_grp_JOIN_INNER_TD_9897.push_back(events_h2d_wr_JOIN_INNER_TD_9897[0]);
    events_grp_JOIN_INNER_TD_9897.push_back(events_JOIN_INNER_TD_8407[0]);
    krnl_JOIN_INNER_TD_9897_build.run(0, &(events_grp_JOIN_INNER_TD_9897), &(events_JOIN_INNER_TD_9897[0]));
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_9897_build_done;
    events_grp_JOIN_INNER_TD_9897_build_done.push_back(events_JOIN_INNER_TD_9897[0]);
    krnl_JOIN_INNER_TD_9897_probe.run(0, &(events_grp_JOIN_INNER_TD_9897_build_done), &(events_JOIN_INNER_TD_9897[1]));
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_9897_probe_done;
    events_grp_JOIN_INNER_TD_9897_probe_done.push_back(events_JOIN_INNER_TD_9897[1]);
    trans_JOIN_INNER_TD_9897_out.add(&(tbl_JOIN_INNER_TD_9897_output));
    trans_JOIN_INNER_TD_9897_out.add(&(cfg_JOIN_INNER_TD_9897_cmds_build));
    trans_JOIN_INNER_TD_9897_out.add(&(cfg_JOIN_INNER_TD_9897_cmds_probe));
    trans_JOIN_INNER_TD_9897_out.dev2host(0, &(events_grp_JOIN_INNER_TD_9897_probe_done), &(events_d2h_rd_JOIN_INNER_TD_9897[0]));
    q_h.flush();
    q_h.finish();
    tbl_JOIN_INNER_TD_9897_output.setNumRow((cfg_JOIN_INNER_TD_9897_cmds_probe.meta_out)->getColLen());
    tbl_JOIN_INNER_TD_9897_output.colToTable();
    SW_Consolidated_JOIN_INNER_TD_9897_output(tbl_JOIN_INNER_TD_8407_consolidated_output, tbl_SerializeFromObject_TD_10879_input, tbl_JOIN_INNER_TD_9897_output, tbl_JOIN_INNER_TD_9897_consolidated_output);
    gettimeofday(&tv_r_JOIN_INNER_9_318_e, 0);

    struct timeval tv_r_Filter_6_540_s, tv_r_Filter_6_540_e;
    gettimeofday(&tv_r_Filter_6_540_s, 0);
    SW_Filter_TD_6465(tbl_SerializeFromObject_TD_737_input, tbl_Filter_TD_6465_output);
    gettimeofday(&tv_r_Filter_6_540_e, 0);

    struct timeval tv_r_JOIN_INNER_5_417_s, tv_r_JOIN_INNER_5_417_e;
    gettimeofday(&tv_r_JOIN_INNER_5_417_s, 0);
    prev_events_grp_JOIN_INNER_TD_5425.push_back(events_h2d_wr_JOIN_INNER_TD_9897[0]);
    tbl_JOIN_INNER_TD_9897_consolidated_output.tableToCol();
    tbl_Filter_TD_6465_output.tableToCol();
    krnl_JOIN_INNER_TD_5425_build.updateMeta(tbl_JOIN_INNER_TD_9897_consolidated_output.getNumRow(), 3);
    krnl_JOIN_INNER_TD_5425_probe.updateMeta(tbl_Filter_TD_6465_output.getNumRow(), 3);
    trans_JOIN_INNER_TD_5425.add(&(tbl_Filter_TD_6465_output));
    trans_JOIN_INNER_TD_5425.host2dev(0, &(prev_events_grp_JOIN_INNER_TD_5425), &(events_h2d_wr_JOIN_INNER_TD_5425[0]));
    events_grp_JOIN_INNER_TD_5425.push_back(events_h2d_wr_JOIN_INNER_TD_5425[0]);
    events_grp_JOIN_INNER_TD_5425.push_back(events_JOIN_INNER_TD_9897[0]);
    krnl_JOIN_INNER_TD_5425_build.run(0, &(events_grp_JOIN_INNER_TD_5425), &(events_JOIN_INNER_TD_5425[0]));
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_5425_build_done;
    events_grp_JOIN_INNER_TD_5425_build_done.push_back(events_JOIN_INNER_TD_5425[0]);
    krnl_JOIN_INNER_TD_5425_probe.run(0, &(events_grp_JOIN_INNER_TD_5425_build_done), &(events_JOIN_INNER_TD_5425[1]));
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_5425_probe_done;
    events_grp_JOIN_INNER_TD_5425_probe_done.push_back(events_JOIN_INNER_TD_5425[1]);
    trans_JOIN_INNER_TD_5425_out.add(&(tbl_JOIN_INNER_TD_5425_output));
    trans_JOIN_INNER_TD_5425_out.add(&(cfg_JOIN_INNER_TD_5425_cmds_build));
    trans_JOIN_INNER_TD_5425_out.add(&(cfg_JOIN_INNER_TD_5425_cmds_probe));
    trans_JOIN_INNER_TD_5425_out.dev2host(0, &(events_grp_JOIN_INNER_TD_5425_probe_done), &(events_d2h_rd_JOIN_INNER_TD_5425[0]));
    q_h.flush();
    q_h.finish();
    tbl_JOIN_INNER_TD_5425_output.setNumRow((cfg_JOIN_INNER_TD_5425_cmds_probe.meta_out)->getColLen());
    tbl_JOIN_INNER_TD_5425_output.colToTable();
    SW_Consolidated_JOIN_INNER_TD_5425_output(tbl_JOIN_INNER_TD_9897_consolidated_output, tbl_Filter_TD_6465_output, tbl_JOIN_INNER_TD_5425_output, tbl_JOIN_INNER_TD_5425_consolidated_output);
    gettimeofday(&tv_r_JOIN_INNER_5_417_e, 0);

    struct timeval tv_r_Aggregate_6_709_s, tv_r_Aggregate_6_709_e;
    gettimeofday(&tv_r_Aggregate_6_709_s, 0);
    SW_Aggregate_TD_6224(tbl_JOIN_INNER_TD_5425_consolidated_output, tbl_Aggregate_TD_6224_output);
    gettimeofday(&tv_r_Aggregate_6_709_e, 0);

    struct timeval tv_r_JOIN_INNER_4_565_s, tv_r_JOIN_INNER_4_565_e;
    gettimeofday(&tv_r_JOIN_INNER_4_565_s, 0);
    prev_events_grp_JOIN_INNER_TD_4452.push_back(events_h2d_wr_JOIN_INNER_TD_5425[0]);
    tbl_Aggregate_TD_6224_output.tableToCol();
    tbl_JOIN_INNER_TD_5425_consolidated_output.tableToCol();
    krnl_JOIN_INNER_TD_4452_build.updateMeta(tbl_Aggregate_TD_6224_output.getNumRow(), 3);
    krnl_JOIN_INNER_TD_4452_probe.updateMeta(tbl_JOIN_INNER_TD_5425_consolidated_output.getNumRow(), 3);
    trans_JOIN_INNER_TD_4452.add(&(tbl_Aggregate_TD_6224_output));
    trans_JOIN_INNER_TD_4452.host2dev(0, &(prev_events_grp_JOIN_INNER_TD_4452), &(events_h2d_wr_JOIN_INNER_TD_4452[0]));
    events_grp_JOIN_INNER_TD_4452.push_back(events_h2d_wr_JOIN_INNER_TD_4452[0]);
    events_grp_JOIN_INNER_TD_4452.push_back(events_JOIN_INNER_TD_5425[0]);
    krnl_JOIN_INNER_TD_4452_build.run(0, &(events_grp_JOIN_INNER_TD_4452), &(events_JOIN_INNER_TD_4452[0]));
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_4452_build_done;
    events_grp_JOIN_INNER_TD_4452_build_done.push_back(events_JOIN_INNER_TD_4452[0]);
    krnl_JOIN_INNER_TD_4452_probe.run(0, &(events_grp_JOIN_INNER_TD_4452_build_done), &(events_JOIN_INNER_TD_4452[1]));
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_4452_probe_done;
    events_grp_JOIN_INNER_TD_4452_probe_done.push_back(events_JOIN_INNER_TD_4452[1]);
    trans_JOIN_INNER_TD_4452_out.add(&(tbl_JOIN_INNER_TD_4452_output));
    trans_JOIN_INNER_TD_4452_out.add(&(cfg_JOIN_INNER_TD_4452_cmds_build));
    trans_JOIN_INNER_TD_4452_out.add(&(cfg_JOIN_INNER_TD_4452_cmds_probe));
    trans_JOIN_INNER_TD_4452_out.dev2host(0, &(events_grp_JOIN_INNER_TD_4452_probe_done), &(events_d2h_rd_JOIN_INNER_TD_4452[0]));
    q_h.flush();
    q_h.finish();
    tbl_JOIN_INNER_TD_4452_output.setNumRow((cfg_JOIN_INNER_TD_4452_cmds_probe.meta_out)->getColLen());
    tbl_JOIN_INNER_TD_4452_output.colToTable();
    SW_Consolidated_JOIN_INNER_TD_4452_output(tbl_Aggregate_TD_6224_output, tbl_JOIN_INNER_TD_5425_consolidated_output, tbl_JOIN_INNER_TD_4452_output, tbl_JOIN_INNER_TD_4452_consolidated_output);
    gettimeofday(&tv_r_JOIN_INNER_4_565_e, 0);

    struct timeval tv_r_JOIN_INNER_3_404_s, tv_r_JOIN_INNER_3_404_e;
    gettimeofday(&tv_r_JOIN_INNER_3_404_s, 0);
    prev_events_grp_JOIN_INNER_TD_3352.push_back(events_h2d_wr_JOIN_INNER_TD_4452[0]);
    tbl_JOIN_INNER_TD_4452_consolidated_output.tableToCol();
    tbl_SerializeFromObject_TD_458_input_stringRowIDSubstitute.tableToCol();
    krnl_JOIN_INNER_TD_3352_build.updateMeta(tbl_JOIN_INNER_TD_4452_consolidated_output.getNumRow(), 3);
    krnl_JOIN_INNER_TD_3352_probe.updateMeta(tbl_SerializeFromObject_TD_458_input_stringRowIDSubstitute.getNumRow(), 3);
    trans_JOIN_INNER_TD_3352.host2dev(0, &(prev_events_grp_JOIN_INNER_TD_3352), &(events_h2d_wr_JOIN_INNER_TD_3352[0]));
    events_grp_JOIN_INNER_TD_3352.push_back(events_h2d_wr_JOIN_INNER_TD_3352[0]);
    events_grp_JOIN_INNER_TD_3352.push_back(events_JOIN_INNER_TD_4452[0]);
    krnl_JOIN_INNER_TD_3352_build.run(0, &(events_grp_JOIN_INNER_TD_3352), &(events_JOIN_INNER_TD_3352[0]));
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_3352_build_done;
    events_grp_JOIN_INNER_TD_3352_build_done.push_back(events_JOIN_INNER_TD_3352[0]);
    krnl_JOIN_INNER_TD_3352_probe.run(0, &(events_grp_JOIN_INNER_TD_3352_build_done), &(events_JOIN_INNER_TD_3352[1]));
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_3352_probe_done;
    events_grp_JOIN_INNER_TD_3352_probe_done.push_back(events_JOIN_INNER_TD_3352[1]);
    trans_JOIN_INNER_TD_3352_out.add(&(tbl_JOIN_INNER_TD_3352_output));
    trans_JOIN_INNER_TD_3352_out.add(&(cfg_JOIN_INNER_TD_3352_cmds_build));
    trans_JOIN_INNER_TD_3352_out.add(&(cfg_JOIN_INNER_TD_3352_cmds_probe));
    trans_JOIN_INNER_TD_3352_out.dev2host(0, &(events_grp_JOIN_INNER_TD_3352_probe_done), &(events_d2h_rd_JOIN_INNER_TD_3352[0]));
    q_h.flush();
    q_h.finish();
    tbl_JOIN_INNER_TD_3352_output.setNumRow((cfg_JOIN_INNER_TD_3352_cmds_probe.meta_out)->getColLen());
    tbl_JOIN_INNER_TD_3352_output.colToTable();
    SW_Consolidated_JOIN_INNER_TD_3352_output(tbl_JOIN_INNER_TD_4452_consolidated_output, tbl_SerializeFromObject_TD_458_input_stringRowIDSubstitute, tbl_JOIN_INNER_TD_3352_output, tbl_JOIN_INNER_TD_3352_consolidated_output);
    gettimeofday(&tv_r_JOIN_INNER_3_404_e, 0);

    struct timeval tv_r_JOIN_INNER_2_448_s, tv_r_JOIN_INNER_2_448_e;
    gettimeofday(&tv_r_JOIN_INNER_2_448_s, 0);
    SW_JOIN_INNER_TD_2465(tbl_JOIN_INNER_TD_3352_consolidated_output, tbl_SerializeFromObject_TD_373_input_stringRowIDSubstitute, tbl_JOIN_INNER_TD_2465_output);
    gettimeofday(&tv_r_JOIN_INNER_2_448_e, 0);

    struct timeval tv_r_JOIN_INNER_1_831_s, tv_r_JOIN_INNER_1_831_e;
    gettimeofday(&tv_r_JOIN_INNER_1_831_s, 0);
    SW_JOIN_INNER_TD_1483(tbl_JOIN_INNER_TD_2465_output, tbl_Filter_TD_8242_output, tbl_JOIN_INNER_TD_1483_output);
    gettimeofday(&tv_r_JOIN_INNER_1_831_e, 0);

    struct timeval tv_r_Sort_0_508_s, tv_r_Sort_0_508_e;
    gettimeofday(&tv_r_Sort_0_508_s, 0);
    SW_Sort_TD_0943(tbl_JOIN_INNER_TD_1483_output, tbl_SerializeFromObject_TD_737_input, tbl_SerializeFromObject_TD_458_input, tbl_SerializeFromObject_TD_373_input, tbl_Sort_TD_0943_output);
    gettimeofday(&tv_r_Sort_0_508_e, 0);

std::cout << "CPU TIME: " << (tvdiff(&tv_r_Filter_8_931_s, &tv_r_Filter_8_931_e) + 
tvdiff(&tv_r_Filter_6_540_s, &tv_r_Filter_6_540_e) + 
tvdiff(&tv_r_Aggregate_6_709_s, &tv_r_Aggregate_6_709_e) + 
tvdiff(&tv_r_JOIN_INNER_2_448_s, &tv_r_JOIN_INNER_2_448_e) + 
tvdiff(&tv_r_JOIN_INNER_1_831_s, &tv_r_JOIN_INNER_1_831_e) + 
tvdiff(&tv_r_Sort_0_508_s, &tv_r_Sort_0_508_e) + 
1) / 1000.0 << std::endl;

std::cout << "FPGA TIME: " << (tvdiff(&tv_r_JOIN_INNER_7_742_s, &tv_r_JOIN_INNER_7_742_e) + 
tvdiff(&tv_r_JOIN_INNER_8_691_s, &tv_r_JOIN_INNER_8_691_e) + 
tvdiff(&tv_r_JOIN_INNER_9_318_s, &tv_r_JOIN_INNER_9_318_e) + 
tvdiff(&tv_r_JOIN_INNER_5_417_s, &tv_r_JOIN_INNER_5_417_e) + 
tvdiff(&tv_r_JOIN_INNER_4_565_s, &tv_r_JOIN_INNER_4_565_e) + 
tvdiff(&tv_r_JOIN_INNER_3_404_s, &tv_r_JOIN_INNER_3_404_e) + 
1) / 1000.0 << std::endl;

    gettimeofday(&tv_r_e, 0); 
    // **************************** Print Execution Time ************************** // 
    std::cout << std::endl; 
    std::cout << "Filter_8: " << tvdiff(&tv_r_Filter_8_931_s, &tv_r_Filter_8_931_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer((isnotnull(r_name#175) AND (r_name#175 = AMERICA))) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_9675_input.getNumRow() << " -> tbl_SerializeFromObject_TD_9675_input" 
     << std::endl << "      #COL: 2: " << "ListBuffer(r_regionkey#174, r_name#175)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_8242_output.getNumRow() << " -> tbl_Filter_TD_8242_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(r_regionkey#174)" 
     << std::endl; 

    std::cout << "JOIN_INNER_7: " << tvdiff(&tv_r_JOIN_INNER_7_742_s, &tv_r_JOIN_INNER_7_742_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer((n_regionkey#146 = r_regionkey#174)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_9517_input.getNumRow() << " -> tbl_SerializeFromObject_TD_9517_input" 
     << std::endl << "      #COL: 2: " << "ListBuffer(n_nationkey#144, n_regionkey#146)" 
     << std::endl << "      #ROW: " << tbl_Filter_TD_8242_output.getNumRow() << " -> tbl_Filter_TD_8242_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(r_regionkey#174)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_7860_consolidated_output.getNumRow() << " -> tbl_JOIN_INNER_TD_7860_consolidated_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(n_nationkey#144)" 
     << std::endl; 

    std::cout << "JOIN_INNER_8: " << tvdiff(&tv_r_JOIN_INNER_8_691_s, &tv_r_JOIN_INNER_8_691_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer((s_nationkey#345 = n_nationkey#144)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_458_input_stringRowIDSubstitute.getNumRow() << " -> tbl_SerializeFromObject_TD_458_input_stringRowIDSubstitute" 
     << std::endl << "      #COL: 7: " << "ListBuffer(s_suppkey#342, s_name#343, s_address#344, s_nationkey#345, s_phone#346, s_acctbal#347, s_comment#348)" 
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_7860_consolidated_output.getNumRow() << " -> tbl_JOIN_INNER_TD_7860_consolidated_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(n_nationkey#144)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_8407_consolidated_output.getNumRow() << " -> tbl_JOIN_INNER_TD_8407_consolidated_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(s_suppkey#342)" 
     << std::endl; 

    std::cout << "JOIN_INNER_9: " << tvdiff(&tv_r_JOIN_INNER_9_318_s, &tv_r_JOIN_INNER_9_318_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer((s_suppkey#342 = ps_suppkey#306)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_8407_consolidated_output.getNumRow() << " -> tbl_JOIN_INNER_TD_8407_consolidated_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(s_suppkey#342)" 
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_10879_input.getNumRow() << " -> tbl_SerializeFromObject_TD_10879_input" 
     << std::endl << "      #COL: 3: " << "ListBuffer(ps_partkey#305, ps_suppkey#306, ps_supplycost#308)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_9897_consolidated_output.getNumRow() << " -> tbl_JOIN_INNER_TD_9897_consolidated_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(ps_partkey#305, ps_supplycost#308, ps_suppkey#306)" 
     << std::endl; 

    std::cout << "Filter_6: " << tvdiff(&tv_r_Filter_6_540_s, &tv_r_Filter_6_540_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer((isnotnull(p_type#262) AND ((p_size#263 = 19) AND EndsWith(p_type#262, NICKEL)))) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_737_input.getNumRow() << " -> tbl_SerializeFromObject_TD_737_input" 
     << std::endl << "      #COL: 4: " << "ListBuffer(p_partkey#258, p_mfgr#260, p_type#262, p_size#263)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_6465_output.getNumRow() << " -> tbl_Filter_TD_6465_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(p_partkey#258, p_mfgr#260)" 
     << std::endl; 

    std::cout << "JOIN_INNER_5: " << tvdiff(&tv_r_JOIN_INNER_5_417_s, &tv_r_JOIN_INNER_5_417_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer((p_partkey#258 = ps_partkey#305)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_6465_output.getNumRow() << " -> tbl_Filter_TD_6465_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(p_partkey#258, p_mfgr#260)" 
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_9897_consolidated_output.getNumRow() << " -> tbl_JOIN_INNER_TD_9897_consolidated_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(ps_partkey#305, ps_supplycost#308, ps_suppkey#306)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_5425_consolidated_output.getNumRow() << " -> tbl_JOIN_INNER_TD_5425_consolidated_output" 
     << std::endl << "      #COL: 5: " << "ListBuffer(p_partkey#258, p_mfgr#260, ps_suppkey#306, ps_supplycost#308, ps_partkey#305)" 
     << std::endl; 

    std::cout << "Aggregate_6: " << tvdiff(&tv_r_Aggregate_6_709_s, &tv_r_Aggregate_6_709_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(ps_partkey#305, min(ps_supplycost#308) AS min(ps_supplycost)#579, ps_partkey#305 AS ps_partkey#305#663) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_5425_consolidated_output.getNumRow() << " -> tbl_JOIN_INNER_TD_5425_consolidated_output" 
     << std::endl << "      #COL: 5: " << "ListBuffer(p_partkey#258, p_mfgr#260, ps_suppkey#306, ps_supplycost#308, ps_partkey#305)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_6224_output.getNumRow() << " -> tbl_Aggregate_TD_6224_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(min(ps_supplycost)#579, ps_partkey#305#663)" 
     << std::endl; 

    std::cout << "JOIN_INNER_4: " << tvdiff(&tv_r_JOIN_INNER_4_565_s, &tv_r_JOIN_INNER_4_565_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer(((ps_supplycost#308 = min(ps_supplycost)#579) AND (p_partkey#258 = ps_partkey#305#663))) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_5425_consolidated_output.getNumRow() << " -> tbl_JOIN_INNER_TD_5425_consolidated_output" 
     << std::endl << "      #COL: 5: " << "ListBuffer(p_partkey#258, p_mfgr#260, ps_suppkey#306, ps_supplycost#308, ps_partkey#305)" 
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_6224_output.getNumRow() << " -> tbl_Aggregate_TD_6224_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(min(ps_supplycost)#579, ps_partkey#305#663)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_4452_consolidated_output.getNumRow() << " -> tbl_JOIN_INNER_TD_4452_consolidated_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(p_partkey#258, p_mfgr#260, ps_suppkey#306)" 
     << std::endl; 

    std::cout << "JOIN_INNER_3: " << tvdiff(&tv_r_JOIN_INNER_3_404_s, &tv_r_JOIN_INNER_3_404_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer((s_suppkey#342 = ps_suppkey#306)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_4452_consolidated_output.getNumRow() << " -> tbl_JOIN_INNER_TD_4452_consolidated_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(p_partkey#258, p_mfgr#260, ps_suppkey#306)" 
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_458_input_stringRowIDSubstitute.getNumRow() << " -> tbl_SerializeFromObject_TD_458_input_stringRowIDSubstitute" 
     << std::endl << "      #COL: 7: " << "ListBuffer(s_suppkey#342, s_name#343, s_address#344, s_nationkey#345, s_phone#346, s_acctbal#347, s_comment#348)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_3352_consolidated_output.getNumRow() << " -> tbl_JOIN_INNER_TD_3352_consolidated_output" 
     << std::endl << "      #COL: 8: " << "ListBuffer(p_partkey#258, p_mfgr#260, s_name#343, s_address#344, s_nationkey#345, s_phone#346, s_acctbal#347, s_comment#348)" 
     << std::endl; 

    std::cout << "JOIN_INNER_2: " << tvdiff(&tv_r_JOIN_INNER_2_448_s, &tv_r_JOIN_INNER_2_448_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer((s_nationkey#345 = n_nationkey#144)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_3352_consolidated_output.getNumRow() << " -> tbl_JOIN_INNER_TD_3352_consolidated_output" 
     << std::endl << "      #COL: 8: " << "ListBuffer(p_partkey#258, p_mfgr#260, s_name#343, s_address#344, s_nationkey#345, s_phone#346, s_acctbal#347, s_comment#348)" 
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_373_input_stringRowIDSubstitute.getNumRow() << " -> tbl_SerializeFromObject_TD_373_input_stringRowIDSubstitute" 
     << std::endl << "      #COL: 3: " << "ListBuffer(n_nationkey#144, n_name#145, n_regionkey#146)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_2465_output.getNumRow() << " -> tbl_JOIN_INNER_TD_2465_output" 
     << std::endl << "      #COL: 9: " << "ListBuffer(p_partkey#258, p_mfgr#260, s_name#343, s_address#344, s_phone#346, s_acctbal#347, s_comment#348, n_name#145, n_regionkey#146)" 
     << std::endl; 

    std::cout << "JOIN_INNER_1: " << tvdiff(&tv_r_JOIN_INNER_1_831_s, &tv_r_JOIN_INNER_1_831_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer((n_regionkey#146 = r_regionkey#174)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_2465_output.getNumRow() << " -> tbl_JOIN_INNER_TD_2465_output" 
     << std::endl << "      #COL: 9: " << "ListBuffer(p_partkey#258, p_mfgr#260, s_name#343, s_address#344, s_phone#346, s_acctbal#347, s_comment#348, n_name#145, n_regionkey#146)" 
     << std::endl << "      #ROW: " << tbl_Filter_TD_8242_output.getNumRow() << " -> tbl_Filter_TD_8242_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(r_regionkey#174)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_1483_output.getNumRow() << " -> tbl_JOIN_INNER_TD_1483_output" 
     << std::endl << "      #COL: 8: " << "ListBuffer(s_acctbal#347, s_name#343, n_name#145, p_partkey#258, p_mfgr#260, s_address#344, s_phone#346, s_comment#348)" 
     << std::endl; 

    std::cout << "Sort_0: " << tvdiff(&tv_r_Sort_0_508_s, &tv_r_Sort_0_508_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(s_acctbal#347 DESC NULLS LAST, n_name#145 ASC NULLS FIRST, s_name#343 ASC NULLS FIRST, p_partkey#258 ASC NULLS FIRST) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_1483_output.getNumRow() << " -> tbl_JOIN_INNER_TD_1483_output" 
     << std::endl << "      #COL: 8: " << "ListBuffer(s_acctbal#347, s_name#343, n_name#145, p_partkey#258, p_mfgr#260, s_address#344, s_phone#346, s_comment#348)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Sort_TD_0943_output.getNumRow() << " -> tbl_Sort_TD_0943_output" 
     << std::endl << "      #COL: 8: " << "ListBuffer(s_acctbal#347, s_name#343, n_name#145, p_partkey#258, p_mfgr#260, s_address#344, s_phone#346, s_comment#348)" 
     << std::endl; 

    std::cout << std::endl << " Total execution time: " << tvdiff(&tv_r_s, &tv_r_e) / 1000 << " ms"; 

    std::cout << std::endl << " Spark elapsed time: " << 3.68033 * 1000 << "ms" << std::endl; 
    return 0; 
}
