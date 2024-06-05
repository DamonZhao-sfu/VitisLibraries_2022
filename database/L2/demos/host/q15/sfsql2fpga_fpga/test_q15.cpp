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

#include "cfgFunc_q15.hpp" 
#include "q15.hpp" 

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
    std::cout << "NOTE:running query #15\n."; 
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
    Table tbl_Sort_TD_0131_output("tbl_Sort_TD_0131_output", 183000000, 5, "");
    tbl_Sort_TD_0131_output.allocateHost();
    Table tbl_JOIN_INNER_TD_1396_output("tbl_JOIN_INNER_TD_1396_output", 183000000, 5, "");
    tbl_JOIN_INNER_TD_1396_output.allocateHost();
    Table tbl_SerializeFromObject_TD_2209_input_stringRowIDSubstitute;
    tbl_SerializeFromObject_TD_2209_input_stringRowIDSubstitute = Table("supplier", supplier_n, 4, in_dir);
    tbl_SerializeFromObject_TD_2209_input_stringRowIDSubstitute.addCol("s_suppkey", 4);
    tbl_SerializeFromObject_TD_2209_input_stringRowIDSubstitute.addCol("s_name", 4, 1, 0);
    tbl_SerializeFromObject_TD_2209_input_stringRowIDSubstitute.addCol("s_address", 4, 1, 0);
    tbl_SerializeFromObject_TD_2209_input_stringRowIDSubstitute.addCol("s_phone", 4, 1, 0);
    tbl_SerializeFromObject_TD_2209_input_stringRowIDSubstitute.allocateHost();
    tbl_SerializeFromObject_TD_2209_input_stringRowIDSubstitute.loadHost();
    Table tbl_SerializeFromObject_TD_2209_input;
    tbl_SerializeFromObject_TD_2209_input = Table("supplier", supplier_n, 4, in_dir);
    tbl_SerializeFromObject_TD_2209_input.addCol("s_suppkey", 4);
    tbl_SerializeFromObject_TD_2209_input.addCol("s_name", TPCH_READ_S_NAME_LEN+1);
    tbl_SerializeFromObject_TD_2209_input.addCol("s_address", TPCH_READ_S_ADDR_MAX+1);
    tbl_SerializeFromObject_TD_2209_input.addCol("s_phone", TPCH_READ_PHONE_LEN+1);
    tbl_SerializeFromObject_TD_2209_input.allocateHost();
    tbl_SerializeFromObject_TD_2209_input.loadHost();
    Table tbl_Filter_TD_2745_output("tbl_Filter_TD_2745_output", 183000000, 2, "");
    tbl_Filter_TD_2745_output.allocateHost();
    Table tbl_Aggregate_TD_4400_output_preprocess("tbl_Aggregate_TD_4400_output_preprocess", 20000, 16, "");
    tbl_Aggregate_TD_4400_output_preprocess.allocateHost(1.2, hpTimes_aggr);
    Table tbl_Aggregate_TD_4400_output("tbl_Aggregate_TD_4400_output", 20000, 2, "");
    Table tbl_Aggregate_TD_4855_output("tbl_Aggregate_TD_4855_output", 183000000, 1, "");
    tbl_Aggregate_TD_4855_output.allocateHost();
    Table tbl_Aggregate_TD_510_output("tbl_Aggregate_TD_510_output", 183000000, 1, "");
    tbl_Aggregate_TD_510_output.allocateHost();
    Table tbl_Filter_TD_6803_output("tbl_Filter_TD_6803_output", 183000000, 3, "");
    tbl_Filter_TD_6803_output.selectOverlayVersion(0);
    tbl_Filter_TD_6803_output.allocateHost();
    Table tbl_Filter_TD_6803_output_partition("tbl_Filter_TD_6803_output_partition", 183000000, 3, "");
    tbl_Filter_TD_6803_output_partition.allocateHost(1.2, hpTimes_aggr);
    tbl_Filter_TD_6803_output_partition.allocateDevBuffer(context_a, 32);
    Table tbl_Filter_TD_6803_output_partition_array[hpTimes_aggr];
    for (int i(0); i < hpTimes_aggr; ++i) {
        tbl_Filter_TD_6803_output_partition_array[i] = tbl_Filter_TD_6803_output_partition.createSubTable(i);
    }
    Table tbl_SerializeFromObject_TD_7212_input;
    tbl_SerializeFromObject_TD_7212_input = Table("lineitem", lineitem_n, 4, in_dir);
    tbl_SerializeFromObject_TD_7212_input.addCol("l_suppkey", 4);
    tbl_SerializeFromObject_TD_7212_input.addCol("l_extendedprice", 4);
    tbl_SerializeFromObject_TD_7212_input.addCol("l_discount", 4);
    tbl_SerializeFromObject_TD_7212_input.addCol("l_shipdate", 4);
    tbl_SerializeFromObject_TD_7212_input.allocateHost();
    tbl_SerializeFromObject_TD_7212_input.loadHost();
    // ********************** Allocate Device Buffer ******************** // 
    tbl_Aggregate_TD_4400_output_preprocess.allocateDevBuffer(context_a, 33);
    Table tbl_Aggregate_TD_4400_output_partition_array[hpTimes_aggr];
    for (int i(0); i < hpTimes_aggr; ++i) {
        tbl_Aggregate_TD_4400_output_partition_array[i] = tbl_Aggregate_TD_4400_output_preprocess.createSubTable(i);
    }
    tbl_Filter_TD_6803_output.allocateDevBuffer(context_a, 33);
    // ****************************** Config **************************** // 
    AggrCfgCmd cfg_Aggregate_TD_4400_cmds;
    cfg_Aggregate_TD_4400_cmds.allocateHost();
    get_cfg_dat_Aggregate_TD_4400_gqe_aggr(cfg_Aggregate_TD_4400_cmds.cmd);
    cfg_Aggregate_TD_4400_cmds.allocateDevBuffer(context_a, 32);
    AggrCfgCmd cfg_Aggregate_TD_4400_cmds_out;
    cfg_Aggregate_TD_4400_cmds_out.allocateHost();
    cfg_Aggregate_TD_4400_cmds_out.allocateDevBuffer(context_a, 33);
    // *************************** Kernel Setup ************************* // 
    AggrBufferTmp buftmp_a(context_a, 0); 
    buftmp_a.BufferInitial(q_a); 
    std::cout << std::endl; 
    AggrKrnlEngine krnl_Aggregate_TD_4400_part;
    krnl_Aggregate_TD_4400_part = AggrKrnlEngine(program_a, q_a, "gqePart");
    krnl_Aggregate_TD_4400_part.setup_hp(512, 0, power_of_hpTimes_aggr, tbl_Filter_TD_6803_output, tbl_Filter_TD_6803_output_partition, cfg_Aggregate_TD_4400_cmds_part);
    AggrKrnlEngine krnl_Aggregate_TD_4400[hpTimes_aggr];
    for (int i = 0; i < hpTimes_aggr; i++) {
        krnl_Aggregate_TD_4400[i] = AggrKrnlEngine(program_a, q_a, "gqeAggr");
    }
    for (int i = 0; i < hpTimes_aggr; i++) {
        krnl_Aggregate_TD_4400[i].setup(tbl_Filter_TD_6803_output_partition_array[i], tbl_Aggregate_TD_4400_output_partition_array[i], cfg_Aggregate_TD_4400_cmds, cfg_Aggregate_TD_4400_cmds_out, buftmp_a);
    }
    // ************************** Transfer Engine *********************** // 
    transEngine trans_Aggregate_TD_4400;
    trans_Aggregate_TD_4400.setq(q_a, 0);
    trans_Aggregate_TD_4400.add(&(cfg_Aggregate_TD_4400_cmds_part));
    trans_Aggregate_TD_4400.add(&(cfg_Aggregate_TD_4400_cmds));
    trans_Aggregate_TD_4400.add(&(cfg_Aggregate_TD_4400_cmds_out));
    transEngine trans_Aggregate_TD_4400_out;
    trans_Aggregate_TD_4400_out.setq(q_a, 0);
    q_a.finish();
    // ****************************** Events **************************** // 
    std::vector<cl::Event> events_h2d_wr_Aggregate_TD_4400;
    std::vector<cl::Event> events_d2h_rd_Aggregate_TD_4400;
    std::vector<cl::Event> events_Aggregate_TD_4400[2];
    events_h2d_wr_Aggregate_TD_4400.resize(1);
    events_d2h_rd_Aggregate_TD_4400.resize(1);
    events_Aggregate_TD_4400[0].resize(1);
    events_Aggregate_TD_4400[1].resize(hpTimes_aggr);
    std::vector<cl::Event> events_grp_Aggregate_TD_4400;
    std::vector<cl::Event> prev_events_grp_Aggregate_TD_4400;
    // **************************** Operations ************************** // 
    struct timeval tv_r_s, tv_r_e; 
    gettimeofday(&tv_r_s, 0); 

    struct timeval tv_r_Filter_6_749_s, tv_r_Filter_6_749_e;
    gettimeofday(&tv_r_Filter_6_749_s, 0);
    SW_Filter_TD_6803(tbl_SerializeFromObject_TD_7212_input, tbl_Filter_TD_6803_output);
    gettimeofday(&tv_r_Filter_6_749_e, 0);

    struct timeval tv_r_Aggregate_5_336_s, tv_r_Aggregate_5_336_e;
    gettimeofday(&tv_r_Aggregate_5_336_s, 0);
    SW_Aggregate_TD_510(tbl_Filter_TD_6803_output, tbl_Aggregate_TD_510_output);
    gettimeofday(&tv_r_Aggregate_5_336_e, 0);

    struct timeval tv_r_Aggregate_4_646_s, tv_r_Aggregate_4_646_e;
    gettimeofday(&tv_r_Aggregate_4_646_s, 0);
    SW_Aggregate_TD_4855(tbl_Aggregate_TD_510_output, tbl_Aggregate_TD_4855_output);
    gettimeofday(&tv_r_Aggregate_4_646_e, 0);

    struct timeval tv_r_Aggregate_4_724_s, tv_r_Aggregate_4_724_e;
    gettimeofday(&tv_r_Aggregate_4_724_s, 0);
    trans_Aggregate_TD_4400.add(&(tbl_Filter_TD_6803_output));
    trans_Aggregate_TD_4400.host2dev(0, &(prev_events_grp_Aggregate_TD_4400), &(events_h2d_wr_Aggregate_TD_4400[0]));
    events_grp_Aggregate_TD_4400.push_back(events_h2d_wr_Aggregate_TD_4400[0]);
    krnl_Aggregate_TD_4400_part.run(0, &(events_grp_Aggregate_TD_4400), &(events_Aggregate_TD_4400[0][0]));
    for (int i(0); i < hpTimes_aggr; ++i) {
        krnl_Aggregate_TD_4400[i].run(0, &(events_Aggregate_TD_4400[0]), &(events_Aggregate_TD_4400[1][i]));
    }
    
    for (int i(0); i < hpTimes_aggr; ++i) {
        trans_Aggregate_TD_4400_out.add(&(tbl_Aggregate_TD_4400_output_partition_array[i]));
    }
    trans_Aggregate_TD_4400_out.dev2host(0, &(events_Aggregate_TD_4400[1]), &(events_d2h_rd_Aggregate_TD_4400[0]));
    q_a.flush();
    q_a.finish();
    
    SW_Aggregate_TD_4400_consolidate(tbl_Aggregate_TD_4400_output_partition_array, tbl_Aggregate_TD_4400_output, hpTimes_aggr);
    gettimeofday(&tv_r_Aggregate_4_724_e, 0);

    struct timeval tv_r_Filter_2_620_s, tv_r_Filter_2_620_e;
    gettimeofday(&tv_r_Filter_2_620_s, 0);
    SW_Filter_TD_2745(tbl_Aggregate_TD_4400_output, tbl_Aggregate_TD_4855_output, tbl_Filter_TD_2745_output);
    gettimeofday(&tv_r_Filter_2_620_e, 0);

    struct timeval tv_r_JOIN_INNER_1_646_s, tv_r_JOIN_INNER_1_646_e;
    gettimeofday(&tv_r_JOIN_INNER_1_646_s, 0);
    SW_JOIN_INNER_TD_1396(tbl_SerializeFromObject_TD_2209_input_stringRowIDSubstitute, tbl_Filter_TD_2745_output, tbl_JOIN_INNER_TD_1396_output);
    gettimeofday(&tv_r_JOIN_INNER_1_646_e, 0);

    struct timeval tv_r_Sort_0_77_s, tv_r_Sort_0_77_e;
    gettimeofday(&tv_r_Sort_0_77_s, 0);
    SW_Sort_TD_0131(tbl_JOIN_INNER_TD_1396_output, tbl_SerializeFromObject_TD_2209_input, tbl_Sort_TD_0131_output);
    gettimeofday(&tv_r_Sort_0_77_e, 0);

