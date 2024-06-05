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

#include "cfgFunc_q4.hpp" 
#include "q4.hpp" 

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
    std::cout << "NOTE:running query #4\n."; 
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
    Table tbl_Sort_TD_0912_output("tbl_Sort_TD_0912_output", 183000000, 2, "");
    tbl_Sort_TD_0912_output.allocateHost();
    Table tbl_Aggregate_TD_1491_output("tbl_Aggregate_TD_1491_output", 183000000, 2, "");
    tbl_Aggregate_TD_1491_output.allocateHost();
    Table tbl_JOIN_LEFTSEMI_TD_2502_output("tbl_JOIN_LEFTSEMI_TD_2502_output", 53000, 1, "");
    tbl_JOIN_LEFTSEMI_TD_2502_output.allocateHost(1.2, hpTimes_join);
    Table tbl_Filter_TD_3683_output("tbl_Filter_TD_3683_output", 183000000, 2, "");
    tbl_Filter_TD_3683_output.selectOverlayVersion(0);
    tbl_Filter_TD_3683_output.allocateHost();
    Table tbl_Filter_TD_3683_output_partition("tbl_Filter_TD_3683_output_partition", 183000000, 2, "");
    tbl_Filter_TD_3683_output_partition.allocateHost(1.2, hpTimes_join);
    tbl_Filter_TD_3683_output_partition.allocateDevBuffer(context_h, 32);
    Table tbl_Filter_TD_3683_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_Filter_TD_3683_output_partition_array[i] = tbl_Filter_TD_3683_output_partition.createSubTable(i);
    }
    Table tbl_Filter_TD_3823_output("tbl_Filter_TD_3823_output", 183000000, 1, "");
    tbl_Filter_TD_3823_output.selectOverlayVersion(0);
    tbl_Filter_TD_3823_output.allocateHost();
    Table tbl_Filter_TD_3823_output_partition("tbl_Filter_TD_3823_output_partition", 183000000, 1, "");
    tbl_Filter_TD_3823_output_partition.allocateHost(1.2, hpTimes_join);
    tbl_Filter_TD_3823_output_partition.allocateDevBuffer(context_h, 32);
    Table tbl_Filter_TD_3823_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_Filter_TD_3823_output_partition_array[i] = tbl_Filter_TD_3823_output_partition.createSubTable(i);
    }
    Table tbl_SerializeFromObject_TD_4406_input_stringRowIDSubstitute;
    tbl_SerializeFromObject_TD_4406_input_stringRowIDSubstitute = Table("order", order_n, 3, in_dir);
    tbl_SerializeFromObject_TD_4406_input_stringRowIDSubstitute.addCol("o_orderkey", 4);
    tbl_SerializeFromObject_TD_4406_input_stringRowIDSubstitute.addCol("o_orderpriority", 4, 1, 0);
    tbl_SerializeFromObject_TD_4406_input_stringRowIDSubstitute.addCol("o_orderdate", 4);
    tbl_SerializeFromObject_TD_4406_input_stringRowIDSubstitute.allocateHost();
    tbl_SerializeFromObject_TD_4406_input_stringRowIDSubstitute.loadHost();
    Table tbl_SerializeFromObject_TD_4406_input;
    tbl_SerializeFromObject_TD_4406_input = Table("order", order_n, 3, in_dir);
    tbl_SerializeFromObject_TD_4406_input.addCol("o_orderkey", 4);
    tbl_SerializeFromObject_TD_4406_input.addCol("o_orderpriority", TPCH_READ_MAXAGG_LEN+1);
    tbl_SerializeFromObject_TD_4406_input.addCol("o_orderdate", 4);
    tbl_SerializeFromObject_TD_4406_input.allocateHost();
    tbl_SerializeFromObject_TD_4406_input.loadHost();
    Table tbl_SerializeFromObject_TD_4648_input;
    tbl_SerializeFromObject_TD_4648_input = Table("lineitem", lineitem_n, 3, in_dir);
    tbl_SerializeFromObject_TD_4648_input.addCol("l_orderkey", 4);
    tbl_SerializeFromObject_TD_4648_input.addCol("l_commitdate", 4);
    tbl_SerializeFromObject_TD_4648_input.addCol("l_receiptdate", 4);
    tbl_SerializeFromObject_TD_4648_input.allocateHost();
    tbl_SerializeFromObject_TD_4648_input.loadHost();
    // ********************** Allocate Device Buffer ******************** // 
    tbl_JOIN_LEFTSEMI_TD_2502_output.allocateDevBuffer(context_h, 32);
    Table tbl_JOIN_LEFTSEMI_TD_2502_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_JOIN_LEFTSEMI_TD_2502_output_partition_array[i] = tbl_JOIN_LEFTSEMI_TD_2502_output.createSubTable(i);
    }
    tbl_Filter_TD_3683_output.allocateDevBuffer(context_h, 33);
    tbl_Filter_TD_3823_output.allocateDevBuffer(context_h, 33);
    // ****************************** Config **************************** // 
    cfgCmd cfg_JOIN_LEFTSEMI_TD_2502_cmds;
    cfg_JOIN_LEFTSEMI_TD_2502_cmds.allocateHost();
    get_cfg_dat_JOIN_LEFTSEMI_TD_2502_gqe_join (cfg_JOIN_LEFTSEMI_TD_2502_cmds.cmd);
    cfg_JOIN_LEFTSEMI_TD_2502_cmds.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_LEFTSEMI_TD_2502_cmds_part;
    cfg_JOIN_LEFTSEMI_TD_2502_cmds_part.allocateHost();
    get_cfg_dat_JOIN_LEFTSEMI_TD_2502_gqe_join_part (cfg_JOIN_LEFTSEMI_TD_2502_cmds_part.cmd);
    cfg_JOIN_LEFTSEMI_TD_2502_cmds_part.allocateDevBuffer(context_h, 32);
    // *************************** Kernel Setup ************************* // 
    bufferTmp buftmp_h(context_h, 0); 
    buftmp_h.initBuffer(q_h); 
    std::cout << std::endl; 
    krnlEngine krnl_JOIN_LEFTSEMI_TD_2502_part_left;
    krnl_JOIN_LEFTSEMI_TD_2502_part_left = krnlEngine(program_h, q_h, context_h, "gqePart");
    krnl_JOIN_LEFTSEMI_TD_2502_part_left.setup_hp(512, 0, power_of_hpTimes_join, tbl_Filter_TD_3823_output, tbl_Filter_TD_3823_output_partition, cfg_JOIN_LEFTSEMI_TD_2502_cmds_part);
    krnlEngine krnl_JOIN_LEFTSEMI_TD_2502_part_right;
    krnl_JOIN_LEFTSEMI_TD_2502_part_right = krnlEngine(program_h, q_h, context_h, "gqePart");
    krnl_JOIN_LEFTSEMI_TD_2502_part_right.setup_hp(512, 1, power_of_hpTimes_join, tbl_Filter_TD_3683_output, tbl_Filter_TD_3683_output_partition, cfg_JOIN_LEFTSEMI_TD_2502_cmds_part);
    krnlEngine krnl_JOIN_LEFTSEMI_TD_2502[hpTimes_join];
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_LEFTSEMI_TD_2502[i] = krnlEngine(program_h, q_h, "gqeJoin");
    }
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_LEFTSEMI_TD_2502[i].setup(tbl_Filter_TD_3823_output_partition_array[i], tbl_Filter_TD_3683_output_partition_array[i], tbl_JOIN_LEFTSEMI_TD_2502_output_partition_array[i], cfg_JOIN_LEFTSEMI_TD_2502_cmds, buftmp_h);
    }
    krnlEngine krnl_JOIN_LEFTSEMI_TD_2502_build;
    krnl_JOIN_LEFTSEMI_TD_2502_build = krnlEngine(program_h, q_h, context_h, "gqeKernel", 0);
    krnl_JOIN_LEFTSEMI_TD_2502_build.setup(tbl_Filter_TD_3823_output, tbl_Filter_TD_3683_output, tbl_JOIN_LEFTSEMI_TD_2502_output, cfg_JOIN_LEFTSEMI_TD_2502_cmds_build, buftmp_h);
    krnlEngine krnl_JOIN_LEFTSEMI_TD_2502_probe;
    krnl_JOIN_LEFTSEMI_TD_2502_probe = krnlEngine(program_h, q_h, context_h, "gqeKernel", 1);
    krnl_JOIN_LEFTSEMI_TD_2502_probe.setup(tbl_Filter_TD_3683_output, tbl_Filter_TD_3823_output, tbl_JOIN_LEFTSEMI_TD_2502_output, cfg_JOIN_LEFTSEMI_TD_2502_cmds_probe, buftmp_h);
    // ************************** Transfer Engine *********************** // 
    transEngine trans_JOIN_LEFTSEMI_TD_2502;
    trans_JOIN_LEFTSEMI_TD_2502.setq(q_h,0);
    trans_JOIN_LEFTSEMI_TD_2502.add(&(cfg_JOIN_LEFTSEMI_TD_2502_cmds));
    transEngine trans_JOIN_LEFTSEMI_TD_2502_out;
    trans_JOIN_LEFTSEMI_TD_2502_out.setq(q_h,0);
    q_h.finish();
    // ****************************** Events **************************** // 
    std::vector<cl::Event> events_h2d_wr_JOIN_LEFTSEMI_TD_2502;
    std::vector<cl::Event> events_d2h_rd_JOIN_LEFTSEMI_TD_2502;
    std::vector<cl::Event> events_JOIN_LEFTSEMI_TD_2502[2];
    events_h2d_wr_JOIN_LEFTSEMI_TD_2502.resize(1);
    events_d2h_rd_JOIN_LEFTSEMI_TD_2502.resize(1);
    events_JOIN_LEFTSEMI_TD_2502[0].resize(2);
    events_JOIN_LEFTSEMI_TD_2502[1].resize(hpTimes_join);
    std::vector<cl::Event> events_grp_JOIN_LEFTSEMI_TD_2502;
    std::vector<cl::Event> prev_events_grp_JOIN_LEFTSEMI_TD_2502;
    // **************************** Operations ************************** // 
    struct timeval tv_r_s, tv_r_e; 
    gettimeofday(&tv_r_s, 0); 

    struct timeval tv_r_Filter_3_321_s, tv_r_Filter_3_321_e;
    gettimeofday(&tv_r_Filter_3_321_s, 0);
    SW_Filter_TD_3823(tbl_SerializeFromObject_TD_4648_input, tbl_Filter_TD_3823_output);
    gettimeofday(&tv_r_Filter_3_321_e, 0);

    struct timeval tv_r_Filter_3_125_s, tv_r_Filter_3_125_e;
    gettimeofday(&tv_r_Filter_3_125_s, 0);
    SW_Filter_TD_3683(tbl_SerializeFromObject_TD_4406_input_stringRowIDSubstitute, tbl_Filter_TD_3683_output);
    gettimeofday(&tv_r_Filter_3_125_e, 0);

    struct timeval tv_r_JOIN_LEFTSEMI_2_134_s, tv_r_JOIN_LEFTSEMI_2_134_e;
    gettimeofday(&tv_r_JOIN_LEFTSEMI_2_134_s, 0);
    trans_JOIN_LEFTSEMI_TD_2502.add(&(tbl_Filter_TD_3683_output));
    trans_JOIN_LEFTSEMI_TD_2502.add(&(tbl_Filter_TD_3823_output));
    trans_JOIN_LEFTSEMI_TD_2502.host2dev(0, &(prev_events_grp_JOIN_LEFTSEMI_TD_2502), &(events_h2d_wr_JOIN_LEFTSEMI_TD_2502[0]));
    events_grp_JOIN_LEFTSEMI_TD_2502.push_back(events_h2d_wr_JOIN_LEFTSEMI_TD_2502[0]);
    krnl_JOIN_LEFTSEMI_TD_2502_part_left.run(0, &(events_grp_JOIN_LEFTSEMI_TD_2502), &(events_JOIN_LEFTSEMI_TD_2502[0][0]));
    krnl_JOIN_LEFTSEMI_TD_2502_part_right.run(0, &(events_grp_JOIN_LEFTSEMI_TD_2502), &(events_JOIN_LEFTSEMI_TD_2502[0][1]));
    for (int i(0); i < hpTimes_join; ++i) {
        krnl_JOIN_LEFTSEMI_TD_2502[i].run(0, &(events_JOIN_LEFTSEMI_TD_2502[0]), &(events_JOIN_LEFTSEMI_TD_2502[1][i]));
    }
    for (int i(0); i < hpTimes_join; ++i) {
        trans_JOIN_LEFTSEMI_TD_2502_out.add(&(tbl_JOIN_LEFTSEMI_TD_2502_output_partition_array[i]));
    }
    trans_JOIN_LEFTSEMI_TD_2502_out.dev2host(0, &(events_JOIN_LEFTSEMI_TD_2502[1]), &(events_d2h_rd_JOIN_LEFTSEMI_TD_2502[0]));
    q_h.flush();
    q_h.finish();
    gettimeofday(&tv_r_JOIN_LEFTSEMI_2_134_e, 0);

    struct timeval tv_r_Aggregate_1_946_s, tv_r_Aggregate_1_946_e;
    gettimeofday(&tv_r_Aggregate_1_946_s, 0);
    SW_Aggregate_TD_1491(tbl_JOIN_LEFTSEMI_TD_2502_output_partition_array, tbl_SerializeFromObject_TD_4406_input, tbl_SerializeFromObject_TD_4648_input, tbl_Aggregate_TD_1491_output, hpTimes_join);
    gettimeofday(&tv_r_Aggregate_1_946_e, 0);

    struct timeval tv_r_Sort_0_559_s, tv_r_Sort_0_559_e;
    gettimeofday(&tv_r_Sort_0_559_s, 0);
    SW_Sort_TD_0912(tbl_Aggregate_TD_1491_output, tbl_Sort_TD_0912_output);
    gettimeofday(&tv_r_Sort_0_559_e, 0);

