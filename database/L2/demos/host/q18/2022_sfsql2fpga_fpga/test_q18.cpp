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
    // *********************** Partition Infomation ********************* // 
    int hpTimes_join = 32; 
    int power_of_hpTimes_join = log2(hpTimes_join); 
    std::cout << "Number of partition (gqeJoin) is: " << hpTimes_join << std::endl; 
    int hpTimes_aggr = 256; 
    int power_of_hpTimes_aggr = log2(hpTimes_aggr); 
    std::cout << "Number of partition (gqeAggr) is: " << hpTimes_aggr << std::endl; 
    // ****************************** Tables **************************** // 
    Table tbl_Sort_TD_065_output("tbl_Sort_TD_065_output", 183000000, 6, "");
    tbl_Sort_TD_065_output.allocateHost();
    Table tbl_Aggregate_TD_1710_output("tbl_Aggregate_TD_1710_output", 183000000, 6, "");
    tbl_Aggregate_TD_1710_output.allocateHost();
    Table tbl_JOIN_INNER_TD_2592_output("tbl_JOIN_INNER_TD_2592_output", 100, 4, "");
    tbl_JOIN_INNER_TD_2592_output.allocateHost(1.2, hpTimes_join);
    Table tbl_JOIN_INNER_TD_2592_consolidated_output("tbl_JOIN_INNER_TD_2592_consolidated_output", 100, 6, "");
    tbl_JOIN_INNER_TD_2592_consolidated_output.selectOverlayVersion(1);
    tbl_JOIN_INNER_TD_2592_consolidated_output.allocateHost();
    Table tbl_JOIN_INNER_TD_3608_output("tbl_JOIN_INNER_TD_3608_output", 100, 4, "");
    tbl_JOIN_INNER_TD_3608_output.allocateHost(1.2, hpTimes_join);
    Table tbl_JOIN_INNER_TD_3608_consolidated_output("tbl_JOIN_INNER_TD_3608_consolidated_output", 100, 5, "");
    tbl_JOIN_INNER_TD_3608_consolidated_output.selectOverlayVersion(1);
    tbl_JOIN_INNER_TD_3608_consolidated_output.allocateHost();
    Table tbl_JOIN_LEFTSEMI_TD_3435_output("tbl_JOIN_LEFTSEMI_TD_3435_output", 100, 4, "");
    tbl_JOIN_LEFTSEMI_TD_3435_output.allocateHost(1.2, hpTimes_join);
    Table tbl_JOIN_LEFTSEMI_TD_3435_consolidated_output("tbl_JOIN_LEFTSEMI_TD_3435_consolidated_output", 100, 2, "");
    tbl_JOIN_LEFTSEMI_TD_3435_consolidated_output.selectOverlayVersion(1);
    tbl_JOIN_LEFTSEMI_TD_3435_consolidated_output.allocateHost();
    Table tbl_SerializeFromObject_TD_4246_input_stringRowIDSubstitute;
    tbl_SerializeFromObject_TD_4246_input_stringRowIDSubstitute = Table("customer", customer_n, 2, in_dir);
    tbl_SerializeFromObject_TD_4246_input_stringRowIDSubstitute.addCol("c_custkey", 4);
    tbl_SerializeFromObject_TD_4246_input_stringRowIDSubstitute.addCol("c_name", 4, 1, 0);
    tbl_SerializeFromObject_TD_4246_input_stringRowIDSubstitute.allocateHost();
    tbl_SerializeFromObject_TD_4246_input_stringRowIDSubstitute.loadHost();
    Table tbl_SerializeFromObject_TD_4246_input_stringRowIDSubstitute_partition("tbl_SerializeFromObject_TD_4246_input_stringRowIDSubstitute_partition", customer_n, 2, "");
    tbl_SerializeFromObject_TD_4246_input_stringRowIDSubstitute_partition.allocateHost(1.2, hpTimes_join);
    tbl_SerializeFromObject_TD_4246_input_stringRowIDSubstitute_partition.allocateDevBuffer(context_h, 32);
    Table tbl_SerializeFromObject_TD_4246_input_stringRowIDSubstitute_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_SerializeFromObject_TD_4246_input_stringRowIDSubstitute_partition_array[i] = tbl_SerializeFromObject_TD_4246_input_stringRowIDSubstitute_partition.createSubTable(i);
    }
    Table tbl_SerializeFromObject_TD_4246_input;
    tbl_SerializeFromObject_TD_4246_input = Table("customer", customer_n, 2, in_dir);
    tbl_SerializeFromObject_TD_4246_input.addCol("c_custkey", 4);
    tbl_SerializeFromObject_TD_4246_input.addCol("c_name", TPCH_READ_C_NAME_LEN+1);
    tbl_SerializeFromObject_TD_4246_input.allocateHost();
    tbl_SerializeFromObject_TD_4246_input.loadHost();
    Table tbl_JOIN_LEFTSEMI_TD_4287_output("tbl_JOIN_LEFTSEMI_TD_4287_output", 100, 4, "");
    tbl_JOIN_LEFTSEMI_TD_4287_output.allocateHost(1.2, hpTimes_join);
    Table tbl_JOIN_LEFTSEMI_TD_4287_consolidated_output("tbl_JOIN_LEFTSEMI_TD_4287_consolidated_output", 100, 4, "");
    tbl_JOIN_LEFTSEMI_TD_4287_consolidated_output.selectOverlayVersion(1);
    tbl_JOIN_LEFTSEMI_TD_4287_consolidated_output.allocateHost();
    Table tbl_SerializeFromObject_TD_4766_input;
    tbl_SerializeFromObject_TD_4766_input = Table("lineitem", lineitem_n, 2, in_dir);
    tbl_SerializeFromObject_TD_4766_input.selectOverlayVersion(1);
    tbl_SerializeFromObject_TD_4766_input.addCol("l_orderkey", 4);
    tbl_SerializeFromObject_TD_4766_input.addCol("l_quantity", 4);
    tbl_SerializeFromObject_TD_4766_input.allocateHost();
    tbl_SerializeFromObject_TD_4766_input.loadHost();
    Table tbl_SerializeFromObject_TD_4766_input_partition("tbl_SerializeFromObject_TD_4766_input_partition", lineitem_n, 2, "");
    tbl_SerializeFromObject_TD_4766_input_partition.allocateHost(1.2, hpTimes_join);
    tbl_SerializeFromObject_TD_4766_input_partition.allocateDevBuffer(context_h, 32);
    Table tbl_SerializeFromObject_TD_4766_input_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_SerializeFromObject_TD_4766_input_partition_array[i] = tbl_SerializeFromObject_TD_4766_input_partition.createSubTable(i);
    }
    Table tbl_Filter_TD_4654_output("tbl_Filter_TD_4654_output", 183000000, 1, "");
    tbl_Filter_TD_4654_output.selectOverlayVersion(1);
    tbl_Filter_TD_4654_output.allocateHost();
    Table tbl_Filter_TD_4654_output_partition("tbl_Filter_TD_4654_output_partition", 183000000, 1, "");
    tbl_Filter_TD_4654_output_partition.allocateHost(1.2, hpTimes_join);
    tbl_Filter_TD_4654_output_partition.allocateDevBuffer(context_h, 32);
    Table tbl_Filter_TD_4654_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_Filter_TD_4654_output_partition_array[i] = tbl_Filter_TD_4654_output_partition.createSubTable(i);
    }
    Table tbl_SerializeFromObject_TD_5226_input;
    tbl_SerializeFromObject_TD_5226_input = Table("order", order_n, 4, in_dir);
    tbl_SerializeFromObject_TD_5226_input.selectOverlayVersion(1);
    tbl_SerializeFromObject_TD_5226_input.addCol("o_orderkey", 4);
    tbl_SerializeFromObject_TD_5226_input.addCol("o_custkey", 4);
    tbl_SerializeFromObject_TD_5226_input.addCol("o_totalprice", 4);
    tbl_SerializeFromObject_TD_5226_input.addCol("o_orderdate", 4);
    tbl_SerializeFromObject_TD_5226_input.allocateHost();
    tbl_SerializeFromObject_TD_5226_input.loadHost();
    Table tbl_SerializeFromObject_TD_5226_input_partition("tbl_SerializeFromObject_TD_5226_input_partition", order_n, 4, "");
    tbl_SerializeFromObject_TD_5226_input_partition.allocateHost(1.2, hpTimes_join);
    tbl_SerializeFromObject_TD_5226_input_partition.allocateDevBuffer(context_h, 32);
    Table tbl_SerializeFromObject_TD_5226_input_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_SerializeFromObject_TD_5226_input_partition_array[i] = tbl_SerializeFromObject_TD_5226_input_partition.createSubTable(i);
    }
    Table tbl_Filter_TD_5586_output("tbl_Filter_TD_5586_output", 183000000, 1, "");
    tbl_Filter_TD_5586_output.selectOverlayVersion(1);
    tbl_Filter_TD_5586_output.allocateHost();
    Table tbl_Filter_TD_5586_output_partition("tbl_Filter_TD_5586_output_partition", 183000000, 1, "");
    tbl_Filter_TD_5586_output_partition.allocateHost(1.2, hpTimes_join);
    tbl_Filter_TD_5586_output_partition.allocateDevBuffer(context_h, 32);
    Table tbl_Filter_TD_5586_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_Filter_TD_5586_output_partition_array[i] = tbl_Filter_TD_5586_output_partition.createSubTable(i);
    }
    Table tbl_Aggregate_TD_6132_output_preprocess("tbl_Aggregate_TD_6132_output_preprocess", 3000000, 16, "");
    tbl_Aggregate_TD_6132_output_preprocess.allocateHost(1.2, hpTimes_aggr);
    Table tbl_Aggregate_TD_6132_output("tbl_Aggregate_TD_6132_output", 3000000, 2, "");
    Table tbl_SerializeFromObject_TD_7564_input;
    tbl_SerializeFromObject_TD_7564_input = Table("lineitem", lineitem_n, 2, in_dir);
    tbl_SerializeFromObject_TD_7564_input.addCol("l_orderkey", 4);
    tbl_SerializeFromObject_TD_7564_input.addCol("l_quantity", 4);
    tbl_SerializeFromObject_TD_7564_input.allocateHost();
    tbl_SerializeFromObject_TD_7564_input.loadHost();
    Table tbl_SerializeFromObject_TD_7564_input_partition("tbl_SerializeFromObject_TD_7564_input_partition", lineitem_n, 2, "");
    tbl_SerializeFromObject_TD_7564_input_partition.allocateHost(1.2, hpTimes_aggr);
    tbl_SerializeFromObject_TD_7564_input_partition.allocateDevBuffer(context_a, 32);
    Table tbl_SerializeFromObject_TD_7564_input_partition_array[hpTimes_aggr];
    for (int i(0); i < hpTimes_aggr; ++i) {
        tbl_SerializeFromObject_TD_7564_input_partition_array[i] = tbl_SerializeFromObject_TD_7564_input_partition.createSubTable(i);
    }
    // ********************** Allocate Device Buffer ******************** // 
    tbl_JOIN_INNER_TD_2592_output.allocateDevBuffer(context_h, 32);
    Table tbl_JOIN_INNER_TD_2592_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_JOIN_INNER_TD_2592_output_partition_array[i] = tbl_JOIN_INNER_TD_2592_output.createSubTable(i);
    }
    tbl_JOIN_INNER_TD_3608_output.allocateDevBuffer(context_h, 32);
    Table tbl_JOIN_INNER_TD_3608_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_JOIN_INNER_TD_3608_output_partition_array[i] = tbl_JOIN_INNER_TD_3608_output.createSubTable(i);
    }
    tbl_JOIN_LEFTSEMI_TD_3435_output.allocateDevBuffer(context_h, 32);
    Table tbl_JOIN_LEFTSEMI_TD_3435_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_JOIN_LEFTSEMI_TD_3435_output_partition_array[i] = tbl_JOIN_LEFTSEMI_TD_3435_output.createSubTable(i);
    }
    tbl_SerializeFromObject_TD_4246_input_stringRowIDSubstitute.allocateDevBuffer(context_h, 33);
    tbl_JOIN_LEFTSEMI_TD_4287_output.allocateDevBuffer(context_h, 32);
    Table tbl_JOIN_LEFTSEMI_TD_4287_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_JOIN_LEFTSEMI_TD_4287_output_partition_array[i] = tbl_JOIN_LEFTSEMI_TD_4287_output.createSubTable(i);
    }
    tbl_SerializeFromObject_TD_4766_input.allocateDevBuffer(context_h, 33);
    tbl_Filter_TD_4654_output.allocateDevBuffer(context_h, 33);
    tbl_SerializeFromObject_TD_5226_input.allocateDevBuffer(context_h, 33);
    tbl_Filter_TD_5586_output.allocateDevBuffer(context_h, 33);
    tbl_Aggregate_TD_6132_output_preprocess.allocateDevBuffer(context_a, 33);
    Table tbl_Aggregate_TD_6132_output_partition_array[hpTimes_aggr];
    for (int i(0); i < hpTimes_aggr; ++i) {
        tbl_Aggregate_TD_6132_output_partition_array[i] = tbl_Aggregate_TD_6132_output_preprocess.createSubTable(i);
    }
    tbl_SerializeFromObject_TD_7564_input.allocateDevBuffer(context_a, 33);
    // ****************************** Config **************************** // 
    
    xf::database::gqe::KernelCommand krn_cmd_build_735 = xf::database::gqe::KernelCommand();
    xf::database::gqe::KernelCommand krn_cmd_probe_735 = xf::database::gqe::KernelCommand();
    cfgCmd cfg_JOIN_INNER_TD_2592_cmds_build;
    cfg_JOIN_INNER_TD_2592_cmds_build.setup(1);
    cfg_JOIN_INNER_TD_2592_cmds_build.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_2592_gqe_join_build(krn_cmd_build_735);
    cfg_JOIN_INNER_TD_2592_cmds_build.cmd = krn_cmd_build_735.getConfigBits();
    get_meta_info_JOIN_INNER_TD_2592_gqe_join_build(cfg_JOIN_INNER_TD_2592_cmds_build.meta_in, tbl_JOIN_INNER_TD_3608_consolidated_output.nrow, 3);
    get_meta_info_JOIN_INNER_TD_2592_gqe_join_build(cfg_JOIN_INNER_TD_2592_cmds_build.meta_out, tbl_JOIN_INNER_TD_2592_output.nrow, 4);
    // cfg_JOIN_INNER_TD_2592_cmds_build.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_2592_cmds_probe;
    cfg_JOIN_INNER_TD_2592_cmds_probe.setup(1);
    cfg_JOIN_INNER_TD_2592_cmds_probe.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_2592_gqe_join_probe(krn_cmd_probe_735);
    cfg_JOIN_INNER_TD_2592_cmds_probe.cmd = krn_cmd_probe_735.getConfigBits();
    get_meta_info_JOIN_INNER_TD_2592_gqe_join_probe(cfg_JOIN_INNER_TD_2592_cmds_probe.meta_in, tbl_JOIN_LEFTSEMI_TD_3435_consolidated_output.nrow, 3);
    get_meta_info_JOIN_INNER_TD_2592_gqe_join_probe(cfg_JOIN_INNER_TD_2592_cmds_probe.meta_out, tbl_JOIN_INNER_TD_2592_output.nrow, 3);
    // cfg_JOIN_INNER_TD_2592_cmds_probe.allocateDevBuffer(context_h, 32);
    cfg_JOIN_INNER_TD_2592_cmds_build.input_key_columns = {2, -1, -1};
    cfg_JOIN_INNER_TD_2592_cmds_probe.input_key_columns = {0, -1, -1};
    cfg_JOIN_INNER_TD_2592_cmds_build.output_key_columns = {0, 1, 2, 3};
    cfg_JOIN_INNER_TD_2592_cmds_probe.output_key_columns = {0, 1, 2, 3};
    
    xf::database::gqe::KernelCommand krn_cmd_build_295 = xf::database::gqe::KernelCommand();
    xf::database::gqe::KernelCommand krn_cmd_probe_295 = xf::database::gqe::KernelCommand();
    cfgCmd cfg_JOIN_INNER_TD_3608_cmds_build;
    cfg_JOIN_INNER_TD_3608_cmds_build.setup(1);
    cfg_JOIN_INNER_TD_3608_cmds_build.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_3608_gqe_join_build(krn_cmd_build_295);
    cfg_JOIN_INNER_TD_3608_cmds_build.cmd = krn_cmd_build_295.getConfigBits();
    get_meta_info_JOIN_INNER_TD_3608_gqe_join_build(cfg_JOIN_INNER_TD_3608_cmds_build.meta_in, tbl_SerializeFromObject_TD_4246_input_stringRowIDSubstitute.nrow, 3);
    get_meta_info_JOIN_INNER_TD_3608_gqe_join_build(cfg_JOIN_INNER_TD_3608_cmds_build.meta_out, tbl_JOIN_INNER_TD_3608_output.nrow, 4);
    // cfg_JOIN_INNER_TD_3608_cmds_build.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_3608_cmds_probe;
    cfg_JOIN_INNER_TD_3608_cmds_probe.setup(1);
    cfg_JOIN_INNER_TD_3608_cmds_probe.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_3608_gqe_join_probe(krn_cmd_probe_295);
    cfg_JOIN_INNER_TD_3608_cmds_probe.cmd = krn_cmd_probe_295.getConfigBits();
    get_meta_info_JOIN_INNER_TD_3608_gqe_join_probe(cfg_JOIN_INNER_TD_3608_cmds_probe.meta_in, tbl_JOIN_LEFTSEMI_TD_4287_consolidated_output.nrow, 3);
    get_meta_info_JOIN_INNER_TD_3608_gqe_join_probe(cfg_JOIN_INNER_TD_3608_cmds_probe.meta_out, tbl_JOIN_INNER_TD_3608_output.nrow, 3);
    // cfg_JOIN_INNER_TD_3608_cmds_probe.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_3608_cmds_part;
    cfg_JOIN_INNER_TD_3608_cmds_part.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_3608_gqe_join_part (cfg_JOIN_INNER_TD_3608_cmds_part.cmd);
    cfg_JOIN_INNER_TD_3608_cmds_part.allocateDevBuffer(context_h, 32);
    cfg_JOIN_INNER_TD_3608_cmds_build.input_key_columns = {0, -1, -1};
    cfg_JOIN_INNER_TD_3608_cmds_probe.input_key_columns = {1, -1, -1};
    cfg_JOIN_INNER_TD_3608_cmds_build.output_key_columns = {0, 1, 2, 3};
    cfg_JOIN_INNER_TD_3608_cmds_probe.output_key_columns = {0, 1, 2, 3};
    
    xf::database::gqe::KernelCommand krn_cmd_build_307 = xf::database::gqe::KernelCommand();
    xf::database::gqe::KernelCommand krn_cmd_probe_307 = xf::database::gqe::KernelCommand();
    cfgCmd cfg_JOIN_LEFTSEMI_TD_3435_cmds_build;
    cfg_JOIN_LEFTSEMI_TD_3435_cmds_build.setup(1);
    cfg_JOIN_LEFTSEMI_TD_3435_cmds_build.allocateHost();
    get_cfg_dat_JOIN_LEFTSEMI_TD_3435_gqe_join_build(krn_cmd_build_307);
    cfg_JOIN_LEFTSEMI_TD_3435_cmds_build.cmd = krn_cmd_build_307.getConfigBits();
    get_meta_info_JOIN_LEFTSEMI_TD_3435_gqe_join_build(cfg_JOIN_LEFTSEMI_TD_3435_cmds_build.meta_in, tbl_Filter_TD_4654_output.nrow, 3);
    get_meta_info_JOIN_LEFTSEMI_TD_3435_gqe_join_build(cfg_JOIN_LEFTSEMI_TD_3435_cmds_build.meta_out, tbl_JOIN_LEFTSEMI_TD_3435_output.nrow, 4);
    // cfg_JOIN_LEFTSEMI_TD_3435_cmds_build.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_LEFTSEMI_TD_3435_cmds_probe;
    cfg_JOIN_LEFTSEMI_TD_3435_cmds_probe.setup(1);
    cfg_JOIN_LEFTSEMI_TD_3435_cmds_probe.allocateHost();
    get_cfg_dat_JOIN_LEFTSEMI_TD_3435_gqe_join_probe(krn_cmd_probe_307);
    cfg_JOIN_LEFTSEMI_TD_3435_cmds_probe.cmd = krn_cmd_probe_307.getConfigBits();
    get_meta_info_JOIN_LEFTSEMI_TD_3435_gqe_join_probe(cfg_JOIN_LEFTSEMI_TD_3435_cmds_probe.meta_in, tbl_SerializeFromObject_TD_4766_input.nrow, 3);
    get_meta_info_JOIN_LEFTSEMI_TD_3435_gqe_join_probe(cfg_JOIN_LEFTSEMI_TD_3435_cmds_probe.meta_out, tbl_JOIN_LEFTSEMI_TD_3435_output.nrow, 3);
    // cfg_JOIN_LEFTSEMI_TD_3435_cmds_probe.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_LEFTSEMI_TD_3435_cmds_part;
    cfg_JOIN_LEFTSEMI_TD_3435_cmds_part.allocateHost();
    get_cfg_dat_JOIN_LEFTSEMI_TD_3435_gqe_join_part (cfg_JOIN_LEFTSEMI_TD_3435_cmds_part.cmd);
    cfg_JOIN_LEFTSEMI_TD_3435_cmds_part.allocateDevBuffer(context_h, 32);
    cfg_JOIN_LEFTSEMI_TD_3435_cmds_build.input_key_columns = {0, -1, -1};
    cfg_JOIN_LEFTSEMI_TD_3435_cmds_probe.input_key_columns = {0, -1, -1};
    cfg_JOIN_LEFTSEMI_TD_3435_cmds_build.output_key_columns = {0, 1, 2, 3};
    cfg_JOIN_LEFTSEMI_TD_3435_cmds_probe.output_key_columns = {0, 1, 2, 3};
    
    xf::database::gqe::KernelCommand krn_cmd_build_474 = xf::database::gqe::KernelCommand();
    xf::database::gqe::KernelCommand krn_cmd_probe_474 = xf::database::gqe::KernelCommand();
    cfgCmd cfg_JOIN_LEFTSEMI_TD_4287_cmds_build;
    cfg_JOIN_LEFTSEMI_TD_4287_cmds_build.setup(1);
    cfg_JOIN_LEFTSEMI_TD_4287_cmds_build.allocateHost();
    get_cfg_dat_JOIN_LEFTSEMI_TD_4287_gqe_join_build(krn_cmd_build_474);
    cfg_JOIN_LEFTSEMI_TD_4287_cmds_build.cmd = krn_cmd_build_474.getConfigBits();
    get_meta_info_JOIN_LEFTSEMI_TD_4287_gqe_join_build(cfg_JOIN_LEFTSEMI_TD_4287_cmds_build.meta_in, tbl_Filter_TD_5586_output.nrow, 3);
    get_meta_info_JOIN_LEFTSEMI_TD_4287_gqe_join_build(cfg_JOIN_LEFTSEMI_TD_4287_cmds_build.meta_out, tbl_JOIN_LEFTSEMI_TD_4287_output.nrow, 4);
    // cfg_JOIN_LEFTSEMI_TD_4287_cmds_build.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_LEFTSEMI_TD_4287_cmds_probe;
    cfg_JOIN_LEFTSEMI_TD_4287_cmds_probe.setup(1);
    cfg_JOIN_LEFTSEMI_TD_4287_cmds_probe.allocateHost();
    get_cfg_dat_JOIN_LEFTSEMI_TD_4287_gqe_join_probe(krn_cmd_probe_474);
    cfg_JOIN_LEFTSEMI_TD_4287_cmds_probe.cmd = krn_cmd_probe_474.getConfigBits();
    get_meta_info_JOIN_LEFTSEMI_TD_4287_gqe_join_probe(cfg_JOIN_LEFTSEMI_TD_4287_cmds_probe.meta_in, tbl_SerializeFromObject_TD_5226_input.nrow, 3);
    get_meta_info_JOIN_LEFTSEMI_TD_4287_gqe_join_probe(cfg_JOIN_LEFTSEMI_TD_4287_cmds_probe.meta_out, tbl_JOIN_LEFTSEMI_TD_4287_output.nrow, 3);
    // cfg_JOIN_LEFTSEMI_TD_4287_cmds_probe.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_LEFTSEMI_TD_4287_cmds_part;
    cfg_JOIN_LEFTSEMI_TD_4287_cmds_part.allocateHost();
    get_cfg_dat_JOIN_LEFTSEMI_TD_4287_gqe_join_part (cfg_JOIN_LEFTSEMI_TD_4287_cmds_part.cmd);
    cfg_JOIN_LEFTSEMI_TD_4287_cmds_part.allocateDevBuffer(context_h, 32);
    cfg_JOIN_LEFTSEMI_TD_4287_cmds_build.input_key_columns = {0, -1, -1};
    cfg_JOIN_LEFTSEMI_TD_4287_cmds_probe.input_key_columns = {0, -1, -1};
    cfg_JOIN_LEFTSEMI_TD_4287_cmds_build.output_key_columns = {0, 1, 2, 3};
    cfg_JOIN_LEFTSEMI_TD_4287_cmds_probe.output_key_columns = {0, 1, 2, 3};
    AggrCfgCmd cfg_Aggregate_TD_6132_cmds;
    cfg_Aggregate_TD_6132_cmds.allocateHost();
    get_cfg_dat_Aggregate_TD_6132_gqe_aggr(cfg_Aggregate_TD_6132_cmds.cmd);
    cfg_Aggregate_TD_6132_cmds.allocateDevBuffer(context_a, 32);
    AggrCfgCmd cfg_Aggregate_TD_6132_cmds_out;
    cfg_Aggregate_TD_6132_cmds_out.allocateHost();
    cfg_Aggregate_TD_6132_cmds_out.allocateDevBuffer(context_a, 33);
    // *************************** Kernel Setup ************************* // 
    bufferTmp buftmp_h(context_h, 1); 
    buftmp_h.initBuffer(q_h); 
    AggrBufferTmp buftmp_a(context_a, 1); 
    buftmp_a.BufferInitial(q_a); 
    std::cout << std::endl; 
    krnlEngine krnl_JOIN_INNER_TD_2592[hpTimes_join];
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_INNER_TD_2592[i] = krnlEngine(program_h, q_h, "gqeJoin");
    }
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_INNER_TD_2592[i].setup(tbl_JOIN_INNER_TD_3608_consolidated_output_partition_array[i], tbl_JOIN_LEFTSEMI_TD_3435_consolidated_output_partition_array[i], tbl_JOIN_INNER_TD_2592_output_partition_array[i], , buftmp_h);
    }
    krnlEngine krnl_JOIN_INNER_TD_2592_build;
    krnl_JOIN_INNER_TD_2592_build = krnlEngine(program_h, q_h, context_h, "gqeKernel", 0);
    krnl_JOIN_INNER_TD_2592_build.setup(tbl_JOIN_INNER_TD_3608_consolidated_output, tbl_JOIN_LEFTSEMI_TD_3435_consolidated_output, tbl_JOIN_INNER_TD_2592_output, cfg_JOIN_INNER_TD_2592_cmds_build, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_2592_probe;
    krnl_JOIN_INNER_TD_2592_probe = krnlEngine(program_h, q_h, context_h, "gqeKernel", 1);
    krnl_JOIN_INNER_TD_2592_probe.setup(tbl_JOIN_LEFTSEMI_TD_3435_consolidated_output, tbl_JOIN_INNER_TD_3608_consolidated_output, tbl_JOIN_INNER_TD_2592_output, cfg_JOIN_INNER_TD_2592_cmds_probe, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_3608_part_left;
    krnl_JOIN_INNER_TD_3608_part_left = krnlEngine(program_h, q_h, context_h, "gqePart");
    krnl_JOIN_INNER_TD_3608_part_left.setup_hp(512, 0, power_of_hpTimes_join, tbl_SerializeFromObject_TD_4246_input_stringRowIDSubstitute, tbl_SerializeFromObject_TD_4246_input_stringRowIDSubstitute_partition, cfg_JOIN_INNER_TD_3608_cmds_part);
    krnlEngine krnl_JOIN_INNER_TD_3608[hpTimes_join];
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_INNER_TD_3608[i] = krnlEngine(program_h, q_h, "gqeJoin");
    }
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_INNER_TD_3608[i].setup(tbl_SerializeFromObject_TD_4246_input_stringRowIDSubstitute_partition_array[i], tbl_JOIN_LEFTSEMI_TD_4287_consolidated_output_partition_array[i], tbl_JOIN_INNER_TD_3608_output_partition_array[i], , buftmp_h);
    }
    krnlEngine krnl_JOIN_INNER_TD_3608_build;
    krnl_JOIN_INNER_TD_3608_build = krnlEngine(program_h, q_h, context_h, "gqeKernel", 0);
    krnl_JOIN_INNER_TD_3608_build.setup(tbl_SerializeFromObject_TD_4246_input_stringRowIDSubstitute, tbl_JOIN_LEFTSEMI_TD_4287_consolidated_output, tbl_JOIN_INNER_TD_3608_output, cfg_JOIN_INNER_TD_3608_cmds_build, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_3608_probe;
    krnl_JOIN_INNER_TD_3608_probe = krnlEngine(program_h, q_h, context_h, "gqeKernel", 1);
    krnl_JOIN_INNER_TD_3608_probe.setup(tbl_JOIN_LEFTSEMI_TD_4287_consolidated_output, tbl_SerializeFromObject_TD_4246_input_stringRowIDSubstitute, tbl_JOIN_INNER_TD_3608_output, cfg_JOIN_INNER_TD_3608_cmds_probe, buftmp_h);
    krnlEngine krnl_JOIN_LEFTSEMI_TD_3435_part_left;
    krnl_JOIN_LEFTSEMI_TD_3435_part_left = krnlEngine(program_h, q_h, context_h, "gqePart");
    krnl_JOIN_LEFTSEMI_TD_3435_part_left.setup_hp(512, 0, power_of_hpTimes_join, tbl_Filter_TD_4654_output, tbl_Filter_TD_4654_output_partition, cfg_JOIN_LEFTSEMI_TD_3435_cmds_part);
    krnlEngine krnl_JOIN_LEFTSEMI_TD_3435_part_right;
    krnl_JOIN_LEFTSEMI_TD_3435_part_right = krnlEngine(program_h, q_h, context_h, "gqePart");
    krnl_JOIN_LEFTSEMI_TD_3435_part_right.setup_hp(512, 1, power_of_hpTimes_join, tbl_SerializeFromObject_TD_4766_input, tbl_SerializeFromObject_TD_4766_input_partition, cfg_JOIN_LEFTSEMI_TD_3435_cmds_part);
    krnlEngine krnl_JOIN_LEFTSEMI_TD_3435[hpTimes_join];
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_LEFTSEMI_TD_3435[i] = krnlEngine(program_h, q_h, "gqeJoin");
    }
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_LEFTSEMI_TD_3435[i].setup(tbl_Filter_TD_4654_output_partition_array[i], tbl_SerializeFromObject_TD_4766_input_partition_array[i], tbl_JOIN_LEFTSEMI_TD_3435_output_partition_array[i], , buftmp_h);
    }
    krnlEngine krnl_JOIN_LEFTSEMI_TD_3435_build;
    krnl_JOIN_LEFTSEMI_TD_3435_build = krnlEngine(program_h, q_h, context_h, "gqeKernel", 0);
    krnl_JOIN_LEFTSEMI_TD_3435_build.setup(tbl_Filter_TD_4654_output, tbl_SerializeFromObject_TD_4766_input, tbl_JOIN_LEFTSEMI_TD_3435_output, cfg_JOIN_LEFTSEMI_TD_3435_cmds_build, buftmp_h);
    krnlEngine krnl_JOIN_LEFTSEMI_TD_3435_probe;
    krnl_JOIN_LEFTSEMI_TD_3435_probe = krnlEngine(program_h, q_h, context_h, "gqeKernel", 1);
    krnl_JOIN_LEFTSEMI_TD_3435_probe.setup(tbl_SerializeFromObject_TD_4766_input, tbl_Filter_TD_4654_output, tbl_JOIN_LEFTSEMI_TD_3435_output, cfg_JOIN_LEFTSEMI_TD_3435_cmds_probe, buftmp_h);
    krnlEngine krnl_JOIN_LEFTSEMI_TD_4287_part_left;
    krnl_JOIN_LEFTSEMI_TD_4287_part_left = krnlEngine(program_h, q_h, context_h, "gqePart");
    krnl_JOIN_LEFTSEMI_TD_4287_part_left.setup_hp(512, 0, power_of_hpTimes_join, tbl_Filter_TD_5586_output, tbl_Filter_TD_5586_output_partition, cfg_JOIN_LEFTSEMI_TD_4287_cmds_part);
    krnlEngine krnl_JOIN_LEFTSEMI_TD_4287_part_right;
    krnl_JOIN_LEFTSEMI_TD_4287_part_right = krnlEngine(program_h, q_h, context_h, "gqePart");
    krnl_JOIN_LEFTSEMI_TD_4287_part_right.setup_hp(512, 1, power_of_hpTimes_join, tbl_SerializeFromObject_TD_5226_input, tbl_SerializeFromObject_TD_5226_input_partition, cfg_JOIN_LEFTSEMI_TD_4287_cmds_part);
    krnlEngine krnl_JOIN_LEFTSEMI_TD_4287[hpTimes_join];
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_LEFTSEMI_TD_4287[i] = krnlEngine(program_h, q_h, "gqeJoin");
    }
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_LEFTSEMI_TD_4287[i].setup(tbl_Filter_TD_5586_output_partition_array[i], tbl_SerializeFromObject_TD_5226_input_partition_array[i], tbl_JOIN_LEFTSEMI_TD_4287_output_partition_array[i], , buftmp_h);
    }
    krnlEngine krnl_JOIN_LEFTSEMI_TD_4287_build;
    krnl_JOIN_LEFTSEMI_TD_4287_build = krnlEngine(program_h, q_h, context_h, "gqeKernel", 0);
    krnl_JOIN_LEFTSEMI_TD_4287_build.setup(tbl_Filter_TD_5586_output, tbl_SerializeFromObject_TD_5226_input, tbl_JOIN_LEFTSEMI_TD_4287_output, cfg_JOIN_LEFTSEMI_TD_4287_cmds_build, buftmp_h);
    krnlEngine krnl_JOIN_LEFTSEMI_TD_4287_probe;
    krnl_JOIN_LEFTSEMI_TD_4287_probe = krnlEngine(program_h, q_h, context_h, "gqeKernel", 1);
    krnl_JOIN_LEFTSEMI_TD_4287_probe.setup(tbl_SerializeFromObject_TD_5226_input, tbl_Filter_TD_5586_output, tbl_JOIN_LEFTSEMI_TD_4287_output, cfg_JOIN_LEFTSEMI_TD_4287_cmds_probe, buftmp_h);
    AggrKrnlEngine krnl_Aggregate_TD_6132_part;
    krnl_Aggregate_TD_6132_part = AggrKrnlEngine(program_a, q_a, "gqePart");
    krnl_Aggregate_TD_6132_part.setup_hp(512, 0, power_of_hpTimes_aggr, tbl_SerializeFromObject_TD_7564_input, tbl_SerializeFromObject_TD_7564_input_partition, cfg_Aggregate_TD_6132_cmds_part);
    AggrKrnlEngine krnl_Aggregate_TD_6132[hpTimes_aggr];
    for (int i = 0; i < hpTimes_aggr; i++) {
        krnl_Aggregate_TD_6132[i] = AggrKrnlEngine(program_a, q_a, "gqeAggr");
    }
    for (int i = 0; i < hpTimes_aggr; i++) {
        krnl_Aggregate_TD_6132[i].setup(tbl_SerializeFromObject_TD_7564_input_partition_array[i], tbl_Aggregate_TD_6132_output_partition_array[i], cfg_Aggregate_TD_6132_cmds, cfg_Aggregate_TD_6132_cmds_out, buftmp_a);
    }
    // ************************** Transfer Engine *********************** // 
    transEngine trans_JOIN_INNER_TD_2592;
    trans_JOIN_INNER_TD_2592.setq(q_h,1);
    trans_JOIN_INNER_TD_2592.add(&(cfg_JOIN_INNER_TD_2592_cmds_part));
    trans_JOIN_INNER_TD_2592.add(&(cfg_JOIN_INNER_TD_2592_cmds_build));
    trans_JOIN_INNER_TD_2592.add(&(cfg_JOIN_INNER_TD_2592_cmds_probe));
    transEngine trans_JOIN_INNER_TD_2592_out;
    trans_JOIN_INNER_TD_2592_out.setq(q_h,1);
    q_h.finish();
    transEngine trans_JOIN_INNER_TD_3608;
    trans_JOIN_INNER_TD_3608.setq(q_h,1);
    trans_JOIN_INNER_TD_3608.add(&(cfg_JOIN_INNER_TD_3608_cmds_part));
    trans_JOIN_INNER_TD_3608.add(&(cfg_JOIN_INNER_TD_3608_cmds_build));
    trans_JOIN_INNER_TD_3608.add(&(cfg_JOIN_INNER_TD_3608_cmds_probe));
    trans_JOIN_INNER_TD_3608.add(&(tbl_SerializeFromObject_TD_4246_input_stringRowIDSubstitute));
    transEngine trans_JOIN_INNER_TD_3608_out;
    trans_JOIN_INNER_TD_3608_out.setq(q_h,1);
    q_h.finish();
    transEngine trans_JOIN_LEFTSEMI_TD_3435;
    trans_JOIN_LEFTSEMI_TD_3435.setq(q_h,1);
    trans_JOIN_LEFTSEMI_TD_3435.add(&(cfg_JOIN_LEFTSEMI_TD_3435_cmds_part));
    trans_JOIN_LEFTSEMI_TD_3435.add(&(cfg_JOIN_LEFTSEMI_TD_3435_cmds_build));
    trans_JOIN_LEFTSEMI_TD_3435.add(&(cfg_JOIN_LEFTSEMI_TD_3435_cmds_probe));
    trans_JOIN_LEFTSEMI_TD_3435.add(&(tbl_SerializeFromObject_TD_4766_input));
    transEngine trans_JOIN_LEFTSEMI_TD_3435_out;
    trans_JOIN_LEFTSEMI_TD_3435_out.setq(q_h,1);
    q_h.finish();
    transEngine trans_JOIN_LEFTSEMI_TD_4287;
    trans_JOIN_LEFTSEMI_TD_4287.setq(q_h,1);
    trans_JOIN_LEFTSEMI_TD_4287.add(&(cfg_JOIN_LEFTSEMI_TD_4287_cmds_part));
    trans_JOIN_LEFTSEMI_TD_4287.add(&(cfg_JOIN_LEFTSEMI_TD_4287_cmds_build));
    trans_JOIN_LEFTSEMI_TD_4287.add(&(cfg_JOIN_LEFTSEMI_TD_4287_cmds_probe));
    trans_JOIN_LEFTSEMI_TD_4287.add(&(tbl_SerializeFromObject_TD_5226_input));
    transEngine trans_JOIN_LEFTSEMI_TD_4287_out;
    trans_JOIN_LEFTSEMI_TD_4287_out.setq(q_h,1);
    q_h.finish();
    transEngine trans_Aggregate_TD_6132;
    trans_Aggregate_TD_6132.setq(q_a, 0);
    trans_Aggregate_TD_6132.add(&(cfg_Aggregate_TD_6132_cmds_part));
    trans_Aggregate_TD_6132.add(&(cfg_Aggregate_TD_6132_cmds));
    trans_Aggregate_TD_6132.add(&(cfg_Aggregate_TD_6132_cmds_out));
    trans_Aggregate_TD_6132.add(&(tbl_SerializeFromObject_TD_7564_input));
    transEngine trans_Aggregate_TD_6132_out;
    trans_Aggregate_TD_6132_out.setq(q_a, 0);
    q_a.finish();
    // ****************************** Events **************************** // 
    std::vector<cl::Event> events_h2d_wr_JOIN_INNER_TD_2592;
    std::vector<cl::Event> events_d2h_rd_JOIN_INNER_TD_2592;
    std::vector<cl::Event> events_JOIN_INNER_TD_2592[2];
    events_h2d_wr_JOIN_INNER_TD_2592.resize(1);
    events_d2h_rd_JOIN_INNER_TD_2592.resize(1);
    events_JOIN_INNER_TD_2592[0].resize(0);
    events_JOIN_INNER_TD_2592[1].resize(hpTimes_join);
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_2592;
    std::vector<cl::Event> prev_events_grp_JOIN_INNER_TD_2592;
    std::vector<cl::Event> events_h2d_wr_JOIN_INNER_TD_3608;
    std::vector<cl::Event> events_d2h_rd_JOIN_INNER_TD_3608;
    std::vector<cl::Event> events_JOIN_INNER_TD_3608[2];
    events_h2d_wr_JOIN_INNER_TD_3608.resize(1);
    events_d2h_rd_JOIN_INNER_TD_3608.resize(1);
    events_JOIN_INNER_TD_3608[0].resize(1);
    events_JOIN_INNER_TD_3608[1].resize(hpTimes_join);
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_3608;
    std::vector<cl::Event> prev_events_grp_JOIN_INNER_TD_3608;
    std::vector<cl::Event> events_h2d_wr_JOIN_LEFTSEMI_TD_3435;
    std::vector<cl::Event> events_d2h_rd_JOIN_LEFTSEMI_TD_3435;
    std::vector<cl::Event> events_JOIN_LEFTSEMI_TD_3435[2];
    events_h2d_wr_JOIN_LEFTSEMI_TD_3435.resize(1);
    events_d2h_rd_JOIN_LEFTSEMI_TD_3435.resize(1);
    events_JOIN_LEFTSEMI_TD_3435[0].resize(2);
    events_JOIN_LEFTSEMI_TD_3435[1].resize(hpTimes_join);
    std::vector<cl::Event> events_grp_JOIN_LEFTSEMI_TD_3435;
    std::vector<cl::Event> prev_events_grp_JOIN_LEFTSEMI_TD_3435;
    std::vector<cl::Event> events_h2d_wr_JOIN_LEFTSEMI_TD_4287;
    std::vector<cl::Event> events_d2h_rd_JOIN_LEFTSEMI_TD_4287;
    std::vector<cl::Event> events_JOIN_LEFTSEMI_TD_4287[2];
    events_h2d_wr_JOIN_LEFTSEMI_TD_4287.resize(1);
    events_d2h_rd_JOIN_LEFTSEMI_TD_4287.resize(1);
    events_JOIN_LEFTSEMI_TD_4287[0].resize(2);
    events_JOIN_LEFTSEMI_TD_4287[1].resize(hpTimes_join);
    std::vector<cl::Event> events_grp_JOIN_LEFTSEMI_TD_4287;
    std::vector<cl::Event> prev_events_grp_JOIN_LEFTSEMI_TD_4287;
    std::vector<cl::Event> events_h2d_wr_Aggregate_TD_6132;
    std::vector<cl::Event> events_d2h_rd_Aggregate_TD_6132;
    std::vector<cl::Event> events_Aggregate_TD_6132[2];
    events_h2d_wr_Aggregate_TD_6132.resize(1);
    events_d2h_rd_Aggregate_TD_6132.resize(1);
    events_Aggregate_TD_6132[0].resize(1);
    events_Aggregate_TD_6132[1].resize(hpTimes_aggr);
    std::vector<cl::Event> events_grp_Aggregate_TD_6132;
    std::vector<cl::Event> prev_events_grp_Aggregate_TD_6132;
    // **************************** Operations ************************** // 
    struct timeval tv_r_s, tv_r_e; 
    gettimeofday(&tv_r_s, 0); 

    struct timeval tv_r_Aggregate_6_826_s, tv_r_Aggregate_6_826_e;
    gettimeofday(&tv_r_Aggregate_6_826_s, 0);
    trans_Aggregate_TD_6132.host2dev(0, &(prev_events_grp_Aggregate_TD_6132), &(events_h2d_wr_Aggregate_TD_6132[0]));
    events_grp_Aggregate_TD_6132.push_back(events_h2d_wr_Aggregate_TD_6132[0]);
    krnl_Aggregate_TD_6132_part.run(0, &(events_grp_Aggregate_TD_6132), &(events_Aggregate_TD_6132[0][0]));
    for (int i(0); i < hpTimes_aggr; ++i) {
        krnl_Aggregate_TD_6132[i].run(0, &(events_Aggregate_TD_6132[0]), &(events_Aggregate_TD_6132[1][i]));
    }
    
    for (int i(0); i < hpTimes_aggr; ++i) {
        trans_Aggregate_TD_6132_out.add(&(tbl_Aggregate_TD_6132_output_partition_array[i]));
    }
    trans_Aggregate_TD_6132_out.dev2host(0, &(events_Aggregate_TD_6132[1]), &(events_d2h_rd_Aggregate_TD_6132[0]));
    q_a.flush();
    q_a.finish();
    
    SW_Aggregate_TD_6132_consolidate(tbl_Aggregate_TD_6132_output_partition_array, tbl_Aggregate_TD_6132_output, hpTimes_aggr);
    gettimeofday(&tv_r_Aggregate_6_826_e, 0);

    struct timeval tv_r_Filter_5_527_s, tv_r_Filter_5_527_e;
    gettimeofday(&tv_r_Filter_5_527_s, 0);
    SW_Filter_TD_5586(tbl_Aggregate_TD_6132_output, tbl_Filter_TD_5586_output);
    gettimeofday(&tv_r_Filter_5_527_e, 0);

    struct timeval tv_r_Filter_4_888_s, tv_r_Filter_4_888_e;
    gettimeofday(&tv_r_Filter_4_888_s, 0);
    SW_Filter_TD_4654(tbl_Aggregate_TD_6132_output, tbl_Filter_TD_4654_output);
    gettimeofday(&tv_r_Filter_4_888_e, 0);

    struct timeval tv_r_JOIN_LEFTSEMI_4_611_s, tv_r_JOIN_LEFTSEMI_4_611_e;
    gettimeofday(&tv_r_JOIN_LEFTSEMI_4_611_s, 0);
    tbl_Filter_TD_5586_output.tableToCol();
    tbl_SerializeFromObject_TD_5226_input.tableToCol();
    krnl_JOIN_LEFTSEMI_TD_4287_build.updateMeta(tbl_Filter_TD_5586_output.getNumRow(), 3);
    krnl_JOIN_LEFTSEMI_TD_4287_probe.updateMeta(tbl_SerializeFromObject_TD_5226_input.getNumRow(), 3);
    trans_JOIN_LEFTSEMI_TD_4287.add(&(tbl_Filter_TD_5586_output));
    trans_JOIN_LEFTSEMI_TD_4287.host2dev(0, &(prev_events_grp_JOIN_LEFTSEMI_TD_4287), &(events_h2d_wr_JOIN_LEFTSEMI_TD_4287[0]));
    events_grp_JOIN_LEFTSEMI_TD_4287.push_back(events_h2d_wr_JOIN_LEFTSEMI_TD_4287[0]);
    krnl_JOIN_LEFTSEMI_TD_4287_part_left.run(0, &(events_grp_JOIN_LEFTSEMI_TD_4287), &(events_JOIN_LEFTSEMI_TD_4287[0][0]));
    krnl_JOIN_LEFTSEMI_TD_4287_part_right.run(0, &(events_grp_JOIN_LEFTSEMI_TD_4287), &(events_JOIN_LEFTSEMI_TD_4287[0][1]));
    for (int i(0); i < hpTimes_join; ++i) {
        krnl_JOIN_LEFTSEMI_TD_4287[i].run(0, &(events_JOIN_LEFTSEMI_TD_4287[0]), &(events_JOIN_LEFTSEMI_TD_4287[1][i]));
    }
    std::vector<cl::Event> events_grp_JOIN_LEFTSEMI_TD_4287_build_done;
    events_grp_JOIN_LEFTSEMI_TD_4287_build_done.push_back(events_JOIN_LEFTSEMI_TD_4287[0]);
    krnl_JOIN_LEFTSEMI_TD_4287_part_left.run(0, &(events_grp_JOIN_LEFTSEMI_TD_4287), &(events_JOIN_LEFTSEMI_TD_4287[0][0]));
    krnl_JOIN_LEFTSEMI_TD_4287_part_right.run(0, &(events_grp_JOIN_LEFTSEMI_TD_4287), &(events_JOIN_LEFTSEMI_TD_4287[0][1]));
    for (int i(0); i < hpTimes_join; ++i) {
        krnl_JOIN_LEFTSEMI_TD_4287[i].run(0, &(events_JOIN_LEFTSEMI_TD_4287[0]), &(events_JOIN_LEFTSEMI_TD_4287[1][i]));
    }
    std::vector<cl::Event> events_grp_JOIN_LEFTSEMI_TD_4287_probe_done;
    events_grp_JOIN_LEFTSEMI_TD_4287_probe_done.push_back(events_JOIN_LEFTSEMI_TD_4287[1]);
    for (int i(0); i < hpTimes_join; ++i) {
        trans_JOIN_LEFTSEMI_TD_4287_out.add(&(tbl_JOIN_LEFTSEMI_TD_4287_output_partition_array[i]));
    }
    trans_JOIN_LEFTSEMI_TD_4287_out.dev2host(0, &(events_JOIN_LEFTSEMI_TD_4287[1]), &(events_d2h_rd_JOIN_LEFTSEMI_TD_4287[0]));
    q_h.flush();
    q_h.finish();
    tbl_JOIN_LEFTSEMI_TD_4287_output.setNumRow((cfg_JOIN_LEFTSEMI_TD_4287_cmds_probe.meta_out)->getColLen());
    tbl_JOIN_LEFTSEMI_TD_4287_output.colToTable();
    SW_Consolidated_JOIN_LEFTSEMI_TD_4287_output(tbl_Filter_TD_5586_output, tbl_SerializeFromObject_TD_5226_input, tbl_JOIN_LEFTSEMI_TD_4287_output, tbl_JOIN_LEFTSEMI_TD_4287_consolidated_output);
    gettimeofday(&tv_r_JOIN_LEFTSEMI_4_611_e, 0);

    struct timeval tv_r_JOIN_LEFTSEMI_3_624_s, tv_r_JOIN_LEFTSEMI_3_624_e;
    gettimeofday(&tv_r_JOIN_LEFTSEMI_3_624_s, 0);
    tbl_Filter_TD_4654_output.tableToCol();
    tbl_SerializeFromObject_TD_4766_input.tableToCol();
    krnl_JOIN_LEFTSEMI_TD_3435_build.updateMeta(tbl_Filter_TD_4654_output.getNumRow(), 3);
    krnl_JOIN_LEFTSEMI_TD_3435_probe.updateMeta(tbl_SerializeFromObject_TD_4766_input.getNumRow(), 3);
    trans_JOIN_LEFTSEMI_TD_3435.add(&(tbl_Filter_TD_4654_output));
    trans_JOIN_LEFTSEMI_TD_3435.host2dev(0, &(prev_events_grp_JOIN_LEFTSEMI_TD_3435), &(events_h2d_wr_JOIN_LEFTSEMI_TD_3435[0]));
    events_grp_JOIN_LEFTSEMI_TD_3435.push_back(events_h2d_wr_JOIN_LEFTSEMI_TD_3435[0]);
    krnl_JOIN_LEFTSEMI_TD_3435_part_left.run(0, &(events_grp_JOIN_LEFTSEMI_TD_3435), &(events_JOIN_LEFTSEMI_TD_3435[0][0]));
    krnl_JOIN_LEFTSEMI_TD_3435_part_right.run(0, &(events_grp_JOIN_LEFTSEMI_TD_3435), &(events_JOIN_LEFTSEMI_TD_3435[0][1]));
    for (int i(0); i < hpTimes_join; ++i) {
        krnl_JOIN_LEFTSEMI_TD_3435[i].run(0, &(events_JOIN_LEFTSEMI_TD_3435[0]), &(events_JOIN_LEFTSEMI_TD_3435[1][i]));
    }
    std::vector<cl::Event> events_grp_JOIN_LEFTSEMI_TD_3435_build_done;
    events_grp_JOIN_LEFTSEMI_TD_3435_build_done.push_back(events_JOIN_LEFTSEMI_TD_3435[0]);
    krnl_JOIN_LEFTSEMI_TD_3435_part_left.run(0, &(events_grp_JOIN_LEFTSEMI_TD_3435), &(events_JOIN_LEFTSEMI_TD_3435[0][0]));
    krnl_JOIN_LEFTSEMI_TD_3435_part_right.run(0, &(events_grp_JOIN_LEFTSEMI_TD_3435), &(events_JOIN_LEFTSEMI_TD_3435[0][1]));
    for (int i(0); i < hpTimes_join; ++i) {
        krnl_JOIN_LEFTSEMI_TD_3435[i].run(0, &(events_JOIN_LEFTSEMI_TD_3435[0]), &(events_JOIN_LEFTSEMI_TD_3435[1][i]));
    }
    std::vector<cl::Event> events_grp_JOIN_LEFTSEMI_TD_3435_probe_done;
    events_grp_JOIN_LEFTSEMI_TD_3435_probe_done.push_back(events_JOIN_LEFTSEMI_TD_3435[1]);
    for (int i(0); i < hpTimes_join; ++i) {
        trans_JOIN_LEFTSEMI_TD_3435_out.add(&(tbl_JOIN_LEFTSEMI_TD_3435_output_partition_array[i]));
    }
    trans_JOIN_LEFTSEMI_TD_3435_out.dev2host(0, &(events_JOIN_LEFTSEMI_TD_3435[1]), &(events_d2h_rd_JOIN_LEFTSEMI_TD_3435[0]));
    q_h.flush();
    q_h.finish();
    tbl_JOIN_LEFTSEMI_TD_3435_output.setNumRow((cfg_JOIN_LEFTSEMI_TD_3435_cmds_probe.meta_out)->getColLen());
    tbl_JOIN_LEFTSEMI_TD_3435_output.colToTable();
    SW_Consolidated_JOIN_LEFTSEMI_TD_3435_output(tbl_Filter_TD_4654_output, tbl_SerializeFromObject_TD_4766_input, tbl_JOIN_LEFTSEMI_TD_3435_output, tbl_JOIN_LEFTSEMI_TD_3435_consolidated_output);
    gettimeofday(&tv_r_JOIN_LEFTSEMI_3_624_e, 0);

    struct timeval tv_r_JOIN_INNER_3_209_s, tv_r_JOIN_INNER_3_209_e;
    gettimeofday(&tv_r_JOIN_INNER_3_209_s, 0);
    prev_events_grp_JOIN_INNER_TD_3608.push_back(events_h2d_wr_JOIN_LEFTSEMI_TD_4287[0]);
    tbl_SerializeFromObject_TD_4246_input_stringRowIDSubstitute.tableToCol();
    tbl_JOIN_LEFTSEMI_TD_4287_consolidated_output.tableToCol();
    krnl_JOIN_INNER_TD_3608_build.updateMeta(tbl_SerializeFromObject_TD_4246_input_stringRowIDSubstitute.getNumRow(), 3);
    krnl_JOIN_INNER_TD_3608_probe.updateMeta(tbl_JOIN_LEFTSEMI_TD_4287_consolidated_output.getNumRow(), 3);
    trans_JOIN_INNER_TD_3608.host2dev(0, &(prev_events_grp_JOIN_INNER_TD_3608), &(events_h2d_wr_JOIN_INNER_TD_3608[0]));
    events_grp_JOIN_INNER_TD_3608.push_back(events_h2d_wr_JOIN_INNER_TD_3608[0]);
    events_grp_JOIN_INNER_TD_3608.insert(std::end(events_grp_JOIN_INNER_TD_3608), std::begin(events_JOIN_LEFTSEMI_TD_4287[0]), std::end(events_JOIN_LEFTSEMI_TD_4287[0]));
    events_grp_JOIN_INNER_TD_3608.insert(std::end(events_grp_JOIN_INNER_TD_3608), std::begin(events_JOIN_LEFTSEMI_TD_4287[1]), std::end(events_JOIN_LEFTSEMI_TD_4287[1]));
    krnl_JOIN_INNER_TD_3608_part_left.run(0, &(events_grp_JOIN_INNER_TD_3608), &(events_JOIN_INNER_TD_3608[0][0]));
    for (int i(0); i < hpTimes_join; ++i) {
        krnl_JOIN_INNER_TD_3608[i].run(0, &(events_JOIN_INNER_TD_3608[0]), &(events_JOIN_INNER_TD_3608[1][i]));
    }
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_3608_build_done;
    events_grp_JOIN_INNER_TD_3608_build_done.push_back(events_JOIN_INNER_TD_3608[0]);
    krnl_JOIN_INNER_TD_3608_part_left.run(0, &(events_grp_JOIN_INNER_TD_3608), &(events_JOIN_INNER_TD_3608[0][0]));
    for (int i(0); i < hpTimes_join; ++i) {
        krnl_JOIN_INNER_TD_3608[i].run(0, &(events_JOIN_INNER_TD_3608[0]), &(events_JOIN_INNER_TD_3608[1][i]));
    }
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_3608_probe_done;
    events_grp_JOIN_INNER_TD_3608_probe_done.push_back(events_JOIN_INNER_TD_3608[1]);
    for (int i(0); i < hpTimes_join; ++i) {
        trans_JOIN_INNER_TD_3608_out.add(&(tbl_JOIN_INNER_TD_3608_output_partition_array[i]));
    }
    trans_JOIN_INNER_TD_3608_out.dev2host(0, &(events_JOIN_INNER_TD_3608[1]), &(events_d2h_rd_JOIN_INNER_TD_3608[0]));
    q_h.flush();
    q_h.finish();
    tbl_JOIN_INNER_TD_3608_output.setNumRow((cfg_JOIN_INNER_TD_3608_cmds_probe.meta_out)->getColLen());
    tbl_JOIN_INNER_TD_3608_output.colToTable();
    SW_Consolidated_JOIN_INNER_TD_3608_output(tbl_SerializeFromObject_TD_4246_input_stringRowIDSubstitute, tbl_JOIN_LEFTSEMI_TD_4287_consolidated_output, tbl_JOIN_INNER_TD_3608_output, tbl_JOIN_INNER_TD_3608_consolidated_output);
    gettimeofday(&tv_r_JOIN_INNER_3_209_e, 0);

    struct timeval tv_r_JOIN_INNER_2_957_s, tv_r_JOIN_INNER_2_957_e;
    gettimeofday(&tv_r_JOIN_INNER_2_957_s, 0);
    prev_events_grp_JOIN_INNER_TD_2592.push_back(events_h2d_wr_JOIN_INNER_TD_3608[0]);
    prev_events_grp_JOIN_INNER_TD_2592.push_back(events_h2d_wr_JOIN_LEFTSEMI_TD_3435[0]);
    tbl_JOIN_INNER_TD_3608_consolidated_output.tableToCol();
    tbl_JOIN_LEFTSEMI_TD_3435_consolidated_output.tableToCol();
    krnl_JOIN_INNER_TD_2592_build.updateMeta(tbl_JOIN_INNER_TD_3608_consolidated_output.getNumRow(), 3);
    krnl_JOIN_INNER_TD_2592_probe.updateMeta(tbl_JOIN_LEFTSEMI_TD_3435_consolidated_output.getNumRow(), 3);
    trans_JOIN_INNER_TD_2592.host2dev(0, &(prev_events_grp_JOIN_INNER_TD_2592), &(events_h2d_wr_JOIN_INNER_TD_2592[0]));
    events_grp_JOIN_INNER_TD_2592.push_back(events_h2d_wr_JOIN_INNER_TD_2592[0]);
    events_grp_JOIN_INNER_TD_2592.insert(std::end(events_grp_JOIN_INNER_TD_2592), std::begin(events_JOIN_INNER_TD_3608[0]), std::end(events_JOIN_INNER_TD_3608[0]));
    events_grp_JOIN_INNER_TD_2592.insert(std::end(events_grp_JOIN_INNER_TD_2592), std::begin(events_JOIN_INNER_TD_3608[1]), std::end(events_JOIN_INNER_TD_3608[1]));
    events_grp_JOIN_INNER_TD_2592.insert(std::end(events_grp_JOIN_INNER_TD_2592), std::begin(events_JOIN_LEFTSEMI_TD_3435[0]), std::end(events_JOIN_LEFTSEMI_TD_3435[0]));
    events_grp_JOIN_INNER_TD_2592.insert(std::end(events_grp_JOIN_INNER_TD_2592), std::begin(events_JOIN_LEFTSEMI_TD_3435[1]), std::end(events_JOIN_LEFTSEMI_TD_3435[1]));
    for (int i(0); i < hpTimes_join; ++i) {
        krnl_JOIN_INNER_TD_2592[i].run(0, &(events_grp_JOIN_INNER_TD_2592), &(events_JOIN_INNER_TD_2592[1][i]));
    }
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_2592_build_done;
    events_grp_JOIN_INNER_TD_2592_build_done.push_back(events_JOIN_INNER_TD_2592[0]);
    for (int i(0); i < hpTimes_join; ++i) {
        krnl_JOIN_INNER_TD_2592[i].run(0, &(events_grp_JOIN_INNER_TD_2592), &(events_JOIN_INNER_TD_2592[1][i]));
    }
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_2592_probe_done;
    events_grp_JOIN_INNER_TD_2592_probe_done.push_back(events_JOIN_INNER_TD_2592[1]);
    for (int i(0); i < hpTimes_join; ++i) {
        trans_JOIN_INNER_TD_2592_out.add(&(tbl_JOIN_INNER_TD_2592_output_partition_array[i]));
    }
    trans_JOIN_INNER_TD_2592_out.dev2host(0, &(events_JOIN_INNER_TD_2592[1]), &(events_d2h_rd_JOIN_INNER_TD_2592[0]));
    q_h.flush();
    q_h.finish();
    tbl_JOIN_INNER_TD_2592_output.setNumRow((cfg_JOIN_INNER_TD_2592_cmds_probe.meta_out)->getColLen());
    tbl_JOIN_INNER_TD_2592_output.colToTable();
    SW_Consolidated_JOIN_INNER_TD_2592_output(tbl_JOIN_INNER_TD_3608_consolidated_output, tbl_JOIN_LEFTSEMI_TD_3435_consolidated_output, tbl_JOIN_INNER_TD_2592_output, tbl_JOIN_INNER_TD_2592_consolidated_output);
    gettimeofday(&tv_r_JOIN_INNER_2_957_e, 0);

    struct timeval tv_r_Aggregate_1_307_s, tv_r_Aggregate_1_307_e;
    gettimeofday(&tv_r_Aggregate_1_307_s, 0);
    SW_Aggregate_TD_1710(tbl_JOIN_INNER_TD_2592_consolidated_output_partition_array, tbl_SerializeFromObject_TD_4246_input, tbl_Aggregate_TD_1710_output, hpTimes_join);
    gettimeofday(&tv_r_Aggregate_1_307_e, 0);

    struct timeval tv_r_Sort_0_654_s, tv_r_Sort_0_654_e;
    gettimeofday(&tv_r_Sort_0_654_s, 0);
    SW_Sort_TD_065(tbl_Aggregate_TD_1710_output, tbl_Sort_TD_065_output);
    gettimeofday(&tv_r_Sort_0_654_e, 0);

