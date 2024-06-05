// number of overlays (w/o fusion): 0 
// number of overlays (w/ fusion): 0 
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

#include "cfgFunc_q8.hpp" 
#include "q8.hpp" 

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
    std::cout << "NOTE:running query #8\n."; 
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
    // *********************** Partition Infomation ********************* // 
    int hpTimes_join = 32; 
    int power_of_hpTimes_join = log2(hpTimes_join); 
    std::cout << "Number of partition (gqeJoin) is: " << hpTimes_join << std::endl; 
    int hpTimes_aggr = 256; 
    int power_of_hpTimes_aggr = log2(hpTimes_aggr); 
    std::cout << "Number of partition (gqeAggr) is: " << hpTimes_aggr << std::endl; 
    // ****************************** Tables **************************** // 
    Table tbl_Sort_TD_044_output("tbl_Sort_TD_044_output", 183000000, 2, "");
    tbl_Sort_TD_044_output.allocateHost();
    Table tbl_Aggregate_TD_1947_output("tbl_Aggregate_TD_1947_output", 183000000, 2, "");
    tbl_Aggregate_TD_1947_output.allocateHost();
    Table tbl_Project_TD_284_output("tbl_Project_TD_284_output", 183000000, 3, "");
    tbl_Project_TD_284_output.allocateHost();
    Table tbl_JOIN_INNER_TD_3880_output("tbl_JOIN_INNER_TD_3880_output", 183000000, 4, "");
    tbl_JOIN_INNER_TD_3880_output.allocateHost();
    Table tbl_JOIN_INNER_TD_4563_output("tbl_JOIN_INNER_TD_4563_output", 183000000, 5, "");
    tbl_JOIN_INNER_TD_4563_output.allocateHost();
    Table tbl_SerializeFromObject_TD_4289_input;
    tbl_SerializeFromObject_TD_4289_input = Table("nation", nation_n, 2, in_dir);
    tbl_SerializeFromObject_TD_4289_input.addCol("n_nationkey", 4);
    tbl_SerializeFromObject_TD_4289_input.addCol("n_name", TPCH_READ_NATION_LEN+1);
    tbl_SerializeFromObject_TD_4289_input.allocateHost();
    tbl_SerializeFromObject_TD_4289_input.loadHost();
    Table tbl_JOIN_INNER_TD_5831_output("tbl_JOIN_INNER_TD_5831_output", 183000000, 5, "");
    tbl_JOIN_INNER_TD_5831_output.allocateHost();
    Table tbl_Filter_TD_4917_output("tbl_Filter_TD_4917_output", 183000000, 1, "");
    tbl_Filter_TD_4917_output.allocateHost();
    Table tbl_JOIN_INNER_TD_6109_output("tbl_JOIN_INNER_TD_6109_output", 183000000, 5, "");
    tbl_JOIN_INNER_TD_6109_output.allocateHost();
    Table tbl_SerializeFromObject_TD_6669_input;
    tbl_SerializeFromObject_TD_6669_input = Table("nation", nation_n, 2, in_dir);
    tbl_SerializeFromObject_TD_6669_input.addCol("n_nationkey", 4);
    tbl_SerializeFromObject_TD_6669_input.addCol("n_regionkey", 4);
    tbl_SerializeFromObject_TD_6669_input.allocateHost();
    tbl_SerializeFromObject_TD_6669_input.loadHost();
    Table tbl_SerializeFromObject_TD_5525_input;
    tbl_SerializeFromObject_TD_5525_input = Table("region", region_n, 2, in_dir);
    tbl_SerializeFromObject_TD_5525_input.addCol("r_regionkey", 4);
    tbl_SerializeFromObject_TD_5525_input.addCol("r_name", TPCH_READ_REGION_LEN+1);
    tbl_SerializeFromObject_TD_5525_input.allocateHost();
    tbl_SerializeFromObject_TD_5525_input.loadHost();
    Table tbl_JOIN_INNER_TD_7188_output("tbl_JOIN_INNER_TD_7188_output", 183000000, 5, "");
    tbl_JOIN_INNER_TD_7188_output.allocateHost();
    Table tbl_SerializeFromObject_TD_7554_input;
    tbl_SerializeFromObject_TD_7554_input = Table("customer", customer_n, 2, in_dir);
    tbl_SerializeFromObject_TD_7554_input.addCol("c_custkey", 4);
    tbl_SerializeFromObject_TD_7554_input.addCol("c_nationkey", 4);
    tbl_SerializeFromObject_TD_7554_input.allocateHost();
    tbl_SerializeFromObject_TD_7554_input.loadHost();
    Table tbl_JOIN_INNER_TD_8414_output("tbl_JOIN_INNER_TD_8414_output", 183000000, 4, "");
    tbl_JOIN_INNER_TD_8414_output.allocateHost();
    Table tbl_Filter_TD_8973_output("tbl_Filter_TD_8973_output", 183000000, 3, "");
    tbl_Filter_TD_8973_output.allocateHost();
    Table tbl_JOIN_INNER_TD_9626_output("tbl_JOIN_INNER_TD_9626_output", 183000000, 4, "");
    tbl_JOIN_INNER_TD_9626_output.allocateHost();
    Table tbl_SerializeFromObject_TD_9489_input;
    tbl_SerializeFromObject_TD_9489_input = Table("supplier", supplier_n, 2, in_dir);
    tbl_SerializeFromObject_TD_9489_input.addCol("s_suppkey", 4);
    tbl_SerializeFromObject_TD_9489_input.addCol("s_nationkey", 4);
    tbl_SerializeFromObject_TD_9489_input.allocateHost();
    tbl_SerializeFromObject_TD_9489_input.loadHost();
    Table tbl_SerializeFromObject_TD_9879_input;
    tbl_SerializeFromObject_TD_9879_input = Table("order", order_n, 3, in_dir);
    tbl_SerializeFromObject_TD_9879_input.addCol("o_orderkey", 4);
    tbl_SerializeFromObject_TD_9879_input.addCol("o_custkey", 4);
    tbl_SerializeFromObject_TD_9879_input.addCol("o_orderdate", 4);
    tbl_SerializeFromObject_TD_9879_input.allocateHost();
    tbl_SerializeFromObject_TD_9879_input.loadHost();
    Table tbl_Filter_TD_10202_output("tbl_Filter_TD_10202_output", 183000000, 1, "");
    tbl_Filter_TD_10202_output.allocateHost();
    Table tbl_SerializeFromObject_TD_10614_input;
    tbl_SerializeFromObject_TD_10614_input = Table("lineitem", lineitem_n, 5, in_dir);
    tbl_SerializeFromObject_TD_10614_input.addCol("l_orderkey", 4);
    tbl_SerializeFromObject_TD_10614_input.addCol("l_partkey", 4);
    tbl_SerializeFromObject_TD_10614_input.addCol("l_suppkey", 4);
    tbl_SerializeFromObject_TD_10614_input.addCol("l_extendedprice", 4);
    tbl_SerializeFromObject_TD_10614_input.addCol("l_discount", 4);
    tbl_SerializeFromObject_TD_10614_input.allocateHost();
    tbl_SerializeFromObject_TD_10614_input.loadHost();
    Table tbl_SerializeFromObject_TD_11841_input;
    tbl_SerializeFromObject_TD_11841_input = Table("part", part_n, 2, in_dir);
    tbl_SerializeFromObject_TD_11841_input.addCol("p_partkey", 4);
    tbl_SerializeFromObject_TD_11841_input.addCol("p_type", TPCH_READ_P_TYPE_LEN+1);
    tbl_SerializeFromObject_TD_11841_input.allocateHost();
    tbl_SerializeFromObject_TD_11841_input.loadHost();
    // ********************** Allocate Device Buffer ******************** // 
    // ****************************** Config **************************** // 
    // *************************** Kernel Setup ************************* // 
    std::cout << std::endl; 
    // ************************** Transfer Engine *********************** // 
    // ****************************** Events **************************** // 
    // **************************** Operations ************************** // 
    struct timeval tv_r_s, tv_r_e; 
    gettimeofday(&tv_r_s, 0); 

    struct timeval tv_r_Filter_10_102_s, tv_r_Filter_10_102_e;
    gettimeofday(&tv_r_Filter_10_102_s, 0);
    SW_Filter_TD_10202(tbl_SerializeFromObject_TD_11841_input, tbl_Filter_TD_10202_output);
    gettimeofday(&tv_r_Filter_10_102_e, 0);

    struct timeval tv_r_JOIN_INNER_9_783_s, tv_r_JOIN_INNER_9_783_e;
    gettimeofday(&tv_r_JOIN_INNER_9_783_s, 0);
    SW_JOIN_INNER_TD_9626(tbl_Filter_TD_10202_output, tbl_SerializeFromObject_TD_10614_input, tbl_JOIN_INNER_TD_9626_output);
    gettimeofday(&tv_r_JOIN_INNER_9_783_e, 0);

    struct timeval tv_r_Filter_8_227_s, tv_r_Filter_8_227_e;
    gettimeofday(&tv_r_Filter_8_227_s, 0);
    SW_Filter_TD_8973(tbl_SerializeFromObject_TD_9879_input, tbl_Filter_TD_8973_output);
    gettimeofday(&tv_r_Filter_8_227_e, 0);

    struct timeval tv_r_JOIN_INNER_8_662_s, tv_r_JOIN_INNER_8_662_e;
    gettimeofday(&tv_r_JOIN_INNER_8_662_s, 0);
    SW_JOIN_INNER_TD_8414(tbl_JOIN_INNER_TD_9626_output, tbl_SerializeFromObject_TD_9489_input, tbl_JOIN_INNER_TD_8414_output);
    gettimeofday(&tv_r_JOIN_INNER_8_662_e, 0);

    struct timeval tv_r_JOIN_INNER_7_855_s, tv_r_JOIN_INNER_7_855_e;
    gettimeofday(&tv_r_JOIN_INNER_7_855_s, 0);
    SW_JOIN_INNER_TD_7188(tbl_JOIN_INNER_TD_8414_output, tbl_Filter_TD_8973_output, tbl_JOIN_INNER_TD_7188_output);
    gettimeofday(&tv_r_JOIN_INNER_7_855_e, 0);

    struct timeval tv_r_JOIN_INNER_6_304_s, tv_r_JOIN_INNER_6_304_e;
    gettimeofday(&tv_r_JOIN_INNER_6_304_s, 0);
    SW_JOIN_INNER_TD_6109(tbl_JOIN_INNER_TD_7188_output, tbl_SerializeFromObject_TD_7554_input, tbl_JOIN_INNER_TD_6109_output);
    gettimeofday(&tv_r_JOIN_INNER_6_304_e, 0);

    struct timeval tv_r_Filter_4_131_s, tv_r_Filter_4_131_e;
    gettimeofday(&tv_r_Filter_4_131_s, 0);
    SW_Filter_TD_4917(tbl_SerializeFromObject_TD_5525_input, tbl_Filter_TD_4917_output);
    gettimeofday(&tv_r_Filter_4_131_e, 0);

    struct timeval tv_r_JOIN_INNER_5_368_s, tv_r_JOIN_INNER_5_368_e;
    gettimeofday(&tv_r_JOIN_INNER_5_368_s, 0);
    SW_JOIN_INNER_TD_5831(tbl_JOIN_INNER_TD_6109_output, tbl_SerializeFromObject_TD_6669_input, tbl_JOIN_INNER_TD_5831_output);
    gettimeofday(&tv_r_JOIN_INNER_5_368_e, 0);

    struct timeval tv_r_JOIN_INNER_4_300_s, tv_r_JOIN_INNER_4_300_e;
    gettimeofday(&tv_r_JOIN_INNER_4_300_s, 0);
    SW_JOIN_INNER_TD_4563(tbl_JOIN_INNER_TD_5831_output, tbl_Filter_TD_4917_output, tbl_JOIN_INNER_TD_4563_output);
    gettimeofday(&tv_r_JOIN_INNER_4_300_e, 0);

    struct timeval tv_r_JOIN_INNER_3_522_s, tv_r_JOIN_INNER_3_522_e;
    gettimeofday(&tv_r_JOIN_INNER_3_522_s, 0);
    SW_JOIN_INNER_TD_3880(tbl_JOIN_INNER_TD_4563_output, tbl_SerializeFromObject_TD_4289_input, tbl_JOIN_INNER_TD_3880_output);
    gettimeofday(&tv_r_JOIN_INNER_3_522_e, 0);

    struct timeval tv_r_Project_2_543_s, tv_r_Project_2_543_e;
    gettimeofday(&tv_r_Project_2_543_s, 0);
    SW_Project_TD_284(tbl_JOIN_INNER_TD_3880_output, tbl_Project_TD_284_output);
    gettimeofday(&tv_r_Project_2_543_e, 0);

    struct timeval tv_r_Aggregate_1_890_s, tv_r_Aggregate_1_890_e;
    gettimeofday(&tv_r_Aggregate_1_890_s, 0);
    SW_Aggregate_TD_1947(tbl_Project_TD_284_output, tbl_Aggregate_TD_1947_output);
    gettimeofday(&tv_r_Aggregate_1_890_e, 0);

    struct timeval tv_r_Sort_0_28_s, tv_r_Sort_0_28_e;
    gettimeofday(&tv_r_Sort_0_28_s, 0);
    SW_Sort_TD_044(tbl_Aggregate_TD_1947_output, tbl_Sort_TD_044_output);
    gettimeofday(&tv_r_Sort_0_28_e, 0);

