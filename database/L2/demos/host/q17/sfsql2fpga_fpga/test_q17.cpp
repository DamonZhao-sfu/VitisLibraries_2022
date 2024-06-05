// number of overlays (w/o fusion): 1 
// number of overlays (w/ fusion): 1 
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

#include "cfgFunc_q17.hpp" 
#include "q17.hpp" 

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
    std::cout << "NOTE:running query #17\n."; 
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
    Table tbl_Aggregate_TD_0585_output("tbl_Aggregate_TD_0585_output", 183000000, 1, "");
    tbl_Aggregate_TD_0585_output.allocateHost();
    Table tbl_JOIN_INNER_TD_1399_output("tbl_JOIN_INNER_TD_1399_output", 183000000, 1, "");
    tbl_JOIN_INNER_TD_1399_output.allocateHost();
    Table tbl_JOIN_INNER_TD_265_output("tbl_JOIN_INNER_TD_265_output", 1507, 3, "");
    tbl_JOIN_INNER_TD_265_output.allocateHost(1.2, hpTimes_join);
    Table tbl_Aggregate_TD_3359_output("tbl_Aggregate_TD_3359_output", 183000000, 2, "");
    tbl_Aggregate_TD_3359_output.allocateHost();
    Table tbl_SerializeFromObject_TD_317_input;
    tbl_SerializeFromObject_TD_317_input = Table("lineitem", lineitem_n, 3, in_dir);
    tbl_SerializeFromObject_TD_317_input.selectOverlayVersion(0);
    tbl_SerializeFromObject_TD_317_input.addCol("l_partkey", 4);
    tbl_SerializeFromObject_TD_317_input.addCol("l_quantity", 4);
    tbl_SerializeFromObject_TD_317_input.addCol("l_extendedprice", 4);
    tbl_SerializeFromObject_TD_317_input.allocateHost();
    tbl_SerializeFromObject_TD_317_input.loadHost();
    Table tbl_SerializeFromObject_TD_317_input_partition("tbl_SerializeFromObject_TD_317_input_partition", lineitem_n, 3, "");
    tbl_SerializeFromObject_TD_317_input_partition.allocateHost(1.2, hpTimes_join);
    tbl_SerializeFromObject_TD_317_input_partition.allocateDevBuffer(context_h, 32);
    Table tbl_SerializeFromObject_TD_317_input_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_SerializeFromObject_TD_317_input_partition_array[i] = tbl_SerializeFromObject_TD_317_input_partition.createSubTable(i);
    }
    Table tbl_Filter_TD_381_output("tbl_Filter_TD_381_output", 183000000, 1, "");
    tbl_Filter_TD_381_output.selectOverlayVersion(0);
    tbl_Filter_TD_381_output.allocateHost();
    Table tbl_Filter_TD_381_output_partition("tbl_Filter_TD_381_output_partition", 183000000, 1, "");
    tbl_Filter_TD_381_output_partition.allocateHost(1.2, hpTimes_join);
    tbl_Filter_TD_381_output_partition.allocateDevBuffer(context_h, 32);
    Table tbl_Filter_TD_381_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_Filter_TD_381_output_partition_array[i] = tbl_Filter_TD_381_output_partition.createSubTable(i);
    }
    Table tbl_SerializeFromObject_TD_4544_input;
    tbl_SerializeFromObject_TD_4544_input = Table("lineitem", lineitem_n, 2, in_dir);
    tbl_SerializeFromObject_TD_4544_input.addCol("l_partkey", 4);
    tbl_SerializeFromObject_TD_4544_input.addCol("l_quantity", 4);
    tbl_SerializeFromObject_TD_4544_input.allocateHost();
    tbl_SerializeFromObject_TD_4544_input.loadHost();
    Table tbl_SerializeFromObject_TD_4325_input;
    tbl_SerializeFromObject_TD_4325_input = Table("part", part_n, 3, in_dir);
    tbl_SerializeFromObject_TD_4325_input.addCol("p_partkey", 4);
    tbl_SerializeFromObject_TD_4325_input.addCol("p_brand", TPCH_READ_P_BRND_LEN+1);
    tbl_SerializeFromObject_TD_4325_input.addCol("p_container", TPCH_READ_P_CNTR_LEN+1);
    tbl_SerializeFromObject_TD_4325_input.allocateHost();
    tbl_SerializeFromObject_TD_4325_input.loadHost();
    // ********************** Allocate Device Buffer ******************** // 
    tbl_JOIN_INNER_TD_265_output.allocateDevBuffer(context_h, 32);
    Table tbl_JOIN_INNER_TD_265_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_JOIN_INNER_TD_265_output_partition_array[i] = tbl_JOIN_INNER_TD_265_output.createSubTable(i);
    }
    tbl_SerializeFromObject_TD_317_input.allocateDevBuffer(context_h, 33);
    tbl_Filter_TD_381_output.allocateDevBuffer(context_h, 33);
    // ****************************** Config **************************** // 
    cfgCmd cfg_JOIN_INNER_TD_265_cmds;
    cfg_JOIN_INNER_TD_265_cmds.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_265_gqe_join (cfg_JOIN_INNER_TD_265_cmds.cmd);
    cfg_JOIN_INNER_TD_265_cmds.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_265_cmds_part;
    cfg_JOIN_INNER_TD_265_cmds_part.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_265_gqe_join_part (cfg_JOIN_INNER_TD_265_cmds_part.cmd);
    cfg_JOIN_INNER_TD_265_cmds_part.allocateDevBuffer(context_h, 32);
    // *************************** Kernel Setup ************************* // 
    bufferTmp buftmp_h(context_h, 0); 
    buftmp_h.initBuffer(q_h); 
    std::cout << std::endl; 
    krnlEngine krnl_JOIN_INNER_TD_265_part_left;
    krnl_JOIN_INNER_TD_265_part_left = krnlEngine(program_h, q_h, context_h, "gqePart");
    krnl_JOIN_INNER_TD_265_part_left.setup_hp(512, 0, power_of_hpTimes_join, tbl_Filter_TD_381_output, tbl_Filter_TD_381_output_partition, cfg_JOIN_INNER_TD_265_cmds_part);
    krnlEngine krnl_JOIN_INNER_TD_265_part_right;
    krnl_JOIN_INNER_TD_265_part_right = krnlEngine(program_h, q_h, context_h, "gqePart");
    krnl_JOIN_INNER_TD_265_part_right.setup_hp(512, 1, power_of_hpTimes_join, tbl_SerializeFromObject_TD_317_input, tbl_SerializeFromObject_TD_317_input_partition, cfg_JOIN_INNER_TD_265_cmds_part);
    krnlEngine krnl_JOIN_INNER_TD_265[hpTimes_join];
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_INNER_TD_265[i] = krnlEngine(program_h, q_h, "gqeJoin");
    }
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_INNER_TD_265[i].setup(tbl_Filter_TD_381_output_partition_array[i], tbl_SerializeFromObject_TD_317_input_partition_array[i], tbl_JOIN_INNER_TD_265_output_partition_array[i], cfg_JOIN_INNER_TD_265_cmds, buftmp_h);
    }
    krnlEngine krnl_JOIN_INNER_TD_265_build;
    krnl_JOIN_INNER_TD_265_build = krnlEngine(program_h, q_h, context_h, "gqeKernel", 0);
    krnl_JOIN_INNER_TD_265_build.setup(tbl_Filter_TD_381_output, tbl_SerializeFromObject_TD_317_input, tbl_JOIN_INNER_TD_265_output, cfg_JOIN_INNER_TD_265_cmds_build, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_265_probe;
    krnl_JOIN_INNER_TD_265_probe = krnlEngine(program_h, q_h, context_h, "gqeKernel", 1);
    krnl_JOIN_INNER_TD_265_probe.setup(tbl_SerializeFromObject_TD_317_input, tbl_Filter_TD_381_output, tbl_JOIN_INNER_TD_265_output, cfg_JOIN_INNER_TD_265_cmds_probe, buftmp_h);
    // ************************** Transfer Engine *********************** // 
    transEngine trans_JOIN_INNER_TD_265;
    trans_JOIN_INNER_TD_265.setq(q_h,0);
    trans_JOIN_INNER_TD_265.add(&(cfg_JOIN_INNER_TD_265_cmds_part));
    trans_JOIN_INNER_TD_265.add(&(cfg_JOIN_INNER_TD_265_cmds));
    trans_JOIN_INNER_TD_265.add(&(tbl_SerializeFromObject_TD_317_input));
    transEngine trans_JOIN_INNER_TD_265_out;
    trans_JOIN_INNER_TD_265_out.setq(q_h,0);
    q_h.finish();
    // ****************************** Events **************************** // 
    std::vector<cl::Event> events_h2d_wr_JOIN_INNER_TD_265;
    std::vector<cl::Event> events_d2h_rd_JOIN_INNER_TD_265;
    std::vector<cl::Event> events_JOIN_INNER_TD_265[2];
    events_h2d_wr_JOIN_INNER_TD_265.resize(1);
    events_d2h_rd_JOIN_INNER_TD_265.resize(1);
    events_JOIN_INNER_TD_265[0].resize(2);
    events_JOIN_INNER_TD_265[1].resize(hpTimes_join);
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_265;
    std::vector<cl::Event> prev_events_grp_JOIN_INNER_TD_265;
    // **************************** Operations ************************** // 
    struct timeval tv_r_s, tv_r_e; 
    gettimeofday(&tv_r_s, 0); 

    struct timeval tv_r_Filter_3_315_s, tv_r_Filter_3_315_e;
    gettimeofday(&tv_r_Filter_3_315_s, 0);
    SW_Filter_TD_381(tbl_SerializeFromObject_TD_4325_input, tbl_Filter_TD_381_output);
    gettimeofday(&tv_r_Filter_3_315_e, 0);

    struct timeval tv_r_Aggregate_3_311_s, tv_r_Aggregate_3_311_e;
    gettimeofday(&tv_r_Aggregate_3_311_s, 0);
    SW_Aggregate_TD_3359(tbl_SerializeFromObject_TD_4544_input, tbl_Aggregate_TD_3359_output);
    gettimeofday(&tv_r_Aggregate_3_311_e, 0);

    struct timeval tv_r_JOIN_INNER_2_496_s, tv_r_JOIN_INNER_2_496_e;
    gettimeofday(&tv_r_JOIN_INNER_2_496_s, 0);
    trans_JOIN_INNER_TD_265.add(&(tbl_Filter_TD_381_output));
    trans_JOIN_INNER_TD_265.host2dev(0, &(prev_events_grp_JOIN_INNER_TD_265), &(events_h2d_wr_JOIN_INNER_TD_265[0]));
    events_grp_JOIN_INNER_TD_265.push_back(events_h2d_wr_JOIN_INNER_TD_265[0]);
    krnl_JOIN_INNER_TD_265_part_left.run(0, &(events_grp_JOIN_INNER_TD_265), &(events_JOIN_INNER_TD_265[0][0]));
    krnl_JOIN_INNER_TD_265_part_right.run(0, &(events_grp_JOIN_INNER_TD_265), &(events_JOIN_INNER_TD_265[0][1]));
    for (int i(0); i < hpTimes_join; ++i) {
        krnl_JOIN_INNER_TD_265[i].run(0, &(events_JOIN_INNER_TD_265[0]), &(events_JOIN_INNER_TD_265[1][i]));
    }
    for (int i(0); i < hpTimes_join; ++i) {
        trans_JOIN_INNER_TD_265_out.add(&(tbl_JOIN_INNER_TD_265_output_partition_array[i]));
    }
    trans_JOIN_INNER_TD_265_out.dev2host(0, &(events_JOIN_INNER_TD_265[1]), &(events_d2h_rd_JOIN_INNER_TD_265[0]));
    q_h.flush();
    q_h.finish();
    gettimeofday(&tv_r_JOIN_INNER_2_496_e, 0);

    struct timeval tv_r_JOIN_INNER_1_739_s, tv_r_JOIN_INNER_1_739_e;
    gettimeofday(&tv_r_JOIN_INNER_1_739_s, 0);
    SW_JOIN_INNER_TD_1399(tbl_JOIN_INNER_TD_265_output_partition_array, tbl_Aggregate_TD_3359_output, tbl_JOIN_INNER_TD_1399_output, hpTimes_join);
    gettimeofday(&tv_r_JOIN_INNER_1_739_e, 0);

    struct timeval tv_r_Aggregate_0_651_s, tv_r_Aggregate_0_651_e;
    gettimeofday(&tv_r_Aggregate_0_651_s, 0);
    SW_Aggregate_TD_0585(tbl_JOIN_INNER_TD_1399_output, tbl_Aggregate_TD_0585_output);
    gettimeofday(&tv_r_Aggregate_0_651_e, 0);

std::cout << "CPU TIME: " << (tvdiff(&tv_r_Filter_3_315_s, &tv_r_Filter_3_315_e) + 
tvdiff(&tv_r_Aggregate_3_311_s, &tv_r_Aggregate_3_311_e) + 
tvdiff(&tv_r_JOIN_INNER_1_739_s, &tv_r_JOIN_INNER_1_739_e) + 
tvdiff(&tv_r_Aggregate_0_651_s, &tv_r_Aggregate_0_651_e) + 
1) / 1000.0 << std::endl;

std::cout << "FPGA TIME: " << (tvdiff(&tv_r_JOIN_INNER_2_496_s, &tv_r_JOIN_INNER_2_496_e) + 
1) / 1000.0 << std::endl;

    gettimeofday(&tv_r_e, 0); 
    // **************************** Print Execution Time ************************** // 
    std::cout << std::endl; 
    std::cout << "Filter_3: " << tvdiff(&tv_r_Filter_3_315_s, &tv_r_Filter_3_315_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(((isnotnull(p_brand#261) AND isnotnull(p_container#264)) AND ((p_brand#261 = Brand#21) AND (p_container#264 = WRAP BAG)))) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_4325_input.getNumRow() << " -> tbl_SerializeFromObject_TD_4325_input" 
     << std::endl << "      #COL: 3: " << "ListBuffer(p_partkey#258, p_brand#261, p_container#264)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_381_output.getNumRow() << " -> tbl_Filter_TD_381_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(p_partkey#258)" 
     << std::endl; 

    std::cout << "Aggregate_3: " << tvdiff(&tv_r_Aggregate_3_311_s, &tv_r_Aggregate_3_311_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(l_partkey#78, (0.2 * avg(l_quantity#81)) AS (0.2 * avg(l_quantity))#1645, l_partkey#78 AS l_partkey#78#1696) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_4544_input.getNumRow() << " -> tbl_SerializeFromObject_TD_4544_input" 
     << std::endl << "      #COL: 2: " << "ListBuffer(l_partkey#78, l_quantity#81)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_3359_output.getNumRow() << " -> tbl_Aggregate_TD_3359_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer((0.2 * avg(l_quantity))#1645, l_partkey#78#1696)" 
     << std::endl; 

    std::cout << "JOIN_INNER_2: " << tvdiff(&tv_r_JOIN_INNER_2_496_s, &tv_r_JOIN_INNER_2_496_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer((p_partkey#258 = l_partkey#78)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_317_input.getNumRow() << " -> tbl_SerializeFromObject_TD_317_input" 
     << std::endl << "      #COL: 3: " << "ListBuffer(l_partkey#78, l_quantity#81, l_extendedprice#82)" 
     << std::endl << "      #ROW: " << tbl_Filter_TD_381_output.getNumRow() << " -> tbl_Filter_TD_381_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(p_partkey#258)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_265_output.getNumRow() << " -> tbl_JOIN_INNER_TD_265_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(l_quantity#81, l_extendedprice#82, p_partkey#258)" 
     << std::endl; 

    std::cout << "JOIN_INNER_1: " << tvdiff(&tv_r_JOIN_INNER_1_739_s, &tv_r_JOIN_INNER_1_739_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(((cast(l_quantity#81 as double) < (0.2 * avg(l_quantity))#1645) AND (l_partkey#78#1696 = p_partkey#258))) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_265_output.getNumRow() << " -> tbl_JOIN_INNER_TD_265_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(l_quantity#81, l_extendedprice#82, p_partkey#258)" 
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_3359_output.getNumRow() << " -> tbl_Aggregate_TD_3359_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer((0.2 * avg(l_quantity))#1645, l_partkey#78#1696)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_1399_output.getNumRow() << " -> tbl_JOIN_INNER_TD_1399_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(l_extendedprice#82)" 
     << std::endl; 

    std::cout << "Aggregate_0: " << tvdiff(&tv_r_Aggregate_0_651_s, &tv_r_Aggregate_0_651_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(CheckOverflow((promote_precision(cast(cast(sum(l_extendedprice#82) as decimal(20,0)) as decimal(21,1))) / 7.0), DecimalType(27,6), true) AS avg_yearly#1642) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_1399_output.getNumRow() << " -> tbl_JOIN_INNER_TD_1399_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(l_extendedprice#82)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_0585_output.getNumRow() << " -> tbl_Aggregate_TD_0585_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(avg_yearly#1642)" 
     << std::endl; 

    std::cout << std::endl << " Total execution time: " << tvdiff(&tv_r_s, &tv_r_e) / 1000 << " ms"; 

    std::cout << std::endl << " Spark elapsed time: " << 42.10218 * 1000 << "ms" << std::endl; 
    return 0; 
}