std::cout << "CPU TIME: " << (tvdiff(&tv_r_Filter_5_527_s, &tv_r_Filter_5_527_e) + 
tvdiff(&tv_r_Filter_4_888_s, &tv_r_Filter_4_888_e) + 
tvdiff(&tv_r_Aggregate_1_307_s, &tv_r_Aggregate_1_307_e) + 
tvdiff(&tv_r_Sort_0_654_s, &tv_r_Sort_0_654_e) + 
1) / 1000.0 << std::endl;

std::cout << "FPGA TIME: " << (tvdiff(&tv_r_Aggregate_6_826_s, &tv_r_Aggregate_6_826_e) + 
tvdiff(&tv_r_JOIN_LEFTSEMI_4_611_s, &tv_r_JOIN_LEFTSEMI_4_611_e) + 
tvdiff(&tv_r_JOIN_LEFTSEMI_3_624_s, &tv_r_JOIN_LEFTSEMI_3_624_e) + 
tvdiff(&tv_r_JOIN_INNER_3_209_s, &tv_r_JOIN_INNER_3_209_e) + 
tvdiff(&tv_r_JOIN_INNER_2_957_s, &tv_r_JOIN_INNER_2_957_e) + 
1) / 1000.0 << std::endl;

    gettimeofday(&tv_r_e, 0); 
    // **************************** Print Execution Time ************************** // 
    std::cout << std::endl; 
    std::cout << "Aggregate_6: " << tvdiff(&tv_r_Aggregate_6_826_s, &tv_r_Aggregate_6_826_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer(l_orderkey#77, sum(l_quantity#81) AS sum(l_quantity#81)#1711L) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_7564_input.getNumRow() << " -> tbl_SerializeFromObject_TD_7564_input" 
     << std::endl << "      #COL: 2: " << "ListBuffer(l_orderkey#77, l_quantity#81)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_6132_output.getNumRow() << " -> tbl_Aggregate_TD_6132_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(l_orderkey#77, sum(l_quantity#81)#1711L)" 
     << std::endl; 

    std::cout << "Filter_5: " << tvdiff(&tv_r_Filter_5_527_s, &tv_r_Filter_5_527_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer((isnotnull(sum(l_quantity#81)#1711L) AND (sum(l_quantity#81)#1711L > 312))) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_6132_output.getNumRow() << " -> tbl_Aggregate_TD_6132_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(l_orderkey#77, sum(l_quantity#81)#1711L)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_5586_output.getNumRow() << " -> tbl_Filter_TD_5586_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(l_orderkey#77)" 
     << std::endl; 

    std::cout << "Filter_4: " << tvdiff(&tv_r_Filter_4_888_s, &tv_r_Filter_4_888_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer((isnotnull(sum(l_quantity#81)#1711L) AND (sum(l_quantity#81)#1711L > 312))) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_6132_output.getNumRow() << " -> tbl_Aggregate_TD_6132_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(l_orderkey#77, sum(l_quantity#81)#1711L)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_4654_output.getNumRow() << " -> tbl_Filter_TD_4654_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(l_orderkey#77#1788)" 
     << std::endl; 

    std::cout << "JOIN_LEFTSEMI_4: " << tvdiff(&tv_r_JOIN_LEFTSEMI_4_611_s, &tv_r_JOIN_LEFTSEMI_4_611_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer((o_orderkey#207 = l_orderkey#77)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_5226_input.getNumRow() << " -> tbl_SerializeFromObject_TD_5226_input" 
     << std::endl << "      #COL: 4: " << "ListBuffer(o_orderkey#207, o_custkey#208, o_totalprice#210, o_orderdate#211)" 
     << std::endl << "      #ROW: " << tbl_Filter_TD_5586_output.getNumRow() << " -> tbl_Filter_TD_5586_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(l_orderkey#77)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_LEFTSEMI_TD_4287_consolidated_output.getNumRow() << " -> tbl_JOIN_LEFTSEMI_TD_4287_consolidated_output" 
     << std::endl << "      #COL: 4: " << "ListBuffer(o_orderkey#207, o_custkey#208, o_totalprice#210, o_orderdate#211)" 
     << std::endl; 

    std::cout << "JOIN_LEFTSEMI_3: " << tvdiff(&tv_r_JOIN_LEFTSEMI_3_624_s, &tv_r_JOIN_LEFTSEMI_3_624_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer((l_orderkey#77 = l_orderkey#77#1788)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_4766_input.getNumRow() << " -> tbl_SerializeFromObject_TD_4766_input" 
     << std::endl << "      #COL: 2: " << "ListBuffer(l_orderkey#77, l_quantity#81)" 
     << std::endl << "      #ROW: " << tbl_Filter_TD_4654_output.getNumRow() << " -> tbl_Filter_TD_4654_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(l_orderkey#77#1788)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_LEFTSEMI_TD_3435_consolidated_output.getNumRow() << " -> tbl_JOIN_LEFTSEMI_TD_3435_consolidated_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(l_orderkey#77, l_quantity#81)" 
     << std::endl; 

    std::cout << "JOIN_INNER_3: " << tvdiff(&tv_r_JOIN_INNER_3_209_s, &tv_r_JOIN_INNER_3_209_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer((c_custkey#22 = o_custkey#208)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_4246_input_stringRowIDSubstitute.getNumRow() << " -> tbl_SerializeFromObject_TD_4246_input_stringRowIDSubstitute" 
     << std::endl << "      #COL: 2: " << "ListBuffer(c_custkey#22, c_name#23)" 
     << std::endl << "      #ROW: " << tbl_JOIN_LEFTSEMI_TD_4287_consolidated_output.getNumRow() << " -> tbl_JOIN_LEFTSEMI_TD_4287_consolidated_output" 
     << std::endl << "      #COL: 4: " << "ListBuffer(o_orderkey#207, o_custkey#208, o_totalprice#210, o_orderdate#211)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_3608_consolidated_output.getNumRow() << " -> tbl_JOIN_INNER_TD_3608_consolidated_output" 
     << std::endl << "      #COL: 5: " << "ListBuffer(c_custkey#22, c_name#23, o_orderkey#207, o_totalprice#210, o_orderdate#211)" 
     << std::endl; 

    std::cout << "JOIN_INNER_2: " << tvdiff(&tv_r_JOIN_INNER_2_957_s, &tv_r_JOIN_INNER_2_957_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer((o_orderkey#207 = l_orderkey#77)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_3608_consolidated_output.getNumRow() << " -> tbl_JOIN_INNER_TD_3608_consolidated_output" 
     << std::endl << "      #COL: 5: " << "ListBuffer(c_custkey#22, c_name#23, o_orderkey#207, o_totalprice#210, o_orderdate#211)" 
     << std::endl << "      #ROW: " << tbl_JOIN_LEFTSEMI_TD_3435_consolidated_output.getNumRow() << " -> tbl_JOIN_LEFTSEMI_TD_3435_consolidated_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(l_orderkey#77, l_quantity#81)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_2592_consolidated_output.getNumRow() << " -> tbl_JOIN_INNER_TD_2592_consolidated_output" 
     << std::endl << "      #COL: 6: " << "ListBuffer(c_custkey#22, c_name#23, o_orderkey#207, o_totalprice#210, o_orderdate#211, l_quantity#81)" 
     << std::endl; 

    std::cout << "Aggregate_1: " << tvdiff(&tv_r_Aggregate_1_307_s, &tv_r_Aggregate_1_307_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(c_name#23, c_custkey#22, o_orderkey#207, o_orderdate#211, o_totalprice#210, sum(l_quantity#81) AS sum(l_quantity)#1714L) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_2592_consolidated_output.getNumRow() << " -> tbl_JOIN_INNER_TD_2592_consolidated_output" 
     << std::endl << "      #COL: 6: " << "ListBuffer(c_custkey#22, c_name#23, o_orderkey#207, o_totalprice#210, o_orderdate#211, l_quantity#81)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_1710_output.getNumRow() << " -> tbl_Aggregate_TD_1710_output" 
     << std::endl << "      #COL: 6: " << "ListBuffer(c_name#23, c_custkey#22, o_orderkey#207, o_orderdate#211, o_totalprice#210, sum(l_quantity)#1714L)" 
     << std::endl; 

    std::cout << "Sort_0: " << tvdiff(&tv_r_Sort_0_654_s, &tv_r_Sort_0_654_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(o_totalprice#210 DESC NULLS LAST, o_orderdate#211 ASC NULLS FIRST) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_1710_output.getNumRow() << " -> tbl_Aggregate_TD_1710_output" 
     << std::endl << "      #COL: 6: " << "ListBuffer(c_name#23, c_custkey#22, o_orderkey#207, o_orderdate#211, o_totalprice#210, sum(l_quantity)#1714L)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Sort_TD_065_output.getNumRow() << " -> tbl_Sort_TD_065_output" 
     << std::endl << "      #COL: 6: " << "ListBuffer(c_name#23, c_custkey#22, o_orderkey#207, o_orderdate#211, o_totalprice#210, sum(l_quantity)#1714L)" 
     << std::endl; 

    std::cout << std::endl << " Total execution time: " << tvdiff(&tv_r_s, &tv_r_e) / 1000 << " ms"; 

    std::cout << std::endl << " Spark elapsed time: " << 53.51238 * 1000 << "ms" << std::endl; 
    return 0; 
}
