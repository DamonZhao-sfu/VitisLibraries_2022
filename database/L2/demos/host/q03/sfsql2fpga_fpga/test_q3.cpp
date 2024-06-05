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

#include "cfgFunc_q3.hpp" 
#include "q3.hpp" 

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
    std::cout << "NOTE:running query #3\n."; 
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
    Table tbl_Sort_TD_0224_output("tbl_Sort_TD_0224_output", 183000000, 4, "");
    tbl_Sort_TD_0224_output.allocateHost();
    Table tbl_Aggregate_TD_1746_output_preprocess("tbl_Aggregate_TD_1746_output_preprocess", 24000, 16, "");
    tbl_Aggregate_TD_1746_output_preprocess.allocateHost(1.2, hpTimes_aggr);
    Table tbl_Aggregate_TD_1746_output("tbl_Aggregate_TD_1746_output", 24000, 4, "");
    Table tbl_JOIN_INNER_TD_2156_output_preprocess("tbl_JOIN_INNER_TD_2156_output_preprocess", 30000, 5, "");
    tbl_JOIN_INNER_TD_2156_output_preprocess.selectOverlayVersion(0);
    tbl_JOIN_INNER_TD_2156_output_preprocess.allocateHost();
    Table tbl_JOIN_INNER_TD_2156_output("tbl_JOIN_INNER_TD_2156_output", 30000, 5, "");
    tbl_JOIN_INNER_TD_2156_output_preprocess.selectOverlayVersion(0);
    tbl_JOIN_INNER_TD_2156_output.allocateHost();
    Table tbl_Filter_TD_3418_output("tbl_Filter_TD_3418_output", 183000000, 3, "");
    tbl_Filter_TD_3418_output.selectOverlayVersion(0);
    tbl_Filter_TD_3418_output.allocateHost();
    Table tbl_Filter_TD_3418_output_partition("tbl_Filter_TD_3418_output_partition", 183000000, 3, "");
    tbl_Filter_TD_3418_output_partition.allocateHost(1.2, hpTimes_join);
    tbl_Filter_TD_3418_output_partition.allocateDevBuffer(context_h, 32);
    Table tbl_Filter_TD_3418_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_Filter_TD_3418_output_partition_array[i] = tbl_Filter_TD_3418_output_partition.createSubTable(i);
    }
    Table tbl_JOIN_INNER_TD_3451_output("tbl_JOIN_INNER_TD_3451_output", 150000, 3, "");
    tbl_JOIN_INNER_TD_3451_output.allocateHost(1.2, hpTimes_join);
    Table tbl_SerializeFromObject_TD_4640_input;
    tbl_SerializeFromObject_TD_4640_input = Table("lineitem", lineitem_n, 4, in_dir);
    tbl_SerializeFromObject_TD_4640_input.addCol("l_orderkey", 4);
    tbl_SerializeFromObject_TD_4640_input.addCol("l_extendedprice", 4);
    tbl_SerializeFromObject_TD_4640_input.addCol("l_discount", 4);
    tbl_SerializeFromObject_TD_4640_input.addCol("l_shipdate", 4);
    tbl_SerializeFromObject_TD_4640_input.allocateHost();
    tbl_SerializeFromObject_TD_4640_input.loadHost();
    Table tbl_Filter_TD_4197_output("tbl_Filter_TD_4197_output", 183000000, 1, "");
    tbl_Filter_TD_4197_output.selectOverlayVersion(0);
    tbl_Filter_TD_4197_output.allocateHost();
    Table tbl_Filter_TD_4197_output_partition("tbl_Filter_TD_4197_output_partition", 183000000, 1, "");
    tbl_Filter_TD_4197_output_partition.allocateHost(1.2, hpTimes_join);
    tbl_Filter_TD_4197_output_partition.allocateDevBuffer(context_h, 32);
    Table tbl_Filter_TD_4197_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_Filter_TD_4197_output_partition_array[i] = tbl_Filter_TD_4197_output_partition.createSubTable(i);
    }
    Table tbl_Filter_TD_4427_output("tbl_Filter_TD_4427_output", 183000000, 4, "");
    tbl_Filter_TD_4427_output.selectOverlayVersion(0);
    tbl_Filter_TD_4427_output.allocateHost();
    Table tbl_Filter_TD_4427_output_partition("tbl_Filter_TD_4427_output_partition", 183000000, 4, "");
    tbl_Filter_TD_4427_output_partition.allocateHost(1.2, hpTimes_join);
    tbl_Filter_TD_4427_output_partition.allocateDevBuffer(context_h, 32);
    Table tbl_Filter_TD_4427_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_Filter_TD_4427_output_partition_array[i] = tbl_Filter_TD_4427_output_partition.createSubTable(i);
    }
    Table tbl_SerializeFromObject_TD_5595_input;
    tbl_SerializeFromObject_TD_5595_input = Table("customer", customer_n, 2, in_dir);
    tbl_SerializeFromObject_TD_5595_input.addCol("c_custkey", 4);
    tbl_SerializeFromObject_TD_5595_input.addCol("c_mktsegment", TPCH_READ_MAXAGG_LEN+1);
    tbl_SerializeFromObject_TD_5595_input.allocateHost();
    tbl_SerializeFromObject_TD_5595_input.loadHost();
    Table tbl_SerializeFromObject_TD_5252_input;
    tbl_SerializeFromObject_TD_5252_input = Table("order", order_n, 4, in_dir);
    tbl_SerializeFromObject_TD_5252_input.addCol("o_orderkey", 4);
    tbl_SerializeFromObject_TD_5252_input.addCol("o_custkey", 4);
    tbl_SerializeFromObject_TD_5252_input.addCol("o_orderdate", 4);
    tbl_SerializeFromObject_TD_5252_input.addCol("o_shippriority", 4);
    tbl_SerializeFromObject_TD_5252_input.allocateHost();
    tbl_SerializeFromObject_TD_5252_input.loadHost();
    // ********************** Allocate Device Buffer ******************** // 
    tbl_JOIN_INNER_TD_2156_output.allocateDevBuffer(context_a, 32);
    tbl_Aggregate_TD_1746_output_preprocess.allocateDevBuffer(context_a, 33);
    Table tbl_Aggregate_TD_1746_output_partition_array[hpTimes_aggr];
    for (int i(0); i < hpTimes_aggr; ++i) {
        tbl_Aggregate_TD_1746_output_partition_array[i] = tbl_Aggregate_TD_1746_output_preprocess.createSubTable(i);
    }
    tbl_JOIN_INNER_TD_2156_output_preprocess.allocateDevBuffer(context_h, 32);
    tbl_Filter_TD_3418_output.allocateDevBuffer(context_h, 33);
    tbl_JOIN_INNER_TD_3451_output.allocateDevBuffer(context_h, 32);
    Table tbl_JOIN_INNER_TD_3451_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_JOIN_INNER_TD_3451_output_partition_array[i] = tbl_JOIN_INNER_TD_3451_output.createSubTable(i);
    }
    tbl_Filter_TD_4197_output.allocateDevBuffer(context_h, 33);
    tbl_Filter_TD_4427_output.allocateDevBuffer(context_h, 33);
    // ****************************** Config **************************** // 
    AggrCfgCmd cfg_Aggregate_TD_1746_cmds;
    cfg_Aggregate_TD_1746_cmds.allocateHost();
    get_cfg_dat_Aggregate_TD_1746_gqe_aggr(cfg_Aggregate_TD_1746_cmds.cmd);
    cfg_Aggregate_TD_1746_cmds.allocateDevBuffer(context_a, 32);
    AggrCfgCmd cfg_Aggregate_TD_1746_cmds_out;
    cfg_Aggregate_TD_1746_cmds_out.allocateHost();
    cfg_Aggregate_TD_1746_cmds_out.allocateDevBuffer(context_a, 33);
    cfgCmd cfg_JOIN_INNER_TD_2156_cmds;
    cfg_JOIN_INNER_TD_2156_cmds.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_2156_gqe_join (cfg_JOIN_INNER_TD_2156_cmds.cmd);
    cfg_JOIN_INNER_TD_2156_cmds.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_2156_cmds_part;
    cfg_JOIN_INNER_TD_2156_cmds_part.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_2156_gqe_join_part (cfg_JOIN_INNER_TD_2156_cmds_part.cmd);
    cfg_JOIN_INNER_TD_2156_cmds_part.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_3451_cmds;
    cfg_JOIN_INNER_TD_3451_cmds.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_3451_gqe_join (cfg_JOIN_INNER_TD_3451_cmds.cmd);
    cfg_JOIN_INNER_TD_3451_cmds.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_3451_cmds_part;
    cfg_JOIN_INNER_TD_3451_cmds_part.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_3451_gqe_join_part (cfg_JOIN_INNER_TD_3451_cmds_part.cmd);
    cfg_JOIN_INNER_TD_3451_cmds_part.allocateDevBuffer(context_h, 32);
    // *************************** Kernel Setup ************************* // 
    bufferTmp buftmp_h(context_h, 0); 
    buftmp_h.initBuffer(q_h); 
    AggrBufferTmp buftmp_a(context_a, 0); 
    buftmp_a.BufferInitial(q_a); 
    std::cout << std::endl; 
    AggrKrnlEngine krnl_Aggregate_TD_1746_part;
    krnl_Aggregate_TD_1746_part = AggrKrnlEngine(program_a, q_a, "gqePart");
    krnl_Aggregate_TD_1746_part.setup_hp(512, 0, power_of_hpTimes_aggr, tbl_JOIN_INNER_TD_2156_output, tbl_JOIN_INNER_TD_2156_output_partition, cfg_Aggregate_TD_1746_cmds_part);
    AggrKrnlEngine krnl_Aggregate_TD_1746[hpTimes_aggr];
    for (int i = 0; i < hpTimes_aggr; i++) {
        krnl_Aggregate_TD_1746[i] = AggrKrnlEngine(program_a, q_a, "gqeAggr");
    }
    for (int i = 0; i < hpTimes_aggr; i++) {
        krnl_Aggregate_TD_1746[i].setup(tbl_JOIN_INNER_TD_2156_output_partition_array[i], tbl_Aggregate_TD_1746_output_partition_array[i], cfg_Aggregate_TD_1746_cmds, cfg_Aggregate_TD_1746_cmds_out, buftmp_a);
    }
    krnlEngine krnl_JOIN_INNER_TD_2156_part_right;
    krnl_JOIN_INNER_TD_2156_part_right = krnlEngine(program_h, q_h, context_h, "gqePart");
    krnl_JOIN_INNER_TD_2156_part_right.setup_hp(512, 1, power_of_hpTimes_join, tbl_Filter_TD_3418_output, tbl_Filter_TD_3418_output_partition, cfg_JOIN_INNER_TD_2156_cmds_part);
    krnlEngine krnl_JOIN_INNER_TD_2156[hpTimes_join];
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_INNER_TD_2156[i] = krnlEngine(program_h, q_h, "gqeJoin");
    }
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_INNER_TD_2156[i].setup(tbl_JOIN_INNER_TD_3451_output_partition_array[i], tbl_Filter_TD_3418_output_partition_array[i], tbl_JOIN_INNER_TD_2156_output_preprocess_partition_array[i], cfg_JOIN_INNER_TD_2156_cmds, buftmp_h);
    }
    krnlEngine krnl_JOIN_INNER_TD_2156_build;
    krnl_JOIN_INNER_TD_2156_build = krnlEngine(program_h, q_h, context_h, "gqeKernel", 0);
    krnl_JOIN_INNER_TD_2156_build.setup(tbl_JOIN_INNER_TD_3451_output, tbl_Filter_TD_3418_output, tbl_JOIN_INNER_TD_2156_output_preprocess, cfg_JOIN_INNER_TD_2156_cmds_build, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_2156_probe;
    krnl_JOIN_INNER_TD_2156_probe = krnlEngine(program_h, q_h, context_h, "gqeKernel", 1);
    krnl_JOIN_INNER_TD_2156_probe.setup(tbl_Filter_TD_3418_output, tbl_JOIN_INNER_TD_3451_output, tbl_JOIN_INNER_TD_2156_output_preprocess, cfg_JOIN_INNER_TD_2156_cmds_probe, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_3451_part_left;
    krnl_JOIN_INNER_TD_3451_part_left = krnlEngine(program_h, q_h, context_h, "gqePart");
    krnl_JOIN_INNER_TD_3451_part_left.setup_hp(512, 0, power_of_hpTimes_join, tbl_Filter_TD_4197_output, tbl_Filter_TD_4197_output_partition, cfg_JOIN_INNER_TD_3451_cmds_part);
    krnlEngine krnl_JOIN_INNER_TD_3451_part_right;
    krnl_JOIN_INNER_TD_3451_part_right = krnlEngine(program_h, q_h, context_h, "gqePart");
    krnl_JOIN_INNER_TD_3451_part_right.setup_hp(512, 1, power_of_hpTimes_join, tbl_Filter_TD_4427_output, tbl_Filter_TD_4427_output_partition, cfg_JOIN_INNER_TD_3451_cmds_part);
    krnlEngine krnl_JOIN_INNER_TD_3451[hpTimes_join];
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_INNER_TD_3451[i] = krnlEngine(program_h, q_h, "gqeJoin");
    }
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_INNER_TD_3451[i].setup(tbl_Filter_TD_4197_output_partition_array[i], tbl_Filter_TD_4427_output_partition_array[i], tbl_JOIN_INNER_TD_3451_output_partition_array[i], cfg_JOIN_INNER_TD_3451_cmds, buftmp_h);
    }
    krnlEngine krnl_JOIN_INNER_TD_3451_build;
    krnl_JOIN_INNER_TD_3451_build = krnlEngine(program_h, q_h, context_h, "gqeKernel", 0);
    krnl_JOIN_INNER_TD_3451_build.setup(tbl_Filter_TD_4197_output, tbl_Filter_TD_4427_output, tbl_JOIN_INNER_TD_3451_output, cfg_JOIN_INNER_TD_3451_cmds_build, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_3451_probe;
    krnl_JOIN_INNER_TD_3451_probe = krnlEngine(program_h, q_h, context_h, "gqeKernel", 1);
    krnl_JOIN_INNER_TD_3451_probe.setup(tbl_Filter_TD_4427_output, tbl_Filter_TD_4197_output, tbl_JOIN_INNER_TD_3451_output, cfg_JOIN_INNER_TD_3451_cmds_probe, buftmp_h);
    // ************************** Transfer Engine *********************** // 
    transEngine trans_Aggregate_TD_1746;
    trans_Aggregate_TD_1746.setq(q_a, 0);
    trans_Aggregate_TD_1746.add(&(cfg_Aggregate_TD_1746_cmds_part));
    trans_Aggregate_TD_1746.add(&(cfg_Aggregate_TD_1746_cmds));
    trans_Aggregate_TD_1746.add(&(cfg_Aggregate_TD_1746_cmds_out));
    transEngine trans_Aggregate_TD_1746_out;
    trans_Aggregate_TD_1746_out.setq(q_a, 0);
    q_a.finish();
    transEngine trans_JOIN_INNER_TD_2156;
    trans_JOIN_INNER_TD_2156.setq(q_h,0);
    trans_JOIN_INNER_TD_2156.add(&(cfg_JOIN_INNER_TD_2156_cmds_part));
    trans_JOIN_INNER_TD_2156.add(&(cfg_JOIN_INNER_TD_2156_cmds));
    transEngine trans_JOIN_INNER_TD_2156_out;
    trans_JOIN_INNER_TD_2156_out.setq(q_h,0);
    q_h.finish();
    transEngine trans_JOIN_INNER_TD_3451;
    trans_JOIN_INNER_TD_3451.setq(q_h,0);
    trans_JOIN_INNER_TD_3451.add(&(cfg_JOIN_INNER_TD_3451_cmds));
    q_h.finish();
    // ****************************** Events **************************** // 
    std::vector<cl::Event> events_h2d_wr_Aggregate_TD_1746;
    std::vector<cl::Event> events_d2h_rd_Aggregate_TD_1746;
    std::vector<cl::Event> events_Aggregate_TD_1746[2];
    events_h2d_wr_Aggregate_TD_1746.resize(1);
    events_d2h_rd_Aggregate_TD_1746.resize(1);
    events_Aggregate_TD_1746[0].resize(1);
    events_Aggregate_TD_1746[1].resize(hpTimes_aggr);
    std::vector<cl::Event> events_grp_Aggregate_TD_1746;
    std::vector<cl::Event> prev_events_grp_Aggregate_TD_1746;
    std::vector<cl::Event> events_h2d_wr_JOIN_INNER_TD_2156;
    std::vector<cl::Event> events_d2h_rd_JOIN_INNER_TD_2156;
    std::vector<cl::Event> events_JOIN_INNER_TD_2156[2];
    events_h2d_wr_JOIN_INNER_TD_2156.resize(1);
    events_d2h_rd_JOIN_INNER_TD_2156.resize(1);
    events_JOIN_INNER_TD_2156[0].resize(1);
    events_JOIN_INNER_TD_2156[1].resize(hpTimes_join);
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_2156;
    std::vector<cl::Event> prev_events_grp_JOIN_INNER_TD_2156;
    std::vector<cl::Event> events_h2d_wr_JOIN_INNER_TD_3451;
    std::vector<cl::Event> events_d2h_rd_JOIN_INNER_TD_3451;
    std::vector<cl::Event> events_JOIN_INNER_TD_3451[2];
    events_h2d_wr_JOIN_INNER_TD_3451.resize(1);
    events_d2h_rd_JOIN_INNER_TD_3451.resize(1);
    events_JOIN_INNER_TD_3451[0].resize(2);
    events_JOIN_INNER_TD_3451[1].resize(hpTimes_join);
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_3451;
    std::vector<cl::Event> prev_events_grp_JOIN_INNER_TD_3451;
    // **************************** Operations ************************** // 
    struct timeval tv_r_s, tv_r_e; 
    gettimeofday(&tv_r_s, 0); 

    struct timeval tv_r_Filter_4_139_s, tv_r_Filter_4_139_e;
    gettimeofday(&tv_r_Filter_4_139_s, 0);
    SW_Filter_TD_4427(tbl_SerializeFromObject_TD_5252_input, tbl_Filter_TD_4427_output);
    gettimeofday(&tv_r_Filter_4_139_e, 0);

    struct timeval tv_r_Filter_4_484_s, tv_r_Filter_4_484_e;
    gettimeofday(&tv_r_Filter_4_484_s, 0);
    SW_Filter_TD_4197(tbl_SerializeFromObject_TD_5595_input, tbl_Filter_TD_4197_output);
    gettimeofday(&tv_r_Filter_4_484_e, 0);

    struct timeval tv_r_JOIN_INNER_3_649_s, tv_r_JOIN_INNER_3_649_e;
    gettimeofday(&tv_r_JOIN_INNER_3_649_s, 0);
    trans_JOIN_INNER_TD_3451.add(&(tbl_Filter_TD_4197_output));
    trans_JOIN_INNER_TD_3451.add(&(tbl_Filter_TD_4427_output));
    trans_JOIN_INNER_TD_3451.host2dev(0, &(prev_events_grp_JOIN_INNER_TD_3451), &(events_h2d_wr_JOIN_INNER_TD_3451[0]));
    events_grp_JOIN_INNER_TD_3451.push_back(events_h2d_wr_JOIN_INNER_TD_3451[0]);
    krnl_JOIN_INNER_TD_3451_part_left.run(0, &(events_grp_JOIN_INNER_TD_3451), &(events_JOIN_INNER_TD_3451[0][0]));
    krnl_JOIN_INNER_TD_3451_part_right.run(0, &(events_grp_JOIN_INNER_TD_3451), &(events_JOIN_INNER_TD_3451[0][1]));
    for (int i(0); i < hpTimes_join; ++i) {
        krnl_JOIN_INNER_TD_3451[i].run(0, &(events_JOIN_INNER_TD_3451[0]), &(events_JOIN_INNER_TD_3451[1][i]));
    }
    gettimeofday(&tv_r_JOIN_INNER_3_649_e, 0);

    struct timeval tv_r_Filter_3_219_s, tv_r_Filter_3_219_e;
    gettimeofday(&tv_r_Filter_3_219_s, 0);
    SW_Filter_TD_3418(tbl_SerializeFromObject_TD_4640_input, tbl_Filter_TD_3418_output);
    gettimeofday(&tv_r_Filter_3_219_e, 0);

    struct timeval tv_r_JOIN_INNER_2_71_s, tv_r_JOIN_INNER_2_71_e;
    gettimeofday(&tv_r_JOIN_INNER_2_71_s, 0);
    prev_events_grp_JOIN_INNER_TD_2156.push_back(events_h2d_wr_JOIN_INNER_TD_3451[0]);
    trans_JOIN_INNER_TD_2156.add(&(tbl_Filter_TD_3418_output));
    trans_JOIN_INNER_TD_2156.host2dev(0, &(prev_events_grp_JOIN_INNER_TD_2156), &(events_h2d_wr_JOIN_INNER_TD_2156[0]));
    events_grp_JOIN_INNER_TD_2156.push_back(events_h2d_wr_JOIN_INNER_TD_2156[0]);
    events_grp_JOIN_INNER_TD_2156.insert(std::end(events_grp_JOIN_INNER_TD_2156), std::begin(events_JOIN_INNER_TD_3451[0]), std::end(events_JOIN_INNER_TD_3451[0]));
    events_grp_JOIN_INNER_TD_2156.insert(std::end(events_grp_JOIN_INNER_TD_2156), std::begin(events_JOIN_INNER_TD_3451[1]), std::end(events_JOIN_INNER_TD_3451[1]));
    krnl_JOIN_INNER_TD_2156_part_right.run(0, &(events_grp_JOIN_INNER_TD_2156), &(events_JOIN_INNER_TD_2156[0][0]));
    for (int i(0); i < hpTimes_join; ++i) {
        krnl_JOIN_INNER_TD_2156[i].run(0, &(events_grp_JOIN_INNER_TD_2156), &(events_JOIN_INNER_TD_2156[1][i]));
    }
    for (int i(0); i < hpTimes_join; ++i) {
        trans_JOIN_INNER_TD_2156_out.add(&(tbl_JOIN_INNER_TD_2156_output_preprocess_partition_array[i]));
    }
    trans_JOIN_INNER_TD_2156_out.dev2host(0, &(events_JOIN_INNER_TD_2156[1]), &(events_d2h_rd_JOIN_INNER_TD_2156[0]));
    q_h.flush();
    q_h.finish();
    WaitForEvents(events_d2h_rd_JOIN_INNER_TD_2156);
    tbl_JOIN_INNER_TD_2156_output_preprocess.copyTableData(&(tbl_JOIN_INNER_TD_2156_output));
    gettimeofday(&tv_r_JOIN_INNER_2_71_e, 0);

    struct timeval tv_r_Aggregate_1_20_s, tv_r_Aggregate_1_20_e;
    gettimeofday(&tv_r_Aggregate_1_20_s, 0);
    trans_Aggregate_TD_1746.add(&(tbl_JOIN_INNER_TD_2156_output));
    trans_Aggregate_TD_1746.host2dev(0, &(prev_events_grp_Aggregate_TD_1746), &(events_h2d_wr_Aggregate_TD_1746[0]));
    events_grp_Aggregate_TD_1746.push_back(events_h2d_wr_Aggregate_TD_1746[0]);
    krnl_Aggregate_TD_1746_part.run(0, &(events_grp_Aggregate_TD_1746), &(events_Aggregate_TD_1746[0][0]));
    for (int i(0); i < hpTimes_aggr; ++i) {
        krnl_Aggregate_TD_1746[i].run(0, &(events_Aggregate_TD_1746[0]), &(events_Aggregate_TD_1746[1][i]));
    }
    
    for (int i(0); i < hpTimes_aggr; ++i) {
        trans_Aggregate_TD_1746_out.add(&(tbl_Aggregate_TD_1746_output_partition_array[i]));
    }
    trans_Aggregate_TD_1746_out.dev2host(0, &(events_Aggregate_TD_1746[1]), &(events_d2h_rd_Aggregate_TD_1746[0]));
    q_a.flush();
    q_a.finish();
    
    SW_Aggregate_TD_1746_consolidate(tbl_Aggregate_TD_1746_output_partition_array, tbl_Aggregate_TD_1746_output, hpTimes_aggr);
    gettimeofday(&tv_r_Aggregate_1_20_e, 0);

    struct timeval tv_r_Sort_0_374_s, tv_r_Sort_0_374_e;
    gettimeofday(&tv_r_Sort_0_374_s, 0);
    SW_Sort_TD_0224(tbl_Aggregate_TD_1746_output, tbl_Sort_TD_0224_output);
    gettimeofday(&tv_r_Sort_0_374_e, 0);

std::cout << "CPU TIME: " << (tvdiff(&tv_r_Filter_4_139_s, &tv_r_Filter_4_139_e) + 
tvdiff(&tv_r_Filter_4_484_s, &tv_r_Filter_4_484_e) + 
tvdiff(&tv_r_Filter_3_219_s, &tv_r_Filter_3_219_e) + 
tvdiff(&tv_r_Sort_0_374_s, &tv_r_Sort_0_374_e) + 
1) / 1000.0 << std::endl;

std::cout << "FPGA TIME: " << (tvdiff(&tv_r_JOIN_INNER_3_649_s, &tv_r_JOIN_INNER_3_649_e) + 
tvdiff(&tv_r_JOIN_INNER_2_71_s, &tv_r_JOIN_INNER_2_71_e) + 
tvdiff(&tv_r_Aggregate_1_20_s, &tv_r_Aggregate_1_20_e) + 
1) / 1000.0 << std::endl;

    gettimeofday(&tv_r_e, 0); 
    // **************************** Print Execution Time ************************** // 
    std::cout << std::endl; 
    std::cout << "Filter_4: " << tvdiff(&tv_r_Filter_4_139_s, &tv_r_Filter_4_139_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer((o_orderdate#211 < 19950307)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_5252_input.getNumRow() << " -> tbl_SerializeFromObject_TD_5252_input" 
     << std::endl << "      #COL: 4: " << "ListBuffer(o_orderkey#207, o_custkey#208, o_orderdate#211, o_shippriority#214)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_4427_output.getNumRow() << " -> tbl_Filter_TD_4427_output" 
     << std::endl << "      #COL: 4: " << "ListBuffer(o_orderkey#207, o_custkey#208, o_orderdate#211, o_shippriority#214)" 
     << std::endl; 

    std::cout << "Filter_4: " << tvdiff(&tv_r_Filter_4_484_s, &tv_r_Filter_4_484_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer((isnotnull(c_mktsegment#28) AND (c_mktsegment#28 = MACHINERY))) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_5595_input.getNumRow() << " -> tbl_SerializeFromObject_TD_5595_input" 
     << std::endl << "      #COL: 2: " << "ListBuffer(c_custkey#22, c_mktsegment#28)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_4197_output.getNumRow() << " -> tbl_Filter_TD_4197_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(c_custkey#22)" 
     << std::endl; 

    std::cout << "JOIN_INNER_3: " << tvdiff(&tv_r_JOIN_INNER_3_649_s, &tv_r_JOIN_INNER_3_649_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer((c_custkey#22 = o_custkey#208)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_4197_output.getNumRow() << " -> tbl_Filter_TD_4197_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(c_custkey#22)" 
     << std::endl << "      #ROW: " << tbl_Filter_TD_4427_output.getNumRow() << " -> tbl_Filter_TD_4427_output" 
     << std::endl << "      #COL: 4: " << "ListBuffer(o_orderkey#207, o_custkey#208, o_orderdate#211, o_shippriority#214)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_3451_output.getNumRow() << " -> tbl_JOIN_INNER_TD_3451_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(o_orderkey#207, o_orderdate#211, o_shippriority#214)" 
     << std::endl; 

    std::cout << "Filter_3: " << tvdiff(&tv_r_Filter_3_219_s, &tv_r_Filter_3_219_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer((l_shipdate#87 > 19950307)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_4640_input.getNumRow() << " -> tbl_SerializeFromObject_TD_4640_input" 
     << std::endl << "      #COL: 4: " << "ListBuffer(l_orderkey#77, l_extendedprice#82, l_discount#83, l_shipdate#87)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_3418_output.getNumRow() << " -> tbl_Filter_TD_3418_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(l_orderkey#77, l_extendedprice#82, l_discount#83)" 
     << std::endl; 

    std::cout << "JOIN_INNER_2: " << tvdiff(&tv_r_JOIN_INNER_2_71_s, &tv_r_JOIN_INNER_2_71_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer((l_orderkey#77 = o_orderkey#207)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_3418_output.getNumRow() << " -> tbl_Filter_TD_3418_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(l_orderkey#77, l_extendedprice#82, l_discount#83)" 
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_3451_output.getNumRow() << " -> tbl_JOIN_INNER_TD_3451_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(o_orderkey#207, o_orderdate#211, o_shippriority#214)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_2156_output.getNumRow() << " -> tbl_JOIN_INNER_TD_2156_output" 
     << std::endl << "      #COL: 5: " << "ListBuffer(o_orderdate#211, o_shippriority#214, l_orderkey#77, l_extendedprice#82, l_discount#83)" 
     << std::endl; 

    std::cout << "Aggregate_1: " << tvdiff(&tv_r_Aggregate_1_20_s, &tv_r_Aggregate_1_20_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer(l_orderkey#77, o_orderdate#211, o_shippriority#214, sum((l_extendedprice#82 * (100 - l_discount#83))) AS revenue#686L) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_2156_output.getNumRow() << " -> tbl_JOIN_INNER_TD_2156_output" 
     << std::endl << "      #COL: 5: " << "ListBuffer(o_orderdate#211, o_shippriority#214, l_orderkey#77, l_extendedprice#82, l_discount#83)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_1746_output.getNumRow() << " -> tbl_Aggregate_TD_1746_output" 
     << std::endl << "      #COL: 4: " << "ListBuffer(l_orderkey#77, revenue#686L, o_orderdate#211, o_shippriority#214)" 
     << std::endl; 

    std::cout << "Sort_0: " << tvdiff(&tv_r_Sort_0_374_s, &tv_r_Sort_0_374_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(revenue#686L DESC NULLS LAST, o_orderdate#211 ASC NULLS FIRST) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_1746_output.getNumRow() << " -> tbl_Aggregate_TD_1746_output" 
     << std::endl << "      #COL: 4: " << "ListBuffer(l_orderkey#77, revenue#686L, o_orderdate#211, o_shippriority#214)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Sort_TD_0224_output.getNumRow() << " -> tbl_Sort_TD_0224_output" 
     << std::endl << "      #COL: 4: " << "ListBuffer(l_orderkey#77, revenue#686L, o_orderdate#211, o_shippriority#214)" 
     << std::endl; 

    std::cout << std::endl << " Total execution time: " << tvdiff(&tv_r_s, &tv_r_e) / 1000 << " ms"; 

    std::cout << std::endl << " Spark elapsed time: " << 24.487436 * 1000 << "ms" << std::endl; 
    return 0; 
}
