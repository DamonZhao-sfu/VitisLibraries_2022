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
    // *********************** Partition Infomation ********************* // 
    int hpTimes_join = 32; 
    int power_of_hpTimes_join = log2(hpTimes_join); 
    std::cout << "Number of partition (gqeJoin) is: " << hpTimes_join << std::endl; 
    int hpTimes_aggr = 256; 
    int power_of_hpTimes_aggr = log2(hpTimes_aggr); 
    std::cout << "Number of partition (gqeAggr) is: " << hpTimes_aggr << std::endl; 
    // ****************************** Tables **************************** // 
    Table tbl_Sort_TD_0482_output("tbl_Sort_TD_0482_output", 183000000, 2, "");
    tbl_Sort_TD_0482_output.allocateHost();
    Table tbl_Aggregate_TD_1648_output("tbl_Aggregate_TD_1648_output", 183000000, 2, "");
    tbl_Aggregate_TD_1648_output.allocateHost();
    Table tbl_JOIN_INNER_TD_2684_output("tbl_JOIN_INNER_TD_2684_output", 25255, 4, "");
    tbl_JOIN_INNER_TD_2684_output.allocateHost(1.2, hpTimes_join);
    Table tbl_JOIN_INNER_TD_2684_consolidated_output("tbl_JOIN_INNER_TD_2684_consolidated_output", 25255, 1, "");
    tbl_JOIN_INNER_TD_2684_consolidated_output.selectOverlayVersion(1);
    tbl_JOIN_INNER_TD_2684_consolidated_output.allocateHost();
    Table tbl_Filter_TD_3321_output("tbl_Filter_TD_3321_output", 183000000, 1, "");
    tbl_Filter_TD_3321_output.selectOverlayVersion(1);
    tbl_Filter_TD_3321_output.allocateHost();
    Table tbl_Filter_TD_3321_output_partition("tbl_Filter_TD_3321_output_partition", 183000000, 1, "");
    tbl_Filter_TD_3321_output_partition.allocateHost(1.2, hpTimes_join);
    tbl_Filter_TD_3321_output_partition.allocateDevBuffer(context_h, 32);
    Table tbl_Filter_TD_3321_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_Filter_TD_3321_output_partition_array[i] = tbl_Filter_TD_3321_output_partition.createSubTable(i);
    }
    Table tbl_JOIN_INNER_TD_4616_output("tbl_JOIN_INNER_TD_4616_output", 52212, 4, "");
    tbl_JOIN_INNER_TD_4616_output.allocateHost(1.2, hpTimes_join);
    Table tbl_JOIN_INNER_TD_4616_consolidated_output("tbl_JOIN_INNER_TD_4616_consolidated_output", 52212, 2, "");
    tbl_JOIN_INNER_TD_4616_consolidated_output.selectOverlayVersion(1);
    tbl_JOIN_INNER_TD_4616_consolidated_output.allocateHost();
    Table tbl_SerializeFromObject_TD_5180_input;
    tbl_SerializeFromObject_TD_5180_input = Table("order", order_n, 2, in_dir);
    tbl_SerializeFromObject_TD_5180_input.addCol("o_orderkey", 4);
    tbl_SerializeFromObject_TD_5180_input.addCol("o_orderstatus", 4);
    tbl_SerializeFromObject_TD_5180_input.allocateHost();
    tbl_SerializeFromObject_TD_5180_input.loadHost();
    Table tbl_JOIN_INNER_TD_3653_output("tbl_JOIN_INNER_TD_3653_output", 377, 4, "");
    tbl_JOIN_INNER_TD_3653_output.allocateHost(1.2, hpTimes_join);
    Table tbl_JOIN_INNER_TD_3653_consolidated_output("tbl_JOIN_INNER_TD_3653_consolidated_output", 377, 2, "");
    tbl_JOIN_INNER_TD_3653_consolidated_output.selectOverlayVersion(1);
    tbl_JOIN_INNER_TD_3653_consolidated_output.allocateHost();
    Table tbl_JOIN_LEFTANTI_TD_5733_output("tbl_JOIN_LEFTANTI_TD_5733_output", 1375555, 4, "");
    tbl_JOIN_LEFTANTI_TD_5733_output.allocateHost(1.2, hpTimes_join);
    Table tbl_JOIN_LEFTANTI_TD_5733_consolidated_output("tbl_JOIN_LEFTANTI_TD_5733_consolidated_output", 1375555, 2, "");
    tbl_JOIN_LEFTANTI_TD_5733_consolidated_output.selectOverlayVersion(1);
    tbl_JOIN_LEFTANTI_TD_5733_consolidated_output.allocateHost();
    Table tbl_SerializeFromObject_TD_5188_input_stringRowIDSubstitute;
    tbl_SerializeFromObject_TD_5188_input_stringRowIDSubstitute = Table("supplier", supplier_n, 3, in_dir);
    tbl_SerializeFromObject_TD_5188_input_stringRowIDSubstitute.addCol("s_suppkey", 4);
    tbl_SerializeFromObject_TD_5188_input_stringRowIDSubstitute.addCol("s_name", 4, 1, 0);
    tbl_SerializeFromObject_TD_5188_input_stringRowIDSubstitute.addCol("s_nationkey", 4);
    tbl_SerializeFromObject_TD_5188_input_stringRowIDSubstitute.allocateHost();
    tbl_SerializeFromObject_TD_5188_input_stringRowIDSubstitute.loadHost();
    Table tbl_SerializeFromObject_TD_5188_input_stringRowIDSubstitute_partition("tbl_SerializeFromObject_TD_5188_input_stringRowIDSubstitute_partition", supplier_n, 3, "");
    tbl_SerializeFromObject_TD_5188_input_stringRowIDSubstitute_partition.allocateHost(1.2, hpTimes_join);
    tbl_SerializeFromObject_TD_5188_input_stringRowIDSubstitute_partition.allocateDevBuffer(context_h, 32);
    Table tbl_SerializeFromObject_TD_5188_input_stringRowIDSubstitute_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_SerializeFromObject_TD_5188_input_stringRowIDSubstitute_partition_array[i] = tbl_SerializeFromObject_TD_5188_input_stringRowIDSubstitute_partition.createSubTable(i);
    }
    Table tbl_SerializeFromObject_TD_5188_input;
    tbl_SerializeFromObject_TD_5188_input = Table("supplier", supplier_n, 3, in_dir);
    tbl_SerializeFromObject_TD_5188_input.addCol("s_suppkey", 4);
    tbl_SerializeFromObject_TD_5188_input.addCol("s_name", TPCH_READ_S_NAME_LEN+1);
    tbl_SerializeFromObject_TD_5188_input.addCol("s_nationkey", 4);
    tbl_SerializeFromObject_TD_5188_input.allocateHost();
    tbl_SerializeFromObject_TD_5188_input.loadHost();
    Table tbl_Filter_TD_3611_output("tbl_Filter_TD_3611_output", 183000000, 1, "");
    tbl_Filter_TD_3611_output.selectOverlayVersion(1);
    tbl_Filter_TD_3611_output.allocateHost();
    Table tbl_Filter_TD_3611_output_partition("tbl_Filter_TD_3611_output_partition", 183000000, 1, "");
    tbl_Filter_TD_3611_output_partition.allocateHost(1.2, hpTimes_join);
    tbl_Filter_TD_3611_output_partition.allocateDevBuffer(context_h, 32);
    Table tbl_Filter_TD_3611_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_Filter_TD_3611_output_partition_array[i] = tbl_Filter_TD_3611_output_partition.createSubTable(i);
    }
    Table tbl_JOIN_LEFTSEMI_TD_6827_output("tbl_JOIN_LEFTSEMI_TD_6827_output", 2417632, 4, "");
    tbl_JOIN_LEFTSEMI_TD_6827_output.allocateHost(1.2, hpTimes_join);
    Table tbl_JOIN_LEFTSEMI_TD_6827_consolidated_output("tbl_JOIN_LEFTSEMI_TD_6827_consolidated_output", 2417632, 2, "");
    tbl_JOIN_LEFTSEMI_TD_6827_consolidated_output.selectOverlayVersion(1);
    tbl_JOIN_LEFTSEMI_TD_6827_consolidated_output.allocateHost();
    Table tbl_SerializeFromObject_TD_4935_input;
    tbl_SerializeFromObject_TD_4935_input = Table("nation", nation_n, 2, in_dir);
    tbl_SerializeFromObject_TD_4935_input.addCol("n_nationkey", 4);
    tbl_SerializeFromObject_TD_4935_input.addCol("n_name", TPCH_READ_NATION_LEN+1);
    tbl_SerializeFromObject_TD_4935_input.allocateHost();
    tbl_SerializeFromObject_TD_4935_input.loadHost();
    Table tbl_JOIN_LEFTSEMI_TD_7346_output("tbl_JOIN_LEFTSEMI_TD_7346_output", 2844584, 4, "");
    tbl_JOIN_LEFTSEMI_TD_7346_output.allocateHost(1.2, hpTimes_join);
    Table tbl_JOIN_LEFTSEMI_TD_7346_consolidated_output("tbl_JOIN_LEFTSEMI_TD_7346_consolidated_output", 2844584, 2, "");
    tbl_JOIN_LEFTSEMI_TD_7346_consolidated_output.selectOverlayVersion(1);
    tbl_JOIN_LEFTSEMI_TD_7346_consolidated_output.allocateHost();
    Table tbl_Filter_TD_7203_output("tbl_Filter_TD_7203_output", 183000000, 2, "");
    tbl_Filter_TD_7203_output.selectOverlayVersion(1);
    tbl_Filter_TD_7203_output.allocateHost();
    Table tbl_Filter_TD_7203_output_partition("tbl_Filter_TD_7203_output_partition", 183000000, 2, "");
    tbl_Filter_TD_7203_output_partition.allocateHost(1.2, hpTimes_join);
    tbl_Filter_TD_7203_output_partition.allocateDevBuffer(context_h, 32);
    Table tbl_Filter_TD_7203_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_Filter_TD_7203_output_partition_array[i] = tbl_Filter_TD_7203_output_partition.createSubTable(i);
    }
    Table tbl_Filter_TD_8644_output("tbl_Filter_TD_8644_output", 183000000, 2, "");
    tbl_Filter_TD_8644_output.selectOverlayVersion(1);
    tbl_Filter_TD_8644_output.allocateHost();
    Table tbl_Filter_TD_8644_output_partition("tbl_Filter_TD_8644_output_partition", 183000000, 2, "");
    tbl_Filter_TD_8644_output_partition.allocateHost(1.2, hpTimes_join);
    tbl_Filter_TD_8644_output_partition.allocateDevBuffer(context_h, 32);
    Table tbl_Filter_TD_8644_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_Filter_TD_8644_output_partition_array[i] = tbl_Filter_TD_8644_output_partition.createSubTable(i);
    }
    Table tbl_SerializeFromObject_TD_8225_input;
    tbl_SerializeFromObject_TD_8225_input = Table("lineitem", lineitem_n, 2, in_dir);
    tbl_SerializeFromObject_TD_8225_input.selectOverlayVersion(1);
    tbl_SerializeFromObject_TD_8225_input.addCol("l_orderkey", 4);
    tbl_SerializeFromObject_TD_8225_input.addCol("l_suppkey", 4);
    tbl_SerializeFromObject_TD_8225_input.allocateHost();
    tbl_SerializeFromObject_TD_8225_input.loadHost();
    Table tbl_SerializeFromObject_TD_8225_input_partition("tbl_SerializeFromObject_TD_8225_input_partition", lineitem_n, 2, "");
    tbl_SerializeFromObject_TD_8225_input_partition.allocateHost(1.2, hpTimes_join);
    tbl_SerializeFromObject_TD_8225_input_partition.allocateDevBuffer(context_h, 32);
    Table tbl_SerializeFromObject_TD_8225_input_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_SerializeFromObject_TD_8225_input_partition_array[i] = tbl_SerializeFromObject_TD_8225_input_partition.createSubTable(i);
    }
    Table tbl_SerializeFromObject_TD_9935_input;
    tbl_SerializeFromObject_TD_9935_input = Table("lineitem", lineitem_n, 4, in_dir);
    tbl_SerializeFromObject_TD_9935_input.addCol("l_orderkey", 4);
    tbl_SerializeFromObject_TD_9935_input.addCol("l_suppkey", 4);
    tbl_SerializeFromObject_TD_9935_input.addCol("l_receiptdate", 4);
    tbl_SerializeFromObject_TD_9935_input.addCol("l_commitdate", 4);
    tbl_SerializeFromObject_TD_9935_input.allocateHost();
    tbl_SerializeFromObject_TD_9935_input.loadHost();
    // ********************** Allocate Device Buffer ******************** // 
    tbl_JOIN_INNER_TD_2684_output.allocateDevBuffer(context_h, 32);
    Table tbl_JOIN_INNER_TD_2684_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_JOIN_INNER_TD_2684_output_partition_array[i] = tbl_JOIN_INNER_TD_2684_output.createSubTable(i);
    }
    tbl_Filter_TD_3321_output.allocateDevBuffer(context_h, 33);
    tbl_JOIN_INNER_TD_4616_output.allocateDevBuffer(context_h, 32);
    Table tbl_JOIN_INNER_TD_4616_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_JOIN_INNER_TD_4616_output_partition_array[i] = tbl_JOIN_INNER_TD_4616_output.createSubTable(i);
    }
    tbl_JOIN_INNER_TD_3653_output.allocateDevBuffer(context_h, 32);
    Table tbl_JOIN_INNER_TD_3653_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_JOIN_INNER_TD_3653_output_partition_array[i] = tbl_JOIN_INNER_TD_3653_output.createSubTable(i);
    }
    tbl_JOIN_LEFTANTI_TD_5733_output.allocateDevBuffer(context_h, 32);
    Table tbl_JOIN_LEFTANTI_TD_5733_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_JOIN_LEFTANTI_TD_5733_output_partition_array[i] = tbl_JOIN_LEFTANTI_TD_5733_output.createSubTable(i);
    }
    tbl_SerializeFromObject_TD_5188_input_stringRowIDSubstitute.allocateDevBuffer(context_h, 33);
    tbl_Filter_TD_3611_output.allocateDevBuffer(context_h, 33);
    tbl_JOIN_LEFTSEMI_TD_6827_output.allocateDevBuffer(context_h, 32);
    Table tbl_JOIN_LEFTSEMI_TD_6827_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_JOIN_LEFTSEMI_TD_6827_output_partition_array[i] = tbl_JOIN_LEFTSEMI_TD_6827_output.createSubTable(i);
    }
    tbl_JOIN_LEFTSEMI_TD_7346_output.allocateDevBuffer(context_h, 32);
    Table tbl_JOIN_LEFTSEMI_TD_7346_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_JOIN_LEFTSEMI_TD_7346_output_partition_array[i] = tbl_JOIN_LEFTSEMI_TD_7346_output.createSubTable(i);
    }
    tbl_Filter_TD_7203_output.allocateDevBuffer(context_h, 33);
    tbl_Filter_TD_8644_output.allocateDevBuffer(context_h, 33);
    tbl_SerializeFromObject_TD_8225_input.allocateDevBuffer(context_h, 33);
    // ****************************** Config **************************** // 
    
    xf::database::gqe::KernelCommand krn_cmd_build_557 = xf::database::gqe::KernelCommand();
    xf::database::gqe::KernelCommand krn_cmd_probe_557 = xf::database::gqe::KernelCommand();
    cfgCmd cfg_JOIN_INNER_TD_2684_cmds_build;
    cfg_JOIN_INNER_TD_2684_cmds_build.setup(1);
    cfg_JOIN_INNER_TD_2684_cmds_build.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_2684_gqe_join_build(krn_cmd_build_557);
    cfg_JOIN_INNER_TD_2684_cmds_build.cmd = krn_cmd_build_557.getConfigBits();
    get_meta_info_JOIN_INNER_TD_2684_gqe_join_build(cfg_JOIN_INNER_TD_2684_cmds_build.meta_in, tbl_JOIN_INNER_TD_4616_consolidated_output.nrow, 3);
    get_meta_info_JOIN_INNER_TD_2684_gqe_join_build(cfg_JOIN_INNER_TD_2684_cmds_build.meta_out, tbl_JOIN_INNER_TD_2684_output.nrow, 4);
    // cfg_JOIN_INNER_TD_2684_cmds_build.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_2684_cmds_probe;
    cfg_JOIN_INNER_TD_2684_cmds_probe.setup(1);
    cfg_JOIN_INNER_TD_2684_cmds_probe.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_2684_gqe_join_probe(krn_cmd_probe_557);
    cfg_JOIN_INNER_TD_2684_cmds_probe.cmd = krn_cmd_probe_557.getConfigBits();
    get_meta_info_JOIN_INNER_TD_2684_gqe_join_probe(cfg_JOIN_INNER_TD_2684_cmds_probe.meta_in, tbl_Filter_TD_3321_output.nrow, 3);
    get_meta_info_JOIN_INNER_TD_2684_gqe_join_probe(cfg_JOIN_INNER_TD_2684_cmds_probe.meta_out, tbl_JOIN_INNER_TD_2684_output.nrow, 3);
    // cfg_JOIN_INNER_TD_2684_cmds_probe.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_2684_cmds_part;
    cfg_JOIN_INNER_TD_2684_cmds_part.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_2684_gqe_join_part (cfg_JOIN_INNER_TD_2684_cmds_part.cmd);
    cfg_JOIN_INNER_TD_2684_cmds_part.allocateDevBuffer(context_h, 32);
    cfg_JOIN_INNER_TD_2684_cmds_build.input_key_columns = {1, -1, -1};
    cfg_JOIN_INNER_TD_2684_cmds_probe.input_key_columns = {0, -1, -1};
    cfg_JOIN_INNER_TD_2684_cmds_build.output_key_columns = {0, 1, 2, 3};
    cfg_JOIN_INNER_TD_2684_cmds_probe.output_key_columns = {0, 1, 2, 3};
    
    xf::database::gqe::KernelCommand krn_cmd_build_441 = xf::database::gqe::KernelCommand();
    xf::database::gqe::KernelCommand krn_cmd_probe_441 = xf::database::gqe::KernelCommand();
    cfgCmd cfg_JOIN_INNER_TD_4616_cmds_build;
    cfg_JOIN_INNER_TD_4616_cmds_build.setup(1);
    cfg_JOIN_INNER_TD_4616_cmds_build.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_4616_gqe_join_build(krn_cmd_build_441);
    cfg_JOIN_INNER_TD_4616_cmds_build.cmd = krn_cmd_build_441.getConfigBits();
    get_meta_info_JOIN_INNER_TD_4616_gqe_join_build(cfg_JOIN_INNER_TD_4616_cmds_build.meta_in, tbl_JOIN_INNER_TD_3653_consolidated_output.nrow, 3);
    get_meta_info_JOIN_INNER_TD_4616_gqe_join_build(cfg_JOIN_INNER_TD_4616_cmds_build.meta_out, tbl_JOIN_INNER_TD_4616_output.nrow, 4);
    // cfg_JOIN_INNER_TD_4616_cmds_build.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_4616_cmds_probe;
    cfg_JOIN_INNER_TD_4616_cmds_probe.setup(1);
    cfg_JOIN_INNER_TD_4616_cmds_probe.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_4616_gqe_join_probe(krn_cmd_probe_441);
    cfg_JOIN_INNER_TD_4616_cmds_probe.cmd = krn_cmd_probe_441.getConfigBits();
    get_meta_info_JOIN_INNER_TD_4616_gqe_join_probe(cfg_JOIN_INNER_TD_4616_cmds_probe.meta_in, tbl_JOIN_LEFTANTI_TD_5733_consolidated_output.nrow, 3);
    get_meta_info_JOIN_INNER_TD_4616_gqe_join_probe(cfg_JOIN_INNER_TD_4616_cmds_probe.meta_out, tbl_JOIN_INNER_TD_4616_output.nrow, 3);
    // cfg_JOIN_INNER_TD_4616_cmds_probe.allocateDevBuffer(context_h, 32);
    cfg_JOIN_INNER_TD_4616_cmds_build.input_key_columns = {0, -1, -1};
    cfg_JOIN_INNER_TD_4616_cmds_probe.input_key_columns = {1, -1, -1};
    cfg_JOIN_INNER_TD_4616_cmds_build.output_key_columns = {0, 1, 2, 3};
    cfg_JOIN_INNER_TD_4616_cmds_probe.output_key_columns = {0, 1, 2, 3};
    
    xf::database::gqe::KernelCommand krn_cmd_build_126 = xf::database::gqe::KernelCommand();
    xf::database::gqe::KernelCommand krn_cmd_probe_126 = xf::database::gqe::KernelCommand();
    cfgCmd cfg_JOIN_INNER_TD_3653_cmds_build;
    cfg_JOIN_INNER_TD_3653_cmds_build.setup(1);
    cfg_JOIN_INNER_TD_3653_cmds_build.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_3653_gqe_join_build(krn_cmd_build_126);
    cfg_JOIN_INNER_TD_3653_cmds_build.cmd = krn_cmd_build_126.getConfigBits();
    get_meta_info_JOIN_INNER_TD_3653_gqe_join_build(cfg_JOIN_INNER_TD_3653_cmds_build.meta_in, tbl_SerializeFromObject_TD_5188_input_stringRowIDSubstitute.nrow, 3);
    get_meta_info_JOIN_INNER_TD_3653_gqe_join_build(cfg_JOIN_INNER_TD_3653_cmds_build.meta_out, tbl_JOIN_INNER_TD_3653_output.nrow, 4);
    // cfg_JOIN_INNER_TD_3653_cmds_build.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_3653_cmds_probe;
    cfg_JOIN_INNER_TD_3653_cmds_probe.setup(1);
    cfg_JOIN_INNER_TD_3653_cmds_probe.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_3653_gqe_join_probe(krn_cmd_probe_126);
    cfg_JOIN_INNER_TD_3653_cmds_probe.cmd = krn_cmd_probe_126.getConfigBits();
    get_meta_info_JOIN_INNER_TD_3653_gqe_join_probe(cfg_JOIN_INNER_TD_3653_cmds_probe.meta_in, tbl_Filter_TD_3611_output.nrow, 3);
    get_meta_info_JOIN_INNER_TD_3653_gqe_join_probe(cfg_JOIN_INNER_TD_3653_cmds_probe.meta_out, tbl_JOIN_INNER_TD_3653_output.nrow, 3);
    // cfg_JOIN_INNER_TD_3653_cmds_probe.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_3653_cmds_part;
    cfg_JOIN_INNER_TD_3653_cmds_part.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_3653_gqe_join_part (cfg_JOIN_INNER_TD_3653_cmds_part.cmd);
    cfg_JOIN_INNER_TD_3653_cmds_part.allocateDevBuffer(context_h, 32);
    cfg_JOIN_INNER_TD_3653_cmds_build.input_key_columns = {2, -1, -1};
    cfg_JOIN_INNER_TD_3653_cmds_probe.input_key_columns = {0, -1, -1};
    cfg_JOIN_INNER_TD_3653_cmds_build.output_key_columns = {0, 1, 2, 3};
    cfg_JOIN_INNER_TD_3653_cmds_probe.output_key_columns = {0, 1, 2, 3};
    
    xf::database::gqe::KernelCommand krn_cmd_build_386 = xf::database::gqe::KernelCommand();
    xf::database::gqe::KernelCommand krn_cmd_probe_386 = xf::database::gqe::KernelCommand();
    cfgCmd cfg_JOIN_LEFTANTI_TD_5733_cmds_build;
    cfg_JOIN_LEFTANTI_TD_5733_cmds_build.setup(1);
    cfg_JOIN_LEFTANTI_TD_5733_cmds_build.allocateHost();
    get_cfg_dat_JOIN_LEFTANTI_TD_5733_gqe_join_build(krn_cmd_build_386);
    cfg_JOIN_LEFTANTI_TD_5733_cmds_build.cmd = krn_cmd_build_386.getConfigBits();
    get_meta_info_JOIN_LEFTANTI_TD_5733_gqe_join_build(cfg_JOIN_LEFTANTI_TD_5733_cmds_build.meta_in, tbl_JOIN_LEFTSEMI_TD_6827_consolidated_output.nrow, 3);
    get_meta_info_JOIN_LEFTANTI_TD_5733_gqe_join_build(cfg_JOIN_LEFTANTI_TD_5733_cmds_build.meta_out, tbl_JOIN_LEFTANTI_TD_5733_output.nrow, 4);
    // cfg_JOIN_LEFTANTI_TD_5733_cmds_build.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_LEFTANTI_TD_5733_cmds_probe;
    cfg_JOIN_LEFTANTI_TD_5733_cmds_probe.setup(1);
    cfg_JOIN_LEFTANTI_TD_5733_cmds_probe.allocateHost();
    get_cfg_dat_JOIN_LEFTANTI_TD_5733_gqe_join_probe(krn_cmd_probe_386);
    cfg_JOIN_LEFTANTI_TD_5733_cmds_probe.cmd = krn_cmd_probe_386.getConfigBits();
    get_meta_info_JOIN_LEFTANTI_TD_5733_gqe_join_probe(cfg_JOIN_LEFTANTI_TD_5733_cmds_probe.meta_in, tbl_Filter_TD_7203_output.nrow, 3);
    get_meta_info_JOIN_LEFTANTI_TD_5733_gqe_join_probe(cfg_JOIN_LEFTANTI_TD_5733_cmds_probe.meta_out, tbl_JOIN_LEFTANTI_TD_5733_output.nrow, 3);
    // cfg_JOIN_LEFTANTI_TD_5733_cmds_probe.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_LEFTANTI_TD_5733_cmds_part;
    cfg_JOIN_LEFTANTI_TD_5733_cmds_part.allocateHost();
    get_cfg_dat_JOIN_LEFTANTI_TD_5733_gqe_join_part (cfg_JOIN_LEFTANTI_TD_5733_cmds_part.cmd);
    cfg_JOIN_LEFTANTI_TD_5733_cmds_part.allocateDevBuffer(context_h, 32);
    cfg_JOIN_LEFTANTI_TD_5733_cmds_build.input_key_columns = {0, 1, -1};
    cfg_JOIN_LEFTANTI_TD_5733_cmds_probe.input_key_columns = {0, 1, -1};
    cfg_JOIN_LEFTANTI_TD_5733_cmds_build.output_key_columns = {0, 1, 2, 3};
    cfg_JOIN_LEFTANTI_TD_5733_cmds_probe.output_key_columns = {0, 1, 2, 3};
    
    xf::database::gqe::KernelCommand krn_cmd_build_846 = xf::database::gqe::KernelCommand();
    xf::database::gqe::KernelCommand krn_cmd_probe_846 = xf::database::gqe::KernelCommand();
    cfgCmd cfg_JOIN_LEFTSEMI_TD_6827_cmds_build;
    cfg_JOIN_LEFTSEMI_TD_6827_cmds_build.setup(1);
    cfg_JOIN_LEFTSEMI_TD_6827_cmds_build.allocateHost();
    get_cfg_dat_JOIN_LEFTSEMI_TD_6827_gqe_join_build(krn_cmd_build_846);
    cfg_JOIN_LEFTSEMI_TD_6827_cmds_build.cmd = krn_cmd_build_846.getConfigBits();
    get_meta_info_JOIN_LEFTSEMI_TD_6827_gqe_join_build(cfg_JOIN_LEFTSEMI_TD_6827_cmds_build.meta_in, tbl_Filter_TD_7203_output.nrow, 3);
    get_meta_info_JOIN_LEFTSEMI_TD_6827_gqe_join_build(cfg_JOIN_LEFTSEMI_TD_6827_cmds_build.meta_out, tbl_JOIN_LEFTSEMI_TD_6827_output.nrow, 4);
    // cfg_JOIN_LEFTSEMI_TD_6827_cmds_build.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_LEFTSEMI_TD_6827_cmds_probe;
    cfg_JOIN_LEFTSEMI_TD_6827_cmds_probe.setup(1);
    cfg_JOIN_LEFTSEMI_TD_6827_cmds_probe.allocateHost();
    get_cfg_dat_JOIN_LEFTSEMI_TD_6827_gqe_join_probe(krn_cmd_probe_846);
    cfg_JOIN_LEFTSEMI_TD_6827_cmds_probe.cmd = krn_cmd_probe_846.getConfigBits();
    get_meta_info_JOIN_LEFTSEMI_TD_6827_gqe_join_probe(cfg_JOIN_LEFTSEMI_TD_6827_cmds_probe.meta_in, tbl_JOIN_LEFTSEMI_TD_7346_consolidated_output.nrow, 3);
    get_meta_info_JOIN_LEFTSEMI_TD_6827_gqe_join_probe(cfg_JOIN_LEFTSEMI_TD_6827_cmds_probe.meta_out, tbl_JOIN_LEFTSEMI_TD_6827_output.nrow, 3);
    // cfg_JOIN_LEFTSEMI_TD_6827_cmds_probe.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_LEFTSEMI_TD_6827_cmds_part;
    cfg_JOIN_LEFTSEMI_TD_6827_cmds_part.allocateHost();
    get_cfg_dat_JOIN_LEFTSEMI_TD_6827_gqe_join_part (cfg_JOIN_LEFTSEMI_TD_6827_cmds_part.cmd);
    cfg_JOIN_LEFTSEMI_TD_6827_cmds_part.allocateDevBuffer(context_h, 32);
    cfg_JOIN_LEFTSEMI_TD_6827_cmds_build.input_key_columns = {0, -1, -1};
    cfg_JOIN_LEFTSEMI_TD_6827_cmds_probe.input_key_columns = {0, -1, -1};
    cfg_JOIN_LEFTSEMI_TD_6827_cmds_build.output_key_columns = {0, 1, 2, 3};
    cfg_JOIN_LEFTSEMI_TD_6827_cmds_probe.output_key_columns = {0, 1, 2, 3};
    
    xf::database::gqe::KernelCommand krn_cmd_build_282 = xf::database::gqe::KernelCommand();
    xf::database::gqe::KernelCommand krn_cmd_probe_282 = xf::database::gqe::KernelCommand();
    cfgCmd cfg_JOIN_LEFTSEMI_TD_7346_cmds_build;
    cfg_JOIN_LEFTSEMI_TD_7346_cmds_build.setup(1);
    cfg_JOIN_LEFTSEMI_TD_7346_cmds_build.allocateHost();
    get_cfg_dat_JOIN_LEFTSEMI_TD_7346_gqe_join_build(krn_cmd_build_282);
    cfg_JOIN_LEFTSEMI_TD_7346_cmds_build.cmd = krn_cmd_build_282.getConfigBits();
    get_meta_info_JOIN_LEFTSEMI_TD_7346_gqe_join_build(cfg_JOIN_LEFTSEMI_TD_7346_cmds_build.meta_in, tbl_SerializeFromObject_TD_8225_input.nrow, 3);
    get_meta_info_JOIN_LEFTSEMI_TD_7346_gqe_join_build(cfg_JOIN_LEFTSEMI_TD_7346_cmds_build.meta_out, tbl_JOIN_LEFTSEMI_TD_7346_output.nrow, 4);
    // cfg_JOIN_LEFTSEMI_TD_7346_cmds_build.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_LEFTSEMI_TD_7346_cmds_probe;
    cfg_JOIN_LEFTSEMI_TD_7346_cmds_probe.setup(1);
    cfg_JOIN_LEFTSEMI_TD_7346_cmds_probe.allocateHost();
    get_cfg_dat_JOIN_LEFTSEMI_TD_7346_gqe_join_probe(krn_cmd_probe_282);
    cfg_JOIN_LEFTSEMI_TD_7346_cmds_probe.cmd = krn_cmd_probe_282.getConfigBits();
    get_meta_info_JOIN_LEFTSEMI_TD_7346_gqe_join_probe(cfg_JOIN_LEFTSEMI_TD_7346_cmds_probe.meta_in, tbl_Filter_TD_8644_output.nrow, 3);
    get_meta_info_JOIN_LEFTSEMI_TD_7346_gqe_join_probe(cfg_JOIN_LEFTSEMI_TD_7346_cmds_probe.meta_out, tbl_JOIN_LEFTSEMI_TD_7346_output.nrow, 3);
    // cfg_JOIN_LEFTSEMI_TD_7346_cmds_probe.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_LEFTSEMI_TD_7346_cmds_part;
    cfg_JOIN_LEFTSEMI_TD_7346_cmds_part.allocateHost();
    get_cfg_dat_JOIN_LEFTSEMI_TD_7346_gqe_join_part (cfg_JOIN_LEFTSEMI_TD_7346_cmds_part.cmd);
    cfg_JOIN_LEFTSEMI_TD_7346_cmds_part.allocateDevBuffer(context_h, 32);
    cfg_JOIN_LEFTSEMI_TD_7346_cmds_build.input_key_columns = {0, -1, -1};
    cfg_JOIN_LEFTSEMI_TD_7346_cmds_probe.input_key_columns = {0, -1, -1};
    cfg_JOIN_LEFTSEMI_TD_7346_cmds_build.output_key_columns = {0, 1, 2, 3};
    cfg_JOIN_LEFTSEMI_TD_7346_cmds_probe.output_key_columns = {0, 1, 2, 3};
    // *************************** Kernel Setup ************************* // 
    bufferTmp buftmp_h(context_h, 1); 
    buftmp_h.initBuffer(q_h); 
    std::cout << std::endl; 
    krnlEngine krnl_JOIN_INNER_TD_2684_part_right;
    krnl_JOIN_INNER_TD_2684_part_right = krnlEngine(program_h, q_h, context_h, "gqePart");
    krnl_JOIN_INNER_TD_2684_part_right.setup_hp(512, 1, power_of_hpTimes_join, tbl_Filter_TD_3321_output, tbl_Filter_TD_3321_output_partition, cfg_JOIN_INNER_TD_2684_cmds_part);
    krnlEngine krnl_JOIN_INNER_TD_2684[hpTimes_join];
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_INNER_TD_2684[i] = krnlEngine(program_h, q_h, "gqeJoin");
    }
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_INNER_TD_2684[i].setup(tbl_JOIN_INNER_TD_4616_consolidated_output_partition_array[i], tbl_Filter_TD_3321_output_partition_array[i], tbl_JOIN_INNER_TD_2684_output_partition_array[i], , buftmp_h);
    }
    krnlEngine krnl_JOIN_INNER_TD_2684_build;
    krnl_JOIN_INNER_TD_2684_build = krnlEngine(program_h, q_h, context_h, "gqeKernel", 0);
    krnl_JOIN_INNER_TD_2684_build.setup(tbl_JOIN_INNER_TD_4616_consolidated_output, tbl_Filter_TD_3321_output, tbl_JOIN_INNER_TD_2684_output, cfg_JOIN_INNER_TD_2684_cmds_build, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_2684_probe;
    krnl_JOIN_INNER_TD_2684_probe = krnlEngine(program_h, q_h, context_h, "gqeKernel", 1);
    krnl_JOIN_INNER_TD_2684_probe.setup(tbl_Filter_TD_3321_output, tbl_JOIN_INNER_TD_4616_consolidated_output, tbl_JOIN_INNER_TD_2684_output, cfg_JOIN_INNER_TD_2684_cmds_probe, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_4616[hpTimes_join];
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_INNER_TD_4616[i] = krnlEngine(program_h, q_h, "gqeJoin");
    }
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_INNER_TD_4616[i].setup(tbl_JOIN_INNER_TD_3653_consolidated_output_partition_array[i], tbl_JOIN_LEFTANTI_TD_5733_consolidated_output_partition_array[i], tbl_JOIN_INNER_TD_4616_output_partition_array[i], , buftmp_h);
    }
    krnlEngine krnl_JOIN_INNER_TD_4616_build;
    krnl_JOIN_INNER_TD_4616_build = krnlEngine(program_h, q_h, context_h, "gqeKernel", 0);
    krnl_JOIN_INNER_TD_4616_build.setup(tbl_JOIN_INNER_TD_3653_consolidated_output, tbl_JOIN_LEFTANTI_TD_5733_consolidated_output, tbl_JOIN_INNER_TD_4616_output, cfg_JOIN_INNER_TD_4616_cmds_build, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_4616_probe;
    krnl_JOIN_INNER_TD_4616_probe = krnlEngine(program_h, q_h, context_h, "gqeKernel", 1);
    krnl_JOIN_INNER_TD_4616_probe.setup(tbl_JOIN_LEFTANTI_TD_5733_consolidated_output, tbl_JOIN_INNER_TD_3653_consolidated_output, tbl_JOIN_INNER_TD_4616_output, cfg_JOIN_INNER_TD_4616_cmds_probe, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_3653_part_left;
    krnl_JOIN_INNER_TD_3653_part_left = krnlEngine(program_h, q_h, context_h, "gqePart");
    krnl_JOIN_INNER_TD_3653_part_left.setup_hp(512, 0, power_of_hpTimes_join, tbl_SerializeFromObject_TD_5188_input_stringRowIDSubstitute, tbl_SerializeFromObject_TD_5188_input_stringRowIDSubstitute_partition, cfg_JOIN_INNER_TD_3653_cmds_part);
    krnlEngine krnl_JOIN_INNER_TD_3653_part_right;
    krnl_JOIN_INNER_TD_3653_part_right = krnlEngine(program_h, q_h, context_h, "gqePart");
    krnl_JOIN_INNER_TD_3653_part_right.setup_hp(512, 1, power_of_hpTimes_join, tbl_Filter_TD_3611_output, tbl_Filter_TD_3611_output_partition, cfg_JOIN_INNER_TD_3653_cmds_part);
    krnlEngine krnl_JOIN_INNER_TD_3653[hpTimes_join];
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_INNER_TD_3653[i] = krnlEngine(program_h, q_h, "gqeJoin");
    }
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_INNER_TD_3653[i].setup(tbl_SerializeFromObject_TD_5188_input_stringRowIDSubstitute_partition_array[i], tbl_Filter_TD_3611_output_partition_array[i], tbl_JOIN_INNER_TD_3653_output_partition_array[i], , buftmp_h);
    }
    krnlEngine krnl_JOIN_INNER_TD_3653_build;
    krnl_JOIN_INNER_TD_3653_build = krnlEngine(program_h, q_h, context_h, "gqeKernel", 0);
    krnl_JOIN_INNER_TD_3653_build.setup(tbl_SerializeFromObject_TD_5188_input_stringRowIDSubstitute, tbl_Filter_TD_3611_output, tbl_JOIN_INNER_TD_3653_output, cfg_JOIN_INNER_TD_3653_cmds_build, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_3653_probe;
    krnl_JOIN_INNER_TD_3653_probe = krnlEngine(program_h, q_h, context_h, "gqeKernel", 1);
    krnl_JOIN_INNER_TD_3653_probe.setup(tbl_Filter_TD_3611_output, tbl_SerializeFromObject_TD_5188_input_stringRowIDSubstitute, tbl_JOIN_INNER_TD_3653_output, cfg_JOIN_INNER_TD_3653_cmds_probe, buftmp_h);
    krnlEngine krnl_JOIN_LEFTANTI_TD_5733_part_right;
    krnl_JOIN_LEFTANTI_TD_5733_part_right = krnlEngine(program_h, q_h, context_h, "gqePart");
    krnl_JOIN_LEFTANTI_TD_5733_part_right.setup_hp(512, 1, power_of_hpTimes_join, tbl_Filter_TD_7203_output, tbl_Filter_TD_7203_output_partition, cfg_JOIN_LEFTANTI_TD_5733_cmds_part);
    krnlEngine krnl_JOIN_LEFTANTI_TD_5733[hpTimes_join];
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_LEFTANTI_TD_5733[i] = krnlEngine(program_h, q_h, "gqeJoin");
    }
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_LEFTANTI_TD_5733[i].setup(tbl_JOIN_LEFTSEMI_TD_6827_consolidated_output_partition_array[i], tbl_Filter_TD_7203_output_partition_array[i], tbl_JOIN_LEFTANTI_TD_5733_output_partition_array[i], , buftmp_h);
    }
    krnlEngine krnl_JOIN_LEFTANTI_TD_5733_build;
    krnl_JOIN_LEFTANTI_TD_5733_build = krnlEngine(program_h, q_h, context_h, "gqeKernel", 0);
    krnl_JOIN_LEFTANTI_TD_5733_build.setup(tbl_JOIN_LEFTSEMI_TD_6827_consolidated_output, tbl_Filter_TD_7203_output, tbl_JOIN_LEFTANTI_TD_5733_output, cfg_JOIN_LEFTANTI_TD_5733_cmds_build, buftmp_h);
    krnlEngine krnl_JOIN_LEFTANTI_TD_5733_probe;
    krnl_JOIN_LEFTANTI_TD_5733_probe = krnlEngine(program_h, q_h, context_h, "gqeKernel", 1);
    krnl_JOIN_LEFTANTI_TD_5733_probe.setup(tbl_Filter_TD_7203_output, tbl_JOIN_LEFTSEMI_TD_6827_consolidated_output, tbl_JOIN_LEFTANTI_TD_5733_output, cfg_JOIN_LEFTANTI_TD_5733_cmds_probe, buftmp_h);
    krnlEngine krnl_JOIN_LEFTSEMI_TD_6827_part_left;
    krnl_JOIN_LEFTSEMI_TD_6827_part_left = krnlEngine(program_h, q_h, context_h, "gqePart");
    krnl_JOIN_LEFTSEMI_TD_6827_part_left.setup_hp(512, 0, power_of_hpTimes_join, tbl_Filter_TD_7203_output, tbl_Filter_TD_7203_output_partition, cfg_JOIN_LEFTSEMI_TD_6827_cmds_part);
    krnlEngine krnl_JOIN_LEFTSEMI_TD_6827[hpTimes_join];
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_LEFTSEMI_TD_6827[i] = krnlEngine(program_h, q_h, "gqeJoin");
    }
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_LEFTSEMI_TD_6827[i].setup(tbl_Filter_TD_7203_output_partition_array[i], tbl_JOIN_LEFTSEMI_TD_7346_consolidated_output_partition_array[i], tbl_JOIN_LEFTSEMI_TD_6827_output_partition_array[i], , buftmp_h);
    }
    krnlEngine krnl_JOIN_LEFTSEMI_TD_6827_build;
    krnl_JOIN_LEFTSEMI_TD_6827_build = krnlEngine(program_h, q_h, context_h, "gqeKernel", 0);
    krnl_JOIN_LEFTSEMI_TD_6827_build.setup(tbl_Filter_TD_7203_output, tbl_JOIN_LEFTSEMI_TD_7346_consolidated_output, tbl_JOIN_LEFTSEMI_TD_6827_output, cfg_JOIN_LEFTSEMI_TD_6827_cmds_build, buftmp_h);
    krnlEngine krnl_JOIN_LEFTSEMI_TD_6827_probe;
    krnl_JOIN_LEFTSEMI_TD_6827_probe = krnlEngine(program_h, q_h, context_h, "gqeKernel", 1);
    krnl_JOIN_LEFTSEMI_TD_6827_probe.setup(tbl_JOIN_LEFTSEMI_TD_7346_consolidated_output, tbl_Filter_TD_7203_output, tbl_JOIN_LEFTSEMI_TD_6827_output, cfg_JOIN_LEFTSEMI_TD_6827_cmds_probe, buftmp_h);
    krnlEngine krnl_JOIN_LEFTSEMI_TD_7346_part_left;
    krnl_JOIN_LEFTSEMI_TD_7346_part_left = krnlEngine(program_h, q_h, context_h, "gqePart");
    krnl_JOIN_LEFTSEMI_TD_7346_part_left.setup_hp(512, 0, power_of_hpTimes_join, tbl_SerializeFromObject_TD_8225_input, tbl_SerializeFromObject_TD_8225_input_partition, cfg_JOIN_LEFTSEMI_TD_7346_cmds_part);
    krnlEngine krnl_JOIN_LEFTSEMI_TD_7346_part_right;
    krnl_JOIN_LEFTSEMI_TD_7346_part_right = krnlEngine(program_h, q_h, context_h, "gqePart");
    krnl_JOIN_LEFTSEMI_TD_7346_part_right.setup_hp(512, 1, power_of_hpTimes_join, tbl_Filter_TD_8644_output, tbl_Filter_TD_8644_output_partition, cfg_JOIN_LEFTSEMI_TD_7346_cmds_part);
    krnlEngine krnl_JOIN_LEFTSEMI_TD_7346[hpTimes_join];
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_LEFTSEMI_TD_7346[i] = krnlEngine(program_h, q_h, "gqeJoin");
    }
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_LEFTSEMI_TD_7346[i].setup(tbl_SerializeFromObject_TD_8225_input_partition_array[i], tbl_Filter_TD_8644_output_partition_array[i], tbl_JOIN_LEFTSEMI_TD_7346_output_partition_array[i], , buftmp_h);
    }
    krnlEngine krnl_JOIN_LEFTSEMI_TD_7346_build;
    krnl_JOIN_LEFTSEMI_TD_7346_build = krnlEngine(program_h, q_h, context_h, "gqeKernel", 0);
    krnl_JOIN_LEFTSEMI_TD_7346_build.setup(tbl_SerializeFromObject_TD_8225_input, tbl_Filter_TD_8644_output, tbl_JOIN_LEFTSEMI_TD_7346_output, cfg_JOIN_LEFTSEMI_TD_7346_cmds_build, buftmp_h);
    krnlEngine krnl_JOIN_LEFTSEMI_TD_7346_probe;
    krnl_JOIN_LEFTSEMI_TD_7346_probe = krnlEngine(program_h, q_h, context_h, "gqeKernel", 1);
    krnl_JOIN_LEFTSEMI_TD_7346_probe.setup(tbl_Filter_TD_8644_output, tbl_SerializeFromObject_TD_8225_input, tbl_JOIN_LEFTSEMI_TD_7346_output, cfg_JOIN_LEFTSEMI_TD_7346_cmds_probe, buftmp_h);
    // ************************** Transfer Engine *********************** // 
    transEngine trans_JOIN_INNER_TD_2684;
    trans_JOIN_INNER_TD_2684.setq(q_h,1);
    trans_JOIN_INNER_TD_2684.add(&(cfg_JOIN_INNER_TD_2684_cmds_part));
    trans_JOIN_INNER_TD_2684.add(&(cfg_JOIN_INNER_TD_2684_cmds_build));
    trans_JOIN_INNER_TD_2684.add(&(cfg_JOIN_INNER_TD_2684_cmds_probe));
    transEngine trans_JOIN_INNER_TD_2684_out;
    trans_JOIN_INNER_TD_2684_out.setq(q_h,1);
    q_h.finish();
    transEngine trans_JOIN_INNER_TD_4616;
    trans_JOIN_INNER_TD_4616.setq(q_h,1);
    trans_JOIN_INNER_TD_4616.add(&(cfg_JOIN_INNER_TD_4616_cmds_part));
    trans_JOIN_INNER_TD_4616.add(&(cfg_JOIN_INNER_TD_4616_cmds_build));
    trans_JOIN_INNER_TD_4616.add(&(cfg_JOIN_INNER_TD_4616_cmds_probe));
    transEngine trans_JOIN_INNER_TD_4616_out;
    trans_JOIN_INNER_TD_4616_out.setq(q_h,1);
    q_h.finish();
    transEngine trans_JOIN_INNER_TD_3653;
    trans_JOIN_INNER_TD_3653.setq(q_h,1);
    trans_JOIN_INNER_TD_3653.add(&(cfg_JOIN_INNER_TD_3653_cmds_part));
    trans_JOIN_INNER_TD_3653.add(&(cfg_JOIN_INNER_TD_3653_cmds_build));
    trans_JOIN_INNER_TD_3653.add(&(cfg_JOIN_INNER_TD_3653_cmds_probe));
    trans_JOIN_INNER_TD_3653.add(&(tbl_SerializeFromObject_TD_5188_input_stringRowIDSubstitute));
    transEngine trans_JOIN_INNER_TD_3653_out;
    trans_JOIN_INNER_TD_3653_out.setq(q_h,1);
    q_h.finish();
    transEngine trans_JOIN_LEFTANTI_TD_5733;
    trans_JOIN_LEFTANTI_TD_5733.setq(q_h,1);
    trans_JOIN_LEFTANTI_TD_5733.add(&(cfg_JOIN_LEFTANTI_TD_5733_cmds_part));
    trans_JOIN_LEFTANTI_TD_5733.add(&(cfg_JOIN_LEFTANTI_TD_5733_cmds_build));
    trans_JOIN_LEFTANTI_TD_5733.add(&(cfg_JOIN_LEFTANTI_TD_5733_cmds_probe));
    transEngine trans_JOIN_LEFTANTI_TD_5733_out;
    trans_JOIN_LEFTANTI_TD_5733_out.setq(q_h,1);
    q_h.finish();
    transEngine trans_JOIN_LEFTSEMI_TD_6827;
    trans_JOIN_LEFTSEMI_TD_6827.setq(q_h,1);
    trans_JOIN_LEFTSEMI_TD_6827.add(&(cfg_JOIN_LEFTSEMI_TD_6827_cmds_part));
    trans_JOIN_LEFTSEMI_TD_6827.add(&(cfg_JOIN_LEFTSEMI_TD_6827_cmds_build));
    trans_JOIN_LEFTSEMI_TD_6827.add(&(cfg_JOIN_LEFTSEMI_TD_6827_cmds_probe));
    transEngine trans_JOIN_LEFTSEMI_TD_6827_out;
    trans_JOIN_LEFTSEMI_TD_6827_out.setq(q_h,1);
    q_h.finish();
    transEngine trans_JOIN_LEFTSEMI_TD_7346;
    trans_JOIN_LEFTSEMI_TD_7346.setq(q_h,1);
    trans_JOIN_LEFTSEMI_TD_7346.add(&(cfg_JOIN_LEFTSEMI_TD_7346_cmds_part));
    trans_JOIN_LEFTSEMI_TD_7346.add(&(cfg_JOIN_LEFTSEMI_TD_7346_cmds_build));
    trans_JOIN_LEFTSEMI_TD_7346.add(&(cfg_JOIN_LEFTSEMI_TD_7346_cmds_probe));
    trans_JOIN_LEFTSEMI_TD_7346.add(&(tbl_SerializeFromObject_TD_8225_input));
    transEngine trans_JOIN_LEFTSEMI_TD_7346_out;
    trans_JOIN_LEFTSEMI_TD_7346_out.setq(q_h,1);
    q_h.finish();
    // ****************************** Events **************************** // 
    std::vector<cl::Event> events_h2d_wr_JOIN_INNER_TD_2684;
    std::vector<cl::Event> events_d2h_rd_JOIN_INNER_TD_2684;
    std::vector<cl::Event> events_JOIN_INNER_TD_2684[2];
    events_h2d_wr_JOIN_INNER_TD_2684.resize(1);
    events_d2h_rd_JOIN_INNER_TD_2684.resize(1);
    events_JOIN_INNER_TD_2684[0].resize(1);
    events_JOIN_INNER_TD_2684[1].resize(hpTimes_join);
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_2684;
    std::vector<cl::Event> prev_events_grp_JOIN_INNER_TD_2684;
    std::vector<cl::Event> events_h2d_wr_JOIN_INNER_TD_4616;
    std::vector<cl::Event> events_d2h_rd_JOIN_INNER_TD_4616;
    std::vector<cl::Event> events_JOIN_INNER_TD_4616[2];
    events_h2d_wr_JOIN_INNER_TD_4616.resize(1);
    events_d2h_rd_JOIN_INNER_TD_4616.resize(1);
    events_JOIN_INNER_TD_4616[0].resize(0);
    events_JOIN_INNER_TD_4616[1].resize(hpTimes_join);
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_4616;
    std::vector<cl::Event> prev_events_grp_JOIN_INNER_TD_4616;
    std::vector<cl::Event> events_h2d_wr_JOIN_INNER_TD_3653;
    std::vector<cl::Event> events_d2h_rd_JOIN_INNER_TD_3653;
    std::vector<cl::Event> events_JOIN_INNER_TD_3653[2];
    events_h2d_wr_JOIN_INNER_TD_3653.resize(1);
    events_d2h_rd_JOIN_INNER_TD_3653.resize(1);
    events_JOIN_INNER_TD_3653[0].resize(2);
    events_JOIN_INNER_TD_3653[1].resize(hpTimes_join);
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_3653;
    std::vector<cl::Event> prev_events_grp_JOIN_INNER_TD_3653;
    std::vector<cl::Event> events_h2d_wr_JOIN_LEFTANTI_TD_5733;
    std::vector<cl::Event> events_d2h_rd_JOIN_LEFTANTI_TD_5733;
    std::vector<cl::Event> events_JOIN_LEFTANTI_TD_5733[2];
    events_h2d_wr_JOIN_LEFTANTI_TD_5733.resize(1);
    events_d2h_rd_JOIN_LEFTANTI_TD_5733.resize(1);
    events_JOIN_LEFTANTI_TD_5733[0].resize(1);
    events_JOIN_LEFTANTI_TD_5733[1].resize(hpTimes_join);
    std::vector<cl::Event> events_grp_JOIN_LEFTANTI_TD_5733;
    std::vector<cl::Event> prev_events_grp_JOIN_LEFTANTI_TD_5733;
    std::vector<cl::Event> events_h2d_wr_JOIN_LEFTSEMI_TD_6827;
    std::vector<cl::Event> events_d2h_rd_JOIN_LEFTSEMI_TD_6827;
    std::vector<cl::Event> events_JOIN_LEFTSEMI_TD_6827[2];
    events_h2d_wr_JOIN_LEFTSEMI_TD_6827.resize(1);
    events_d2h_rd_JOIN_LEFTSEMI_TD_6827.resize(1);
    events_JOIN_LEFTSEMI_TD_6827[0].resize(1);
    events_JOIN_LEFTSEMI_TD_6827[1].resize(hpTimes_join);
    std::vector<cl::Event> events_grp_JOIN_LEFTSEMI_TD_6827;
    std::vector<cl::Event> prev_events_grp_JOIN_LEFTSEMI_TD_6827;
    std::vector<cl::Event> events_h2d_wr_JOIN_LEFTSEMI_TD_7346;
    std::vector<cl::Event> events_d2h_rd_JOIN_LEFTSEMI_TD_7346;
    std::vector<cl::Event> events_JOIN_LEFTSEMI_TD_7346[2];
    events_h2d_wr_JOIN_LEFTSEMI_TD_7346.resize(1);
    events_d2h_rd_JOIN_LEFTSEMI_TD_7346.resize(1);
    events_JOIN_LEFTSEMI_TD_7346[0].resize(2);
    events_JOIN_LEFTSEMI_TD_7346[1].resize(hpTimes_join);
    std::vector<cl::Event> events_grp_JOIN_LEFTSEMI_TD_7346;
    std::vector<cl::Event> prev_events_grp_JOIN_LEFTSEMI_TD_7346;
    // **************************** Operations ************************** // 
    struct timeval tv_r_s, tv_r_e; 
    gettimeofday(&tv_r_s, 0); 

    struct timeval tv_r_Filter_8_421_s, tv_r_Filter_8_421_e;
    gettimeofday(&tv_r_Filter_8_421_s, 0);
    SW_Filter_TD_8644(tbl_SerializeFromObject_TD_9935_input, tbl_Filter_TD_8644_output);
    gettimeofday(&tv_r_Filter_8_421_e, 0);

    struct timeval tv_r_Filter_7_107_s, tv_r_Filter_7_107_e;
    gettimeofday(&tv_r_Filter_7_107_s, 0);
    SW_Filter_TD_7203(tbl_SerializeFromObject_TD_9935_input, tbl_Filter_TD_7203_output);
    gettimeofday(&tv_r_Filter_7_107_e, 0);

    struct timeval tv_r_JOIN_LEFTSEMI_7_884_s, tv_r_JOIN_LEFTSEMI_7_884_e;
    gettimeofday(&tv_r_JOIN_LEFTSEMI_7_884_s, 0);
    tbl_SerializeFromObject_TD_8225_input.tableToCol();
    tbl_Filter_TD_8644_output.tableToCol();
    krnl_JOIN_LEFTSEMI_TD_7346_build.updateMeta(tbl_SerializeFromObject_TD_8225_input.getNumRow(), 3);
    krnl_JOIN_LEFTSEMI_TD_7346_probe.updateMeta(tbl_Filter_TD_8644_output.getNumRow(), 3);
    trans_JOIN_LEFTSEMI_TD_7346.add(&(tbl_Filter_TD_8644_output));
    trans_JOIN_LEFTSEMI_TD_7346.host2dev(0, &(prev_events_grp_JOIN_LEFTSEMI_TD_7346), &(events_h2d_wr_JOIN_LEFTSEMI_TD_7346[0]));
    events_grp_JOIN_LEFTSEMI_TD_7346.push_back(events_h2d_wr_JOIN_LEFTSEMI_TD_7346[0]);
    krnl_JOIN_LEFTSEMI_TD_7346_part_left.run(0, &(events_grp_JOIN_LEFTSEMI_TD_7346), &(events_JOIN_LEFTSEMI_TD_7346[0][0]));
    krnl_JOIN_LEFTSEMI_TD_7346_part_right.run(0, &(events_grp_JOIN_LEFTSEMI_TD_7346), &(events_JOIN_LEFTSEMI_TD_7346[0][1]));
    for (int i(0); i < hpTimes_join; ++i) {
        krnl_JOIN_LEFTSEMI_TD_7346[i].run(0, &(events_JOIN_LEFTSEMI_TD_7346[0]), &(events_JOIN_LEFTSEMI_TD_7346[1][i]));
    }
    std::vector<cl::Event> events_grp_JOIN_LEFTSEMI_TD_7346_build_done;
    events_grp_JOIN_LEFTSEMI_TD_7346_build_done.push_back(events_JOIN_LEFTSEMI_TD_7346[0]);
    krnl_JOIN_LEFTSEMI_TD_7346_part_left.run(0, &(events_grp_JOIN_LEFTSEMI_TD_7346), &(events_JOIN_LEFTSEMI_TD_7346[0][0]));
    krnl_JOIN_LEFTSEMI_TD_7346_part_right.run(0, &(events_grp_JOIN_LEFTSEMI_TD_7346), &(events_JOIN_LEFTSEMI_TD_7346[0][1]));
    for (int i(0); i < hpTimes_join; ++i) {
        krnl_JOIN_LEFTSEMI_TD_7346[i].run(0, &(events_JOIN_LEFTSEMI_TD_7346[0]), &(events_JOIN_LEFTSEMI_TD_7346[1][i]));
    }
    std::vector<cl::Event> events_grp_JOIN_LEFTSEMI_TD_7346_probe_done;
    events_grp_JOIN_LEFTSEMI_TD_7346_probe_done.push_back(events_JOIN_LEFTSEMI_TD_7346[1]);
    for (int i(0); i < hpTimes_join; ++i) {
        trans_JOIN_LEFTSEMI_TD_7346_out.add(&(tbl_JOIN_LEFTSEMI_TD_7346_output_partition_array[i]));
    }
    trans_JOIN_LEFTSEMI_TD_7346_out.dev2host(0, &(events_JOIN_LEFTSEMI_TD_7346[1]), &(events_d2h_rd_JOIN_LEFTSEMI_TD_7346[0]));
    q_h.flush();
    q_h.finish();
    tbl_JOIN_LEFTSEMI_TD_7346_output.setNumRow((cfg_JOIN_LEFTSEMI_TD_7346_cmds_probe.meta_out)->getColLen());
    tbl_JOIN_LEFTSEMI_TD_7346_output.colToTable();
    SW_Consolidated_JOIN_LEFTSEMI_TD_7346_output(tbl_SerializeFromObject_TD_8225_input, tbl_Filter_TD_8644_output, tbl_JOIN_LEFTSEMI_TD_7346_output, tbl_JOIN_LEFTSEMI_TD_7346_consolidated_output);
    gettimeofday(&tv_r_JOIN_LEFTSEMI_7_884_e, 0);

    struct timeval tv_r_JOIN_LEFTSEMI_6_559_s, tv_r_JOIN_LEFTSEMI_6_559_e;
    gettimeofday(&tv_r_JOIN_LEFTSEMI_6_559_s, 0);
    prev_events_grp_JOIN_LEFTSEMI_TD_6827.push_back(events_h2d_wr_JOIN_LEFTSEMI_TD_7346[0]);
    tbl_Filter_TD_7203_output.tableToCol();
    tbl_JOIN_LEFTSEMI_TD_7346_consolidated_output.tableToCol();
    krnl_JOIN_LEFTSEMI_TD_6827_build.updateMeta(tbl_Filter_TD_7203_output.getNumRow(), 3);
    krnl_JOIN_LEFTSEMI_TD_6827_probe.updateMeta(tbl_JOIN_LEFTSEMI_TD_7346_consolidated_output.getNumRow(), 3);
    trans_JOIN_LEFTSEMI_TD_6827.add(&(tbl_Filter_TD_7203_output));
    trans_JOIN_LEFTSEMI_TD_6827.host2dev(0, &(prev_events_grp_JOIN_LEFTSEMI_TD_6827), &(events_h2d_wr_JOIN_LEFTSEMI_TD_6827[0]));
    events_grp_JOIN_LEFTSEMI_TD_6827.push_back(events_h2d_wr_JOIN_LEFTSEMI_TD_6827[0]);
    events_grp_JOIN_LEFTSEMI_TD_6827.insert(std::end(events_grp_JOIN_LEFTSEMI_TD_6827), std::begin(events_JOIN_LEFTSEMI_TD_7346[0]), std::end(events_JOIN_LEFTSEMI_TD_7346[0]));
    events_grp_JOIN_LEFTSEMI_TD_6827.insert(std::end(events_grp_JOIN_LEFTSEMI_TD_6827), std::begin(events_JOIN_LEFTSEMI_TD_7346[1]), std::end(events_JOIN_LEFTSEMI_TD_7346[1]));
    krnl_JOIN_LEFTSEMI_TD_6827_part_left.run(0, &(events_grp_JOIN_LEFTSEMI_TD_6827), &(events_JOIN_LEFTSEMI_TD_6827[0][0]));
    for (int i(0); i < hpTimes_join; ++i) {
        krnl_JOIN_LEFTSEMI_TD_6827[i].run(0, &(events_JOIN_LEFTSEMI_TD_6827[0]), &(events_JOIN_LEFTSEMI_TD_6827[1][i]));
    }
    std::vector<cl::Event> events_grp_JOIN_LEFTSEMI_TD_6827_build_done;
    events_grp_JOIN_LEFTSEMI_TD_6827_build_done.push_back(events_JOIN_LEFTSEMI_TD_6827[0]);
    krnl_JOIN_LEFTSEMI_TD_6827_part_left.run(0, &(events_grp_JOIN_LEFTSEMI_TD_6827), &(events_JOIN_LEFTSEMI_TD_6827[0][0]));
    for (int i(0); i < hpTimes_join; ++i) {
        krnl_JOIN_LEFTSEMI_TD_6827[i].run(0, &(events_JOIN_LEFTSEMI_TD_6827[0]), &(events_JOIN_LEFTSEMI_TD_6827[1][i]));
    }
    std::vector<cl::Event> events_grp_JOIN_LEFTSEMI_TD_6827_probe_done;
    events_grp_JOIN_LEFTSEMI_TD_6827_probe_done.push_back(events_JOIN_LEFTSEMI_TD_6827[1]);
    for (int i(0); i < hpTimes_join; ++i) {
        trans_JOIN_LEFTSEMI_TD_6827_out.add(&(tbl_JOIN_LEFTSEMI_TD_6827_output_partition_array[i]));
    }
    trans_JOIN_LEFTSEMI_TD_6827_out.dev2host(0, &(events_JOIN_LEFTSEMI_TD_6827[1]), &(events_d2h_rd_JOIN_LEFTSEMI_TD_6827[0]));
    q_h.flush();
    q_h.finish();
    tbl_JOIN_LEFTSEMI_TD_6827_output.setNumRow((cfg_JOIN_LEFTSEMI_TD_6827_cmds_probe.meta_out)->getColLen());
    tbl_JOIN_LEFTSEMI_TD_6827_output.colToTable();
    SW_Consolidated_JOIN_LEFTSEMI_TD_6827_output(tbl_Filter_TD_7203_output, tbl_JOIN_LEFTSEMI_TD_7346_consolidated_output, tbl_JOIN_LEFTSEMI_TD_6827_output, tbl_JOIN_LEFTSEMI_TD_6827_consolidated_output);
    gettimeofday(&tv_r_JOIN_LEFTSEMI_6_559_e, 0);

    struct timeval tv_r_Filter_3_944_s, tv_r_Filter_3_944_e;
    gettimeofday(&tv_r_Filter_3_944_s, 0);
    SW_Filter_TD_3611(tbl_SerializeFromObject_TD_4935_input, tbl_Filter_TD_3611_output);
    gettimeofday(&tv_r_Filter_3_944_e, 0);

    struct timeval tv_r_JOIN_LEFTANTI_5_798_s, tv_r_JOIN_LEFTANTI_5_798_e;
    gettimeofday(&tv_r_JOIN_LEFTANTI_5_798_s, 0);
    prev_events_grp_JOIN_LEFTANTI_TD_5733.push_back(events_h2d_wr_JOIN_LEFTSEMI_TD_6827[0]);
    tbl_JOIN_LEFTSEMI_TD_6827_consolidated_output.tableToCol();
    tbl_Filter_TD_7203_output.tableToCol();
    krnl_JOIN_LEFTANTI_TD_5733_build.updateMeta(tbl_JOIN_LEFTSEMI_TD_6827_consolidated_output.getNumRow(), 3);
    krnl_JOIN_LEFTANTI_TD_5733_probe.updateMeta(tbl_Filter_TD_7203_output.getNumRow(), 3);
    trans_JOIN_LEFTANTI_TD_5733.add(&(tbl_Filter_TD_7203_output));
    trans_JOIN_LEFTANTI_TD_5733.host2dev(0, &(prev_events_grp_JOIN_LEFTANTI_TD_5733), &(events_h2d_wr_JOIN_LEFTANTI_TD_5733[0]));
    events_grp_JOIN_LEFTANTI_TD_5733.push_back(events_h2d_wr_JOIN_LEFTANTI_TD_5733[0]);
    events_grp_JOIN_LEFTANTI_TD_5733.insert(std::end(events_grp_JOIN_LEFTANTI_TD_5733), std::begin(events_JOIN_LEFTSEMI_TD_6827[0]), std::end(events_JOIN_LEFTSEMI_TD_6827[0]));
    events_grp_JOIN_LEFTANTI_TD_5733.insert(std::end(events_grp_JOIN_LEFTANTI_TD_5733), std::begin(events_JOIN_LEFTSEMI_TD_6827[1]), std::end(events_JOIN_LEFTSEMI_TD_6827[1]));
    krnl_JOIN_LEFTANTI_TD_5733_part_right.run(0, &(events_grp_JOIN_LEFTANTI_TD_5733), &(events_JOIN_LEFTANTI_TD_5733[0][0]));
    for (int i(0); i < hpTimes_join; ++i) {
        krnl_JOIN_LEFTANTI_TD_5733[i].run(0, &(events_grp_JOIN_LEFTANTI_TD_5733), &(events_JOIN_LEFTANTI_TD_5733[1][i]));
    }
    std::vector<cl::Event> events_grp_JOIN_LEFTANTI_TD_5733_build_done;
    events_grp_JOIN_LEFTANTI_TD_5733_build_done.push_back(events_JOIN_LEFTANTI_TD_5733[0]);
    krnl_JOIN_LEFTANTI_TD_5733_part_right.run(0, &(events_grp_JOIN_LEFTANTI_TD_5733), &(events_JOIN_LEFTANTI_TD_5733[0][0]));
    for (int i(0); i < hpTimes_join; ++i) {
        krnl_JOIN_LEFTANTI_TD_5733[i].run(0, &(events_grp_JOIN_LEFTANTI_TD_5733), &(events_JOIN_LEFTANTI_TD_5733[1][i]));
    }
    std::vector<cl::Event> events_grp_JOIN_LEFTANTI_TD_5733_probe_done;
    events_grp_JOIN_LEFTANTI_TD_5733_probe_done.push_back(events_JOIN_LEFTANTI_TD_5733[1]);
    for (int i(0); i < hpTimes_join; ++i) {
        trans_JOIN_LEFTANTI_TD_5733_out.add(&(tbl_JOIN_LEFTANTI_TD_5733_output_partition_array[i]));
    }
    trans_JOIN_LEFTANTI_TD_5733_out.dev2host(0, &(events_JOIN_LEFTANTI_TD_5733[1]), &(events_d2h_rd_JOIN_LEFTANTI_TD_5733[0]));
    q_h.flush();
    q_h.finish();
    tbl_JOIN_LEFTANTI_TD_5733_output.setNumRow((cfg_JOIN_LEFTANTI_TD_5733_cmds_probe.meta_out)->getColLen());
    tbl_JOIN_LEFTANTI_TD_5733_output.colToTable();
    SW_Consolidated_JOIN_LEFTANTI_TD_5733_output(tbl_JOIN_LEFTSEMI_TD_6827_consolidated_output, tbl_Filter_TD_7203_output, tbl_JOIN_LEFTANTI_TD_5733_output, tbl_JOIN_LEFTANTI_TD_5733_consolidated_output);
    gettimeofday(&tv_r_JOIN_LEFTANTI_5_798_e, 0);

    struct timeval tv_r_JOIN_INNER_3_711_s, tv_r_JOIN_INNER_3_711_e;
    gettimeofday(&tv_r_JOIN_INNER_3_711_s, 0);
    tbl_SerializeFromObject_TD_5188_input_stringRowIDSubstitute.tableToCol();
    tbl_Filter_TD_3611_output.tableToCol();
    krnl_JOIN_INNER_TD_3653_build.updateMeta(tbl_SerializeFromObject_TD_5188_input_stringRowIDSubstitute.getNumRow(), 3);
    krnl_JOIN_INNER_TD_3653_probe.updateMeta(tbl_Filter_TD_3611_output.getNumRow(), 3);
    trans_JOIN_INNER_TD_3653.add(&(tbl_Filter_TD_3611_output));
    trans_JOIN_INNER_TD_3653.host2dev(0, &(prev_events_grp_JOIN_INNER_TD_3653), &(events_h2d_wr_JOIN_INNER_TD_3653[0]));
    events_grp_JOIN_INNER_TD_3653.push_back(events_h2d_wr_JOIN_INNER_TD_3653[0]);
    krnl_JOIN_INNER_TD_3653_part_left.run(0, &(events_grp_JOIN_INNER_TD_3653), &(events_JOIN_INNER_TD_3653[0][0]));
    krnl_JOIN_INNER_TD_3653_part_right.run(0, &(events_grp_JOIN_INNER_TD_3653), &(events_JOIN_INNER_TD_3653[0][1]));
    for (int i(0); i < hpTimes_join; ++i) {
        krnl_JOIN_INNER_TD_3653[i].run(0, &(events_JOIN_INNER_TD_3653[0]), &(events_JOIN_INNER_TD_3653[1][i]));
    }
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_3653_build_done;
    events_grp_JOIN_INNER_TD_3653_build_done.push_back(events_JOIN_INNER_TD_3653[0]);
    krnl_JOIN_INNER_TD_3653_part_left.run(0, &(events_grp_JOIN_INNER_TD_3653), &(events_JOIN_INNER_TD_3653[0][0]));
    krnl_JOIN_INNER_TD_3653_part_right.run(0, &(events_grp_JOIN_INNER_TD_3653), &(events_JOIN_INNER_TD_3653[0][1]));
    for (int i(0); i < hpTimes_join; ++i) {
        krnl_JOIN_INNER_TD_3653[i].run(0, &(events_JOIN_INNER_TD_3653[0]), &(events_JOIN_INNER_TD_3653[1][i]));
    }
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_3653_probe_done;
    events_grp_JOIN_INNER_TD_3653_probe_done.push_back(events_JOIN_INNER_TD_3653[1]);
    for (int i(0); i < hpTimes_join; ++i) {
        trans_JOIN_INNER_TD_3653_out.add(&(tbl_JOIN_INNER_TD_3653_output_partition_array[i]));
    }
    trans_JOIN_INNER_TD_3653_out.dev2host(0, &(events_JOIN_INNER_TD_3653[1]), &(events_d2h_rd_JOIN_INNER_TD_3653[0]));
    q_h.flush();
    q_h.finish();
    tbl_JOIN_INNER_TD_3653_output.setNumRow((cfg_JOIN_INNER_TD_3653_cmds_probe.meta_out)->getColLen());
    tbl_JOIN_INNER_TD_3653_output.colToTable();
    SW_Consolidated_JOIN_INNER_TD_3653_output(tbl_SerializeFromObject_TD_5188_input_stringRowIDSubstitute, tbl_Filter_TD_3611_output, tbl_JOIN_INNER_TD_3653_output, tbl_JOIN_INNER_TD_3653_consolidated_output);
    gettimeofday(&tv_r_JOIN_INNER_3_711_e, 0);

    struct timeval tv_r_JOIN_INNER_4_244_s, tv_r_JOIN_INNER_4_244_e;
    gettimeofday(&tv_r_JOIN_INNER_4_244_s, 0);
    prev_events_grp_JOIN_INNER_TD_4616.push_back(events_h2d_wr_JOIN_INNER_TD_3653[0]);
    prev_events_grp_JOIN_INNER_TD_4616.push_back(events_h2d_wr_JOIN_LEFTANTI_TD_5733[0]);
    tbl_JOIN_INNER_TD_3653_consolidated_output.tableToCol();
    tbl_JOIN_LEFTANTI_TD_5733_consolidated_output.tableToCol();
    krnl_JOIN_INNER_TD_4616_build.updateMeta(tbl_JOIN_INNER_TD_3653_consolidated_output.getNumRow(), 3);
    krnl_JOIN_INNER_TD_4616_probe.updateMeta(tbl_JOIN_LEFTANTI_TD_5733_consolidated_output.getNumRow(), 3);
    trans_JOIN_INNER_TD_4616.host2dev(0, &(prev_events_grp_JOIN_INNER_TD_4616), &(events_h2d_wr_JOIN_INNER_TD_4616[0]));
    events_grp_JOIN_INNER_TD_4616.push_back(events_h2d_wr_JOIN_INNER_TD_4616[0]);
    events_grp_JOIN_INNER_TD_4616.insert(std::end(events_grp_JOIN_INNER_TD_4616), std::begin(events_JOIN_INNER_TD_3653[0]), std::end(events_JOIN_INNER_TD_3653[0]));
    events_grp_JOIN_INNER_TD_4616.insert(std::end(events_grp_JOIN_INNER_TD_4616), std::begin(events_JOIN_INNER_TD_3653[1]), std::end(events_JOIN_INNER_TD_3653[1]));
    events_grp_JOIN_INNER_TD_4616.insert(std::end(events_grp_JOIN_INNER_TD_4616), std::begin(events_JOIN_LEFTANTI_TD_5733[0]), std::end(events_JOIN_LEFTANTI_TD_5733[0]));
    events_grp_JOIN_INNER_TD_4616.insert(std::end(events_grp_JOIN_INNER_TD_4616), std::begin(events_JOIN_LEFTANTI_TD_5733[1]), std::end(events_JOIN_LEFTANTI_TD_5733[1]));
    for (int i(0); i < hpTimes_join; ++i) {
        krnl_JOIN_INNER_TD_4616[i].run(0, &(events_grp_JOIN_INNER_TD_4616), &(events_JOIN_INNER_TD_4616[1][i]));
    }
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_4616_build_done;
    events_grp_JOIN_INNER_TD_4616_build_done.push_back(events_JOIN_INNER_TD_4616[0]);
    for (int i(0); i < hpTimes_join; ++i) {
        krnl_JOIN_INNER_TD_4616[i].run(0, &(events_grp_JOIN_INNER_TD_4616), &(events_JOIN_INNER_TD_4616[1][i]));
    }
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_4616_probe_done;
    events_grp_JOIN_INNER_TD_4616_probe_done.push_back(events_JOIN_INNER_TD_4616[1]);
    for (int i(0); i < hpTimes_join; ++i) {
        trans_JOIN_INNER_TD_4616_out.add(&(tbl_JOIN_INNER_TD_4616_output_partition_array[i]));
    }
    trans_JOIN_INNER_TD_4616_out.dev2host(0, &(events_JOIN_INNER_TD_4616[1]), &(events_d2h_rd_JOIN_INNER_TD_4616[0]));
    q_h.flush();
    q_h.finish();
    tbl_JOIN_INNER_TD_4616_output.setNumRow((cfg_JOIN_INNER_TD_4616_cmds_probe.meta_out)->getColLen());
    tbl_JOIN_INNER_TD_4616_output.colToTable();
    SW_Consolidated_JOIN_INNER_TD_4616_output(tbl_JOIN_INNER_TD_3653_consolidated_output, tbl_JOIN_LEFTANTI_TD_5733_consolidated_output, tbl_JOIN_INNER_TD_4616_output, tbl_JOIN_INNER_TD_4616_consolidated_output);
    gettimeofday(&tv_r_JOIN_INNER_4_244_e, 0);

    struct timeval tv_r_Filter_3_693_s, tv_r_Filter_3_693_e;
    gettimeofday(&tv_r_Filter_3_693_s, 0);
    SW_Filter_TD_3321(tbl_SerializeFromObject_TD_5180_input, tbl_Filter_TD_3321_output);
    gettimeofday(&tv_r_Filter_3_693_e, 0);

    struct timeval tv_r_JOIN_INNER_2_917_s, tv_r_JOIN_INNER_2_917_e;
    gettimeofday(&tv_r_JOIN_INNER_2_917_s, 0);
    prev_events_grp_JOIN_INNER_TD_2684.push_back(events_h2d_wr_JOIN_INNER_TD_4616[0]);
    tbl_JOIN_INNER_TD_4616_consolidated_output.tableToCol();
    tbl_Filter_TD_3321_output.tableToCol();
    krnl_JOIN_INNER_TD_2684_build.updateMeta(tbl_JOIN_INNER_TD_4616_consolidated_output.getNumRow(), 3);
    krnl_JOIN_INNER_TD_2684_probe.updateMeta(tbl_Filter_TD_3321_output.getNumRow(), 3);
    trans_JOIN_INNER_TD_2684.add(&(tbl_Filter_TD_3321_output));
    trans_JOIN_INNER_TD_2684.host2dev(0, &(prev_events_grp_JOIN_INNER_TD_2684), &(events_h2d_wr_JOIN_INNER_TD_2684[0]));
    events_grp_JOIN_INNER_TD_2684.push_back(events_h2d_wr_JOIN_INNER_TD_2684[0]);
    events_grp_JOIN_INNER_TD_2684.insert(std::end(events_grp_JOIN_INNER_TD_2684), std::begin(events_JOIN_INNER_TD_4616[0]), std::end(events_JOIN_INNER_TD_4616[0]));
    events_grp_JOIN_INNER_TD_2684.insert(std::end(events_grp_JOIN_INNER_TD_2684), std::begin(events_JOIN_INNER_TD_4616[1]), std::end(events_JOIN_INNER_TD_4616[1]));
    krnl_JOIN_INNER_TD_2684_part_right.run(0, &(events_grp_JOIN_INNER_TD_2684), &(events_JOIN_INNER_TD_2684[0][0]));
    for (int i(0); i < hpTimes_join; ++i) {
        krnl_JOIN_INNER_TD_2684[i].run(0, &(events_grp_JOIN_INNER_TD_2684), &(events_JOIN_INNER_TD_2684[1][i]));
    }
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_2684_build_done;
    events_grp_JOIN_INNER_TD_2684_build_done.push_back(events_JOIN_INNER_TD_2684[0]);
    krnl_JOIN_INNER_TD_2684_part_right.run(0, &(events_grp_JOIN_INNER_TD_2684), &(events_JOIN_INNER_TD_2684[0][0]));
    for (int i(0); i < hpTimes_join; ++i) {
        krnl_JOIN_INNER_TD_2684[i].run(0, &(events_grp_JOIN_INNER_TD_2684), &(events_JOIN_INNER_TD_2684[1][i]));
    }
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_2684_probe_done;
    events_grp_JOIN_INNER_TD_2684_probe_done.push_back(events_JOIN_INNER_TD_2684[1]);
    for (int i(0); i < hpTimes_join; ++i) {
        trans_JOIN_INNER_TD_2684_out.add(&(tbl_JOIN_INNER_TD_2684_output_partition_array[i]));
    }
    trans_JOIN_INNER_TD_2684_out.dev2host(0, &(events_JOIN_INNER_TD_2684[1]), &(events_d2h_rd_JOIN_INNER_TD_2684[0]));
    q_h.flush();
    q_h.finish();
    tbl_JOIN_INNER_TD_2684_output.setNumRow((cfg_JOIN_INNER_TD_2684_cmds_probe.meta_out)->getColLen());
    tbl_JOIN_INNER_TD_2684_output.colToTable();
    SW_Consolidated_JOIN_INNER_TD_2684_output(tbl_JOIN_INNER_TD_4616_consolidated_output, tbl_Filter_TD_3321_output, tbl_JOIN_INNER_TD_2684_output, tbl_JOIN_INNER_TD_2684_consolidated_output);
    gettimeofday(&tv_r_JOIN_INNER_2_917_e, 0);

    struct timeval tv_r_Aggregate_1_679_s, tv_r_Aggregate_1_679_e;
    gettimeofday(&tv_r_Aggregate_1_679_s, 0);
    SW_Aggregate_TD_1648(tbl_JOIN_INNER_TD_2684_consolidated_output_partition_array, tbl_SerializeFromObject_TD_5180_input, tbl_SerializeFromObject_TD_5188_input, tbl_SerializeFromObject_TD_4935_input, tbl_Aggregate_TD_1648_output, hpTimes_join);
    gettimeofday(&tv_r_Aggregate_1_679_e, 0);

    struct timeval tv_r_Sort_0_461_s, tv_r_Sort_0_461_e;
    gettimeofday(&tv_r_Sort_0_461_s, 0);
    SW_Sort_TD_0482(tbl_Aggregate_TD_1648_output, tbl_Sort_TD_0482_output);
    gettimeofday(&tv_r_Sort_0_461_e, 0);

