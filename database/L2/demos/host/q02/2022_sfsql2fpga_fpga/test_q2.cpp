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
    // *********************** Partition Infomation ********************* // 
    int hpTimes_join = 32; 
    int power_of_hpTimes_join = log2(hpTimes_join); 
    std::cout << "Number of partition (gqeJoin) is: " << hpTimes_join << std::endl; 
    int hpTimes_aggr = 256; 
    int power_of_hpTimes_aggr = log2(hpTimes_aggr); 
    std::cout << "Number of partition (gqeAggr) is: " << hpTimes_aggr << std::endl; 
    // ****************************** Tables **************************** // 
    Table tbl_Sort_TD_0156_output("tbl_Sort_TD_0156_output", 183000000, 8, "");
    tbl_Sort_TD_0156_output.allocateHost();
    Table tbl_JOIN_INNER_TD_1647_output("tbl_JOIN_INNER_TD_1647_output", 183000000, 8, "");
    tbl_JOIN_INNER_TD_1647_output.allocateHost();
    Table tbl_JOIN_INNER_TD_2849_output("tbl_JOIN_INNER_TD_2849_output", 183000000, 9, "");
    tbl_JOIN_INNER_TD_2849_output.allocateHost();
    Table tbl_JOIN_INNER_TD_3450_output("tbl_JOIN_INNER_TD_3450_output", -1, 4, "");
    tbl_JOIN_INNER_TD_3450_output.allocateHost(1.2, hpTimes_join);
    Table tbl_JOIN_INNER_TD_3450_consolidated_output("tbl_JOIN_INNER_TD_3450_consolidated_output", -1, 8, "");
    tbl_JOIN_INNER_TD_3450_consolidated_output.selectOverlayVersion(1);
    tbl_JOIN_INNER_TD_3450_consolidated_output.allocateHost();
    Table tbl_SerializeFromObject_TD_3358_input_stringRowIDSubstitute;
    tbl_SerializeFromObject_TD_3358_input_stringRowIDSubstitute = Table("nation", nation_n, 3, in_dir);
    tbl_SerializeFromObject_TD_3358_input_stringRowIDSubstitute.addCol("n_nationkey", 4);
    tbl_SerializeFromObject_TD_3358_input_stringRowIDSubstitute.addCol("n_name", 4, 1, 0);
    tbl_SerializeFromObject_TD_3358_input_stringRowIDSubstitute.addCol("n_regionkey", 4);
    tbl_SerializeFromObject_TD_3358_input_stringRowIDSubstitute.allocateHost();
    tbl_SerializeFromObject_TD_3358_input_stringRowIDSubstitute.loadHost();
    Table tbl_SerializeFromObject_TD_3358_input;
    tbl_SerializeFromObject_TD_3358_input = Table("nation", nation_n, 3, in_dir);
    tbl_SerializeFromObject_TD_3358_input.addCol("n_nationkey", 4);
    tbl_SerializeFromObject_TD_3358_input.addCol("n_name", TPCH_READ_NATION_LEN+1);
    tbl_SerializeFromObject_TD_3358_input.addCol("n_regionkey", 4);
    tbl_SerializeFromObject_TD_3358_input.allocateHost();
    tbl_SerializeFromObject_TD_3358_input.loadHost();
    Table tbl_JOIN_INNER_TD_4265_output("tbl_JOIN_INNER_TD_4265_output", -1, 4, "");
    tbl_JOIN_INNER_TD_4265_output.allocateHost(1.2, hpTimes_join);
    Table tbl_JOIN_INNER_TD_4265_consolidated_output("tbl_JOIN_INNER_TD_4265_consolidated_output", -1, 3, "");
    tbl_JOIN_INNER_TD_4265_consolidated_output.selectOverlayVersion(1);
    tbl_JOIN_INNER_TD_4265_consolidated_output.allocateHost();
    Table tbl_Aggregate_TD_6602_output("tbl_Aggregate_TD_6602_output", 183000000, 2, "");
    tbl_Aggregate_TD_6602_output.selectOverlayVersion(0);
    tbl_Aggregate_TD_6602_output.allocateHost();
    Table tbl_Aggregate_TD_6602_output_partition("tbl_Aggregate_TD_6602_output_partition", 183000000, 2, "");
    tbl_Aggregate_TD_6602_output_partition.allocateHost(1.2, hpTimes_join);
    tbl_Aggregate_TD_6602_output_partition.allocateDevBuffer(context_h, 32);
    Table tbl_Aggregate_TD_6602_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_Aggregate_TD_6602_output_partition_array[i] = tbl_Aggregate_TD_6602_output_partition.createSubTable(i);
    }
    Table tbl_JOIN_INNER_TD_5245_output("tbl_JOIN_INNER_TD_5245_output", 628, 4, "");
    tbl_JOIN_INNER_TD_5245_output.allocateHost(1.2, hpTimes_join);
    Table tbl_JOIN_INNER_TD_5245_consolidated_output("tbl_JOIN_INNER_TD_5245_consolidated_output", 628, 5, "");
    tbl_JOIN_INNER_TD_5245_consolidated_output.selectOverlayVersion(1);
    tbl_JOIN_INNER_TD_5245_consolidated_output.allocateHost();
    Table tbl_Filter_TD_6491_output("tbl_Filter_TD_6491_output", 183000000, 2, "");
    tbl_Filter_TD_6491_output.selectOverlayVersion(1);
    tbl_Filter_TD_6491_output.allocateHost();
    Table tbl_Filter_TD_6491_output_partition("tbl_Filter_TD_6491_output_partition", 183000000, 2, "");
    tbl_Filter_TD_6491_output_partition.allocateHost(1.2, hpTimes_join);
    tbl_Filter_TD_6491_output_partition.allocateDevBuffer(context_h, 32);
    Table tbl_Filter_TD_6491_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_Filter_TD_6491_output_partition_array[i] = tbl_Filter_TD_6491_output_partition.createSubTable(i);
    }
    Table tbl_JOIN_INNER_TD_9816_output("tbl_JOIN_INNER_TD_9816_output", 162880, 4, "");
    tbl_JOIN_INNER_TD_9816_output.allocateHost(1.2, hpTimes_join);
    Table tbl_JOIN_INNER_TD_9816_consolidated_output("tbl_JOIN_INNER_TD_9816_consolidated_output", 162880, 3, "");
    tbl_JOIN_INNER_TD_9816_consolidated_output.selectOverlayVersion(1);
    tbl_JOIN_INNER_TD_9816_consolidated_output.allocateHost();
    Table tbl_SerializeFromObject_TD_7772_input;
    tbl_SerializeFromObject_TD_7772_input = Table("part", part_n, 4, in_dir);
    tbl_SerializeFromObject_TD_7772_input.addCol("p_partkey", 4);
    tbl_SerializeFromObject_TD_7772_input.addCol("p_mfgr", TPCH_READ_P_MFG_LEN+1);
    tbl_SerializeFromObject_TD_7772_input.addCol("p_type", TPCH_READ_P_TYPE_LEN+1);
    tbl_SerializeFromObject_TD_7772_input.addCol("p_size", 4);
    tbl_SerializeFromObject_TD_7772_input.allocateHost();
    tbl_SerializeFromObject_TD_7772_input.loadHost();
    Table tbl_JOIN_INNER_TD_8808_output("tbl_JOIN_INNER_TD_8808_output", 2036, 4, "");
    tbl_JOIN_INNER_TD_8808_output.allocateHost(1.2, hpTimes_join);
    Table tbl_JOIN_INNER_TD_8808_consolidated_output("tbl_JOIN_INNER_TD_8808_consolidated_output", 2036, 1, "");
    tbl_JOIN_INNER_TD_8808_consolidated_output.selectOverlayVersion(1);
    tbl_JOIN_INNER_TD_8808_consolidated_output.allocateHost();
    Table tbl_SerializeFromObject_TD_10433_input;
    tbl_SerializeFromObject_TD_10433_input = Table("partsupp", partsupp_n, 3, in_dir);
    tbl_SerializeFromObject_TD_10433_input.selectOverlayVersion(1);
    tbl_SerializeFromObject_TD_10433_input.addCol("ps_partkey", 4);
    tbl_SerializeFromObject_TD_10433_input.addCol("ps_suppkey", 4);
    tbl_SerializeFromObject_TD_10433_input.addCol("ps_supplycost", 4);
    tbl_SerializeFromObject_TD_10433_input.allocateHost();
    tbl_SerializeFromObject_TD_10433_input.loadHost();
    Table tbl_SerializeFromObject_TD_10433_input_partition("tbl_SerializeFromObject_TD_10433_input_partition", partsupp_n, 3, "");
    tbl_SerializeFromObject_TD_10433_input_partition.allocateHost(1.2, hpTimes_join);
    tbl_SerializeFromObject_TD_10433_input_partition.allocateDevBuffer(context_h, 32);
    Table tbl_SerializeFromObject_TD_10433_input_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_SerializeFromObject_TD_10433_input_partition_array[i] = tbl_SerializeFromObject_TD_10433_input_partition.createSubTable(i);
    }
    Table tbl_SerializeFromObject_TD_4212_input_stringRowIDSubstitute;
    tbl_SerializeFromObject_TD_4212_input_stringRowIDSubstitute = Table("supplier", supplier_n, 7, in_dir);
    tbl_SerializeFromObject_TD_4212_input_stringRowIDSubstitute.addCol("s_suppkey", 4);
    tbl_SerializeFromObject_TD_4212_input_stringRowIDSubstitute.addCol("s_name", 4, 1, 0);
    tbl_SerializeFromObject_TD_4212_input_stringRowIDSubstitute.addCol("s_address", 4, 1, 0);
    tbl_SerializeFromObject_TD_4212_input_stringRowIDSubstitute.addCol("s_nationkey", 4);
    tbl_SerializeFromObject_TD_4212_input_stringRowIDSubstitute.addCol("s_phone", 4, 1, 0);
    tbl_SerializeFromObject_TD_4212_input_stringRowIDSubstitute.addCol("s_acctbal", 4);
    tbl_SerializeFromObject_TD_4212_input_stringRowIDSubstitute.addCol("s_comment", 4, 1, 0);
    tbl_SerializeFromObject_TD_4212_input_stringRowIDSubstitute.allocateHost();
    tbl_SerializeFromObject_TD_4212_input_stringRowIDSubstitute.loadHost();
    Table tbl_SerializeFromObject_TD_4212_input_stringRowIDSubstitute_partition("tbl_SerializeFromObject_TD_4212_input_stringRowIDSubstitute_partition", supplier_n, 7, "");
    tbl_SerializeFromObject_TD_4212_input_stringRowIDSubstitute_partition.allocateHost(1.2, hpTimes_join);
    tbl_SerializeFromObject_TD_4212_input_stringRowIDSubstitute_partition.allocateDevBuffer(context_h, 32);
    Table tbl_SerializeFromObject_TD_4212_input_stringRowIDSubstitute_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_SerializeFromObject_TD_4212_input_stringRowIDSubstitute_partition_array[i] = tbl_SerializeFromObject_TD_4212_input_stringRowIDSubstitute_partition.createSubTable(i);
    }
    Table tbl_SerializeFromObject_TD_4212_input;
    tbl_SerializeFromObject_TD_4212_input = Table("supplier", supplier_n, 7, in_dir);
    tbl_SerializeFromObject_TD_4212_input.addCol("s_suppkey", 4);
    tbl_SerializeFromObject_TD_4212_input.addCol("s_name", TPCH_READ_S_NAME_LEN+1);
    tbl_SerializeFromObject_TD_4212_input.addCol("s_address", TPCH_READ_S_ADDR_MAX+1);
    tbl_SerializeFromObject_TD_4212_input.addCol("s_nationkey", 4);
    tbl_SerializeFromObject_TD_4212_input.addCol("s_phone", TPCH_READ_PHONE_LEN+1);
    tbl_SerializeFromObject_TD_4212_input.addCol("s_acctbal", 4);
    tbl_SerializeFromObject_TD_4212_input.addCol("s_comment", TPCH_READ_S_CMNT_MAX+1);
    tbl_SerializeFromObject_TD_4212_input.allocateHost();
    tbl_SerializeFromObject_TD_4212_input.loadHost();
    Table tbl_JOIN_INNER_TD_7380_output("tbl_JOIN_INNER_TD_7380_output", 5, 4, "");
    tbl_JOIN_INNER_TD_7380_output.allocateHost(1.2, hpTimes_join);
    Table tbl_JOIN_INNER_TD_7380_consolidated_output("tbl_JOIN_INNER_TD_7380_consolidated_output", 5, 1, "");
    tbl_JOIN_INNER_TD_7380_consolidated_output.selectOverlayVersion(1);
    tbl_JOIN_INNER_TD_7380_consolidated_output.allocateHost();
    Table tbl_SerializeFromObject_TD_9465_input;
    tbl_SerializeFromObject_TD_9465_input = Table("nation", nation_n, 2, in_dir);
    tbl_SerializeFromObject_TD_9465_input.selectOverlayVersion(1);
    tbl_SerializeFromObject_TD_9465_input.addCol("n_nationkey", 4);
    tbl_SerializeFromObject_TD_9465_input.addCol("n_regionkey", 4);
    tbl_SerializeFromObject_TD_9465_input.allocateHost();
    tbl_SerializeFromObject_TD_9465_input.loadHost();
    Table tbl_SerializeFromObject_TD_9465_input_partition("tbl_SerializeFromObject_TD_9465_input_partition", nation_n, 2, "");
    tbl_SerializeFromObject_TD_9465_input_partition.allocateHost(1.2, hpTimes_join);
    tbl_SerializeFromObject_TD_9465_input_partition.allocateDevBuffer(context_h, 32);
    Table tbl_SerializeFromObject_TD_9465_input_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_SerializeFromObject_TD_9465_input_partition_array[i] = tbl_SerializeFromObject_TD_9465_input_partition.createSubTable(i);
    }
    Table tbl_Filter_TD_8914_output("tbl_Filter_TD_8914_output", 183000000, 1, "");
    tbl_Filter_TD_8914_output.selectOverlayVersion(1);
    tbl_Filter_TD_8914_output.allocateHost();
    Table tbl_Filter_TD_8914_output_partition("tbl_Filter_TD_8914_output_partition", 183000000, 1, "");
    tbl_Filter_TD_8914_output_partition.allocateHost(1.2, hpTimes_join);
    tbl_Filter_TD_8914_output_partition.allocateDevBuffer(context_h, 32);
    Table tbl_Filter_TD_8914_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_Filter_TD_8914_output_partition_array[i] = tbl_Filter_TD_8914_output_partition.createSubTable(i);
    }
    Table tbl_SerializeFromObject_TD_9419_input;
    tbl_SerializeFromObject_TD_9419_input = Table("region", region_n, 2, in_dir);
    tbl_SerializeFromObject_TD_9419_input.addCol("r_regionkey", 4);
    tbl_SerializeFromObject_TD_9419_input.addCol("r_name", TPCH_READ_REGION_LEN+1);
    tbl_SerializeFromObject_TD_9419_input.allocateHost();
    tbl_SerializeFromObject_TD_9419_input.loadHost();
    // ********************** Allocate Device Buffer ******************** // 
    tbl_JOIN_INNER_TD_3450_output.allocateDevBuffer(context_h, 32);
    Table tbl_JOIN_INNER_TD_3450_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_JOIN_INNER_TD_3450_output_partition_array[i] = tbl_JOIN_INNER_TD_3450_output.createSubTable(i);
    }
    tbl_JOIN_INNER_TD_4265_output.allocateDevBuffer(context_h, 32);
    Table tbl_JOIN_INNER_TD_4265_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_JOIN_INNER_TD_4265_output_partition_array[i] = tbl_JOIN_INNER_TD_4265_output.createSubTable(i);
    }
    tbl_Aggregate_TD_6602_output.allocateDevBuffer(context_h, 33);
    tbl_JOIN_INNER_TD_5245_output.allocateDevBuffer(context_h, 32);
    Table tbl_JOIN_INNER_TD_5245_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_JOIN_INNER_TD_5245_output_partition_array[i] = tbl_JOIN_INNER_TD_5245_output.createSubTable(i);
    }
    tbl_Filter_TD_6491_output.allocateDevBuffer(context_h, 33);
    tbl_JOIN_INNER_TD_9816_output.allocateDevBuffer(context_h, 32);
    Table tbl_JOIN_INNER_TD_9816_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_JOIN_INNER_TD_9816_output_partition_array[i] = tbl_JOIN_INNER_TD_9816_output.createSubTable(i);
    }
    tbl_JOIN_INNER_TD_8808_output.allocateDevBuffer(context_h, 32);
    Table tbl_JOIN_INNER_TD_8808_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_JOIN_INNER_TD_8808_output_partition_array[i] = tbl_JOIN_INNER_TD_8808_output.createSubTable(i);
    }
    tbl_SerializeFromObject_TD_10433_input.allocateDevBuffer(context_h, 33);
    tbl_SerializeFromObject_TD_4212_input_stringRowIDSubstitute.allocateDevBuffer(context_h, 33);
    tbl_JOIN_INNER_TD_7380_output.allocateDevBuffer(context_h, 32);
    Table tbl_JOIN_INNER_TD_7380_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_JOIN_INNER_TD_7380_output_partition_array[i] = tbl_JOIN_INNER_TD_7380_output.createSubTable(i);
    }
    tbl_SerializeFromObject_TD_9465_input.allocateDevBuffer(context_h, 33);
    tbl_Filter_TD_8914_output.allocateDevBuffer(context_h, 33);
    // ****************************** Config **************************** // 
    
    xf::database::gqe::KernelCommand krn_cmd_build_336 = xf::database::gqe::KernelCommand();
    xf::database::gqe::KernelCommand krn_cmd_probe_336 = xf::database::gqe::KernelCommand();
    cfgCmd cfg_JOIN_INNER_TD_3450_cmds_build;
    cfg_JOIN_INNER_TD_3450_cmds_build.setup(1);
    cfg_JOIN_INNER_TD_3450_cmds_build.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_3450_gqe_join_build(krn_cmd_build_336);
    cfg_JOIN_INNER_TD_3450_cmds_build.cmd = krn_cmd_build_336.getConfigBits();
    get_meta_info_JOIN_INNER_TD_3450_gqe_join_build(cfg_JOIN_INNER_TD_3450_cmds_build.meta_in, tbl_JOIN_INNER_TD_4265_consolidated_output.nrow, 3);
    get_meta_info_JOIN_INNER_TD_3450_gqe_join_build(cfg_JOIN_INNER_TD_3450_cmds_build.meta_out, tbl_JOIN_INNER_TD_3450_output.nrow, 4);
    // cfg_JOIN_INNER_TD_3450_cmds_build.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_3450_cmds_probe;
    cfg_JOIN_INNER_TD_3450_cmds_probe.setup(1);
    cfg_JOIN_INNER_TD_3450_cmds_probe.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_3450_gqe_join_probe(krn_cmd_probe_336);
    cfg_JOIN_INNER_TD_3450_cmds_probe.cmd = krn_cmd_probe_336.getConfigBits();
    get_meta_info_JOIN_INNER_TD_3450_gqe_join_probe(cfg_JOIN_INNER_TD_3450_cmds_probe.meta_in, tbl_SerializeFromObject_TD_4212_input_stringRowIDSubstitute.nrow, 3);
    get_meta_info_JOIN_INNER_TD_3450_gqe_join_probe(cfg_JOIN_INNER_TD_3450_cmds_probe.meta_out, tbl_JOIN_INNER_TD_3450_output.nrow, 3);
    // cfg_JOIN_INNER_TD_3450_cmds_probe.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_3450_cmds_part;
    cfg_JOIN_INNER_TD_3450_cmds_part.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_3450_gqe_join_part (cfg_JOIN_INNER_TD_3450_cmds_part.cmd);
    cfg_JOIN_INNER_TD_3450_cmds_part.allocateDevBuffer(context_h, 32);
    cfg_JOIN_INNER_TD_3450_cmds_build.input_key_columns = {2, -1, -1};
    cfg_JOIN_INNER_TD_3450_cmds_probe.input_key_columns = {0, -1, -1};
    cfg_JOIN_INNER_TD_3450_cmds_build.output_key_columns = {0, 1, 2, 3};
    cfg_JOIN_INNER_TD_3450_cmds_probe.output_key_columns = {0, 1, 2, 3};
    
    xf::database::gqe::KernelCommand krn_cmd_build_578 = xf::database::gqe::KernelCommand();
    xf::database::gqe::KernelCommand krn_cmd_probe_578 = xf::database::gqe::KernelCommand();
    cfgCmd cfg_JOIN_INNER_TD_4265_cmds_build;
    cfg_JOIN_INNER_TD_4265_cmds_build.setup(1);
    cfg_JOIN_INNER_TD_4265_cmds_build.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_4265_gqe_join_build(krn_cmd_build_578);
    cfg_JOIN_INNER_TD_4265_cmds_build.cmd = krn_cmd_build_578.getConfigBits();
    get_meta_info_JOIN_INNER_TD_4265_gqe_join_build(cfg_JOIN_INNER_TD_4265_cmds_build.meta_in, tbl_Aggregate_TD_6602_output.nrow, 3);
    get_meta_info_JOIN_INNER_TD_4265_gqe_join_build(cfg_JOIN_INNER_TD_4265_cmds_build.meta_out, tbl_JOIN_INNER_TD_4265_output.nrow, 4);
    // cfg_JOIN_INNER_TD_4265_cmds_build.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_4265_cmds_probe;
    cfg_JOIN_INNER_TD_4265_cmds_probe.setup(1);
    cfg_JOIN_INNER_TD_4265_cmds_probe.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_4265_gqe_join_probe(krn_cmd_probe_578);
    cfg_JOIN_INNER_TD_4265_cmds_probe.cmd = krn_cmd_probe_578.getConfigBits();
    get_meta_info_JOIN_INNER_TD_4265_gqe_join_probe(cfg_JOIN_INNER_TD_4265_cmds_probe.meta_in, tbl_JOIN_INNER_TD_5245_consolidated_output.nrow, 3);
    get_meta_info_JOIN_INNER_TD_4265_gqe_join_probe(cfg_JOIN_INNER_TD_4265_cmds_probe.meta_out, tbl_JOIN_INNER_TD_4265_output.nrow, 3);
    // cfg_JOIN_INNER_TD_4265_cmds_probe.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_4265_cmds_part;
    cfg_JOIN_INNER_TD_4265_cmds_part.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_4265_gqe_join_part (cfg_JOIN_INNER_TD_4265_cmds_part.cmd);
    cfg_JOIN_INNER_TD_4265_cmds_part.allocateDevBuffer(context_h, 32);
    cfg_JOIN_INNER_TD_4265_cmds_build.input_key_columns = {0, 1, -1};
    cfg_JOIN_INNER_TD_4265_cmds_probe.input_key_columns = {3, 0, -1};
    cfg_JOIN_INNER_TD_4265_cmds_build.output_key_columns = {0, 1, 2, 3};
    cfg_JOIN_INNER_TD_4265_cmds_probe.output_key_columns = {0, 1, 2, 3};
    
    xf::database::gqe::KernelCommand krn_cmd_build_525 = xf::database::gqe::KernelCommand();
    xf::database::gqe::KernelCommand krn_cmd_probe_525 = xf::database::gqe::KernelCommand();
    cfgCmd cfg_JOIN_INNER_TD_5245_cmds_build;
    cfg_JOIN_INNER_TD_5245_cmds_build.setup(1);
    cfg_JOIN_INNER_TD_5245_cmds_build.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_5245_gqe_join_build(krn_cmd_build_525);
    cfg_JOIN_INNER_TD_5245_cmds_build.cmd = krn_cmd_build_525.getConfigBits();
    get_meta_info_JOIN_INNER_TD_5245_gqe_join_build(cfg_JOIN_INNER_TD_5245_cmds_build.meta_in, tbl_JOIN_INNER_TD_9816_consolidated_output.nrow, 3);
    get_meta_info_JOIN_INNER_TD_5245_gqe_join_build(cfg_JOIN_INNER_TD_5245_cmds_build.meta_out, tbl_JOIN_INNER_TD_5245_output.nrow, 4);
    // cfg_JOIN_INNER_TD_5245_cmds_build.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_5245_cmds_probe;
    cfg_JOIN_INNER_TD_5245_cmds_probe.setup(1);
    cfg_JOIN_INNER_TD_5245_cmds_probe.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_5245_gqe_join_probe(krn_cmd_probe_525);
    cfg_JOIN_INNER_TD_5245_cmds_probe.cmd = krn_cmd_probe_525.getConfigBits();
    get_meta_info_JOIN_INNER_TD_5245_gqe_join_probe(cfg_JOIN_INNER_TD_5245_cmds_probe.meta_in, tbl_Filter_TD_6491_output.nrow, 3);
    get_meta_info_JOIN_INNER_TD_5245_gqe_join_probe(cfg_JOIN_INNER_TD_5245_cmds_probe.meta_out, tbl_JOIN_INNER_TD_5245_output.nrow, 3);
    // cfg_JOIN_INNER_TD_5245_cmds_probe.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_5245_cmds_part;
    cfg_JOIN_INNER_TD_5245_cmds_part.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_5245_gqe_join_part (cfg_JOIN_INNER_TD_5245_cmds_part.cmd);
    cfg_JOIN_INNER_TD_5245_cmds_part.allocateDevBuffer(context_h, 32);
    cfg_JOIN_INNER_TD_5245_cmds_build.input_key_columns = {0, -1, -1};
    cfg_JOIN_INNER_TD_5245_cmds_probe.input_key_columns = {0, -1, -1};
    cfg_JOIN_INNER_TD_5245_cmds_build.output_key_columns = {0, 1, 2, 3};
    cfg_JOIN_INNER_TD_5245_cmds_probe.output_key_columns = {0, 1, 2, 3};
    
    xf::database::gqe::KernelCommand krn_cmd_build_372 = xf::database::gqe::KernelCommand();
    xf::database::gqe::KernelCommand krn_cmd_probe_372 = xf::database::gqe::KernelCommand();
    cfgCmd cfg_JOIN_INNER_TD_9816_cmds_build;
    cfg_JOIN_INNER_TD_9816_cmds_build.setup(1);
    cfg_JOIN_INNER_TD_9816_cmds_build.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_9816_gqe_join_build(krn_cmd_build_372);
    cfg_JOIN_INNER_TD_9816_cmds_build.cmd = krn_cmd_build_372.getConfigBits();
    get_meta_info_JOIN_INNER_TD_9816_gqe_join_build(cfg_JOIN_INNER_TD_9816_cmds_build.meta_in, tbl_JOIN_INNER_TD_8808_consolidated_output.nrow, 3);
    get_meta_info_JOIN_INNER_TD_9816_gqe_join_build(cfg_JOIN_INNER_TD_9816_cmds_build.meta_out, tbl_JOIN_INNER_TD_9816_output.nrow, 4);
    // cfg_JOIN_INNER_TD_9816_cmds_build.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_9816_cmds_probe;
    cfg_JOIN_INNER_TD_9816_cmds_probe.setup(1);
    cfg_JOIN_INNER_TD_9816_cmds_probe.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_9816_gqe_join_probe(krn_cmd_probe_372);
    cfg_JOIN_INNER_TD_9816_cmds_probe.cmd = krn_cmd_probe_372.getConfigBits();
    get_meta_info_JOIN_INNER_TD_9816_gqe_join_probe(cfg_JOIN_INNER_TD_9816_cmds_probe.meta_in, tbl_SerializeFromObject_TD_10433_input.nrow, 3);
    get_meta_info_JOIN_INNER_TD_9816_gqe_join_probe(cfg_JOIN_INNER_TD_9816_cmds_probe.meta_out, tbl_JOIN_INNER_TD_9816_output.nrow, 3);
    // cfg_JOIN_INNER_TD_9816_cmds_probe.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_9816_cmds_part;
    cfg_JOIN_INNER_TD_9816_cmds_part.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_9816_gqe_join_part (cfg_JOIN_INNER_TD_9816_cmds_part.cmd);
    cfg_JOIN_INNER_TD_9816_cmds_part.allocateDevBuffer(context_h, 32);
    cfg_JOIN_INNER_TD_9816_cmds_build.input_key_columns = {0, -1, -1};
    cfg_JOIN_INNER_TD_9816_cmds_probe.input_key_columns = {1, -1, -1};
    cfg_JOIN_INNER_TD_9816_cmds_build.output_key_columns = {0, 1, 2, 3};
    cfg_JOIN_INNER_TD_9816_cmds_probe.output_key_columns = {0, 1, 2, 3};
    
    xf::database::gqe::KernelCommand krn_cmd_build_684 = xf::database::gqe::KernelCommand();
    xf::database::gqe::KernelCommand krn_cmd_probe_684 = xf::database::gqe::KernelCommand();
    cfgCmd cfg_JOIN_INNER_TD_8808_cmds_build;
    cfg_JOIN_INNER_TD_8808_cmds_build.setup(1);
    cfg_JOIN_INNER_TD_8808_cmds_build.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_8808_gqe_join_build(krn_cmd_build_684);
    cfg_JOIN_INNER_TD_8808_cmds_build.cmd = krn_cmd_build_684.getConfigBits();
    get_meta_info_JOIN_INNER_TD_8808_gqe_join_build(cfg_JOIN_INNER_TD_8808_cmds_build.meta_in, tbl_SerializeFromObject_TD_4212_input_stringRowIDSubstitute.nrow, 3);
    get_meta_info_JOIN_INNER_TD_8808_gqe_join_build(cfg_JOIN_INNER_TD_8808_cmds_build.meta_out, tbl_JOIN_INNER_TD_8808_output.nrow, 4);
    // cfg_JOIN_INNER_TD_8808_cmds_build.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_8808_cmds_probe;
    cfg_JOIN_INNER_TD_8808_cmds_probe.setup(1);
    cfg_JOIN_INNER_TD_8808_cmds_probe.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_8808_gqe_join_probe(krn_cmd_probe_684);
    cfg_JOIN_INNER_TD_8808_cmds_probe.cmd = krn_cmd_probe_684.getConfigBits();
    get_meta_info_JOIN_INNER_TD_8808_gqe_join_probe(cfg_JOIN_INNER_TD_8808_cmds_probe.meta_in, tbl_JOIN_INNER_TD_7380_consolidated_output.nrow, 3);
    get_meta_info_JOIN_INNER_TD_8808_gqe_join_probe(cfg_JOIN_INNER_TD_8808_cmds_probe.meta_out, tbl_JOIN_INNER_TD_8808_output.nrow, 3);
    // cfg_JOIN_INNER_TD_8808_cmds_probe.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_8808_cmds_part;
    cfg_JOIN_INNER_TD_8808_cmds_part.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_8808_gqe_join_part (cfg_JOIN_INNER_TD_8808_cmds_part.cmd);
    cfg_JOIN_INNER_TD_8808_cmds_part.allocateDevBuffer(context_h, 32);
    cfg_JOIN_INNER_TD_8808_cmds_build.input_key_columns = {3, -1, -1};
    cfg_JOIN_INNER_TD_8808_cmds_probe.input_key_columns = {0, -1, -1};
    cfg_JOIN_INNER_TD_8808_cmds_build.output_key_columns = {0, 1, 2, 3};
    cfg_JOIN_INNER_TD_8808_cmds_probe.output_key_columns = {0, 1, 2, 3};
    
    xf::database::gqe::KernelCommand krn_cmd_build_33 = xf::database::gqe::KernelCommand();
    xf::database::gqe::KernelCommand krn_cmd_probe_33 = xf::database::gqe::KernelCommand();
    cfgCmd cfg_JOIN_INNER_TD_7380_cmds_build;
    cfg_JOIN_INNER_TD_7380_cmds_build.setup(1);
    cfg_JOIN_INNER_TD_7380_cmds_build.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_7380_gqe_join_build(krn_cmd_build_33);
    cfg_JOIN_INNER_TD_7380_cmds_build.cmd = krn_cmd_build_33.getConfigBits();
    get_meta_info_JOIN_INNER_TD_7380_gqe_join_build(cfg_JOIN_INNER_TD_7380_cmds_build.meta_in, tbl_SerializeFromObject_TD_9465_input.nrow, 3);
    get_meta_info_JOIN_INNER_TD_7380_gqe_join_build(cfg_JOIN_INNER_TD_7380_cmds_build.meta_out, tbl_JOIN_INNER_TD_7380_output.nrow, 4);
    // cfg_JOIN_INNER_TD_7380_cmds_build.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_7380_cmds_probe;
    cfg_JOIN_INNER_TD_7380_cmds_probe.setup(1);
    cfg_JOIN_INNER_TD_7380_cmds_probe.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_7380_gqe_join_probe(krn_cmd_probe_33);
    cfg_JOIN_INNER_TD_7380_cmds_probe.cmd = krn_cmd_probe_33.getConfigBits();
    get_meta_info_JOIN_INNER_TD_7380_gqe_join_probe(cfg_JOIN_INNER_TD_7380_cmds_probe.meta_in, tbl_Filter_TD_8914_output.nrow, 3);
    get_meta_info_JOIN_INNER_TD_7380_gqe_join_probe(cfg_JOIN_INNER_TD_7380_cmds_probe.meta_out, tbl_JOIN_INNER_TD_7380_output.nrow, 3);
    // cfg_JOIN_INNER_TD_7380_cmds_probe.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_7380_cmds_part;
    cfg_JOIN_INNER_TD_7380_cmds_part.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_7380_gqe_join_part (cfg_JOIN_INNER_TD_7380_cmds_part.cmd);
    cfg_JOIN_INNER_TD_7380_cmds_part.allocateDevBuffer(context_h, 32);
    cfg_JOIN_INNER_TD_7380_cmds_build.input_key_columns = {1, -1, -1};
    cfg_JOIN_INNER_TD_7380_cmds_probe.input_key_columns = {0, -1, -1};
    cfg_JOIN_INNER_TD_7380_cmds_build.output_key_columns = {0, 1, 2, 3};
    cfg_JOIN_INNER_TD_7380_cmds_probe.output_key_columns = {0, 1, 2, 3};
    // *************************** Kernel Setup ************************* // 
    bufferTmp buftmp_h(context_h, 1); 
    buftmp_h.initBuffer(q_h); 
    std::cout << std::endl; 
    krnlEngine krnl_JOIN_INNER_TD_3450_part_right;
    krnl_JOIN_INNER_TD_3450_part_right = krnlEngine(program_h, q_h, context_h, "gqePart");
    krnl_JOIN_INNER_TD_3450_part_right.setup_hp(512, 1, power_of_hpTimes_join, tbl_SerializeFromObject_TD_4212_input_stringRowIDSubstitute, tbl_SerializeFromObject_TD_4212_input_stringRowIDSubstitute_partition, cfg_JOIN_INNER_TD_3450_cmds_part);
    krnlEngine krnl_JOIN_INNER_TD_3450[hpTimes_join];
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_INNER_TD_3450[i] = krnlEngine(program_h, q_h, "gqeJoin");
    }
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_INNER_TD_3450[i].setup(tbl_JOIN_INNER_TD_4265_consolidated_output_partition_array[i], tbl_SerializeFromObject_TD_4212_input_stringRowIDSubstitute_partition_array[i], tbl_JOIN_INNER_TD_3450_output_partition_array[i], , buftmp_h);
    }
    krnlEngine krnl_JOIN_INNER_TD_3450_build;
    krnl_JOIN_INNER_TD_3450_build = krnlEngine(program_h, q_h, context_h, "gqeKernel", 0);
    krnl_JOIN_INNER_TD_3450_build.setup(tbl_JOIN_INNER_TD_4265_consolidated_output, tbl_SerializeFromObject_TD_4212_input_stringRowIDSubstitute, tbl_JOIN_INNER_TD_3450_output, cfg_JOIN_INNER_TD_3450_cmds_build, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_3450_probe;
    krnl_JOIN_INNER_TD_3450_probe = krnlEngine(program_h, q_h, context_h, "gqeKernel", 1);
    krnl_JOIN_INNER_TD_3450_probe.setup(tbl_SerializeFromObject_TD_4212_input_stringRowIDSubstitute, tbl_JOIN_INNER_TD_4265_consolidated_output, tbl_JOIN_INNER_TD_3450_output, cfg_JOIN_INNER_TD_3450_cmds_probe, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_4265_part_left;
    krnl_JOIN_INNER_TD_4265_part_left = krnlEngine(program_h, q_h, context_h, "gqePart");
    krnl_JOIN_INNER_TD_4265_part_left.setup_hp(512, 0, power_of_hpTimes_join, tbl_Aggregate_TD_6602_output, tbl_Aggregate_TD_6602_output_partition, cfg_JOIN_INNER_TD_4265_cmds_part);
    krnlEngine krnl_JOIN_INNER_TD_4265[hpTimes_join];
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_INNER_TD_4265[i] = krnlEngine(program_h, q_h, "gqeJoin");
    }
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_INNER_TD_4265[i].setup(tbl_Aggregate_TD_6602_output_partition_array[i], tbl_JOIN_INNER_TD_5245_consolidated_output_partition_array[i], tbl_JOIN_INNER_TD_4265_output_partition_array[i], , buftmp_h);
    }
    krnlEngine krnl_JOIN_INNER_TD_4265_build;
    krnl_JOIN_INNER_TD_4265_build = krnlEngine(program_h, q_h, context_h, "gqeKernel", 0);
    krnl_JOIN_INNER_TD_4265_build.setup(tbl_Aggregate_TD_6602_output, tbl_JOIN_INNER_TD_5245_consolidated_output, tbl_JOIN_INNER_TD_4265_output, cfg_JOIN_INNER_TD_4265_cmds_build, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_4265_probe;
    krnl_JOIN_INNER_TD_4265_probe = krnlEngine(program_h, q_h, context_h, "gqeKernel", 1);
    krnl_JOIN_INNER_TD_4265_probe.setup(tbl_JOIN_INNER_TD_5245_consolidated_output, tbl_Aggregate_TD_6602_output, tbl_JOIN_INNER_TD_4265_output, cfg_JOIN_INNER_TD_4265_cmds_probe, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_5245_part_right;
    krnl_JOIN_INNER_TD_5245_part_right = krnlEngine(program_h, q_h, context_h, "gqePart");
    krnl_JOIN_INNER_TD_5245_part_right.setup_hp(512, 1, power_of_hpTimes_join, tbl_Filter_TD_6491_output, tbl_Filter_TD_6491_output_partition, cfg_JOIN_INNER_TD_5245_cmds_part);
    krnlEngine krnl_JOIN_INNER_TD_5245[hpTimes_join];
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_INNER_TD_5245[i] = krnlEngine(program_h, q_h, "gqeJoin");
    }
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_INNER_TD_5245[i].setup(tbl_JOIN_INNER_TD_9816_consolidated_output_partition_array[i], tbl_Filter_TD_6491_output_partition_array[i], tbl_JOIN_INNER_TD_5245_output_partition_array[i], , buftmp_h);
    }
    krnlEngine krnl_JOIN_INNER_TD_5245_build;
    krnl_JOIN_INNER_TD_5245_build = krnlEngine(program_h, q_h, context_h, "gqeKernel", 0);
    krnl_JOIN_INNER_TD_5245_build.setup(tbl_JOIN_INNER_TD_9816_consolidated_output, tbl_Filter_TD_6491_output, tbl_JOIN_INNER_TD_5245_output, cfg_JOIN_INNER_TD_5245_cmds_build, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_5245_probe;
    krnl_JOIN_INNER_TD_5245_probe = krnlEngine(program_h, q_h, context_h, "gqeKernel", 1);
    krnl_JOIN_INNER_TD_5245_probe.setup(tbl_Filter_TD_6491_output, tbl_JOIN_INNER_TD_9816_consolidated_output, tbl_JOIN_INNER_TD_5245_output, cfg_JOIN_INNER_TD_5245_cmds_probe, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_9816_part_right;
    krnl_JOIN_INNER_TD_9816_part_right = krnlEngine(program_h, q_h, context_h, "gqePart");
    krnl_JOIN_INNER_TD_9816_part_right.setup_hp(512, 1, power_of_hpTimes_join, tbl_SerializeFromObject_TD_10433_input, tbl_SerializeFromObject_TD_10433_input_partition, cfg_JOIN_INNER_TD_9816_cmds_part);
    krnlEngine krnl_JOIN_INNER_TD_9816[hpTimes_join];
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_INNER_TD_9816[i] = krnlEngine(program_h, q_h, "gqeJoin");
    }
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_INNER_TD_9816[i].setup(tbl_JOIN_INNER_TD_8808_consolidated_output_partition_array[i], tbl_SerializeFromObject_TD_10433_input_partition_array[i], tbl_JOIN_INNER_TD_9816_output_partition_array[i], , buftmp_h);
    }
    krnlEngine krnl_JOIN_INNER_TD_9816_build;
    krnl_JOIN_INNER_TD_9816_build = krnlEngine(program_h, q_h, context_h, "gqeKernel", 0);
    krnl_JOIN_INNER_TD_9816_build.setup(tbl_JOIN_INNER_TD_8808_consolidated_output, tbl_SerializeFromObject_TD_10433_input, tbl_JOIN_INNER_TD_9816_output, cfg_JOIN_INNER_TD_9816_cmds_build, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_9816_probe;
    krnl_JOIN_INNER_TD_9816_probe = krnlEngine(program_h, q_h, context_h, "gqeKernel", 1);
    krnl_JOIN_INNER_TD_9816_probe.setup(tbl_SerializeFromObject_TD_10433_input, tbl_JOIN_INNER_TD_8808_consolidated_output, tbl_JOIN_INNER_TD_9816_output, cfg_JOIN_INNER_TD_9816_cmds_probe, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_8808_part_left;
    krnl_JOIN_INNER_TD_8808_part_left = krnlEngine(program_h, q_h, context_h, "gqePart");
    krnl_JOIN_INNER_TD_8808_part_left.setup_hp(512, 0, power_of_hpTimes_join, tbl_SerializeFromObject_TD_4212_input_stringRowIDSubstitute, tbl_SerializeFromObject_TD_4212_input_stringRowIDSubstitute_partition, cfg_JOIN_INNER_TD_8808_cmds_part);
    krnlEngine krnl_JOIN_INNER_TD_8808[hpTimes_join];
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_INNER_TD_8808[i] = krnlEngine(program_h, q_h, "gqeJoin");
    }
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_INNER_TD_8808[i].setup(tbl_SerializeFromObject_TD_4212_input_stringRowIDSubstitute_partition_array[i], tbl_JOIN_INNER_TD_7380_consolidated_output_partition_array[i], tbl_JOIN_INNER_TD_8808_output_partition_array[i], , buftmp_h);
    }
    krnlEngine krnl_JOIN_INNER_TD_8808_build;
    krnl_JOIN_INNER_TD_8808_build = krnlEngine(program_h, q_h, context_h, "gqeKernel", 0);
    krnl_JOIN_INNER_TD_8808_build.setup(tbl_SerializeFromObject_TD_4212_input_stringRowIDSubstitute, tbl_JOIN_INNER_TD_7380_consolidated_output, tbl_JOIN_INNER_TD_8808_output, cfg_JOIN_INNER_TD_8808_cmds_build, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_8808_probe;
    krnl_JOIN_INNER_TD_8808_probe = krnlEngine(program_h, q_h, context_h, "gqeKernel", 1);
    krnl_JOIN_INNER_TD_8808_probe.setup(tbl_JOIN_INNER_TD_7380_consolidated_output, tbl_SerializeFromObject_TD_4212_input_stringRowIDSubstitute, tbl_JOIN_INNER_TD_8808_output, cfg_JOIN_INNER_TD_8808_cmds_probe, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_7380_part_left;
    krnl_JOIN_INNER_TD_7380_part_left = krnlEngine(program_h, q_h, context_h, "gqePart");
    krnl_JOIN_INNER_TD_7380_part_left.setup_hp(512, 0, power_of_hpTimes_join, tbl_SerializeFromObject_TD_9465_input, tbl_SerializeFromObject_TD_9465_input_partition, cfg_JOIN_INNER_TD_7380_cmds_part);
    krnlEngine krnl_JOIN_INNER_TD_7380_part_right;
    krnl_JOIN_INNER_TD_7380_part_right = krnlEngine(program_h, q_h, context_h, "gqePart");
    krnl_JOIN_INNER_TD_7380_part_right.setup_hp(512, 1, power_of_hpTimes_join, tbl_Filter_TD_8914_output, tbl_Filter_TD_8914_output_partition, cfg_JOIN_INNER_TD_7380_cmds_part);
    krnlEngine krnl_JOIN_INNER_TD_7380[hpTimes_join];
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_INNER_TD_7380[i] = krnlEngine(program_h, q_h, "gqeJoin");
    }
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_INNER_TD_7380[i].setup(tbl_SerializeFromObject_TD_9465_input_partition_array[i], tbl_Filter_TD_8914_output_partition_array[i], tbl_JOIN_INNER_TD_7380_output_partition_array[i], , buftmp_h);
    }
    krnlEngine krnl_JOIN_INNER_TD_7380_build;
    krnl_JOIN_INNER_TD_7380_build = krnlEngine(program_h, q_h, context_h, "gqeKernel", 0);
    krnl_JOIN_INNER_TD_7380_build.setup(tbl_SerializeFromObject_TD_9465_input, tbl_Filter_TD_8914_output, tbl_JOIN_INNER_TD_7380_output, cfg_JOIN_INNER_TD_7380_cmds_build, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_7380_probe;
    krnl_JOIN_INNER_TD_7380_probe = krnlEngine(program_h, q_h, context_h, "gqeKernel", 1);
    krnl_JOIN_INNER_TD_7380_probe.setup(tbl_Filter_TD_8914_output, tbl_SerializeFromObject_TD_9465_input, tbl_JOIN_INNER_TD_7380_output, cfg_JOIN_INNER_TD_7380_cmds_probe, buftmp_h);
    // ************************** Transfer Engine *********************** // 
    transEngine trans_JOIN_INNER_TD_3450;
    trans_JOIN_INNER_TD_3450.setq(q_h,1);
    trans_JOIN_INNER_TD_3450.add(&(cfg_JOIN_INNER_TD_3450_cmds_part));
    trans_JOIN_INNER_TD_3450.add(&(cfg_JOIN_INNER_TD_3450_cmds_build));
    trans_JOIN_INNER_TD_3450.add(&(cfg_JOIN_INNER_TD_3450_cmds_probe));
    trans_JOIN_INNER_TD_3450.add(&(tbl_SerializeFromObject_TD_4212_input_stringRowIDSubstitute));
    transEngine trans_JOIN_INNER_TD_3450_out;
    trans_JOIN_INNER_TD_3450_out.setq(q_h,1);
    q_h.finish();
    transEngine trans_JOIN_INNER_TD_4265;
    trans_JOIN_INNER_TD_4265.setq(q_h,1);
    trans_JOIN_INNER_TD_4265.add(&(cfg_JOIN_INNER_TD_4265_cmds_part));
    trans_JOIN_INNER_TD_4265.add(&(cfg_JOIN_INNER_TD_4265_cmds_build));
    trans_JOIN_INNER_TD_4265.add(&(cfg_JOIN_INNER_TD_4265_cmds_probe));
    transEngine trans_JOIN_INNER_TD_4265_out;
    trans_JOIN_INNER_TD_4265_out.setq(q_h,1);
    q_h.finish();
    transEngine trans_JOIN_INNER_TD_5245;
    trans_JOIN_INNER_TD_5245.setq(q_h,1);
    trans_JOIN_INNER_TD_5245.add(&(cfg_JOIN_INNER_TD_5245_cmds_part));
    trans_JOIN_INNER_TD_5245.add(&(cfg_JOIN_INNER_TD_5245_cmds_build));
    trans_JOIN_INNER_TD_5245.add(&(cfg_JOIN_INNER_TD_5245_cmds_probe));
    transEngine trans_JOIN_INNER_TD_5245_out;
    trans_JOIN_INNER_TD_5245_out.setq(q_h,1);
    q_h.finish();
    transEngine trans_JOIN_INNER_TD_9816;
    trans_JOIN_INNER_TD_9816.setq(q_h,1);
    trans_JOIN_INNER_TD_9816.add(&(cfg_JOIN_INNER_TD_9816_cmds_part));
    trans_JOIN_INNER_TD_9816.add(&(cfg_JOIN_INNER_TD_9816_cmds_build));
    trans_JOIN_INNER_TD_9816.add(&(cfg_JOIN_INNER_TD_9816_cmds_probe));
    trans_JOIN_INNER_TD_9816.add(&(tbl_SerializeFromObject_TD_10433_input));
    transEngine trans_JOIN_INNER_TD_9816_out;
    trans_JOIN_INNER_TD_9816_out.setq(q_h,1);
    q_h.finish();
    transEngine trans_JOIN_INNER_TD_8808;
    trans_JOIN_INNER_TD_8808.setq(q_h,1);
    trans_JOIN_INNER_TD_8808.add(&(cfg_JOIN_INNER_TD_8808_cmds_part));
    trans_JOIN_INNER_TD_8808.add(&(cfg_JOIN_INNER_TD_8808_cmds_build));
    trans_JOIN_INNER_TD_8808.add(&(cfg_JOIN_INNER_TD_8808_cmds_probe));
    trans_JOIN_INNER_TD_8808.add(&(tbl_SerializeFromObject_TD_4212_input_stringRowIDSubstitute));
    transEngine trans_JOIN_INNER_TD_8808_out;
    trans_JOIN_INNER_TD_8808_out.setq(q_h,1);
    q_h.finish();
    transEngine trans_JOIN_INNER_TD_7380;
    trans_JOIN_INNER_TD_7380.setq(q_h,1);
    trans_JOIN_INNER_TD_7380.add(&(cfg_JOIN_INNER_TD_7380_cmds_part));
    trans_JOIN_INNER_TD_7380.add(&(cfg_JOIN_INNER_TD_7380_cmds_build));
    trans_JOIN_INNER_TD_7380.add(&(cfg_JOIN_INNER_TD_7380_cmds_probe));
    trans_JOIN_INNER_TD_7380.add(&(tbl_SerializeFromObject_TD_9465_input));
    transEngine trans_JOIN_INNER_TD_7380_out;
    trans_JOIN_INNER_TD_7380_out.setq(q_h,1);
    q_h.finish();
    // ****************************** Events **************************** // 
    std::vector<cl::Event> events_h2d_wr_JOIN_INNER_TD_3450;
    std::vector<cl::Event> events_d2h_rd_JOIN_INNER_TD_3450;
    std::vector<cl::Event> events_JOIN_INNER_TD_3450[2];
    events_h2d_wr_JOIN_INNER_TD_3450.resize(1);
    events_d2h_rd_JOIN_INNER_TD_3450.resize(1);
    events_JOIN_INNER_TD_3450[0].resize(1);
    events_JOIN_INNER_TD_3450[1].resize(hpTimes_join);
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_3450;
    std::vector<cl::Event> prev_events_grp_JOIN_INNER_TD_3450;
    std::vector<cl::Event> events_h2d_wr_JOIN_INNER_TD_4265;
    std::vector<cl::Event> events_d2h_rd_JOIN_INNER_TD_4265;
    std::vector<cl::Event> events_JOIN_INNER_TD_4265[2];
    events_h2d_wr_JOIN_INNER_TD_4265.resize(1);
    events_d2h_rd_JOIN_INNER_TD_4265.resize(1);
    events_JOIN_INNER_TD_4265[0].resize(1);
    events_JOIN_INNER_TD_4265[1].resize(hpTimes_join);
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_4265;
    std::vector<cl::Event> prev_events_grp_JOIN_INNER_TD_4265;
    std::vector<cl::Event> events_h2d_wr_JOIN_INNER_TD_5245;
    std::vector<cl::Event> events_d2h_rd_JOIN_INNER_TD_5245;
    std::vector<cl::Event> events_JOIN_INNER_TD_5245[2];
    events_h2d_wr_JOIN_INNER_TD_5245.resize(1);
    events_d2h_rd_JOIN_INNER_TD_5245.resize(1);
    events_JOIN_INNER_TD_5245[0].resize(1);
    events_JOIN_INNER_TD_5245[1].resize(hpTimes_join);
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_5245;
    std::vector<cl::Event> prev_events_grp_JOIN_INNER_TD_5245;
    std::vector<cl::Event> events_h2d_wr_JOIN_INNER_TD_9816;
    std::vector<cl::Event> events_d2h_rd_JOIN_INNER_TD_9816;
    std::vector<cl::Event> events_JOIN_INNER_TD_9816[2];
    events_h2d_wr_JOIN_INNER_TD_9816.resize(1);
    events_d2h_rd_JOIN_INNER_TD_9816.resize(1);
    events_JOIN_INNER_TD_9816[0].resize(1);
    events_JOIN_INNER_TD_9816[1].resize(hpTimes_join);
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_9816;
    std::vector<cl::Event> prev_events_grp_JOIN_INNER_TD_9816;
    std::vector<cl::Event> events_h2d_wr_JOIN_INNER_TD_8808;
    std::vector<cl::Event> events_d2h_rd_JOIN_INNER_TD_8808;
    std::vector<cl::Event> events_JOIN_INNER_TD_8808[2];
    events_h2d_wr_JOIN_INNER_TD_8808.resize(1);
    events_d2h_rd_JOIN_INNER_TD_8808.resize(1);
    events_JOIN_INNER_TD_8808[0].resize(1);
    events_JOIN_INNER_TD_8808[1].resize(hpTimes_join);
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_8808;
    std::vector<cl::Event> prev_events_grp_JOIN_INNER_TD_8808;
    std::vector<cl::Event> events_h2d_wr_JOIN_INNER_TD_7380;
    std::vector<cl::Event> events_d2h_rd_JOIN_INNER_TD_7380;
    std::vector<cl::Event> events_JOIN_INNER_TD_7380[2];
    events_h2d_wr_JOIN_INNER_TD_7380.resize(1);
    events_d2h_rd_JOIN_INNER_TD_7380.resize(1);
    events_JOIN_INNER_TD_7380[0].resize(2);
    events_JOIN_INNER_TD_7380[1].resize(hpTimes_join);
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_7380;
    std::vector<cl::Event> prev_events_grp_JOIN_INNER_TD_7380;
    // **************************** Operations ************************** // 
    struct timeval tv_r_s, tv_r_e; 
    gettimeofday(&tv_r_s, 0); 

    struct timeval tv_r_Filter_8_53_s, tv_r_Filter_8_53_e;
    gettimeofday(&tv_r_Filter_8_53_s, 0);
    SW_Filter_TD_8914(tbl_SerializeFromObject_TD_9419_input, tbl_Filter_TD_8914_output);
    gettimeofday(&tv_r_Filter_8_53_e, 0);

    struct timeval tv_r_JOIN_INNER_7_584_s, tv_r_JOIN_INNER_7_584_e;
    gettimeofday(&tv_r_JOIN_INNER_7_584_s, 0);
    tbl_SerializeFromObject_TD_9465_input.tableToCol();
    tbl_Filter_TD_8914_output.tableToCol();
    krnl_JOIN_INNER_TD_7380_build.updateMeta(tbl_SerializeFromObject_TD_9465_input.getNumRow(), 3);
    krnl_JOIN_INNER_TD_7380_probe.updateMeta(tbl_Filter_TD_8914_output.getNumRow(), 3);
    trans_JOIN_INNER_TD_7380.add(&(tbl_Filter_TD_8914_output));
    trans_JOIN_INNER_TD_7380.host2dev(0, &(prev_events_grp_JOIN_INNER_TD_7380), &(events_h2d_wr_JOIN_INNER_TD_7380[0]));
    events_grp_JOIN_INNER_TD_7380.push_back(events_h2d_wr_JOIN_INNER_TD_7380[0]);
    krnl_JOIN_INNER_TD_7380_part_left.run(0, &(events_grp_JOIN_INNER_TD_7380), &(events_JOIN_INNER_TD_7380[0][0]));
    krnl_JOIN_INNER_TD_7380_part_right.run(0, &(events_grp_JOIN_INNER_TD_7380), &(events_JOIN_INNER_TD_7380[0][1]));
    for (int i(0); i < hpTimes_join; ++i) {
        krnl_JOIN_INNER_TD_7380[i].run(0, &(events_JOIN_INNER_TD_7380[0]), &(events_JOIN_INNER_TD_7380[1][i]));
    }
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_7380_build_done;
    events_grp_JOIN_INNER_TD_7380_build_done.push_back(events_JOIN_INNER_TD_7380[0]);
    krnl_JOIN_INNER_TD_7380_part_left.run(0, &(events_grp_JOIN_INNER_TD_7380), &(events_JOIN_INNER_TD_7380[0][0]));
    krnl_JOIN_INNER_TD_7380_part_right.run(0, &(events_grp_JOIN_INNER_TD_7380), &(events_JOIN_INNER_TD_7380[0][1]));
    for (int i(0); i < hpTimes_join; ++i) {
        krnl_JOIN_INNER_TD_7380[i].run(0, &(events_JOIN_INNER_TD_7380[0]), &(events_JOIN_INNER_TD_7380[1][i]));
    }
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_7380_probe_done;
    events_grp_JOIN_INNER_TD_7380_probe_done.push_back(events_JOIN_INNER_TD_7380[1]);
    for (int i(0); i < hpTimes_join; ++i) {
        trans_JOIN_INNER_TD_7380_out.add(&(tbl_JOIN_INNER_TD_7380_output_partition_array[i]));
    }
    trans_JOIN_INNER_TD_7380_out.dev2host(0, &(events_JOIN_INNER_TD_7380[1]), &(events_d2h_rd_JOIN_INNER_TD_7380[0]));
    q_h.flush();
    q_h.finish();
    tbl_JOIN_INNER_TD_7380_output.setNumRow((cfg_JOIN_INNER_TD_7380_cmds_probe.meta_out)->getColLen());
    tbl_JOIN_INNER_TD_7380_output.colToTable();
    SW_Consolidated_JOIN_INNER_TD_7380_output(tbl_SerializeFromObject_TD_9465_input, tbl_Filter_TD_8914_output, tbl_JOIN_INNER_TD_7380_output, tbl_JOIN_INNER_TD_7380_consolidated_output);
    gettimeofday(&tv_r_JOIN_INNER_7_584_e, 0);

    struct timeval tv_r_JOIN_INNER_8_266_s, tv_r_JOIN_INNER_8_266_e;
    gettimeofday(&tv_r_JOIN_INNER_8_266_s, 0);
    prev_events_grp_JOIN_INNER_TD_8808.push_back(events_h2d_wr_JOIN_INNER_TD_7380[0]);
    tbl_SerializeFromObject_TD_4212_input_stringRowIDSubstitute.tableToCol();
    tbl_JOIN_INNER_TD_7380_consolidated_output.tableToCol();
    krnl_JOIN_INNER_TD_8808_build.updateMeta(tbl_SerializeFromObject_TD_4212_input_stringRowIDSubstitute.getNumRow(), 3);
    krnl_JOIN_INNER_TD_8808_probe.updateMeta(tbl_JOIN_INNER_TD_7380_consolidated_output.getNumRow(), 3);
    trans_JOIN_INNER_TD_8808.host2dev(0, &(prev_events_grp_JOIN_INNER_TD_8808), &(events_h2d_wr_JOIN_INNER_TD_8808[0]));
    events_grp_JOIN_INNER_TD_8808.push_back(events_h2d_wr_JOIN_INNER_TD_8808[0]);
    events_grp_JOIN_INNER_TD_8808.insert(std::end(events_grp_JOIN_INNER_TD_8808), std::begin(events_JOIN_INNER_TD_7380[0]), std::end(events_JOIN_INNER_TD_7380[0]));
    events_grp_JOIN_INNER_TD_8808.insert(std::end(events_grp_JOIN_INNER_TD_8808), std::begin(events_JOIN_INNER_TD_7380[1]), std::end(events_JOIN_INNER_TD_7380[1]));
    krnl_JOIN_INNER_TD_8808_part_left.run(0, &(events_grp_JOIN_INNER_TD_8808), &(events_JOIN_INNER_TD_8808[0][0]));
    for (int i(0); i < hpTimes_join; ++i) {
        krnl_JOIN_INNER_TD_8808[i].run(0, &(events_JOIN_INNER_TD_8808[0]), &(events_JOIN_INNER_TD_8808[1][i]));
    }
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_8808_build_done;
    events_grp_JOIN_INNER_TD_8808_build_done.push_back(events_JOIN_INNER_TD_8808[0]);
    krnl_JOIN_INNER_TD_8808_part_left.run(0, &(events_grp_JOIN_INNER_TD_8808), &(events_JOIN_INNER_TD_8808[0][0]));
    for (int i(0); i < hpTimes_join; ++i) {
        krnl_JOIN_INNER_TD_8808[i].run(0, &(events_JOIN_INNER_TD_8808[0]), &(events_JOIN_INNER_TD_8808[1][i]));
    }
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_8808_probe_done;
    events_grp_JOIN_INNER_TD_8808_probe_done.push_back(events_JOIN_INNER_TD_8808[1]);
    for (int i(0); i < hpTimes_join; ++i) {
        trans_JOIN_INNER_TD_8808_out.add(&(tbl_JOIN_INNER_TD_8808_output_partition_array[i]));
    }
    trans_JOIN_INNER_TD_8808_out.dev2host(0, &(events_JOIN_INNER_TD_8808[1]), &(events_d2h_rd_JOIN_INNER_TD_8808[0]));
    q_h.flush();
    q_h.finish();
    tbl_JOIN_INNER_TD_8808_output.setNumRow((cfg_JOIN_INNER_TD_8808_cmds_probe.meta_out)->getColLen());
    tbl_JOIN_INNER_TD_8808_output.colToTable();
    SW_Consolidated_JOIN_INNER_TD_8808_output(tbl_SerializeFromObject_TD_4212_input_stringRowIDSubstitute, tbl_JOIN_INNER_TD_7380_consolidated_output, tbl_JOIN_INNER_TD_8808_output, tbl_JOIN_INNER_TD_8808_consolidated_output);
    gettimeofday(&tv_r_JOIN_INNER_8_266_e, 0);

    struct timeval tv_r_JOIN_INNER_9_930_s, tv_r_JOIN_INNER_9_930_e;
    gettimeofday(&tv_r_JOIN_INNER_9_930_s, 0);
    prev_events_grp_JOIN_INNER_TD_9816.push_back(events_h2d_wr_JOIN_INNER_TD_8808[0]);
    tbl_JOIN_INNER_TD_8808_consolidated_output.tableToCol();
    tbl_SerializeFromObject_TD_10433_input.tableToCol();
    krnl_JOIN_INNER_TD_9816_build.updateMeta(tbl_JOIN_INNER_TD_8808_consolidated_output.getNumRow(), 3);
    krnl_JOIN_INNER_TD_9816_probe.updateMeta(tbl_SerializeFromObject_TD_10433_input.getNumRow(), 3);
    trans_JOIN_INNER_TD_9816.host2dev(0, &(prev_events_grp_JOIN_INNER_TD_9816), &(events_h2d_wr_JOIN_INNER_TD_9816[0]));
    events_grp_JOIN_INNER_TD_9816.push_back(events_h2d_wr_JOIN_INNER_TD_9816[0]);
    events_grp_JOIN_INNER_TD_9816.insert(std::end(events_grp_JOIN_INNER_TD_9816), std::begin(events_JOIN_INNER_TD_8808[0]), std::end(events_JOIN_INNER_TD_8808[0]));
    events_grp_JOIN_INNER_TD_9816.insert(std::end(events_grp_JOIN_INNER_TD_9816), std::begin(events_JOIN_INNER_TD_8808[1]), std::end(events_JOIN_INNER_TD_8808[1]));
    krnl_JOIN_INNER_TD_9816_part_right.run(0, &(events_grp_JOIN_INNER_TD_9816), &(events_JOIN_INNER_TD_9816[0][0]));
    for (int i(0); i < hpTimes_join; ++i) {
        krnl_JOIN_INNER_TD_9816[i].run(0, &(events_grp_JOIN_INNER_TD_9816), &(events_JOIN_INNER_TD_9816[1][i]));
    }
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_9816_build_done;
    events_grp_JOIN_INNER_TD_9816_build_done.push_back(events_JOIN_INNER_TD_9816[0]);
    krnl_JOIN_INNER_TD_9816_part_right.run(0, &(events_grp_JOIN_INNER_TD_9816), &(events_JOIN_INNER_TD_9816[0][0]));
    for (int i(0); i < hpTimes_join; ++i) {
        krnl_JOIN_INNER_TD_9816[i].run(0, &(events_grp_JOIN_INNER_TD_9816), &(events_JOIN_INNER_TD_9816[1][i]));
    }
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_9816_probe_done;
    events_grp_JOIN_INNER_TD_9816_probe_done.push_back(events_JOIN_INNER_TD_9816[1]);
    for (int i(0); i < hpTimes_join; ++i) {
        trans_JOIN_INNER_TD_9816_out.add(&(tbl_JOIN_INNER_TD_9816_output_partition_array[i]));
    }
    trans_JOIN_INNER_TD_9816_out.dev2host(0, &(events_JOIN_INNER_TD_9816[1]), &(events_d2h_rd_JOIN_INNER_TD_9816[0]));
    q_h.flush();
    q_h.finish();
    tbl_JOIN_INNER_TD_9816_output.setNumRow((cfg_JOIN_INNER_TD_9816_cmds_probe.meta_out)->getColLen());
    tbl_JOIN_INNER_TD_9816_output.colToTable();
    SW_Consolidated_JOIN_INNER_TD_9816_output(tbl_JOIN_INNER_TD_8808_consolidated_output, tbl_SerializeFromObject_TD_10433_input, tbl_JOIN_INNER_TD_9816_output, tbl_JOIN_INNER_TD_9816_consolidated_output);
    gettimeofday(&tv_r_JOIN_INNER_9_930_e, 0);

    struct timeval tv_r_Filter_6_650_s, tv_r_Filter_6_650_e;
    gettimeofday(&tv_r_Filter_6_650_s, 0);
    SW_Filter_TD_6491(tbl_SerializeFromObject_TD_7772_input, tbl_Filter_TD_6491_output);
    gettimeofday(&tv_r_Filter_6_650_e, 0);

    struct timeval tv_r_JOIN_INNER_5_1_s, tv_r_JOIN_INNER_5_1_e;
    gettimeofday(&tv_r_JOIN_INNER_5_1_s, 0);
    prev_events_grp_JOIN_INNER_TD_5245.push_back(events_h2d_wr_JOIN_INNER_TD_9816[0]);
    tbl_JOIN_INNER_TD_9816_consolidated_output.tableToCol();
    tbl_Filter_TD_6491_output.tableToCol();
    krnl_JOIN_INNER_TD_5245_build.updateMeta(tbl_JOIN_INNER_TD_9816_consolidated_output.getNumRow(), 3);
    krnl_JOIN_INNER_TD_5245_probe.updateMeta(tbl_Filter_TD_6491_output.getNumRow(), 3);
    trans_JOIN_INNER_TD_5245.add(&(tbl_Filter_TD_6491_output));
    trans_JOIN_INNER_TD_5245.host2dev(0, &(prev_events_grp_JOIN_INNER_TD_5245), &(events_h2d_wr_JOIN_INNER_TD_5245[0]));
    events_grp_JOIN_INNER_TD_5245.push_back(events_h2d_wr_JOIN_INNER_TD_5245[0]);
    events_grp_JOIN_INNER_TD_5245.insert(std::end(events_grp_JOIN_INNER_TD_5245), std::begin(events_JOIN_INNER_TD_9816[0]), std::end(events_JOIN_INNER_TD_9816[0]));
    events_grp_JOIN_INNER_TD_5245.insert(std::end(events_grp_JOIN_INNER_TD_5245), std::begin(events_JOIN_INNER_TD_9816[1]), std::end(events_JOIN_INNER_TD_9816[1]));
    krnl_JOIN_INNER_TD_5245_part_right.run(0, &(events_grp_JOIN_INNER_TD_5245), &(events_JOIN_INNER_TD_5245[0][0]));
    for (int i(0); i < hpTimes_join; ++i) {
        krnl_JOIN_INNER_TD_5245[i].run(0, &(events_grp_JOIN_INNER_TD_5245), &(events_JOIN_INNER_TD_5245[1][i]));
    }
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_5245_build_done;
    events_grp_JOIN_INNER_TD_5245_build_done.push_back(events_JOIN_INNER_TD_5245[0]);
    krnl_JOIN_INNER_TD_5245_part_right.run(0, &(events_grp_JOIN_INNER_TD_5245), &(events_JOIN_INNER_TD_5245[0][0]));
    for (int i(0); i < hpTimes_join; ++i) {
        krnl_JOIN_INNER_TD_5245[i].run(0, &(events_grp_JOIN_INNER_TD_5245), &(events_JOIN_INNER_TD_5245[1][i]));
    }
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_5245_probe_done;
    events_grp_JOIN_INNER_TD_5245_probe_done.push_back(events_JOIN_INNER_TD_5245[1]);
    for (int i(0); i < hpTimes_join; ++i) {
        trans_JOIN_INNER_TD_5245_out.add(&(tbl_JOIN_INNER_TD_5245_output_partition_array[i]));
    }
    trans_JOIN_INNER_TD_5245_out.dev2host(0, &(events_JOIN_INNER_TD_5245[1]), &(events_d2h_rd_JOIN_INNER_TD_5245[0]));
    q_h.flush();
    q_h.finish();
    tbl_JOIN_INNER_TD_5245_output.setNumRow((cfg_JOIN_INNER_TD_5245_cmds_probe.meta_out)->getColLen());
    tbl_JOIN_INNER_TD_5245_output.colToTable();
    SW_Consolidated_JOIN_INNER_TD_5245_output(tbl_JOIN_INNER_TD_9816_consolidated_output, tbl_Filter_TD_6491_output, tbl_JOIN_INNER_TD_5245_output, tbl_JOIN_INNER_TD_5245_consolidated_output);
    gettimeofday(&tv_r_JOIN_INNER_5_1_e, 0);

    struct timeval tv_r_Aggregate_6_947_s, tv_r_Aggregate_6_947_e;
    gettimeofday(&tv_r_Aggregate_6_947_s, 0);
    SW_Aggregate_TD_6602(tbl_JOIN_INNER_TD_5245_consolidated_output_partition_array, tbl_Aggregate_TD_6602_output, hpTimes_join);
    gettimeofday(&tv_r_Aggregate_6_947_e, 0);

    struct timeval tv_r_JOIN_INNER_4_922_s, tv_r_JOIN_INNER_4_922_e;
    gettimeofday(&tv_r_JOIN_INNER_4_922_s, 0);
    prev_events_grp_JOIN_INNER_TD_4265.push_back(events_h2d_wr_JOIN_INNER_TD_5245[0]);
    tbl_Aggregate_TD_6602_output.tableToCol();
    tbl_JOIN_INNER_TD_5245_consolidated_output.tableToCol();
    krnl_JOIN_INNER_TD_4265_build.updateMeta(tbl_Aggregate_TD_6602_output.getNumRow(), 3);
    krnl_JOIN_INNER_TD_4265_probe.updateMeta(tbl_JOIN_INNER_TD_5245_consolidated_output.getNumRow(), 3);
    trans_JOIN_INNER_TD_4265.add(&(tbl_Aggregate_TD_6602_output));
    trans_JOIN_INNER_TD_4265.host2dev(0, &(prev_events_grp_JOIN_INNER_TD_4265), &(events_h2d_wr_JOIN_INNER_TD_4265[0]));
    events_grp_JOIN_INNER_TD_4265.push_back(events_h2d_wr_JOIN_INNER_TD_4265[0]);
    events_grp_JOIN_INNER_TD_4265.insert(std::end(events_grp_JOIN_INNER_TD_4265), std::begin(events_JOIN_INNER_TD_5245[0]), std::end(events_JOIN_INNER_TD_5245[0]));
    events_grp_JOIN_INNER_TD_4265.insert(std::end(events_grp_JOIN_INNER_TD_4265), std::begin(events_JOIN_INNER_TD_5245[1]), std::end(events_JOIN_INNER_TD_5245[1]));
    krnl_JOIN_INNER_TD_4265_part_left.run(0, &(events_grp_JOIN_INNER_TD_4265), &(events_JOIN_INNER_TD_4265[0][0]));
    for (int i(0); i < hpTimes_join; ++i) {
        krnl_JOIN_INNER_TD_4265[i].run(0, &(events_JOIN_INNER_TD_4265[0]), &(events_JOIN_INNER_TD_4265[1][i]));
    }
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_4265_build_done;
    events_grp_JOIN_INNER_TD_4265_build_done.push_back(events_JOIN_INNER_TD_4265[0]);
    krnl_JOIN_INNER_TD_4265_part_left.run(0, &(events_grp_JOIN_INNER_TD_4265), &(events_JOIN_INNER_TD_4265[0][0]));
    for (int i(0); i < hpTimes_join; ++i) {
        krnl_JOIN_INNER_TD_4265[i].run(0, &(events_JOIN_INNER_TD_4265[0]), &(events_JOIN_INNER_TD_4265[1][i]));
    }
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_4265_probe_done;
    events_grp_JOIN_INNER_TD_4265_probe_done.push_back(events_JOIN_INNER_TD_4265[1]);
    for (int i(0); i < hpTimes_join; ++i) {
        trans_JOIN_INNER_TD_4265_out.add(&(tbl_JOIN_INNER_TD_4265_output_partition_array[i]));
    }
    trans_JOIN_INNER_TD_4265_out.dev2host(0, &(events_JOIN_INNER_TD_4265[1]), &(events_d2h_rd_JOIN_INNER_TD_4265[0]));
    q_h.flush();
    q_h.finish();
    tbl_JOIN_INNER_TD_4265_output.setNumRow((cfg_JOIN_INNER_TD_4265_cmds_probe.meta_out)->getColLen());
    tbl_JOIN_INNER_TD_4265_output.colToTable();
    SW_Consolidated_JOIN_INNER_TD_4265_output(tbl_Aggregate_TD_6602_output, tbl_JOIN_INNER_TD_5245_consolidated_output, tbl_JOIN_INNER_TD_4265_output, tbl_JOIN_INNER_TD_4265_consolidated_output);
    gettimeofday(&tv_r_JOIN_INNER_4_922_e, 0);

    struct timeval tv_r_JOIN_INNER_3_554_s, tv_r_JOIN_INNER_3_554_e;
    gettimeofday(&tv_r_JOIN_INNER_3_554_s, 0);
    prev_events_grp_JOIN_INNER_TD_3450.push_back(events_h2d_wr_JOIN_INNER_TD_4265[0]);
    tbl_JOIN_INNER_TD_4265_consolidated_output.tableToCol();
    tbl_SerializeFromObject_TD_4212_input_stringRowIDSubstitute.tableToCol();
    krnl_JOIN_INNER_TD_3450_build.updateMeta(tbl_JOIN_INNER_TD_4265_consolidated_output.getNumRow(), 3);
    krnl_JOIN_INNER_TD_3450_probe.updateMeta(tbl_SerializeFromObject_TD_4212_input_stringRowIDSubstitute.getNumRow(), 3);
    trans_JOIN_INNER_TD_3450.host2dev(0, &(prev_events_grp_JOIN_INNER_TD_3450), &(events_h2d_wr_JOIN_INNER_TD_3450[0]));
    events_grp_JOIN_INNER_TD_3450.push_back(events_h2d_wr_JOIN_INNER_TD_3450[0]);
    events_grp_JOIN_INNER_TD_3450.insert(std::end(events_grp_JOIN_INNER_TD_3450), std::begin(events_JOIN_INNER_TD_4265[0]), std::end(events_JOIN_INNER_TD_4265[0]));
    events_grp_JOIN_INNER_TD_3450.insert(std::end(events_grp_JOIN_INNER_TD_3450), std::begin(events_JOIN_INNER_TD_4265[1]), std::end(events_JOIN_INNER_TD_4265[1]));
    krnl_JOIN_INNER_TD_3450_part_right.run(0, &(events_grp_JOIN_INNER_TD_3450), &(events_JOIN_INNER_TD_3450[0][0]));
    for (int i(0); i < hpTimes_join; ++i) {
        krnl_JOIN_INNER_TD_3450[i].run(0, &(events_grp_JOIN_INNER_TD_3450), &(events_JOIN_INNER_TD_3450[1][i]));
    }
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_3450_build_done;
    events_grp_JOIN_INNER_TD_3450_build_done.push_back(events_JOIN_INNER_TD_3450[0]);
    krnl_JOIN_INNER_TD_3450_part_right.run(0, &(events_grp_JOIN_INNER_TD_3450), &(events_JOIN_INNER_TD_3450[0][0]));
    for (int i(0); i < hpTimes_join; ++i) {
        krnl_JOIN_INNER_TD_3450[i].run(0, &(events_grp_JOIN_INNER_TD_3450), &(events_JOIN_INNER_TD_3450[1][i]));
    }
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_3450_probe_done;
    events_grp_JOIN_INNER_TD_3450_probe_done.push_back(events_JOIN_INNER_TD_3450[1]);
    for (int i(0); i < hpTimes_join; ++i) {
        trans_JOIN_INNER_TD_3450_out.add(&(tbl_JOIN_INNER_TD_3450_output_partition_array[i]));
    }
    trans_JOIN_INNER_TD_3450_out.dev2host(0, &(events_JOIN_INNER_TD_3450[1]), &(events_d2h_rd_JOIN_INNER_TD_3450[0]));
    q_h.flush();
    q_h.finish();
    tbl_JOIN_INNER_TD_3450_output.setNumRow((cfg_JOIN_INNER_TD_3450_cmds_probe.meta_out)->getColLen());
    tbl_JOIN_INNER_TD_3450_output.colToTable();
    SW_Consolidated_JOIN_INNER_TD_3450_output(tbl_JOIN_INNER_TD_4265_consolidated_output, tbl_SerializeFromObject_TD_4212_input_stringRowIDSubstitute, tbl_JOIN_INNER_TD_3450_output, tbl_JOIN_INNER_TD_3450_consolidated_output);
    gettimeofday(&tv_r_JOIN_INNER_3_554_e, 0);

    struct timeval tv_r_JOIN_INNER_2_275_s, tv_r_JOIN_INNER_2_275_e;
    gettimeofday(&tv_r_JOIN_INNER_2_275_s, 0);
    SW_JOIN_INNER_TD_2849(tbl_JOIN_INNER_TD_3450_consolidated_output_partition_array, tbl_SerializeFromObject_TD_3358_input_stringRowIDSubstitute, tbl_JOIN_INNER_TD_2849_output, hpTimes_join);
    gettimeofday(&tv_r_JOIN_INNER_2_275_e, 0);

    struct timeval tv_r_JOIN_INNER_1_595_s, tv_r_JOIN_INNER_1_595_e;
    gettimeofday(&tv_r_JOIN_INNER_1_595_s, 0);
    SW_JOIN_INNER_TD_1647(tbl_JOIN_INNER_TD_2849_output, tbl_Filter_TD_8914_output, tbl_JOIN_INNER_TD_1647_output);
    gettimeofday(&tv_r_JOIN_INNER_1_595_e, 0);

    struct timeval tv_r_Sort_0_697_s, tv_r_Sort_0_697_e;
    gettimeofday(&tv_r_Sort_0_697_s, 0);
    SW_Sort_TD_0156(tbl_JOIN_INNER_TD_1647_output, tbl_SerializeFromObject_TD_7772_input, tbl_SerializeFromObject_TD_4212_input, tbl_SerializeFromObject_TD_3358_input, tbl_Sort_TD_0156_output);
    gettimeofday(&tv_r_Sort_0_697_e, 0);

std::cout << "CPU TIME: " << (tvdiff(&tv_r_Filter_8_53_s, &tv_r_Filter_8_53_e) + 
tvdiff(&tv_r_Filter_6_650_s, &tv_r_Filter_6_650_e) + 
tvdiff(&tv_r_Aggregate_6_947_s, &tv_r_Aggregate_6_947_e) + 
tvdiff(&tv_r_JOIN_INNER_2_275_s, &tv_r_JOIN_INNER_2_275_e) + 
tvdiff(&tv_r_JOIN_INNER_1_595_s, &tv_r_JOIN_INNER_1_595_e) + 
tvdiff(&tv_r_Sort_0_697_s, &tv_r_Sort_0_697_e) + 
1) / 1000.0 << std::endl;

std::cout << "FPGA TIME: " << (tvdiff(&tv_r_JOIN_INNER_7_584_s, &tv_r_JOIN_INNER_7_584_e) + 
tvdiff(&tv_r_JOIN_INNER_8_266_s, &tv_r_JOIN_INNER_8_266_e) + 
tvdiff(&tv_r_JOIN_INNER_9_930_s, &tv_r_JOIN_INNER_9_930_e) + 
tvdiff(&tv_r_JOIN_INNER_5_1_s, &tv_r_JOIN_INNER_5_1_e) + 
tvdiff(&tv_r_JOIN_INNER_4_922_s, &tv_r_JOIN_INNER_4_922_e) + 
tvdiff(&tv_r_JOIN_INNER_3_554_s, &tv_r_JOIN_INNER_3_554_e) + 
1) / 1000.0 << std::endl;

    gettimeofday(&tv_r_e, 0); 
    // **************************** Print Execution Time ************************** // 
    std::cout << std::endl; 
    std::cout << "Filter_8: " << tvdiff(&tv_r_Filter_8_53_s, &tv_r_Filter_8_53_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer((isnotnull(r_name#175) AND (r_name#175 = AMERICA))) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_9419_input.getNumRow() << " -> tbl_SerializeFromObject_TD_9419_input" 
     << std::endl << "      #COL: 2: " << "ListBuffer(r_regionkey#174, r_name#175)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_8914_output.getNumRow() << " -> tbl_Filter_TD_8914_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(r_regionkey#174)" 
     << std::endl; 

    std::cout << "JOIN_INNER_7: " << tvdiff(&tv_r_JOIN_INNER_7_584_s, &tv_r_JOIN_INNER_7_584_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer((n_regionkey#146 = r_regionkey#174)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_9465_input.getNumRow() << " -> tbl_SerializeFromObject_TD_9465_input" 
     << std::endl << "      #COL: 2: " << "ListBuffer(n_nationkey#144, n_regionkey#146)" 
     << std::endl << "      #ROW: " << tbl_Filter_TD_8914_output.getNumRow() << " -> tbl_Filter_TD_8914_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(r_regionkey#174)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_7380_consolidated_output.getNumRow() << " -> tbl_JOIN_INNER_TD_7380_consolidated_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(n_nationkey#144)" 
     << std::endl; 

    std::cout << "JOIN_INNER_8: " << tvdiff(&tv_r_JOIN_INNER_8_266_s, &tv_r_JOIN_INNER_8_266_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer((s_nationkey#345 = n_nationkey#144)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_4212_input_stringRowIDSubstitute.getNumRow() << " -> tbl_SerializeFromObject_TD_4212_input_stringRowIDSubstitute" 
     << std::endl << "      #COL: 7: " << "ListBuffer(s_suppkey#342, s_name#343, s_address#344, s_nationkey#345, s_phone#346, s_acctbal#347, s_comment#348)" 
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_7380_consolidated_output.getNumRow() << " -> tbl_JOIN_INNER_TD_7380_consolidated_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(n_nationkey#144)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_8808_consolidated_output.getNumRow() << " -> tbl_JOIN_INNER_TD_8808_consolidated_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(s_suppkey#342)" 
     << std::endl; 

    std::cout << "JOIN_INNER_9: " << tvdiff(&tv_r_JOIN_INNER_9_930_s, &tv_r_JOIN_INNER_9_930_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer((s_suppkey#342 = ps_suppkey#306)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_8808_consolidated_output.getNumRow() << " -> tbl_JOIN_INNER_TD_8808_consolidated_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(s_suppkey#342)" 
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_10433_input.getNumRow() << " -> tbl_SerializeFromObject_TD_10433_input" 
     << std::endl << "      #COL: 3: " << "ListBuffer(ps_partkey#305, ps_suppkey#306, ps_supplycost#308)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_9816_consolidated_output.getNumRow() << " -> tbl_JOIN_INNER_TD_9816_consolidated_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(ps_partkey#305, ps_supplycost#308, ps_suppkey#306)" 
     << std::endl; 

    std::cout << "Filter_6: " << tvdiff(&tv_r_Filter_6_650_s, &tv_r_Filter_6_650_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer((isnotnull(p_type#262) AND ((p_size#263 = 19) AND EndsWith(p_type#262, NICKEL)))) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_7772_input.getNumRow() << " -> tbl_SerializeFromObject_TD_7772_input" 
     << std::endl << "      #COL: 4: " << "ListBuffer(p_partkey#258, p_mfgr#260, p_type#262, p_size#263)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_6491_output.getNumRow() << " -> tbl_Filter_TD_6491_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(p_partkey#258, p_mfgr#260)" 
     << std::endl; 

    std::cout << "JOIN_INNER_5: " << tvdiff(&tv_r_JOIN_INNER_5_1_s, &tv_r_JOIN_INNER_5_1_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer((p_partkey#258 = ps_partkey#305)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_6491_output.getNumRow() << " -> tbl_Filter_TD_6491_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(p_partkey#258, p_mfgr#260)" 
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_9816_consolidated_output.getNumRow() << " -> tbl_JOIN_INNER_TD_9816_consolidated_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(ps_partkey#305, ps_supplycost#308, ps_suppkey#306)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_5245_consolidated_output.getNumRow() << " -> tbl_JOIN_INNER_TD_5245_consolidated_output" 
     << std::endl << "      #COL: 5: " << "ListBuffer(p_partkey#258, p_mfgr#260, ps_suppkey#306, ps_supplycost#308, ps_partkey#305)" 
     << std::endl; 

    std::cout << "Aggregate_6: " << tvdiff(&tv_r_Aggregate_6_947_s, &tv_r_Aggregate_6_947_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(ps_partkey#305, min(ps_supplycost#308) AS min(ps_supplycost)#579, ps_partkey#305 AS ps_partkey#305#663) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_5245_consolidated_output.getNumRow() << " -> tbl_JOIN_INNER_TD_5245_consolidated_output" 
     << std::endl << "      #COL: 5: " << "ListBuffer(p_partkey#258, p_mfgr#260, ps_suppkey#306, ps_supplycost#308, ps_partkey#305)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_6602_output.getNumRow() << " -> tbl_Aggregate_TD_6602_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(min(ps_supplycost)#579, ps_partkey#305#663)" 
     << std::endl; 

    std::cout << "JOIN_INNER_4: " << tvdiff(&tv_r_JOIN_INNER_4_922_s, &tv_r_JOIN_INNER_4_922_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer(((ps_supplycost#308 = min(ps_supplycost)#579) AND (p_partkey#258 = ps_partkey#305#663))) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_5245_consolidated_output.getNumRow() << " -> tbl_JOIN_INNER_TD_5245_consolidated_output" 
     << std::endl << "      #COL: 5: " << "ListBuffer(p_partkey#258, p_mfgr#260, ps_suppkey#306, ps_supplycost#308, ps_partkey#305)" 
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_6602_output.getNumRow() << " -> tbl_Aggregate_TD_6602_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(min(ps_supplycost)#579, ps_partkey#305#663)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_4265_consolidated_output.getNumRow() << " -> tbl_JOIN_INNER_TD_4265_consolidated_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(p_partkey#258, p_mfgr#260, ps_suppkey#306)" 
     << std::endl; 

    std::cout << "JOIN_INNER_3: " << tvdiff(&tv_r_JOIN_INNER_3_554_s, &tv_r_JOIN_INNER_3_554_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer((s_suppkey#342 = ps_suppkey#306)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_4265_consolidated_output.getNumRow() << " -> tbl_JOIN_INNER_TD_4265_consolidated_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(p_partkey#258, p_mfgr#260, ps_suppkey#306)" 
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_4212_input_stringRowIDSubstitute.getNumRow() << " -> tbl_SerializeFromObject_TD_4212_input_stringRowIDSubstitute" 
     << std::endl << "      #COL: 7: " << "ListBuffer(s_suppkey#342, s_name#343, s_address#344, s_nationkey#345, s_phone#346, s_acctbal#347, s_comment#348)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_3450_consolidated_output.getNumRow() << " -> tbl_JOIN_INNER_TD_3450_consolidated_output" 
     << std::endl << "      #COL: 8: " << "ListBuffer(p_partkey#258, p_mfgr#260, s_name#343, s_address#344, s_nationkey#345, s_phone#346, s_acctbal#347, s_comment#348)" 
     << std::endl; 

    std::cout << "JOIN_INNER_2: " << tvdiff(&tv_r_JOIN_INNER_2_275_s, &tv_r_JOIN_INNER_2_275_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer((s_nationkey#345 = n_nationkey#144)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_3450_consolidated_output.getNumRow() << " -> tbl_JOIN_INNER_TD_3450_consolidated_output" 
     << std::endl << "      #COL: 8: " << "ListBuffer(p_partkey#258, p_mfgr#260, s_name#343, s_address#344, s_nationkey#345, s_phone#346, s_acctbal#347, s_comment#348)" 
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_3358_input_stringRowIDSubstitute.getNumRow() << " -> tbl_SerializeFromObject_TD_3358_input_stringRowIDSubstitute" 
     << std::endl << "      #COL: 3: " << "ListBuffer(n_nationkey#144, n_name#145, n_regionkey#146)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_2849_output.getNumRow() << " -> tbl_JOIN_INNER_TD_2849_output" 
     << std::endl << "      #COL: 9: " << "ListBuffer(p_partkey#258, p_mfgr#260, s_name#343, s_address#344, s_phone#346, s_acctbal#347, s_comment#348, n_name#145, n_regionkey#146)" 
     << std::endl; 

    std::cout << "JOIN_INNER_1: " << tvdiff(&tv_r_JOIN_INNER_1_595_s, &tv_r_JOIN_INNER_1_595_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer((n_regionkey#146 = r_regionkey#174)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_2849_output.getNumRow() << " -> tbl_JOIN_INNER_TD_2849_output" 
     << std::endl << "      #COL: 9: " << "ListBuffer(p_partkey#258, p_mfgr#260, s_name#343, s_address#344, s_phone#346, s_acctbal#347, s_comment#348, n_name#145, n_regionkey#146)" 
     << std::endl << "      #ROW: " << tbl_Filter_TD_8914_output.getNumRow() << " -> tbl_Filter_TD_8914_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(r_regionkey#174)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_1647_output.getNumRow() << " -> tbl_JOIN_INNER_TD_1647_output" 
     << std::endl << "      #COL: 8: " << "ListBuffer(s_acctbal#347, s_name#343, n_name#145, p_partkey#258, p_mfgr#260, s_address#344, s_phone#346, s_comment#348)" 
     << std::endl; 

    std::cout << "Sort_0: " << tvdiff(&tv_r_Sort_0_697_s, &tv_r_Sort_0_697_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(s_acctbal#347 DESC NULLS LAST, n_name#145 ASC NULLS FIRST, s_name#343 ASC NULLS FIRST, p_partkey#258 ASC NULLS FIRST) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_1647_output.getNumRow() << " -> tbl_JOIN_INNER_TD_1647_output" 
     << std::endl << "      #COL: 8: " << "ListBuffer(s_acctbal#347, s_name#343, n_name#145, p_partkey#258, p_mfgr#260, s_address#344, s_phone#346, s_comment#348)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Sort_TD_0156_output.getNumRow() << " -> tbl_Sort_TD_0156_output" 
     << std::endl << "      #COL: 8: " << "ListBuffer(s_acctbal#347, s_name#343, n_name#145, p_partkey#258, p_mfgr#260, s_address#344, s_phone#346, s_comment#348)" 
     << std::endl; 

    std::cout << std::endl << " Total execution time: " << tvdiff(&tv_r_s, &tv_r_e) / 1000 << " ms"; 

    std::cout << std::endl << " Spark elapsed time: " << 3.5998456 * 1000 << "ms" << std::endl; 
    return 0; 
}
