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

#include "cfgFunc_q22.hpp" 
#include "q22.hpp" 

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
    std::cout << "NOTE:running query #22\n."; 
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
    Table tbl_Sort_TD_0201_output("tbl_Sort_TD_0201_output", 183000000, 3, "");
    tbl_Sort_TD_0201_output.allocateHost();
    Table tbl_Aggregate_TD_1612_output("tbl_Aggregate_TD_1612_output", 183000000, 3, "");
    tbl_Aggregate_TD_1612_output.allocateHost();
    Table tbl_Project_TD_2427_output("tbl_Project_TD_2427_output", 183000000, 2, "");
    tbl_Project_TD_2427_output.allocateHost();
    Table tbl_JOIN_LEFTANTI_TD_3443_output("tbl_JOIN_LEFTANTI_TD_3443_output", 6283, 2, "");
    tbl_JOIN_LEFTANTI_TD_3443_output.allocateHost(1.2, hpTimes_join);
    Table tbl_Filter_TD_4613_output("tbl_Filter_TD_4613_output", 183000000, 3, "");
    tbl_Filter_TD_4613_output.selectOverlayVersion(0);
    tbl_Filter_TD_4613_output.allocateHost();
    Table tbl_Filter_TD_4613_output_partition("tbl_Filter_TD_4613_output_partition", 183000000, 3, "");
    tbl_Filter_TD_4613_output_partition.allocateHost(1.2, hpTimes_join);
    tbl_Filter_TD_4613_output_partition.allocateDevBuffer(context_h, 32);
    Table tbl_Filter_TD_4613_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_Filter_TD_4613_output_partition_array[i] = tbl_Filter_TD_4613_output_partition.createSubTable(i);
    }
    Table tbl_SerializeFromObject_TD_4387_input;
    tbl_SerializeFromObject_TD_4387_input = Table("order", order_n, 1, in_dir);
    tbl_SerializeFromObject_TD_4387_input.selectOverlayVersion(0);
    tbl_SerializeFromObject_TD_4387_input.addCol("o_custkey", 4);
    tbl_SerializeFromObject_TD_4387_input.allocateHost();
    tbl_SerializeFromObject_TD_4387_input.loadHost();
    Table tbl_SerializeFromObject_TD_4387_input_partition("tbl_SerializeFromObject_TD_4387_input_partition", order_n, 1, "");
    tbl_SerializeFromObject_TD_4387_input_partition.allocateHost(1.2, hpTimes_join);
    tbl_SerializeFromObject_TD_4387_input_partition.allocateDevBuffer(context_h, 32);
    Table tbl_SerializeFromObject_TD_4387_input_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_SerializeFromObject_TD_4387_input_partition_array[i] = tbl_SerializeFromObject_TD_4387_input_partition.createSubTable(i);
    }
    Table tbl_Aggregate_TD_5653_output("tbl_Aggregate_TD_5653_output", 183000000, 1, "");
    tbl_Aggregate_TD_5653_output.allocateHost();
    Table tbl_Filter_TD_6793_output("tbl_Filter_TD_6793_output", 183000000, 1, "");
    tbl_Filter_TD_6793_output.allocateHost();
    Table tbl_SerializeFromObject_TD_5488_input;
    tbl_SerializeFromObject_TD_5488_input = Table("customer", customer_n, 3, in_dir);
    tbl_SerializeFromObject_TD_5488_input.addCol("c_custkey", 4);
    tbl_SerializeFromObject_TD_5488_input.addCol("c_phone", TPCH_READ_PHONE_LEN+1);
    tbl_SerializeFromObject_TD_5488_input.addCol("c_acctbal", 4);
    tbl_SerializeFromObject_TD_5488_input.allocateHost();
    tbl_SerializeFromObject_TD_5488_input.loadHost();
    // ********************** Allocate Device Buffer ******************** // 
    tbl_JOIN_LEFTANTI_TD_3443_output.allocateDevBuffer(context_h, 32);
    Table tbl_JOIN_LEFTANTI_TD_3443_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_JOIN_LEFTANTI_TD_3443_output_partition_array[i] = tbl_JOIN_LEFTANTI_TD_3443_output.createSubTable(i);
    }
    tbl_Filter_TD_4613_output.allocateDevBuffer(context_h, 33);
    tbl_SerializeFromObject_TD_4387_input.allocateDevBuffer(context_h, 33);
    // ****************************** Config **************************** // 
    cfgCmd cfg_JOIN_LEFTANTI_TD_3443_cmds;
    cfg_JOIN_LEFTANTI_TD_3443_cmds.allocateHost();
    get_cfg_dat_JOIN_LEFTANTI_TD_3443_gqe_join (cfg_JOIN_LEFTANTI_TD_3443_cmds.cmd);
    cfg_JOIN_LEFTANTI_TD_3443_cmds.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_LEFTANTI_TD_3443_cmds_part;
    cfg_JOIN_LEFTANTI_TD_3443_cmds_part.allocateHost();
    get_cfg_dat_JOIN_LEFTANTI_TD_3443_gqe_join_part (cfg_JOIN_LEFTANTI_TD_3443_cmds_part.cmd);
    cfg_JOIN_LEFTANTI_TD_3443_cmds_part.allocateDevBuffer(context_h, 32);
    // *************************** Kernel Setup ************************* // 
    bufferTmp buftmp_h(context_h, 0); 
    buftmp_h.initBuffer(q_h); 
    std::cout << std::endl; 
    krnlEngine krnl_JOIN_LEFTANTI_TD_3443_part_left;
    krnl_JOIN_LEFTANTI_TD_3443_part_left = krnlEngine(program_h, q_h, context_h, "gqePart");
    krnl_JOIN_LEFTANTI_TD_3443_part_left.setup_hp(512, 0, power_of_hpTimes_join, tbl_SerializeFromObject_TD_4387_input, tbl_SerializeFromObject_TD_4387_input_partition, cfg_JOIN_LEFTANTI_TD_3443_cmds_part);
    krnlEngine krnl_JOIN_LEFTANTI_TD_3443_part_right;
    krnl_JOIN_LEFTANTI_TD_3443_part_right = krnlEngine(program_h, q_h, context_h, "gqePart");
    krnl_JOIN_LEFTANTI_TD_3443_part_right.setup_hp(512, 1, power_of_hpTimes_join, tbl_Filter_TD_4613_output, tbl_Filter_TD_4613_output_partition, cfg_JOIN_LEFTANTI_TD_3443_cmds_part);
    krnlEngine krnl_JOIN_LEFTANTI_TD_3443[hpTimes_join];
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_LEFTANTI_TD_3443[i] = krnlEngine(program_h, q_h, "gqeJoin");
    }
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_LEFTANTI_TD_3443[i].setup(tbl_SerializeFromObject_TD_4387_input_partition_array[i], tbl_Filter_TD_4613_output_partition_array[i], tbl_JOIN_LEFTANTI_TD_3443_output_partition_array[i], cfg_JOIN_LEFTANTI_TD_3443_cmds, buftmp_h);
    }
    krnlEngine krnl_JOIN_LEFTANTI_TD_3443_build;
    krnl_JOIN_LEFTANTI_TD_3443_build = krnlEngine(program_h, q_h, context_h, "gqeKernel", 0);
    krnl_JOIN_LEFTANTI_TD_3443_build.setup(tbl_SerializeFromObject_TD_4387_input, tbl_Filter_TD_4613_output, tbl_JOIN_LEFTANTI_TD_3443_output, cfg_JOIN_LEFTANTI_TD_3443_cmds_build, buftmp_h);
    krnlEngine krnl_JOIN_LEFTANTI_TD_3443_probe;
    krnl_JOIN_LEFTANTI_TD_3443_probe = krnlEngine(program_h, q_h, context_h, "gqeKernel", 1);
    krnl_JOIN_LEFTANTI_TD_3443_probe.setup(tbl_Filter_TD_4613_output, tbl_SerializeFromObject_TD_4387_input, tbl_JOIN_LEFTANTI_TD_3443_output, cfg_JOIN_LEFTANTI_TD_3443_cmds_probe, buftmp_h);
    // ************************** Transfer Engine *********************** // 
    transEngine trans_JOIN_LEFTANTI_TD_3443;
    trans_JOIN_LEFTANTI_TD_3443.setq(q_h,0);
    trans_JOIN_LEFTANTI_TD_3443.add(&(cfg_JOIN_LEFTANTI_TD_3443_cmds_part));
    trans_JOIN_LEFTANTI_TD_3443.add(&(cfg_JOIN_LEFTANTI_TD_3443_cmds));
    trans_JOIN_LEFTANTI_TD_3443.add(&(tbl_SerializeFromObject_TD_4387_input));
    transEngine trans_JOIN_LEFTANTI_TD_3443_out;
    trans_JOIN_LEFTANTI_TD_3443_out.setq(q_h,0);
    q_h.finish();
    // ****************************** Events **************************** // 
    std::vector<cl::Event> events_h2d_wr_JOIN_LEFTANTI_TD_3443;
    std::vector<cl::Event> events_d2h_rd_JOIN_LEFTANTI_TD_3443;
    std::vector<cl::Event> events_JOIN_LEFTANTI_TD_3443[2];
    events_h2d_wr_JOIN_LEFTANTI_TD_3443.resize(1);
    events_d2h_rd_JOIN_LEFTANTI_TD_3443.resize(1);
    events_JOIN_LEFTANTI_TD_3443[0].resize(2);
    events_JOIN_LEFTANTI_TD_3443[1].resize(hpTimes_join);
    std::vector<cl::Event> events_grp_JOIN_LEFTANTI_TD_3443;
    std::vector<cl::Event> prev_events_grp_JOIN_LEFTANTI_TD_3443;
    // **************************** Operations ************************** // 
    struct timeval tv_r_s, tv_r_e; 
    gettimeofday(&tv_r_s, 0); 

    struct timeval tv_r_Filter_6_846_s, tv_r_Filter_6_846_e;
    gettimeofday(&tv_r_Filter_6_846_s, 0);
    SW_Filter_TD_6793(tbl_SerializeFromObject_TD_5488_input, tbl_Filter_TD_6793_output);
    gettimeofday(&tv_r_Filter_6_846_e, 0);

    struct timeval tv_r_Aggregate_5_255_s, tv_r_Aggregate_5_255_e;
    gettimeofday(&tv_r_Aggregate_5_255_s, 0);
    SW_Aggregate_TD_5653(tbl_Filter_TD_6793_output, tbl_Aggregate_TD_5653_output);
    gettimeofday(&tv_r_Aggregate_5_255_e, 0);

    struct timeval tv_r_Filter_4_98_s, tv_r_Filter_4_98_e;
    gettimeofday(&tv_r_Filter_4_98_s, 0);
    SW_Filter_TD_4613(tbl_SerializeFromObject_TD_5488_input, tbl_Aggregate_TD_5653_output, tbl_Filter_TD_4613_output);
    gettimeofday(&tv_r_Filter_4_98_e, 0);

    struct timeval tv_r_JOIN_LEFTANTI_3_887_s, tv_r_JOIN_LEFTANTI_3_887_e;
    gettimeofday(&tv_r_JOIN_LEFTANTI_3_887_s, 0);
    trans_JOIN_LEFTANTI_TD_3443.add(&(tbl_Filter_TD_4613_output));
    trans_JOIN_LEFTANTI_TD_3443.host2dev(0, &(prev_events_grp_JOIN_LEFTANTI_TD_3443), &(events_h2d_wr_JOIN_LEFTANTI_TD_3443[0]));
    events_grp_JOIN_LEFTANTI_TD_3443.push_back(events_h2d_wr_JOIN_LEFTANTI_TD_3443[0]);
    krnl_JOIN_LEFTANTI_TD_3443_part_left.run(0, &(events_grp_JOIN_LEFTANTI_TD_3443), &(events_JOIN_LEFTANTI_TD_3443[0][0]));
    krnl_JOIN_LEFTANTI_TD_3443_part_right.run(0, &(events_grp_JOIN_LEFTANTI_TD_3443), &(events_JOIN_LEFTANTI_TD_3443[0][1]));
    for (int i(0); i < hpTimes_join; ++i) {
        krnl_JOIN_LEFTANTI_TD_3443[i].run(0, &(events_JOIN_LEFTANTI_TD_3443[0]), &(events_JOIN_LEFTANTI_TD_3443[1][i]));
    }
    for (int i(0); i < hpTimes_join; ++i) {
        trans_JOIN_LEFTANTI_TD_3443_out.add(&(tbl_JOIN_LEFTANTI_TD_3443_output_partition_array[i]));
    }
    trans_JOIN_LEFTANTI_TD_3443_out.dev2host(0, &(events_JOIN_LEFTANTI_TD_3443[1]), &(events_d2h_rd_JOIN_LEFTANTI_TD_3443[0]));
    q_h.flush();
    q_h.finish();
    gettimeofday(&tv_r_JOIN_LEFTANTI_3_887_e, 0);

    struct timeval tv_r_Project_2_98_s, tv_r_Project_2_98_e;
    gettimeofday(&tv_r_Project_2_98_s, 0);
    SW_Project_TD_2427(tbl_JOIN_LEFTANTI_TD_3443_output_partition_array, tbl_SerializeFromObject_TD_5488_input, tbl_Project_TD_2427_output, hpTimes_join);
    gettimeofday(&tv_r_Project_2_98_e, 0);

    struct timeval tv_r_Aggregate_1_175_s, tv_r_Aggregate_1_175_e;
    gettimeofday(&tv_r_Aggregate_1_175_s, 0);
    SW_Aggregate_TD_1612(tbl_Project_TD_2427_output, tbl_Aggregate_TD_1612_output);
    gettimeofday(&tv_r_Aggregate_1_175_e, 0);

    struct timeval tv_r_Sort_0_418_s, tv_r_Sort_0_418_e;
    gettimeofday(&tv_r_Sort_0_418_s, 0);
    SW_Sort_TD_0201(tbl_Aggregate_TD_1612_output, tbl_Sort_TD_0201_output);
    gettimeofday(&tv_r_Sort_0_418_e, 0);

