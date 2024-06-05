// number of overlays (w/o fusion): 2 
// number of overlays (w/ fusion): 2 
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

#include "cfgFunc_q13.hpp" 
#include "q13.hpp" 

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
    std::cout << "NOTE:running query #13\n."; 
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
    Table tbl_Sort_TD_0954_output("tbl_Sort_TD_0954_output", 183000000, 2, "");
    tbl_Sort_TD_0954_output.allocateHost();
    Table tbl_Aggregate_TD_1576_output("tbl_Aggregate_TD_1576_output", 183000000, 2, "");
    tbl_Aggregate_TD_1576_output.allocateHost();
    Table tbl_Aggregate_TD_2425_output("tbl_Aggregate_TD_2425_output", 183000000, 1, "");
    tbl_Aggregate_TD_2425_output.allocateHost();
    Table tbl_JOIN_LEFTANTI_TD_3852_output("tbl_JOIN_LEFTANTI_TD_3852_output", 1531000, 2, "");
    tbl_JOIN_LEFTANTI_TD_3852_output.allocateHost(1.2, hpTimes_join);
    Table tbl_JOIN_INNER_TD_0301_output("tbl_JOIN_INNER_TD_0301_output", 1480133, 3, "");
    tbl_JOIN_INNER_TD_0301_output.allocateHost(1.2, hpTimes_join);
    Table tbl_SerializeFromObject_TD_4887_input;
    tbl_SerializeFromObject_TD_4887_input = Table("customer", customer_n, 1, in_dir);
    tbl_SerializeFromObject_TD_4887_input.selectOverlayVersion(0);
    tbl_SerializeFromObject_TD_4887_input.addCol("c_custkey", 4);
    tbl_SerializeFromObject_TD_4887_input.allocateHost();
    tbl_SerializeFromObject_TD_4887_input.loadHost();
    Table tbl_SerializeFromObject_TD_4887_input_partition("tbl_SerializeFromObject_TD_4887_input_partition", customer_n, 1, "");
    tbl_SerializeFromObject_TD_4887_input_partition.allocateHost(1.2, hpTimes_join);
    tbl_SerializeFromObject_TD_4887_input_partition.allocateDevBuffer(context_h, 32);
    Table tbl_SerializeFromObject_TD_4887_input_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_SerializeFromObject_TD_4887_input_partition_array[i] = tbl_SerializeFromObject_TD_4887_input_partition.createSubTable(i);
    }
    Table tbl_Filter_TD_4209_output("tbl_Filter_TD_4209_output", 183000000, 2, "");
    tbl_Filter_TD_4209_output.selectOverlayVersion(0);
    tbl_Filter_TD_4209_output.allocateHost();
    Table tbl_Filter_TD_4209_output_partition("tbl_Filter_TD_4209_output_partition", 183000000, 2, "");
    tbl_Filter_TD_4209_output_partition.allocateHost(1.2, hpTimes_join);
    tbl_Filter_TD_4209_output_partition.allocateDevBuffer(context_h, 32);
    Table tbl_Filter_TD_4209_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_Filter_TD_4209_output_partition_array[i] = tbl_Filter_TD_4209_output_partition.createSubTable(i);
    }
    Table tbl_SerializeFromObject_TD_5828_input;
    tbl_SerializeFromObject_TD_5828_input = Table("order", order_n, 3, in_dir);
    tbl_SerializeFromObject_TD_5828_input.addCol("o_orderkey", 4);
    tbl_SerializeFromObject_TD_5828_input.addCol("o_custkey", 4);
    tbl_SerializeFromObject_TD_5828_input.addCol("o_comment", TPCH_READ_O_CMNT_MAX+1);
    tbl_SerializeFromObject_TD_5828_input.allocateHost();
    tbl_SerializeFromObject_TD_5828_input.loadHost();
    // ********************** Allocate Device Buffer ******************** // 
    tbl_JOIN_LEFTANTI_TD_3852_output.allocateDevBuffer(context_h, 32);
    Table tbl_JOIN_LEFTANTI_TD_3852_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_JOIN_LEFTANTI_TD_3852_output_partition_array[i] = tbl_JOIN_LEFTANTI_TD_3852_output.createSubTable(i);
    }
    tbl_JOIN_INNER_TD_0301_output.allocateDevBuffer(context_h, 32);
    Table tbl_JOIN_INNER_TD_0301_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_JOIN_INNER_TD_0301_output_partition_array[i] = tbl_JOIN_INNER_TD_0301_output.createSubTable(i);
    }
    tbl_SerializeFromObject_TD_4887_input.allocateDevBuffer(context_h, 33);
    tbl_Filter_TD_4209_output.allocateDevBuffer(context_h, 33);
    // ****************************** Config **************************** // 
    cfgCmd cfg_JOIN_LEFTANTI_TD_3852_cmds;
    cfg_JOIN_LEFTANTI_TD_3852_cmds.allocateHost();
    get_cfg_dat_JOIN_LEFTANTI_TD_3852_gqe_join (cfg_JOIN_LEFTANTI_TD_3852_cmds.cmd);
    cfg_JOIN_LEFTANTI_TD_3852_cmds.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_LEFTANTI_TD_3852_cmds_part;
    cfg_JOIN_LEFTANTI_TD_3852_cmds_part.allocateHost();
    get_cfg_dat_JOIN_LEFTANTI_TD_3852_gqe_join_part (cfg_JOIN_LEFTANTI_TD_3852_cmds_part.cmd);
    cfg_JOIN_LEFTANTI_TD_3852_cmds_part.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_0301_cmds;
    cfg_JOIN_INNER_TD_0301_cmds.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_0301_gqe_join (cfg_JOIN_INNER_TD_0301_cmds.cmd);
    cfg_JOIN_INNER_TD_0301_cmds.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_0301_cmds_part;
    cfg_JOIN_INNER_TD_0301_cmds_part.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_0301_gqe_join_part (cfg_JOIN_INNER_TD_0301_cmds_part.cmd);
    cfg_JOIN_INNER_TD_0301_cmds_part.allocateDevBuffer(context_h, 32);
    // *************************** Kernel Setup ************************* // 
    bufferTmp buftmp_h(context_h, 0); 
    buftmp_h.initBuffer(q_h); 
    std::cout << std::endl; 
    krnlEngine krnl_JOIN_LEFTANTI_TD_3852_part_left;
    krnl_JOIN_LEFTANTI_TD_3852_part_left = krnlEngine(program_h, q_h, context_h, "gqePart");
    krnl_JOIN_LEFTANTI_TD_3852_part_left.setup_hp(512, 0, power_of_hpTimes_join, tbl_SerializeFromObject_TD_4887_input, tbl_SerializeFromObject_TD_4887_input_partition, cfg_JOIN_LEFTANTI_TD_3852_cmds_part);
    krnlEngine krnl_JOIN_LEFTANTI_TD_3852[hpTimes_join];
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_LEFTANTI_TD_3852[i] = krnlEngine(program_h, q_h, "gqeJoin");
    }
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_LEFTANTI_TD_3852[i].setup(tbl_SerializeFromObject_TD_4887_input_partition_array[i], tbl_JOIN_INNER_TD_0301_output_partition_array[i], tbl_JOIN_LEFTANTI_TD_3852_output_partition_array[i], cfg_JOIN_LEFTANTI_TD_3852_cmds, buftmp_h);
    }
    krnlEngine krnl_JOIN_LEFTANTI_TD_3852_build;
    krnl_JOIN_LEFTANTI_TD_3852_build = krnlEngine(program_h, q_h, context_h, "gqeKernel", 0);
    krnl_JOIN_LEFTANTI_TD_3852_build.setup(tbl_SerializeFromObject_TD_4887_input, tbl_JOIN_INNER_TD_0301_output, tbl_JOIN_LEFTANTI_TD_3852_output, cfg_JOIN_LEFTANTI_TD_3852_cmds_build, buftmp_h);
    krnlEngine krnl_JOIN_LEFTANTI_TD_3852_probe;
    krnl_JOIN_LEFTANTI_TD_3852_probe = krnlEngine(program_h, q_h, context_h, "gqeKernel", 1);
    krnl_JOIN_LEFTANTI_TD_3852_probe.setup(tbl_JOIN_INNER_TD_0301_output, tbl_SerializeFromObject_TD_4887_input, tbl_JOIN_LEFTANTI_TD_3852_output, cfg_JOIN_LEFTANTI_TD_3852_cmds_probe, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_0301_part_left;
    krnl_JOIN_INNER_TD_0301_part_left = krnlEngine(program_h, q_h, context_h, "gqePart");
    krnl_JOIN_INNER_TD_0301_part_left.setup_hp(512, 0, power_of_hpTimes_join, tbl_SerializeFromObject_TD_4887_input, tbl_SerializeFromObject_TD_4887_input_partition, cfg_JOIN_INNER_TD_0301_cmds_part);
    krnlEngine krnl_JOIN_INNER_TD_0301_part_right;
    krnl_JOIN_INNER_TD_0301_part_right = krnlEngine(program_h, q_h, context_h, "gqePart");
    krnl_JOIN_INNER_TD_0301_part_right.setup_hp(512, 1, power_of_hpTimes_join, tbl_Filter_TD_4209_output, tbl_Filter_TD_4209_output_partition, cfg_JOIN_INNER_TD_0301_cmds_part);
    krnlEngine krnl_JOIN_INNER_TD_0301[hpTimes_join];
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_INNER_TD_0301[i] = krnlEngine(program_h, q_h, "gqeJoin");
    }
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_INNER_TD_0301[i].setup(tbl_SerializeFromObject_TD_4887_input_partition_array[i], tbl_Filter_TD_4209_output_partition_array[i], tbl_JOIN_INNER_TD_0301_output_partition_array[i], cfg_JOIN_INNER_TD_0301_cmds, buftmp_h);
    }
    krnlEngine krnl_JOIN_INNER_TD_0301_build;
    krnl_JOIN_INNER_TD_0301_build = krnlEngine(program_h, q_h, context_h, "gqeKernel", 0);
    krnl_JOIN_INNER_TD_0301_build.setup(tbl_SerializeFromObject_TD_4887_input, tbl_Filter_TD_4209_output, tbl_JOIN_INNER_TD_0301_output, cfg_JOIN_INNER_TD_0301_cmds_build, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_0301_probe;
    krnl_JOIN_INNER_TD_0301_probe = krnlEngine(program_h, q_h, context_h, "gqeKernel", 1);
    krnl_JOIN_INNER_TD_0301_probe.setup(tbl_Filter_TD_4209_output, tbl_SerializeFromObject_TD_4887_input, tbl_JOIN_INNER_TD_0301_output, cfg_JOIN_INNER_TD_0301_cmds_probe, buftmp_h);
    // ************************** Transfer Engine *********************** // 
    transEngine trans_JOIN_LEFTANTI_TD_3852;
    trans_JOIN_LEFTANTI_TD_3852.setq(q_h,0);
    trans_JOIN_LEFTANTI_TD_3852.add(&(cfg_JOIN_LEFTANTI_TD_3852_cmds_part));
    trans_JOIN_LEFTANTI_TD_3852.add(&(cfg_JOIN_LEFTANTI_TD_3852_cmds));
    trans_JOIN_LEFTANTI_TD_3852.add(&(tbl_SerializeFromObject_TD_4887_input));
    transEngine trans_JOIN_LEFTANTI_TD_3852_out;
    trans_JOIN_LEFTANTI_TD_3852_out.setq(q_h,0);
    q_h.finish();
    transEngine trans_JOIN_INNER_TD_0301;
    trans_JOIN_INNER_TD_0301.setq(q_h,0);
    trans_JOIN_INNER_TD_0301.add(&(cfg_JOIN_INNER_TD_0301_cmds_part));
    trans_JOIN_INNER_TD_0301.add(&(cfg_JOIN_INNER_TD_0301_cmds));
    trans_JOIN_INNER_TD_0301.add(&(tbl_SerializeFromObject_TD_4887_input));
    q_h.finish();
    // ****************************** Events **************************** // 
    std::vector<cl::Event> events_h2d_wr_JOIN_LEFTANTI_TD_3852;
    std::vector<cl::Event> events_d2h_rd_JOIN_LEFTANTI_TD_3852;
    std::vector<cl::Event> events_JOIN_LEFTANTI_TD_3852[2];
    events_h2d_wr_JOIN_LEFTANTI_TD_3852.resize(1);
    events_d2h_rd_JOIN_LEFTANTI_TD_3852.resize(1);
    events_JOIN_LEFTANTI_TD_3852[0].resize(1);
    events_JOIN_LEFTANTI_TD_3852[1].resize(hpTimes_join);
    std::vector<cl::Event> events_grp_JOIN_LEFTANTI_TD_3852;
    std::vector<cl::Event> prev_events_grp_JOIN_LEFTANTI_TD_3852;
    std::vector<cl::Event> events_h2d_wr_JOIN_INNER_TD_0301;
    std::vector<cl::Event> events_d2h_rd_JOIN_INNER_TD_0301;
    std::vector<cl::Event> events_JOIN_INNER_TD_0301[2];
    events_h2d_wr_JOIN_INNER_TD_0301.resize(1);
    events_d2h_rd_JOIN_INNER_TD_0301.resize(1);
    events_JOIN_INNER_TD_0301[0].resize(2);
    events_JOIN_INNER_TD_0301[1].resize(hpTimes_join);
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_0301;
    std::vector<cl::Event> prev_events_grp_JOIN_INNER_TD_0301;
    // **************************** Operations ************************** // 
    struct timeval tv_r_s, tv_r_e; 
    gettimeofday(&tv_r_s, 0); 

    struct timeval tv_r_Filter_4_206_s, tv_r_Filter_4_206_e;
    gettimeofday(&tv_r_Filter_4_206_s, 0);
    SW_Filter_TD_4209(tbl_SerializeFromObject_TD_5828_input, tbl_Filter_TD_4209_output);
    gettimeofday(&tv_r_Filter_4_206_e, 0);

    struct timeval tv_r_JOIN_INNER_0_399_s, tv_r_JOIN_INNER_0_399_e;
    gettimeofday(&tv_r_JOIN_INNER_0_399_s, 0);
    trans_JOIN_INNER_TD_0301.add(&(tbl_Filter_TD_4209_output));
    trans_JOIN_INNER_TD_0301.host2dev(0, &(prev_events_grp_JOIN_INNER_TD_0301), &(events_h2d_wr_JOIN_INNER_TD_0301[0]));
    events_grp_JOIN_INNER_TD_0301.push_back(events_h2d_wr_JOIN_INNER_TD_0301[0]);
    krnl_JOIN_INNER_TD_0301_part_left.run(0, &(events_grp_JOIN_INNER_TD_0301), &(events_JOIN_INNER_TD_0301[0][0]));
    krnl_JOIN_INNER_TD_0301_part_right.run(0, &(events_grp_JOIN_INNER_TD_0301), &(events_JOIN_INNER_TD_0301[0][1]));
    for (int i(0); i < hpTimes_join; ++i) {
        krnl_JOIN_INNER_TD_0301[i].run(0, &(events_JOIN_INNER_TD_0301[0]), &(events_JOIN_INNER_TD_0301[1][i]));
    }
    gettimeofday(&tv_r_JOIN_INNER_0_399_e, 0);

    struct timeval tv_r_JOIN_LEFTANTI_3_272_s, tv_r_JOIN_LEFTANTI_3_272_e;
    gettimeofday(&tv_r_JOIN_LEFTANTI_3_272_s, 0);
    prev_events_grp_JOIN_LEFTANTI_TD_3852.push_back(events_h2d_wr_JOIN_INNER_TD_0301[0]);
    trans_JOIN_LEFTANTI_TD_3852.host2dev(0, &(prev_events_grp_JOIN_LEFTANTI_TD_3852), &(events_h2d_wr_JOIN_LEFTANTI_TD_3852[0]));
    events_grp_JOIN_LEFTANTI_TD_3852.push_back(events_h2d_wr_JOIN_LEFTANTI_TD_3852[0]);
    events_grp_JOIN_LEFTANTI_TD_3852.insert(std::end(events_grp_JOIN_LEFTANTI_TD_3852), std::begin(events_JOIN_INNER_TD_0301[0]), std::end(events_JOIN_INNER_TD_0301[0]));
    events_grp_JOIN_LEFTANTI_TD_3852.insert(std::end(events_grp_JOIN_LEFTANTI_TD_3852), std::begin(events_JOIN_INNER_TD_0301[1]), std::end(events_JOIN_INNER_TD_0301[1]));
    krnl_JOIN_LEFTANTI_TD_3852_part_left.run(0, &(events_grp_JOIN_LEFTANTI_TD_3852), &(events_JOIN_LEFTANTI_TD_3852[0][0]));
    for (int i(0); i < hpTimes_join; ++i) {
        krnl_JOIN_LEFTANTI_TD_3852[i].run(0, &(events_JOIN_LEFTANTI_TD_3852[0]), &(events_JOIN_LEFTANTI_TD_3852[1][i]));
    }
    
    for (int i(0); i < hpTimes_join; ++i) {
        trans_JOIN_LEFTANTI_TD_3852_out.add(&(tbl_JOIN_LEFTANTI_TD_3852_output_partition_array[i]));
        trans_JOIN_LEFTANTI_TD_3852_out.add(&(tbl_JOIN_INNER_TD_0301_output_partition_array[i]));
    }
    trans_JOIN_LEFTANTI_TD_3852_out.dev2host(0, &(events_JOIN_LEFTANTI_TD_3852[1]), &(events_d2h_rd_JOIN_LEFTANTI_TD_3852[0]));
    q_h.flush();
    q_h.finish();
    SW_JOIN_LEFTANTI_TD_3852_concatenate(tbl_JOIN_LEFTANTI_TD_3852_output, tbl_JOIN_INNER_TD_0301_output);
    tbl_JOIN_LEFTANTI_TD_3852_output.copyTableData(&(tbl_JOIN_LEFTANTI_TD_3852_output));
    gettimeofday(&tv_r_JOIN_LEFTANTI_3_272_e, 0);

    struct timeval tv_r_Aggregate_2_14_s, tv_r_Aggregate_2_14_e;
    gettimeofday(&tv_r_Aggregate_2_14_s, 0);
    SW_Aggregate_TD_2425(tbl_JOIN_LEFTANTI_TD_3852_output_partition_array, tbl_Aggregate_TD_2425_output, hpTimes_join);
    gettimeofday(&tv_r_Aggregate_2_14_e, 0);

    struct timeval tv_r_Aggregate_1_770_s, tv_r_Aggregate_1_770_e;
    gettimeofday(&tv_r_Aggregate_1_770_s, 0);
    SW_Aggregate_TD_1576(tbl_Aggregate_TD_2425_output, tbl_Aggregate_TD_1576_output);
    gettimeofday(&tv_r_Aggregate_1_770_e, 0);

    struct timeval tv_r_Sort_0_110_s, tv_r_Sort_0_110_e;
    gettimeofday(&tv_r_Sort_0_110_s, 0);
    SW_Sort_TD_0954(tbl_Aggregate_TD_1576_output, tbl_Sort_TD_0954_output);
    gettimeofday(&tv_r_Sort_0_110_e, 0);

std::cout << "CPU TIME: " << (tvdiff(&tv_r_Filter_4_206_s, &tv_r_Filter_4_206_e) + 
tvdiff(&tv_r_Aggregate_2_14_s, &tv_r_Aggregate_2_14_e) + 
tvdiff(&tv_r_Aggregate_1_770_s, &tv_r_Aggregate_1_770_e) + 
tvdiff(&tv_r_Sort_0_110_s, &tv_r_Sort_0_110_e) + 
1) / 1000.0 << std::endl;

std::cout << "FPGA TIME: " << (tvdiff(&tv_r_JOIN_INNER_0_399_s, &tv_r_JOIN_INNER_0_399_e) + 
tvdiff(&tv_r_JOIN_LEFTANTI_3_272_s, &tv_r_JOIN_LEFTANTI_3_272_e) + 
1) / 1000.0 << std::endl;

    gettimeofday(&tv_r_e, 0); 
    // **************************** Print Execution Time ************************** // 
    std::cout << std::endl; 
    std::cout << "Filter_4: " << tvdiff(&tv_r_Filter_4_206_s, &tv_r_Filter_4_206_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer((isnotnull(o_comment#215) AND NOT o_comment#215 LIKE %unusual%deposits%)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_5828_input.getNumRow() << " -> tbl_SerializeFromObject_TD_5828_input" 
     << std::endl << "      #COL: 3: " << "ListBuffer(o_orderkey#207, o_custkey#208, o_comment#215)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_4209_output.getNumRow() << " -> tbl_Filter_TD_4209_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(o_orderkey#207, o_custkey#208)" 
     << std::endl; 

    std::cout << "JOIN_INNER_0: " << tvdiff(&tv_r_JOIN_INNER_0_399_s, &tv_r_JOIN_INNER_0_399_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer((c_custkey#22 = o_custkey#208)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_4887_input.getNumRow() << " -> tbl_SerializeFromObject_TD_4887_input" 
     << std::endl << "      #COL: 1: " << "ListBuffer(c_custkey#22)" 
     << std::endl << "      #ROW: " << tbl_Filter_TD_4209_output.getNumRow() << " -> tbl_Filter_TD_4209_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(o_orderkey#207, o_custkey#208)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_0301_output.getNumRow() << " -> tbl_JOIN_INNER_TD_0301_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(c_custkey#22, o_orderkey#207, o_custkey#208)" 
     << std::endl; 

    std::cout << "JOIN_LEFTANTI_3: " << tvdiff(&tv_r_JOIN_LEFTANTI_3_272_s, &tv_r_JOIN_LEFTANTI_3_272_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer((c_custkey#22 = o_custkey#208)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_0301_output.getNumRow() << " -> tbl_JOIN_INNER_TD_0301_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(c_custkey#22, o_orderkey#207, o_custkey#208)" 
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_4887_input.getNumRow() << " -> tbl_SerializeFromObject_TD_4887_input" 
     << std::endl << "      #COL: 1: " << "ListBuffer(c_custkey#22)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_LEFTANTI_TD_3852_output.getNumRow() << " -> tbl_JOIN_LEFTANTI_TD_3852_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(c_custkey#22, o_orderkey#207)" 
     << std::endl; 

    std::cout << "Aggregate_2: " << tvdiff(&tv_r_Aggregate_2_14_s, &tv_r_Aggregate_2_14_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(c_custkey#22, count(o_orderkey#207) AS c_count#1367L) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_LEFTANTI_TD_3852_output.getNumRow() << " -> tbl_JOIN_LEFTANTI_TD_3852_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(c_custkey#22, o_orderkey#207)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_2425_output.getNumRow() << " -> tbl_Aggregate_TD_2425_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(c_count#1367L)" 
     << std::endl; 

    std::cout << "Aggregate_1: " << tvdiff(&tv_r_Aggregate_1_770_s, &tv_r_Aggregate_1_770_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(c_count#1367L, count(1) AS custdist#1362L) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_2425_output.getNumRow() << " -> tbl_Aggregate_TD_2425_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(c_count#1367L)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_1576_output.getNumRow() << " -> tbl_Aggregate_TD_1576_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(c_count#1367L, custdist#1362L)" 
     << std::endl; 

    std::cout << "Sort_0: " << tvdiff(&tv_r_Sort_0_110_s, &tv_r_Sort_0_110_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(custdist#1362L DESC NULLS LAST, c_count#1367L DESC NULLS LAST) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_1576_output.getNumRow() << " -> tbl_Aggregate_TD_1576_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(c_count#1367L, custdist#1362L)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Sort_TD_0954_output.getNumRow() << " -> tbl_Sort_TD_0954_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(c_count#1367L, custdist#1362L)" 
     << std::endl; 

    std::cout << std::endl << " Total execution time: " << tvdiff(&tv_r_s, &tv_r_e) / 1000 << " ms"; 

    std::cout << std::endl << " Spark elapsed time: " << 6.594588 * 1000 << "ms" << std::endl; 
    return 0; 
}
