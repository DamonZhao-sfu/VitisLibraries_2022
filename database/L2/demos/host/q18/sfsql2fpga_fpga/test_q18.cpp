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
    Table tbl_Sort_TD_0197_output("tbl_Sort_TD_0197_output", 183000000, 6, "");
    tbl_Sort_TD_0197_output.allocateHost();
    Table tbl_Aggregate_TD_1635_output("tbl_Aggregate_TD_1635_output", 183000000, 6, "");
    tbl_Aggregate_TD_1635_output.allocateHost();
    Table tbl_JOIN_INNER_TD_2234_output("tbl_JOIN_INNER_TD_2234_output", 100, 6, "");
    tbl_JOIN_INNER_TD_2234_output.allocateHost(1.2, hpTimes_join);
    Table tbl_JOIN_INNER_TD_3777_output("tbl_JOIN_INNER_TD_3777_output", 100, 5, "");
    tbl_JOIN_INNER_TD_3777_output.allocateHost(1.2, hpTimes_join);
    Table tbl_JOIN_LEFTSEMI_TD_342_output("tbl_JOIN_LEFTSEMI_TD_342_output", 100, 2, "");
    tbl_JOIN_LEFTSEMI_TD_342_output.allocateHost(1.2, hpTimes_join);
    Table tbl_SerializeFromObject_TD_4949_input_stringRowIDSubstitute;
    tbl_SerializeFromObject_TD_4949_input_stringRowIDSubstitute = Table("customer", customer_n, 2, in_dir);
    tbl_SerializeFromObject_TD_4949_input_stringRowIDSubstitute.addCol("c_custkey", 4);
    tbl_SerializeFromObject_TD_4949_input_stringRowIDSubstitute.addCol("c_name", 4, 1, 0);
    tbl_SerializeFromObject_TD_4949_input_stringRowIDSubstitute.allocateHost();
    tbl_SerializeFromObject_TD_4949_input_stringRowIDSubstitute.loadHost();
    Table tbl_SerializeFromObject_TD_4949_input_stringRowIDSubstitute_partition("tbl_SerializeFromObject_TD_4949_input_stringRowIDSubstitute_partition", customer_n, 2, "");
    tbl_SerializeFromObject_TD_4949_input_stringRowIDSubstitute_partition.allocateHost(1.2, hpTimes_join);
    tbl_SerializeFromObject_TD_4949_input_stringRowIDSubstitute_partition.allocateDevBuffer(context_h, 32);
    Table tbl_SerializeFromObject_TD_4949_input_stringRowIDSubstitute_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_SerializeFromObject_TD_4949_input_stringRowIDSubstitute_partition_array[i] = tbl_SerializeFromObject_TD_4949_input_stringRowIDSubstitute_partition.createSubTable(i);
    }
    Table tbl_SerializeFromObject_TD_4949_input;
    tbl_SerializeFromObject_TD_4949_input = Table("customer", customer_n, 2, in_dir);
    tbl_SerializeFromObject_TD_4949_input.addCol("c_custkey", 4);
    tbl_SerializeFromObject_TD_4949_input.addCol("c_name", TPCH_READ_C_NAME_LEN+1);
    tbl_SerializeFromObject_TD_4949_input.allocateHost();
    tbl_SerializeFromObject_TD_4949_input.loadHost();
    Table tbl_JOIN_LEFTSEMI_TD_4183_output("tbl_JOIN_LEFTSEMI_TD_4183_output", 100, 4, "");
    tbl_JOIN_LEFTSEMI_TD_4183_output.allocateHost(1.2, hpTimes_join);
    Table tbl_SerializeFromObject_TD_4186_input;
    tbl_SerializeFromObject_TD_4186_input = Table("lineitem", lineitem_n, 2, in_dir);
    tbl_SerializeFromObject_TD_4186_input.selectOverlayVersion(0);
    tbl_SerializeFromObject_TD_4186_input.addCol("l_orderkey", 4);
    tbl_SerializeFromObject_TD_4186_input.addCol("l_quantity", 4);
    tbl_SerializeFromObject_TD_4186_input.allocateHost();
    tbl_SerializeFromObject_TD_4186_input.loadHost();
    Table tbl_SerializeFromObject_TD_4186_input_partition("tbl_SerializeFromObject_TD_4186_input_partition", lineitem_n, 2, "");
    tbl_SerializeFromObject_TD_4186_input_partition.allocateHost(1.2, hpTimes_join);
    tbl_SerializeFromObject_TD_4186_input_partition.allocateDevBuffer(context_h, 32);
    Table tbl_SerializeFromObject_TD_4186_input_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_SerializeFromObject_TD_4186_input_partition_array[i] = tbl_SerializeFromObject_TD_4186_input_partition.createSubTable(i);
    }
    Table tbl_Filter_TD_4454_output("tbl_Filter_TD_4454_output", 183000000, 1, "");
    tbl_Filter_TD_4454_output.selectOverlayVersion(0);
    tbl_Filter_TD_4454_output.allocateHost();
    Table tbl_Filter_TD_4454_output_partition("tbl_Filter_TD_4454_output_partition", 183000000, 1, "");
    tbl_Filter_TD_4454_output_partition.allocateHost(1.2, hpTimes_join);
    tbl_Filter_TD_4454_output_partition.allocateDevBuffer(context_h, 32);
    Table tbl_Filter_TD_4454_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_Filter_TD_4454_output_partition_array[i] = tbl_Filter_TD_4454_output_partition.createSubTable(i);
    }
    Table tbl_SerializeFromObject_TD_5145_input;
    tbl_SerializeFromObject_TD_5145_input = Table("order", order_n, 4, in_dir);
    tbl_SerializeFromObject_TD_5145_input.selectOverlayVersion(0);
    tbl_SerializeFromObject_TD_5145_input.addCol("o_orderkey", 4);
    tbl_SerializeFromObject_TD_5145_input.addCol("o_custkey", 4);
    tbl_SerializeFromObject_TD_5145_input.addCol("o_totalprice", 4);
    tbl_SerializeFromObject_TD_5145_input.addCol("o_orderdate", 4);
    tbl_SerializeFromObject_TD_5145_input.allocateHost();
    tbl_SerializeFromObject_TD_5145_input.loadHost();
    Table tbl_SerializeFromObject_TD_5145_input_partition("tbl_SerializeFromObject_TD_5145_input_partition", order_n, 4, "");
    tbl_SerializeFromObject_TD_5145_input_partition.allocateHost(1.2, hpTimes_join);
    tbl_SerializeFromObject_TD_5145_input_partition.allocateDevBuffer(context_h, 32);
    Table tbl_SerializeFromObject_TD_5145_input_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_SerializeFromObject_TD_5145_input_partition_array[i] = tbl_SerializeFromObject_TD_5145_input_partition.createSubTable(i);
    }
    Table tbl_Filter_TD_523_output("tbl_Filter_TD_523_output", 183000000, 1, "");
    tbl_Filter_TD_523_output.selectOverlayVersion(0);
    tbl_Filter_TD_523_output.allocateHost();
    Table tbl_Filter_TD_523_output_partition("tbl_Filter_TD_523_output_partition", 183000000, 1, "");
    tbl_Filter_TD_523_output_partition.allocateHost(1.2, hpTimes_join);
    tbl_Filter_TD_523_output_partition.allocateDevBuffer(context_h, 32);
    Table tbl_Filter_TD_523_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_Filter_TD_523_output_partition_array[i] = tbl_Filter_TD_523_output_partition.createSubTable(i);
    }
    Table tbl_Aggregate_TD_6900_output_preprocess("tbl_Aggregate_TD_6900_output_preprocess", 3000000, 16, "");
    tbl_Aggregate_TD_6900_output_preprocess.allocateHost(1.2, hpTimes_aggr);
    Table tbl_Aggregate_TD_6900_output("tbl_Aggregate_TD_6900_output", 3000000, 2, "");
    Table tbl_SerializeFromObject_TD_7413_input;
    tbl_SerializeFromObject_TD_7413_input = Table("lineitem", lineitem_n, 2, in_dir);
    tbl_SerializeFromObject_TD_7413_input.addCol("l_orderkey", 4);
    tbl_SerializeFromObject_TD_7413_input.addCol("l_quantity", 4);
    tbl_SerializeFromObject_TD_7413_input.allocateHost();
    tbl_SerializeFromObject_TD_7413_input.loadHost();
    Table tbl_SerializeFromObject_TD_7413_input_partition("tbl_SerializeFromObject_TD_7413_input_partition", lineitem_n, 2, "");
    tbl_SerializeFromObject_TD_7413_input_partition.allocateHost(1.2, hpTimes_aggr);
    tbl_SerializeFromObject_TD_7413_input_partition.allocateDevBuffer(context_a, 32);
    Table tbl_SerializeFromObject_TD_7413_input_partition_array[hpTimes_aggr];
    for (int i(0); i < hpTimes_aggr; ++i) {
        tbl_SerializeFromObject_TD_7413_input_partition_array[i] = tbl_SerializeFromObject_TD_7413_input_partition.createSubTable(i);
    }
    // ********************** Allocate Device Buffer ******************** // 
    tbl_JOIN_INNER_TD_2234_output.allocateDevBuffer(context_h, 32);
    Table tbl_JOIN_INNER_TD_2234_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_JOIN_INNER_TD_2234_output_partition_array[i] = tbl_JOIN_INNER_TD_2234_output.createSubTable(i);
    }
    tbl_JOIN_INNER_TD_3777_output.allocateDevBuffer(context_h, 32);
    Table tbl_JOIN_INNER_TD_3777_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_JOIN_INNER_TD_3777_output_partition_array[i] = tbl_JOIN_INNER_TD_3777_output.createSubTable(i);
    }
    tbl_JOIN_LEFTSEMI_TD_342_output.allocateDevBuffer(context_h, 32);
    Table tbl_JOIN_LEFTSEMI_TD_342_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_JOIN_LEFTSEMI_TD_342_output_partition_array[i] = tbl_JOIN_LEFTSEMI_TD_342_output.createSubTable(i);
    }
    tbl_SerializeFromObject_TD_4949_input_stringRowIDSubstitute.allocateDevBuffer(context_h, 33);
    tbl_JOIN_LEFTSEMI_TD_4183_output.allocateDevBuffer(context_h, 32);
    Table tbl_JOIN_LEFTSEMI_TD_4183_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_JOIN_LEFTSEMI_TD_4183_output_partition_array[i] = tbl_JOIN_LEFTSEMI_TD_4183_output.createSubTable(i);
    }
    tbl_SerializeFromObject_TD_4186_input.allocateDevBuffer(context_h, 33);
    tbl_Filter_TD_4454_output.allocateDevBuffer(context_h, 33);
    tbl_SerializeFromObject_TD_5145_input.allocateDevBuffer(context_h, 33);
    tbl_Filter_TD_523_output.allocateDevBuffer(context_h, 33);
    tbl_Aggregate_TD_6900_output_preprocess.allocateDevBuffer(context_a, 33);
    Table tbl_Aggregate_TD_6900_output_partition_array[hpTimes_aggr];
    for (int i(0); i < hpTimes_aggr; ++i) {
        tbl_Aggregate_TD_6900_output_partition_array[i] = tbl_Aggregate_TD_6900_output_preprocess.createSubTable(i);
    }
    tbl_SerializeFromObject_TD_7413_input.allocateDevBuffer(context_a, 33);
    // ****************************** Config **************************** // 
    cfgCmd cfg_JOIN_INNER_TD_2234_cmds;
    cfg_JOIN_INNER_TD_2234_cmds.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_2234_gqe_join (cfg_JOIN_INNER_TD_2234_cmds.cmd);
    cfg_JOIN_INNER_TD_2234_cmds.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_3777_cmds;
    cfg_JOIN_INNER_TD_3777_cmds.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_3777_gqe_join (cfg_JOIN_INNER_TD_3777_cmds.cmd);
    cfg_JOIN_INNER_TD_3777_cmds.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_3777_cmds_part;
    cfg_JOIN_INNER_TD_3777_cmds_part.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_3777_gqe_join_part (cfg_JOIN_INNER_TD_3777_cmds_part.cmd);
    cfg_JOIN_INNER_TD_3777_cmds_part.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_LEFTSEMI_TD_342_cmds;
    cfg_JOIN_LEFTSEMI_TD_342_cmds.allocateHost();
    get_cfg_dat_JOIN_LEFTSEMI_TD_342_gqe_join (cfg_JOIN_LEFTSEMI_TD_342_cmds.cmd);
    cfg_JOIN_LEFTSEMI_TD_342_cmds.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_LEFTSEMI_TD_342_cmds_part;
    cfg_JOIN_LEFTSEMI_TD_342_cmds_part.allocateHost();
    get_cfg_dat_JOIN_LEFTSEMI_TD_342_gqe_join_part (cfg_JOIN_LEFTSEMI_TD_342_cmds_part.cmd);
    cfg_JOIN_LEFTSEMI_TD_342_cmds_part.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_LEFTSEMI_TD_4183_cmds;
    cfg_JOIN_LEFTSEMI_TD_4183_cmds.allocateHost();
    get_cfg_dat_JOIN_LEFTSEMI_TD_4183_gqe_join (cfg_JOIN_LEFTSEMI_TD_4183_cmds.cmd);
    cfg_JOIN_LEFTSEMI_TD_4183_cmds.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_LEFTSEMI_TD_4183_cmds_part;
    cfg_JOIN_LEFTSEMI_TD_4183_cmds_part.allocateHost();
    get_cfg_dat_JOIN_LEFTSEMI_TD_4183_gqe_join_part (cfg_JOIN_LEFTSEMI_TD_4183_cmds_part.cmd);
    cfg_JOIN_LEFTSEMI_TD_4183_cmds_part.allocateDevBuffer(context_h, 32);
    AggrCfgCmd cfg_Aggregate_TD_6900_cmds;
    cfg_Aggregate_TD_6900_cmds.allocateHost();
    get_cfg_dat_Aggregate_TD_6900_gqe_aggr(cfg_Aggregate_TD_6900_cmds.cmd);
    cfg_Aggregate_TD_6900_cmds.allocateDevBuffer(context_a, 32);
    AggrCfgCmd cfg_Aggregate_TD_6900_cmds_out;
    cfg_Aggregate_TD_6900_cmds_out.allocateHost();
    cfg_Aggregate_TD_6900_cmds_out.allocateDevBuffer(context_a, 33);
    // *************************** Kernel Setup ************************* // 
    bufferTmp buftmp_h(context_h, 0); 
    buftmp_h.initBuffer(q_h); 
    AggrBufferTmp buftmp_a(context_a, 0); 
    buftmp_a.BufferInitial(q_a); 
    std::cout << std::endl; 
    krnlEngine krnl_JOIN_INNER_TD_2234[hpTimes_join];
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_INNER_TD_2234[i] = krnlEngine(program_h, q_h, "gqeJoin");
    }
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_INNER_TD_2234[i].setup(tbl_JOIN_INNER_TD_3777_output_partition_array[i], tbl_JOIN_LEFTSEMI_TD_342_output_partition_array[i], tbl_JOIN_INNER_TD_2234_output_partition_array[i], cfg_JOIN_INNER_TD_2234_cmds, buftmp_h);
    }
    krnlEngine krnl_JOIN_INNER_TD_2234_build;
    krnl_JOIN_INNER_TD_2234_build = krnlEngine(program_h, q_h, context_h, "gqeKernel", 0);
    krnl_JOIN_INNER_TD_2234_build.setup(tbl_JOIN_INNER_TD_3777_output, tbl_JOIN_LEFTSEMI_TD_342_output, tbl_JOIN_INNER_TD_2234_output, cfg_JOIN_INNER_TD_2234_cmds_build, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_2234_probe;
    krnl_JOIN_INNER_TD_2234_probe = krnlEngine(program_h, q_h, context_h, "gqeKernel", 1);
    krnl_JOIN_INNER_TD_2234_probe.setup(tbl_JOIN_LEFTSEMI_TD_342_output, tbl_JOIN_INNER_TD_3777_output, tbl_JOIN_INNER_TD_2234_output, cfg_JOIN_INNER_TD_2234_cmds_probe, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_3777_part_left;
    krnl_JOIN_INNER_TD_3777_part_left = krnlEngine(program_h, q_h, context_h, "gqePart");
    krnl_JOIN_INNER_TD_3777_part_left.setup_hp(512, 0, power_of_hpTimes_join, tbl_SerializeFromObject_TD_4949_input_stringRowIDSubstitute, tbl_SerializeFromObject_TD_4949_input_stringRowIDSubstitute_partition, cfg_JOIN_INNER_TD_3777_cmds_part);
    krnlEngine krnl_JOIN_INNER_TD_3777[hpTimes_join];
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_INNER_TD_3777[i] = krnlEngine(program_h, q_h, "gqeJoin");
    }
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_INNER_TD_3777[i].setup(tbl_SerializeFromObject_TD_4949_input_stringRowIDSubstitute_partition_array[i], tbl_JOIN_LEFTSEMI_TD_4183_output_partition_array[i], tbl_JOIN_INNER_TD_3777_output_partition_array[i], cfg_JOIN_INNER_TD_3777_cmds, buftmp_h);
    }
    krnlEngine krnl_JOIN_INNER_TD_3777_build;
    krnl_JOIN_INNER_TD_3777_build = krnlEngine(program_h, q_h, context_h, "gqeKernel", 0);
    krnl_JOIN_INNER_TD_3777_build.setup(tbl_SerializeFromObject_TD_4949_input_stringRowIDSubstitute, tbl_JOIN_LEFTSEMI_TD_4183_output, tbl_JOIN_INNER_TD_3777_output, cfg_JOIN_INNER_TD_3777_cmds_build, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_3777_probe;
    krnl_JOIN_INNER_TD_3777_probe = krnlEngine(program_h, q_h, context_h, "gqeKernel", 1);
    krnl_JOIN_INNER_TD_3777_probe.setup(tbl_JOIN_LEFTSEMI_TD_4183_output, tbl_SerializeFromObject_TD_4949_input_stringRowIDSubstitute, tbl_JOIN_INNER_TD_3777_output, cfg_JOIN_INNER_TD_3777_cmds_probe, buftmp_h);
    krnlEngine krnl_JOIN_LEFTSEMI_TD_342_part_left;
    krnl_JOIN_LEFTSEMI_TD_342_part_left = krnlEngine(program_h, q_h, context_h, "gqePart");
    krnl_JOIN_LEFTSEMI_TD_342_part_left.setup_hp(512, 0, power_of_hpTimes_join, tbl_Filter_TD_4454_output, tbl_Filter_TD_4454_output_partition, cfg_JOIN_LEFTSEMI_TD_342_cmds_part);
    krnlEngine krnl_JOIN_LEFTSEMI_TD_342_part_right;
    krnl_JOIN_LEFTSEMI_TD_342_part_right = krnlEngine(program_h, q_h, context_h, "gqePart");
    krnl_JOIN_LEFTSEMI_TD_342_part_right.setup_hp(512, 1, power_of_hpTimes_join, tbl_SerializeFromObject_TD_4186_input, tbl_SerializeFromObject_TD_4186_input_partition, cfg_JOIN_LEFTSEMI_TD_342_cmds_part);
    krnlEngine krnl_JOIN_LEFTSEMI_TD_342[hpTimes_join];
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_LEFTSEMI_TD_342[i] = krnlEngine(program_h, q_h, "gqeJoin");
    }
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_LEFTSEMI_TD_342[i].setup(tbl_Filter_TD_4454_output_partition_array[i], tbl_SerializeFromObject_TD_4186_input_partition_array[i], tbl_JOIN_LEFTSEMI_TD_342_output_partition_array[i], cfg_JOIN_LEFTSEMI_TD_342_cmds, buftmp_h);
    }
    krnlEngine krnl_JOIN_LEFTSEMI_TD_342_build;
    krnl_JOIN_LEFTSEMI_TD_342_build = krnlEngine(program_h, q_h, context_h, "gqeKernel", 0);
    krnl_JOIN_LEFTSEMI_TD_342_build.setup(tbl_Filter_TD_4454_output, tbl_SerializeFromObject_TD_4186_input, tbl_JOIN_LEFTSEMI_TD_342_output, cfg_JOIN_LEFTSEMI_TD_342_cmds_build, buftmp_h);
    krnlEngine krnl_JOIN_LEFTSEMI_TD_342_probe;
    krnl_JOIN_LEFTSEMI_TD_342_probe = krnlEngine(program_h, q_h, context_h, "gqeKernel", 1);
    krnl_JOIN_LEFTSEMI_TD_342_probe.setup(tbl_SerializeFromObject_TD_4186_input, tbl_Filter_TD_4454_output, tbl_JOIN_LEFTSEMI_TD_342_output, cfg_JOIN_LEFTSEMI_TD_342_cmds_probe, buftmp_h);
    krnlEngine krnl_JOIN_LEFTSEMI_TD_4183_part_left;
    krnl_JOIN_LEFTSEMI_TD_4183_part_left = krnlEngine(program_h, q_h, context_h, "gqePart");
    krnl_JOIN_LEFTSEMI_TD_4183_part_left.setup_hp(512, 0, power_of_hpTimes_join, tbl_Filter_TD_523_output, tbl_Filter_TD_523_output_partition, cfg_JOIN_LEFTSEMI_TD_4183_cmds_part);
    krnlEngine krnl_JOIN_LEFTSEMI_TD_4183_part_right;
    krnl_JOIN_LEFTSEMI_TD_4183_part_right = krnlEngine(program_h, q_h, context_h, "gqePart");
    krnl_JOIN_LEFTSEMI_TD_4183_part_right.setup_hp(512, 1, power_of_hpTimes_join, tbl_SerializeFromObject_TD_5145_input, tbl_SerializeFromObject_TD_5145_input_partition, cfg_JOIN_LEFTSEMI_TD_4183_cmds_part);
    krnlEngine krnl_JOIN_LEFTSEMI_TD_4183[hpTimes_join];
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_LEFTSEMI_TD_4183[i] = krnlEngine(program_h, q_h, "gqeJoin");
    }
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_LEFTSEMI_TD_4183[i].setup(tbl_Filter_TD_523_output_partition_array[i], tbl_SerializeFromObject_TD_5145_input_partition_array[i], tbl_JOIN_LEFTSEMI_TD_4183_output_partition_array[i], cfg_JOIN_LEFTSEMI_TD_4183_cmds, buftmp_h);
    }
    krnlEngine krnl_JOIN_LEFTSEMI_TD_4183_build;
    krnl_JOIN_LEFTSEMI_TD_4183_build = krnlEngine(program_h, q_h, context_h, "gqeKernel", 0);
    krnl_JOIN_LEFTSEMI_TD_4183_build.setup(tbl_Filter_TD_523_output, tbl_SerializeFromObject_TD_5145_input, tbl_JOIN_LEFTSEMI_TD_4183_output, cfg_JOIN_LEFTSEMI_TD_4183_cmds_build, buftmp_h);
    krnlEngine krnl_JOIN_LEFTSEMI_TD_4183_probe;
    krnl_JOIN_LEFTSEMI_TD_4183_probe = krnlEngine(program_h, q_h, context_h, "gqeKernel", 1);
    krnl_JOIN_LEFTSEMI_TD_4183_probe.setup(tbl_SerializeFromObject_TD_5145_input, tbl_Filter_TD_523_output, tbl_JOIN_LEFTSEMI_TD_4183_output, cfg_JOIN_LEFTSEMI_TD_4183_cmds_probe, buftmp_h);
    AggrKrnlEngine krnl_Aggregate_TD_6900_part;
    krnl_Aggregate_TD_6900_part = AggrKrnlEngine(program_a, q_a, "gqePart");
    krnl_Aggregate_TD_6900_part.setup_hp(512, 0, power_of_hpTimes_aggr, tbl_SerializeFromObject_TD_7413_input, tbl_SerializeFromObject_TD_7413_input_partition, cfg_Aggregate_TD_6900_cmds_part);
    AggrKrnlEngine krnl_Aggregate_TD_6900[hpTimes_aggr];
    for (int i = 0; i < hpTimes_aggr; i++) {
        krnl_Aggregate_TD_6900[i] = AggrKrnlEngine(program_a, q_a, "gqeAggr");
    }
    for (int i = 0; i < hpTimes_aggr; i++) {
        krnl_Aggregate_TD_6900[i].setup(tbl_SerializeFromObject_TD_7413_input_partition_array[i], tbl_Aggregate_TD_6900_output_partition_array[i], cfg_Aggregate_TD_6900_cmds, cfg_Aggregate_TD_6900_cmds_out, buftmp_a);
    }
    // ************************** Transfer Engine *********************** // 
    transEngine trans_JOIN_INNER_TD_2234;
    trans_JOIN_INNER_TD_2234.setq(q_h,0);
    trans_JOIN_INNER_TD_2234.add(&(cfg_JOIN_INNER_TD_2234_cmds_part));
    trans_JOIN_INNER_TD_2234.add(&(cfg_JOIN_INNER_TD_2234_cmds));
    transEngine trans_JOIN_INNER_TD_2234_out;
    trans_JOIN_INNER_TD_2234_out.setq(q_h,0);
    q_h.finish();
    transEngine trans_JOIN_INNER_TD_3777;
    trans_JOIN_INNER_TD_3777.setq(q_h,0);
    trans_JOIN_INNER_TD_3777.add(&(cfg_JOIN_INNER_TD_3777_cmds_part));
    trans_JOIN_INNER_TD_3777.add(&(cfg_JOIN_INNER_TD_3777_cmds));
    trans_JOIN_INNER_TD_3777.add(&(tbl_SerializeFromObject_TD_4949_input_stringRowIDSubstitute));
    q_h.finish();
    transEngine trans_JOIN_LEFTSEMI_TD_342;
    trans_JOIN_LEFTSEMI_TD_342.setq(q_h,0);
    trans_JOIN_LEFTSEMI_TD_342.add(&(cfg_JOIN_LEFTSEMI_TD_342_cmds_part));
    trans_JOIN_LEFTSEMI_TD_342.add(&(cfg_JOIN_LEFTSEMI_TD_342_cmds));
    trans_JOIN_LEFTSEMI_TD_342.add(&(tbl_SerializeFromObject_TD_4186_input));
    q_h.finish();
    transEngine trans_JOIN_LEFTSEMI_TD_4183;
    trans_JOIN_LEFTSEMI_TD_4183.setq(q_h,0);
    trans_JOIN_LEFTSEMI_TD_4183.add(&(cfg_JOIN_LEFTSEMI_TD_4183_cmds_part));
    trans_JOIN_LEFTSEMI_TD_4183.add(&(cfg_JOIN_LEFTSEMI_TD_4183_cmds));
    trans_JOIN_LEFTSEMI_TD_4183.add(&(tbl_SerializeFromObject_TD_5145_input));
    q_h.finish();
    transEngine trans_Aggregate_TD_6900;
    trans_Aggregate_TD_6900.setq(q_a, 0);
    trans_Aggregate_TD_6900.add(&(cfg_Aggregate_TD_6900_cmds_part));
    trans_Aggregate_TD_6900.add(&(cfg_Aggregate_TD_6900_cmds));
    trans_Aggregate_TD_6900.add(&(cfg_Aggregate_TD_6900_cmds_out));
    trans_Aggregate_TD_6900.add(&(tbl_SerializeFromObject_TD_7413_input));
    transEngine trans_Aggregate_TD_6900_out;
    trans_Aggregate_TD_6900_out.setq(q_a, 0);
    q_a.finish();
    // ****************************** Events **************************** // 
    std::vector<cl::Event> events_h2d_wr_JOIN_INNER_TD_2234;
    std::vector<cl::Event> events_d2h_rd_JOIN_INNER_TD_2234;
    std::vector<cl::Event> events_JOIN_INNER_TD_2234[2];
    events_h2d_wr_JOIN_INNER_TD_2234.resize(1);
    events_d2h_rd_JOIN_INNER_TD_2234.resize(1);
    events_JOIN_INNER_TD_2234[0].resize(0);
    events_JOIN_INNER_TD_2234[1].resize(hpTimes_join);
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_2234;
    std::vector<cl::Event> prev_events_grp_JOIN_INNER_TD_2234;
    std::vector<cl::Event> events_h2d_wr_JOIN_INNER_TD_3777;
    std::vector<cl::Event> events_d2h_rd_JOIN_INNER_TD_3777;
    std::vector<cl::Event> events_JOIN_INNER_TD_3777[2];
    events_h2d_wr_JOIN_INNER_TD_3777.resize(1);
    events_d2h_rd_JOIN_INNER_TD_3777.resize(1);
    events_JOIN_INNER_TD_3777[0].resize(1);
    events_JOIN_INNER_TD_3777[1].resize(hpTimes_join);
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_3777;
    std::vector<cl::Event> prev_events_grp_JOIN_INNER_TD_3777;
    std::vector<cl::Event> events_h2d_wr_JOIN_LEFTSEMI_TD_342;
    std::vector<cl::Event> events_d2h_rd_JOIN_LEFTSEMI_TD_342;
    std::vector<cl::Event> events_JOIN_LEFTSEMI_TD_342[2];
    events_h2d_wr_JOIN_LEFTSEMI_TD_342.resize(1);
    events_d2h_rd_JOIN_LEFTSEMI_TD_342.resize(1);
    events_JOIN_LEFTSEMI_TD_342[0].resize(2);
    events_JOIN_LEFTSEMI_TD_342[1].resize(hpTimes_join);
    std::vector<cl::Event> events_grp_JOIN_LEFTSEMI_TD_342;
    std::vector<cl::Event> prev_events_grp_JOIN_LEFTSEMI_TD_342;
    std::vector<cl::Event> events_h2d_wr_JOIN_LEFTSEMI_TD_4183;
    std::vector<cl::Event> events_d2h_rd_JOIN_LEFTSEMI_TD_4183;
    std::vector<cl::Event> events_JOIN_LEFTSEMI_TD_4183[2];
    events_h2d_wr_JOIN_LEFTSEMI_TD_4183.resize(1);
    events_d2h_rd_JOIN_LEFTSEMI_TD_4183.resize(1);
    events_JOIN_LEFTSEMI_TD_4183[0].resize(2);
    events_JOIN_LEFTSEMI_TD_4183[1].resize(hpTimes_join);
    std::vector<cl::Event> events_grp_JOIN_LEFTSEMI_TD_4183;
    std::vector<cl::Event> prev_events_grp_JOIN_LEFTSEMI_TD_4183;
    std::vector<cl::Event> events_h2d_wr_Aggregate_TD_6900;
    std::vector<cl::Event> events_d2h_rd_Aggregate_TD_6900;
    std::vector<cl::Event> events_Aggregate_TD_6900[2];
    events_h2d_wr_Aggregate_TD_6900.resize(1);
    events_d2h_rd_Aggregate_TD_6900.resize(1);
    events_Aggregate_TD_6900[0].resize(1);
    events_Aggregate_TD_6900[1].resize(hpTimes_aggr);
    std::vector<cl::Event> events_grp_Aggregate_TD_6900;
    std::vector<cl::Event> prev_events_grp_Aggregate_TD_6900;
    // **************************** Operations ************************** // 
    struct timeval tv_r_s, tv_r_e; 
    gettimeofday(&tv_r_s, 0); 

    struct timeval tv_r_Aggregate_6_809_s, tv_r_Aggregate_6_809_e;
    gettimeofday(&tv_r_Aggregate_6_809_s, 0);
    trans_Aggregate_TD_6900.host2dev(0, &(prev_events_grp_Aggregate_TD_6900), &(events_h2d_wr_Aggregate_TD_6900[0]));
    events_grp_Aggregate_TD_6900.push_back(events_h2d_wr_Aggregate_TD_6900[0]);
    krnl_Aggregate_TD_6900_part.run(0, &(events_grp_Aggregate_TD_6900), &(events_Aggregate_TD_6900[0][0]));
    for (int i(0); i < hpTimes_aggr; ++i) {
        krnl_Aggregate_TD_6900[i].run(0, &(events_Aggregate_TD_6900[0]), &(events_Aggregate_TD_6900[1][i]));
    }
    
    for (int i(0); i < hpTimes_aggr; ++i) {
        trans_Aggregate_TD_6900_out.add(&(tbl_Aggregate_TD_6900_output_partition_array[i]));
    }
    trans_Aggregate_TD_6900_out.dev2host(0, &(events_Aggregate_TD_6900[1]), &(events_d2h_rd_Aggregate_TD_6900[0]));
    q_a.flush();
    q_a.finish();
    
    SW_Aggregate_TD_6900_consolidate(tbl_Aggregate_TD_6900_output_partition_array, tbl_Aggregate_TD_6900_output, hpTimes_aggr);
    gettimeofday(&tv_r_Aggregate_6_809_e, 0);

    struct timeval tv_r_Filter_5_145_s, tv_r_Filter_5_145_e;
    gettimeofday(&tv_r_Filter_5_145_s, 0);
    SW_Filter_TD_523(tbl_Aggregate_TD_6900_output, tbl_Filter_TD_523_output);
    gettimeofday(&tv_r_Filter_5_145_e, 0);

    struct timeval tv_r_Filter_4_86_s, tv_r_Filter_4_86_e;
    gettimeofday(&tv_r_Filter_4_86_s, 0);
    SW_Filter_TD_4454(tbl_Aggregate_TD_6900_output, tbl_Filter_TD_4454_output);
    gettimeofday(&tv_r_Filter_4_86_e, 0);

    struct timeval tv_r_JOIN_LEFTSEMI_4_755_s, tv_r_JOIN_LEFTSEMI_4_755_e;
    gettimeofday(&tv_r_JOIN_LEFTSEMI_4_755_s, 0);
    trans_JOIN_LEFTSEMI_TD_4183.add(&(tbl_Filter_TD_523_output));
    trans_JOIN_LEFTSEMI_TD_4183.host2dev(0, &(prev_events_grp_JOIN_LEFTSEMI_TD_4183), &(events_h2d_wr_JOIN_LEFTSEMI_TD_4183[0]));
    events_grp_JOIN_LEFTSEMI_TD_4183.push_back(events_h2d_wr_JOIN_LEFTSEMI_TD_4183[0]);
    krnl_JOIN_LEFTSEMI_TD_4183_part_left.run(0, &(events_grp_JOIN_LEFTSEMI_TD_4183), &(events_JOIN_LEFTSEMI_TD_4183[0][0]));
    krnl_JOIN_LEFTSEMI_TD_4183_part_right.run(0, &(events_grp_JOIN_LEFTSEMI_TD_4183), &(events_JOIN_LEFTSEMI_TD_4183[0][1]));
    for (int i(0); i < hpTimes_join; ++i) {
        krnl_JOIN_LEFTSEMI_TD_4183[i].run(0, &(events_JOIN_LEFTSEMI_TD_4183[0]), &(events_JOIN_LEFTSEMI_TD_4183[1][i]));
    }
    gettimeofday(&tv_r_JOIN_LEFTSEMI_4_755_e, 0);

    struct timeval tv_r_JOIN_LEFTSEMI_3_790_s, tv_r_JOIN_LEFTSEMI_3_790_e;
    gettimeofday(&tv_r_JOIN_LEFTSEMI_3_790_s, 0);
    trans_JOIN_LEFTSEMI_TD_342.add(&(tbl_Filter_TD_4454_output));
    trans_JOIN_LEFTSEMI_TD_342.host2dev(0, &(prev_events_grp_JOIN_LEFTSEMI_TD_342), &(events_h2d_wr_JOIN_LEFTSEMI_TD_342[0]));
    events_grp_JOIN_LEFTSEMI_TD_342.push_back(events_h2d_wr_JOIN_LEFTSEMI_TD_342[0]);
    krnl_JOIN_LEFTSEMI_TD_342_part_left.run(0, &(events_grp_JOIN_LEFTSEMI_TD_342), &(events_JOIN_LEFTSEMI_TD_342[0][0]));
    krnl_JOIN_LEFTSEMI_TD_342_part_right.run(0, &(events_grp_JOIN_LEFTSEMI_TD_342), &(events_JOIN_LEFTSEMI_TD_342[0][1]));
    for (int i(0); i < hpTimes_join; ++i) {
        krnl_JOIN_LEFTSEMI_TD_342[i].run(0, &(events_JOIN_LEFTSEMI_TD_342[0]), &(events_JOIN_LEFTSEMI_TD_342[1][i]));
    }
    gettimeofday(&tv_r_JOIN_LEFTSEMI_3_790_e, 0);

    struct timeval tv_r_JOIN_INNER_3_960_s, tv_r_JOIN_INNER_3_960_e;
    gettimeofday(&tv_r_JOIN_INNER_3_960_s, 0);
    prev_events_grp_JOIN_INNER_TD_3777.push_back(events_h2d_wr_JOIN_LEFTSEMI_TD_4183[0]);
    trans_JOIN_INNER_TD_3777.host2dev(0, &(prev_events_grp_JOIN_INNER_TD_3777), &(events_h2d_wr_JOIN_INNER_TD_3777[0]));
    events_grp_JOIN_INNER_TD_3777.push_back(events_h2d_wr_JOIN_INNER_TD_3777[0]);
    events_grp_JOIN_INNER_TD_3777.insert(std::end(events_grp_JOIN_INNER_TD_3777), std::begin(events_JOIN_LEFTSEMI_TD_4183[0]), std::end(events_JOIN_LEFTSEMI_TD_4183[0]));
    events_grp_JOIN_INNER_TD_3777.insert(std::end(events_grp_JOIN_INNER_TD_3777), std::begin(events_JOIN_LEFTSEMI_TD_4183[1]), std::end(events_JOIN_LEFTSEMI_TD_4183[1]));
    krnl_JOIN_INNER_TD_3777_part_left.run(0, &(events_grp_JOIN_INNER_TD_3777), &(events_JOIN_INNER_TD_3777[0][0]));
    for (int i(0); i < hpTimes_join; ++i) {
        krnl_JOIN_INNER_TD_3777[i].run(0, &(events_JOIN_INNER_TD_3777[0]), &(events_JOIN_INNER_TD_3777[1][i]));
    }
    gettimeofday(&tv_r_JOIN_INNER_3_960_e, 0);

    struct timeval tv_r_JOIN_INNER_2_925_s, tv_r_JOIN_INNER_2_925_e;
    gettimeofday(&tv_r_JOIN_INNER_2_925_s, 0);
    prev_events_grp_JOIN_INNER_TD_2234.push_back(events_h2d_wr_JOIN_INNER_TD_3777[0]);
    prev_events_grp_JOIN_INNER_TD_2234.push_back(events_h2d_wr_JOIN_LEFTSEMI_TD_342[0]);
    trans_JOIN_INNER_TD_2234.host2dev(0, &(prev_events_grp_JOIN_INNER_TD_2234), &(events_h2d_wr_JOIN_INNER_TD_2234[0]));
    events_grp_JOIN_INNER_TD_2234.push_back(events_h2d_wr_JOIN_INNER_TD_2234[0]);
    events_grp_JOIN_INNER_TD_2234.insert(std::end(events_grp_JOIN_INNER_TD_2234), std::begin(events_JOIN_INNER_TD_3777[0]), std::end(events_JOIN_INNER_TD_3777[0]));
    events_grp_JOIN_INNER_TD_2234.insert(std::end(events_grp_JOIN_INNER_TD_2234), std::begin(events_JOIN_INNER_TD_3777[1]), std::end(events_JOIN_INNER_TD_3777[1]));
    events_grp_JOIN_INNER_TD_2234.insert(std::end(events_grp_JOIN_INNER_TD_2234), std::begin(events_JOIN_LEFTSEMI_TD_342[0]), std::end(events_JOIN_LEFTSEMI_TD_342[0]));
    events_grp_JOIN_INNER_TD_2234.insert(std::end(events_grp_JOIN_INNER_TD_2234), std::begin(events_JOIN_LEFTSEMI_TD_342[1]), std::end(events_JOIN_LEFTSEMI_TD_342[1]));
    for (int i(0); i < hpTimes_join; ++i) {
        krnl_JOIN_INNER_TD_2234[i].run(0, &(events_grp_JOIN_INNER_TD_2234), &(events_JOIN_INNER_TD_2234[1][i]));
    }
    for (int i(0); i < hpTimes_join; ++i) {
        trans_JOIN_INNER_TD_2234_out.add(&(tbl_JOIN_INNER_TD_2234_output_partition_array[i]));
    }
    trans_JOIN_INNER_TD_2234_out.dev2host(0, &(events_JOIN_INNER_TD_2234[1]), &(events_d2h_rd_JOIN_INNER_TD_2234[0]));
    q_h.flush();
    q_h.finish();
    gettimeofday(&tv_r_JOIN_INNER_2_925_e, 0);

    struct timeval tv_r_Aggregate_1_976_s, tv_r_Aggregate_1_976_e;
    gettimeofday(&tv_r_Aggregate_1_976_s, 0);
    SW_Aggregate_TD_1635(tbl_JOIN_INNER_TD_2234_output_partition_array, tbl_SerializeFromObject_TD_4949_input, tbl_Aggregate_TD_1635_output, hpTimes_join);
    gettimeofday(&tv_r_Aggregate_1_976_e, 0);

    struct timeval tv_r_Sort_0_749_s, tv_r_Sort_0_749_e;
    gettimeofday(&tv_r_Sort_0_749_s, 0);
    SW_Sort_TD_0197(tbl_Aggregate_TD_1635_output, tbl_Sort_TD_0197_output);
    gettimeofday(&tv_r_Sort_0_749_e, 0);