std::cout << "CPU TIME: " << (tvdiff(&tv_r_Filter_8_421_s, &tv_r_Filter_8_421_e) + 
tvdiff(&tv_r_Filter_7_107_s, &tv_r_Filter_7_107_e) + 
tvdiff(&tv_r_Filter_3_944_s, &tv_r_Filter_3_944_e) + 
tvdiff(&tv_r_Filter_3_693_s, &tv_r_Filter_3_693_e) + 
tvdiff(&tv_r_Aggregate_1_679_s, &tv_r_Aggregate_1_679_e) + 
tvdiff(&tv_r_Sort_0_461_s, &tv_r_Sort_0_461_e) + 
1) / 1000.0 << std::endl;

std::cout << "FPGA TIME: " << (tvdiff(&tv_r_JOIN_LEFTSEMI_7_884_s, &tv_r_JOIN_LEFTSEMI_7_884_e) + 
tvdiff(&tv_r_JOIN_LEFTSEMI_6_559_s, &tv_r_JOIN_LEFTSEMI_6_559_e) + 
tvdiff(&tv_r_JOIN_LEFTANTI_5_798_s, &tv_r_JOIN_LEFTANTI_5_798_e) + 
tvdiff(&tv_r_JOIN_INNER_3_711_s, &tv_r_JOIN_INNER_3_711_e) + 
tvdiff(&tv_r_JOIN_INNER_4_244_s, &tv_r_JOIN_INNER_4_244_e) + 
tvdiff(&tv_r_JOIN_INNER_2_917_s, &tv_r_JOIN_INNER_2_917_e) + 
1) / 1000.0 << std::endl;

    gettimeofday(&tv_r_e, 0); 
    // **************************** Print Execution Time ************************** // 
    std::cout << std::endl; 
    std::cout << "Filter_8: " << tvdiff(&tv_r_Filter_8_421_s, &tv_r_Filter_8_421_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer((l_receiptdate#89 > l_commitdate#88)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_9935_input.getNumRow() << " -> tbl_SerializeFromObject_TD_9935_input" 
     << std::endl << "      #COL: 4: " << "ListBuffer(l_orderkey#77, l_suppkey#79, l_receiptdate#89, l_commitdate#88)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_8644_output.getNumRow() << " -> tbl_Filter_TD_8644_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(l_orderkey#77, l_suppkey#79)" 
     << std::endl; 

    std::cout << "Filter_7: " << tvdiff(&tv_r_Filter_7_107_s, &tv_r_Filter_7_107_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer((l_receiptdate#89 > l_commitdate#88)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_9935_input.getNumRow() << " -> tbl_SerializeFromObject_TD_9935_input" 
     << std::endl << "      #COL: 4: " << "ListBuffer(l_orderkey#77, l_suppkey#79, l_receiptdate#89, l_commitdate#88)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_7203_output.getNumRow() << " -> tbl_Filter_TD_7203_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(l_orderkey#77#2010, l_suppkey#79#2012)" 
     << std::endl; 

    std::cout << "JOIN_LEFTSEMI_7: " << tvdiff(&tv_r_JOIN_LEFTSEMI_7_884_s, &tv_r_JOIN_LEFTSEMI_7_884_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer(((l_orderkey#77#1994 = l_orderkey#77) AND NOT (l_suppkey#79#1996 = l_suppkey#79))) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_8644_output.getNumRow() << " -> tbl_Filter_TD_8644_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(l_orderkey#77, l_suppkey#79)" 
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_8225_input.getNumRow() << " -> tbl_SerializeFromObject_TD_8225_input" 
     << std::endl << "      #COL: 2: " << "ListBuffer(l_orderkey#77#1994, l_suppkey#79#1996)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_LEFTSEMI_TD_7346_consolidated_output.getNumRow() << " -> tbl_JOIN_LEFTSEMI_TD_7346_consolidated_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(l_orderkey#77, l_suppkey#79)" 
     << std::endl; 

    std::cout << "JOIN_LEFTSEMI_6: " << tvdiff(&tv_r_JOIN_LEFTSEMI_6_559_s, &tv_r_JOIN_LEFTSEMI_6_559_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer(((l_orderkey#77#2010 = l_orderkey#77) AND NOT (l_suppkey#79#2012 = l_suppkey#79))) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_LEFTSEMI_TD_7346_consolidated_output.getNumRow() << " -> tbl_JOIN_LEFTSEMI_TD_7346_consolidated_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(l_orderkey#77, l_suppkey#79)" 
     << std::endl << "      #ROW: " << tbl_Filter_TD_7203_output.getNumRow() << " -> tbl_Filter_TD_7203_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(l_orderkey#77#2010, l_suppkey#79#2012)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_LEFTSEMI_TD_6827_consolidated_output.getNumRow() << " -> tbl_JOIN_LEFTSEMI_TD_6827_consolidated_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(l_orderkey#77, l_suppkey#79)" 
     << std::endl; 

    std::cout << "Filter_3: " << tvdiff(&tv_r_Filter_3_944_s, &tv_r_Filter_3_944_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer((isnotnull(n_name#145) AND (n_name#145 = JAPAN))) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_4935_input.getNumRow() << " -> tbl_SerializeFromObject_TD_4935_input" 
     << std::endl << "      #COL: 2: " << "ListBuffer(n_nationkey#144, n_name#145)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_3611_output.getNumRow() << " -> tbl_Filter_TD_3611_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(n_nationkey#144)" 
     << std::endl; 

    std::cout << "JOIN_LEFTANTI_5: " << tvdiff(&tv_r_JOIN_LEFTANTI_5_798_s, &tv_r_JOIN_LEFTANTI_5_798_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer(((l_orderkey#77#2010 = l_orderkey#77) AND NOT (l_suppkey#79#2012 = l_suppkey#79))) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_7203_output.getNumRow() << " -> tbl_Filter_TD_7203_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(l_orderkey#77#2010, l_suppkey#79#2012)" 
     << std::endl << "      #ROW: " << tbl_JOIN_LEFTSEMI_TD_6827_consolidated_output.getNumRow() << " -> tbl_JOIN_LEFTSEMI_TD_6827_consolidated_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(l_orderkey#77, l_suppkey#79)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_LEFTANTI_TD_5733_consolidated_output.getNumRow() << " -> tbl_JOIN_LEFTANTI_TD_5733_consolidated_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(l_orderkey#77, l_suppkey#79)" 
     << std::endl; 

    std::cout << "JOIN_INNER_3: " << tvdiff(&tv_r_JOIN_INNER_3_711_s, &tv_r_JOIN_INNER_3_711_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer((s_nationkey#345 = n_nationkey#144)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_5188_input_stringRowIDSubstitute.getNumRow() << " -> tbl_SerializeFromObject_TD_5188_input_stringRowIDSubstitute" 
     << std::endl << "      #COL: 3: " << "ListBuffer(s_suppkey#342, s_name#343, s_nationkey#345)" 
     << std::endl << "      #ROW: " << tbl_Filter_TD_3611_output.getNumRow() << " -> tbl_Filter_TD_3611_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(n_nationkey#144)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_3653_consolidated_output.getNumRow() << " -> tbl_JOIN_INNER_TD_3653_consolidated_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(s_suppkey#342, s_name#343)" 
     << std::endl; 

    std::cout << "JOIN_INNER_4: " << tvdiff(&tv_r_JOIN_INNER_4_244_s, &tv_r_JOIN_INNER_4_244_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer((s_suppkey#342 = l_suppkey#79)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_3653_consolidated_output.getNumRow() << " -> tbl_JOIN_INNER_TD_3653_consolidated_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(s_suppkey#342, s_name#343)" 
     << std::endl << "      #ROW: " << tbl_JOIN_LEFTANTI_TD_5733_consolidated_output.getNumRow() << " -> tbl_JOIN_LEFTANTI_TD_5733_consolidated_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(l_orderkey#77, l_suppkey#79)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_4616_consolidated_output.getNumRow() << " -> tbl_JOIN_INNER_TD_4616_consolidated_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(s_name#343, l_orderkey#77)" 
     << std::endl; 

    std::cout << "Filter_3: " << tvdiff(&tv_r_Filter_3_693_s, &tv_r_Filter_3_693_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer((o_orderstatus#209 = 70)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_5180_input.getNumRow() << " -> tbl_SerializeFromObject_TD_5180_input" 
     << std::endl << "      #COL: 2: " << "ListBuffer(o_orderkey#207, o_orderstatus#209)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_3321_output.getNumRow() << " -> tbl_Filter_TD_3321_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(o_orderkey#207)" 
     << std::endl; 

    std::cout << "JOIN_INNER_2: " << tvdiff(&tv_r_JOIN_INNER_2_917_s, &tv_r_JOIN_INNER_2_917_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer((o_orderkey#207 = l_orderkey#77)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_3321_output.getNumRow() << " -> tbl_Filter_TD_3321_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(o_orderkey#207)" 
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_4616_consolidated_output.getNumRow() << " -> tbl_JOIN_INNER_TD_4616_consolidated_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(s_name#343, l_orderkey#77)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_2684_consolidated_output.getNumRow() << " -> tbl_JOIN_INNER_TD_2684_consolidated_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(s_name#343)" 
     << std::endl; 

    std::cout << "Aggregate_1: " << tvdiff(&tv_r_Aggregate_1_679_s, &tv_r_Aggregate_1_679_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(s_name#343, count(1) AS numwait#1897L) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_2684_consolidated_output.getNumRow() << " -> tbl_JOIN_INNER_TD_2684_consolidated_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(s_name#343)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_1648_output.getNumRow() << " -> tbl_Aggregate_TD_1648_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(s_name#343, numwait#1897L)" 
     << std::endl; 

    std::cout << "Sort_0: " << tvdiff(&tv_r_Sort_0_461_s, &tv_r_Sort_0_461_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(numwait#1897L DESC NULLS LAST, s_name#343 ASC NULLS FIRST) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_1648_output.getNumRow() << " -> tbl_Aggregate_TD_1648_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(s_name#343, numwait#1897L)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Sort_TD_0482_output.getNumRow() << " -> tbl_Sort_TD_0482_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(s_name#343, numwait#1897L)" 
     << std::endl; 

    std::cout << std::endl << " Total execution time: " << tvdiff(&tv_r_s, &tv_r_e) / 1000 << " ms"; 

    std::cout << std::endl << " Spark elapsed time: " << 69.02752 * 1000 << "ms" << std::endl; 
    return 0; 
}
