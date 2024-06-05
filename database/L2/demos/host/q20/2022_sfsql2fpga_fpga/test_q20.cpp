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
    // *********************** Partition Infomation ********************* // 
    int hpTimes_join = 32; 
    int power_of_hpTimes_join = log2(hpTimes_join); 
    std::cout << "Number of partition (gqeJoin) is: " << hpTimes_join << std::endl; 
    int hpTimes_aggr = 256; 
    int power_of_hpTimes_aggr = log2(hpTimes_aggr); 
    std::cout << "Number of partition (gqeAggr) is: " << hpTimes_aggr << std::endl; 
    // ****************************** Tables **************************** // 
    Table tbl_Sort_TD_0649_output("tbl_Sort_TD_0649_output", 183000000, 2, "");
    tbl_Sort_TD_0649_output.allocateHost();
    Table tbl_JOIN_INNER_TD_1763_output("tbl_JOIN_INNER_TD_1763_output", 210, 4, "");
    tbl_JOIN_INNER_TD_1763_output.allocateHost(1.2, hpTimes_join);
    Table tbl_JOIN_INNER_TD_1763_consolidated_output("tbl_JOIN_INNER_TD_1763_consolidated_output", 210, 2, "");
    tbl_JOIN_INNER_TD_1763_consolidated_output.selectOverlayVersion(1);
    tbl_JOIN_INNER_TD_1763_consolidated_output.allocateHost();
    Table tbl_JOIN_LEFTSEMI_TD_2112_output("tbl_JOIN_LEFTSEMI_TD_2112_output", 5366, 4, "");
    tbl_JOIN_LEFTSEMI_TD_2112_output.allocateHost(1.2, hpTimes_join);
    Table tbl_JOIN_LEFTSEMI_TD_2112_consolidated_output("tbl_JOIN_LEFTSEMI_TD_2112_consolidated_output", 5366, 3, "");
    tbl_JOIN_LEFTSEMI_TD_2112_consolidated_output.selectOverlayVersion(1);
    tbl_JOIN_LEFTSEMI_TD_2112_consolidated_output.allocateHost();
    Table tbl_Filter_TD_2221_output("tbl_Filter_TD_2221_output", 183000000, 1, "");
    tbl_Filter_TD_2221_output.selectOverlayVersion(1);
    tbl_Filter_TD_2221_output.allocateHost();
    Table tbl_Filter_TD_2221_output_partition("tbl_Filter_TD_2221_output_partition", 183000000, 1, "");
    tbl_Filter_TD_2221_output_partition.allocateHost(1.2, hpTimes_join);
    tbl_Filter_TD_2221_output_partition.allocateDevBuffer(context_h, 32);
    Table tbl_Filter_TD_2221_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_Filter_TD_2221_output_partition_array[i] = tbl_Filter_TD_2221_output_partition.createSubTable(i);
    }
    Table tbl_SerializeFromObject_TD_3404_input_stringRowIDSubstitute;
    tbl_SerializeFromObject_TD_3404_input_stringRowIDSubstitute = Table("supplier", supplier_n, 4, in_dir);
    tbl_SerializeFromObject_TD_3404_input_stringRowIDSubstitute.addCol("s_suppkey", 4);
    tbl_SerializeFromObject_TD_3404_input_stringRowIDSubstitute.addCol("s_name", 4, 1, 0);
    tbl_SerializeFromObject_TD_3404_input_stringRowIDSubstitute.addCol("s_address", 4, 1, 0);
    tbl_SerializeFromObject_TD_3404_input_stringRowIDSubstitute.addCol("s_nationkey", 4);
    tbl_SerializeFromObject_TD_3404_input_stringRowIDSubstitute.allocateHost();
    tbl_SerializeFromObject_TD_3404_input_stringRowIDSubstitute.loadHost();
    Table tbl_SerializeFromObject_TD_3404_input_stringRowIDSubstitute_partition("tbl_SerializeFromObject_TD_3404_input_stringRowIDSubstitute_partition", supplier_n, 4, "");
    tbl_SerializeFromObject_TD_3404_input_stringRowIDSubstitute_partition.allocateHost(1.2, hpTimes_join);
    tbl_SerializeFromObject_TD_3404_input_stringRowIDSubstitute_partition.allocateDevBuffer(context_h, 32);
    Table tbl_SerializeFromObject_TD_3404_input_stringRowIDSubstitute_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_SerializeFromObject_TD_3404_input_stringRowIDSubstitute_partition_array[i] = tbl_SerializeFromObject_TD_3404_input_stringRowIDSubstitute_partition.createSubTable(i);
    }
    Table tbl_SerializeFromObject_TD_3404_input;
    tbl_SerializeFromObject_TD_3404_input = Table("supplier", supplier_n, 4, in_dir);
    tbl_SerializeFromObject_TD_3404_input.addCol("s_suppkey", 4);
    tbl_SerializeFromObject_TD_3404_input.addCol("s_name", TPCH_READ_S_NAME_LEN+1);
    tbl_SerializeFromObject_TD_3404_input.addCol("s_address", TPCH_READ_S_ADDR_MAX+1);
    tbl_SerializeFromObject_TD_3404_input.addCol("s_nationkey", 4);
    tbl_SerializeFromObject_TD_3404_input.allocateHost();
    tbl_SerializeFromObject_TD_3404_input.loadHost();
    Table tbl_JOIN_INNER_TD_3971_output("tbl_JOIN_INNER_TD_3971_output", 7670, 4, "");
    tbl_JOIN_INNER_TD_3971_output.allocateHost(1.2, hpTimes_join);
    Table tbl_JOIN_INNER_TD_3971_consolidated_output("tbl_JOIN_INNER_TD_3971_consolidated_output", 7670, 1, "");
    tbl_JOIN_INNER_TD_3971_consolidated_output.selectOverlayVersion(1);
    tbl_JOIN_INNER_TD_3971_consolidated_output.allocateHost();
    Table tbl_SerializeFromObject_TD_342_input;
    tbl_SerializeFromObject_TD_342_input = Table("nation", nation_n, 2, in_dir);
    tbl_SerializeFromObject_TD_342_input.addCol("n_nationkey", 4);
    tbl_SerializeFromObject_TD_342_input.addCol("n_name", TPCH_READ_NATION_LEN+1);
    tbl_SerializeFromObject_TD_342_input.allocateHost();
    tbl_SerializeFromObject_TD_342_input.loadHost();
    Table tbl_JOIN_LEFTSEMI_TD_487_output("tbl_JOIN_LEFTSEMI_TD_487_output", 11160, 4, "");
    tbl_JOIN_LEFTSEMI_TD_487_output.allocateHost(1.2, hpTimes_join);
    Table tbl_JOIN_LEFTSEMI_TD_487_consolidated_output("tbl_JOIN_LEFTSEMI_TD_487_consolidated_output", 11160, 3, "");
    tbl_JOIN_LEFTSEMI_TD_487_consolidated_output.selectOverlayVersion(1);
    tbl_JOIN_LEFTSEMI_TD_487_consolidated_output.allocateHost();
    Table tbl_Filter_TD_4970_output("tbl_Filter_TD_4970_output", 183000000, 3, "");
    tbl_Filter_TD_4970_output.selectOverlayVersion(1);
    tbl_Filter_TD_4970_output.allocateHost();
    Table tbl_Filter_TD_4970_output_partition("tbl_Filter_TD_4970_output_partition", 183000000, 3, "");
    tbl_Filter_TD_4970_output_partition.allocateHost(1.2, hpTimes_join);
    tbl_Filter_TD_4970_output_partition.allocateDevBuffer(context_h, 32);
    Table tbl_Filter_TD_4970_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_Filter_TD_4970_output_partition_array[i] = tbl_Filter_TD_4970_output_partition.createSubTable(i);
    }
    Table tbl_SerializeFromObject_TD_5662_input;
    tbl_SerializeFromObject_TD_5662_input = Table("partsupp", partsupp_n, 3, in_dir);
    tbl_SerializeFromObject_TD_5662_input.selectOverlayVersion(1);
    tbl_SerializeFromObject_TD_5662_input.addCol("ps_partkey", 4);
    tbl_SerializeFromObject_TD_5662_input.addCol("ps_suppkey", 4);
    tbl_SerializeFromObject_TD_5662_input.addCol("ps_availqty", 4);
    tbl_SerializeFromObject_TD_5662_input.allocateHost();
    tbl_SerializeFromObject_TD_5662_input.loadHost();
    Table tbl_SerializeFromObject_TD_5662_input_partition("tbl_SerializeFromObject_TD_5662_input_partition", partsupp_n, 3, "");
    tbl_SerializeFromObject_TD_5662_input_partition.allocateHost(1.2, hpTimes_join);
    tbl_SerializeFromObject_TD_5662_input_partition.allocateDevBuffer(context_h, 32);
    Table tbl_SerializeFromObject_TD_5662_input_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_SerializeFromObject_TD_5662_input_partition_array[i] = tbl_SerializeFromObject_TD_5662_input_partition.createSubTable(i);
    }
    Table tbl_Filter_TD_5130_output("tbl_Filter_TD_5130_output", 183000000, 1, "");
    tbl_Filter_TD_5130_output.selectOverlayVersion(1);
    tbl_Filter_TD_5130_output.allocateHost();
    Table tbl_Filter_TD_5130_output_partition("tbl_Filter_TD_5130_output_partition", 183000000, 1, "");
    tbl_Filter_TD_5130_output_partition.allocateHost(1.2, hpTimes_join);
    tbl_Filter_TD_5130_output_partition.allocateDevBuffer(context_h, 32);
    Table tbl_Filter_TD_5130_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_Filter_TD_5130_output_partition_array[i] = tbl_Filter_TD_5130_output_partition.createSubTable(i);
    }
    Table tbl_Aggregate_TD_5646_output("tbl_Aggregate_TD_5646_output", 183000000, 3, "");
    tbl_Aggregate_TD_5646_output.allocateHost();
    Table tbl_SerializeFromObject_TD_6257_input;
    tbl_SerializeFromObject_TD_6257_input = Table("part", part_n, 2, in_dir);
    tbl_SerializeFromObject_TD_6257_input.addCol("p_partkey", 4);
    tbl_SerializeFromObject_TD_6257_input.addCol("p_name", TPCH_READ_P_NAME_LEN+1);
    tbl_SerializeFromObject_TD_6257_input.allocateHost();
    tbl_SerializeFromObject_TD_6257_input.loadHost();
    Table tbl_JOIN_LEFTSEMI_TD_6400_output("tbl_JOIN_LEFTSEMI_TD_6400_output", 12816, 4, "");
    tbl_JOIN_LEFTSEMI_TD_6400_output.allocateHost(1.2, hpTimes_join);
    Table tbl_JOIN_LEFTSEMI_TD_6400_consolidated_output("tbl_JOIN_LEFTSEMI_TD_6400_consolidated_output", 12816, 3, "");
    tbl_JOIN_LEFTSEMI_TD_6400_consolidated_output.selectOverlayVersion(1);
    tbl_JOIN_LEFTSEMI_TD_6400_consolidated_output.allocateHost();
    Table tbl_Filter_TD_7493_output("tbl_Filter_TD_7493_output", 183000000, 3, "");
    tbl_Filter_TD_7493_output.selectOverlayVersion(1);
    tbl_Filter_TD_7493_output.allocateHost();
    Table tbl_Filter_TD_7493_output_partition("tbl_Filter_TD_7493_output_partition", 183000000, 3, "");
    tbl_Filter_TD_7493_output_partition.allocateHost(1.2, hpTimes_join);
    tbl_Filter_TD_7493_output_partition.allocateDevBuffer(context_h, 32);
    Table tbl_Filter_TD_7493_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_Filter_TD_7493_output_partition_array[i] = tbl_Filter_TD_7493_output_partition.createSubTable(i);
    }
    Table tbl_Filter_TD_7282_output("tbl_Filter_TD_7282_output", 183000000, 1, "");
    tbl_Filter_TD_7282_output.selectOverlayVersion(1);
    tbl_Filter_TD_7282_output.allocateHost();
    Table tbl_Filter_TD_7282_output_partition("tbl_Filter_TD_7282_output_partition", 183000000, 1, "");
    tbl_Filter_TD_7282_output_partition.allocateHost(1.2, hpTimes_join);
    tbl_Filter_TD_7282_output_partition.allocateDevBuffer(context_h, 32);
    Table tbl_Filter_TD_7282_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_Filter_TD_7282_output_partition_array[i] = tbl_Filter_TD_7282_output_partition.createSubTable(i);
    }
    Table tbl_SerializeFromObject_TD_8847_input;
    tbl_SerializeFromObject_TD_8847_input = Table("lineitem", lineitem_n, 4, in_dir);
    tbl_SerializeFromObject_TD_8847_input.addCol("l_partkey", 4);
    tbl_SerializeFromObject_TD_8847_input.addCol("l_suppkey", 4);
    tbl_SerializeFromObject_TD_8847_input.addCol("l_quantity", 4);
    tbl_SerializeFromObject_TD_8847_input.addCol("l_shipdate", 4);
    tbl_SerializeFromObject_TD_8847_input.allocateHost();
    tbl_SerializeFromObject_TD_8847_input.loadHost();
    Table tbl_SerializeFromObject_TD_8954_input;
    tbl_SerializeFromObject_TD_8954_input = Table("part", part_n, 2, in_dir);
    tbl_SerializeFromObject_TD_8954_input.addCol("p_partkey", 4);
    tbl_SerializeFromObject_TD_8954_input.addCol("p_name", TPCH_READ_P_NAME_LEN+1);
    tbl_SerializeFromObject_TD_8954_input.allocateHost();
    tbl_SerializeFromObject_TD_8954_input.loadHost();
    // ********************** Allocate Device Buffer ******************** // 
    tbl_JOIN_INNER_TD_1763_output.allocateDevBuffer(context_h, 32);
    Table tbl_JOIN_INNER_TD_1763_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_JOIN_INNER_TD_1763_output_partition_array[i] = tbl_JOIN_INNER_TD_1763_output.createSubTable(i);
    }
    tbl_JOIN_LEFTSEMI_TD_2112_output.allocateDevBuffer(context_h, 32);
    Table tbl_JOIN_LEFTSEMI_TD_2112_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_JOIN_LEFTSEMI_TD_2112_output_partition_array[i] = tbl_JOIN_LEFTSEMI_TD_2112_output.createSubTable(i);
    }
    tbl_Filter_TD_2221_output.allocateDevBuffer(context_h, 33);
    tbl_SerializeFromObject_TD_3404_input_stringRowIDSubstitute.allocateDevBuffer(context_h, 33);
    tbl_JOIN_INNER_TD_3971_output.allocateDevBuffer(context_h, 32);
    Table tbl_JOIN_INNER_TD_3971_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_JOIN_INNER_TD_3971_output_partition_array[i] = tbl_JOIN_INNER_TD_3971_output.createSubTable(i);
    }
    tbl_JOIN_LEFTSEMI_TD_487_output.allocateDevBuffer(context_h, 32);
    Table tbl_JOIN_LEFTSEMI_TD_487_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_JOIN_LEFTSEMI_TD_487_output_partition_array[i] = tbl_JOIN_LEFTSEMI_TD_487_output.createSubTable(i);
    }
    tbl_Filter_TD_4970_output.allocateDevBuffer(context_h, 33);
    tbl_SerializeFromObject_TD_5662_input.allocateDevBuffer(context_h, 33);
    tbl_Filter_TD_5130_output.allocateDevBuffer(context_h, 33);
    tbl_JOIN_LEFTSEMI_TD_6400_output.allocateDevBuffer(context_h, 32);
    Table tbl_JOIN_LEFTSEMI_TD_6400_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_JOIN_LEFTSEMI_TD_6400_output_partition_array[i] = tbl_JOIN_LEFTSEMI_TD_6400_output.createSubTable(i);
    }
    tbl_Filter_TD_7493_output.allocateDevBuffer(context_h, 33);
    tbl_Filter_TD_7282_output.allocateDevBuffer(context_h, 33);
    // ****************************** Config **************************** // 
    
    xf::database::gqe::KernelCommand krn_cmd_build_969 = xf::database::gqe::KernelCommand();
    xf::database::gqe::KernelCommand krn_cmd_probe_969 = xf::database::gqe::KernelCommand();
    cfgCmd cfg_JOIN_INNER_TD_1763_cmds_build;
    cfg_JOIN_INNER_TD_1763_cmds_build.setup(1);
    cfg_JOIN_INNER_TD_1763_cmds_build.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_1763_gqe_join_build(krn_cmd_build_969);
    cfg_JOIN_INNER_TD_1763_cmds_build.cmd = krn_cmd_build_969.getConfigBits();
    get_meta_info_JOIN_INNER_TD_1763_gqe_join_build(cfg_JOIN_INNER_TD_1763_cmds_build.meta_in, tbl_JOIN_LEFTSEMI_TD_2112_consolidated_output.nrow, 3);
    get_meta_info_JOIN_INNER_TD_1763_gqe_join_build(cfg_JOIN_INNER_TD_1763_cmds_build.meta_out, tbl_JOIN_INNER_TD_1763_output.nrow, 4);
    // cfg_JOIN_INNER_TD_1763_cmds_build.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_1763_cmds_probe;
    cfg_JOIN_INNER_TD_1763_cmds_probe.setup(1);
    cfg_JOIN_INNER_TD_1763_cmds_probe.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_1763_gqe_join_probe(krn_cmd_probe_969);
    cfg_JOIN_INNER_TD_1763_cmds_probe.cmd = krn_cmd_probe_969.getConfigBits();
    get_meta_info_JOIN_INNER_TD_1763_gqe_join_probe(cfg_JOIN_INNER_TD_1763_cmds_probe.meta_in, tbl_Filter_TD_2221_output.nrow, 3);
    get_meta_info_JOIN_INNER_TD_1763_gqe_join_probe(cfg_JOIN_INNER_TD_1763_cmds_probe.meta_out, tbl_JOIN_INNER_TD_1763_output.nrow, 3);
    // cfg_JOIN_INNER_TD_1763_cmds_probe.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_1763_cmds_part;
    cfg_JOIN_INNER_TD_1763_cmds_part.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_1763_gqe_join_part (cfg_JOIN_INNER_TD_1763_cmds_part.cmd);
    cfg_JOIN_INNER_TD_1763_cmds_part.allocateDevBuffer(context_h, 32);
    cfg_JOIN_INNER_TD_1763_cmds_build.input_key_columns = {2, -1, -1};
    cfg_JOIN_INNER_TD_1763_cmds_probe.input_key_columns = {0, -1, -1};
    cfg_JOIN_INNER_TD_1763_cmds_build.output_key_columns = {0, 1, 2, 3};
    cfg_JOIN_INNER_TD_1763_cmds_probe.output_key_columns = {0, 1, 2, 3};
    
    xf::database::gqe::KernelCommand krn_cmd_build_339 = xf::database::gqe::KernelCommand();
    xf::database::gqe::KernelCommand krn_cmd_probe_339 = xf::database::gqe::KernelCommand();
    cfgCmd cfg_JOIN_LEFTSEMI_TD_2112_cmds_build;
    cfg_JOIN_LEFTSEMI_TD_2112_cmds_build.setup(1);
    cfg_JOIN_LEFTSEMI_TD_2112_cmds_build.allocateHost();
    get_cfg_dat_JOIN_LEFTSEMI_TD_2112_gqe_join_build(krn_cmd_build_339);
    cfg_JOIN_LEFTSEMI_TD_2112_cmds_build.cmd = krn_cmd_build_339.getConfigBits();
    get_meta_info_JOIN_LEFTSEMI_TD_2112_gqe_join_build(cfg_JOIN_LEFTSEMI_TD_2112_cmds_build.meta_in, tbl_JOIN_INNER_TD_3971_consolidated_output.nrow, 3);
    get_meta_info_JOIN_LEFTSEMI_TD_2112_gqe_join_build(cfg_JOIN_LEFTSEMI_TD_2112_cmds_build.meta_out, tbl_JOIN_LEFTSEMI_TD_2112_output.nrow, 4);
    // cfg_JOIN_LEFTSEMI_TD_2112_cmds_build.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_LEFTSEMI_TD_2112_cmds_probe;
    cfg_JOIN_LEFTSEMI_TD_2112_cmds_probe.setup(1);
    cfg_JOIN_LEFTSEMI_TD_2112_cmds_probe.allocateHost();
    get_cfg_dat_JOIN_LEFTSEMI_TD_2112_gqe_join_probe(krn_cmd_probe_339);
    cfg_JOIN_LEFTSEMI_TD_2112_cmds_probe.cmd = krn_cmd_probe_339.getConfigBits();
    get_meta_info_JOIN_LEFTSEMI_TD_2112_gqe_join_probe(cfg_JOIN_LEFTSEMI_TD_2112_cmds_probe.meta_in, tbl_SerializeFromObject_TD_3404_input_stringRowIDSubstitute.nrow, 3);
    get_meta_info_JOIN_LEFTSEMI_TD_2112_gqe_join_probe(cfg_JOIN_LEFTSEMI_TD_2112_cmds_probe.meta_out, tbl_JOIN_LEFTSEMI_TD_2112_output.nrow, 3);
    // cfg_JOIN_LEFTSEMI_TD_2112_cmds_probe.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_LEFTSEMI_TD_2112_cmds_part;
    cfg_JOIN_LEFTSEMI_TD_2112_cmds_part.allocateHost();
    get_cfg_dat_JOIN_LEFTSEMI_TD_2112_gqe_join_part (cfg_JOIN_LEFTSEMI_TD_2112_cmds_part.cmd);
    cfg_JOIN_LEFTSEMI_TD_2112_cmds_part.allocateDevBuffer(context_h, 32);
    cfg_JOIN_LEFTSEMI_TD_2112_cmds_build.input_key_columns = {0, -1, -1};
    cfg_JOIN_LEFTSEMI_TD_2112_cmds_probe.input_key_columns = {0, -1, -1};
    cfg_JOIN_LEFTSEMI_TD_2112_cmds_build.output_key_columns = {0, 1, 2, 3};
    cfg_JOIN_LEFTSEMI_TD_2112_cmds_probe.output_key_columns = {0, 1, 2, 3};
    
    xf::database::gqe::KernelCommand krn_cmd_build_920 = xf::database::gqe::KernelCommand();
    xf::database::gqe::KernelCommand krn_cmd_probe_920 = xf::database::gqe::KernelCommand();
    cfgCmd cfg_JOIN_INNER_TD_3971_cmds_build;
    cfg_JOIN_INNER_TD_3971_cmds_build.setup(1);
    cfg_JOIN_INNER_TD_3971_cmds_build.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_3971_gqe_join_build(krn_cmd_build_920);
    cfg_JOIN_INNER_TD_3971_cmds_build.cmd = krn_cmd_build_920.getConfigBits();
    get_meta_info_JOIN_INNER_TD_3971_gqe_join_build(cfg_JOIN_INNER_TD_3971_cmds_build.meta_in, tbl_JOIN_LEFTSEMI_TD_487_consolidated_output.nrow, 3);
    get_meta_info_JOIN_INNER_TD_3971_gqe_join_build(cfg_JOIN_INNER_TD_3971_cmds_build.meta_out, tbl_JOIN_INNER_TD_3971_output.nrow, 4);
    // cfg_JOIN_INNER_TD_3971_cmds_build.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_3971_cmds_probe;
    cfg_JOIN_INNER_TD_3971_cmds_probe.setup(1);
    cfg_JOIN_INNER_TD_3971_cmds_probe.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_3971_gqe_join_probe(krn_cmd_probe_920);
    cfg_JOIN_INNER_TD_3971_cmds_probe.cmd = krn_cmd_probe_920.getConfigBits();
    get_meta_info_JOIN_INNER_TD_3971_gqe_join_probe(cfg_JOIN_INNER_TD_3971_cmds_probe.meta_in, tbl_Filter_TD_4970_output.nrow, 3);
    get_meta_info_JOIN_INNER_TD_3971_gqe_join_probe(cfg_JOIN_INNER_TD_3971_cmds_probe.meta_out, tbl_JOIN_INNER_TD_3971_output.nrow, 3);
    // cfg_JOIN_INNER_TD_3971_cmds_probe.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_3971_cmds_part;
    cfg_JOIN_INNER_TD_3971_cmds_part.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_3971_gqe_join_part (cfg_JOIN_INNER_TD_3971_cmds_part.cmd);
    cfg_JOIN_INNER_TD_3971_cmds_part.allocateDevBuffer(context_h, 32);
    cfg_JOIN_INNER_TD_3971_cmds_build.input_key_columns = {0, 1, -1};
    cfg_JOIN_INNER_TD_3971_cmds_probe.input_key_columns = {1, 2, -1};
    cfg_JOIN_INNER_TD_3971_cmds_build.output_key_columns = {0, 1, 2, 3};
    cfg_JOIN_INNER_TD_3971_cmds_probe.output_key_columns = {0, 1, 2, 3};
    
    xf::database::gqe::KernelCommand krn_cmd_build_972 = xf::database::gqe::KernelCommand();
    xf::database::gqe::KernelCommand krn_cmd_probe_972 = xf::database::gqe::KernelCommand();
    cfgCmd cfg_JOIN_LEFTSEMI_TD_487_cmds_build;
    cfg_JOIN_LEFTSEMI_TD_487_cmds_build.setup(1);
    cfg_JOIN_LEFTSEMI_TD_487_cmds_build.allocateHost();
    get_cfg_dat_JOIN_LEFTSEMI_TD_487_gqe_join_build(krn_cmd_build_972);
    cfg_JOIN_LEFTSEMI_TD_487_cmds_build.cmd = krn_cmd_build_972.getConfigBits();
    get_meta_info_JOIN_LEFTSEMI_TD_487_gqe_join_build(cfg_JOIN_LEFTSEMI_TD_487_cmds_build.meta_in, tbl_Filter_TD_5130_output.nrow, 3);
    get_meta_info_JOIN_LEFTSEMI_TD_487_gqe_join_build(cfg_JOIN_LEFTSEMI_TD_487_cmds_build.meta_out, tbl_JOIN_LEFTSEMI_TD_487_output.nrow, 4);
    // cfg_JOIN_LEFTSEMI_TD_487_cmds_build.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_LEFTSEMI_TD_487_cmds_probe;
    cfg_JOIN_LEFTSEMI_TD_487_cmds_probe.setup(1);
    cfg_JOIN_LEFTSEMI_TD_487_cmds_probe.allocateHost();
    get_cfg_dat_JOIN_LEFTSEMI_TD_487_gqe_join_probe(krn_cmd_probe_972);
    cfg_JOIN_LEFTSEMI_TD_487_cmds_probe.cmd = krn_cmd_probe_972.getConfigBits();
    get_meta_info_JOIN_LEFTSEMI_TD_487_gqe_join_probe(cfg_JOIN_LEFTSEMI_TD_487_cmds_probe.meta_in, tbl_SerializeFromObject_TD_5662_input.nrow, 3);
    get_meta_info_JOIN_LEFTSEMI_TD_487_gqe_join_probe(cfg_JOIN_LEFTSEMI_TD_487_cmds_probe.meta_out, tbl_JOIN_LEFTSEMI_TD_487_output.nrow, 3);
    // cfg_JOIN_LEFTSEMI_TD_487_cmds_probe.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_LEFTSEMI_TD_487_cmds_part;
    cfg_JOIN_LEFTSEMI_TD_487_cmds_part.allocateHost();
    get_cfg_dat_JOIN_LEFTSEMI_TD_487_gqe_join_part (cfg_JOIN_LEFTSEMI_TD_487_cmds_part.cmd);
    cfg_JOIN_LEFTSEMI_TD_487_cmds_part.allocateDevBuffer(context_h, 32);
    cfg_JOIN_LEFTSEMI_TD_487_cmds_build.input_key_columns = {0, -1, -1};
    cfg_JOIN_LEFTSEMI_TD_487_cmds_probe.input_key_columns = {0, -1, -1};
    cfg_JOIN_LEFTSEMI_TD_487_cmds_build.output_key_columns = {0, 1, 2, 3};
    cfg_JOIN_LEFTSEMI_TD_487_cmds_probe.output_key_columns = {0, 1, 2, 3};
    
    xf::database::gqe::KernelCommand krn_cmd_build_296 = xf::database::gqe::KernelCommand();
    xf::database::gqe::KernelCommand krn_cmd_probe_296 = xf::database::gqe::KernelCommand();
    cfgCmd cfg_JOIN_LEFTSEMI_TD_6400_cmds_build;
    cfg_JOIN_LEFTSEMI_TD_6400_cmds_build.setup(1);
    cfg_JOIN_LEFTSEMI_TD_6400_cmds_build.allocateHost();
    get_cfg_dat_JOIN_LEFTSEMI_TD_6400_gqe_join_build(krn_cmd_build_296);
    cfg_JOIN_LEFTSEMI_TD_6400_cmds_build.cmd = krn_cmd_build_296.getConfigBits();
    get_meta_info_JOIN_LEFTSEMI_TD_6400_gqe_join_build(cfg_JOIN_LEFTSEMI_TD_6400_cmds_build.meta_in, tbl_Filter_TD_7282_output.nrow, 3);
    get_meta_info_JOIN_LEFTSEMI_TD_6400_gqe_join_build(cfg_JOIN_LEFTSEMI_TD_6400_cmds_build.meta_out, tbl_JOIN_LEFTSEMI_TD_6400_output.nrow, 4);
    // cfg_JOIN_LEFTSEMI_TD_6400_cmds_build.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_LEFTSEMI_TD_6400_cmds_probe;
    cfg_JOIN_LEFTSEMI_TD_6400_cmds_probe.setup(1);
    cfg_JOIN_LEFTSEMI_TD_6400_cmds_probe.allocateHost();
    get_cfg_dat_JOIN_LEFTSEMI_TD_6400_gqe_join_probe(krn_cmd_probe_296);
    cfg_JOIN_LEFTSEMI_TD_6400_cmds_probe.cmd = krn_cmd_probe_296.getConfigBits();
    get_meta_info_JOIN_LEFTSEMI_TD_6400_gqe_join_probe(cfg_JOIN_LEFTSEMI_TD_6400_cmds_probe.meta_in, tbl_Filter_TD_7493_output.nrow, 3);
    get_meta_info_JOIN_LEFTSEMI_TD_6400_gqe_join_probe(cfg_JOIN_LEFTSEMI_TD_6400_cmds_probe.meta_out, tbl_JOIN_LEFTSEMI_TD_6400_output.nrow, 3);
    // cfg_JOIN_LEFTSEMI_TD_6400_cmds_probe.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_LEFTSEMI_TD_6400_cmds_part;
    cfg_JOIN_LEFTSEMI_TD_6400_cmds_part.allocateHost();
    get_cfg_dat_JOIN_LEFTSEMI_TD_6400_gqe_join_part (cfg_JOIN_LEFTSEMI_TD_6400_cmds_part.cmd);
    cfg_JOIN_LEFTSEMI_TD_6400_cmds_part.allocateDevBuffer(context_h, 32);
    cfg_JOIN_LEFTSEMI_TD_6400_cmds_build.input_key_columns = {0, -1, -1};
    cfg_JOIN_LEFTSEMI_TD_6400_cmds_probe.input_key_columns = {0, -1, -1};
    cfg_JOIN_LEFTSEMI_TD_6400_cmds_build.output_key_columns = {0, 1, 2, 3};
    cfg_JOIN_LEFTSEMI_TD_6400_cmds_probe.output_key_columns = {0, 1, 2, 3};
    // *************************** Kernel Setup ************************* // 
    bufferTmp buftmp_h(context_h, 1); 
    buftmp_h.initBuffer(q_h); 
    std::cout << std::endl; 
    krnlEngine krnl_JOIN_INNER_TD_1763_part_right;
    krnl_JOIN_INNER_TD_1763_part_right = krnlEngine(program_h, q_h, context_h, "gqePart");
    krnl_JOIN_INNER_TD_1763_part_right.setup_hp(512, 1, power_of_hpTimes_join, tbl_Filter_TD_2221_output, tbl_Filter_TD_2221_output_partition, cfg_JOIN_INNER_TD_1763_cmds_part);
    krnlEngine krnl_JOIN_INNER_TD_1763[hpTimes_join];
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_INNER_TD_1763[i] = krnlEngine(program_h, q_h, "gqeJoin");
    }
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_INNER_TD_1763[i].setup(tbl_JOIN_LEFTSEMI_TD_2112_consolidated_output_partition_array[i], tbl_Filter_TD_2221_output_partition_array[i], tbl_JOIN_INNER_TD_1763_output_partition_array[i], , buftmp_h);
    }
    krnlEngine krnl_JOIN_INNER_TD_1763_build;
    krnl_JOIN_INNER_TD_1763_build = krnlEngine(program_h, q_h, context_h, "gqeKernel", 0);
    krnl_JOIN_INNER_TD_1763_build.setup(tbl_JOIN_LEFTSEMI_TD_2112_consolidated_output, tbl_Filter_TD_2221_output, tbl_JOIN_INNER_TD_1763_output, cfg_JOIN_INNER_TD_1763_cmds_build, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_1763_probe;
    krnl_JOIN_INNER_TD_1763_probe = krnlEngine(program_h, q_h, context_h, "gqeKernel", 1);
    krnl_JOIN_INNER_TD_1763_probe.setup(tbl_Filter_TD_2221_output, tbl_JOIN_LEFTSEMI_TD_2112_consolidated_output, tbl_JOIN_INNER_TD_1763_output, cfg_JOIN_INNER_TD_1763_cmds_probe, buftmp_h);
    krnlEngine krnl_JOIN_LEFTSEMI_TD_2112_part_right;
    krnl_JOIN_LEFTSEMI_TD_2112_part_right = krnlEngine(program_h, q_h, context_h, "gqePart");
    krnl_JOIN_LEFTSEMI_TD_2112_part_right.setup_hp(512, 1, power_of_hpTimes_join, tbl_SerializeFromObject_TD_3404_input_stringRowIDSubstitute, tbl_SerializeFromObject_TD_3404_input_stringRowIDSubstitute_partition, cfg_JOIN_LEFTSEMI_TD_2112_cmds_part);
    krnlEngine krnl_JOIN_LEFTSEMI_TD_2112[hpTimes_join];
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_LEFTSEMI_TD_2112[i] = krnlEngine(program_h, q_h, "gqeJoin");
    }
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_LEFTSEMI_TD_2112[i].setup(tbl_JOIN_INNER_TD_3971_consolidated_output_partition_array[i], tbl_SerializeFromObject_TD_3404_input_stringRowIDSubstitute_partition_array[i], tbl_JOIN_LEFTSEMI_TD_2112_output_partition_array[i], , buftmp_h);
    }
    krnlEngine krnl_JOIN_LEFTSEMI_TD_2112_build;
    krnl_JOIN_LEFTSEMI_TD_2112_build = krnlEngine(program_h, q_h, context_h, "gqeKernel", 0);
    krnl_JOIN_LEFTSEMI_TD_2112_build.setup(tbl_JOIN_INNER_TD_3971_consolidated_output, tbl_SerializeFromObject_TD_3404_input_stringRowIDSubstitute, tbl_JOIN_LEFTSEMI_TD_2112_output, cfg_JOIN_LEFTSEMI_TD_2112_cmds_build, buftmp_h);
    krnlEngine krnl_JOIN_LEFTSEMI_TD_2112_probe;
    krnl_JOIN_LEFTSEMI_TD_2112_probe = krnlEngine(program_h, q_h, context_h, "gqeKernel", 1);
    krnl_JOIN_LEFTSEMI_TD_2112_probe.setup(tbl_SerializeFromObject_TD_3404_input_stringRowIDSubstitute, tbl_JOIN_INNER_TD_3971_consolidated_output, tbl_JOIN_LEFTSEMI_TD_2112_output, cfg_JOIN_LEFTSEMI_TD_2112_cmds_probe, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_3971_part_right;
    krnl_JOIN_INNER_TD_3971_part_right = krnlEngine(program_h, q_h, context_h, "gqePart");
    krnl_JOIN_INNER_TD_3971_part_right.setup_hp(512, 1, power_of_hpTimes_join, tbl_Filter_TD_4970_output, tbl_Filter_TD_4970_output_partition, cfg_JOIN_INNER_TD_3971_cmds_part);
    krnlEngine krnl_JOIN_INNER_TD_3971[hpTimes_join];
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_INNER_TD_3971[i] = krnlEngine(program_h, q_h, "gqeJoin");
    }
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_INNER_TD_3971[i].setup(tbl_JOIN_LEFTSEMI_TD_487_consolidated_output_partition_array[i], tbl_Filter_TD_4970_output_partition_array[i], tbl_JOIN_INNER_TD_3971_output_partition_array[i], , buftmp_h);
    }
    krnlEngine krnl_JOIN_INNER_TD_3971_build;
    krnl_JOIN_INNER_TD_3971_build = krnlEngine(program_h, q_h, context_h, "gqeKernel", 0);
    krnl_JOIN_INNER_TD_3971_build.setup(tbl_JOIN_LEFTSEMI_TD_487_consolidated_output, tbl_Filter_TD_4970_output, tbl_JOIN_INNER_TD_3971_output, cfg_JOIN_INNER_TD_3971_cmds_build, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_3971_probe;
    krnl_JOIN_INNER_TD_3971_probe = krnlEngine(program_h, q_h, context_h, "gqeKernel", 1);
    krnl_JOIN_INNER_TD_3971_probe.setup(tbl_Filter_TD_4970_output, tbl_JOIN_LEFTSEMI_TD_487_consolidated_output, tbl_JOIN_INNER_TD_3971_output, cfg_JOIN_INNER_TD_3971_cmds_probe, buftmp_h);
    krnlEngine krnl_JOIN_LEFTSEMI_TD_487_part_left;
    krnl_JOIN_LEFTSEMI_TD_487_part_left = krnlEngine(program_h, q_h, context_h, "gqePart");
    krnl_JOIN_LEFTSEMI_TD_487_part_left.setup_hp(512, 0, power_of_hpTimes_join, tbl_Filter_TD_5130_output, tbl_Filter_TD_5130_output_partition, cfg_JOIN_LEFTSEMI_TD_487_cmds_part);
    krnlEngine krnl_JOIN_LEFTSEMI_TD_487_part_right;
    krnl_JOIN_LEFTSEMI_TD_487_part_right = krnlEngine(program_h, q_h, context_h, "gqePart");
    krnl_JOIN_LEFTSEMI_TD_487_part_right.setup_hp(512, 1, power_of_hpTimes_join, tbl_SerializeFromObject_TD_5662_input, tbl_SerializeFromObject_TD_5662_input_partition, cfg_JOIN_LEFTSEMI_TD_487_cmds_part);
    krnlEngine krnl_JOIN_LEFTSEMI_TD_487[hpTimes_join];
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_LEFTSEMI_TD_487[i] = krnlEngine(program_h, q_h, "gqeJoin");
    }
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_LEFTSEMI_TD_487[i].setup(tbl_Filter_TD_5130_output_partition_array[i], tbl_SerializeFromObject_TD_5662_input_partition_array[i], tbl_JOIN_LEFTSEMI_TD_487_output_partition_array[i], , buftmp_h);
    }
    krnlEngine krnl_JOIN_LEFTSEMI_TD_487_build;
    krnl_JOIN_LEFTSEMI_TD_487_build = krnlEngine(program_h, q_h, context_h, "gqeKernel", 0);
    krnl_JOIN_LEFTSEMI_TD_487_build.setup(tbl_Filter_TD_5130_output, tbl_SerializeFromObject_TD_5662_input, tbl_JOIN_LEFTSEMI_TD_487_output, cfg_JOIN_LEFTSEMI_TD_487_cmds_build, buftmp_h);
    krnlEngine krnl_JOIN_LEFTSEMI_TD_487_probe;
    krnl_JOIN_LEFTSEMI_TD_487_probe = krnlEngine(program_h, q_h, context_h, "gqeKernel", 1);
    krnl_JOIN_LEFTSEMI_TD_487_probe.setup(tbl_SerializeFromObject_TD_5662_input, tbl_Filter_TD_5130_output, tbl_JOIN_LEFTSEMI_TD_487_output, cfg_JOIN_LEFTSEMI_TD_487_cmds_probe, buftmp_h);
    krnlEngine krnl_JOIN_LEFTSEMI_TD_6400_part_left;
    krnl_JOIN_LEFTSEMI_TD_6400_part_left = krnlEngine(program_h, q_h, context_h, "gqePart");
    krnl_JOIN_LEFTSEMI_TD_6400_part_left.setup_hp(512, 0, power_of_hpTimes_join, tbl_Filter_TD_7282_output, tbl_Filter_TD_7282_output_partition, cfg_JOIN_LEFTSEMI_TD_6400_cmds_part);
    krnlEngine krnl_JOIN_LEFTSEMI_TD_6400_part_right;
    krnl_JOIN_LEFTSEMI_TD_6400_part_right = krnlEngine(program_h, q_h, context_h, "gqePart");
    krnl_JOIN_LEFTSEMI_TD_6400_part_right.setup_hp(512, 1, power_of_hpTimes_join, tbl_Filter_TD_7493_output, tbl_Filter_TD_7493_output_partition, cfg_JOIN_LEFTSEMI_TD_6400_cmds_part);
    krnlEngine krnl_JOIN_LEFTSEMI_TD_6400[hpTimes_join];
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_LEFTSEMI_TD_6400[i] = krnlEngine(program_h, q_h, "gqeJoin");
    }
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_LEFTSEMI_TD_6400[i].setup(tbl_Filter_TD_7282_output_partition_array[i], tbl_Filter_TD_7493_output_partition_array[i], tbl_JOIN_LEFTSEMI_TD_6400_output_partition_array[i], , buftmp_h);
    }
    krnlEngine krnl_JOIN_LEFTSEMI_TD_6400_build;
    krnl_JOIN_LEFTSEMI_TD_6400_build = krnlEngine(program_h, q_h, context_h, "gqeKernel", 0);
    krnl_JOIN_LEFTSEMI_TD_6400_build.setup(tbl_Filter_TD_7282_output, tbl_Filter_TD_7493_output, tbl_JOIN_LEFTSEMI_TD_6400_output, cfg_JOIN_LEFTSEMI_TD_6400_cmds_build, buftmp_h);
    krnlEngine krnl_JOIN_LEFTSEMI_TD_6400_probe;
    krnl_JOIN_LEFTSEMI_TD_6400_probe = krnlEngine(program_h, q_h, context_h, "gqeKernel", 1);
    krnl_JOIN_LEFTSEMI_TD_6400_probe.setup(tbl_Filter_TD_7493_output, tbl_Filter_TD_7282_output, tbl_JOIN_LEFTSEMI_TD_6400_output, cfg_JOIN_LEFTSEMI_TD_6400_cmds_probe, buftmp_h);
    // ************************** Transfer Engine *********************** // 
    transEngine trans_JOIN_INNER_TD_1763;
    trans_JOIN_INNER_TD_1763.setq(q_h,1);
    trans_JOIN_INNER_TD_1763.add(&(cfg_JOIN_INNER_TD_1763_cmds_part));
    trans_JOIN_INNER_TD_1763.add(&(cfg_JOIN_INNER_TD_1763_cmds_build));
    trans_JOIN_INNER_TD_1763.add(&(cfg_JOIN_INNER_TD_1763_cmds_probe));
    transEngine trans_JOIN_INNER_TD_1763_out;
    trans_JOIN_INNER_TD_1763_out.setq(q_h,1);
    q_h.finish();
    transEngine trans_JOIN_LEFTSEMI_TD_2112;
    trans_JOIN_LEFTSEMI_TD_2112.setq(q_h,1);
    trans_JOIN_LEFTSEMI_TD_2112.add(&(cfg_JOIN_LEFTSEMI_TD_2112_cmds_part));
    trans_JOIN_LEFTSEMI_TD_2112.add(&(cfg_JOIN_LEFTSEMI_TD_2112_cmds_build));
    trans_JOIN_LEFTSEMI_TD_2112.add(&(cfg_JOIN_LEFTSEMI_TD_2112_cmds_probe));
    trans_JOIN_LEFTSEMI_TD_2112.add(&(tbl_SerializeFromObject_TD_3404_input_stringRowIDSubstitute));
    transEngine trans_JOIN_LEFTSEMI_TD_2112_out;
    trans_JOIN_LEFTSEMI_TD_2112_out.setq(q_h,1);
    q_h.finish();
    transEngine trans_JOIN_INNER_TD_3971;
    trans_JOIN_INNER_TD_3971.setq(q_h,1);
    trans_JOIN_INNER_TD_3971.add(&(cfg_JOIN_INNER_TD_3971_cmds_part));
    trans_JOIN_INNER_TD_3971.add(&(cfg_JOIN_INNER_TD_3971_cmds_build));
    trans_JOIN_INNER_TD_3971.add(&(cfg_JOIN_INNER_TD_3971_cmds_probe));
    transEngine trans_JOIN_INNER_TD_3971_out;
    trans_JOIN_INNER_TD_3971_out.setq(q_h,1);
    q_h.finish();
    transEngine trans_JOIN_LEFTSEMI_TD_487;
    trans_JOIN_LEFTSEMI_TD_487.setq(q_h,1);
    trans_JOIN_LEFTSEMI_TD_487.add(&(cfg_JOIN_LEFTSEMI_TD_487_cmds_part));
    trans_JOIN_LEFTSEMI_TD_487.add(&(cfg_JOIN_LEFTSEMI_TD_487_cmds_build));
    trans_JOIN_LEFTSEMI_TD_487.add(&(cfg_JOIN_LEFTSEMI_TD_487_cmds_probe));
    trans_JOIN_LEFTSEMI_TD_487.add(&(tbl_SerializeFromObject_TD_5662_input));
    transEngine trans_JOIN_LEFTSEMI_TD_487_out;
    trans_JOIN_LEFTSEMI_TD_487_out.setq(q_h,1);
    q_h.finish();
    transEngine trans_JOIN_LEFTSEMI_TD_6400;
    trans_JOIN_LEFTSEMI_TD_6400.setq(q_h,1);
    trans_JOIN_LEFTSEMI_TD_6400.add(&(cfg_JOIN_LEFTSEMI_TD_6400_cmds_build));
    trans_JOIN_LEFTSEMI_TD_6400.add(&(cfg_JOIN_LEFTSEMI_TD_6400_cmds_probe));
    transEngine trans_JOIN_LEFTSEMI_TD_6400_out;
    trans_JOIN_LEFTSEMI_TD_6400_out.setq(q_h,1);
    q_h.finish();
    // ****************************** Events **************************** // 
    std::vector<cl::Event> events_h2d_wr_JOIN_INNER_TD_1763;
    std::vector<cl::Event> events_d2h_rd_JOIN_INNER_TD_1763;
    std::vector<cl::Event> events_JOIN_INNER_TD_1763[2];
    events_h2d_wr_JOIN_INNER_TD_1763.resize(1);
    events_d2h_rd_JOIN_INNER_TD_1763.resize(1);
    events_JOIN_INNER_TD_1763[0].resize(1);
    events_JOIN_INNER_TD_1763[1].resize(hpTimes_join);
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_1763;
    std::vector<cl::Event> prev_events_grp_JOIN_INNER_TD_1763;
    std::vector<cl::Event> events_h2d_wr_JOIN_LEFTSEMI_TD_2112;
    std::vector<cl::Event> events_d2h_rd_JOIN_LEFTSEMI_TD_2112;
    std::vector<cl::Event> events_JOIN_LEFTSEMI_TD_2112[2];
    events_h2d_wr_JOIN_LEFTSEMI_TD_2112.resize(1);
    events_d2h_rd_JOIN_LEFTSEMI_TD_2112.resize(1);
    events_JOIN_LEFTSEMI_TD_2112[0].resize(1);
    events_JOIN_LEFTSEMI_TD_2112[1].resize(hpTimes_join);
    std::vector<cl::Event> events_grp_JOIN_LEFTSEMI_TD_2112;
    std::vector<cl::Event> prev_events_grp_JOIN_LEFTSEMI_TD_2112;
    std::vector<cl::Event> events_h2d_wr_JOIN_INNER_TD_3971;
    std::vector<cl::Event> events_d2h_rd_JOIN_INNER_TD_3971;
    std::vector<cl::Event> events_JOIN_INNER_TD_3971[2];
    events_h2d_wr_JOIN_INNER_TD_3971.resize(1);
    events_d2h_rd_JOIN_INNER_TD_3971.resize(1);
    events_JOIN_INNER_TD_3971[0].resize(1);
    events_JOIN_INNER_TD_3971[1].resize(hpTimes_join);
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_3971;
    std::vector<cl::Event> prev_events_grp_JOIN_INNER_TD_3971;
    std::vector<cl::Event> events_h2d_wr_JOIN_LEFTSEMI_TD_487;
    std::vector<cl::Event> events_d2h_rd_JOIN_LEFTSEMI_TD_487;
    std::vector<cl::Event> events_JOIN_LEFTSEMI_TD_487[2];
    events_h2d_wr_JOIN_LEFTSEMI_TD_487.resize(1);
    events_d2h_rd_JOIN_LEFTSEMI_TD_487.resize(1);
    events_JOIN_LEFTSEMI_TD_487[0].resize(2);
    events_JOIN_LEFTSEMI_TD_487[1].resize(hpTimes_join);
    std::vector<cl::Event> events_grp_JOIN_LEFTSEMI_TD_487;
    std::vector<cl::Event> prev_events_grp_JOIN_LEFTSEMI_TD_487;
    std::vector<cl::Event> events_h2d_wr_JOIN_LEFTSEMI_TD_6400;
    std::vector<cl::Event> events_d2h_rd_JOIN_LEFTSEMI_TD_6400;
    std::vector<cl::Event> events_JOIN_LEFTSEMI_TD_6400[2];
    events_h2d_wr_JOIN_LEFTSEMI_TD_6400.resize(1);
    events_d2h_rd_JOIN_LEFTSEMI_TD_6400.resize(1);
    events_JOIN_LEFTSEMI_TD_6400[0].resize(2);
    events_JOIN_LEFTSEMI_TD_6400[1].resize(hpTimes_join);
    std::vector<cl::Event> events_grp_JOIN_LEFTSEMI_TD_6400;
    std::vector<cl::Event> prev_events_grp_JOIN_LEFTSEMI_TD_6400;
    // **************************** Operations ************************** // 
    struct timeval tv_r_s, tv_r_e; 
    gettimeofday(&tv_r_s, 0); 

    struct timeval tv_r_Filter_7_793_s, tv_r_Filter_7_793_e;
    gettimeofday(&tv_r_Filter_7_793_s, 0);
    SW_Filter_TD_7282(tbl_SerializeFromObject_TD_8954_input, tbl_Filter_TD_7282_output);
    gettimeofday(&tv_r_Filter_7_793_e, 0);

    struct timeval tv_r_Filter_7_573_s, tv_r_Filter_7_573_e;
    gettimeofday(&tv_r_Filter_7_573_s, 0);
    SW_Filter_TD_7493(tbl_SerializeFromObject_TD_8847_input, tbl_Filter_TD_7493_output);
    gettimeofday(&tv_r_Filter_7_573_e, 0);

    struct timeval tv_r_JOIN_LEFTSEMI_6_226_s, tv_r_JOIN_LEFTSEMI_6_226_e;
    gettimeofday(&tv_r_JOIN_LEFTSEMI_6_226_s, 0);
    tbl_Filter_TD_7282_output.tableToCol();
    tbl_Filter_TD_7493_output.tableToCol();
    krnl_JOIN_LEFTSEMI_TD_6400_build.updateMeta(tbl_Filter_TD_7282_output.getNumRow(), 3);
    krnl_JOIN_LEFTSEMI_TD_6400_probe.updateMeta(tbl_Filter_TD_7493_output.getNumRow(), 3);
    trans_JOIN_LEFTSEMI_TD_6400.add(&(tbl_Filter_TD_7493_output));
    trans_JOIN_LEFTSEMI_TD_6400.add(&(tbl_Filter_TD_7282_output));
    trans_JOIN_LEFTSEMI_TD_6400.host2dev(0, &(prev_events_grp_JOIN_LEFTSEMI_TD_6400), &(events_h2d_wr_JOIN_LEFTSEMI_TD_6400[0]));
    events_grp_JOIN_LEFTSEMI_TD_6400.push_back(events_h2d_wr_JOIN_LEFTSEMI_TD_6400[0]);
    krnl_JOIN_LEFTSEMI_TD_6400_part_left.run(0, &(events_grp_JOIN_LEFTSEMI_TD_6400), &(events_JOIN_LEFTSEMI_TD_6400[0][0]));
    krnl_JOIN_LEFTSEMI_TD_6400_part_right.run(0, &(events_grp_JOIN_LEFTSEMI_TD_6400), &(events_JOIN_LEFTSEMI_TD_6400[0][1]));
    for (int i(0); i < hpTimes_join; ++i) {
        krnl_JOIN_LEFTSEMI_TD_6400[i].run(0, &(events_JOIN_LEFTSEMI_TD_6400[0]), &(events_JOIN_LEFTSEMI_TD_6400[1][i]));
    }
    std::vector<cl::Event> events_grp_JOIN_LEFTSEMI_TD_6400_build_done;
    events_grp_JOIN_LEFTSEMI_TD_6400_build_done.push_back(events_JOIN_LEFTSEMI_TD_6400[0]);
    krnl_JOIN_LEFTSEMI_TD_6400_part_left.run(0, &(events_grp_JOIN_LEFTSEMI_TD_6400), &(events_JOIN_LEFTSEMI_TD_6400[0][0]));
    krnl_JOIN_LEFTSEMI_TD_6400_part_right.run(0, &(events_grp_JOIN_LEFTSEMI_TD_6400), &(events_JOIN_LEFTSEMI_TD_6400[0][1]));
    for (int i(0); i < hpTimes_join; ++i) {
        krnl_JOIN_LEFTSEMI_TD_6400[i].run(0, &(events_JOIN_LEFTSEMI_TD_6400[0]), &(events_JOIN_LEFTSEMI_TD_6400[1][i]));
    }
    std::vector<cl::Event> events_grp_JOIN_LEFTSEMI_TD_6400_probe_done;
    events_grp_JOIN_LEFTSEMI_TD_6400_probe_done.push_back(events_JOIN_LEFTSEMI_TD_6400[1]);
    for (int i(0); i < hpTimes_join; ++i) {
        trans_JOIN_LEFTSEMI_TD_6400_out.add(&(tbl_JOIN_LEFTSEMI_TD_6400_output_partition_array[i]));
    }
    trans_JOIN_LEFTSEMI_TD_6400_out.dev2host(0, &(events_JOIN_LEFTSEMI_TD_6400[1]), &(events_d2h_rd_JOIN_LEFTSEMI_TD_6400[0]));
    q_h.flush();
    q_h.finish();
    tbl_JOIN_LEFTSEMI_TD_6400_output.setNumRow((cfg_JOIN_LEFTSEMI_TD_6400_cmds_probe.meta_out)->getColLen());
    tbl_JOIN_LEFTSEMI_TD_6400_output.colToTable();
    SW_Consolidated_JOIN_LEFTSEMI_TD_6400_output(tbl_Filter_TD_7282_output, tbl_Filter_TD_7493_output, tbl_JOIN_LEFTSEMI_TD_6400_output, tbl_JOIN_LEFTSEMI_TD_6400_consolidated_output);
    gettimeofday(&tv_r_JOIN_LEFTSEMI_6_226_e, 0);

    struct timeval tv_r_Aggregate_5_651_s, tv_r_Aggregate_5_651_e;
    gettimeofday(&tv_r_Aggregate_5_651_s, 0);
    SW_Aggregate_TD_5646(tbl_JOIN_LEFTSEMI_TD_6400_consolidated_output_partition_array, tbl_Aggregate_TD_5646_output, hpTimes_join);
    gettimeofday(&tv_r_Aggregate_5_651_e, 0);

    struct timeval tv_r_Filter_5_85_s, tv_r_Filter_5_85_e;
    gettimeofday(&tv_r_Filter_5_85_s, 0);
    SW_Filter_TD_5130(tbl_SerializeFromObject_TD_6257_input, tbl_Filter_TD_5130_output);
    gettimeofday(&tv_r_Filter_5_85_e, 0);

    struct timeval tv_r_Filter_4_924_s, tv_r_Filter_4_924_e;
    gettimeofday(&tv_r_Filter_4_924_s, 0);
    SW_Filter_TD_4970(tbl_Aggregate_TD_5646_output, tbl_Filter_TD_4970_output);
    gettimeofday(&tv_r_Filter_4_924_e, 0);

    struct timeval tv_r_JOIN_LEFTSEMI_4_444_s, tv_r_JOIN_LEFTSEMI_4_444_e;
    gettimeofday(&tv_r_JOIN_LEFTSEMI_4_444_s, 0);
    tbl_Filter_TD_5130_output.tableToCol();
    tbl_SerializeFromObject_TD_5662_input.tableToCol();
    krnl_JOIN_LEFTSEMI_TD_487_build.updateMeta(tbl_Filter_TD_5130_output.getNumRow(), 3);
    krnl_JOIN_LEFTSEMI_TD_487_probe.updateMeta(tbl_SerializeFromObject_TD_5662_input.getNumRow(), 3);
    trans_JOIN_LEFTSEMI_TD_487.add(&(tbl_Filter_TD_5130_output));
    trans_JOIN_LEFTSEMI_TD_487.host2dev(0, &(prev_events_grp_JOIN_LEFTSEMI_TD_487), &(events_h2d_wr_JOIN_LEFTSEMI_TD_487[0]));
    events_grp_JOIN_LEFTSEMI_TD_487.push_back(events_h2d_wr_JOIN_LEFTSEMI_TD_487[0]);
    krnl_JOIN_LEFTSEMI_TD_487_part_left.run(0, &(events_grp_JOIN_LEFTSEMI_TD_487), &(events_JOIN_LEFTSEMI_TD_487[0][0]));
    krnl_JOIN_LEFTSEMI_TD_487_part_right.run(0, &(events_grp_JOIN_LEFTSEMI_TD_487), &(events_JOIN_LEFTSEMI_TD_487[0][1]));
    for (int i(0); i < hpTimes_join; ++i) {
        krnl_JOIN_LEFTSEMI_TD_487[i].run(0, &(events_JOIN_LEFTSEMI_TD_487[0]), &(events_JOIN_LEFTSEMI_TD_487[1][i]));
    }
    std::vector<cl::Event> events_grp_JOIN_LEFTSEMI_TD_487_build_done;
    events_grp_JOIN_LEFTSEMI_TD_487_build_done.push_back(events_JOIN_LEFTSEMI_TD_487[0]);
    krnl_JOIN_LEFTSEMI_TD_487_part_left.run(0, &(events_grp_JOIN_LEFTSEMI_TD_487), &(events_JOIN_LEFTSEMI_TD_487[0][0]));
    krnl_JOIN_LEFTSEMI_TD_487_part_right.run(0, &(events_grp_JOIN_LEFTSEMI_TD_487), &(events_JOIN_LEFTSEMI_TD_487[0][1]));
    for (int i(0); i < hpTimes_join; ++i) {
        krnl_JOIN_LEFTSEMI_TD_487[i].run(0, &(events_JOIN_LEFTSEMI_TD_487[0]), &(events_JOIN_LEFTSEMI_TD_487[1][i]));
    }
    std::vector<cl::Event> events_grp_JOIN_LEFTSEMI_TD_487_probe_done;
    events_grp_JOIN_LEFTSEMI_TD_487_probe_done.push_back(events_JOIN_LEFTSEMI_TD_487[1]);
    for (int i(0); i < hpTimes_join; ++i) {
        trans_JOIN_LEFTSEMI_TD_487_out.add(&(tbl_JOIN_LEFTSEMI_TD_487_output_partition_array[i]));
    }
    trans_JOIN_LEFTSEMI_TD_487_out.dev2host(0, &(events_JOIN_LEFTSEMI_TD_487[1]), &(events_d2h_rd_JOIN_LEFTSEMI_TD_487[0]));
    q_h.flush();
    q_h.finish();
    tbl_JOIN_LEFTSEMI_TD_487_output.setNumRow((cfg_JOIN_LEFTSEMI_TD_487_cmds_probe.meta_out)->getColLen());
    tbl_JOIN_LEFTSEMI_TD_487_output.colToTable();
    SW_Consolidated_JOIN_LEFTSEMI_TD_487_output(tbl_Filter_TD_5130_output, tbl_SerializeFromObject_TD_5662_input, tbl_JOIN_LEFTSEMI_TD_487_output, tbl_JOIN_LEFTSEMI_TD_487_consolidated_output);
    gettimeofday(&tv_r_JOIN_LEFTSEMI_4_444_e, 0);

    struct timeval tv_r_JOIN_INNER_3_196_s, tv_r_JOIN_INNER_3_196_e;
    gettimeofday(&tv_r_JOIN_INNER_3_196_s, 0);
    prev_events_grp_JOIN_INNER_TD_3971.push_back(events_h2d_wr_JOIN_LEFTSEMI_TD_487[0]);
    tbl_JOIN_LEFTSEMI_TD_487_consolidated_output.tableToCol();
    tbl_Filter_TD_4970_output.tableToCol();
    krnl_JOIN_INNER_TD_3971_build.updateMeta(tbl_JOIN_LEFTSEMI_TD_487_consolidated_output.getNumRow(), 3);
    krnl_JOIN_INNER_TD_3971_probe.updateMeta(tbl_Filter_TD_4970_output.getNumRow(), 3);
    trans_JOIN_INNER_TD_3971.add(&(tbl_Filter_TD_4970_output));
    trans_JOIN_INNER_TD_3971.host2dev(0, &(prev_events_grp_JOIN_INNER_TD_3971), &(events_h2d_wr_JOIN_INNER_TD_3971[0]));
    events_grp_JOIN_INNER_TD_3971.push_back(events_h2d_wr_JOIN_INNER_TD_3971[0]);
    events_grp_JOIN_INNER_TD_3971.insert(std::end(events_grp_JOIN_INNER_TD_3971), std::begin(events_JOIN_LEFTSEMI_TD_487[0]), std::end(events_JOIN_LEFTSEMI_TD_487[0]));
    events_grp_JOIN_INNER_TD_3971.insert(std::end(events_grp_JOIN_INNER_TD_3971), std::begin(events_JOIN_LEFTSEMI_TD_487[1]), std::end(events_JOIN_LEFTSEMI_TD_487[1]));
    krnl_JOIN_INNER_TD_3971_part_right.run(0, &(events_grp_JOIN_INNER_TD_3971), &(events_JOIN_INNER_TD_3971[0][0]));
    for (int i(0); i < hpTimes_join; ++i) {
        krnl_JOIN_INNER_TD_3971[i].run(0, &(events_grp_JOIN_INNER_TD_3971), &(events_JOIN_INNER_TD_3971[1][i]));
    }
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_3971_build_done;
    events_grp_JOIN_INNER_TD_3971_build_done.push_back(events_JOIN_INNER_TD_3971[0]);
    krnl_JOIN_INNER_TD_3971_part_right.run(0, &(events_grp_JOIN_INNER_TD_3971), &(events_JOIN_INNER_TD_3971[0][0]));
    for (int i(0); i < hpTimes_join; ++i) {
        krnl_JOIN_INNER_TD_3971[i].run(0, &(events_grp_JOIN_INNER_TD_3971), &(events_JOIN_INNER_TD_3971[1][i]));
    }
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_3971_probe_done;
    events_grp_JOIN_INNER_TD_3971_probe_done.push_back(events_JOIN_INNER_TD_3971[1]);
    for (int i(0); i < hpTimes_join; ++i) {
        trans_JOIN_INNER_TD_3971_out.add(&(tbl_JOIN_INNER_TD_3971_output_partition_array[i]));
    }
    trans_JOIN_INNER_TD_3971_out.dev2host(0, &(events_JOIN_INNER_TD_3971[1]), &(events_d2h_rd_JOIN_INNER_TD_3971[0]));
    q_h.flush();
    q_h.finish();
    tbl_JOIN_INNER_TD_3971_output.setNumRow((cfg_JOIN_INNER_TD_3971_cmds_probe.meta_out)->getColLen());
    tbl_JOIN_INNER_TD_3971_output.colToTable();
    SW_Consolidated_JOIN_INNER_TD_3971_output(tbl_JOIN_LEFTSEMI_TD_487_consolidated_output, tbl_Filter_TD_4970_output, tbl_JOIN_INNER_TD_3971_output, tbl_JOIN_INNER_TD_3971_consolidated_output);
    gettimeofday(&tv_r_JOIN_INNER_3_196_e, 0);

    struct timeval tv_r_Filter_2_942_s, tv_r_Filter_2_942_e;
    gettimeofday(&tv_r_Filter_2_942_s, 0);
    SW_Filter_TD_2221(tbl_SerializeFromObject_TD_342_input, tbl_Filter_TD_2221_output);
    gettimeofday(&tv_r_Filter_2_942_e, 0);

    struct timeval tv_r_JOIN_LEFTSEMI_2_469_s, tv_r_JOIN_LEFTSEMI_2_469_e;
    gettimeofday(&tv_r_JOIN_LEFTSEMI_2_469_s, 0);
    prev_events_grp_JOIN_LEFTSEMI_TD_2112.push_back(events_h2d_wr_JOIN_INNER_TD_3971[0]);
    tbl_JOIN_INNER_TD_3971_consolidated_output.tableToCol();
    tbl_SerializeFromObject_TD_3404_input_stringRowIDSubstitute.tableToCol();
    krnl_JOIN_LEFTSEMI_TD_2112_build.updateMeta(tbl_JOIN_INNER_TD_3971_consolidated_output.getNumRow(), 3);
    krnl_JOIN_LEFTSEMI_TD_2112_probe.updateMeta(tbl_SerializeFromObject_TD_3404_input_stringRowIDSubstitute.getNumRow(), 3);
    trans_JOIN_LEFTSEMI_TD_2112.host2dev(0, &(prev_events_grp_JOIN_LEFTSEMI_TD_2112), &(events_h2d_wr_JOIN_LEFTSEMI_TD_2112[0]));
    events_grp_JOIN_LEFTSEMI_TD_2112.push_back(events_h2d_wr_JOIN_LEFTSEMI_TD_2112[0]);
    events_grp_JOIN_LEFTSEMI_TD_2112.insert(std::end(events_grp_JOIN_LEFTSEMI_TD_2112), std::begin(events_JOIN_INNER_TD_3971[0]), std::end(events_JOIN_INNER_TD_3971[0]));
    events_grp_JOIN_LEFTSEMI_TD_2112.insert(std::end(events_grp_JOIN_LEFTSEMI_TD_2112), std::begin(events_JOIN_INNER_TD_3971[1]), std::end(events_JOIN_INNER_TD_3971[1]));
    krnl_JOIN_LEFTSEMI_TD_2112_part_right.run(0, &(events_grp_JOIN_LEFTSEMI_TD_2112), &(events_JOIN_LEFTSEMI_TD_2112[0][0]));
    for (int i(0); i < hpTimes_join; ++i) {
        krnl_JOIN_LEFTSEMI_TD_2112[i].run(0, &(events_grp_JOIN_LEFTSEMI_TD_2112), &(events_JOIN_LEFTSEMI_TD_2112[1][i]));
    }
    std::vector<cl::Event> events_grp_JOIN_LEFTSEMI_TD_2112_build_done;
    events_grp_JOIN_LEFTSEMI_TD_2112_build_done.push_back(events_JOIN_LEFTSEMI_TD_2112[0]);
    krnl_JOIN_LEFTSEMI_TD_2112_part_right.run(0, &(events_grp_JOIN_LEFTSEMI_TD_2112), &(events_JOIN_LEFTSEMI_TD_2112[0][0]));
    for (int i(0); i < hpTimes_join; ++i) {
        krnl_JOIN_LEFTSEMI_TD_2112[i].run(0, &(events_grp_JOIN_LEFTSEMI_TD_2112), &(events_JOIN_LEFTSEMI_TD_2112[1][i]));
    }
    std::vector<cl::Event> events_grp_JOIN_LEFTSEMI_TD_2112_probe_done;
    events_grp_JOIN_LEFTSEMI_TD_2112_probe_done.push_back(events_JOIN_LEFTSEMI_TD_2112[1]);
    for (int i(0); i < hpTimes_join; ++i) {
        trans_JOIN_LEFTSEMI_TD_2112_out.add(&(tbl_JOIN_LEFTSEMI_TD_2112_output_partition_array[i]));
    }
    trans_JOIN_LEFTSEMI_TD_2112_out.dev2host(0, &(events_JOIN_LEFTSEMI_TD_2112[1]), &(events_d2h_rd_JOIN_LEFTSEMI_TD_2112[0]));
    q_h.flush();
    q_h.finish();
    tbl_JOIN_LEFTSEMI_TD_2112_output.setNumRow((cfg_JOIN_LEFTSEMI_TD_2112_cmds_probe.meta_out)->getColLen());
    tbl_JOIN_LEFTSEMI_TD_2112_output.colToTable();
    SW_Consolidated_JOIN_LEFTSEMI_TD_2112_output(tbl_JOIN_INNER_TD_3971_consolidated_output, tbl_SerializeFromObject_TD_3404_input_stringRowIDSubstitute, tbl_JOIN_LEFTSEMI_TD_2112_output, tbl_JOIN_LEFTSEMI_TD_2112_consolidated_output);
    gettimeofday(&tv_r_JOIN_LEFTSEMI_2_469_e, 0);

    struct timeval tv_r_JOIN_INNER_1_513_s, tv_r_JOIN_INNER_1_513_e;
    gettimeofday(&tv_r_JOIN_INNER_1_513_s, 0);
    prev_events_grp_JOIN_INNER_TD_1763.push_back(events_h2d_wr_JOIN_LEFTSEMI_TD_2112[0]);
    tbl_JOIN_LEFTSEMI_TD_2112_consolidated_output.tableToCol();
    tbl_Filter_TD_2221_output.tableToCol();
    krnl_JOIN_INNER_TD_1763_build.updateMeta(tbl_JOIN_LEFTSEMI_TD_2112_consolidated_output.getNumRow(), 3);
    krnl_JOIN_INNER_TD_1763_probe.updateMeta(tbl_Filter_TD_2221_output.getNumRow(), 3);
    trans_JOIN_INNER_TD_1763.add(&(tbl_Filter_TD_2221_output));
    trans_JOIN_INNER_TD_1763.host2dev(0, &(prev_events_grp_JOIN_INNER_TD_1763), &(events_h2d_wr_JOIN_INNER_TD_1763[0]));
    events_grp_JOIN_INNER_TD_1763.push_back(events_h2d_wr_JOIN_INNER_TD_1763[0]);
    events_grp_JOIN_INNER_TD_1763.insert(std::end(events_grp_JOIN_INNER_TD_1763), std::begin(events_JOIN_LEFTSEMI_TD_2112[0]), std::end(events_JOIN_LEFTSEMI_TD_2112[0]));
    events_grp_JOIN_INNER_TD_1763.insert(std::end(events_grp_JOIN_INNER_TD_1763), std::begin(events_JOIN_LEFTSEMI_TD_2112[1]), std::end(events_JOIN_LEFTSEMI_TD_2112[1]));
    krnl_JOIN_INNER_TD_1763_part_right.run(0, &(events_grp_JOIN_INNER_TD_1763), &(events_JOIN_INNER_TD_1763[0][0]));
    for (int i(0); i < hpTimes_join; ++i) {
        krnl_JOIN_INNER_TD_1763[i].run(0, &(events_grp_JOIN_INNER_TD_1763), &(events_JOIN_INNER_TD_1763[1][i]));
    }
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_1763_build_done;
    events_grp_JOIN_INNER_TD_1763_build_done.push_back(events_JOIN_INNER_TD_1763[0]);
    krnl_JOIN_INNER_TD_1763_part_right.run(0, &(events_grp_JOIN_INNER_TD_1763), &(events_JOIN_INNER_TD_1763[0][0]));
    for (int i(0); i < hpTimes_join; ++i) {
        krnl_JOIN_INNER_TD_1763[i].run(0, &(events_grp_JOIN_INNER_TD_1763), &(events_JOIN_INNER_TD_1763[1][i]));
    }
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_1763_probe_done;
    events_grp_JOIN_INNER_TD_1763_probe_done.push_back(events_JOIN_INNER_TD_1763[1]);
    for (int i(0); i < hpTimes_join; ++i) {
        trans_JOIN_INNER_TD_1763_out.add(&(tbl_JOIN_INNER_TD_1763_output_partition_array[i]));
    }
    trans_JOIN_INNER_TD_1763_out.dev2host(0, &(events_JOIN_INNER_TD_1763[1]), &(events_d2h_rd_JOIN_INNER_TD_1763[0]));
    q_h.flush();
    q_h.finish();
    tbl_JOIN_INNER_TD_1763_output.setNumRow((cfg_JOIN_INNER_TD_1763_cmds_probe.meta_out)->getColLen());
    tbl_JOIN_INNER_TD_1763_output.colToTable();
    SW_Consolidated_JOIN_INNER_TD_1763_output(tbl_JOIN_LEFTSEMI_TD_2112_consolidated_output, tbl_Filter_TD_2221_output, tbl_JOIN_INNER_TD_1763_output, tbl_JOIN_INNER_TD_1763_consolidated_output);
    gettimeofday(&tv_r_JOIN_INNER_1_513_e, 0);

    struct timeval tv_r_Sort_0_525_s, tv_r_Sort_0_525_e;
    gettimeofday(&tv_r_Sort_0_525_s, 0);
    SW_Sort_TD_0649(tbl_JOIN_INNER_TD_1763_consolidated_output_partition_array, tbl_SerializeFromObject_TD_3404_input, tbl_SerializeFromObject_TD_342_input, tbl_Sort_TD_0649_output, hpTimes_join);
    gettimeofday(&tv_r_Sort_0_525_e, 0);