std::cout << "CPU TIME: " << (tvdiff(&tv_r_Filter_6_846_s, &tv_r_Filter_6_846_e) + 
tvdiff(&tv_r_Aggregate_5_255_s, &tv_r_Aggregate_5_255_e) + 
tvdiff(&tv_r_Filter_4_98_s, &tv_r_Filter_4_98_e) + 
tvdiff(&tv_r_Project_2_98_s, &tv_r_Project_2_98_e) + 
tvdiff(&tv_r_Aggregate_1_175_s, &tv_r_Aggregate_1_175_e) + 
tvdiff(&tv_r_Sort_0_418_s, &tv_r_Sort_0_418_e) + 
1) / 1000.0 << std::endl;

std::cout << "FPGA TIME: " << (tvdiff(&tv_r_JOIN_LEFTANTI_3_887_s, &tv_r_JOIN_LEFTANTI_3_887_e) + 
1) / 1000.0 << std::endl;

    gettimeofday(&tv_r_e, 0); 
    // **************************** Print Execution Time ************************** // 
    std::cout << std::endl; 
    std::cout << "Filter_6: " << tvdiff(&tv_r_Filter_6_846_s, &tv_r_Filter_6_846_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(((c_acctbal#27 > 0) AND substring(c_phone#26, 1, 2) IN (19,11,16,27,15,22,12))) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_5488_input.getNumRow() << " -> tbl_SerializeFromObject_TD_5488_input" 
     << std::endl << "      #COL: 3: " << "ListBuffer(c_custkey#22, c_phone#26, c_acctbal#27)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_6793_output.getNumRow() << " -> tbl_Filter_TD_6793_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(c_acctbal#27)" 
     << std::endl; 

    std::cout << "Aggregate_5: " << tvdiff(&tv_r_Aggregate_5_255_s, &tv_r_Aggregate_5_255_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(avg(c_acctbal#27) AS avg(c_acctbal)#2048) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_6793_output.getNumRow() << " -> tbl_Filter_TD_6793_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(c_acctbal#27)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_5653_output.getNumRow() << " -> tbl_Aggregate_TD_5653_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(avg(c_acctbal)#2048)" 
     << std::endl; 

    std::cout << "Filter_4: " << tvdiff(&tv_r_Filter_4_98_s, &tv_r_Filter_4_98_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer((substring(c_phone#26, 1, 2) IN (19,11,16,27,15,22,12) AND (cast(c_acctbal#27 as double) > scalar-subquery#2042 []))) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_5488_input.getNumRow() << " -> tbl_SerializeFromObject_TD_5488_input" 
     << std::endl << "      #COL: 3: " << "ListBuffer(c_custkey#22, c_phone#26, c_acctbal#27)" 
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_5653_output.getNumRow() << " -> tbl_Aggregate_TD_5653_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(avg(c_acctbal)#2048)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_4613_output.getNumRow() << " -> tbl_Filter_TD_4613_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(c_custkey#22, c_phone#26, c_acctbal#27)" 
     << std::endl; 

    std::cout << "JOIN_LEFTANTI_3: " << tvdiff(&tv_r_JOIN_LEFTANTI_3_887_s, &tv_r_JOIN_LEFTANTI_3_887_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer((o_custkey#208 = c_custkey#22)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_4613_output.getNumRow() << " -> tbl_Filter_TD_4613_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(c_custkey#22, c_phone#26, c_acctbal#27)" 
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_4387_input.getNumRow() << " -> tbl_SerializeFromObject_TD_4387_input" 
     << std::endl << "      #COL: 1: " << "ListBuffer(o_custkey#208)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_LEFTANTI_TD_3443_output.getNumRow() << " -> tbl_JOIN_LEFTANTI_TD_3443_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(c_phone#26, c_acctbal#27)" 
     << std::endl; 

    std::cout << "Project_2: " << tvdiff(&tv_r_Project_2_98_s, &tv_r_Project_2_98_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(substring(c_phone#26, 1, 2) AS cntrycode#2041) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_LEFTANTI_TD_3443_output.getNumRow() << " -> tbl_JOIN_LEFTANTI_TD_3443_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(c_phone#26, c_acctbal#27)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Project_TD_2427_output.getNumRow() << " -> tbl_Project_TD_2427_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(cntrycode#2041, c_acctbal#27)" 
     << std::endl; 

    std::cout << "Aggregate_1: " << tvdiff(&tv_r_Aggregate_1_175_s, &tv_r_Aggregate_1_175_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(cntrycode#2041, count(1) AS numcust#2044L, sum(c_acctbal#27) AS totacctbal#2045L) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_Project_TD_2427_output.getNumRow() << " -> tbl_Project_TD_2427_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(cntrycode#2041, c_acctbal#27)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_1612_output.getNumRow() << " -> tbl_Aggregate_TD_1612_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(cntrycode#2041, numcust#2044L, totacctbal#2045L)" 
     << std::endl; 

    std::cout << "Sort_0: " << tvdiff(&tv_r_Sort_0_418_s, &tv_r_Sort_0_418_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(cntrycode#2041 ASC NULLS FIRST) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_1612_output.getNumRow() << " -> tbl_Aggregate_TD_1612_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(cntrycode#2041, numcust#2044L, totacctbal#2045L)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Sort_TD_0201_output.getNumRow() << " -> tbl_Sort_TD_0201_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(cntrycode#2041, numcust#2044L, totacctbal#2045L)" 
     << std::endl; 

    std::cout << std::endl << " Total execution time: " << tvdiff(&tv_r_s, &tv_r_e) / 1000 << " ms"; 

    std::cout << std::endl << " Spark elapsed time: " << 4.4909563 * 1000 << "ms" << std::endl; 
    return 0; 
}