std::cout << "CPU TIME: " << (tvdiff(&tv_r_Filter_10_102_s, &tv_r_Filter_10_102_e) + 
tvdiff(&tv_r_JOIN_INNER_9_783_s, &tv_r_JOIN_INNER_9_783_e) + 
tvdiff(&tv_r_Filter_8_227_s, &tv_r_Filter_8_227_e) + 
tvdiff(&tv_r_JOIN_INNER_8_662_s, &tv_r_JOIN_INNER_8_662_e) + 
tvdiff(&tv_r_JOIN_INNER_7_855_s, &tv_r_JOIN_INNER_7_855_e) + 
tvdiff(&tv_r_JOIN_INNER_6_304_s, &tv_r_JOIN_INNER_6_304_e) + 
tvdiff(&tv_r_Filter_4_131_s, &tv_r_Filter_4_131_e) + 
tvdiff(&tv_r_JOIN_INNER_5_368_s, &tv_r_JOIN_INNER_5_368_e) + 
tvdiff(&tv_r_JOIN_INNER_4_300_s, &tv_r_JOIN_INNER_4_300_e) + 
tvdiff(&tv_r_JOIN_INNER_3_522_s, &tv_r_JOIN_INNER_3_522_e) + 
tvdiff(&tv_r_Project_2_543_s, &tv_r_Project_2_543_e) + 
tvdiff(&tv_r_Aggregate_1_890_s, &tv_r_Aggregate_1_890_e) + 
tvdiff(&tv_r_Sort_0_28_s, &tv_r_Sort_0_28_e) + 
1) / 1000.0 << std::endl;

