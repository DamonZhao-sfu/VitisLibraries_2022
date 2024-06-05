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
    Table tbl_Sort_TD_0574_output("tbl_Sort_TD_0574_output", 183000000, 8, "");
    tbl_Sort_TD_0574_output.allocateHost();
    Table tbl_JOIN_INNER_TD_1772_output("tbl_JOIN_INNER_TD_1772_output", 183000000, 8, "");
    tbl_JOIN_INNER_TD_1772_output.allocateHost();
    Table tbl_JOIN_INNER_TD_2441_output("tbl_JOIN_INNER_TD_2441_output", 183000000, 9, "");
    tbl_JOIN_INNER_TD_2441_output.allocateHost();
    Table tbl_JOIN_INNER_TD_3872_output("tbl_JOIN_INNER_TD_3872_output", -1, 8, "");
    tbl_JOIN_INNER_TD_3872_output.allocateHost(1.2, hpTimes_join);
    Table tbl_SerializeFromObject_TD_3217_input_stringRowIDSubstitute;
    tbl_SerializeFromObject_TD_3217_input_stringRowIDSubstitute = Table("nation", nation_n, 3, in_dir);
    tbl_SerializeFromObject_TD_3217_input_stringRowIDSubstitute.addCol("n_nationkey", 4);
    tbl_SerializeFromObject_TD_3217_input_stringRowIDSubstitute.addCol("n_name", 4, 1, 0);
    tbl_SerializeFromObject_TD_3217_input_stringRowIDSubstitute.addCol("n_regionkey", 4);
    tbl_SerializeFromObject_TD_3217_input_stringRowIDSubstitute.allocateHost();
    tbl_SerializeFromObject_TD_3217_input_stringRowIDSubstitute.loadHost();
    Table tbl_SerializeFromObject_TD_3217_input;
    tbl_SerializeFromObject_TD_3217_input = Table("nation", nation_n, 3, in_dir);
    tbl_SerializeFromObject_TD_3217_input.addCol("n_nationkey", 4);
    tbl_SerializeFromObject_TD_3217_input.addCol("n_name", TPCH_READ_NATION_LEN+1);
    tbl_SerializeFromObject_TD_3217_input.addCol("n_regionkey", 4);
    tbl_SerializeFromObject_TD_3217_input.allocateHost();
    tbl_SerializeFromObject_TD_3217_input.loadHost();
    Table tbl_JOIN_INNER_TD_4192_output("tbl_JOIN_INNER_TD_4192_output", -1, 3, "");
    tbl_JOIN_INNER_TD_4192_output.allocateHost(1.2, hpTimes_join);
    Table tbl_Aggregate_TD_68_output("tbl_Aggregate_TD_68_output", 183000000, 2, "");
    tbl_Aggregate_TD_68_output.selectOverlayVersion(0);
    tbl_Aggregate_TD_68_output.allocateHost();
    Table tbl_Aggregate_TD_68_output_partition("tbl_Aggregate_TD_68_output_partition", 183000000, 2, "");
    tbl_Aggregate_TD_68_output_partition.allocateHost(1.2, hpTimes_join);
    tbl_Aggregate_TD_68_output_partition.allocateDevBuffer(context_h, 32);
    Table tbl_Aggregate_TD_68_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_Aggregate_TD_68_output_partition_array[i] = tbl_Aggregate_TD_68_output_partition.createSubTable(i);
    }
    Table tbl_JOIN_INNER_TD_5695_output("tbl_JOIN_INNER_TD_5695_output", 628, 5, "");
    tbl_JOIN_INNER_TD_5695_output.allocateHost(1.2, hpTimes_join);
    Table tbl_Filter_TD_6838_output("tbl_Filter_TD_6838_output", 183000000, 2, "");
    tbl_Filter_TD_6838_output.selectOverlayVersion(0);
    tbl_Filter_TD_6838_output.allocateHost();
    Table tbl_Filter_TD_6838_output_partition("tbl_Filter_TD_6838_output_partition", 183000000, 2, "");
    tbl_Filter_TD_6838_output_partition.allocateHost(1.2, hpTimes_join);
    tbl_Filter_TD_6838_output_partition.allocateDevBuffer(context_h, 32);
    Table tbl_Filter_TD_6838_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_Filter_TD_6838_output_partition_array[i] = tbl_Filter_TD_6838_output_partition.createSubTable(i);
    }
    Table tbl_JOIN_INNER_TD_956_output("tbl_JOIN_INNER_TD_956_output", 162880, 3, "");
    tbl_JOIN_INNER_TD_956_output.allocateHost(1.2, hpTimes_join);
    Table tbl_SerializeFromObject_TD_7649_input;
    tbl_SerializeFromObject_TD_7649_input = Table("part", part_n, 4, in_dir);
    tbl_SerializeFromObject_TD_7649_input.addCol("p_partkey", 4);
    tbl_SerializeFromObject_TD_7649_input.addCol("p_mfgr", TPCH_READ_P_MFG_LEN+1);
    tbl_SerializeFromObject_TD_7649_input.addCol("p_type", TPCH_READ_P_TYPE_LEN+1);
    tbl_SerializeFromObject_TD_7649_input.addCol("p_size", 4);
    tbl_SerializeFromObject_TD_7649_input.allocateHost();
    tbl_SerializeFromObject_TD_7649_input.loadHost();
    Table tbl_JOIN_INNER_TD_8292_output("tbl_JOIN_INNER_TD_8292_output", 2036, 1, "");
    tbl_JOIN_INNER_TD_8292_output.allocateHost(1.2, hpTimes_join);
    Table tbl_SerializeFromObject_TD_10344_input;
    tbl_SerializeFromObject_TD_10344_input = Table("partsupp", partsupp_n, 3, in_dir);
    tbl_SerializeFromObject_TD_10344_input.selectOverlayVersion(0);
    tbl_SerializeFromObject_TD_10344_input.addCol("ps_partkey", 4);
    tbl_SerializeFromObject_TD_10344_input.addCol("ps_suppkey", 4);
    tbl_SerializeFromObject_TD_10344_input.addCol("ps_supplycost", 4);
    tbl_SerializeFromObject_TD_10344_input.allocateHost();
    tbl_SerializeFromObject_TD_10344_input.loadHost();
    Table tbl_SerializeFromObject_TD_10344_input_partition("tbl_SerializeFromObject_TD_10344_input_partition", partsupp_n, 3, "");
    tbl_SerializeFromObject_TD_10344_input_partition.allocateHost(1.2, hpTimes_join);
    tbl_SerializeFromObject_TD_10344_input_partition.allocateDevBuffer(context_h, 32);
    Table tbl_SerializeFromObject_TD_10344_input_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_SerializeFromObject_TD_10344_input_partition_array[i] = tbl_SerializeFromObject_TD_10344_input_partition.createSubTable(i);
    }
    Table tbl_SerializeFromObject_TD_4540_input_stringRowIDSubstitute;
    tbl_SerializeFromObject_TD_4540_input_stringRowIDSubstitute = Table("supplier", supplier_n, 7, in_dir);
    tbl_SerializeFromObject_TD_4540_input_stringRowIDSubstitute.addCol("s_suppkey", 4);
    tbl_SerializeFromObject_TD_4540_input_stringRowIDSubstitute.addCol("s_name", 4, 1, 0);
    tbl_SerializeFromObject_TD_4540_input_stringRowIDSubstitute.addCol("s_address", 4, 1, 0);
    tbl_SerializeFromObject_TD_4540_input_stringRowIDSubstitute.addCol("s_nationkey", 4);
    tbl_SerializeFromObject_TD_4540_input_stringRowIDSubstitute.addCol("s_phone", 4, 1, 0);
    tbl_SerializeFromObject_TD_4540_input_stringRowIDSubstitute.addCol("s_acctbal", 4);
    tbl_SerializeFromObject_TD_4540_input_stringRowIDSubstitute.addCol("s_comment", 4, 1, 0);
    tbl_SerializeFromObject_TD_4540_input_stringRowIDSubstitute.allocateHost();
    tbl_SerializeFromObject_TD_4540_input_stringRowIDSubstitute.loadHost();
    Table tbl_SerializeFromObject_TD_4540_input_stringRowIDSubstitute_partition("tbl_SerializeFromObject_TD_4540_input_stringRowIDSubstitute_partition", supplier_n, 7, "");
    tbl_SerializeFromObject_TD_4540_input_stringRowIDSubstitute_partition.allocateHost(1.2, hpTimes_join);
    tbl_SerializeFromObject_TD_4540_input_stringRowIDSubstitute_partition.allocateDevBuffer(context_h, 32);
    Table tbl_SerializeFromObject_TD_4540_input_stringRowIDSubstitute_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_SerializeFromObject_TD_4540_input_stringRowIDSubstitute_partition_array[i] = tbl_SerializeFromObject_TD_4540_input_stringRowIDSubstitute_partition.createSubTable(i);
    }
    Table tbl_SerializeFromObject_TD_4540_input;
    tbl_SerializeFromObject_TD_4540_input = Table("supplier", supplier_n, 7, in_dir);
    tbl_SerializeFromObject_TD_4540_input.addCol("s_suppkey", 4);
    tbl_SerializeFromObject_TD_4540_input.addCol("s_name", TPCH_READ_S_NAME_LEN+1);
    tbl_SerializeFromObject_TD_4540_input.addCol("s_address", TPCH_READ_S_ADDR_MAX+1);
    tbl_SerializeFromObject_TD_4540_input.addCol("s_nationkey", 4);
    tbl_SerializeFromObject_TD_4540_input.addCol("s_phone", TPCH_READ_PHONE_LEN+1);
    tbl_SerializeFromObject_TD_4540_input.addCol("s_acctbal", 4);
    tbl_SerializeFromObject_TD_4540_input.addCol("s_comment", TPCH_READ_S_CMNT_MAX+1);
    tbl_SerializeFromObject_TD_4540_input.allocateHost();
    tbl_SerializeFromObject_TD_4540_input.loadHost();
    Table tbl_JOIN_INNER_TD_7792_output("tbl_JOIN_INNER_TD_7792_output", 5, 1, "");
    tbl_JOIN_INNER_TD_7792_output.allocateHost(1.2, hpTimes_join);
    Table tbl_SerializeFromObject_TD_9512_input;
    tbl_SerializeFromObject_TD_9512_input = Table("nation", nation_n, 2, in_dir);
    tbl_SerializeFromObject_TD_9512_input.selectOverlayVersion(0);
    tbl_SerializeFromObject_TD_9512_input.addCol("n_nationkey", 4);
    tbl_SerializeFromObject_TD_9512_input.addCol("n_regionkey", 4);
    tbl_SerializeFromObject_TD_9512_input.allocateHost();
    tbl_SerializeFromObject_TD_9512_input.loadHost();
    Table tbl_SerializeFromObject_TD_9512_input_partition("tbl_SerializeFromObject_TD_9512_input_partition", nation_n, 2, "");
    tbl_SerializeFromObject_TD_9512_input_partition.allocateHost(1.2, hpTimes_join);
    tbl_SerializeFromObject_TD_9512_input_partition.allocateDevBuffer(context_h, 32);
    Table tbl_SerializeFromObject_TD_9512_input_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_SerializeFromObject_TD_9512_input_partition_array[i] = tbl_SerializeFromObject_TD_9512_input_partition.createSubTable(i);
    }
    Table tbl_Filter_TD_8930_output("tbl_Filter_TD_8930_output", 183000000, 1, "");
    tbl_Filter_TD_8930_output.selectOverlayVersion(0);
    tbl_Filter_TD_8930_output.allocateHost();
    Table tbl_Filter_TD_8930_output_partition("tbl_Filter_TD_8930_output_partition", 183000000, 1, "");
    tbl_Filter_TD_8930_output_partition.allocateHost(1.2, hpTimes_join);
    tbl_Filter_TD_8930_output_partition.allocateDevBuffer(context_h, 32);
    Table tbl_Filter_TD_8930_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_Filter_TD_8930_output_partition_array[i] = tbl_Filter_TD_8930_output_partition.createSubTable(i);
    }
    Table tbl_SerializeFromObject_TD_9742_input;
    tbl_SerializeFromObject_TD_9742_input = Table("region", region_n, 2, in_dir);
    tbl_SerializeFromObject_TD_9742_input.addCol("r_regionkey", 4);
    tbl_SerializeFromObject_TD_9742_input.addCol("r_name", TPCH_READ_REGION_LEN+1);
    tbl_SerializeFromObject_TD_9742_input.allocateHost();
    tbl_SerializeFromObject_TD_9742_input.loadHost();
    // ********************** Allocate Device Buffer ******************** // 
    tbl_JOIN_INNER_TD_3872_output.allocateDevBuffer(context_h, 32);
    Table tbl_JOIN_INNER_TD_3872_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_JOIN_INNER_TD_3872_output_partition_array[i] = tbl_JOIN_INNER_TD_3872_output.createSubTable(i);
    }
    tbl_JOIN_INNER_TD_4192_output.allocateDevBuffer(context_h, 32);
    Table tbl_JOIN_INNER_TD_4192_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_JOIN_INNER_TD_4192_output_partition_array[i] = tbl_JOIN_INNER_TD_4192_output.createSubTable(i);
    }
    tbl_Aggregate_TD_68_output.allocateDevBuffer(context_h, 33);
    tbl_JOIN_INNER_TD_5695_output.allocateDevBuffer(context_h, 32);
    Table tbl_JOIN_INNER_TD_5695_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_JOIN_INNER_TD_5695_output_partition_array[i] = tbl_JOIN_INNER_TD_5695_output.createSubTable(i);
    }
    tbl_Filter_TD_6838_output.allocateDevBuffer(context_h, 33);
    tbl_JOIN_INNER_TD_956_output.allocateDevBuffer(context_h, 32);
    Table tbl_JOIN_INNER_TD_956_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_JOIN_INNER_TD_956_output_partition_array[i] = tbl_JOIN_INNER_TD_956_output.createSubTable(i);
    }
    tbl_JOIN_INNER_TD_8292_output.allocateDevBuffer(context_h, 32);
    Table tbl_JOIN_INNER_TD_8292_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_JOIN_INNER_TD_8292_output_partition_array[i] = tbl_JOIN_INNER_TD_8292_output.createSubTable(i);
    }
    tbl_SerializeFromObject_TD_10344_input.allocateDevBuffer(context_h, 33);
    tbl_SerializeFromObject_TD_4540_input_stringRowIDSubstitute.allocateDevBuffer(context_h, 33);
    tbl_JOIN_INNER_TD_7792_output.allocateDevBuffer(context_h, 32);
    Table tbl_JOIN_INNER_TD_7792_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_JOIN_INNER_TD_7792_output_partition_array[i] = tbl_JOIN_INNER_TD_7792_output.createSubTable(i);
    }
    tbl_SerializeFromObject_TD_9512_input.allocateDevBuffer(context_h, 33);
    tbl_Filter_TD_8930_output.allocateDevBuffer(context_h, 33);
    // ****************************** Config **************************** // 
    cfgCmd cfg_JOIN_INNER_TD_3872_cmds;
    cfg_JOIN_INNER_TD_3872_cmds.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_3872_gqe_join (cfg_JOIN_INNER_TD_3872_cmds.cmd);
    cfg_JOIN_INNER_TD_3872_cmds.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_3872_cmds_part;
    cfg_JOIN_INNER_TD_3872_cmds_part.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_3872_gqe_join_part (cfg_JOIN_INNER_TD_3872_cmds_part.cmd);
    cfg_JOIN_INNER_TD_3872_cmds_part.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_4192_cmds;
    cfg_JOIN_INNER_TD_4192_cmds.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_4192_gqe_join (cfg_JOIN_INNER_TD_4192_cmds.cmd);
    cfg_JOIN_INNER_TD_4192_cmds.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_4192_cmds_part;
    cfg_JOIN_INNER_TD_4192_cmds_part.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_4192_gqe_join_part (cfg_JOIN_INNER_TD_4192_cmds_part.cmd);
    cfg_JOIN_INNER_TD_4192_cmds_part.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_5695_cmds;
    cfg_JOIN_INNER_TD_5695_cmds.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_5695_gqe_join (cfg_JOIN_INNER_TD_5695_cmds.cmd);
    cfg_JOIN_INNER_TD_5695_cmds.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_5695_cmds_part;
    cfg_JOIN_INNER_TD_5695_cmds_part.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_5695_gqe_join_part (cfg_JOIN_INNER_TD_5695_cmds_part.cmd);
    cfg_JOIN_INNER_TD_5695_cmds_part.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_956_cmds;
    cfg_JOIN_INNER_TD_956_cmds.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_956_gqe_join (cfg_JOIN_INNER_TD_956_cmds.cmd);
    cfg_JOIN_INNER_TD_956_cmds.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_956_cmds_part;
    cfg_JOIN_INNER_TD_956_cmds_part.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_956_gqe_join_part (cfg_JOIN_INNER_TD_956_cmds_part.cmd);
    cfg_JOIN_INNER_TD_956_cmds_part.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_8292_cmds;
    cfg_JOIN_INNER_TD_8292_cmds.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_8292_gqe_join (cfg_JOIN_INNER_TD_8292_cmds.cmd);
    cfg_JOIN_INNER_TD_8292_cmds.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_8292_cmds_part;
    cfg_JOIN_INNER_TD_8292_cmds_part.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_8292_gqe_join_part (cfg_JOIN_INNER_TD_8292_cmds_part.cmd);
    cfg_JOIN_INNER_TD_8292_cmds_part.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_7792_cmds;
    cfg_JOIN_INNER_TD_7792_cmds.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_7792_gqe_join (cfg_JOIN_INNER_TD_7792_cmds.cmd);
    cfg_JOIN_INNER_TD_7792_cmds.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_7792_cmds_part;
    cfg_JOIN_INNER_TD_7792_cmds_part.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_7792_gqe_join_part (cfg_JOIN_INNER_TD_7792_cmds_part.cmd);
    cfg_JOIN_INNER_TD_7792_cmds_part.allocateDevBuffer(context_h, 32);
    // *************************** Kernel Setup ************************* // 
    bufferTmp buftmp_h(context_h, 0); 
    buftmp_h.initBuffer(q_h); 
    std::cout << std::endl; 
    krnlEngine krnl_JOIN_INNER_TD_3872_part_right;
    krnl_JOIN_INNER_TD_3872_part_right = krnlEngine(program_h, q_h, context_h, "gqePart");
    krnl_JOIN_INNER_TD_3872_part_right.setup_hp(512, 1, power_of_hpTimes_join, tbl_SerializeFromObject_TD_4540_input_stringRowIDSubstitute, tbl_SerializeFromObject_TD_4540_input_stringRowIDSubstitute_partition, cfg_JOIN_INNER_TD_3872_cmds_part);
    krnlEngine krnl_JOIN_INNER_TD_3872[hpTimes_join];
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_INNER_TD_3872[i] = krnlEngine(program_h, q_h, "gqeJoin");
    }
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_INNER_TD_3872[i].setup(tbl_JOIN_INNER_TD_4192_output_partition_array[i], tbl_SerializeFromObject_TD_4540_input_stringRowIDSubstitute_partition_array[i], tbl_JOIN_INNER_TD_3872_output_partition_array[i], cfg_JOIN_INNER_TD_3872_cmds, buftmp_h);
    }
    krnlEngine krnl_JOIN_INNER_TD_3872_build;
    krnl_JOIN_INNER_TD_3872_build = krnlEngine(program_h, q_h, context_h, "gqeKernel", 0);
    krnl_JOIN_INNER_TD_3872_build.setup(tbl_JOIN_INNER_TD_4192_output, tbl_SerializeFromObject_TD_4540_input_stringRowIDSubstitute, tbl_JOIN_INNER_TD_3872_output, cfg_JOIN_INNER_TD_3872_cmds_build, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_3872_probe;
    krnl_JOIN_INNER_TD_3872_probe = krnlEngine(program_h, q_h, context_h, "gqeKernel", 1);
    krnl_JOIN_INNER_TD_3872_probe.setup(tbl_SerializeFromObject_TD_4540_input_stringRowIDSubstitute, tbl_JOIN_INNER_TD_4192_output, tbl_JOIN_INNER_TD_3872_output, cfg_JOIN_INNER_TD_3872_cmds_probe, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_4192_part_left;
    krnl_JOIN_INNER_TD_4192_part_left = krnlEngine(program_h, q_h, context_h, "gqePart");
    krnl_JOIN_INNER_TD_4192_part_left.setup_hp(512, 0, power_of_hpTimes_join, tbl_Aggregate_TD_68_output, tbl_Aggregate_TD_68_output_partition, cfg_JOIN_INNER_TD_4192_cmds_part);
    krnlEngine krnl_JOIN_INNER_TD_4192[hpTimes_join];
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_INNER_TD_4192[i] = krnlEngine(program_h, q_h, "gqeJoin");
    }
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_INNER_TD_4192[i].setup(tbl_Aggregate_TD_68_output_partition_array[i], tbl_JOIN_INNER_TD_5695_output_partition_array[i], tbl_JOIN_INNER_TD_4192_output_partition_array[i], cfg_JOIN_INNER_TD_4192_cmds, buftmp_h);
    }
    krnlEngine krnl_JOIN_INNER_TD_4192_build;
    krnl_JOIN_INNER_TD_4192_build = krnlEngine(program_h, q_h, context_h, "gqeKernel", 0);
    krnl_JOIN_INNER_TD_4192_build.setup(tbl_Aggregate_TD_68_output, tbl_JOIN_INNER_TD_5695_output, tbl_JOIN_INNER_TD_4192_output, cfg_JOIN_INNER_TD_4192_cmds_build, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_4192_probe;
    krnl_JOIN_INNER_TD_4192_probe = krnlEngine(program_h, q_h, context_h, "gqeKernel", 1);
    krnl_JOIN_INNER_TD_4192_probe.setup(tbl_JOIN_INNER_TD_5695_output, tbl_Aggregate_TD_68_output, tbl_JOIN_INNER_TD_4192_output, cfg_JOIN_INNER_TD_4192_cmds_probe, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_5695_part_right;
    krnl_JOIN_INNER_TD_5695_part_right = krnlEngine(program_h, q_h, context_h, "gqePart");
    krnl_JOIN_INNER_TD_5695_part_right.setup_hp(512, 1, power_of_hpTimes_join, tbl_Filter_TD_6838_output, tbl_Filter_TD_6838_output_partition, cfg_JOIN_INNER_TD_5695_cmds_part);
    krnlEngine krnl_JOIN_INNER_TD_5695[hpTimes_join];
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_INNER_TD_5695[i] = krnlEngine(program_h, q_h, "gqeJoin");
    }
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_INNER_TD_5695[i].setup(tbl_JOIN_INNER_TD_956_output_partition_array[i], tbl_Filter_TD_6838_output_partition_array[i], tbl_JOIN_INNER_TD_5695_output_partition_array[i], cfg_JOIN_INNER_TD_5695_cmds, buftmp_h);
    }
    krnlEngine krnl_JOIN_INNER_TD_5695_build;
    krnl_JOIN_INNER_TD_5695_build = krnlEngine(program_h, q_h, context_h, "gqeKernel", 0);
    krnl_JOIN_INNER_TD_5695_build.setup(tbl_JOIN_INNER_TD_956_output, tbl_Filter_TD_6838_output, tbl_JOIN_INNER_TD_5695_output, cfg_JOIN_INNER_TD_5695_cmds_build, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_5695_probe;
    krnl_JOIN_INNER_TD_5695_probe = krnlEngine(program_h, q_h, context_h, "gqeKernel", 1);
    krnl_JOIN_INNER_TD_5695_probe.setup(tbl_Filter_TD_6838_output, tbl_JOIN_INNER_TD_956_output, tbl_JOIN_INNER_TD_5695_output, cfg_JOIN_INNER_TD_5695_cmds_probe, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_956_part_right;
    krnl_JOIN_INNER_TD_956_part_right = krnlEngine(program_h, q_h, context_h, "gqePart");
    krnl_JOIN_INNER_TD_956_part_right.setup_hp(512, 1, power_of_hpTimes_join, tbl_SerializeFromObject_TD_10344_input, tbl_SerializeFromObject_TD_10344_input_partition, cfg_JOIN_INNER_TD_956_cmds_part);
    krnlEngine krnl_JOIN_INNER_TD_956[hpTimes_join];
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_INNER_TD_956[i] = krnlEngine(program_h, q_h, "gqeJoin");
    }
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_INNER_TD_956[i].setup(tbl_JOIN_INNER_TD_8292_output_partition_array[i], tbl_SerializeFromObject_TD_10344_input_partition_array[i], tbl_JOIN_INNER_TD_956_output_partition_array[i], cfg_JOIN_INNER_TD_956_cmds, buftmp_h);
    }
    krnlEngine krnl_JOIN_INNER_TD_956_build;
    krnl_JOIN_INNER_TD_956_build = krnlEngine(program_h, q_h, context_h, "gqeKernel", 0);
    krnl_JOIN_INNER_TD_956_build.setup(tbl_JOIN_INNER_TD_8292_output, tbl_SerializeFromObject_TD_10344_input, tbl_JOIN_INNER_TD_956_output, cfg_JOIN_INNER_TD_956_cmds_build, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_956_probe;
    krnl_JOIN_INNER_TD_956_probe = krnlEngine(program_h, q_h, context_h, "gqeKernel", 1);
    krnl_JOIN_INNER_TD_956_probe.setup(tbl_SerializeFromObject_TD_10344_input, tbl_JOIN_INNER_TD_8292_output, tbl_JOIN_INNER_TD_956_output, cfg_JOIN_INNER_TD_956_cmds_probe, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_8292_part_left;
    krnl_JOIN_INNER_TD_8292_part_left = krnlEngine(program_h, q_h, context_h, "gqePart");
    krnl_JOIN_INNER_TD_8292_part_left.setup_hp(512, 0, power_of_hpTimes_join, tbl_SerializeFromObject_TD_4540_input_stringRowIDSubstitute, tbl_SerializeFromObject_TD_4540_input_stringRowIDSubstitute_partition, cfg_JOIN_INNER_TD_8292_cmds_part);
    krnlEngine krnl_JOIN_INNER_TD_8292[hpTimes_join];
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_INNER_TD_8292[i] = krnlEngine(program_h, q_h, "gqeJoin");
    }
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_INNER_TD_8292[i].setup(tbl_SerializeFromObject_TD_4540_input_stringRowIDSubstitute_partition_array[i], tbl_JOIN_INNER_TD_7792_output_partition_array[i], tbl_JOIN_INNER_TD_8292_output_partition_array[i], cfg_JOIN_INNER_TD_8292_cmds, buftmp_h);
    }
    krnlEngine krnl_JOIN_INNER_TD_8292_build;
    krnl_JOIN_INNER_TD_8292_build = krnlEngine(program_h, q_h, context_h, "gqeKernel", 0);
    krnl_JOIN_INNER_TD_8292_build.setup(tbl_SerializeFromObject_TD_4540_input_stringRowIDSubstitute, tbl_JOIN_INNER_TD_7792_output, tbl_JOIN_INNER_TD_8292_output, cfg_JOIN_INNER_TD_8292_cmds_build, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_8292_probe;
    krnl_JOIN_INNER_TD_8292_probe = krnlEngine(program_h, q_h, context_h, "gqeKernel", 1);
    krnl_JOIN_INNER_TD_8292_probe.setup(tbl_JOIN_INNER_TD_7792_output, tbl_SerializeFromObject_TD_4540_input_stringRowIDSubstitute, tbl_JOIN_INNER_TD_8292_output, cfg_JOIN_INNER_TD_8292_cmds_probe, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_7792_part_left;
    krnl_JOIN_INNER_TD_7792_part_left = krnlEngine(program_h, q_h, context_h, "gqePart");
    krnl_JOIN_INNER_TD_7792_part_left.setup_hp(512, 0, power_of_hpTimes_join, tbl_SerializeFromObject_TD_9512_input, tbl_SerializeFromObject_TD_9512_input_partition, cfg_JOIN_INNER_TD_7792_cmds_part);
    krnlEngine krnl_JOIN_INNER_TD_7792_part_right;
    krnl_JOIN_INNER_TD_7792_part_right = krnlEngine(program_h, q_h, context_h, "gqePart");
    krnl_JOIN_INNER_TD_7792_part_right.setup_hp(512, 1, power_of_hpTimes_join, tbl_Filter_TD_8930_output, tbl_Filter_TD_8930_output_partition, cfg_JOIN_INNER_TD_7792_cmds_part);
    krnlEngine krnl_JOIN_INNER_TD_7792[hpTimes_join];
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_INNER_TD_7792[i] = krnlEngine(program_h, q_h, "gqeJoin");
    }
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_INNER_TD_7792[i].setup(tbl_SerializeFromObject_TD_9512_input_partition_array[i], tbl_Filter_TD_8930_output_partition_array[i], tbl_JOIN_INNER_TD_7792_output_partition_array[i], cfg_JOIN_INNER_TD_7792_cmds, buftmp_h);
    }
    krnlEngine krnl_JOIN_INNER_TD_7792_build;
    krnl_JOIN_INNER_TD_7792_build = krnlEngine(program_h, q_h, context_h, "gqeKernel", 0);
    krnl_JOIN_INNER_TD_7792_build.setup(tbl_SerializeFromObject_TD_9512_input, tbl_Filter_TD_8930_output, tbl_JOIN_INNER_TD_7792_output, cfg_JOIN_INNER_TD_7792_cmds_build, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_7792_probe;
    krnl_JOIN_INNER_TD_7792_probe = krnlEngine(program_h, q_h, context_h, "gqeKernel", 1);
    krnl_JOIN_INNER_TD_7792_probe.setup(tbl_Filter_TD_8930_output, tbl_SerializeFromObject_TD_9512_input, tbl_JOIN_INNER_TD_7792_output, cfg_JOIN_INNER_TD_7792_cmds_probe, buftmp_h);
    // ************************** Transfer Engine *********************** // 
    transEngine trans_JOIN_INNER_TD_3872;
    trans_JOIN_INNER_TD_3872.setq(q_h,0);
    trans_JOIN_INNER_TD_3872.add(&(cfg_JOIN_INNER_TD_3872_cmds_part));
    trans_JOIN_INNER_TD_3872.add(&(cfg_JOIN_INNER_TD_3872_cmds));
    trans_JOIN_INNER_TD_3872.add(&(tbl_SerializeFromObject_TD_4540_input_stringRowIDSubstitute));
    transEngine trans_JOIN_INNER_TD_3872_out;
    trans_JOIN_INNER_TD_3872_out.setq(q_h,0);
    q_h.finish();
    transEngine trans_JOIN_INNER_TD_4192;
    trans_JOIN_INNER_TD_4192.setq(q_h,0);
    trans_JOIN_INNER_TD_4192.add(&(cfg_JOIN_INNER_TD_4192_cmds_part));
    trans_JOIN_INNER_TD_4192.add(&(cfg_JOIN_INNER_TD_4192_cmds));
    q_h.finish();
    transEngine trans_JOIN_INNER_TD_5695;
    trans_JOIN_INNER_TD_5695.setq(q_h,0);
    trans_JOIN_INNER_TD_5695.add(&(cfg_JOIN_INNER_TD_5695_cmds_part));
    trans_JOIN_INNER_TD_5695.add(&(cfg_JOIN_INNER_TD_5695_cmds));
    q_h.finish();
    transEngine trans_JOIN_INNER_TD_956;
    trans_JOIN_INNER_TD_956.setq(q_h,0);
    trans_JOIN_INNER_TD_956.add(&(cfg_JOIN_INNER_TD_956_cmds_part));
    trans_JOIN_INNER_TD_956.add(&(cfg_JOIN_INNER_TD_956_cmds));
    trans_JOIN_INNER_TD_956.add(&(tbl_SerializeFromObject_TD_10344_input));
    q_h.finish();
    transEngine trans_JOIN_INNER_TD_8292;
    trans_JOIN_INNER_TD_8292.setq(q_h,0);
    trans_JOIN_INNER_TD_8292.add(&(cfg_JOIN_INNER_TD_8292_cmds_part));
    trans_JOIN_INNER_TD_8292.add(&(cfg_JOIN_INNER_TD_8292_cmds));
    trans_JOIN_INNER_TD_8292.add(&(tbl_SerializeFromObject_TD_4540_input_stringRowIDSubstitute));
    q_h.finish();
    transEngine trans_JOIN_INNER_TD_7792;
    trans_JOIN_INNER_TD_7792.setq(q_h,0);
    trans_JOIN_INNER_TD_7792.add(&(cfg_JOIN_INNER_TD_7792_cmds_part));
    trans_JOIN_INNER_TD_7792.add(&(cfg_JOIN_INNER_TD_7792_cmds));
    trans_JOIN_INNER_TD_7792.add(&(tbl_SerializeFromObject_TD_9512_input));
    q_h.finish();
    // ****************************** Events **************************** // 
    std::vector<cl::Event> events_h2d_wr_JOIN_INNER_TD_3872;
    std::vector<cl::Event> events_d2h_rd_JOIN_INNER_TD_3872;
    std::vector<cl::Event> events_JOIN_INNER_TD_3872[2];
    events_h2d_wr_JOIN_INNER_TD_3872.resize(1);
    events_d2h_rd_JOIN_INNER_TD_3872.resize(1);
    events_JOIN_INNER_TD_3872[0].resize(1);
    events_JOIN_INNER_TD_3872[1].resize(hpTimes_join);
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_3872;
    std::vector<cl::Event> prev_events_grp_JOIN_INNER_TD_3872;
    std::vector<cl::Event> events_h2d_wr_JOIN_INNER_TD_4192;
    std::vector<cl::Event> events_d2h_rd_JOIN_INNER_TD_4192;
    std::vector<cl::Event> events_JOIN_INNER_TD_4192[2];
    events_h2d_wr_JOIN_INNER_TD_4192.resize(1);
    events_d2h_rd_JOIN_INNER_TD_4192.resize(1);
    events_JOIN_INNER_TD_4192[0].resize(1);
    events_JOIN_INNER_TD_4192[1].resize(hpTimes_join);
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_4192;
    std::vector<cl::Event> prev_events_grp_JOIN_INNER_TD_4192;
    std::vector<cl::Event> events_h2d_wr_JOIN_INNER_TD_5695;
    std::vector<cl::Event> events_d2h_rd_JOIN_INNER_TD_5695;
    std::vector<cl::Event> events_JOIN_INNER_TD_5695[2];
    events_h2d_wr_JOIN_INNER_TD_5695.resize(1);
    events_d2h_rd_JOIN_INNER_TD_5695.resize(1);
    events_JOIN_INNER_TD_5695[0].resize(1);
    events_JOIN_INNER_TD_5695[1].resize(hpTimes_join);
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_5695;
    std::vector<cl::Event> prev_events_grp_JOIN_INNER_TD_5695;
    std::vector<cl::Event> events_h2d_wr_JOIN_INNER_TD_956;
    std::vector<cl::Event> events_d2h_rd_JOIN_INNER_TD_956;
    std::vector<cl::Event> events_JOIN_INNER_TD_956[2];
    events_h2d_wr_JOIN_INNER_TD_956.resize(1);
    events_d2h_rd_JOIN_INNER_TD_956.resize(1);
    events_JOIN_INNER_TD_956[0].resize(1);
    events_JOIN_INNER_TD_956[1].resize(hpTimes_join);
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_956;
    std::vector<cl::Event> prev_events_grp_JOIN_INNER_TD_956;
    std::vector<cl::Event> events_h2d_wr_JOIN_INNER_TD_8292;
    std::vector<cl::Event> events_d2h_rd_JOIN_INNER_TD_8292;
    std::vector<cl::Event> events_JOIN_INNER_TD_8292[2];
    events_h2d_wr_JOIN_INNER_TD_8292.resize(1);
    events_d2h_rd_JOIN_INNER_TD_8292.resize(1);
    events_JOIN_INNER_TD_8292[0].resize(1);
    events_JOIN_INNER_TD_8292[1].resize(hpTimes_join);
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_8292;
    std::vector<cl::Event> prev_events_grp_JOIN_INNER_TD_8292;
    std::vector<cl::Event> events_h2d_wr_JOIN_INNER_TD_7792;
    std::vector<cl::Event> events_d2h_rd_JOIN_INNER_TD_7792;
    std::vector<cl::Event> events_JOIN_INNER_TD_7792[2];
    events_h2d_wr_JOIN_INNER_TD_7792.resize(1);
    events_d2h_rd_JOIN_INNER_TD_7792.resize(1);
    events_JOIN_INNER_TD_7792[0].resize(2);
    events_JOIN_INNER_TD_7792[1].resize(hpTimes_join);
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_7792;
    std::vector<cl::Event> prev_events_grp_JOIN_INNER_TD_7792;
    // **************************** Operations ************************** // 
    struct timeval tv_r_s, tv_r_e; 
    gettimeofday(&tv_r_s, 0); 

    struct timeval tv_r_Filter_8_710_s, tv_r_Filter_8_710_e;
    gettimeofday(&tv_r_Filter_8_710_s, 0);
    SW_Filter_TD_8930(tbl_SerializeFromObject_TD_9742_input, tbl_Filter_TD_8930_output);
    gettimeofday(&tv_r_Filter_8_710_e, 0);

    struct timeval tv_r_JOIN_INNER_7_246_s, tv_r_JOIN_INNER_7_246_e;
    gettimeofday(&tv_r_JOIN_INNER_7_246_s, 0);
    trans_JOIN_INNER_TD_7792.add(&(tbl_Filter_TD_8930_output));
    trans_JOIN_INNER_TD_7792.host2dev(0, &(prev_events_grp_JOIN_INNER_TD_7792), &(events_h2d_wr_JOIN_INNER_TD_7792[0]));
    events_grp_JOIN_INNER_TD_7792.push_back(events_h2d_wr_JOIN_INNER_TD_7792[0]);
    krnl_JOIN_INNER_TD_7792_part_left.run(0, &(events_grp_JOIN_INNER_TD_7792), &(events_JOIN_INNER_TD_7792[0][0]));
    krnl_JOIN_INNER_TD_7792_part_right.run(0, &(events_grp_JOIN_INNER_TD_7792), &(events_JOIN_INNER_TD_7792[0][1]));
    for (int i(0); i < hpTimes_join; ++i) {
        krnl_JOIN_INNER_TD_7792[i].run(0, &(events_JOIN_INNER_TD_7792[0]), &(events_JOIN_INNER_TD_7792[1][i]));
    }
    gettimeofday(&tv_r_JOIN_INNER_7_246_e, 0);

    struct timeval tv_r_JOIN_INNER_8_564_s, tv_r_JOIN_INNER_8_564_e;
    gettimeofday(&tv_r_JOIN_INNER_8_564_s, 0);
    prev_events_grp_JOIN_INNER_TD_8292.push_back(events_h2d_wr_JOIN_INNER_TD_7792[0]);
    trans_JOIN_INNER_TD_8292.host2dev(0, &(prev_events_grp_JOIN_INNER_TD_8292), &(events_h2d_wr_JOIN_INNER_TD_8292[0]));
    events_grp_JOIN_INNER_TD_8292.push_back(events_h2d_wr_JOIN_INNER_TD_8292[0]);
    events_grp_JOIN_INNER_TD_8292.insert(std::end(events_grp_JOIN_INNER_TD_8292), std::begin(events_JOIN_INNER_TD_7792[0]), std::end(events_JOIN_INNER_TD_7792[0]));
    events_grp_JOIN_INNER_TD_8292.insert(std::end(events_grp_JOIN_INNER_TD_8292), std::begin(events_JOIN_INNER_TD_7792[1]), std::end(events_JOIN_INNER_TD_7792[1]));
    krnl_JOIN_INNER_TD_8292_part_left.run(0, &(events_grp_JOIN_INNER_TD_8292), &(events_JOIN_INNER_TD_8292[0][0]));
    for (int i(0); i < hpTimes_join; ++i) {
        krnl_JOIN_INNER_TD_8292[i].run(0, &(events_JOIN_INNER_TD_8292[0]), &(events_JOIN_INNER_TD_8292[1][i]));
    }
    gettimeofday(&tv_r_JOIN_INNER_8_564_e, 0);

    struct timeval tv_r_JOIN_INNER_9_462_s, tv_r_JOIN_INNER_9_462_e;
    gettimeofday(&tv_r_JOIN_INNER_9_462_s, 0);
    prev_events_grp_JOIN_INNER_TD_956.push_back(events_h2d_wr_JOIN_INNER_TD_8292[0]);
    trans_JOIN_INNER_TD_956.host2dev(0, &(prev_events_grp_JOIN_INNER_TD_956), &(events_h2d_wr_JOIN_INNER_TD_956[0]));
    events_grp_JOIN_INNER_TD_956.push_back(events_h2d_wr_JOIN_INNER_TD_956[0]);
    events_grp_JOIN_INNER_TD_956.insert(std::end(events_grp_JOIN_INNER_TD_956), std::begin(events_JOIN_INNER_TD_8292[0]), std::end(events_JOIN_INNER_TD_8292[0]));
    events_grp_JOIN_INNER_TD_956.insert(std::end(events_grp_JOIN_INNER_TD_956), std::begin(events_JOIN_INNER_TD_8292[1]), std::end(events_JOIN_INNER_TD_8292[1]));
    krnl_JOIN_INNER_TD_956_part_right.run(0, &(events_grp_JOIN_INNER_TD_956), &(events_JOIN_INNER_TD_956[0][0]));
    for (int i(0); i < hpTimes_join; ++i) {
        krnl_JOIN_INNER_TD_956[i].run(0, &(events_grp_JOIN_INNER_TD_956), &(events_JOIN_INNER_TD_956[1][i]));
    }
    gettimeofday(&tv_r_JOIN_INNER_9_462_e, 0);

    struct timeval tv_r_Filter_6_911_s, tv_r_Filter_6_911_e;
    gettimeofday(&tv_r_Filter_6_911_s, 0);
    SW_Filter_TD_6838(tbl_SerializeFromObject_TD_7649_input, tbl_Filter_TD_6838_output);
    gettimeofday(&tv_r_Filter_6_911_e, 0);

    struct timeval tv_r_JOIN_INNER_5_559_s, tv_r_JOIN_INNER_5_559_e;
    gettimeofday(&tv_r_JOIN_INNER_5_559_s, 0);
    prev_events_grp_JOIN_INNER_TD_5695.push_back(events_h2d_wr_JOIN_INNER_TD_956[0]);
    trans_JOIN_INNER_TD_5695.add(&(tbl_Filter_TD_6838_output));
    trans_JOIN_INNER_TD_5695.host2dev(0, &(prev_events_grp_JOIN_INNER_TD_5695), &(events_h2d_wr_JOIN_INNER_TD_5695[0]));
    events_grp_JOIN_INNER_TD_5695.push_back(events_h2d_wr_JOIN_INNER_TD_5695[0]);
    events_grp_JOIN_INNER_TD_5695.insert(std::end(events_grp_JOIN_INNER_TD_5695), std::begin(events_JOIN_INNER_TD_956[0]), std::end(events_JOIN_INNER_TD_956[0]));
    events_grp_JOIN_INNER_TD_5695.insert(std::end(events_grp_JOIN_INNER_TD_5695), std::begin(events_JOIN_INNER_TD_956[1]), std::end(events_JOIN_INNER_TD_956[1]));
    krnl_JOIN_INNER_TD_5695_part_right.run(0, &(events_grp_JOIN_INNER_TD_5695), &(events_JOIN_INNER_TD_5695[0][0]));
    for (int i(0); i < hpTimes_join; ++i) {
        krnl_JOIN_INNER_TD_5695[i].run(0, &(events_grp_JOIN_INNER_TD_5695), &(events_JOIN_INNER_TD_5695[1][i]));
    }
    gettimeofday(&tv_r_JOIN_INNER_5_559_e, 0);

    struct timeval tv_r_Aggregate_6_168_s, tv_r_Aggregate_6_168_e;
    gettimeofday(&tv_r_Aggregate_6_168_s, 0);
    SW_Aggregate_TD_68(tbl_JOIN_INNER_TD_5695_output_partition_array, tbl_Aggregate_TD_68_output, hpTimes_join);
    gettimeofday(&tv_r_Aggregate_6_168_e, 0);

    struct timeval tv_r_JOIN_INNER_4_215_s, tv_r_JOIN_INNER_4_215_e;
    gettimeofday(&tv_r_JOIN_INNER_4_215_s, 0);
    prev_events_grp_JOIN_INNER_TD_4192.push_back(events_h2d_wr_JOIN_INNER_TD_5695[0]);
    trans_JOIN_INNER_TD_4192.add(&(tbl_Aggregate_TD_68_output));
    trans_JOIN_INNER_TD_4192.host2dev(0, &(prev_events_grp_JOIN_INNER_TD_4192), &(events_h2d_wr_JOIN_INNER_TD_4192[0]));
    events_grp_JOIN_INNER_TD_4192.push_back(events_h2d_wr_JOIN_INNER_TD_4192[0]);
    events_grp_JOIN_INNER_TD_4192.insert(std::end(events_grp_JOIN_INNER_TD_4192), std::begin(events_JOIN_INNER_TD_5695[0]), std::end(events_JOIN_INNER_TD_5695[0]));
    events_grp_JOIN_INNER_TD_4192.insert(std::end(events_grp_JOIN_INNER_TD_4192), std::begin(events_JOIN_INNER_TD_5695[1]), std::end(events_JOIN_INNER_TD_5695[1]));
    krnl_JOIN_INNER_TD_4192_part_left.run(0, &(events_grp_JOIN_INNER_TD_4192), &(events_JOIN_INNER_TD_4192[0][0]));
    for (int i(0); i < hpTimes_join; ++i) {
        krnl_JOIN_INNER_TD_4192[i].run(0, &(events_JOIN_INNER_TD_4192[0]), &(events_JOIN_INNER_TD_4192[1][i]));
    }
    gettimeofday(&tv_r_JOIN_INNER_4_215_e, 0);

    struct timeval tv_r_JOIN_INNER_3_178_s, tv_r_JOIN_INNER_3_178_e;
    gettimeofday(&tv_r_JOIN_INNER_3_178_s, 0);
    prev_events_grp_JOIN_INNER_TD_3872.push_back(events_h2d_wr_JOIN_INNER_TD_4192[0]);
    trans_JOIN_INNER_TD_3872.host2dev(0, &(prev_events_grp_JOIN_INNER_TD_3872), &(events_h2d_wr_JOIN_INNER_TD_3872[0]));
    events_grp_JOIN_INNER_TD_3872.push_back(events_h2d_wr_JOIN_INNER_TD_3872[0]);
    events_grp_JOIN_INNER_TD_3872.insert(std::end(events_grp_JOIN_INNER_TD_3872), std::begin(events_JOIN_INNER_TD_4192[0]), std::end(events_JOIN_INNER_TD_4192[0]));
    events_grp_JOIN_INNER_TD_3872.insert(std::end(events_grp_JOIN_INNER_TD_3872), std::begin(events_JOIN_INNER_TD_4192[1]), std::end(events_JOIN_INNER_TD_4192[1]));
    krnl_JOIN_INNER_TD_3872_part_right.run(0, &(events_grp_JOIN_INNER_TD_3872), &(events_JOIN_INNER_TD_3872[0][0]));
    for (int i(0); i < hpTimes_join; ++i) {
        krnl_JOIN_INNER_TD_3872[i].run(0, &(events_grp_JOIN_INNER_TD_3872), &(events_JOIN_INNER_TD_3872[1][i]));
    }
    for (int i(0); i < hpTimes_join; ++i) {
        trans_JOIN_INNER_TD_3872_out.add(&(tbl_JOIN_INNER_TD_3872_output_partition_array[i]));
    }
    trans_JOIN_INNER_TD_3872_out.dev2host(0, &(events_JOIN_INNER_TD_3872[1]), &(events_d2h_rd_JOIN_INNER_TD_3872[0]));
    q_h.flush();
    q_h.finish();
    gettimeofday(&tv_r_JOIN_INNER_3_178_e, 0);

    struct timeval tv_r_JOIN_INNER_2_966_s, tv_r_JOIN_INNER_2_966_e;
    gettimeofday(&tv_r_JOIN_INNER_2_966_s, 0);
    SW_JOIN_INNER_TD_2441(tbl_JOIN_INNER_TD_3872_output_partition_array, tbl_SerializeFromObject_TD_3217_input_stringRowIDSubstitute, tbl_JOIN_INNER_TD_2441_output, hpTimes_join);
    gettimeofday(&tv_r_JOIN_INNER_2_966_e, 0);

    struct timeval tv_r_JOIN_INNER_1_449_s, tv_r_JOIN_INNER_1_449_e;
    gettimeofday(&tv_r_JOIN_INNER_1_449_s, 0);
    SW_JOIN_INNER_TD_1772(tbl_JOIN_INNER_TD_2441_output, tbl_Filter_TD_8930_output, tbl_JOIN_INNER_TD_1772_output);
    gettimeofday(&tv_r_JOIN_INNER_1_449_e, 0);

    struct timeval tv_r_Sort_0_388_s, tv_r_Sort_0_388_e;
    gettimeofday(&tv_r_Sort_0_388_s, 0);
    SW_Sort_TD_0574(tbl_JOIN_INNER_TD_1772_output, tbl_SerializeFromObject_TD_7649_input, tbl_SerializeFromObject_TD_4540_input, tbl_SerializeFromObject_TD_3217_input, tbl_Sort_TD_0574_output);
    gettimeofday(&tv_r_Sort_0_388_e, 0);

std::cout << "CPU TIME: " << (tvdiff(&tv_r_Filter_8_710_s, &tv_r_Filter_8_710_e) + 
tvdiff(&tv_r_Filter_6_911_s, &tv_r_Filter_6_911_e) + 
tvdiff(&tv_r_Aggregate_6_168_s, &tv_r_Aggregate_6_168_e) + 
tvdiff(&tv_r_JOIN_INNER_2_966_s, &tv_r_JOIN_INNER_2_966_e) + 
tvdiff(&tv_r_JOIN_INNER_1_449_s, &tv_r_JOIN_INNER_1_449_e) + 
tvdiff(&tv_r_Sort_0_388_s, &tv_r_Sort_0_388_e) + 
1) / 1000.0 << std::endl;

std::cout << "FPGA TIME: " << (tvdiff(&tv_r_JOIN_INNER_7_246_s, &tv_r_JOIN_INNER_7_246_e) + 
tvdiff(&tv_r_JOIN_INNER_8_564_s, &tv_r_JOIN_INNER_8_564_e) + 
tvdiff(&tv_r_JOIN_INNER_9_462_s, &tv_r_JOIN_INNER_9_462_e) + 
tvdiff(&tv_r_JOIN_INNER_5_559_s, &tv_r_JOIN_INNER_5_559_e) + 
tvdiff(&tv_r_JOIN_INNER_4_215_s, &tv_r_JOIN_INNER_4_215_e) + 
tvdiff(&tv_r_JOIN_INNER_3_178_s, &tv_r_JOIN_INNER_3_178_e) + 
1) / 1000.0 << std::endl;

    gettimeofday(&tv_r_e, 0); 
    // **************************** Print Execution Time ************************** // 
    std::cout << std::endl; 
    std::cout << "Filter_8: " << tvdiff(&tv_r_Filter_8_710_s, &tv_r_Filter_8_710_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer((isnotnull(r_name#175) AND (r_name#175 = AMERICA))) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_9742_input.getNumRow() << " -> tbl_SerializeFromObject_TD_9742_input" 
     << std::endl << "      #COL: 2: " << "ListBuffer(r_regionkey#174, r_name#175)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_8930_output.getNumRow() << " -> tbl_Filter_TD_8930_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(r_regionkey#174)" 
     << std::endl; 

    std::cout << "JOIN_INNER_7: " << tvdiff(&tv_r_JOIN_INNER_7_246_s, &tv_r_JOIN_INNER_7_246_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer((n_regionkey#146 = r_regionkey#174)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_9512_input.getNumRow() << " -> tbl_SerializeFromObject_TD_9512_input" 
     << std::endl << "      #COL: 2: " << "ListBuffer(n_nationkey#144, n_regionkey#146)" 
     << std::endl << "      #ROW: " << tbl_Filter_TD_8930_output.getNumRow() << " -> tbl_Filter_TD_8930_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(r_regionkey#174)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_7792_output.getNumRow() << " -> tbl_JOIN_INNER_TD_7792_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(n_nationkey#144)" 
     << std::endl; 

    std::cout << "JOIN_INNER_8: " << tvdiff(&tv_r_JOIN_INNER_8_564_s, &tv_r_JOIN_INNER_8_564_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer((s_nationkey#345 = n_nationkey#144)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_4540_input_stringRowIDSubstitute.getNumRow() << " -> tbl_SerializeFromObject_TD_4540_input_stringRowIDSubstitute" 
     << std::endl << "      #COL: 7: " << "ListBuffer(s_suppkey#342, s_name#343, s_address#344, s_nationkey#345, s_phone#346, s_acctbal#347, s_comment#348)" 
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_7792_output.getNumRow() << " -> tbl_JOIN_INNER_TD_7792_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(n_nationkey#144)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_8292_output.getNumRow() << " -> tbl_JOIN_INNER_TD_8292_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(s_suppkey#342)" 
     << std::endl; 

    std::cout << "JOIN_INNER_9: " << tvdiff(&tv_r_JOIN_INNER_9_462_s, &tv_r_JOIN_INNER_9_462_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer((s_suppkey#342 = ps_suppkey#306)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_8292_output.getNumRow() << " -> tbl_JOIN_INNER_TD_8292_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(s_suppkey#342)" 
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_10344_input.getNumRow() << " -> tbl_SerializeFromObject_TD_10344_input" 
     << std::endl << "      #COL: 3: " << "ListBuffer(ps_partkey#305, ps_suppkey#306, ps_supplycost#308)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_956_output.getNumRow() << " -> tbl_JOIN_INNER_TD_956_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(ps_partkey#305, ps_supplycost#308, ps_suppkey#306)" 
     << std::endl; 

    std::cout << "Filter_6: " << tvdiff(&tv_r_Filter_6_911_s, &tv_r_Filter_6_911_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer((isnotnull(p_type#262) AND ((p_size#263 = 19) AND EndsWith(p_type#262, NICKEL)))) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_7649_input.getNumRow() << " -> tbl_SerializeFromObject_TD_7649_input" 
     << std::endl << "      #COL: 4: " << "ListBuffer(p_partkey#258, p_mfgr#260, p_type#262, p_size#263)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_6838_output.getNumRow() << " -> tbl_Filter_TD_6838_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(p_partkey#258, p_mfgr#260)" 
     << std::endl; 

    std::cout << "JOIN_INNER_5: " << tvdiff(&tv_r_JOIN_INNER_5_559_s, &tv_r_JOIN_INNER_5_559_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer((p_partkey#258 = ps_partkey#305)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_6838_output.getNumRow() << " -> tbl_Filter_TD_6838_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(p_partkey#258, p_mfgr#260)" 
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_956_output.getNumRow() << " -> tbl_JOIN_INNER_TD_956_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(ps_partkey#305, ps_supplycost#308, ps_suppkey#306)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_5695_output.getNumRow() << " -> tbl_JOIN_INNER_TD_5695_output" 
     << std::endl << "      #COL: 5: " << "ListBuffer(p_partkey#258, p_mfgr#260, ps_suppkey#306, ps_supplycost#308, ps_partkey#305)" 
     << std::endl; 

    std::cout << "Aggregate_6: " << tvdiff(&tv_r_Aggregate_6_168_s, &tv_r_Aggregate_6_168_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(ps_partkey#305, min(ps_supplycost#308) AS min(ps_supplycost)#579, ps_partkey#305 AS ps_partkey#305#663) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_5695_output.getNumRow() << " -> tbl_JOIN_INNER_TD_5695_output" 
     << std::endl << "      #COL: 5: " << "ListBuffer(p_partkey#258, p_mfgr#260, ps_suppkey#306, ps_supplycost#308, ps_partkey#305)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_68_output.getNumRow() << " -> tbl_Aggregate_TD_68_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(min(ps_supplycost)#579, ps_partkey#305#663)" 
     << std::endl; 

    std::cout << "JOIN_INNER_4: " << tvdiff(&tv_r_JOIN_INNER_4_215_s, &tv_r_JOIN_INNER_4_215_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer(((ps_supplycost#308 = min(ps_supplycost)#579) AND (p_partkey#258 = ps_partkey#305#663))) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_5695_output.getNumRow() << " -> tbl_JOIN_INNER_TD_5695_output" 
     << std::endl << "      #COL: 5: " << "ListBuffer(p_partkey#258, p_mfgr#260, ps_suppkey#306, ps_supplycost#308, ps_partkey#305)" 
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_68_output.getNumRow() << " -> tbl_Aggregate_TD_68_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(min(ps_supplycost)#579, ps_partkey#305#663)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_4192_output.getNumRow() << " -> tbl_JOIN_INNER_TD_4192_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(p_partkey#258, p_mfgr#260, ps_suppkey#306)" 
     << std::endl; 

    std::cout << "JOIN_INNER_3: " << tvdiff(&tv_r_JOIN_INNER_3_178_s, &tv_r_JOIN_INNER_3_178_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer((s_suppkey#342 = ps_suppkey#306)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_4192_output.getNumRow() << " -> tbl_JOIN_INNER_TD_4192_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(p_partkey#258, p_mfgr#260, ps_suppkey#306)" 
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_4540_input_stringRowIDSubstitute.getNumRow() << " -> tbl_SerializeFromObject_TD_4540_input_stringRowIDSubstitute" 
     << std::endl << "      #COL: 7: " << "ListBuffer(s_suppkey#342, s_name#343, s_address#344, s_nationkey#345, s_phone#346, s_acctbal#347, s_comment#348)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_3872_output.getNumRow() << " -> tbl_JOIN_INNER_TD_3872_output" 
     << std::endl << "      #COL: 8: " << "ListBuffer(p_partkey#258, p_mfgr#260, s_name#343, s_address#344, s_nationkey#345, s_phone#346, s_acctbal#347, s_comment#348)" 
     << std::endl; 

    std::cout << "JOIN_INNER_2: " << tvdiff(&tv_r_JOIN_INNER_2_966_s, &tv_r_JOIN_INNER_2_966_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer((s_nationkey#345 = n_nationkey#144)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_3872_output.getNumRow() << " -> tbl_JOIN_INNER_TD_3872_output" 
     << std::endl << "      #COL: 8: " << "ListBuffer(p_partkey#258, p_mfgr#260, s_name#343, s_address#344, s_nationkey#345, s_phone#346, s_acctbal#347, s_comment#348)" 
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_3217_input_stringRowIDSubstitute.getNumRow() << " -> tbl_SerializeFromObject_TD_3217_input_stringRowIDSubstitute" 
     << std::endl << "      #COL: 3: " << "ListBuffer(n_nationkey#144, n_name#145, n_regionkey#146)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_2441_output.getNumRow() << " -> tbl_JOIN_INNER_TD_2441_output" 
     << std::endl << "      #COL: 9: " << "ListBuffer(p_partkey#258, p_mfgr#260, s_name#343, s_address#344, s_phone#346, s_acctbal#347, s_comment#348, n_name#145, n_regionkey#146)" 
     << std::endl; 

    std::cout << "JOIN_INNER_1: " << tvdiff(&tv_r_JOIN_INNER_1_449_s, &tv_r_JOIN_INNER_1_449_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer((n_regionkey#146 = r_regionkey#174)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_2441_output.getNumRow() << " -> tbl_JOIN_INNER_TD_2441_output" 
     << std::endl << "      #COL: 9: " << "ListBuffer(p_partkey#258, p_mfgr#260, s_name#343, s_address#344, s_phone#346, s_acctbal#347, s_comment#348, n_name#145, n_regionkey#146)" 
     << std::endl << "      #ROW: " << tbl_Filter_TD_8930_output.getNumRow() << " -> tbl_Filter_TD_8930_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(r_regionkey#174)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_1772_output.getNumRow() << " -> tbl_JOIN_INNER_TD_1772_output" 
     << std::endl << "      #COL: 8: " << "ListBuffer(s_acctbal#347, s_name#343, n_name#145, p_partkey#258, p_mfgr#260, s_address#344, s_phone#346, s_comment#348)" 
     << std::endl; 

    std::cout << "Sort_0: " << tvdiff(&tv_r_Sort_0_388_s, &tv_r_Sort_0_388_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(s_acctbal#347 DESC NULLS LAST, n_name#145 ASC NULLS FIRST, s_name#343 ASC NULLS FIRST, p_partkey#258 ASC NULLS FIRST) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_1772_output.getNumRow() << " -> tbl_JOIN_INNER_TD_1772_output" 
     << std::endl << "      #COL: 8: " << "ListBuffer(s_acctbal#347, s_name#343, n_name#145, p_partkey#258, p_mfgr#260, s_address#344, s_phone#346, s_comment#348)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Sort_TD_0574_output.getNumRow() << " -> tbl_Sort_TD_0574_output" 
     << std::endl << "      #COL: 8: " << "ListBuffer(s_acctbal#347, s_name#343, n_name#145, p_partkey#258, p_mfgr#260, s_address#344, s_phone#346, s_comment#348)" 
     << std::endl; 

    std::cout << std::endl << " Total execution time: " << tvdiff(&tv_r_s, &tv_r_e) / 1000 << " ms"; 

    std::cout << std::endl << " Spark elapsed time: " << 3.8801997 * 1000 << "ms" << std::endl; 
    return 0; 
}