std::cout << "CPU TIME: " << (tvdiff(&tv_r_Filter_3_321_s, &tv_r_Filter_3_321_e) + 
tvdiff(&tv_r_Filter_3_125_s, &tv_r_Filter_3_125_e) + 
tvdiff(&tv_r_Aggregate_1_946_s, &tv_r_Aggregate_1_946_e) + 
tvdiff(&tv_r_Sort_0_559_s, &tv_r_Sort_0_559_e) + 
1) / 1000.0 << std::endl;

std::cout << "FPGA TIME: " << (tvdiff(&tv_r_JOIN_LEFTSEMI_2_134_s, &tv_r_JOIN_LEFTSEMI_2_134_e) + 
1) / 1000.0 << std::endl;

    gettimeofday(&tv_r_e, 0); 
    // **************************** Print Execution Time ************************** // 
    std::cout << std::endl; 
    std::cout << "Filter_3: " << tvdiff(&tv_r_Filter_3_321_s, &tv_r_Filter_3_321_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer((l_commitdate#88 < l_receiptdate#89)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_4648_input.getNumRow() << " -> tbl_SerializeFromObject_TD_4648_input" 
     << std::endl << "      #COL: 3: " << "ListBuffer(l_orderkey#77, l_commitdate#88, l_receiptdate#89)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_3823_output.getNumRow() << " -> tbl_Filter_TD_3823_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(l_orderkey#77)" 
     << std::endl; 

    std::cout << "Filter_3: " << tvdiff(&tv_r_Filter_3_125_s, &tv_r_Filter_3_125_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(((o_orderdate#211 >= 19930701) AND (o_orderdate#211 < 19931001))) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_4406_input_stringRowIDSubstitute.getNumRow() << " -> tbl_SerializeFromObject_TD_4406_input_stringRowIDSubstitute" 
     << std::endl << "      #COL: 3: " << "ListBuffer(o_orderkey#207, o_orderpriority#212, o_orderdate#211)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_3683_output.getNumRow() << " -> tbl_Filter_TD_3683_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(o_orderkey#207, o_orderpriority#212)" 
     << std::endl; 

    std::cout << "JOIN_LEFTSEMI_2: " << tvdiff(&tv_r_JOIN_LEFTSEMI_2_134_s, &tv_r_JOIN_LEFTSEMI_2_134_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer((l_orderkey#77 = o_orderkey#207)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_3683_output.getNumRow() << " -> tbl_Filter_TD_3683_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(o_orderkey#207, o_orderpriority#212)" 
     << std::endl << "      #ROW: " << tbl_Filter_TD_3823_output.getNumRow() << " -> tbl_Filter_TD_3823_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(l_orderkey#77)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_LEFTSEMI_TD_2502_output.getNumRow() << " -> tbl_JOIN_LEFTSEMI_TD_2502_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(o_orderpriority#212)" 
     << std::endl; 

    std::cout << "Aggregate_1: " << tvdiff(&tv_r_Aggregate_1_946_s, &tv_r_Aggregate_1_946_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(o_orderpriority#212, count(1) AS order_count#747L) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_LEFTSEMI_TD_2502_output.getNumRow() << " -> tbl_JOIN_LEFTSEMI_TD_2502_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(o_orderpriority#212)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_1491_output.getNumRow() << " -> tbl_Aggregate_TD_1491_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(o_orderpriority#212, order_count#747L)" 
     << std::endl; 

    std::cout << "Sort_0: " << tvdiff(&tv_r_Sort_0_559_s, &tv_r_Sort_0_559_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(o_orderpriority#212 ASC NULLS FIRST) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_1491_output.getNumRow() << " -> tbl_Aggregate_TD_1491_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(o_orderpriority#212, order_count#747L)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Sort_TD_0912_output.getNumRow() << " -> tbl_Sort_TD_0912_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(o_orderpriority#212, order_count#747L)" 
     << std::endl; 

    std::cout << std::endl << " Total execution time: " << tvdiff(&tv_r_s, &tv_r_e) / 1000 << " ms"; 

    std::cout << std::endl << " Spark elapsed time: " << 24.64576 * 1000 << "ms" << std::endl; 
    return 0; 
}
