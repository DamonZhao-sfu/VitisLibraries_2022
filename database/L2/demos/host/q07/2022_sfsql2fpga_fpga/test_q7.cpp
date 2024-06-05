// number of overlays (w/o fusion): 4 
// number of overlays (w/ fusion): 4 
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

#include "cfgFunc_q7.hpp" 
#include "q7.hpp" 

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
    std::cout << "NOTE:running query #7\n."; 
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
    Table tbl_Sort_TD_0788_output("tbl_Sort_TD_0788_output", 183000000, 4, "");
    tbl_Sort_TD_0788_output.allocateHost();
    Table tbl_Aggregate_TD_1275_output("tbl_Aggregate_TD_1275_output", 183000000, 4, "");
    tbl_Aggregate_TD_1275_output.allocateHost();
    Table tbl_Project_TD_296_output("tbl_Project_TD_296_output", 183000000, 4, "");
    tbl_Project_TD_296_output.allocateHost();
    Table tbl_JOIN_INNER_TD_3929_output("tbl_JOIN_INNER_TD_3929_output", 183000000, 6, "");
    tbl_JOIN_INNER_TD_3929_output.allocateHost();
    Table tbl_JOIN_INNER_TD_4221_output("tbl_JOIN_INNER_TD_4221_output", 153500, 4, "");
    tbl_JOIN_INNER_TD_4221_output.allocateHost(1.2, hpTimes_join);
    Table tbl_JOIN_INNER_TD_4221_consolidated_output("tbl_JOIN_INNER_TD_4221_consolidated_output", 153500, 5, "");
    tbl_JOIN_INNER_TD_4221_consolidated_output.selectOverlayVersion(1);
    tbl_JOIN_INNER_TD_4221_consolidated_output.allocateHost();
    Table tbl_Filter_TD_4173_output("tbl_Filter_TD_4173_output", 183000000, 2, "");
    tbl_Filter_TD_4173_output.allocateHost();
    Table tbl_JOIN_INNER_TD_5296_output("tbl_JOIN_INNER_TD_5296_output", 153500, 4, "");
    tbl_JOIN_INNER_TD_5296_output.allocateHost(1.2, hpTimes_join);
    Table tbl_JOIN_INNER_TD_5296_consolidated_output("tbl_JOIN_INNER_TD_5296_consolidated_output", 153500, 6, "");
    tbl_JOIN_INNER_TD_5296_consolidated_output.selectOverlayVersion(1);
    tbl_JOIN_INNER_TD_5296_consolidated_output.allocateHost();
    Table tbl_SerializeFromObject_TD_592_input;
    tbl_SerializeFromObject_TD_592_input = Table("customer", customer_n, 2, in_dir);
    tbl_SerializeFromObject_TD_592_input.selectOverlayVersion(1);
    tbl_SerializeFromObject_TD_592_input.addCol("c_custkey", 4);
    tbl_SerializeFromObject_TD_592_input.addCol("c_nationkey", 4);
    tbl_SerializeFromObject_TD_592_input.allocateHost();
    tbl_SerializeFromObject_TD_592_input.loadHost();
    Table tbl_SerializeFromObject_TD_592_input_partition("tbl_SerializeFromObject_TD_592_input_partition", customer_n, 2, "");
    tbl_SerializeFromObject_TD_592_input_partition.allocateHost(1.2, hpTimes_join);
    tbl_SerializeFromObject_TD_592_input_partition.allocateDevBuffer(context_h, 32);
    Table tbl_SerializeFromObject_TD_592_input_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_SerializeFromObject_TD_592_input_partition_array[i] = tbl_SerializeFromObject_TD_592_input_partition.createSubTable(i);
    }
    Table tbl_SerializeFromObject_TD_5132_input;
    tbl_SerializeFromObject_TD_5132_input = Table("nation", nation_n, 2, in_dir);
    tbl_SerializeFromObject_TD_5132_input.addCol("n_nationkey", 4);
    tbl_SerializeFromObject_TD_5132_input.addCol("n_name", TPCH_READ_NATION_LEN+1);
    tbl_SerializeFromObject_TD_5132_input.allocateHost();
    tbl_SerializeFromObject_TD_5132_input.loadHost();
    Table tbl_JOIN_INNER_TD_6477_output("tbl_JOIN_INNER_TD_6477_output", 153500, 4, "");
    tbl_JOIN_INNER_TD_6477_output.allocateHost(1.2, hpTimes_join);
    Table tbl_JOIN_INNER_TD_6477_consolidated_output("tbl_JOIN_INNER_TD_6477_consolidated_output", 153500, 6, "");
    tbl_JOIN_INNER_TD_6477_consolidated_output.selectOverlayVersion(1);
    tbl_JOIN_INNER_TD_6477_consolidated_output.allocateHost();
    Table tbl_SerializeFromObject_TD_661_input;
    tbl_SerializeFromObject_TD_661_input = Table("order", order_n, 2, in_dir);
    tbl_SerializeFromObject_TD_661_input.selectOverlayVersion(1);
    tbl_SerializeFromObject_TD_661_input.addCol("o_orderkey", 4);
    tbl_SerializeFromObject_TD_661_input.addCol("o_custkey", 4);
    tbl_SerializeFromObject_TD_661_input.allocateHost();
    tbl_SerializeFromObject_TD_661_input.loadHost();
    Table tbl_SerializeFromObject_TD_661_input_partition("tbl_SerializeFromObject_TD_661_input_partition", order_n, 2, "");
    tbl_SerializeFromObject_TD_661_input_partition.allocateHost(1.2, hpTimes_join);
    tbl_SerializeFromObject_TD_661_input_partition.allocateDevBuffer(context_h, 32);
    Table tbl_SerializeFromObject_TD_661_input_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_SerializeFromObject_TD_661_input_partition_array[i] = tbl_SerializeFromObject_TD_661_input_partition.createSubTable(i);
    }
    Table tbl_JOIN_INNER_TD_7482_output("tbl_JOIN_INNER_TD_7482_output", 1829000, 4, "");
    tbl_JOIN_INNER_TD_7482_output.allocateHost(1.2, hpTimes_join);
    Table tbl_JOIN_INNER_TD_7482_consolidated_output("tbl_JOIN_INNER_TD_7482_consolidated_output", 1829000, 5, "");
    tbl_JOIN_INNER_TD_7482_consolidated_output.selectOverlayVersion(1);
    tbl_JOIN_INNER_TD_7482_consolidated_output.allocateHost();
    Table tbl_Filter_TD_5186_output("tbl_Filter_TD_5186_output", 183000000, 2, "");
    tbl_Filter_TD_5186_output.selectOverlayVersion(1);
    tbl_Filter_TD_5186_output.allocateHost();
    Table tbl_Filter_TD_5186_output_partition("tbl_Filter_TD_5186_output_partition", 183000000, 2, "");
    tbl_Filter_TD_5186_output_partition.allocateHost(1.2, hpTimes_join);
    tbl_Filter_TD_5186_output_partition.allocateDevBuffer(context_h, 32);
    Table tbl_Filter_TD_5186_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_Filter_TD_5186_output_partition_array[i] = tbl_Filter_TD_5186_output_partition.createSubTable(i);
    }
    Table tbl_SerializeFromObject_TD_8484_input;
    tbl_SerializeFromObject_TD_8484_input = Table("supplier", supplier_n, 2, in_dir);
    tbl_SerializeFromObject_TD_8484_input.selectOverlayVersion(1);
    tbl_SerializeFromObject_TD_8484_input.addCol("s_suppkey", 4);
    tbl_SerializeFromObject_TD_8484_input.addCol("s_nationkey", 4);
    tbl_SerializeFromObject_TD_8484_input.allocateHost();
    tbl_SerializeFromObject_TD_8484_input.loadHost();
    Table tbl_SerializeFromObject_TD_8484_input_partition("tbl_SerializeFromObject_TD_8484_input_partition", supplier_n, 2, "");
    tbl_SerializeFromObject_TD_8484_input_partition.allocateHost(1.2, hpTimes_join);
    tbl_SerializeFromObject_TD_8484_input_partition.allocateDevBuffer(context_h, 32);
    Table tbl_SerializeFromObject_TD_8484_input_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_SerializeFromObject_TD_8484_input_partition_array[i] = tbl_SerializeFromObject_TD_8484_input_partition.createSubTable(i);
    }
    Table tbl_Filter_TD_8404_output("tbl_Filter_TD_8404_output", 183000000, 5, "");
    tbl_Filter_TD_8404_output.selectOverlayVersion(1);
    tbl_Filter_TD_8404_output.allocateHost();
    Table tbl_Filter_TD_8404_output_partition("tbl_Filter_TD_8404_output_partition", 183000000, 5, "");
    tbl_Filter_TD_8404_output_partition.allocateHost(1.2, hpTimes_join);
    tbl_Filter_TD_8404_output_partition.allocateDevBuffer(context_h, 32);
    Table tbl_Filter_TD_8404_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_Filter_TD_8404_output_partition_array[i] = tbl_Filter_TD_8404_output_partition.createSubTable(i);
    }
    Table tbl_SerializeFromObject_TD_6778_input;
    tbl_SerializeFromObject_TD_6778_input = Table("nation", nation_n, 2, in_dir);
    tbl_SerializeFromObject_TD_6778_input.addCol("n_nationkey", 4);
    tbl_SerializeFromObject_TD_6778_input.addCol("n_name", TPCH_READ_NATION_LEN+1);
    tbl_SerializeFromObject_TD_6778_input.allocateHost();
    tbl_SerializeFromObject_TD_6778_input.loadHost();
    Table tbl_SerializeFromObject_TD_9366_input;
    tbl_SerializeFromObject_TD_9366_input = Table("lineitem", lineitem_n, 5, in_dir);
    tbl_SerializeFromObject_TD_9366_input.addCol("l_orderkey", 4);
    tbl_SerializeFromObject_TD_9366_input.addCol("l_suppkey", 4);
    tbl_SerializeFromObject_TD_9366_input.addCol("l_extendedprice", 4);
    tbl_SerializeFromObject_TD_9366_input.addCol("l_discount", 4);
    tbl_SerializeFromObject_TD_9366_input.addCol("l_shipdate", 4);
    tbl_SerializeFromObject_TD_9366_input.allocateHost();
    tbl_SerializeFromObject_TD_9366_input.loadHost();
    // ********************** Allocate Device Buffer ******************** // 
    tbl_JOIN_INNER_TD_4221_output.allocateDevBuffer(context_h, 32);
    Table tbl_JOIN_INNER_TD_4221_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_JOIN_INNER_TD_4221_output_partition_array[i] = tbl_JOIN_INNER_TD_4221_output.createSubTable(i);
    }
    tbl_JOIN_INNER_TD_5296_output.allocateDevBuffer(context_h, 32);
    Table tbl_JOIN_INNER_TD_5296_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_JOIN_INNER_TD_5296_output_partition_array[i] = tbl_JOIN_INNER_TD_5296_output.createSubTable(i);
    }
    tbl_SerializeFromObject_TD_592_input.allocateDevBuffer(context_h, 33);
    tbl_JOIN_INNER_TD_6477_output.allocateDevBuffer(context_h, 32);
    Table tbl_JOIN_INNER_TD_6477_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_JOIN_INNER_TD_6477_output_partition_array[i] = tbl_JOIN_INNER_TD_6477_output.createSubTable(i);
    }
    tbl_SerializeFromObject_TD_661_input.allocateDevBuffer(context_h, 33);
    tbl_JOIN_INNER_TD_7482_output.allocateDevBuffer(context_h, 32);
    Table tbl_JOIN_INNER_TD_7482_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_JOIN_INNER_TD_7482_output_partition_array[i] = tbl_JOIN_INNER_TD_7482_output.createSubTable(i);
    }
    tbl_Filter_TD_5186_output.allocateDevBuffer(context_h, 33);
    tbl_SerializeFromObject_TD_8484_input.allocateDevBuffer(context_h, 33);
    tbl_Filter_TD_8404_output.allocateDevBuffer(context_h, 33);
    // ****************************** Config **************************** // 
    
    xf::database::gqe::KernelCommand krn_cmd_build_935 = xf::database::gqe::KernelCommand();
    xf::database::gqe::KernelCommand krn_cmd_probe_935 = xf::database::gqe::KernelCommand();
    cfgCmd cfg_JOIN_INNER_TD_4221_cmds_build;
    cfg_JOIN_INNER_TD_4221_cmds_build.setup(1);
    cfg_JOIN_INNER_TD_4221_cmds_build.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_4221_gqe_join_build(krn_cmd_build_935);
    cfg_JOIN_INNER_TD_4221_cmds_build.cmd = krn_cmd_build_935.getConfigBits();
    get_meta_info_JOIN_INNER_TD_4221_gqe_join_build(cfg_JOIN_INNER_TD_4221_cmds_build.meta_in, tbl_SerializeFromObject_TD_592_input.nrow, 3);
    get_meta_info_JOIN_INNER_TD_4221_gqe_join_build(cfg_JOIN_INNER_TD_4221_cmds_build.meta_out, tbl_JOIN_INNER_TD_4221_output.nrow, 4);
    // cfg_JOIN_INNER_TD_4221_cmds_build.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_4221_cmds_probe;
    cfg_JOIN_INNER_TD_4221_cmds_probe.setup(1);
    cfg_JOIN_INNER_TD_4221_cmds_probe.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_4221_gqe_join_probe(krn_cmd_probe_935);
    cfg_JOIN_INNER_TD_4221_cmds_probe.cmd = krn_cmd_probe_935.getConfigBits();
    get_meta_info_JOIN_INNER_TD_4221_gqe_join_probe(cfg_JOIN_INNER_TD_4221_cmds_probe.meta_in, tbl_JOIN_INNER_TD_5296_consolidated_output.nrow, 3);
    get_meta_info_JOIN_INNER_TD_4221_gqe_join_probe(cfg_JOIN_INNER_TD_4221_cmds_probe.meta_out, tbl_JOIN_INNER_TD_4221_output.nrow, 3);
    // cfg_JOIN_INNER_TD_4221_cmds_probe.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_4221_cmds_part;
    cfg_JOIN_INNER_TD_4221_cmds_part.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_4221_gqe_join_part (cfg_JOIN_INNER_TD_4221_cmds_part.cmd);
    cfg_JOIN_INNER_TD_4221_cmds_part.allocateDevBuffer(context_h, 32);
    cfg_JOIN_INNER_TD_4221_cmds_build.input_key_columns = {0, -1, -1};
    cfg_JOIN_INNER_TD_4221_cmds_probe.input_key_columns = {4, -1, -1};
    cfg_JOIN_INNER_TD_4221_cmds_build.output_key_columns = {0, 1, 2, 3};
    cfg_JOIN_INNER_TD_4221_cmds_probe.output_key_columns = {0, 1, 2, 3};
    
    xf::database::gqe::KernelCommand krn_cmd_build_567 = xf::database::gqe::KernelCommand();
    xf::database::gqe::KernelCommand krn_cmd_probe_567 = xf::database::gqe::KernelCommand();
    cfgCmd cfg_JOIN_INNER_TD_5296_cmds_build;
    cfg_JOIN_INNER_TD_5296_cmds_build.setup(1);
    cfg_JOIN_INNER_TD_5296_cmds_build.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_5296_gqe_join_build(krn_cmd_build_567);
    cfg_JOIN_INNER_TD_5296_cmds_build.cmd = krn_cmd_build_567.getConfigBits();
    get_meta_info_JOIN_INNER_TD_5296_gqe_join_build(cfg_JOIN_INNER_TD_5296_cmds_build.meta_in, tbl_JOIN_INNER_TD_6477_consolidated_output.nrow, 3);
    get_meta_info_JOIN_INNER_TD_5296_gqe_join_build(cfg_JOIN_INNER_TD_5296_cmds_build.meta_out, tbl_JOIN_INNER_TD_5296_output.nrow, 4);
    // cfg_JOIN_INNER_TD_5296_cmds_build.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_5296_cmds_probe;
    cfg_JOIN_INNER_TD_5296_cmds_probe.setup(1);
    cfg_JOIN_INNER_TD_5296_cmds_probe.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_5296_gqe_join_probe(krn_cmd_probe_567);
    cfg_JOIN_INNER_TD_5296_cmds_probe.cmd = krn_cmd_probe_567.getConfigBits();
    get_meta_info_JOIN_INNER_TD_5296_gqe_join_probe(cfg_JOIN_INNER_TD_5296_cmds_probe.meta_in, tbl_SerializeFromObject_TD_661_input.nrow, 3);
    get_meta_info_JOIN_INNER_TD_5296_gqe_join_probe(cfg_JOIN_INNER_TD_5296_cmds_probe.meta_out, tbl_JOIN_INNER_TD_5296_output.nrow, 3);
    // cfg_JOIN_INNER_TD_5296_cmds_probe.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_5296_cmds_part;
    cfg_JOIN_INNER_TD_5296_cmds_part.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_5296_gqe_join_part (cfg_JOIN_INNER_TD_5296_cmds_part.cmd);
    cfg_JOIN_INNER_TD_5296_cmds_part.allocateDevBuffer(context_h, 32);
    cfg_JOIN_INNER_TD_5296_cmds_build.input_key_columns = {5, -1, -1};
    cfg_JOIN_INNER_TD_5296_cmds_probe.input_key_columns = {0, -1, -1};
    cfg_JOIN_INNER_TD_5296_cmds_build.output_key_columns = {0, 1, 2, 3};
    cfg_JOIN_INNER_TD_5296_cmds_probe.output_key_columns = {0, 1, 2, 3};
    
    xf::database::gqe::KernelCommand krn_cmd_build_700 = xf::database::gqe::KernelCommand();
    xf::database::gqe::KernelCommand krn_cmd_probe_700 = xf::database::gqe::KernelCommand();
    cfgCmd cfg_JOIN_INNER_TD_6477_cmds_build;
    cfg_JOIN_INNER_TD_6477_cmds_build.setup(1);
    cfg_JOIN_INNER_TD_6477_cmds_build.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_6477_gqe_join_build(krn_cmd_build_700);
    cfg_JOIN_INNER_TD_6477_cmds_build.cmd = krn_cmd_build_700.getConfigBits();
    get_meta_info_JOIN_INNER_TD_6477_gqe_join_build(cfg_JOIN_INNER_TD_6477_cmds_build.meta_in, tbl_Filter_TD_5186_output.nrow, 3);
    get_meta_info_JOIN_INNER_TD_6477_gqe_join_build(cfg_JOIN_INNER_TD_6477_cmds_build.meta_out, tbl_JOIN_INNER_TD_6477_output.nrow, 4);
    // cfg_JOIN_INNER_TD_6477_cmds_build.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_6477_cmds_probe;
    cfg_JOIN_INNER_TD_6477_cmds_probe.setup(1);
    cfg_JOIN_INNER_TD_6477_cmds_probe.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_6477_gqe_join_probe(krn_cmd_probe_700);
    cfg_JOIN_INNER_TD_6477_cmds_probe.cmd = krn_cmd_probe_700.getConfigBits();
    get_meta_info_JOIN_INNER_TD_6477_gqe_join_probe(cfg_JOIN_INNER_TD_6477_cmds_probe.meta_in, tbl_JOIN_INNER_TD_7482_consolidated_output.nrow, 3);
    get_meta_info_JOIN_INNER_TD_6477_gqe_join_probe(cfg_JOIN_INNER_TD_6477_cmds_probe.meta_out, tbl_JOIN_INNER_TD_6477_output.nrow, 3);
    // cfg_JOIN_INNER_TD_6477_cmds_probe.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_6477_cmds_part;
    cfg_JOIN_INNER_TD_6477_cmds_part.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_6477_gqe_join_part (cfg_JOIN_INNER_TD_6477_cmds_part.cmd);
    cfg_JOIN_INNER_TD_6477_cmds_part.allocateDevBuffer(context_h, 32);
    cfg_JOIN_INNER_TD_6477_cmds_build.input_key_columns = {0, -1, -1};
    cfg_JOIN_INNER_TD_6477_cmds_probe.input_key_columns = {0, -1, -1};
    cfg_JOIN_INNER_TD_6477_cmds_build.output_key_columns = {0, 1, 2, 3};
    cfg_JOIN_INNER_TD_6477_cmds_probe.output_key_columns = {0, 1, 2, 3};
    
    xf::database::gqe::KernelCommand krn_cmd_build_68 = xf::database::gqe::KernelCommand();
    xf::database::gqe::KernelCommand krn_cmd_probe_68 = xf::database::gqe::KernelCommand();
    cfgCmd cfg_JOIN_INNER_TD_7482_cmds_build;
    cfg_JOIN_INNER_TD_7482_cmds_build.setup(1);
    cfg_JOIN_INNER_TD_7482_cmds_build.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_7482_gqe_join_build(krn_cmd_build_68);
    cfg_JOIN_INNER_TD_7482_cmds_build.cmd = krn_cmd_build_68.getConfigBits();
    get_meta_info_JOIN_INNER_TD_7482_gqe_join_build(cfg_JOIN_INNER_TD_7482_cmds_build.meta_in, tbl_SerializeFromObject_TD_8484_input.nrow, 3);
    get_meta_info_JOIN_INNER_TD_7482_gqe_join_build(cfg_JOIN_INNER_TD_7482_cmds_build.meta_out, tbl_JOIN_INNER_TD_7482_output.nrow, 4);
    // cfg_JOIN_INNER_TD_7482_cmds_build.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_7482_cmds_probe;
    cfg_JOIN_INNER_TD_7482_cmds_probe.setup(1);
    cfg_JOIN_INNER_TD_7482_cmds_probe.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_7482_gqe_join_probe(krn_cmd_probe_68);
    cfg_JOIN_INNER_TD_7482_cmds_probe.cmd = krn_cmd_probe_68.getConfigBits();
    get_meta_info_JOIN_INNER_TD_7482_gqe_join_probe(cfg_JOIN_INNER_TD_7482_cmds_probe.meta_in, tbl_Filter_TD_8404_output.nrow, 3);
    get_meta_info_JOIN_INNER_TD_7482_gqe_join_probe(cfg_JOIN_INNER_TD_7482_cmds_probe.meta_out, tbl_JOIN_INNER_TD_7482_output.nrow, 3);
    // cfg_JOIN_INNER_TD_7482_cmds_probe.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_7482_cmds_part;
    cfg_JOIN_INNER_TD_7482_cmds_part.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_7482_gqe_join_part (cfg_JOIN_INNER_TD_7482_cmds_part.cmd);
    cfg_JOIN_INNER_TD_7482_cmds_part.allocateDevBuffer(context_h, 32);
    cfg_JOIN_INNER_TD_7482_cmds_build.input_key_columns = {0, -1, -1};
    cfg_JOIN_INNER_TD_7482_cmds_probe.input_key_columns = {1, -1, -1};
    cfg_JOIN_INNER_TD_7482_cmds_build.output_key_columns = {0, 1, 2, 3};
    cfg_JOIN_INNER_TD_7482_cmds_probe.output_key_columns = {0, 1, 2, 3};
    // *************************** Kernel Setup ************************* // 
    bufferTmp buftmp_h(context_h, 1); 
    buftmp_h.initBuffer(q_h); 
    std::cout << std::endl; 
    krnlEngine krnl_JOIN_INNER_TD_4221_part_left;
    krnl_JOIN_INNER_TD_4221_part_left = krnlEngine(program_h, q_h, context_h, "gqePart");
    krnl_JOIN_INNER_TD_4221_part_left.setup_hp(512, 0, power_of_hpTimes_join, tbl_SerializeFromObject_TD_592_input, tbl_SerializeFromObject_TD_592_input_partition, cfg_JOIN_INNER_TD_4221_cmds_part);
    krnlEngine krnl_JOIN_INNER_TD_4221[hpTimes_join];
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_INNER_TD_4221[i] = krnlEngine(program_h, q_h, "gqeJoin");
    }
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_INNER_TD_4221[i].setup(tbl_SerializeFromObject_TD_592_input_partition_array[i], tbl_JOIN_INNER_TD_5296_consolidated_output_partition_array[i], tbl_JOIN_INNER_TD_4221_output_partition_array[i], , buftmp_h);
    }
    krnlEngine krnl_JOIN_INNER_TD_4221_build;
    krnl_JOIN_INNER_TD_4221_build = krnlEngine(program_h, q_h, context_h, "gqeKernel", 0);
    krnl_JOIN_INNER_TD_4221_build.setup(tbl_SerializeFromObject_TD_592_input, tbl_JOIN_INNER_TD_5296_consolidated_output, tbl_JOIN_INNER_TD_4221_output, cfg_JOIN_INNER_TD_4221_cmds_build, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_4221_probe;
    krnl_JOIN_INNER_TD_4221_probe = krnlEngine(program_h, q_h, context_h, "gqeKernel", 1);
    krnl_JOIN_INNER_TD_4221_probe.setup(tbl_JOIN_INNER_TD_5296_consolidated_output, tbl_SerializeFromObject_TD_592_input, tbl_JOIN_INNER_TD_4221_output, cfg_JOIN_INNER_TD_4221_cmds_probe, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_5296_part_right;
    krnl_JOIN_INNER_TD_5296_part_right = krnlEngine(program_h, q_h, context_h, "gqePart");
    krnl_JOIN_INNER_TD_5296_part_right.setup_hp(512, 1, power_of_hpTimes_join, tbl_SerializeFromObject_TD_661_input, tbl_SerializeFromObject_TD_661_input_partition, cfg_JOIN_INNER_TD_5296_cmds_part);
    krnlEngine krnl_JOIN_INNER_TD_5296[hpTimes_join];
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_INNER_TD_5296[i] = krnlEngine(program_h, q_h, "gqeJoin");
    }
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_INNER_TD_5296[i].setup(tbl_JOIN_INNER_TD_6477_consolidated_output_partition_array[i], tbl_SerializeFromObject_TD_661_input_partition_array[i], tbl_JOIN_INNER_TD_5296_output_partition_array[i], , buftmp_h);
    }
    krnlEngine krnl_JOIN_INNER_TD_5296_build;
    krnl_JOIN_INNER_TD_5296_build = krnlEngine(program_h, q_h, context_h, "gqeKernel", 0);
    krnl_JOIN_INNER_TD_5296_build.setup(tbl_JOIN_INNER_TD_6477_consolidated_output, tbl_SerializeFromObject_TD_661_input, tbl_JOIN_INNER_TD_5296_output, cfg_JOIN_INNER_TD_5296_cmds_build, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_5296_probe;
    krnl_JOIN_INNER_TD_5296_probe = krnlEngine(program_h, q_h, context_h, "gqeKernel", 1);
    krnl_JOIN_INNER_TD_5296_probe.setup(tbl_SerializeFromObject_TD_661_input, tbl_JOIN_INNER_TD_6477_consolidated_output, tbl_JOIN_INNER_TD_5296_output, cfg_JOIN_INNER_TD_5296_cmds_probe, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_6477_part_left;
    krnl_JOIN_INNER_TD_6477_part_left = krnlEngine(program_h, q_h, context_h, "gqePart");
    krnl_JOIN_INNER_TD_6477_part_left.setup_hp(512, 0, power_of_hpTimes_join, tbl_Filter_TD_5186_output, tbl_Filter_TD_5186_output_partition, cfg_JOIN_INNER_TD_6477_cmds_part);
    krnlEngine krnl_JOIN_INNER_TD_6477[hpTimes_join];
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_INNER_TD_6477[i] = krnlEngine(program_h, q_h, "gqeJoin");
    }
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_INNER_TD_6477[i].setup(tbl_Filter_TD_5186_output_partition_array[i], tbl_JOIN_INNER_TD_7482_consolidated_output_partition_array[i], tbl_JOIN_INNER_TD_6477_output_partition_array[i], , buftmp_h);
    }
    krnlEngine krnl_JOIN_INNER_TD_6477_build;
    krnl_JOIN_INNER_TD_6477_build = krnlEngine(program_h, q_h, context_h, "gqeKernel", 0);
    krnl_JOIN_INNER_TD_6477_build.setup(tbl_Filter_TD_5186_output, tbl_JOIN_INNER_TD_7482_consolidated_output, tbl_JOIN_INNER_TD_6477_output, cfg_JOIN_INNER_TD_6477_cmds_build, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_6477_probe;
    krnl_JOIN_INNER_TD_6477_probe = krnlEngine(program_h, q_h, context_h, "gqeKernel", 1);
    krnl_JOIN_INNER_TD_6477_probe.setup(tbl_JOIN_INNER_TD_7482_consolidated_output, tbl_Filter_TD_5186_output, tbl_JOIN_INNER_TD_6477_output, cfg_JOIN_INNER_TD_6477_cmds_probe, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_7482_part_left;
    krnl_JOIN_INNER_TD_7482_part_left = krnlEngine(program_h, q_h, context_h, "gqePart");
    krnl_JOIN_INNER_TD_7482_part_left.setup_hp(512, 0, power_of_hpTimes_join, tbl_SerializeFromObject_TD_8484_input, tbl_SerializeFromObject_TD_8484_input_partition, cfg_JOIN_INNER_TD_7482_cmds_part);
    krnlEngine krnl_JOIN_INNER_TD_7482_part_right;
    krnl_JOIN_INNER_TD_7482_part_right = krnlEngine(program_h, q_h, context_h, "gqePart");
    krnl_JOIN_INNER_TD_7482_part_right.setup_hp(512, 1, power_of_hpTimes_join, tbl_Filter_TD_8404_output, tbl_Filter_TD_8404_output_partition, cfg_JOIN_INNER_TD_7482_cmds_part);
    krnlEngine krnl_JOIN_INNER_TD_7482[hpTimes_join];
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_INNER_TD_7482[i] = krnlEngine(program_h, q_h, "gqeJoin");
    }
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_INNER_TD_7482[i].setup(tbl_SerializeFromObject_TD_8484_input_partition_array[i], tbl_Filter_TD_8404_output_partition_array[i], tbl_JOIN_INNER_TD_7482_output_partition_array[i], , buftmp_h);
    }
    krnlEngine krnl_JOIN_INNER_TD_7482_build;
    krnl_JOIN_INNER_TD_7482_build = krnlEngine(program_h, q_h, context_h, "gqeKernel", 0);
    krnl_JOIN_INNER_TD_7482_build.setup(tbl_SerializeFromObject_TD_8484_input, tbl_Filter_TD_8404_output, tbl_JOIN_INNER_TD_7482_output, cfg_JOIN_INNER_TD_7482_cmds_build, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_7482_probe;
    krnl_JOIN_INNER_TD_7482_probe = krnlEngine(program_h, q_h, context_h, "gqeKernel", 1);
    krnl_JOIN_INNER_TD_7482_probe.setup(tbl_Filter_TD_8404_output, tbl_SerializeFromObject_TD_8484_input, tbl_JOIN_INNER_TD_7482_output, cfg_JOIN_INNER_TD_7482_cmds_probe, buftmp_h);
    // ************************** Transfer Engine *********************** // 
    transEngine trans_JOIN_INNER_TD_4221;
    trans_JOIN_INNER_TD_4221.setq(q_h,1);
    trans_JOIN_INNER_TD_4221.add(&(cfg_JOIN_INNER_TD_4221_cmds_part));
    trans_JOIN_INNER_TD_4221.add(&(cfg_JOIN_INNER_TD_4221_cmds_build));
    trans_JOIN_INNER_TD_4221.add(&(cfg_JOIN_INNER_TD_4221_cmds_probe));
    trans_JOIN_INNER_TD_4221.add(&(tbl_SerializeFromObject_TD_592_input));
    transEngine trans_JOIN_INNER_TD_4221_out;
    trans_JOIN_INNER_TD_4221_out.setq(q_h,1);
    q_h.finish();
    transEngine trans_JOIN_INNER_TD_5296;
    trans_JOIN_INNER_TD_5296.setq(q_h,1);
    trans_JOIN_INNER_TD_5296.add(&(cfg_JOIN_INNER_TD_5296_cmds_part));
    trans_JOIN_INNER_TD_5296.add(&(cfg_JOIN_INNER_TD_5296_cmds_build));
    trans_JOIN_INNER_TD_5296.add(&(cfg_JOIN_INNER_TD_5296_cmds_probe));
    trans_JOIN_INNER_TD_5296.add(&(tbl_SerializeFromObject_TD_661_input));
    transEngine trans_JOIN_INNER_TD_5296_out;
    trans_JOIN_INNER_TD_5296_out.setq(q_h,1);
    q_h.finish();
    transEngine trans_JOIN_INNER_TD_6477;
    trans_JOIN_INNER_TD_6477.setq(q_h,1);
    trans_JOIN_INNER_TD_6477.add(&(cfg_JOIN_INNER_TD_6477_cmds_part));
    trans_JOIN_INNER_TD_6477.add(&(cfg_JOIN_INNER_TD_6477_cmds_build));
    trans_JOIN_INNER_TD_6477.add(&(cfg_JOIN_INNER_TD_6477_cmds_probe));
    transEngine trans_JOIN_INNER_TD_6477_out;
    trans_JOIN_INNER_TD_6477_out.setq(q_h,1);
    q_h.finish();
    transEngine trans_JOIN_INNER_TD_7482;
    trans_JOIN_INNER_TD_7482.setq(q_h,1);
    trans_JOIN_INNER_TD_7482.add(&(cfg_JOIN_INNER_TD_7482_cmds_part));
    trans_JOIN_INNER_TD_7482.add(&(cfg_JOIN_INNER_TD_7482_cmds_build));
    trans_JOIN_INNER_TD_7482.add(&(cfg_JOIN_INNER_TD_7482_cmds_probe));
    trans_JOIN_INNER_TD_7482.add(&(tbl_SerializeFromObject_TD_8484_input));
    transEngine trans_JOIN_INNER_TD_7482_out;
    trans_JOIN_INNER_TD_7482_out.setq(q_h,1);
    q_h.finish();
    // ****************************** Events **************************** // 
    std::vector<cl::Event> events_h2d_wr_JOIN_INNER_TD_4221;
    std::vector<cl::Event> events_d2h_rd_JOIN_INNER_TD_4221;
    std::vector<cl::Event> events_JOIN_INNER_TD_4221[2];
    events_h2d_wr_JOIN_INNER_TD_4221.resize(1);
    events_d2h_rd_JOIN_INNER_TD_4221.resize(1);
    events_JOIN_INNER_TD_4221[0].resize(1);
    events_JOIN_INNER_TD_4221[1].resize(hpTimes_join);
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_4221;
    std::vector<cl::Event> prev_events_grp_JOIN_INNER_TD_4221;
    std::vector<cl::Event> events_h2d_wr_JOIN_INNER_TD_5296;
    std::vector<cl::Event> events_d2h_rd_JOIN_INNER_TD_5296;
    std::vector<cl::Event> events_JOIN_INNER_TD_5296[2];
    events_h2d_wr_JOIN_INNER_TD_5296.resize(1);
    events_d2h_rd_JOIN_INNER_TD_5296.resize(1);
    events_JOIN_INNER_TD_5296[0].resize(1);
    events_JOIN_INNER_TD_5296[1].resize(hpTimes_join);
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_5296;
    std::vector<cl::Event> prev_events_grp_JOIN_INNER_TD_5296;
    std::vector<cl::Event> events_h2d_wr_JOIN_INNER_TD_6477;
    std::vector<cl::Event> events_d2h_rd_JOIN_INNER_TD_6477;
    std::vector<cl::Event> events_JOIN_INNER_TD_6477[2];
    events_h2d_wr_JOIN_INNER_TD_6477.resize(1);
    events_d2h_rd_JOIN_INNER_TD_6477.resize(1);
    events_JOIN_INNER_TD_6477[0].resize(1);
    events_JOIN_INNER_TD_6477[1].resize(hpTimes_join);
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_6477;
    std::vector<cl::Event> prev_events_grp_JOIN_INNER_TD_6477;
    std::vector<cl::Event> events_h2d_wr_JOIN_INNER_TD_7482;
    std::vector<cl::Event> events_d2h_rd_JOIN_INNER_TD_7482;
    std::vector<cl::Event> events_JOIN_INNER_TD_7482[2];
    events_h2d_wr_JOIN_INNER_TD_7482.resize(1);
    events_d2h_rd_JOIN_INNER_TD_7482.resize(1);
    events_JOIN_INNER_TD_7482[0].resize(2);
    events_JOIN_INNER_TD_7482[1].resize(hpTimes_join);
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_7482;
    std::vector<cl::Event> prev_events_grp_JOIN_INNER_TD_7482;
    // **************************** Operations ************************** // 
    struct timeval tv_r_s, tv_r_e; 
    gettimeofday(&tv_r_s, 0); 

    struct timeval tv_r_Filter_8_317_s, tv_r_Filter_8_317_e;
    gettimeofday(&tv_r_Filter_8_317_s, 0);
    SW_Filter_TD_8404(tbl_SerializeFromObject_TD_9366_input, tbl_Filter_TD_8404_output);
    gettimeofday(&tv_r_Filter_8_317_e, 0);

    struct timeval tv_r_Filter_5_690_s, tv_r_Filter_5_690_e;
    gettimeofday(&tv_r_Filter_5_690_s, 0);
    SW_Filter_TD_5186(tbl_SerializeFromObject_TD_6778_input, tbl_Filter_TD_5186_output);
    gettimeofday(&tv_r_Filter_5_690_e, 0);

    struct timeval tv_r_JOIN_INNER_7_823_s, tv_r_JOIN_INNER_7_823_e;
    gettimeofday(&tv_r_JOIN_INNER_7_823_s, 0);
    tbl_SerializeFromObject_TD_8484_input.tableToCol();
    tbl_Filter_TD_8404_output.tableToCol();
    krnl_JOIN_INNER_TD_7482_build.updateMeta(tbl_SerializeFromObject_TD_8484_input.getNumRow(), 3);
    krnl_JOIN_INNER_TD_7482_probe.updateMeta(tbl_Filter_TD_8404_output.getNumRow(), 3);
    trans_JOIN_INNER_TD_7482.add(&(tbl_Filter_TD_8404_output));
    trans_JOIN_INNER_TD_7482.host2dev(0, &(prev_events_grp_JOIN_INNER_TD_7482), &(events_h2d_wr_JOIN_INNER_TD_7482[0]));
    events_grp_JOIN_INNER_TD_7482.push_back(events_h2d_wr_JOIN_INNER_TD_7482[0]);
    krnl_JOIN_INNER_TD_7482_part_left.run(0, &(events_grp_JOIN_INNER_TD_7482), &(events_JOIN_INNER_TD_7482[0][0]));
    krnl_JOIN_INNER_TD_7482_part_right.run(0, &(events_grp_JOIN_INNER_TD_7482), &(events_JOIN_INNER_TD_7482[0][1]));
    for (int i(0); i < hpTimes_join; ++i) {
        krnl_JOIN_INNER_TD_7482[i].run(0, &(events_JOIN_INNER_TD_7482[0]), &(events_JOIN_INNER_TD_7482[1][i]));
    }
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_7482_build_done;
    events_grp_JOIN_INNER_TD_7482_build_done.push_back(events_JOIN_INNER_TD_7482[0]);
    krnl_JOIN_INNER_TD_7482_part_left.run(0, &(events_grp_JOIN_INNER_TD_7482), &(events_JOIN_INNER_TD_7482[0][0]));
    krnl_JOIN_INNER_TD_7482_part_right.run(0, &(events_grp_JOIN_INNER_TD_7482), &(events_JOIN_INNER_TD_7482[0][1]));
    for (int i(0); i < hpTimes_join; ++i) {
        krnl_JOIN_INNER_TD_7482[i].run(0, &(events_JOIN_INNER_TD_7482[0]), &(events_JOIN_INNER_TD_7482[1][i]));
    }
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_7482_probe_done;
    events_grp_JOIN_INNER_TD_7482_probe_done.push_back(events_JOIN_INNER_TD_7482[1]);
    for (int i(0); i < hpTimes_join; ++i) {
        trans_JOIN_INNER_TD_7482_out.add(&(tbl_JOIN_INNER_TD_7482_output_partition_array[i]));
    }
    trans_JOIN_INNER_TD_7482_out.dev2host(0, &(events_JOIN_INNER_TD_7482[1]), &(events_d2h_rd_JOIN_INNER_TD_7482[0]));
    q_h.flush();
    q_h.finish();
    tbl_JOIN_INNER_TD_7482_output.setNumRow((cfg_JOIN_INNER_TD_7482_cmds_probe.meta_out)->getColLen());
    tbl_JOIN_INNER_TD_7482_output.colToTable();
    SW_Consolidated_JOIN_INNER_TD_7482_output(tbl_SerializeFromObject_TD_8484_input, tbl_Filter_TD_8404_output, tbl_JOIN_INNER_TD_7482_output, tbl_JOIN_INNER_TD_7482_consolidated_output);
    gettimeofday(&tv_r_JOIN_INNER_7_823_e, 0);

    struct timeval tv_r_JOIN_INNER_6_836_s, tv_r_JOIN_INNER_6_836_e;
    gettimeofday(&tv_r_JOIN_INNER_6_836_s, 0);
    prev_events_grp_JOIN_INNER_TD_6477.push_back(events_h2d_wr_JOIN_INNER_TD_7482[0]);
    tbl_Filter_TD_5186_output.tableToCol();
    tbl_JOIN_INNER_TD_7482_consolidated_output.tableToCol();
    krnl_JOIN_INNER_TD_6477_build.updateMeta(tbl_Filter_TD_5186_output.getNumRow(), 3);
    krnl_JOIN_INNER_TD_6477_probe.updateMeta(tbl_JOIN_INNER_TD_7482_consolidated_output.getNumRow(), 3);
    trans_JOIN_INNER_TD_6477.add(&(tbl_Filter_TD_5186_output));
    trans_JOIN_INNER_TD_6477.host2dev(0, &(prev_events_grp_JOIN_INNER_TD_6477), &(events_h2d_wr_JOIN_INNER_TD_6477[0]));
    events_grp_JOIN_INNER_TD_6477.push_back(events_h2d_wr_JOIN_INNER_TD_6477[0]);
    events_grp_JOIN_INNER_TD_6477.insert(std::end(events_grp_JOIN_INNER_TD_6477), std::begin(events_JOIN_INNER_TD_7482[0]), std::end(events_JOIN_INNER_TD_7482[0]));
    events_grp_JOIN_INNER_TD_6477.insert(std::end(events_grp_JOIN_INNER_TD_6477), std::begin(events_JOIN_INNER_TD_7482[1]), std::end(events_JOIN_INNER_TD_7482[1]));
    krnl_JOIN_INNER_TD_6477_part_left.run(0, &(events_grp_JOIN_INNER_TD_6477), &(events_JOIN_INNER_TD_6477[0][0]));
    for (int i(0); i < hpTimes_join; ++i) {
        krnl_JOIN_INNER_TD_6477[i].run(0, &(events_JOIN_INNER_TD_6477[0]), &(events_JOIN_INNER_TD_6477[1][i]));
    }
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_6477_build_done;
    events_grp_JOIN_INNER_TD_6477_build_done.push_back(events_JOIN_INNER_TD_6477[0]);
    krnl_JOIN_INNER_TD_6477_part_left.run(0, &(events_grp_JOIN_INNER_TD_6477), &(events_JOIN_INNER_TD_6477[0][0]));
    for (int i(0); i < hpTimes_join; ++i) {
        krnl_JOIN_INNER_TD_6477[i].run(0, &(events_JOIN_INNER_TD_6477[0]), &(events_JOIN_INNER_TD_6477[1][i]));
    }
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_6477_probe_done;
    events_grp_JOIN_INNER_TD_6477_probe_done.push_back(events_JOIN_INNER_TD_6477[1]);
    for (int i(0); i < hpTimes_join; ++i) {
        trans_JOIN_INNER_TD_6477_out.add(&(tbl_JOIN_INNER_TD_6477_output_partition_array[i]));
    }
    trans_JOIN_INNER_TD_6477_out.dev2host(0, &(events_JOIN_INNER_TD_6477[1]), &(events_d2h_rd_JOIN_INNER_TD_6477[0]));
    q_h.flush();
    q_h.finish();
    tbl_JOIN_INNER_TD_6477_output.setNumRow((cfg_JOIN_INNER_TD_6477_cmds_probe.meta_out)->getColLen());
    tbl_JOIN_INNER_TD_6477_output.colToTable();
    SW_Consolidated_JOIN_INNER_TD_6477_output(tbl_Filter_TD_5186_output, tbl_JOIN_INNER_TD_7482_consolidated_output, tbl_JOIN_INNER_TD_6477_output, tbl_JOIN_INNER_TD_6477_consolidated_output);
    gettimeofday(&tv_r_JOIN_INNER_6_836_e, 0);

    struct timeval tv_r_JOIN_INNER_5_598_s, tv_r_JOIN_INNER_5_598_e;
    gettimeofday(&tv_r_JOIN_INNER_5_598_s, 0);
    prev_events_grp_JOIN_INNER_TD_5296.push_back(events_h2d_wr_JOIN_INNER_TD_6477[0]);
    tbl_JOIN_INNER_TD_6477_consolidated_output.tableToCol();
    tbl_SerializeFromObject_TD_661_input.tableToCol();
    krnl_JOIN_INNER_TD_5296_build.updateMeta(tbl_JOIN_INNER_TD_6477_consolidated_output.getNumRow(), 3);
    krnl_JOIN_INNER_TD_5296_probe.updateMeta(tbl_SerializeFromObject_TD_661_input.getNumRow(), 3);
    trans_JOIN_INNER_TD_5296.host2dev(0, &(prev_events_grp_JOIN_INNER_TD_5296), &(events_h2d_wr_JOIN_INNER_TD_5296[0]));
    events_grp_JOIN_INNER_TD_5296.push_back(events_h2d_wr_JOIN_INNER_TD_5296[0]);
    events_grp_JOIN_INNER_TD_5296.insert(std::end(events_grp_JOIN_INNER_TD_5296), std::begin(events_JOIN_INNER_TD_6477[0]), std::end(events_JOIN_INNER_TD_6477[0]));
    events_grp_JOIN_INNER_TD_5296.insert(std::end(events_grp_JOIN_INNER_TD_5296), std::begin(events_JOIN_INNER_TD_6477[1]), std::end(events_JOIN_INNER_TD_6477[1]));
    krnl_JOIN_INNER_TD_5296_part_right.run(0, &(events_grp_JOIN_INNER_TD_5296), &(events_JOIN_INNER_TD_5296[0][0]));
    for (int i(0); i < hpTimes_join; ++i) {
        krnl_JOIN_INNER_TD_5296[i].run(0, &(events_grp_JOIN_INNER_TD_5296), &(events_JOIN_INNER_TD_5296[1][i]));
    }
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_5296_build_done;
    events_grp_JOIN_INNER_TD_5296_build_done.push_back(events_JOIN_INNER_TD_5296[0]);
    krnl_JOIN_INNER_TD_5296_part_right.run(0, &(events_grp_JOIN_INNER_TD_5296), &(events_JOIN_INNER_TD_5296[0][0]));
    for (int i(0); i < hpTimes_join; ++i) {
        krnl_JOIN_INNER_TD_5296[i].run(0, &(events_grp_JOIN_INNER_TD_5296), &(events_JOIN_INNER_TD_5296[1][i]));
    }
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_5296_probe_done;
    events_grp_JOIN_INNER_TD_5296_probe_done.push_back(events_JOIN_INNER_TD_5296[1]);
    for (int i(0); i < hpTimes_join; ++i) {
        trans_JOIN_INNER_TD_5296_out.add(&(tbl_JOIN_INNER_TD_5296_output_partition_array[i]));
    }
    trans_JOIN_INNER_TD_5296_out.dev2host(0, &(events_JOIN_INNER_TD_5296[1]), &(events_d2h_rd_JOIN_INNER_TD_5296[0]));
    q_h.flush();
    q_h.finish();
    tbl_JOIN_INNER_TD_5296_output.setNumRow((cfg_JOIN_INNER_TD_5296_cmds_probe.meta_out)->getColLen());
    tbl_JOIN_INNER_TD_5296_output.colToTable();
    SW_Consolidated_JOIN_INNER_TD_5296_output(tbl_JOIN_INNER_TD_6477_consolidated_output, tbl_SerializeFromObject_TD_661_input, tbl_JOIN_INNER_TD_5296_output, tbl_JOIN_INNER_TD_5296_consolidated_output);
    gettimeofday(&tv_r_JOIN_INNER_5_598_e, 0);

    struct timeval tv_r_Filter_4_418_s, tv_r_Filter_4_418_e;
    gettimeofday(&tv_r_Filter_4_418_s, 0);
    SW_Filter_TD_4173(tbl_SerializeFromObject_TD_5132_input, tbl_Filter_TD_4173_output);
    gettimeofday(&tv_r_Filter_4_418_e, 0);

    struct timeval tv_r_JOIN_INNER_4_808_s, tv_r_JOIN_INNER_4_808_e;
    gettimeofday(&tv_r_JOIN_INNER_4_808_s, 0);
    prev_events_grp_JOIN_INNER_TD_4221.push_back(events_h2d_wr_JOIN_INNER_TD_5296[0]);
    tbl_SerializeFromObject_TD_592_input.tableToCol();
    tbl_JOIN_INNER_TD_5296_consolidated_output.tableToCol();
    krnl_JOIN_INNER_TD_4221_build.updateMeta(tbl_SerializeFromObject_TD_592_input.getNumRow(), 3);
    krnl_JOIN_INNER_TD_4221_probe.updateMeta(tbl_JOIN_INNER_TD_5296_consolidated_output.getNumRow(), 3);
    trans_JOIN_INNER_TD_4221.host2dev(0, &(prev_events_grp_JOIN_INNER_TD_4221), &(events_h2d_wr_JOIN_INNER_TD_4221[0]));
    events_grp_JOIN_INNER_TD_4221.push_back(events_h2d_wr_JOIN_INNER_TD_4221[0]);
    events_grp_JOIN_INNER_TD_4221.insert(std::end(events_grp_JOIN_INNER_TD_4221), std::begin(events_JOIN_INNER_TD_5296[0]), std::end(events_JOIN_INNER_TD_5296[0]));
    events_grp_JOIN_INNER_TD_4221.insert(std::end(events_grp_JOIN_INNER_TD_4221), std::begin(events_JOIN_INNER_TD_5296[1]), std::end(events_JOIN_INNER_TD_5296[1]));
    krnl_JOIN_INNER_TD_4221_part_left.run(0, &(events_grp_JOIN_INNER_TD_4221), &(events_JOIN_INNER_TD_4221[0][0]));
    for (int i(0); i < hpTimes_join; ++i) {
        krnl_JOIN_INNER_TD_4221[i].run(0, &(events_JOIN_INNER_TD_4221[0]), &(events_JOIN_INNER_TD_4221[1][i]));
    }
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_4221_build_done;
    events_grp_JOIN_INNER_TD_4221_build_done.push_back(events_JOIN_INNER_TD_4221[0]);
    krnl_JOIN_INNER_TD_4221_part_left.run(0, &(events_grp_JOIN_INNER_TD_4221), &(events_JOIN_INNER_TD_4221[0][0]));
    for (int i(0); i < hpTimes_join; ++i) {
        krnl_JOIN_INNER_TD_4221[i].run(0, &(events_JOIN_INNER_TD_4221[0]), &(events_JOIN_INNER_TD_4221[1][i]));
    }
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_4221_probe_done;
    events_grp_JOIN_INNER_TD_4221_probe_done.push_back(events_JOIN_INNER_TD_4221[1]);
    for (int i(0); i < hpTimes_join; ++i) {
        trans_JOIN_INNER_TD_4221_out.add(&(tbl_JOIN_INNER_TD_4221_output_partition_array[i]));
    }
    trans_JOIN_INNER_TD_4221_out.dev2host(0, &(events_JOIN_INNER_TD_4221[1]), &(events_d2h_rd_JOIN_INNER_TD_4221[0]));
    q_h.flush();
    q_h.finish();
    tbl_JOIN_INNER_TD_4221_output.setNumRow((cfg_JOIN_INNER_TD_4221_cmds_probe.meta_out)->getColLen());
    tbl_JOIN_INNER_TD_4221_output.colToTable();
    SW_Consolidated_JOIN_INNER_TD_4221_output(tbl_SerializeFromObject_TD_592_input, tbl_JOIN_INNER_TD_5296_consolidated_output, tbl_JOIN_INNER_TD_4221_output, tbl_JOIN_INNER_TD_4221_consolidated_output);
    gettimeofday(&tv_r_JOIN_INNER_4_808_e, 0);

    struct timeval tv_r_JOIN_INNER_3_239_s, tv_r_JOIN_INNER_3_239_e;
    gettimeofday(&tv_r_JOIN_INNER_3_239_s, 0);
    SW_JOIN_INNER_TD_3929(tbl_JOIN_INNER_TD_4221_consolidated_output_partition_array, tbl_Filter_TD_4173_output, tbl_SerializeFromObject_TD_6778_input, tbl_JOIN_INNER_TD_3929_output, hpTimes_join);
    gettimeofday(&tv_r_JOIN_INNER_3_239_e, 0);

    struct timeval tv_r_Project_2_790_s, tv_r_Project_2_790_e;
    gettimeofday(&tv_r_Project_2_790_s, 0);
    SW_Project_TD_296(tbl_JOIN_INNER_TD_3929_output, tbl_Project_TD_296_output);
    gettimeofday(&tv_r_Project_2_790_e, 0);

    struct timeval tv_r_Aggregate_1_350_s, tv_r_Aggregate_1_350_e;
    gettimeofday(&tv_r_Aggregate_1_350_s, 0);
    SW_Aggregate_TD_1275(tbl_Project_TD_296_output, tbl_Aggregate_TD_1275_output);
    gettimeofday(&tv_r_Aggregate_1_350_e, 0);

    struct timeval tv_r_Sort_0_745_s, tv_r_Sort_0_745_e;
    gettimeofday(&tv_r_Sort_0_745_s, 0);
    SW_Sort_TD_0788(tbl_Aggregate_TD_1275_output, tbl_Sort_TD_0788_output);
    gettimeofday(&tv_r_Sort_0_745_e, 0);

std::cout << "CPU TIME: " << (tvdiff(&tv_r_Filter_8_317_s, &tv_r_Filter_8_317_e) + 
tvdiff(&tv_r_Filter_5_690_s, &tv_r_Filter_5_690_e) + 
tvdiff(&tv_r_Filter_4_418_s, &tv_r_Filter_4_418_e) + 
tvdiff(&tv_r_JOIN_INNER_3_239_s, &tv_r_JOIN_INNER_3_239_e) + 
tvdiff(&tv_r_Project_2_790_s, &tv_r_Project_2_790_e) + 
tvdiff(&tv_r_Aggregate_1_350_s, &tv_r_Aggregate_1_350_e) + 
tvdiff(&tv_r_Sort_0_745_s, &tv_r_Sort_0_745_e) + 
1) / 1000.0 << std::endl;

std::cout << "FPGA TIME: " << (tvdiff(&tv_r_JOIN_INNER_7_823_s, &tv_r_JOIN_INNER_7_823_e) + 
tvdiff(&tv_r_JOIN_INNER_6_836_s, &tv_r_JOIN_INNER_6_836_e) + 
tvdiff(&tv_r_JOIN_INNER_5_598_s, &tv_r_JOIN_INNER_5_598_e) + 
tvdiff(&tv_r_JOIN_INNER_4_808_s, &tv_r_JOIN_INNER_4_808_e) + 
1) / 1000.0 << std::endl;

    gettimeofday(&tv_r_e, 0); 
    // **************************** Print Execution Time ************************** // 
    std::cout << std::endl; 
    std::cout << "Filter_8: " << tvdiff(&tv_r_Filter_8_317_s, &tv_r_Filter_8_317_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(((l_shipdate#87 >= 19950101) AND (l_shipdate#87 <= 19961231))) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_9366_input.getNumRow() << " -> tbl_SerializeFromObject_TD_9366_input" 
     << std::endl << "      #COL: 5: " << "ListBuffer(l_orderkey#77, l_suppkey#79, l_extendedprice#82, l_discount#83, l_shipdate#87)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_8404_output.getNumRow() << " -> tbl_Filter_TD_8404_output" 
     << std::endl << "      #COL: 5: " << "ListBuffer(l_orderkey#77, l_suppkey#79, l_extendedprice#82, l_discount#83, l_shipdate#87)" 
     << std::endl; 

    std::cout << "Filter_5: " << tvdiff(&tv_r_Filter_5_690_s, &tv_r_Filter_5_690_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(((n_name#145 = FRANCE) OR (n_name#145 = IRAQ))) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_6778_input.getNumRow() << " -> tbl_SerializeFromObject_TD_6778_input" 
     << std::endl << "      #COL: 2: " << "ListBuffer(n_nationkey#144, n_name#145)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_5186_output.getNumRow() << " -> tbl_Filter_TD_5186_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(n_nationkey#144, n_name#145)" 
     << std::endl; 

    std::cout << "JOIN_INNER_7: " << tvdiff(&tv_r_JOIN_INNER_7_823_s, &tv_r_JOIN_INNER_7_823_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer((s_suppkey#342 = l_suppkey#79)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_8484_input.getNumRow() << " -> tbl_SerializeFromObject_TD_8484_input" 
     << std::endl << "      #COL: 2: " << "ListBuffer(s_suppkey#342, s_nationkey#345)" 
     << std::endl << "      #ROW: " << tbl_Filter_TD_8404_output.getNumRow() << " -> tbl_Filter_TD_8404_output" 
     << std::endl << "      #COL: 5: " << "ListBuffer(l_orderkey#77, l_suppkey#79, l_extendedprice#82, l_discount#83, l_shipdate#87)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_7482_consolidated_output.getNumRow() << " -> tbl_JOIN_INNER_TD_7482_consolidated_output" 
     << std::endl << "      #COL: 5: " << "ListBuffer(s_nationkey#345, l_orderkey#77, l_extendedprice#82, l_discount#83, l_shipdate#87)" 
     << std::endl; 

    std::cout << "JOIN_INNER_6: " << tvdiff(&tv_r_JOIN_INNER_6_836_s, &tv_r_JOIN_INNER_6_836_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer((s_nationkey#345 = n_nationkey#144)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_7482_consolidated_output.getNumRow() << " -> tbl_JOIN_INNER_TD_7482_consolidated_output" 
     << std::endl << "      #COL: 5: " << "ListBuffer(s_nationkey#345, l_orderkey#77, l_extendedprice#82, l_discount#83, l_shipdate#87)" 
     << std::endl << "      #ROW: " << tbl_Filter_TD_5186_output.getNumRow() << " -> tbl_Filter_TD_5186_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(n_nationkey#144, n_name#145)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_6477_consolidated_output.getNumRow() << " -> tbl_JOIN_INNER_TD_6477_consolidated_output" 
     << std::endl << "      #COL: 6: " << "ListBuffer(l_extendedprice#82, l_discount#83, l_shipdate#87, n_name#145, s_nationkey#345, l_orderkey#77)" 
     << std::endl; 

    std::cout << "JOIN_INNER_5: " << tvdiff(&tv_r_JOIN_INNER_5_598_s, &tv_r_JOIN_INNER_5_598_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer((o_orderkey#207 = l_orderkey#77)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_6477_consolidated_output.getNumRow() << " -> tbl_JOIN_INNER_TD_6477_consolidated_output" 
     << std::endl << "      #COL: 6: " << "ListBuffer(l_extendedprice#82, l_discount#83, l_shipdate#87, n_name#145, s_nationkey#345, l_orderkey#77)" 
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_661_input.getNumRow() << " -> tbl_SerializeFromObject_TD_661_input" 
     << std::endl << "      #COL: 2: " << "ListBuffer(o_orderkey#207, o_custkey#208)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_5296_consolidated_output.getNumRow() << " -> tbl_JOIN_INNER_TD_5296_consolidated_output" 
     << std::endl << "      #COL: 6: " << "ListBuffer(s_nationkey#345, l_extendedprice#82, l_discount#83, l_shipdate#87, o_custkey#208, n_name#145)" 
     << std::endl; 

    std::cout << "Filter_4: " << tvdiff(&tv_r_Filter_4_418_s, &tv_r_Filter_4_418_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(((n_name#871 = IRAQ) OR (n_name#871 = FRANCE))) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_5132_input.getNumRow() << " -> tbl_SerializeFromObject_TD_5132_input" 
     << std::endl << "      #COL: 2: " << "ListBuffer(n_nationkey#870, n_name#871)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_4173_output.getNumRow() << " -> tbl_Filter_TD_4173_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(n_nationkey#870, n_name#871)" 
     << std::endl; 

    std::cout << "JOIN_INNER_4: " << tvdiff(&tv_r_JOIN_INNER_4_808_s, &tv_r_JOIN_INNER_4_808_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer((c_custkey#22 = o_custkey#208)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_5296_consolidated_output.getNumRow() << " -> tbl_JOIN_INNER_TD_5296_consolidated_output" 
     << std::endl << "      #COL: 6: " << "ListBuffer(s_nationkey#345, l_extendedprice#82, l_discount#83, l_shipdate#87, o_custkey#208, n_name#145)" 
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_592_input.getNumRow() << " -> tbl_SerializeFromObject_TD_592_input" 
     << std::endl << "      #COL: 2: " << "ListBuffer(c_custkey#22, c_nationkey#25)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_4221_consolidated_output.getNumRow() << " -> tbl_JOIN_INNER_TD_4221_consolidated_output" 
     << std::endl << "      #COL: 5: " << "ListBuffer(l_extendedprice#82, l_discount#83, l_shipdate#87, c_nationkey#25, n_name#145)" 
     << std::endl; 

    std::cout << "JOIN_INNER_3: " << tvdiff(&tv_r_JOIN_INNER_3_239_s, &tv_r_JOIN_INNER_3_239_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(((c_nationkey#25 = n_nationkey#870) AND (((n_name#145 = FRANCE) AND (n_name#871 = IRAQ)) OR ((n_name#145 = IRAQ) AND (n_name#871 = FRANCE))))) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_4221_consolidated_output.getNumRow() << " -> tbl_JOIN_INNER_TD_4221_consolidated_output" 
     << std::endl << "      #COL: 5: " << "ListBuffer(l_extendedprice#82, l_discount#83, l_shipdate#87, c_nationkey#25, n_name#145)" 
     << std::endl << "      #ROW: " << tbl_Filter_TD_4173_output.getNumRow() << " -> tbl_Filter_TD_4173_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(n_nationkey#870, n_name#871)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_3929_output.getNumRow() << " -> tbl_JOIN_INNER_TD_3929_output" 
     << std::endl << "      #COL: 6: " << "ListBuffer(n_name#145, n_name#871, l_shipdate#87, l_extendedprice#82, l_discount#83, c_nationkey#25)" 
     << std::endl; 

    std::cout << "Project_2: " << tvdiff(&tv_r_Project_2_790_s, &tv_r_Project_2_790_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(n_name#145 AS supp_nation#864, n_name#871 AS cust_nation#865, cast((cast(l_shipdate#87 as double) / 10000.0) as int) AS l_year#866, (l_extendedprice#82 * (100 - l_discount#83)) AS volume#867) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_3929_output.getNumRow() << " -> tbl_JOIN_INNER_TD_3929_output" 
     << std::endl << "      #COL: 6: " << "ListBuffer(n_name#145, n_name#871, l_shipdate#87, l_extendedprice#82, l_discount#83, c_nationkey#25)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Project_TD_296_output.getNumRow() << " -> tbl_Project_TD_296_output" 
     << std::endl << "      #COL: 4: " << "ListBuffer(supp_nation#864, cust_nation#865, l_year#866, volume#867)" 
     << std::endl; 

    std::cout << "Aggregate_1: " << tvdiff(&tv_r_Aggregate_1_350_s, &tv_r_Aggregate_1_350_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(supp_nation#864, cust_nation#865, l_year#866, sum(volume#867) AS revenue#868L) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_Project_TD_296_output.getNumRow() << " -> tbl_Project_TD_296_output" 
     << std::endl << "      #COL: 4: " << "ListBuffer(supp_nation#864, cust_nation#865, l_year#866, volume#867)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_1275_output.getNumRow() << " -> tbl_Aggregate_TD_1275_output" 
     << std::endl << "      #COL: 4: " << "ListBuffer(supp_nation#864, cust_nation#865, l_year#866, revenue#868L)" 
     << std::endl; 

    std::cout << "Sort_0: " << tvdiff(&tv_r_Sort_0_745_s, &tv_r_Sort_0_745_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(supp_nation#864 ASC NULLS FIRST, cust_nation#865 ASC NULLS FIRST, l_year#866 ASC NULLS FIRST) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_1275_output.getNumRow() << " -> tbl_Aggregate_TD_1275_output" 
     << std::endl << "      #COL: 4: " << "ListBuffer(supp_nation#864, cust_nation#865, l_year#866, revenue#868L)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Sort_TD_0788_output.getNumRow() << " -> tbl_Sort_TD_0788_output" 
     << std::endl << "      #COL: 4: " << "ListBuffer(supp_nation#864, cust_nation#865, l_year#866, revenue#868L)" 
     << std::endl; 

    std::cout << std::endl << " Total execution time: " << tvdiff(&tv_r_s, &tv_r_e) / 1000 << " ms"; 

    std::cout << std::endl << " Spark elapsed time: " << 25.354008 * 1000 << "ms" << std::endl; 
    return 0; 
}
