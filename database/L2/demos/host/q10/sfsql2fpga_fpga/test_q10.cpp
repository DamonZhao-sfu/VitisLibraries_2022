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
    Table tbl_Sort_TD_0161_output("tbl_Sort_TD_0161_output", 183000000, 5, "");
    tbl_Sort_TD_0161_output.allocateHost();
    Table tbl_Aggregate_TD_1118_output("tbl_Aggregate_TD_1118_output", 183000000, 5, "");
    tbl_Aggregate_TD_1118_output.allocateHost();
    Table tbl_JOIN_INNER_TD_2966_output("tbl_JOIN_INNER_TD_2966_output", 114347, 6, "");
    tbl_JOIN_INNER_TD_2966_output.allocateHost(1.2, hpTimes_join);
    Table tbl_JOIN_INNER_TD_3291_output("tbl_JOIN_INNER_TD_3291_output", 114347, 6, "");
    tbl_JOIN_INNER_TD_3291_output.allocateHost(1.2, hpTimes_join);
    Table tbl_SerializeFromObject_TD_3872_input_stringRowIDSubstitute;
    tbl_SerializeFromObject_TD_3872_input_stringRowIDSubstitute = Table("nation", nation_n, 2, in_dir);
    tbl_SerializeFromObject_TD_3872_input_stringRowIDSubstitute.addCol("n_nationkey", 4);
    tbl_SerializeFromObject_TD_3872_input_stringRowIDSubstitute.addCol("n_name", 4, 1, 0);
    tbl_SerializeFromObject_TD_3872_input_stringRowIDSubstitute.allocateHost();
    tbl_SerializeFromObject_TD_3872_input_stringRowIDSubstitute.loadHost();
    Table tbl_SerializeFromObject_TD_3872_input_stringRowIDSubstitute_partition("tbl_SerializeFromObject_TD_3872_input_stringRowIDSubstitute_partition", nation_n, 2, "");
    tbl_SerializeFromObject_TD_3872_input_stringRowIDSubstitute_partition.allocateHost(1.2, hpTimes_join);
    tbl_SerializeFromObject_TD_3872_input_stringRowIDSubstitute_partition.allocateDevBuffer(context_h, 32);
    Table tbl_SerializeFromObject_TD_3872_input_stringRowIDSubstitute_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_SerializeFromObject_TD_3872_input_stringRowIDSubstitute_partition_array[i] = tbl_SerializeFromObject_TD_3872_input_stringRowIDSubstitute_partition.createSubTable(i);
    }
    Table tbl_SerializeFromObject_TD_3872_input;
    tbl_SerializeFromObject_TD_3872_input = Table("nation", nation_n, 2, in_dir);
    tbl_SerializeFromObject_TD_3872_input.addCol("n_nationkey", 4);
    tbl_SerializeFromObject_TD_3872_input.addCol("n_name", TPCH_READ_NATION_LEN+1);
    tbl_SerializeFromObject_TD_3872_input.allocateHost();
    tbl_SerializeFromObject_TD_3872_input.loadHost();
    Table tbl_JOIN_INNER_TD_4180_output("tbl_JOIN_INNER_TD_4180_output", 57111, 5, "");
    tbl_JOIN_INNER_TD_4180_output.allocateHost(1.2, hpTimes_join);
    Table tbl_Filter_TD_4769_output("tbl_Filter_TD_4769_output", 183000000, 3, "");
    tbl_Filter_TD_4769_output.selectOverlayVersion(0);
    tbl_Filter_TD_4769_output.allocateHost();
    Table tbl_Filter_TD_4769_output_partition("tbl_Filter_TD_4769_output_partition", 183000000, 3, "");
    tbl_Filter_TD_4769_output_partition.allocateHost(1.2, hpTimes_join);
    tbl_Filter_TD_4769_output_partition.allocateDevBuffer(context_h, 32);
    Table tbl_Filter_TD_4769_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_Filter_TD_4769_output_partition_array[i] = tbl_Filter_TD_4769_output_partition.createSubTable(i);
    }
    Table tbl_SerializeFromObject_TD_5871_input_stringRowIDSubstitute;
    tbl_SerializeFromObject_TD_5871_input_stringRowIDSubstitute = Table("customer", customer_n, 4, in_dir);
    tbl_SerializeFromObject_TD_5871_input_stringRowIDSubstitute.addCol("c_custkey", 4);
    tbl_SerializeFromObject_TD_5871_input_stringRowIDSubstitute.addCol("c_name", 4, 1, 0);
    tbl_SerializeFromObject_TD_5871_input_stringRowIDSubstitute.addCol("c_nationkey", 4);
    tbl_SerializeFromObject_TD_5871_input_stringRowIDSubstitute.addCol("c_acctbal", 4);
    tbl_SerializeFromObject_TD_5871_input_stringRowIDSubstitute.allocateHost();
    tbl_SerializeFromObject_TD_5871_input_stringRowIDSubstitute.loadHost();
    Table tbl_SerializeFromObject_TD_5871_input_stringRowIDSubstitute_partition("tbl_SerializeFromObject_TD_5871_input_stringRowIDSubstitute_partition", customer_n, 4, "");
    tbl_SerializeFromObject_TD_5871_input_stringRowIDSubstitute_partition.allocateHost(1.2, hpTimes_join);
    tbl_SerializeFromObject_TD_5871_input_stringRowIDSubstitute_partition.allocateDevBuffer(context_h, 32);
    Table tbl_SerializeFromObject_TD_5871_input_stringRowIDSubstitute_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_SerializeFromObject_TD_5871_input_stringRowIDSubstitute_partition_array[i] = tbl_SerializeFromObject_TD_5871_input_stringRowIDSubstitute_partition.createSubTable(i);
    }
    Table tbl_SerializeFromObject_TD_5871_input;
    tbl_SerializeFromObject_TD_5871_input = Table("customer", customer_n, 4, in_dir);
    tbl_SerializeFromObject_TD_5871_input.addCol("c_custkey", 4);
    tbl_SerializeFromObject_TD_5871_input.addCol("c_name", TPCH_READ_C_NAME_LEN+1);
    tbl_SerializeFromObject_TD_5871_input.addCol("c_nationkey", 4);
    tbl_SerializeFromObject_TD_5871_input.addCol("c_acctbal", 4);
    tbl_SerializeFromObject_TD_5871_input.allocateHost();
    tbl_SerializeFromObject_TD_5871_input.loadHost();
    Table tbl_Filter_TD_531_output("tbl_Filter_TD_531_output", 183000000, 2, "");
    tbl_Filter_TD_531_output.selectOverlayVersion(0);
    tbl_Filter_TD_531_output.allocateHost();
    Table tbl_Filter_TD_531_output_partition("tbl_Filter_TD_531_output_partition", 183000000, 2, "");
    tbl_Filter_TD_531_output_partition.allocateHost(1.2, hpTimes_join);
    tbl_Filter_TD_531_output_partition.allocateDevBuffer(context_h, 32);
    Table tbl_Filter_TD_531_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_Filter_TD_531_output_partition_array[i] = tbl_Filter_TD_531_output_partition.createSubTable(i);
    }
    Table tbl_SerializeFromObject_TD_5599_input;
    tbl_SerializeFromObject_TD_5599_input = Table("lineitem", lineitem_n, 4, in_dir);
    tbl_SerializeFromObject_TD_5599_input.addCol("l_orderkey", 4);
    tbl_SerializeFromObject_TD_5599_input.addCol("l_extendedprice", 4);
    tbl_SerializeFromObject_TD_5599_input.addCol("l_discount", 4);
    tbl_SerializeFromObject_TD_5599_input.addCol("l_returnflag", 4);
    tbl_SerializeFromObject_TD_5599_input.allocateHost();
    tbl_SerializeFromObject_TD_5599_input.loadHost();
    Table tbl_SerializeFromObject_TD_6198_input;
    tbl_SerializeFromObject_TD_6198_input = Table("order", order_n, 3, in_dir);
    tbl_SerializeFromObject_TD_6198_input.addCol("o_orderkey", 4);
    tbl_SerializeFromObject_TD_6198_input.addCol("o_custkey", 4);
    tbl_SerializeFromObject_TD_6198_input.addCol("o_orderdate", 4);
    tbl_SerializeFromObject_TD_6198_input.allocateHost();
    tbl_SerializeFromObject_TD_6198_input.loadHost();
    // ********************** Allocate Device Buffer ******************** // 
    tbl_JOIN_INNER_TD_2966_output.allocateDevBuffer(context_h, 32);
    Table tbl_JOIN_INNER_TD_2966_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_JOIN_INNER_TD_2966_output_partition_array[i] = tbl_JOIN_INNER_TD_2966_output.createSubTable(i);
    }
    tbl_JOIN_INNER_TD_3291_output.allocateDevBuffer(context_h, 32);
    Table tbl_JOIN_INNER_TD_3291_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_JOIN_INNER_TD_3291_output_partition_array[i] = tbl_JOIN_INNER_TD_3291_output.createSubTable(i);
    }
    tbl_SerializeFromObject_TD_3872_input_stringRowIDSubstitute.allocateDevBuffer(context_h, 33);
    tbl_JOIN_INNER_TD_4180_output.allocateDevBuffer(context_h, 32);
    Table tbl_JOIN_INNER_TD_4180_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_JOIN_INNER_TD_4180_output_partition_array[i] = tbl_JOIN_INNER_TD_4180_output.createSubTable(i);
    }
    tbl_Filter_TD_4769_output.allocateDevBuffer(context_h, 33);
    tbl_SerializeFromObject_TD_5871_input_stringRowIDSubstitute.allocateDevBuffer(context_h, 33);
    tbl_Filter_TD_531_output.allocateDevBuffer(context_h, 33);
    // ****************************** Config **************************** // 
    cfgCmd cfg_JOIN_INNER_TD_2966_cmds;
    cfg_JOIN_INNER_TD_2966_cmds.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_2966_gqe_join (cfg_JOIN_INNER_TD_2966_cmds.cmd);
    cfg_JOIN_INNER_TD_2966_cmds.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_2966_cmds_part;
    cfg_JOIN_INNER_TD_2966_cmds_part.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_2966_gqe_join_part (cfg_JOIN_INNER_TD_2966_cmds_part.cmd);
    cfg_JOIN_INNER_TD_2966_cmds_part.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_3291_cmds;
    cfg_JOIN_INNER_TD_3291_cmds.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_3291_gqe_join (cfg_JOIN_INNER_TD_3291_cmds.cmd);
    cfg_JOIN_INNER_TD_3291_cmds.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_3291_cmds_part;
    cfg_JOIN_INNER_TD_3291_cmds_part.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_3291_gqe_join_part (cfg_JOIN_INNER_TD_3291_cmds_part.cmd);
    cfg_JOIN_INNER_TD_3291_cmds_part.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_4180_cmds;
    cfg_JOIN_INNER_TD_4180_cmds.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_4180_gqe_join (cfg_JOIN_INNER_TD_4180_cmds.cmd);
    cfg_JOIN_INNER_TD_4180_cmds.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_4180_cmds_part;
    cfg_JOIN_INNER_TD_4180_cmds_part.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_4180_gqe_join_part (cfg_JOIN_INNER_TD_4180_cmds_part.cmd);
    cfg_JOIN_INNER_TD_4180_cmds_part.allocateDevBuffer(context_h, 32);
    // *************************** Kernel Setup ************************* // 
    bufferTmp buftmp_h(context_h, 0); 
    buftmp_h.initBuffer(q_h); 
    std::cout << std::endl; 
    krnlEngine krnl_JOIN_INNER_TD_2966_part_left;
    krnl_JOIN_INNER_TD_2966_part_left = krnlEngine(program_h, q_h, context_h, "gqePart");
    krnl_JOIN_INNER_TD_2966_part_left.setup_hp(512, 0, power_of_hpTimes_join, tbl_SerializeFromObject_TD_3872_input_stringRowIDSubstitute, tbl_SerializeFromObject_TD_3872_input_stringRowIDSubstitute_partition, cfg_JOIN_INNER_TD_2966_cmds_part);
    krnlEngine krnl_JOIN_INNER_TD_2966[hpTimes_join];
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_INNER_TD_2966[i] = krnlEngine(program_h, q_h, "gqeJoin");
    }
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_INNER_TD_2966[i].setup(tbl_SerializeFromObject_TD_3872_input_stringRowIDSubstitute_partition_array[i], tbl_JOIN_INNER_TD_3291_output_partition_array[i], tbl_JOIN_INNER_TD_2966_output_partition_array[i], cfg_JOIN_INNER_TD_2966_cmds, buftmp_h);
    }
    krnlEngine krnl_JOIN_INNER_TD_2966_build;
    krnl_JOIN_INNER_TD_2966_build = krnlEngine(program_h, q_h, context_h, "gqeKernel", 0);
    krnl_JOIN_INNER_TD_2966_build.setup(tbl_SerializeFromObject_TD_3872_input_stringRowIDSubstitute, tbl_JOIN_INNER_TD_3291_output, tbl_JOIN_INNER_TD_2966_output, cfg_JOIN_INNER_TD_2966_cmds_build, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_2966_probe;
    krnl_JOIN_INNER_TD_2966_probe = krnlEngine(program_h, q_h, context_h, "gqeKernel", 1);
    krnl_JOIN_INNER_TD_2966_probe.setup(tbl_JOIN_INNER_TD_3291_output, tbl_SerializeFromObject_TD_3872_input_stringRowIDSubstitute, tbl_JOIN_INNER_TD_2966_output, cfg_JOIN_INNER_TD_2966_cmds_probe, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_3291_part_left;
    krnl_JOIN_INNER_TD_3291_part_left = krnlEngine(program_h, q_h, context_h, "gqePart");
    krnl_JOIN_INNER_TD_3291_part_left.setup_hp(512, 0, power_of_hpTimes_join, tbl_Filter_TD_4769_output, tbl_Filter_TD_4769_output_partition, cfg_JOIN_INNER_TD_3291_cmds_part);
    krnlEngine krnl_JOIN_INNER_TD_3291[hpTimes_join];
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_INNER_TD_3291[i] = krnlEngine(program_h, q_h, "gqeJoin");
    }
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_INNER_TD_3291[i].setup(tbl_Filter_TD_4769_output_partition_array[i], tbl_JOIN_INNER_TD_4180_output_partition_array[i], tbl_JOIN_INNER_TD_3291_output_partition_array[i], cfg_JOIN_INNER_TD_3291_cmds, buftmp_h);
    }
    krnlEngine krnl_JOIN_INNER_TD_3291_build;
    krnl_JOIN_INNER_TD_3291_build = krnlEngine(program_h, q_h, context_h, "gqeKernel", 0);
    krnl_JOIN_INNER_TD_3291_build.setup(tbl_Filter_TD_4769_output, tbl_JOIN_INNER_TD_4180_output, tbl_JOIN_INNER_TD_3291_output, cfg_JOIN_INNER_TD_3291_cmds_build, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_3291_probe;
    krnl_JOIN_INNER_TD_3291_probe = krnlEngine(program_h, q_h, context_h, "gqeKernel", 1);
    krnl_JOIN_INNER_TD_3291_probe.setup(tbl_JOIN_INNER_TD_4180_output, tbl_Filter_TD_4769_output, tbl_JOIN_INNER_TD_3291_output, cfg_JOIN_INNER_TD_3291_cmds_probe, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_4180_part_left;
    krnl_JOIN_INNER_TD_4180_part_left = krnlEngine(program_h, q_h, context_h, "gqePart");
    krnl_JOIN_INNER_TD_4180_part_left.setup_hp(512, 0, power_of_hpTimes_join, tbl_Filter_TD_531_output, tbl_Filter_TD_531_output_partition, cfg_JOIN_INNER_TD_4180_cmds_part);
    krnlEngine krnl_JOIN_INNER_TD_4180_part_right;
    krnl_JOIN_INNER_TD_4180_part_right = krnlEngine(program_h, q_h, context_h, "gqePart");
    krnl_JOIN_INNER_TD_4180_part_right.setup_hp(512, 1, power_of_hpTimes_join, tbl_SerializeFromObject_TD_5871_input_stringRowIDSubstitute, tbl_SerializeFromObject_TD_5871_input_stringRowIDSubstitute_partition, cfg_JOIN_INNER_TD_4180_cmds_part);
    krnlEngine krnl_JOIN_INNER_TD_4180[hpTimes_join];
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_INNER_TD_4180[i] = krnlEngine(program_h, q_h, "gqeJoin");
    }
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_INNER_TD_4180[i].setup(tbl_Filter_TD_531_output_partition_array[i], tbl_SerializeFromObject_TD_5871_input_stringRowIDSubstitute_partition_array[i], tbl_JOIN_INNER_TD_4180_output_partition_array[i], cfg_JOIN_INNER_TD_4180_cmds, buftmp_h);
    }
    krnlEngine krnl_JOIN_INNER_TD_4180_build;
    krnl_JOIN_INNER_TD_4180_build = krnlEngine(program_h, q_h, context_h, "gqeKernel", 0);
    krnl_JOIN_INNER_TD_4180_build.setup(tbl_Filter_TD_531_output, tbl_SerializeFromObject_TD_5871_input_stringRowIDSubstitute, tbl_JOIN_INNER_TD_4180_output, cfg_JOIN_INNER_TD_4180_cmds_build, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_4180_probe;
    krnl_JOIN_INNER_TD_4180_probe = krnlEngine(program_h, q_h, context_h, "gqeKernel", 1);
    krnl_JOIN_INNER_TD_4180_probe.setup(tbl_SerializeFromObject_TD_5871_input_stringRowIDSubstitute, tbl_Filter_TD_531_output, tbl_JOIN_INNER_TD_4180_output, cfg_JOIN_INNER_TD_4180_cmds_probe, buftmp_h);
    // ************************** Transfer Engine *********************** // 
    transEngine trans_JOIN_INNER_TD_2966;
    trans_JOIN_INNER_TD_2966.setq(q_h,0);
    trans_JOIN_INNER_TD_2966.add(&(cfg_JOIN_INNER_TD_2966_cmds_part));
    trans_JOIN_INNER_TD_2966.add(&(cfg_JOIN_INNER_TD_2966_cmds));
    trans_JOIN_INNER_TD_2966.add(&(tbl_SerializeFromObject_TD_3872_input_stringRowIDSubstitute));
    transEngine trans_JOIN_INNER_TD_2966_out;
    trans_JOIN_INNER_TD_2966_out.setq(q_h,0);
    q_h.finish();
    transEngine trans_JOIN_INNER_TD_3291;
    trans_JOIN_INNER_TD_3291.setq(q_h,0);
    trans_JOIN_INNER_TD_3291.add(&(cfg_JOIN_INNER_TD_3291_cmds_part));
    trans_JOIN_INNER_TD_3291.add(&(cfg_JOIN_INNER_TD_3291_cmds));
    q_h.finish();
    transEngine trans_JOIN_INNER_TD_4180;
    trans_JOIN_INNER_TD_4180.setq(q_h,0);
    trans_JOIN_INNER_TD_4180.add(&(cfg_JOIN_INNER_TD_4180_cmds_part));
    trans_JOIN_INNER_TD_4180.add(&(cfg_JOIN_INNER_TD_4180_cmds));
    trans_JOIN_INNER_TD_4180.add(&(tbl_SerializeFromObject_TD_5871_input_stringRowIDSubstitute));
    q_h.finish();
    // ****************************** Events **************************** // 
    std::vector<cl::Event> events_h2d_wr_JOIN_INNER_TD_2966;
    std::vector<cl::Event> events_d2h_rd_JOIN_INNER_TD_2966;
    std::vector<cl::Event> events_JOIN_INNER_TD_2966[2];
    events_h2d_wr_JOIN_INNER_TD_2966.resize(1);
    events_d2h_rd_JOIN_INNER_TD_2966.resize(1);
    events_JOIN_INNER_TD_2966[0].resize(1);
    events_JOIN_INNER_TD_2966[1].resize(hpTimes_join);
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_2966;
    std::vector<cl::Event> prev_events_grp_JOIN_INNER_TD_2966;
    std::vector<cl::Event> events_h2d_wr_JOIN_INNER_TD_3291;
    std::vector<cl::Event> events_d2h_rd_JOIN_INNER_TD_3291;
    std::vector<cl::Event> events_JOIN_INNER_TD_3291[2];
    events_h2d_wr_JOIN_INNER_TD_3291.resize(1);
    events_d2h_rd_JOIN_INNER_TD_3291.resize(1);
    events_JOIN_INNER_TD_3291[0].resize(1);
    events_JOIN_INNER_TD_3291[1].resize(hpTimes_join);
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_3291;
    std::vector<cl::Event> prev_events_grp_JOIN_INNER_TD_3291;
    std::vector<cl::Event> events_h2d_wr_JOIN_INNER_TD_4180;
    std::vector<cl::Event> events_d2h_rd_JOIN_INNER_TD_4180;
    std::vector<cl::Event> events_JOIN_INNER_TD_4180[2];
    events_h2d_wr_JOIN_INNER_TD_4180.resize(1);
    events_d2h_rd_JOIN_INNER_TD_4180.resize(1);
    events_JOIN_INNER_TD_4180[0].resize(2);
    events_JOIN_INNER_TD_4180[1].resize(hpTimes_join);
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_4180;
    std::vector<cl::Event> prev_events_grp_JOIN_INNER_TD_4180;
    // **************************** Operations ************************** // 
    struct timeval tv_r_s, tv_r_e; 
    gettimeofday(&tv_r_s, 0); 

    struct timeval tv_r_Filter_5_59_s, tv_r_Filter_5_59_e;
    gettimeofday(&tv_r_Filter_5_59_s, 0);
    SW_Filter_TD_531(tbl_SerializeFromObject_TD_6198_input, tbl_Filter_TD_531_output);
    gettimeofday(&tv_r_Filter_5_59_e, 0);

    struct timeval tv_r_Filter_4_634_s, tv_r_Filter_4_634_e;
    gettimeofday(&tv_r_Filter_4_634_s, 0);
    SW_Filter_TD_4769(tbl_SerializeFromObject_TD_5599_input, tbl_Filter_TD_4769_output);
    gettimeofday(&tv_r_Filter_4_634_e, 0);

    struct timeval tv_r_JOIN_INNER_4_70_s, tv_r_JOIN_INNER_4_70_e;
    gettimeofday(&tv_r_JOIN_INNER_4_70_s, 0);
    trans_JOIN_INNER_TD_4180.add(&(tbl_Filter_TD_531_output));
    trans_JOIN_INNER_TD_4180.host2dev(0, &(prev_events_grp_JOIN_INNER_TD_4180), &(events_h2d_wr_JOIN_INNER_TD_4180[0]));
    events_grp_JOIN_INNER_TD_4180.push_back(events_h2d_wr_JOIN_INNER_TD_4180[0]);
    krnl_JOIN_INNER_TD_4180_part_left.run(0, &(events_grp_JOIN_INNER_TD_4180), &(events_JOIN_INNER_TD_4180[0][0]));
    krnl_JOIN_INNER_TD_4180_part_right.run(0, &(events_grp_JOIN_INNER_TD_4180), &(events_JOIN_INNER_TD_4180[0][1]));
    for (int i(0); i < hpTimes_join; ++i) {
        krnl_JOIN_INNER_TD_4180[i].run(0, &(events_JOIN_INNER_TD_4180[0]), &(events_JOIN_INNER_TD_4180[1][i]));
    }
    gettimeofday(&tv_r_JOIN_INNER_4_70_e, 0);

    struct timeval tv_r_JOIN_INNER_3_321_s, tv_r_JOIN_INNER_3_321_e;
    gettimeofday(&tv_r_JOIN_INNER_3_321_s, 0);
    prev_events_grp_JOIN_INNER_TD_3291.push_back(events_h2d_wr_JOIN_INNER_TD_4180[0]);
    trans_JOIN_INNER_TD_3291.add(&(tbl_Filter_TD_4769_output));
    trans_JOIN_INNER_TD_3291.host2dev(0, &(prev_events_grp_JOIN_INNER_TD_3291), &(events_h2d_wr_JOIN_INNER_TD_3291[0]));
    events_grp_JOIN_INNER_TD_3291.push_back(events_h2d_wr_JOIN_INNER_TD_3291[0]);
    events_grp_JOIN_INNER_TD_3291.insert(std::end(events_grp_JOIN_INNER_TD_3291), std::begin(events_JOIN_INNER_TD_4180[0]), std::end(events_JOIN_INNER_TD_4180[0]));
    events_grp_JOIN_INNER_TD_3291.insert(std::end(events_grp_JOIN_INNER_TD_3291), std::begin(events_JOIN_INNER_TD_4180[1]), std::end(events_JOIN_INNER_TD_4180[1]));
    krnl_JOIN_INNER_TD_3291_part_left.run(0, &(events_grp_JOIN_INNER_TD_3291), &(events_JOIN_INNER_TD_3291[0][0]));
    for (int i(0); i < hpTimes_join; ++i) {
        krnl_JOIN_INNER_TD_3291[i].run(0, &(events_JOIN_INNER_TD_3291[0]), &(events_JOIN_INNER_TD_3291[1][i]));
    }
    gettimeofday(&tv_r_JOIN_INNER_3_321_e, 0);

    struct timeval tv_r_JOIN_INNER_2_768_s, tv_r_JOIN_INNER_2_768_e;
    gettimeofday(&tv_r_JOIN_INNER_2_768_s, 0);
    prev_events_grp_JOIN_INNER_TD_2966.push_back(events_h2d_wr_JOIN_INNER_TD_3291[0]);
    trans_JOIN_INNER_TD_2966.host2dev(0, &(prev_events_grp_JOIN_INNER_TD_2966), &(events_h2d_wr_JOIN_INNER_TD_2966[0]));
    events_grp_JOIN_INNER_TD_2966.push_back(events_h2d_wr_JOIN_INNER_TD_2966[0]);
    events_grp_JOIN_INNER_TD_2966.insert(std::end(events_grp_JOIN_INNER_TD_2966), std::begin(events_JOIN_INNER_TD_3291[0]), std::end(events_JOIN_INNER_TD_3291[0]));
    events_grp_JOIN_INNER_TD_2966.insert(std::end(events_grp_JOIN_INNER_TD_2966), std::begin(events_JOIN_INNER_TD_3291[1]), std::end(events_JOIN_INNER_TD_3291[1]));
    krnl_JOIN_INNER_TD_2966_part_left.run(0, &(events_grp_JOIN_INNER_TD_2966), &(events_JOIN_INNER_TD_2966[0][0]));
    for (int i(0); i < hpTimes_join; ++i) {
        krnl_JOIN_INNER_TD_2966[i].run(0, &(events_JOIN_INNER_TD_2966[0]), &(events_JOIN_INNER_TD_2966[1][i]));
    }
    for (int i(0); i < hpTimes_join; ++i) {
        trans_JOIN_INNER_TD_2966_out.add(&(tbl_JOIN_INNER_TD_2966_output_partition_array[i]));
    }
    trans_JOIN_INNER_TD_2966_out.dev2host(0, &(events_JOIN_INNER_TD_2966[1]), &(events_d2h_rd_JOIN_INNER_TD_2966[0]));
    q_h.flush();
    q_h.finish();
    gettimeofday(&tv_r_JOIN_INNER_2_768_e, 0);

    struct timeval tv_r_Aggregate_1_606_s, tv_r_Aggregate_1_606_e;
    gettimeofday(&tv_r_Aggregate_1_606_s, 0);
    SW_Aggregate_TD_1118(tbl_JOIN_INNER_TD_2966_output_partition_array, tbl_SerializeFromObject_TD_5871_input, tbl_SerializeFromObject_TD_6198_input, tbl_SerializeFromObject_TD_5599_input, tbl_SerializeFromObject_TD_3872_input, tbl_Aggregate_TD_1118_output, hpTimes_join);
    gettimeofday(&tv_r_Aggregate_1_606_e, 0);

    struct timeval tv_r_Sort_0_572_s, tv_r_Sort_0_572_e;
    gettimeofday(&tv_r_Sort_0_572_s, 0);
    SW_Sort_TD_0161(tbl_Aggregate_TD_1118_output, tbl_Sort_TD_0161_output);
    gettimeofday(&tv_r_Sort_0_572_e, 0);

std::cout << "CPU TIME: " << (tvdiff(&tv_r_Filter_5_59_s, &tv_r_Filter_5_59_e) + 
tvdiff(&tv_r_Filter_4_634_s, &tv_r_Filter_4_634_e) + 
tvdiff(&tv_r_Aggregate_1_606_s, &tv_r_Aggregate_1_606_e) + 
tvdiff(&tv_r_Sort_0_572_s, &tv_r_Sort_0_572_e) + 
1) / 1000.0 << std::endl;

std::cout << "FPGA TIME: " << (tvdiff(&tv_r_JOIN_INNER_4_70_s, &tv_r_JOIN_INNER_4_70_e) + 
tvdiff(&tv_r_JOIN_INNER_3_321_s, &tv_r_JOIN_INNER_3_321_e) + 
tvdiff(&tv_r_JOIN_INNER_2_768_s, &tv_r_JOIN_INNER_2_768_e) + 
1) / 1000.0 << std::endl;

    gettimeofday(&tv_r_e, 0); 
    // **************************** Print Execution Time ************************** // 
    std::cout << std::endl; 
    std::cout << "Filter_5: " << tvdiff(&tv_r_Filter_5_59_s, &tv_r_Filter_5_59_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(((o_orderdate#211 >= 19940801) AND (o_orderdate#211 < 19941101))) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_6198_input.getNumRow() << " -> tbl_SerializeFromObject_TD_6198_input" 
     << std::endl << "      #COL: 3: " << "ListBuffer(o_orderkey#207, o_custkey#208, o_orderdate#211)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_531_output.getNumRow() << " -> tbl_Filter_TD_531_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(o_orderkey#207, o_custkey#208)" 
     << std::endl; 

    std::cout << "Filter_4: " << tvdiff(&tv_r_Filter_4_634_s, &tv_r_Filter_4_634_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer((l_returnflag#85 = 82)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_5599_input.getNumRow() << " -> tbl_SerializeFromObject_TD_5599_input" 
     << std::endl << "      #COL: 4: " << "ListBuffer(l_orderkey#77, l_extendedprice#82, l_discount#83, l_returnflag#85)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_4769_output.getNumRow() << " -> tbl_Filter_TD_4769_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(l_orderkey#77, l_extendedprice#82, l_discount#83)" 
     << std::endl; 

    std::cout << "JOIN_INNER_4: " << tvdiff(&tv_r_JOIN_INNER_4_70_s, &tv_r_JOIN_INNER_4_70_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer((c_custkey#22 = o_custkey#208)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_5871_input_stringRowIDSubstitute.getNumRow() << " -> tbl_SerializeFromObject_TD_5871_input_stringRowIDSubstitute" 
     << std::endl << "      #COL: 4: " << "ListBuffer(c_custkey#22, c_name#23, c_nationkey#25, c_acctbal#27)" 
     << std::endl << "      #ROW: " << tbl_Filter_TD_531_output.getNumRow() << " -> tbl_Filter_TD_531_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(o_orderkey#207, o_custkey#208)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_4180_output.getNumRow() << " -> tbl_JOIN_INNER_TD_4180_output" 
     << std::endl << "      #COL: 5: " << "ListBuffer(c_custkey#22, c_name#23, c_nationkey#25, c_acctbal#27, o_orderkey#207)" 
     << std::endl; 

    std::cout << "JOIN_INNER_3: " << tvdiff(&tv_r_JOIN_INNER_3_321_s, &tv_r_JOIN_INNER_3_321_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer((l_orderkey#77 = o_orderkey#207)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_4180_output.getNumRow() << " -> tbl_JOIN_INNER_TD_4180_output" 
     << std::endl << "      #COL: 5: " << "ListBuffer(c_custkey#22, c_name#23, c_nationkey#25, c_acctbal#27, o_orderkey#207)" 
     << std::endl << "      #ROW: " << tbl_Filter_TD_4769_output.getNumRow() << " -> tbl_Filter_TD_4769_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(l_orderkey#77, l_extendedprice#82, l_discount#83)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_3291_output.getNumRow() << " -> tbl_JOIN_INNER_TD_3291_output" 
     << std::endl << "      #COL: 6: " << "ListBuffer(c_custkey#22, c_name#23, c_nationkey#25, c_acctbal#27, l_extendedprice#82, l_discount#83)" 
     << std::endl; 

    std::cout << "JOIN_INNER_2: " << tvdiff(&tv_r_JOIN_INNER_2_768_s, &tv_r_JOIN_INNER_2_768_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer((c_nationkey#25 = n_nationkey#144)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_3291_output.getNumRow() << " -> tbl_JOIN_INNER_TD_3291_output" 
     << std::endl << "      #COL: 6: " << "ListBuffer(c_custkey#22, c_name#23, c_nationkey#25, c_acctbal#27, l_extendedprice#82, l_discount#83)" 
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_3872_input_stringRowIDSubstitute.getNumRow() << " -> tbl_SerializeFromObject_TD_3872_input_stringRowIDSubstitute" 
     << std::endl << "      #COL: 2: " << "ListBuffer(n_nationkey#144, n_name#145)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_2966_output.getNumRow() << " -> tbl_JOIN_INNER_TD_2966_output" 
     << std::endl << "      #COL: 6: " << "ListBuffer(c_custkey#22, c_name#23, c_acctbal#27, l_extendedprice#82, l_discount#83, n_name#145)" 
     << std::endl; 

    std::cout << "Aggregate_1: " << tvdiff(&tv_r_Aggregate_1_606_s, &tv_r_Aggregate_1_606_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(c_custkey#22, c_name#23, c_acctbal#27, n_name#145, sum((l_extendedprice#82 * (100 - l_discount#83))) AS revenue#1132L) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_2966_output.getNumRow() << " -> tbl_JOIN_INNER_TD_2966_output" 
     << std::endl << "      #COL: 6: " << "ListBuffer(c_custkey#22, c_name#23, c_acctbal#27, l_extendedprice#82, l_discount#83, n_name#145)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_1118_output.getNumRow() << " -> tbl_Aggregate_TD_1118_output" 
     << std::endl << "      #COL: 5: " << "ListBuffer(c_custkey#22, c_name#23, revenue#1132L, c_acctbal#27, n_name#145)" 
     << std::endl; 

    std::cout << "Sort_0: " << tvdiff(&tv_r_Sort_0_572_s, &tv_r_Sort_0_572_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(revenue#1132L DESC NULLS LAST) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_1118_output.getNumRow() << " -> tbl_Aggregate_TD_1118_output" 
     << std::endl << "      #COL: 5: " << "ListBuffer(c_custkey#22, c_name#23, revenue#1132L, c_acctbal#27, n_name#145)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Sort_TD_0161_output.getNumRow() << " -> tbl_Sort_TD_0161_output" 
     << std::endl << "      #COL: 5: " << "ListBuffer(c_custkey#22, c_name#23, revenue#1132L, c_acctbal#27, n_name#145)" 
     << std::endl; 

    std::cout << std::endl << " Total execution time: " << tvdiff(&tv_r_s, &tv_r_e) / 1000 << " ms"; 

    std::cout << std::endl << " Spark elapsed time: " << 22.774048 * 1000 << "ms" << std::endl; 
    return 0; 
}
