// number of overlays (w/o fusion): 3 
// number of overlays (w/ fusion): 3 
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

#include "cfgFunc_q10.hpp" 
#include "q10.hpp" 

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
    std::cout << "NOTE:running query #10\n."; 
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
    Table tbl_Sort_TD_066_output("tbl_Sort_TD_066_output", 183000000, 5, "");
    tbl_Sort_TD_066_output.allocateHost();
    Table tbl_Aggregate_TD_1486_output("tbl_Aggregate_TD_1486_output", 183000000, 5, "");
    tbl_Aggregate_TD_1486_output.allocateHost();
    Table tbl_JOIN_INNER_TD_2136_output("tbl_JOIN_INNER_TD_2136_output", 114347, 4, "");
    tbl_JOIN_INNER_TD_2136_output.allocateHost(1.2, hpTimes_join);
    Table tbl_JOIN_INNER_TD_2136_consolidated_output("tbl_JOIN_INNER_TD_2136_consolidated_output", 114347, 6, "");
    tbl_JOIN_INNER_TD_2136_consolidated_output.selectOverlayVersion(1);
    tbl_JOIN_INNER_TD_2136_consolidated_output.allocateHost();
    Table tbl_JOIN_INNER_TD_3858_output("tbl_JOIN_INNER_TD_3858_output", 114347, 4, "");
    tbl_JOIN_INNER_TD_3858_output.allocateHost(1.2, hpTimes_join);
    Table tbl_JOIN_INNER_TD_3858_consolidated_output("tbl_JOIN_INNER_TD_3858_consolidated_output", 114347, 6, "");
    tbl_JOIN_INNER_TD_3858_consolidated_output.selectOverlayVersion(1);
    tbl_JOIN_INNER_TD_3858_consolidated_output.allocateHost();
    Table tbl_SerializeFromObject_TD_338_input_stringRowIDSubstitute;
    tbl_SerializeFromObject_TD_338_input_stringRowIDSubstitute = Table("nation", nation_n, 2, in_dir);
    tbl_SerializeFromObject_TD_338_input_stringRowIDSubstitute.addCol("n_nationkey", 4);
    tbl_SerializeFromObject_TD_338_input_stringRowIDSubstitute.addCol("n_name", 4, 1, 0);
    tbl_SerializeFromObject_TD_338_input_stringRowIDSubstitute.allocateHost();
    tbl_SerializeFromObject_TD_338_input_stringRowIDSubstitute.loadHost();
    Table tbl_SerializeFromObject_TD_338_input_stringRowIDSubstitute_partition("tbl_SerializeFromObject_TD_338_input_stringRowIDSubstitute_partition", nation_n, 2, "");
    tbl_SerializeFromObject_TD_338_input_stringRowIDSubstitute_partition.allocateHost(1.2, hpTimes_join);
    tbl_SerializeFromObject_TD_338_input_stringRowIDSubstitute_partition.allocateDevBuffer(context_h, 32);
    Table tbl_SerializeFromObject_TD_338_input_stringRowIDSubstitute_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_SerializeFromObject_TD_338_input_stringRowIDSubstitute_partition_array[i] = tbl_SerializeFromObject_TD_338_input_stringRowIDSubstitute_partition.createSubTable(i);
    }
    Table tbl_SerializeFromObject_TD_338_input;
    tbl_SerializeFromObject_TD_338_input = Table("nation", nation_n, 2, in_dir);
    tbl_SerializeFromObject_TD_338_input.addCol("n_nationkey", 4);
    tbl_SerializeFromObject_TD_338_input.addCol("n_name", TPCH_READ_NATION_LEN+1);
    tbl_SerializeFromObject_TD_338_input.allocateHost();
    tbl_SerializeFromObject_TD_338_input.loadHost();
    Table tbl_JOIN_INNER_TD_4808_output("tbl_JOIN_INNER_TD_4808_output", 57111, 4, "");
    tbl_JOIN_INNER_TD_4808_output.allocateHost(1.2, hpTimes_join);
    Table tbl_JOIN_INNER_TD_4808_consolidated_output("tbl_JOIN_INNER_TD_4808_consolidated_output", 57111, 5, "");
    tbl_JOIN_INNER_TD_4808_consolidated_output.selectOverlayVersion(1);
    tbl_JOIN_INNER_TD_4808_consolidated_output.allocateHost();
    Table tbl_Filter_TD_4404_output("tbl_Filter_TD_4404_output", 183000000, 3, "");
    tbl_Filter_TD_4404_output.selectOverlayVersion(1);
    tbl_Filter_TD_4404_output.allocateHost();
    Table tbl_Filter_TD_4404_output_partition("tbl_Filter_TD_4404_output_partition", 183000000, 3, "");
    tbl_Filter_TD_4404_output_partition.allocateHost(1.2, hpTimes_join);
    tbl_Filter_TD_4404_output_partition.allocateDevBuffer(context_h, 32);
    Table tbl_Filter_TD_4404_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_Filter_TD_4404_output_partition_array[i] = tbl_Filter_TD_4404_output_partition.createSubTable(i);
    }
    Table tbl_SerializeFromObject_TD_5609_input_stringRowIDSubstitute;
    tbl_SerializeFromObject_TD_5609_input_stringRowIDSubstitute = Table("customer", customer_n, 4, in_dir);
    tbl_SerializeFromObject_TD_5609_input_stringRowIDSubstitute.addCol("c_custkey", 4);
    tbl_SerializeFromObject_TD_5609_input_stringRowIDSubstitute.addCol("c_name", 4, 1, 0);
    tbl_SerializeFromObject_TD_5609_input_stringRowIDSubstitute.addCol("c_nationkey", 4);
    tbl_SerializeFromObject_TD_5609_input_stringRowIDSubstitute.addCol("c_acctbal", 4);
    tbl_SerializeFromObject_TD_5609_input_stringRowIDSubstitute.allocateHost();
    tbl_SerializeFromObject_TD_5609_input_stringRowIDSubstitute.loadHost();
    Table tbl_SerializeFromObject_TD_5609_input_stringRowIDSubstitute_partition("tbl_SerializeFromObject_TD_5609_input_stringRowIDSubstitute_partition", customer_n, 4, "");
    tbl_SerializeFromObject_TD_5609_input_stringRowIDSubstitute_partition.allocateHost(1.2, hpTimes_join);
    tbl_SerializeFromObject_TD_5609_input_stringRowIDSubstitute_partition.allocateDevBuffer(context_h, 32);
    Table tbl_SerializeFromObject_TD_5609_input_stringRowIDSubstitute_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_SerializeFromObject_TD_5609_input_stringRowIDSubstitute_partition_array[i] = tbl_SerializeFromObject_TD_5609_input_stringRowIDSubstitute_partition.createSubTable(i);
    }
    Table tbl_SerializeFromObject_TD_5609_input;
    tbl_SerializeFromObject_TD_5609_input = Table("customer", customer_n, 4, in_dir);
    tbl_SerializeFromObject_TD_5609_input.addCol("c_custkey", 4);
    tbl_SerializeFromObject_TD_5609_input.addCol("c_name", TPCH_READ_C_NAME_LEN+1);
    tbl_SerializeFromObject_TD_5609_input.addCol("c_nationkey", 4);
    tbl_SerializeFromObject_TD_5609_input.addCol("c_acctbal", 4);
    tbl_SerializeFromObject_TD_5609_input.allocateHost();
    tbl_SerializeFromObject_TD_5609_input.loadHost();
    Table tbl_Filter_TD_5436_output("tbl_Filter_TD_5436_output", 183000000, 2, "");
    tbl_Filter_TD_5436_output.selectOverlayVersion(1);
    tbl_Filter_TD_5436_output.allocateHost();
    Table tbl_Filter_TD_5436_output_partition("tbl_Filter_TD_5436_output_partition", 183000000, 2, "");
    tbl_Filter_TD_5436_output_partition.allocateHost(1.2, hpTimes_join);
    tbl_Filter_TD_5436_output_partition.allocateDevBuffer(context_h, 32);
    Table tbl_Filter_TD_5436_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_Filter_TD_5436_output_partition_array[i] = tbl_Filter_TD_5436_output_partition.createSubTable(i);
    }
    Table tbl_SerializeFromObject_TD_5746_input;
    tbl_SerializeFromObject_TD_5746_input = Table("lineitem", lineitem_n, 4, in_dir);
    tbl_SerializeFromObject_TD_5746_input.addCol("l_orderkey", 4);
    tbl_SerializeFromObject_TD_5746_input.addCol("l_extendedprice", 4);
    tbl_SerializeFromObject_TD_5746_input.addCol("l_discount", 4);
    tbl_SerializeFromObject_TD_5746_input.addCol("l_returnflag", 4);
    tbl_SerializeFromObject_TD_5746_input.allocateHost();
    tbl_SerializeFromObject_TD_5746_input.loadHost();
    Table tbl_SerializeFromObject_TD_6481_input;
    tbl_SerializeFromObject_TD_6481_input = Table("order", order_n, 3, in_dir);
    tbl_SerializeFromObject_TD_6481_input.addCol("o_orderkey", 4);
    tbl_SerializeFromObject_TD_6481_input.addCol("o_custkey", 4);
    tbl_SerializeFromObject_TD_6481_input.addCol("o_orderdate", 4);
    tbl_SerializeFromObject_TD_6481_input.allocateHost();
    tbl_SerializeFromObject_TD_6481_input.loadHost();
    // ********************** Allocate Device Buffer ******************** // 
    tbl_JOIN_INNER_TD_2136_output.allocateDevBuffer(context_h, 32);
    Table tbl_JOIN_INNER_TD_2136_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_JOIN_INNER_TD_2136_output_partition_array[i] = tbl_JOIN_INNER_TD_2136_output.createSubTable(i);
    }
    tbl_JOIN_INNER_TD_3858_output.allocateDevBuffer(context_h, 32);
    Table tbl_JOIN_INNER_TD_3858_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_JOIN_INNER_TD_3858_output_partition_array[i] = tbl_JOIN_INNER_TD_3858_output.createSubTable(i);
    }
    tbl_SerializeFromObject_TD_338_input_stringRowIDSubstitute.allocateDevBuffer(context_h, 33);
    tbl_JOIN_INNER_TD_4808_output.allocateDevBuffer(context_h, 32);
    Table tbl_JOIN_INNER_TD_4808_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_JOIN_INNER_TD_4808_output_partition_array[i] = tbl_JOIN_INNER_TD_4808_output.createSubTable(i);
    }
    tbl_Filter_TD_4404_output.allocateDevBuffer(context_h, 33);
    tbl_SerializeFromObject_TD_5609_input_stringRowIDSubstitute.allocateDevBuffer(context_h, 33);
    tbl_Filter_TD_5436_output.allocateDevBuffer(context_h, 33);
    // ****************************** Config **************************** // 
    
    xf::database::gqe::KernelCommand krn_cmd_build_579 = xf::database::gqe::KernelCommand();
    xf::database::gqe::KernelCommand krn_cmd_probe_579 = xf::database::gqe::KernelCommand();
    cfgCmd cfg_JOIN_INNER_TD_2136_cmds_build;
    cfg_JOIN_INNER_TD_2136_cmds_build.setup(1);
    cfg_JOIN_INNER_TD_2136_cmds_build.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_2136_gqe_join_build(krn_cmd_build_579);
    cfg_JOIN_INNER_TD_2136_cmds_build.cmd = krn_cmd_build_579.getConfigBits();
    get_meta_info_JOIN_INNER_TD_2136_gqe_join_build(cfg_JOIN_INNER_TD_2136_cmds_build.meta_in, tbl_SerializeFromObject_TD_338_input_stringRowIDSubstitute.nrow, 3);
    get_meta_info_JOIN_INNER_TD_2136_gqe_join_build(cfg_JOIN_INNER_TD_2136_cmds_build.meta_out, tbl_JOIN_INNER_TD_2136_output.nrow, 4);
    // cfg_JOIN_INNER_TD_2136_cmds_build.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_2136_cmds_probe;
    cfg_JOIN_INNER_TD_2136_cmds_probe.setup(1);
    cfg_JOIN_INNER_TD_2136_cmds_probe.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_2136_gqe_join_probe(krn_cmd_probe_579);
    cfg_JOIN_INNER_TD_2136_cmds_probe.cmd = krn_cmd_probe_579.getConfigBits();
    get_meta_info_JOIN_INNER_TD_2136_gqe_join_probe(cfg_JOIN_INNER_TD_2136_cmds_probe.meta_in, tbl_JOIN_INNER_TD_3858_consolidated_output.nrow, 3);
    get_meta_info_JOIN_INNER_TD_2136_gqe_join_probe(cfg_JOIN_INNER_TD_2136_cmds_probe.meta_out, tbl_JOIN_INNER_TD_2136_output.nrow, 3);
    // cfg_JOIN_INNER_TD_2136_cmds_probe.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_2136_cmds_part;
    cfg_JOIN_INNER_TD_2136_cmds_part.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_2136_gqe_join_part (cfg_JOIN_INNER_TD_2136_cmds_part.cmd);
    cfg_JOIN_INNER_TD_2136_cmds_part.allocateDevBuffer(context_h, 32);
    cfg_JOIN_INNER_TD_2136_cmds_build.input_key_columns = {0, -1, -1};
    cfg_JOIN_INNER_TD_2136_cmds_probe.input_key_columns = {2, -1, -1};
    cfg_JOIN_INNER_TD_2136_cmds_build.output_key_columns = {0, 1, 2, 3};
    cfg_JOIN_INNER_TD_2136_cmds_probe.output_key_columns = {0, 1, 2, 3};
    
    xf::database::gqe::KernelCommand krn_cmd_build_437 = xf::database::gqe::KernelCommand();
    xf::database::gqe::KernelCommand krn_cmd_probe_437 = xf::database::gqe::KernelCommand();
    cfgCmd cfg_JOIN_INNER_TD_3858_cmds_build;
    cfg_JOIN_INNER_TD_3858_cmds_build.setup(1);
    cfg_JOIN_INNER_TD_3858_cmds_build.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_3858_gqe_join_build(krn_cmd_build_437);
    cfg_JOIN_INNER_TD_3858_cmds_build.cmd = krn_cmd_build_437.getConfigBits();
    get_meta_info_JOIN_INNER_TD_3858_gqe_join_build(cfg_JOIN_INNER_TD_3858_cmds_build.meta_in, tbl_Filter_TD_4404_output.nrow, 3);
    get_meta_info_JOIN_INNER_TD_3858_gqe_join_build(cfg_JOIN_INNER_TD_3858_cmds_build.meta_out, tbl_JOIN_INNER_TD_3858_output.nrow, 4);
    // cfg_JOIN_INNER_TD_3858_cmds_build.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_3858_cmds_probe;
    cfg_JOIN_INNER_TD_3858_cmds_probe.setup(1);
    cfg_JOIN_INNER_TD_3858_cmds_probe.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_3858_gqe_join_probe(krn_cmd_probe_437);
    cfg_JOIN_INNER_TD_3858_cmds_probe.cmd = krn_cmd_probe_437.getConfigBits();
    get_meta_info_JOIN_INNER_TD_3858_gqe_join_probe(cfg_JOIN_INNER_TD_3858_cmds_probe.meta_in, tbl_JOIN_INNER_TD_4808_consolidated_output.nrow, 3);
    get_meta_info_JOIN_INNER_TD_3858_gqe_join_probe(cfg_JOIN_INNER_TD_3858_cmds_probe.meta_out, tbl_JOIN_INNER_TD_3858_output.nrow, 3);
    // cfg_JOIN_INNER_TD_3858_cmds_probe.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_3858_cmds_part;
    cfg_JOIN_INNER_TD_3858_cmds_part.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_3858_gqe_join_part (cfg_JOIN_INNER_TD_3858_cmds_part.cmd);
    cfg_JOIN_INNER_TD_3858_cmds_part.allocateDevBuffer(context_h, 32);
    cfg_JOIN_INNER_TD_3858_cmds_build.input_key_columns = {0, -1, -1};
    cfg_JOIN_INNER_TD_3858_cmds_probe.input_key_columns = {4, -1, -1};
    cfg_JOIN_INNER_TD_3858_cmds_build.output_key_columns = {0, 1, 2, 3};
    cfg_JOIN_INNER_TD_3858_cmds_probe.output_key_columns = {0, 1, 2, 3};
    
    xf::database::gqe::KernelCommand krn_cmd_build_275 = xf::database::gqe::KernelCommand();
    xf::database::gqe::KernelCommand krn_cmd_probe_275 = xf::database::gqe::KernelCommand();
    cfgCmd cfg_JOIN_INNER_TD_4808_cmds_build;
    cfg_JOIN_INNER_TD_4808_cmds_build.setup(1);
    cfg_JOIN_INNER_TD_4808_cmds_build.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_4808_gqe_join_build(krn_cmd_build_275);
    cfg_JOIN_INNER_TD_4808_cmds_build.cmd = krn_cmd_build_275.getConfigBits();
    get_meta_info_JOIN_INNER_TD_4808_gqe_join_build(cfg_JOIN_INNER_TD_4808_cmds_build.meta_in, tbl_Filter_TD_5436_output.nrow, 3);
    get_meta_info_JOIN_INNER_TD_4808_gqe_join_build(cfg_JOIN_INNER_TD_4808_cmds_build.meta_out, tbl_JOIN_INNER_TD_4808_output.nrow, 4);
    // cfg_JOIN_INNER_TD_4808_cmds_build.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_4808_cmds_probe;
    cfg_JOIN_INNER_TD_4808_cmds_probe.setup(1);
    cfg_JOIN_INNER_TD_4808_cmds_probe.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_4808_gqe_join_probe(krn_cmd_probe_275);
    cfg_JOIN_INNER_TD_4808_cmds_probe.cmd = krn_cmd_probe_275.getConfigBits();
    get_meta_info_JOIN_INNER_TD_4808_gqe_join_probe(cfg_JOIN_INNER_TD_4808_cmds_probe.meta_in, tbl_SerializeFromObject_TD_5609_input_stringRowIDSubstitute.nrow, 3);
    get_meta_info_JOIN_INNER_TD_4808_gqe_join_probe(cfg_JOIN_INNER_TD_4808_cmds_probe.meta_out, tbl_JOIN_INNER_TD_4808_output.nrow, 3);
    // cfg_JOIN_INNER_TD_4808_cmds_probe.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_4808_cmds_part;
    cfg_JOIN_INNER_TD_4808_cmds_part.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_4808_gqe_join_part (cfg_JOIN_INNER_TD_4808_cmds_part.cmd);
    cfg_JOIN_INNER_TD_4808_cmds_part.allocateDevBuffer(context_h, 32);
    cfg_JOIN_INNER_TD_4808_cmds_build.input_key_columns = {1, -1, -1};
    cfg_JOIN_INNER_TD_4808_cmds_probe.input_key_columns = {0, -1, -1};
    cfg_JOIN_INNER_TD_4808_cmds_build.output_key_columns = {0, 1, 2, 3};
    cfg_JOIN_INNER_TD_4808_cmds_probe.output_key_columns = {0, 1, 2, 3};
    // *************************** Kernel Setup ************************* // 
    bufferTmp buftmp_h(context_h, 1); 
    buftmp_h.initBuffer(q_h); 
    std::cout << std::endl; 
    krnlEngine krnl_JOIN_INNER_TD_2136_part_left;
    krnl_JOIN_INNER_TD_2136_part_left = krnlEngine(program_h, q_h, context_h, "gqePart");
    krnl_JOIN_INNER_TD_2136_part_left.setup_hp(512, 0, power_of_hpTimes_join, tbl_SerializeFromObject_TD_338_input_stringRowIDSubstitute, tbl_SerializeFromObject_TD_338_input_stringRowIDSubstitute_partition, cfg_JOIN_INNER_TD_2136_cmds_part);
    krnlEngine krnl_JOIN_INNER_TD_2136[hpTimes_join];
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_INNER_TD_2136[i] = krnlEngine(program_h, q_h, "gqeJoin");
    }
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_INNER_TD_2136[i].setup(tbl_SerializeFromObject_TD_338_input_stringRowIDSubstitute_partition_array[i], tbl_JOIN_INNER_TD_3858_consolidated_output_partition_array[i], tbl_JOIN_INNER_TD_2136_output_partition_array[i], , buftmp_h);
    }
    krnlEngine krnl_JOIN_INNER_TD_2136_build;
    krnl_JOIN_INNER_TD_2136_build = krnlEngine(program_h, q_h, context_h, "gqeKernel", 0);
    krnl_JOIN_INNER_TD_2136_build.setup(tbl_SerializeFromObject_TD_338_input_stringRowIDSubstitute, tbl_JOIN_INNER_TD_3858_consolidated_output, tbl_JOIN_INNER_TD_2136_output, cfg_JOIN_INNER_TD_2136_cmds_build, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_2136_probe;
    krnl_JOIN_INNER_TD_2136_probe = krnlEngine(program_h, q_h, context_h, "gqeKernel", 1);
    krnl_JOIN_INNER_TD_2136_probe.setup(tbl_JOIN_INNER_TD_3858_consolidated_output, tbl_SerializeFromObject_TD_338_input_stringRowIDSubstitute, tbl_JOIN_INNER_TD_2136_output, cfg_JOIN_INNER_TD_2136_cmds_probe, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_3858_part_left;
    krnl_JOIN_INNER_TD_3858_part_left = krnlEngine(program_h, q_h, context_h, "gqePart");
    krnl_JOIN_INNER_TD_3858_part_left.setup_hp(512, 0, power_of_hpTimes_join, tbl_Filter_TD_4404_output, tbl_Filter_TD_4404_output_partition, cfg_JOIN_INNER_TD_3858_cmds_part);
    krnlEngine krnl_JOIN_INNER_TD_3858[hpTimes_join];
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_INNER_TD_3858[i] = krnlEngine(program_h, q_h, "gqeJoin");
    }
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_INNER_TD_3858[i].setup(tbl_Filter_TD_4404_output_partition_array[i], tbl_JOIN_INNER_TD_4808_consolidated_output_partition_array[i], tbl_JOIN_INNER_TD_3858_output_partition_array[i], , buftmp_h);
    }
    krnlEngine krnl_JOIN_INNER_TD_3858_build;
    krnl_JOIN_INNER_TD_3858_build = krnlEngine(program_h, q_h, context_h, "gqeKernel", 0);
    krnl_JOIN_INNER_TD_3858_build.setup(tbl_Filter_TD_4404_output, tbl_JOIN_INNER_TD_4808_consolidated_output, tbl_JOIN_INNER_TD_3858_output, cfg_JOIN_INNER_TD_3858_cmds_build, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_3858_probe;
    krnl_JOIN_INNER_TD_3858_probe = krnlEngine(program_h, q_h, context_h, "gqeKernel", 1);
    krnl_JOIN_INNER_TD_3858_probe.setup(tbl_JOIN_INNER_TD_4808_consolidated_output, tbl_Filter_TD_4404_output, tbl_JOIN_INNER_TD_3858_output, cfg_JOIN_INNER_TD_3858_cmds_probe, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_4808_part_left;
    krnl_JOIN_INNER_TD_4808_part_left = krnlEngine(program_h, q_h, context_h, "gqePart");
    krnl_JOIN_INNER_TD_4808_part_left.setup_hp(512, 0, power_of_hpTimes_join, tbl_Filter_TD_5436_output, tbl_Filter_TD_5436_output_partition, cfg_JOIN_INNER_TD_4808_cmds_part);
    krnlEngine krnl_JOIN_INNER_TD_4808_part_right;
    krnl_JOIN_INNER_TD_4808_part_right = krnlEngine(program_h, q_h, context_h, "gqePart");
    krnl_JOIN_INNER_TD_4808_part_right.setup_hp(512, 1, power_of_hpTimes_join, tbl_SerializeFromObject_TD_5609_input_stringRowIDSubstitute, tbl_SerializeFromObject_TD_5609_input_stringRowIDSubstitute_partition, cfg_JOIN_INNER_TD_4808_cmds_part);
    krnlEngine krnl_JOIN_INNER_TD_4808[hpTimes_join];
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_INNER_TD_4808[i] = krnlEngine(program_h, q_h, "gqeJoin");
    }
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_INNER_TD_4808[i].setup(tbl_Filter_TD_5436_output_partition_array[i], tbl_SerializeFromObject_TD_5609_input_stringRowIDSubstitute_partition_array[i], tbl_JOIN_INNER_TD_4808_output_partition_array[i], , buftmp_h);
    }
    krnlEngine krnl_JOIN_INNER_TD_4808_build;
    krnl_JOIN_INNER_TD_4808_build = krnlEngine(program_h, q_h, context_h, "gqeKernel", 0);
    krnl_JOIN_INNER_TD_4808_build.setup(tbl_Filter_TD_5436_output, tbl_SerializeFromObject_TD_5609_input_stringRowIDSubstitute, tbl_JOIN_INNER_TD_4808_output, cfg_JOIN_INNER_TD_4808_cmds_build, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_4808_probe;
    krnl_JOIN_INNER_TD_4808_probe = krnlEngine(program_h, q_h, context_h, "gqeKernel", 1);
    krnl_JOIN_INNER_TD_4808_probe.setup(tbl_SerializeFromObject_TD_5609_input_stringRowIDSubstitute, tbl_Filter_TD_5436_output, tbl_JOIN_INNER_TD_4808_output, cfg_JOIN_INNER_TD_4808_cmds_probe, buftmp_h);
    // ************************** Transfer Engine *********************** // 
    transEngine trans_JOIN_INNER_TD_2136;
    trans_JOIN_INNER_TD_2136.setq(q_h,1);
    trans_JOIN_INNER_TD_2136.add(&(cfg_JOIN_INNER_TD_2136_cmds_part));
    trans_JOIN_INNER_TD_2136.add(&(cfg_JOIN_INNER_TD_2136_cmds_build));
    trans_JOIN_INNER_TD_2136.add(&(cfg_JOIN_INNER_TD_2136_cmds_probe));
    trans_JOIN_INNER_TD_2136.add(&(tbl_SerializeFromObject_TD_338_input_stringRowIDSubstitute));
    transEngine trans_JOIN_INNER_TD_2136_out;
    trans_JOIN_INNER_TD_2136_out.setq(q_h,1);
    q_h.finish();
    transEngine trans_JOIN_INNER_TD_3858;
    trans_JOIN_INNER_TD_3858.setq(q_h,1);
    trans_JOIN_INNER_TD_3858.add(&(cfg_JOIN_INNER_TD_3858_cmds_part));
    trans_JOIN_INNER_TD_3858.add(&(cfg_JOIN_INNER_TD_3858_cmds_build));
    trans_JOIN_INNER_TD_3858.add(&(cfg_JOIN_INNER_TD_3858_cmds_probe));
    transEngine trans_JOIN_INNER_TD_3858_out;
    trans_JOIN_INNER_TD_3858_out.setq(q_h,1);
    q_h.finish();
    transEngine trans_JOIN_INNER_TD_4808;
    trans_JOIN_INNER_TD_4808.setq(q_h,1);
    trans_JOIN_INNER_TD_4808.add(&(cfg_JOIN_INNER_TD_4808_cmds_part));
    trans_JOIN_INNER_TD_4808.add(&(cfg_JOIN_INNER_TD_4808_cmds_build));
    trans_JOIN_INNER_TD_4808.add(&(cfg_JOIN_INNER_TD_4808_cmds_probe));
    trans_JOIN_INNER_TD_4808.add(&(tbl_SerializeFromObject_TD_5609_input_stringRowIDSubstitute));
    transEngine trans_JOIN_INNER_TD_4808_out;
    trans_JOIN_INNER_TD_4808_out.setq(q_h,1);
    q_h.finish();
    // ****************************** Events **************************** // 
    std::vector<cl::Event> events_h2d_wr_JOIN_INNER_TD_2136;
    std::vector<cl::Event> events_d2h_rd_JOIN_INNER_TD_2136;
    std::vector<cl::Event> events_JOIN_INNER_TD_2136[2];
    events_h2d_wr_JOIN_INNER_TD_2136.resize(1);
    events_d2h_rd_JOIN_INNER_TD_2136.resize(1);
    events_JOIN_INNER_TD_2136[0].resize(1);
    events_JOIN_INNER_TD_2136[1].resize(hpTimes_join);
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_2136;
    std::vector<cl::Event> prev_events_grp_JOIN_INNER_TD_2136;
    std::vector<cl::Event> events_h2d_wr_JOIN_INNER_TD_3858;
    std::vector<cl::Event> events_d2h_rd_JOIN_INNER_TD_3858;
    std::vector<cl::Event> events_JOIN_INNER_TD_3858[2];
    events_h2d_wr_JOIN_INNER_TD_3858.resize(1);
    events_d2h_rd_JOIN_INNER_TD_3858.resize(1);
    events_JOIN_INNER_TD_3858[0].resize(1);
    events_JOIN_INNER_TD_3858[1].resize(hpTimes_join);
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_3858;
    std::vector<cl::Event> prev_events_grp_JOIN_INNER_TD_3858;
    std::vector<cl::Event> events_h2d_wr_JOIN_INNER_TD_4808;
    std::vector<cl::Event> events_d2h_rd_JOIN_INNER_TD_4808;
    std::vector<cl::Event> events_JOIN_INNER_TD_4808[2];
    events_h2d_wr_JOIN_INNER_TD_4808.resize(1);
    events_d2h_rd_JOIN_INNER_TD_4808.resize(1);
    events_JOIN_INNER_TD_4808[0].resize(2);
    events_JOIN_INNER_TD_4808[1].resize(hpTimes_join);
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_4808;
    std::vector<cl::Event> prev_events_grp_JOIN_INNER_TD_4808;
    // **************************** Operations ************************** // 
    struct timeval tv_r_s, tv_r_e; 
    gettimeofday(&tv_r_s, 0); 

    struct timeval tv_r_Filter_5_101_s, tv_r_Filter_5_101_e;
    gettimeofday(&tv_r_Filter_5_101_s, 0);
    SW_Filter_TD_5436(tbl_SerializeFromObject_TD_6481_input, tbl_Filter_TD_5436_output);
    gettimeofday(&tv_r_Filter_5_101_e, 0);

    struct timeval tv_r_Filter_4_31_s, tv_r_Filter_4_31_e;
    gettimeofday(&tv_r_Filter_4_31_s, 0);
    SW_Filter_TD_4404(tbl_SerializeFromObject_TD_5746_input, tbl_Filter_TD_4404_output);
    gettimeofday(&tv_r_Filter_4_31_e, 0);

    struct timeval tv_r_JOIN_INNER_4_676_s, tv_r_JOIN_INNER_4_676_e;
    gettimeofday(&tv_r_JOIN_INNER_4_676_s, 0);
    tbl_Filter_TD_5436_output.tableToCol();
    tbl_SerializeFromObject_TD_5609_input_stringRowIDSubstitute.tableToCol();
    krnl_JOIN_INNER_TD_4808_build.updateMeta(tbl_Filter_TD_5436_output.getNumRow(), 3);
    krnl_JOIN_INNER_TD_4808_probe.updateMeta(tbl_SerializeFromObject_TD_5609_input_stringRowIDSubstitute.getNumRow(), 3);
    trans_JOIN_INNER_TD_4808.add(&(tbl_Filter_TD_5436_output));
    trans_JOIN_INNER_TD_4808.host2dev(0, &(prev_events_grp_JOIN_INNER_TD_4808), &(events_h2d_wr_JOIN_INNER_TD_4808[0]));
    events_grp_JOIN_INNER_TD_4808.push_back(events_h2d_wr_JOIN_INNER_TD_4808[0]);
    krnl_JOIN_INNER_TD_4808_part_left.run(0, &(events_grp_JOIN_INNER_TD_4808), &(events_JOIN_INNER_TD_4808[0][0]));
    krnl_JOIN_INNER_TD_4808_part_right.run(0, &(events_grp_JOIN_INNER_TD_4808), &(events_JOIN_INNER_TD_4808[0][1]));
    for (int i(0); i < hpTimes_join; ++i) {
        krnl_JOIN_INNER_TD_4808[i].run(0, &(events_JOIN_INNER_TD_4808[0]), &(events_JOIN_INNER_TD_4808[1][i]));
    }
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_4808_build_done;
    events_grp_JOIN_INNER_TD_4808_build_done.push_back(events_JOIN_INNER_TD_4808[0]);
    krnl_JOIN_INNER_TD_4808_part_left.run(0, &(events_grp_JOIN_INNER_TD_4808), &(events_JOIN_INNER_TD_4808[0][0]));
    krnl_JOIN_INNER_TD_4808_part_right.run(0, &(events_grp_JOIN_INNER_TD_4808), &(events_JOIN_INNER_TD_4808[0][1]));
    for (int i(0); i < hpTimes_join; ++i) {
        krnl_JOIN_INNER_TD_4808[i].run(0, &(events_JOIN_INNER_TD_4808[0]), &(events_JOIN_INNER_TD_4808[1][i]));
    }
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_4808_probe_done;
    events_grp_JOIN_INNER_TD_4808_probe_done.push_back(events_JOIN_INNER_TD_4808[1]);
    for (int i(0); i < hpTimes_join; ++i) {
        trans_JOIN_INNER_TD_4808_out.add(&(tbl_JOIN_INNER_TD_4808_output_partition_array[i]));
    }
    trans_JOIN_INNER_TD_4808_out.dev2host(0, &(events_JOIN_INNER_TD_4808[1]), &(events_d2h_rd_JOIN_INNER_TD_4808[0]));
    q_h.flush();
    q_h.finish();
    tbl_JOIN_INNER_TD_4808_output.setNumRow((cfg_JOIN_INNER_TD_4808_cmds_probe.meta_out)->getColLen());
    tbl_JOIN_INNER_TD_4808_output.colToTable();
    SW_Consolidated_JOIN_INNER_TD_4808_output(tbl_Filter_TD_5436_output, tbl_SerializeFromObject_TD_5609_input_stringRowIDSubstitute, tbl_JOIN_INNER_TD_4808_output, tbl_JOIN_INNER_TD_4808_consolidated_output);
    gettimeofday(&tv_r_JOIN_INNER_4_676_e, 0);

    struct timeval tv_r_JOIN_INNER_3_362_s, tv_r_JOIN_INNER_3_362_e;
    gettimeofday(&tv_r_JOIN_INNER_3_362_s, 0);
    prev_events_grp_JOIN_INNER_TD_3858.push_back(events_h2d_wr_JOIN_INNER_TD_4808[0]);
    tbl_Filter_TD_4404_output.tableToCol();
    tbl_JOIN_INNER_TD_4808_consolidated_output.tableToCol();
    krnl_JOIN_INNER_TD_3858_build.updateMeta(tbl_Filter_TD_4404_output.getNumRow(), 3);
    krnl_JOIN_INNER_TD_3858_probe.updateMeta(tbl_JOIN_INNER_TD_4808_consolidated_output.getNumRow(), 3);
    trans_JOIN_INNER_TD_3858.add(&(tbl_Filter_TD_4404_output));
    trans_JOIN_INNER_TD_3858.host2dev(0, &(prev_events_grp_JOIN_INNER_TD_3858), &(events_h2d_wr_JOIN_INNER_TD_3858[0]));
    events_grp_JOIN_INNER_TD_3858.push_back(events_h2d_wr_JOIN_INNER_TD_3858[0]);
    events_grp_JOIN_INNER_TD_3858.insert(std::end(events_grp_JOIN_INNER_TD_3858), std::begin(events_JOIN_INNER_TD_4808[0]), std::end(events_JOIN_INNER_TD_4808[0]));
    events_grp_JOIN_INNER_TD_3858.insert(std::end(events_grp_JOIN_INNER_TD_3858), std::begin(events_JOIN_INNER_TD_4808[1]), std::end(events_JOIN_INNER_TD_4808[1]));
    krnl_JOIN_INNER_TD_3858_part_left.run(0, &(events_grp_JOIN_INNER_TD_3858), &(events_JOIN_INNER_TD_3858[0][0]));
    for (int i(0); i < hpTimes_join; ++i) {
        krnl_JOIN_INNER_TD_3858[i].run(0, &(events_JOIN_INNER_TD_3858[0]), &(events_JOIN_INNER_TD_3858[1][i]));
    }
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_3858_build_done;
    events_grp_JOIN_INNER_TD_3858_build_done.push_back(events_JOIN_INNER_TD_3858[0]);
    krnl_JOIN_INNER_TD_3858_part_left.run(0, &(events_grp_JOIN_INNER_TD_3858), &(events_JOIN_INNER_TD_3858[0][0]));
    for (int i(0); i < hpTimes_join; ++i) {
        krnl_JOIN_INNER_TD_3858[i].run(0, &(events_JOIN_INNER_TD_3858[0]), &(events_JOIN_INNER_TD_3858[1][i]));
    }
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_3858_probe_done;
    events_grp_JOIN_INNER_TD_3858_probe_done.push_back(events_JOIN_INNER_TD_3858[1]);
    for (int i(0); i < hpTimes_join; ++i) {
        trans_JOIN_INNER_TD_3858_out.add(&(tbl_JOIN_INNER_TD_3858_output_partition_array[i]));
    }
    trans_JOIN_INNER_TD_3858_out.dev2host(0, &(events_JOIN_INNER_TD_3858[1]), &(events_d2h_rd_JOIN_INNER_TD_3858[0]));
    q_h.flush();
    q_h.finish();
    tbl_JOIN_INNER_TD_3858_output.setNumRow((cfg_JOIN_INNER_TD_3858_cmds_probe.meta_out)->getColLen());
    tbl_JOIN_INNER_TD_3858_output.colToTable();
    SW_Consolidated_JOIN_INNER_TD_3858_output(tbl_Filter_TD_4404_output, tbl_JOIN_INNER_TD_4808_consolidated_output, tbl_JOIN_INNER_TD_3858_output, tbl_JOIN_INNER_TD_3858_consolidated_output);
    gettimeofday(&tv_r_JOIN_INNER_3_362_e, 0);

    struct timeval tv_r_JOIN_INNER_2_756_s, tv_r_JOIN_INNER_2_756_e;
    gettimeofday(&tv_r_JOIN_INNER_2_756_s, 0);
    prev_events_grp_JOIN_INNER_TD_2136.push_back(events_h2d_wr_JOIN_INNER_TD_3858[0]);
    tbl_SerializeFromObject_TD_338_input_stringRowIDSubstitute.tableToCol();
    tbl_JOIN_INNER_TD_3858_consolidated_output.tableToCol();
    krnl_JOIN_INNER_TD_2136_build.updateMeta(tbl_SerializeFromObject_TD_338_input_stringRowIDSubstitute.getNumRow(), 3);
    krnl_JOIN_INNER_TD_2136_probe.updateMeta(tbl_JOIN_INNER_TD_3858_consolidated_output.getNumRow(), 3);
    trans_JOIN_INNER_TD_2136.host2dev(0, &(prev_events_grp_JOIN_INNER_TD_2136), &(events_h2d_wr_JOIN_INNER_TD_2136[0]));
    events_grp_JOIN_INNER_TD_2136.push_back(events_h2d_wr_JOIN_INNER_TD_2136[0]);
    events_grp_JOIN_INNER_TD_2136.insert(std::end(events_grp_JOIN_INNER_TD_2136), std::begin(events_JOIN_INNER_TD_3858[0]), std::end(events_JOIN_INNER_TD_3858[0]));
    events_grp_JOIN_INNER_TD_2136.insert(std::end(events_grp_JOIN_INNER_TD_2136), std::begin(events_JOIN_INNER_TD_3858[1]), std::end(events_JOIN_INNER_TD_3858[1]));
    krnl_JOIN_INNER_TD_2136_part_left.run(0, &(events_grp_JOIN_INNER_TD_2136), &(events_JOIN_INNER_TD_2136[0][0]));
    for (int i(0); i < hpTimes_join; ++i) {
        krnl_JOIN_INNER_TD_2136[i].run(0, &(events_JOIN_INNER_TD_2136[0]), &(events_JOIN_INNER_TD_2136[1][i]));
    }
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_2136_build_done;
    events_grp_JOIN_INNER_TD_2136_build_done.push_back(events_JOIN_INNER_TD_2136[0]);
    krnl_JOIN_INNER_TD_2136_part_left.run(0, &(events_grp_JOIN_INNER_TD_2136), &(events_JOIN_INNER_TD_2136[0][0]));
    for (int i(0); i < hpTimes_join; ++i) {
        krnl_JOIN_INNER_TD_2136[i].run(0, &(events_JOIN_INNER_TD_2136[0]), &(events_JOIN_INNER_TD_2136[1][i]));
    }
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_2136_probe_done;
    events_grp_JOIN_INNER_TD_2136_probe_done.push_back(events_JOIN_INNER_TD_2136[1]);
    for (int i(0); i < hpTimes_join; ++i) {
        trans_JOIN_INNER_TD_2136_out.add(&(tbl_JOIN_INNER_TD_2136_output_partition_array[i]));
    }
    trans_JOIN_INNER_TD_2136_out.dev2host(0, &(events_JOIN_INNER_TD_2136[1]), &(events_d2h_rd_JOIN_INNER_TD_2136[0]));
    q_h.flush();
    q_h.finish();
    tbl_JOIN_INNER_TD_2136_output.setNumRow((cfg_JOIN_INNER_TD_2136_cmds_probe.meta_out)->getColLen());
    tbl_JOIN_INNER_TD_2136_output.colToTable();
    SW_Consolidated_JOIN_INNER_TD_2136_output(tbl_SerializeFromObject_TD_338_input_stringRowIDSubstitute, tbl_JOIN_INNER_TD_3858_consolidated_output, tbl_JOIN_INNER_TD_2136_output, tbl_JOIN_INNER_TD_2136_consolidated_output);
    gettimeofday(&tv_r_JOIN_INNER_2_756_e, 0);

    struct timeval tv_r_Aggregate_1_231_s, tv_r_Aggregate_1_231_e;
    gettimeofday(&tv_r_Aggregate_1_231_s, 0);
    SW_Aggregate_TD_1486(tbl_JOIN_INNER_TD_2136_consolidated_output_partition_array, tbl_SerializeFromObject_TD_5609_input, tbl_SerializeFromObject_TD_6481_input, tbl_SerializeFromObject_TD_5746_input, tbl_SerializeFromObject_TD_338_input, tbl_Aggregate_TD_1486_output, hpTimes_join);
    gettimeofday(&tv_r_Aggregate_1_231_e, 0);

    struct timeval tv_r_Sort_0_588_s, tv_r_Sort_0_588_e;
    gettimeofday(&tv_r_Sort_0_588_s, 0);
    SW_Sort_TD_066(tbl_Aggregate_TD_1486_output, tbl_Sort_TD_066_output);
    gettimeofday(&tv_r_Sort_0_588_e, 0);

std::cout << "CPU TIME: " << (tvdiff(&tv_r_Filter_5_101_s, &tv_r_Filter_5_101_e) + 
tvdiff(&tv_r_Filter_4_31_s, &tv_r_Filter_4_31_e) + 
tvdiff(&tv_r_Aggregate_1_231_s, &tv_r_Aggregate_1_231_e) + 
tvdiff(&tv_r_Sort_0_588_s, &tv_r_Sort_0_588_e) + 
1) / 1000.0 << std::endl;

std::cout << "FPGA TIME: " << (tvdiff(&tv_r_JOIN_INNER_4_676_s, &tv_r_JOIN_INNER_4_676_e) + 
tvdiff(&tv_r_JOIN_INNER_3_362_s, &tv_r_JOIN_INNER_3_362_e) + 
tvdiff(&tv_r_JOIN_INNER_2_756_s, &tv_r_JOIN_INNER_2_756_e) + 
1) / 1000.0 << std::endl;

    gettimeofday(&tv_r_e, 0); 
    // **************************** Print Execution Time ************************** // 
    std::cout << std::endl; 
    std::cout << "Filter_5: " << tvdiff(&tv_r_Filter_5_101_s, &tv_r_Filter_5_101_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(((o_orderdate#211 >= 19940801) AND (o_orderdate#211 < 19941101))) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_6481_input.getNumRow() << " -> tbl_SerializeFromObject_TD_6481_input" 
     << std::endl << "      #COL: 3: " << "ListBuffer(o_orderkey#207, o_custkey#208, o_orderdate#211)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_5436_output.getNumRow() << " -> tbl_Filter_TD_5436_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(o_orderkey#207, o_custkey#208)" 
     << std::endl; 

    std::cout << "Filter_4: " << tvdiff(&tv_r_Filter_4_31_s, &tv_r_Filter_4_31_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer((l_returnflag#85 = 82)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_5746_input.getNumRow() << " -> tbl_SerializeFromObject_TD_5746_input" 
     << std::endl << "      #COL: 4: " << "ListBuffer(l_orderkey#77, l_extendedprice#82, l_discount#83, l_returnflag#85)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_4404_output.getNumRow() << " -> tbl_Filter_TD_4404_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(l_orderkey#77, l_extendedprice#82, l_discount#83)" 
     << std::endl; 

    std::cout << "JOIN_INNER_4: " << tvdiff(&tv_r_JOIN_INNER_4_676_s, &tv_r_JOIN_INNER_4_676_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer((c_custkey#22 = o_custkey#208)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_5609_input_stringRowIDSubstitute.getNumRow() << " -> tbl_SerializeFromObject_TD_5609_input_stringRowIDSubstitute" 
     << std::endl << "      #COL: 4: " << "ListBuffer(c_custkey#22, c_name#23, c_nationkey#25, c_acctbal#27)" 
     << std::endl << "      #ROW: " << tbl_Filter_TD_5436_output.getNumRow() << " -> tbl_Filter_TD_5436_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(o_orderkey#207, o_custkey#208)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_4808_consolidated_output.getNumRow() << " -> tbl_JOIN_INNER_TD_4808_consolidated_output" 
     << std::endl << "      #COL: 5: " << "ListBuffer(c_custkey#22, c_name#23, c_nationkey#25, c_acctbal#27, o_orderkey#207)" 
     << std::endl; 

    std::cout << "JOIN_INNER_3: " << tvdiff(&tv_r_JOIN_INNER_3_362_s, &tv_r_JOIN_INNER_3_362_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer((l_orderkey#77 = o_orderkey#207)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_4808_consolidated_output.getNumRow() << " -> tbl_JOIN_INNER_TD_4808_consolidated_output" 
     << std::endl << "      #COL: 5: " << "ListBuffer(c_custkey#22, c_name#23, c_nationkey#25, c_acctbal#27, o_orderkey#207)" 
     << std::endl << "      #ROW: " << tbl_Filter_TD_4404_output.getNumRow() << " -> tbl_Filter_TD_4404_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(l_orderkey#77, l_extendedprice#82, l_discount#83)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_3858_consolidated_output.getNumRow() << " -> tbl_JOIN_INNER_TD_3858_consolidated_output" 
     << std::endl << "      #COL: 6: " << "ListBuffer(c_custkey#22, c_name#23, c_nationkey#25, c_acctbal#27, l_extendedprice#82, l_discount#83)" 
     << std::endl; 

    std::cout << "JOIN_INNER_2: " << tvdiff(&tv_r_JOIN_INNER_2_756_s, &tv_r_JOIN_INNER_2_756_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer((c_nationkey#25 = n_nationkey#144)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_3858_consolidated_output.getNumRow() << " -> tbl_JOIN_INNER_TD_3858_consolidated_output" 
     << std::endl << "      #COL: 6: " << "ListBuffer(c_custkey#22, c_name#23, c_nationkey#25, c_acctbal#27, l_extendedprice#82, l_discount#83)" 
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_338_input_stringRowIDSubstitute.getNumRow() << " -> tbl_SerializeFromObject_TD_338_input_stringRowIDSubstitute" 
     << std::endl << "      #COL: 2: " << "ListBuffer(n_nationkey#144, n_name#145)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_2136_consolidated_output.getNumRow() << " -> tbl_JOIN_INNER_TD_2136_consolidated_output" 
     << std::endl << "      #COL: 6: " << "ListBuffer(c_custkey#22, c_name#23, c_acctbal#27, l_extendedprice#82, l_discount#83, n_name#145)" 
     << std::endl; 

    std::cout << "Aggregate_1: " << tvdiff(&tv_r_Aggregate_1_231_s, &tv_r_Aggregate_1_231_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(c_custkey#22, c_name#23, c_acctbal#27, n_name#145, sum((l_extendedprice#82 * (100 - l_discount#83))) AS revenue#1127L) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_2136_consolidated_output.getNumRow() << " -> tbl_JOIN_INNER_TD_2136_consolidated_output" 
     << std::endl << "      #COL: 6: " << "ListBuffer(c_custkey#22, c_name#23, c_acctbal#27, l_extendedprice#82, l_discount#83, n_name#145)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_1486_output.getNumRow() << " -> tbl_Aggregate_TD_1486_output" 
     << std::endl << "      #COL: 5: " << "ListBuffer(c_custkey#22, c_name#23, revenue#1127L, c_acctbal#27, n_name#145)" 
     << std::endl; 

    std::cout << "Sort_0: " << tvdiff(&tv_r_Sort_0_588_s, &tv_r_Sort_0_588_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(revenue#1127L DESC NULLS LAST) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_1486_output.getNumRow() << " -> tbl_Aggregate_TD_1486_output" 
     << std::endl << "      #COL: 5: " << "ListBuffer(c_custkey#22, c_name#23, revenue#1127L, c_acctbal#27, n_name#145)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Sort_TD_066_output.getNumRow() << " -> tbl_Sort_TD_066_output" 
     << std::endl << "      #COL: 5: " << "ListBuffer(c_custkey#22, c_name#23, revenue#1127L, c_acctbal#27, n_name#145)" 
     << std::endl; 

    std::cout << std::endl << " Total execution time: " << tvdiff(&tv_r_s, &tv_r_e) / 1000 << " ms"; 

    std::cout << std::endl << " Spark elapsed time: " << 22.295704 * 1000 << "ms" << std::endl; 
    return 0; 
}