std::cout << "FPGA TIME: " << (1) / 1000.0 << std::endl;

    gettimeofday(&tv_r_e, 0); 
    // **************************** Print Execution Time ************************** // 
    std::cout << std::endl; 
    std::cout << "Filter_10: " << tvdiff(&tv_r_Filter_10_102_s, &tv_r_Filter_10_102_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer((isnotnull(p_type#262) AND (p_type#262 = LARGE ANODIZED STEEL))) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_11841_input.getNumRow() << " -> tbl_SerializeFromObject_TD_11841_input" 
     << std::endl << "      #COL: 2: " << "ListBuffer(p_partkey#258, p_type#262)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_10202_output.getNumRow() << " -> tbl_Filter_TD_10202_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(p_partkey#258)" 
     << std::endl; 

    std::cout << "JOIN_INNER_9: " << tvdiff(&tv_r_JOIN_INNER_9_783_s, &tv_r_JOIN_INNER_9_783_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer((p_partkey#258 = l_partkey#78)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_10202_output.getNumRow() << " -> tbl_Filter_TD_10202_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(p_partkey#258)" 
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_10614_input.getNumRow() << " -> tbl_SerializeFromObject_TD_10614_input" 
     << std::endl << "      #COL: 5: " << "ListBuffer(l_orderkey#77, l_partkey#78, l_suppkey#79, l_extendedprice#82, l_discount#83)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_9626_output.getNumRow() << " -> tbl_JOIN_INNER_TD_9626_output" 
     << std::endl << "      #COL: 4: " << "ListBuffer(l_orderkey#77, l_suppkey#79, l_extendedprice#82, l_discount#83)" 
     << std::endl; 

    std::cout << "Filter_8: " << tvdiff(&tv_r_Filter_8_227_s, &tv_r_Filter_8_227_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(((o_orderdate#211 >= 19950101) AND (o_orderdate#211 <= 19961231))) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_9879_input.getNumRow() << " -> tbl_SerializeFromObject_TD_9879_input" 
     << std::endl << "      #COL: 3: " << "ListBuffer(o_orderkey#207, o_custkey#208, o_orderdate#211)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_8973_output.getNumRow() << " -> tbl_Filter_TD_8973_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(o_orderkey#207, o_custkey#208, o_orderdate#211)" 
     << std::endl; 

    std::cout << "JOIN_INNER_8: " << tvdiff(&tv_r_JOIN_INNER_8_662_s, &tv_r_JOIN_INNER_8_662_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer((s_suppkey#342 = l_suppkey#79)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_9626_output.getNumRow() << " -> tbl_JOIN_INNER_TD_9626_output" 
     << std::endl << "      #COL: 4: " << "ListBuffer(l_orderkey#77, l_suppkey#79, l_extendedprice#82, l_discount#83)" 
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_9489_input.getNumRow() << " -> tbl_SerializeFromObject_TD_9489_input" 
     << std::endl << "      #COL: 2: " << "ListBuffer(s_suppkey#342, s_nationkey#345)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_8414_output.getNumRow() << " -> tbl_JOIN_INNER_TD_8414_output" 
     << std::endl << "      #COL: 4: " << "ListBuffer(l_orderkey#77, l_extendedprice#82, l_discount#83, s_nationkey#345)" 
     << std::endl; 

    std::cout << "JOIN_INNER_7: " << tvdiff(&tv_r_JOIN_INNER_7_855_s, &tv_r_JOIN_INNER_7_855_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer((l_orderkey#77 = o_orderkey#207)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_8414_output.getNumRow() << " -> tbl_JOIN_INNER_TD_8414_output" 
     << std::endl << "      #COL: 4: " << "ListBuffer(l_orderkey#77, l_extendedprice#82, l_discount#83, s_nationkey#345)" 
     << std::endl << "      #ROW: " << tbl_Filter_TD_8973_output.getNumRow() << " -> tbl_Filter_TD_8973_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(o_orderkey#207, o_custkey#208, o_orderdate#211)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_7188_output.getNumRow() << " -> tbl_JOIN_INNER_TD_7188_output" 
     << std::endl << "      #COL: 5: " << "ListBuffer(l_extendedprice#82, l_discount#83, s_nationkey#345, o_custkey#208, o_orderdate#211)" 
     << std::endl; 

    std::cout << "JOIN_INNER_6: " << tvdiff(&tv_r_JOIN_INNER_6_304_s, &tv_r_JOIN_INNER_6_304_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer((o_custkey#208 = c_custkey#22)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_7188_output.getNumRow() << " -> tbl_JOIN_INNER_TD_7188_output" 
     << std::endl << "      #COL: 5: " << "ListBuffer(l_extendedprice#82, l_discount#83, s_nationkey#345, o_custkey#208, o_orderdate#211)" 
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_7554_input.getNumRow() << " -> tbl_SerializeFromObject_TD_7554_input" 
     << std::endl << "      #COL: 2: " << "ListBuffer(c_custkey#22, c_nationkey#25)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_6109_output.getNumRow() << " -> tbl_JOIN_INNER_TD_6109_output" 
     << std::endl << "      #COL: 5: " << "ListBuffer(l_extendedprice#82, l_discount#83, s_nationkey#345, o_orderdate#211, c_nationkey#25)" 
     << std::endl; 

    std::cout << "Filter_4: " << tvdiff(&tv_r_Filter_4_131_s, &tv_r_Filter_4_131_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer((isnotnull(r_name#175) AND (r_name#175 = MIDDLE EAST))) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_5525_input.getNumRow() << " -> tbl_SerializeFromObject_TD_5525_input" 
     << std::endl << "      #COL: 2: " << "ListBuffer(r_regionkey#174, r_name#175)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_4917_output.getNumRow() << " -> tbl_Filter_TD_4917_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(r_regionkey#174)" 
     << std::endl; 

    std::cout << "JOIN_INNER_5: " << tvdiff(&tv_r_JOIN_INNER_5_368_s, &tv_r_JOIN_INNER_5_368_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer((c_nationkey#25 = n_nationkey#144)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_6109_output.getNumRow() << " -> tbl_JOIN_INNER_TD_6109_output" 
     << std::endl << "      #COL: 5: " << "ListBuffer(l_extendedprice#82, l_discount#83, s_nationkey#345, o_orderdate#211, c_nationkey#25)" 
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_6669_input.getNumRow() << " -> tbl_SerializeFromObject_TD_6669_input" 
     << std::endl << "      #COL: 2: " << "ListBuffer(n_nationkey#144, n_regionkey#146)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_5831_output.getNumRow() << " -> tbl_JOIN_INNER_TD_5831_output" 
     << std::endl << "      #COL: 5: " << "ListBuffer(l_extendedprice#82, l_discount#83, s_nationkey#345, o_orderdate#211, n_regionkey#146)" 
     << std::endl; 

    std::cout << "JOIN_INNER_4: " << tvdiff(&tv_r_JOIN_INNER_4_300_s, &tv_r_JOIN_INNER_4_300_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer((n_regionkey#146 = r_regionkey#174)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_5831_output.getNumRow() << " -> tbl_JOIN_INNER_TD_5831_output" 
     << std::endl << "      #COL: 5: " << "ListBuffer(l_extendedprice#82, l_discount#83, s_nationkey#345, o_orderdate#211, n_regionkey#146)" 
     << std::endl << "      #ROW: " << tbl_Filter_TD_4917_output.getNumRow() << " -> tbl_Filter_TD_4917_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(r_regionkey#174)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_4563_output.getNumRow() << " -> tbl_JOIN_INNER_TD_4563_output" 
     << std::endl << "      #COL: 5: " << "ListBuffer(o_orderdate#211, l_extendedprice#82, l_discount#83, s_nationkey#345, n_regionkey#146)" 
     << std::endl; 

    std::cout << "JOIN_INNER_3: " << tvdiff(&tv_r_JOIN_INNER_3_522_s, &tv_r_JOIN_INNER_3_522_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer((s_nationkey#345 = n_nationkey#950)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_4563_output.getNumRow() << " -> tbl_JOIN_INNER_TD_4563_output" 
     << std::endl << "      #COL: 5: " << "ListBuffer(o_orderdate#211, l_extendedprice#82, l_discount#83, s_nationkey#345, n_regionkey#146)" 
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_4289_input.getNumRow() << " -> tbl_SerializeFromObject_TD_4289_input" 
     << std::endl << "      #COL: 2: " << "ListBuffer(n_nationkey#950, n_name#951)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_3880_output.getNumRow() << " -> tbl_JOIN_INNER_TD_3880_output" 
     << std::endl << "      #COL: 4: " << "ListBuffer(o_orderdate#211, l_extendedprice#82, l_discount#83, n_name#951)" 
     << std::endl; 

    std::cout << "Project_2: " << tvdiff(&tv_r_Project_2_543_s, &tv_r_Project_2_543_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(cast((cast(o_orderdate#211 as double) / 10000.0) as int) AS o_year#945, (l_extendedprice#82 * (100 - l_discount#83)) AS volume#946, n_name#951 AS nation#947) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_3880_output.getNumRow() << " -> tbl_JOIN_INNER_TD_3880_output" 
     << std::endl << "      #COL: 4: " << "ListBuffer(o_orderdate#211, l_extendedprice#82, l_discount#83, n_name#951)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Project_TD_284_output.getNumRow() << " -> tbl_Project_TD_284_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(o_year#945, volume#946, nation#947)" 
     << std::endl; 

    std::cout << "Aggregate_1: " << tvdiff(&tv_r_Aggregate_1_890_s, &tv_r_Aggregate_1_890_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(o_year#945, (cast(sum(CASE WHEN (nation#947 = IRAQ) THEN volume#946 ELSE 0 END) as double) / cast(sum(volume#946) as double)) AS mkt_share#948) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_Project_TD_284_output.getNumRow() << " -> tbl_Project_TD_284_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(o_year#945, volume#946, nation#947)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_1947_output.getNumRow() << " -> tbl_Aggregate_TD_1947_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(o_year#945, mkt_share#948)" 
     << std::endl; 

    std::cout << "Sort_0: " << tvdiff(&tv_r_Sort_0_28_s, &tv_r_Sort_0_28_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(o_year#945 ASC NULLS FIRST) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_1947_output.getNumRow() << " -> tbl_Aggregate_TD_1947_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(o_year#945, mkt_share#948)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Sort_TD_044_output.getNumRow() << " -> tbl_Sort_TD_044_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(o_year#945, mkt_share#948)" 
     << std::endl; 

    std::cout << std::endl << " Total execution time: " << tvdiff(&tv_r_s, &tv_r_e) / 1000 << " ms"; 

    std::cout << std::endl << " Spark elapsed time: " << 24.037178 * 1000 << "ms" << std::endl; 
    return 0; 
}