std::cout << "CPU TIME: " << (tvdiff(&tv_r_Filter_6_749_s, &tv_r_Filter_6_749_e) + 
tvdiff(&tv_r_Aggregate_5_336_s, &tv_r_Aggregate_5_336_e) + 
tvdiff(&tv_r_Aggregate_4_646_s, &tv_r_Aggregate_4_646_e) + 
tvdiff(&tv_r_Filter_2_620_s, &tv_r_Filter_2_620_e) + 
tvdiff(&tv_r_JOIN_INNER_1_646_s, &tv_r_JOIN_INNER_1_646_e) + 
tvdiff(&tv_r_Sort_0_77_s, &tv_r_Sort_0_77_e) + 
1) / 1000.0 << std::endl;

std::cout << "FPGA TIME: " << (tvdiff(&tv_r_Aggregate_4_724_s, &tv_r_Aggregate_4_724_e) + 
1) / 1000.0 << std::endl;

    gettimeofday(&tv_r_e, 0); 
    // **************************** Print Execution Time ************************** // 
    std::cout << std::endl; 
    std::cout << "Filter_6: " << tvdiff(&tv_r_Filter_6_749_s, &tv_r_Filter_6_749_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(((l_shipdate#87 >= 19960101) AND (l_shipdate#87 < 19960401))) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_7212_input.getNumRow() << " -> tbl_SerializeFromObject_TD_7212_input" 
     << std::endl << "      #COL: 4: " << "ListBuffer(l_suppkey#79, l_extendedprice#82, l_discount#83, l_shipdate#87)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_6803_output.getNumRow() << " -> tbl_Filter_TD_6803_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(l_suppkey#79, l_extendedprice#82, l_discount#83)" 
     << std::endl; 

    std::cout << "Aggregate_5: " << tvdiff(&tv_r_Aggregate_5_336_s, &tv_r_Aggregate_5_336_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(l_suppkey#79, sum((l_extendedprice#82 * (1 - l_discount#83))) AS total_revenue#1463L) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_6803_output.getNumRow() << " -> tbl_Filter_TD_6803_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(l_suppkey#79, l_extendedprice#82, l_discount#83)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_510_output.getNumRow() << " -> tbl_Aggregate_TD_510_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(total_revenue#1463L)" 
     << std::endl; 

    std::cout << "Aggregate_4: " << tvdiff(&tv_r_Aggregate_4_646_s, &tv_r_Aggregate_4_646_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(max(total_revenue#1463L) AS max(total_revenue)#1465L) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_510_output.getNumRow() << " -> tbl_Aggregate_TD_510_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(total_revenue#1463L)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_4855_output.getNumRow() << " -> tbl_Aggregate_TD_4855_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(max(total_revenue)#1465L)" 
     << std::endl; 

    std::cout << "Aggregate_4: " << tvdiff(&tv_r_Aggregate_4_724_s, &tv_r_Aggregate_4_724_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer(l_suppkey#79, sum((l_extendedprice#82 * (1 - l_discount#83))) AS sum((l_extendedprice * (1 - l_discount)))#1461L) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_6803_output.getNumRow() << " -> tbl_Filter_TD_6803_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(l_suppkey#79, l_extendedprice#82, l_discount#83)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_4400_output.getNumRow() << " -> tbl_Aggregate_TD_4400_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(l_suppkey#79, sum((l_extendedprice * (1 - l_discount)))#1461L)" 
     << std::endl; 

    std::cout << "Filter_2: " << tvdiff(&tv_r_Filter_2_620_s, &tv_r_Filter_2_620_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer((isnotnull(sum((l_extendedprice * (1 - l_discount)))#1461L) AND (sum((l_extendedprice * (1 - l_discount)))#1461L = scalar-subquery#1459 []))) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_4400_output.getNumRow() << " -> tbl_Aggregate_TD_4400_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(l_suppkey#79, sum((l_extendedprice * (1 - l_discount)))#1461L)" 
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_4855_output.getNumRow() << " -> tbl_Aggregate_TD_4855_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(max(total_revenue)#1465L)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_2745_output.getNumRow() << " -> tbl_Filter_TD_2745_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(supplier_no#1462, total_revenue#1463L)" 
     << std::endl; 

    std::cout << "JOIN_INNER_1: " << tvdiff(&tv_r_JOIN_INNER_1_646_s, &tv_r_JOIN_INNER_1_646_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer((s_suppkey#342 = supplier_no#1462)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_2209_input_stringRowIDSubstitute.getNumRow() << " -> tbl_SerializeFromObject_TD_2209_input_stringRowIDSubstitute" 
     << std::endl << "      #COL: 4: " << "ListBuffer(s_suppkey#342, s_name#343, s_address#344, s_phone#346)" 
     << std::endl << "      #ROW: " << tbl_Filter_TD_2745_output.getNumRow() << " -> tbl_Filter_TD_2745_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(supplier_no#1462, total_revenue#1463L)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_1396_output.getNumRow() << " -> tbl_JOIN_INNER_TD_1396_output" 
     << std::endl << "      #COL: 5: " << "ListBuffer(s_suppkey#342, s_name#343, s_address#344, s_phone#346, total_revenue#1463L)" 
     << std::endl; 

    std::cout << "Sort_0: " << tvdiff(&tv_r_Sort_0_77_s, &tv_r_Sort_0_77_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(s_suppkey#342 ASC NULLS FIRST) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_1396_output.getNumRow() << " -> tbl_JOIN_INNER_TD_1396_output" 
     << std::endl << "      #COL: 5: " << "ListBuffer(s_suppkey#342, s_name#343, s_address#344, s_phone#346, total_revenue#1463L)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Sort_TD_0131_output.getNumRow() << " -> tbl_Sort_TD_0131_output" 
     << std::endl << "      #COL: 5: " << "ListBuffer(s_suppkey#342, s_name#343, s_address#344, s_phone#346, total_revenue#1463L)" 
     << std::endl; 

    std::cout << std::endl << " Total execution time: " << tvdiff(&tv_r_s, &tv_r_e) / 1000 << " ms"; 

    std::cout << std::endl << " Spark elapsed time: " << 19.555304 * 1000 << "ms" << std::endl; 
    return 0; 
}