std::cout << "CPU TIME: " << (tvdiff(&tv_r_Filter_7_793_s, &tv_r_Filter_7_793_e) + 
tvdiff(&tv_r_Filter_7_573_s, &tv_r_Filter_7_573_e) + 
tvdiff(&tv_r_Aggregate_5_651_s, &tv_r_Aggregate_5_651_e) + 
tvdiff(&tv_r_Filter_5_85_s, &tv_r_Filter_5_85_e) + 
tvdiff(&tv_r_Filter_4_924_s, &tv_r_Filter_4_924_e) + 
tvdiff(&tv_r_Filter_2_942_s, &tv_r_Filter_2_942_e) + 
tvdiff(&tv_r_Sort_0_525_s, &tv_r_Sort_0_525_e) + 
1) / 1000.0 << std::endl;

std::cout << "FPGA TIME: " << (tvdiff(&tv_r_JOIN_LEFTSEMI_6_226_s, &tv_r_JOIN_LEFTSEMI_6_226_e) + 
tvdiff(&tv_r_JOIN_LEFTSEMI_4_444_s, &tv_r_JOIN_LEFTSEMI_4_444_e) + 
tvdiff(&tv_r_JOIN_INNER_3_196_s, &tv_r_JOIN_INNER_3_196_e) + 
tvdiff(&tv_r_JOIN_LEFTSEMI_2_469_s, &tv_r_JOIN_LEFTSEMI_2_469_e) + 
tvdiff(&tv_r_JOIN_INNER_1_513_s, &tv_r_JOIN_INNER_1_513_e) + 
1) / 1000.0 << std::endl;

    gettimeofday(&tv_r_e, 0); 
    // **************************** Print Execution Time ************************** // 
    std::cout << std::endl; 
    std::cout << "Filter_7: " << tvdiff(&tv_r_Filter_7_793_s, &tv_r_Filter_7_793_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer((isnotnull(p_name#259) AND StartsWith(p_name#259, blush))) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_8954_input.getNumRow() << " -> tbl_SerializeFromObject_TD_8954_input" 
     << std::endl << "      #COL: 2: " << "ListBuffer(p_partkey#258, p_name#259)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_7282_output.getNumRow() << " -> tbl_Filter_TD_7282_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(p_partkey#258)" 
     << std::endl; 

    std::cout << "Filter_7: " << tvdiff(&tv_r_Filter_7_573_s, &tv_r_Filter_7_573_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(((l_shipdate#87 >= 19950101) AND (l_shipdate#87 < 19960101))) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_8847_input.getNumRow() << " -> tbl_SerializeFromObject_TD_8847_input" 
     << std::endl << "      #COL: 4: " << "ListBuffer(l_partkey#78, l_suppkey#79, l_quantity#81, l_shipdate#87)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_7493_output.getNumRow() << " -> tbl_Filter_TD_7493_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(l_partkey#78, l_suppkey#79, l_quantity#81)" 
     << std::endl; 

    std::cout << "JOIN_LEFTSEMI_6: " << tvdiff(&tv_r_JOIN_LEFTSEMI_6_226_s, &tv_r_JOIN_LEFTSEMI_6_226_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer((l_partkey#78 = p_partkey#258)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_7493_output.getNumRow() << " -> tbl_Filter_TD_7493_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(l_partkey#78, l_suppkey#79, l_quantity#81)" 
     << std::endl << "      #ROW: " << tbl_Filter_TD_7282_output.getNumRow() << " -> tbl_Filter_TD_7282_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(p_partkey#258)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_LEFTSEMI_TD_6400_consolidated_output.getNumRow() << " -> tbl_JOIN_LEFTSEMI_TD_6400_consolidated_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(l_partkey#78, l_suppkey#79, l_quantity#81)" 
     << std::endl; 

    std::cout << "Aggregate_5: " << tvdiff(&tv_r_Aggregate_5_651_s, &tv_r_Aggregate_5_651_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(l_partkey#78, l_suppkey#79, sum(l_quantity#81) AS sum(l_quantity)#1848L) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_LEFTSEMI_TD_6400_consolidated_output.getNumRow() << " -> tbl_JOIN_LEFTSEMI_TD_6400_consolidated_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(l_partkey#78, l_suppkey#79, l_quantity#81)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_5646_output.getNumRow() << " -> tbl_Aggregate_TD_5646_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(sum(l_quantity)#1848L, l_partkey#78, l_suppkey#79)" 
     << std::endl; 

    std::cout << "Filter_5: " << tvdiff(&tv_r_Filter_5_85_s, &tv_r_Filter_5_85_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer((isnotnull(p_name#259) AND StartsWith(p_name#259, blush))) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_6257_input.getNumRow() << " -> tbl_SerializeFromObject_TD_6257_input" 
     << std::endl << "      #COL: 2: " << "ListBuffer(p_partkey#258, p_name#259)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_5130_output.getNumRow() << " -> tbl_Filter_TD_5130_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(p_partkey#258)" 
     << std::endl; 

    std::cout << "Filter_4: " << tvdiff(&tv_r_Filter_4_924_s, &tv_r_Filter_4_924_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(isnotnull(sum(l_quantity)#1848L)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_5646_output.getNumRow() << " -> tbl_Aggregate_TD_5646_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(sum(l_quantity)#1848L, l_partkey#78, l_suppkey#79)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_4970_output.getNumRow() << " -> tbl_Filter_TD_4970_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(sum(l_quantity)#1848L, l_partkey#78, l_suppkey#79)" 
     << std::endl; 

    std::cout << "JOIN_LEFTSEMI_4: " << tvdiff(&tv_r_JOIN_LEFTSEMI_4_444_s, &tv_r_JOIN_LEFTSEMI_4_444_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer((ps_partkey#305 = p_partkey#258)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_5662_input.getNumRow() << " -> tbl_SerializeFromObject_TD_5662_input" 
     << std::endl << "      #COL: 3: " << "ListBuffer(ps_partkey#305, ps_suppkey#306, ps_availqty#307)" 
     << std::endl << "      #ROW: " << tbl_Filter_TD_5130_output.getNumRow() << " -> tbl_Filter_TD_5130_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(p_partkey#258)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_LEFTSEMI_TD_487_consolidated_output.getNumRow() << " -> tbl_JOIN_LEFTSEMI_TD_487_consolidated_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(ps_partkey#305, ps_suppkey#306, ps_availqty#307)" 
     << std::endl; 

    std::cout << "JOIN_INNER_3: " << tvdiff(&tv_r_JOIN_INNER_3_196_s, &tv_r_JOIN_INNER_3_196_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer((((cast(ps_availqty#307 as bigint) > sum(l_quantity)#1848L) AND (l_partkey#78 = ps_partkey#305)) AND (l_suppkey#79 = ps_suppkey#306))) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_LEFTSEMI_TD_487_consolidated_output.getNumRow() << " -> tbl_JOIN_LEFTSEMI_TD_487_consolidated_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(ps_partkey#305, ps_suppkey#306, ps_availqty#307)" 
     << std::endl << "      #ROW: " << tbl_Filter_TD_4970_output.getNumRow() << " -> tbl_Filter_TD_4970_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(sum(l_quantity)#1848L, l_partkey#78, l_suppkey#79)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_3971_consolidated_output.getNumRow() << " -> tbl_JOIN_INNER_TD_3971_consolidated_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(ps_suppkey#306)" 
     << std::endl; 

    std::cout << "Filter_2: " << tvdiff(&tv_r_Filter_2_942_s, &tv_r_Filter_2_942_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer((isnotnull(n_name#145) AND (n_name#145 = INDONESIA))) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_342_input.getNumRow() << " -> tbl_SerializeFromObject_TD_342_input" 
     << std::endl << "      #COL: 2: " << "ListBuffer(n_nationkey#144, n_name#145)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_2221_output.getNumRow() << " -> tbl_Filter_TD_2221_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(n_nationkey#144)" 
     << std::endl; 

    std::cout << "JOIN_LEFTSEMI_2: " << tvdiff(&tv_r_JOIN_LEFTSEMI_2_469_s, &tv_r_JOIN_LEFTSEMI_2_469_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer((s_suppkey#342 = ps_suppkey#306)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_3404_input_stringRowIDSubstitute.getNumRow() << " -> tbl_SerializeFromObject_TD_3404_input_stringRowIDSubstitute" 
     << std::endl << "      #COL: 4: " << "ListBuffer(s_suppkey#342, s_name#343, s_address#344, s_nationkey#345)" 
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_3971_consolidated_output.getNumRow() << " -> tbl_JOIN_INNER_TD_3971_consolidated_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(ps_suppkey#306)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_LEFTSEMI_TD_2112_consolidated_output.getNumRow() << " -> tbl_JOIN_LEFTSEMI_TD_2112_consolidated_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(s_name#343, s_address#344, s_nationkey#345)" 
     << std::endl; 

    std::cout << "JOIN_INNER_1: " << tvdiff(&tv_r_JOIN_INNER_1_513_s, &tv_r_JOIN_INNER_1_513_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer((s_nationkey#345 = n_nationkey#144)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_LEFTSEMI_TD_2112_consolidated_output.getNumRow() << " -> tbl_JOIN_LEFTSEMI_TD_2112_consolidated_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(s_name#343, s_address#344, s_nationkey#345)" 
     << std::endl << "      #ROW: " << tbl_Filter_TD_2221_output.getNumRow() << " -> tbl_Filter_TD_2221_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(n_nationkey#144)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_1763_consolidated_output.getNumRow() << " -> tbl_JOIN_INNER_TD_1763_consolidated_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(s_name#343, s_address#344)" 
     << std::endl; 

    std::cout << "Sort_0: " << tvdiff(&tv_r_Sort_0_525_s, &tv_r_Sort_0_525_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(s_name#343 ASC NULLS FIRST) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_1763_consolidated_output.getNumRow() << " -> tbl_JOIN_INNER_TD_1763_consolidated_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(s_name#343, s_address#344)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Sort_TD_0649_output.getNumRow() << " -> tbl_Sort_TD_0649_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(s_name#343, s_address#344)" 
     << std::endl; 

    std::cout << std::endl << " Total execution time: " << tvdiff(&tv_r_s, &tv_r_e) / 1000 << " ms"; 

    std::cout << std::endl << " Spark elapsed time: " << 21.221207 * 1000 << "ms" << std::endl; 
    return 0; 
}