std::cout << "CPU TIME: " << (tvdiff(&tv_r_Filter_5_145_s, &tv_r_Filter_5_145_e) + 
tvdiff(&tv_r_Filter_4_86_s, &tv_r_Filter_4_86_e) + 
tvdiff(&tv_r_Aggregate_1_976_s, &tv_r_Aggregate_1_976_e) + 
tvdiff(&tv_r_Sort_0_749_s, &tv_r_Sort_0_749_e) + 
1) / 1000.0 << std::endl;

std::cout << "FPGA TIME: " << (tvdiff(&tv_r_Aggregate_6_809_s, &tv_r_Aggregate_6_809_e) + 
tvdiff(&tv_r_JOIN_LEFTSEMI_4_755_s, &tv_r_JOIN_LEFTSEMI_4_755_e) + 
tvdiff(&tv_r_JOIN_LEFTSEMI_3_790_s, &tv_r_JOIN_LEFTSEMI_3_790_e) + 
tvdiff(&tv_r_JOIN_INNER_3_960_s, &tv_r_JOIN_INNER_3_960_e) + 
tvdiff(&tv_r_JOIN_INNER_2_925_s, &tv_r_JOIN_INNER_2_925_e) + 
1) / 1000.0 << std::endl;

    gettimeofday(&tv_r_e, 0); 
    // **************************** Print Execution Time ************************** // 
    std::cout << std::endl; 
    std::cout << "Aggregate_6: " << tvdiff(&tv_r_Aggregate_6_809_s, &tv_r_Aggregate_6_809_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer(l_orderkey#77, sum(l_quantity#81) AS sum(l_quantity#81)#1717L) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_7413_input.getNumRow() << " -> tbl_SerializeFromObject_TD_7413_input" 
     << std::endl << "      #COL: 2: " << "ListBuffer(l_orderkey#77, l_quantity#81)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_6900_output.getNumRow() << " -> tbl_Aggregate_TD_6900_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(l_orderkey#77, sum(l_quantity#81)#1717L)" 
     << std::endl; 

    std::cout << "Filter_5: " << tvdiff(&tv_r_Filter_5_145_s, &tv_r_Filter_5_145_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer((isnotnull(sum(l_quantity#81)#1717L) AND (sum(l_quantity#81)#1717L > 312))) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_6900_output.getNumRow() << " -> tbl_Aggregate_TD_6900_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(l_orderkey#77, sum(l_quantity#81)#1717L)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_523_output.getNumRow() << " -> tbl_Filter_TD_523_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(l_orderkey#77)" 
     << std::endl; 

    std::cout << "Filter_4: " << tvdiff(&tv_r_Filter_4_86_s, &tv_r_Filter_4_86_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer((isnotnull(sum(l_quantity#81)#1717L) AND (sum(l_quantity#81)#1717L > 312))) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_6900_output.getNumRow() << " -> tbl_Aggregate_TD_6900_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(l_orderkey#77, sum(l_quantity#81)#1717L)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_4454_output.getNumRow() << " -> tbl_Filter_TD_4454_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(l_orderkey#77#1794)" 
     << std::endl; 

    std::cout << "JOIN_LEFTSEMI_4: " << tvdiff(&tv_r_JOIN_LEFTSEMI_4_755_s, &tv_r_JOIN_LEFTSEMI_4_755_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer((o_orderkey#207 = l_orderkey#77)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_5145_input.getNumRow() << " -> tbl_SerializeFromObject_TD_5145_input" 
     << std::endl << "      #COL: 4: " << "ListBuffer(o_orderkey#207, o_custkey#208, o_totalprice#210, o_orderdate#211)" 
     << std::endl << "      #ROW: " << tbl_Filter_TD_523_output.getNumRow() << " -> tbl_Filter_TD_523_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(l_orderkey#77)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_LEFTSEMI_TD_4183_output.getNumRow() << " -> tbl_JOIN_LEFTSEMI_TD_4183_output" 
     << std::endl << "      #COL: 4: " << "ListBuffer(o_orderkey#207, o_custkey#208, o_totalprice#210, o_orderdate#211)" 
     << std::endl; 

    std::cout << "JOIN_LEFTSEMI_3: " << tvdiff(&tv_r_JOIN_LEFTSEMI_3_790_s, &tv_r_JOIN_LEFTSEMI_3_790_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer((l_orderkey#77 = l_orderkey#77#1794)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_4186_input.getNumRow() << " -> tbl_SerializeFromObject_TD_4186_input" 
     << std::endl << "      #COL: 2: " << "ListBuffer(l_orderkey#77, l_quantity#81)" 
     << std::endl << "      #ROW: " << tbl_Filter_TD_4454_output.getNumRow() << " -> tbl_Filter_TD_4454_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(l_orderkey#77#1794)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_LEFTSEMI_TD_342_output.getNumRow() << " -> tbl_JOIN_LEFTSEMI_TD_342_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(l_orderkey#77, l_quantity#81)" 
     << std::endl; 

    std::cout << "JOIN_INNER_3: " << tvdiff(&tv_r_JOIN_INNER_3_960_s, &tv_r_JOIN_INNER_3_960_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer((c_custkey#22 = o_custkey#208)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_4949_input_stringRowIDSubstitute.getNumRow() << " -> tbl_SerializeFromObject_TD_4949_input_stringRowIDSubstitute" 
     << std::endl << "      #COL: 2: " << "ListBuffer(c_custkey#22, c_name#23)" 
     << std::endl << "      #ROW: " << tbl_JOIN_LEFTSEMI_TD_4183_output.getNumRow() << " -> tbl_JOIN_LEFTSEMI_TD_4183_output" 
     << std::endl << "      #COL: 4: " << "ListBuffer(o_orderkey#207, o_custkey#208, o_totalprice#210, o_orderdate#211)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_3777_output.getNumRow() << " -> tbl_JOIN_INNER_TD_3777_output" 
     << std::endl << "      #COL: 5: " << "ListBuffer(c_custkey#22, c_name#23, o_orderkey#207, o_totalprice#210, o_orderdate#211)" 
     << std::endl; 

    std::cout << "JOIN_INNER_2: " << tvdiff(&tv_r_JOIN_INNER_2_925_s, &tv_r_JOIN_INNER_2_925_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer((o_orderkey#207 = l_orderkey#77)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_3777_output.getNumRow() << " -> tbl_JOIN_INNER_TD_3777_output" 
     << std::endl << "      #COL: 5: " << "ListBuffer(c_custkey#22, c_name#23, o_orderkey#207, o_totalprice#210, o_orderdate#211)" 
     << std::endl << "      #ROW: " << tbl_JOIN_LEFTSEMI_TD_342_output.getNumRow() << " -> tbl_JOIN_LEFTSEMI_TD_342_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(l_orderkey#77, l_quantity#81)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_2234_output.getNumRow() << " -> tbl_JOIN_INNER_TD_2234_output" 
     << std::endl << "      #COL: 6: " << "ListBuffer(c_custkey#22, c_name#23, o_orderkey#207, o_totalprice#210, o_orderdate#211, l_quantity#81)" 
     << std::endl; 

    std::cout << "Aggregate_1: " << tvdiff(&tv_r_Aggregate_1_976_s, &tv_r_Aggregate_1_976_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(c_name#23, c_custkey#22, o_orderkey#207, o_orderdate#211, o_totalprice#210, sum(l_quantity#81) AS sum(l_quantity)#1720L) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_2234_output.getNumRow() << " -> tbl_JOIN_INNER_TD_2234_output" 
     << std::endl << "      #COL: 6: " << "ListBuffer(c_custkey#22, c_name#23, o_orderkey#207, o_totalprice#210, o_orderdate#211, l_quantity#81)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_1635_output.getNumRow() << " -> tbl_Aggregate_TD_1635_output" 
     << std::endl << "      #COL: 6: " << "ListBuffer(c_name#23, c_custkey#22, o_orderkey#207, o_orderdate#211, o_totalprice#210, sum(l_quantity)#1720L)" 
     << std::endl; 

    std::cout << "Sort_0: " << tvdiff(&tv_r_Sort_0_749_s, &tv_r_Sort_0_749_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(o_totalprice#210 DESC NULLS LAST, o_orderdate#211 ASC NULLS FIRST) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_1635_output.getNumRow() << " -> tbl_Aggregate_TD_1635_output" 
     << std::endl << "      #COL: 6: " << "ListBuffer(c_name#23, c_custkey#22, o_orderkey#207, o_orderdate#211, o_totalprice#210, sum(l_quantity)#1720L)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Sort_TD_0197_output.getNumRow() << " -> tbl_Sort_TD_0197_output" 
     << std::endl << "      #COL: 6: " << "ListBuffer(c_name#23, c_custkey#22, o_orderkey#207, o_orderdate#211, o_totalprice#210, sum(l_quantity)#1720L)" 
     << std::endl; 

    std::cout << std::endl << " Total execution time: " << tvdiff(&tv_r_s, &tv_r_e) / 1000 << " ms"; 

    std::cout << std::endl << " Spark elapsed time: " << 6139.7324 * 1000 << "ms" << std::endl; 
    return 0; 
}
