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

#include "cfgFunc_q5.hpp" 
#include "q5.hpp" 

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
    std::cout << "NOTE:running query #5\n."; 
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
    Table tbl_Sort_TD_0927_output("tbl_Sort_TD_0927_output", 183000000, 2, "");
    tbl_Sort_TD_0927_output.allocateHost();
    Table tbl_Aggregate_TD_1889_output("tbl_Aggregate_TD_1889_output", 183000000, 2, "");
    tbl_Aggregate_TD_1889_output.allocateHost();
    Table tbl_JOIN_INNER_TD_2170_output("tbl_JOIN_INNER_TD_2170_output", 183000000, 3, "");
    tbl_JOIN_INNER_TD_2170_output.allocateHost();
    Table tbl_JOIN_INNER_TD_3337_output("tbl_JOIN_INNER_TD_3337_output", 183000000, 4, "");
    tbl_JOIN_INNER_TD_3337_output.allocateHost();
    Table tbl_Filter_TD_3521_output("tbl_Filter_TD_3521_output", 183000000, 1, "");
    tbl_Filter_TD_3521_output.allocateHost();
    Table tbl_JOIN_INNER_TD_4330_output("tbl_JOIN_INNER_TD_4330_output", 183000000, 3, "");
    tbl_JOIN_INNER_TD_4330_output.allocateHost();
    Table tbl_SerializeFromObject_TD_4785_input;
    tbl_SerializeFromObject_TD_4785_input = Table("nation", nation_n, 3, in_dir);
    tbl_SerializeFromObject_TD_4785_input.addCol("n_nationkey", 4);
    tbl_SerializeFromObject_TD_4785_input.addCol("n_name", TPCH_READ_NATION_LEN+1);
    tbl_SerializeFromObject_TD_4785_input.addCol("n_regionkey", 4);
    tbl_SerializeFromObject_TD_4785_input.allocateHost();
    tbl_SerializeFromObject_TD_4785_input.loadHost();
    Table tbl_SerializeFromObject_TD_4247_input;
    tbl_SerializeFromObject_TD_4247_input = Table("region", region_n, 2, in_dir);
    tbl_SerializeFromObject_TD_4247_input.addCol("r_regionkey", 4);
    tbl_SerializeFromObject_TD_4247_input.addCol("r_name", TPCH_READ_REGION_LEN+1);
    tbl_SerializeFromObject_TD_4247_input.allocateHost();
    tbl_SerializeFromObject_TD_4247_input.loadHost();
    Table tbl_JOIN_INNER_TD_5395_output("tbl_JOIN_INNER_TD_5395_output", 183000000, 4, "");
    tbl_JOIN_INNER_TD_5395_output.allocateHost();
    Table tbl_SerializeFromObject_TD_5515_input;
    tbl_SerializeFromObject_TD_5515_input = Table("supplier", supplier_n, 2, in_dir);
    tbl_SerializeFromObject_TD_5515_input.addCol("s_suppkey", 4);
    tbl_SerializeFromObject_TD_5515_input.addCol("s_nationkey", 4);
    tbl_SerializeFromObject_TD_5515_input.allocateHost();
    tbl_SerializeFromObject_TD_5515_input.loadHost();
    Table tbl_JOIN_INNER_TD_6328_output("tbl_JOIN_INNER_TD_6328_output", 183000000, 2, "");
    tbl_JOIN_INNER_TD_6328_output.allocateHost();
    Table tbl_SerializeFromObject_TD_643_input;
    tbl_SerializeFromObject_TD_643_input = Table("lineitem", lineitem_n, 4, in_dir);
    tbl_SerializeFromObject_TD_643_input.addCol("l_orderkey", 4);
    tbl_SerializeFromObject_TD_643_input.addCol("l_suppkey", 4);
    tbl_SerializeFromObject_TD_643_input.addCol("l_extendedprice", 4);
    tbl_SerializeFromObject_TD_643_input.addCol("l_discount", 4);
    tbl_SerializeFromObject_TD_643_input.allocateHost();
    tbl_SerializeFromObject_TD_643_input.loadHost();
    Table tbl_SerializeFromObject_TD_7711_input;
    tbl_SerializeFromObject_TD_7711_input = Table("customer", customer_n, 2, in_dir);
    tbl_SerializeFromObject_TD_7711_input.addCol("c_custkey", 4);
    tbl_SerializeFromObject_TD_7711_input.addCol("c_nationkey", 4);
    tbl_SerializeFromObject_TD_7711_input.allocateHost();
    tbl_SerializeFromObject_TD_7711_input.loadHost();
    Table tbl_Filter_TD_7843_output("tbl_Filter_TD_7843_output", 183000000, 2, "");
    tbl_Filter_TD_7843_output.allocateHost();
    Table tbl_SerializeFromObject_TD_8120_input;
    tbl_SerializeFromObject_TD_8120_input = Table("order", order_n, 3, in_dir);
    tbl_SerializeFromObject_TD_8120_input.addCol("o_orderkey", 4);
    tbl_SerializeFromObject_TD_8120_input.addCol("o_custkey", 4);
    tbl_SerializeFromObject_TD_8120_input.addCol("o_orderdate", 4);
    tbl_SerializeFromObject_TD_8120_input.allocateHost();
    tbl_SerializeFromObject_TD_8120_input.loadHost();
    // ********************** Allocate Device Buffer ******************** // 
    // ****************************** Config **************************** // 
    // *************************** Kernel Setup ************************* // 
    std::cout << std::endl; 
    // ************************** Transfer Engine *********************** // 
    // ****************************** Events **************************** // 
    // **************************** Operations ************************** // 
    struct timeval tv_r_s, tv_r_e; 
    gettimeofday(&tv_r_s, 0); 

    struct timeval tv_r_Filter_7_358_s, tv_r_Filter_7_358_e;
    gettimeofday(&tv_r_Filter_7_358_s, 0);
    SW_Filter_TD_7843(tbl_SerializeFromObject_TD_8120_input, tbl_Filter_TD_7843_output);
    gettimeofday(&tv_r_Filter_7_358_e, 0);

    struct timeval tv_r_JOIN_INNER_6_170_s, tv_r_JOIN_INNER_6_170_e;
    gettimeofday(&tv_r_JOIN_INNER_6_170_s, 0);
    SW_JOIN_INNER_TD_6328(tbl_SerializeFromObject_TD_7711_input, tbl_Filter_TD_7843_output, tbl_JOIN_INNER_TD_6328_output);
    gettimeofday(&tv_r_JOIN_INNER_6_170_e, 0);

    struct timeval tv_r_JOIN_INNER_5_867_s, tv_r_JOIN_INNER_5_867_e;
    gettimeofday(&tv_r_JOIN_INNER_5_867_s, 0);
    SW_JOIN_INNER_TD_5395(tbl_JOIN_INNER_TD_6328_output, tbl_SerializeFromObject_TD_643_input, tbl_JOIN_INNER_TD_5395_output);
    gettimeofday(&tv_r_JOIN_INNER_5_867_e, 0);

    struct timeval tv_r_JOIN_INNER_4_759_s, tv_r_JOIN_INNER_4_759_e;
    gettimeofday(&tv_r_JOIN_INNER_4_759_s, 0);
    SW_JOIN_INNER_TD_4330(tbl_JOIN_INNER_TD_5395_output, tbl_SerializeFromObject_TD_5515_input, tbl_JOIN_INNER_TD_4330_output);
    gettimeofday(&tv_r_JOIN_INNER_4_759_e, 0);

    struct timeval tv_r_Filter_3_622_s, tv_r_Filter_3_622_e;
    gettimeofday(&tv_r_Filter_3_622_s, 0);
    SW_Filter_TD_3521(tbl_SerializeFromObject_TD_4247_input, tbl_Filter_TD_3521_output);
    gettimeofday(&tv_r_Filter_3_622_e, 0);

    struct timeval tv_r_JOIN_INNER_3_586_s, tv_r_JOIN_INNER_3_586_e;
    gettimeofday(&tv_r_JOIN_INNER_3_586_s, 0);
    SW_JOIN_INNER_TD_3337(tbl_JOIN_INNER_TD_4330_output, tbl_SerializeFromObject_TD_4785_input, tbl_JOIN_INNER_TD_3337_output);
    gettimeofday(&tv_r_JOIN_INNER_3_586_e, 0);

    struct timeval tv_r_JOIN_INNER_2_306_s, tv_r_JOIN_INNER_2_306_e;
    gettimeofday(&tv_r_JOIN_INNER_2_306_s, 0);
    SW_JOIN_INNER_TD_2170(tbl_JOIN_INNER_TD_3337_output, tbl_Filter_TD_3521_output, tbl_JOIN_INNER_TD_2170_output);
    gettimeofday(&tv_r_JOIN_INNER_2_306_e, 0);

    struct timeval tv_r_Aggregate_1_867_s, tv_r_Aggregate_1_867_e;
    gettimeofday(&tv_r_Aggregate_1_867_s, 0);
    SW_Aggregate_TD_1889(tbl_JOIN_INNER_TD_2170_output, tbl_Aggregate_TD_1889_output);
    gettimeofday(&tv_r_Aggregate_1_867_e, 0);

    struct timeval tv_r_Sort_0_224_s, tv_r_Sort_0_224_e;
    gettimeofday(&tv_r_Sort_0_224_s, 0);
    SW_Sort_TD_0927(tbl_Aggregate_TD_1889_output, tbl_Sort_TD_0927_output);
    gettimeofday(&tv_r_Sort_0_224_e, 0);

std::cout << "CPU TIME: " << (tvdiff(&tv_r_Filter_7_358_s, &tv_r_Filter_7_358_e) + 
tvdiff(&tv_r_JOIN_INNER_6_170_s, &tv_r_JOIN_INNER_6_170_e) + 
tvdiff(&tv_r_JOIN_INNER_5_867_s, &tv_r_JOIN_INNER_5_867_e) + 
tvdiff(&tv_r_JOIN_INNER_4_759_s, &tv_r_JOIN_INNER_4_759_e) + 
tvdiff(&tv_r_Filter_3_622_s, &tv_r_Filter_3_622_e) + 
tvdiff(&tv_r_JOIN_INNER_3_586_s, &tv_r_JOIN_INNER_3_586_e) + 
tvdiff(&tv_r_JOIN_INNER_2_306_s, &tv_r_JOIN_INNER_2_306_e) + 
tvdiff(&tv_r_Aggregate_1_867_s, &tv_r_Aggregate_1_867_e) + 
tvdiff(&tv_r_Sort_0_224_s, &tv_r_Sort_0_224_e) + 
1) / 1000.0 << std::endl;

std::cout << "FPGA TIME: " << (1) / 1000.0 << std::endl;

    gettimeofday(&tv_r_e, 0); 
    // **************************** Print Execution Time ************************** // 
    std::cout << std::endl; 
    std::cout << "Filter_7: " << tvdiff(&tv_r_Filter_7_358_s, &tv_r_Filter_7_358_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(((o_orderdate#211 >= 19930101) AND (o_orderdate#211 < 19940101))) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_8120_input.getNumRow() << " -> tbl_SerializeFromObject_TD_8120_input" 
     << std::endl << "      #COL: 3: " << "ListBuffer(o_orderkey#207, o_custkey#208, o_orderdate#211)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_7843_output.getNumRow() << " -> tbl_Filter_TD_7843_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(o_orderkey#207, o_custkey#208)" 
     << std::endl; 

    std::cout << "JOIN_INNER_6: " << tvdiff(&tv_r_JOIN_INNER_6_170_s, &tv_r_JOIN_INNER_6_170_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer((c_custkey#22 = o_custkey#208)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_7711_input.getNumRow() << " -> tbl_SerializeFromObject_TD_7711_input" 
     << std::endl << "      #COL: 2: " << "ListBuffer(c_custkey#22, c_nationkey#25)" 
     << std::endl << "      #ROW: " << tbl_Filter_TD_7843_output.getNumRow() << " -> tbl_Filter_TD_7843_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(o_orderkey#207, o_custkey#208)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_6328_output.getNumRow() << " -> tbl_JOIN_INNER_TD_6328_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(c_nationkey#25, o_orderkey#207)" 
     << std::endl; 

    std::cout << "JOIN_INNER_5: " << tvdiff(&tv_r_JOIN_INNER_5_867_s, &tv_r_JOIN_INNER_5_867_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer((l_orderkey#77 = o_orderkey#207)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_6328_output.getNumRow() << " -> tbl_JOIN_INNER_TD_6328_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(c_nationkey#25, o_orderkey#207)" 
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_643_input.getNumRow() << " -> tbl_SerializeFromObject_TD_643_input" 
     << std::endl << "      #COL: 4: " << "ListBuffer(l_orderkey#77, l_suppkey#79, l_extendedprice#82, l_discount#83)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_5395_output.getNumRow() << " -> tbl_JOIN_INNER_TD_5395_output" 
     << std::endl << "      #COL: 4: " << "ListBuffer(c_nationkey#25, l_suppkey#79, l_extendedprice#82, l_discount#83)" 
     << std::endl; 

    std::cout << "JOIN_INNER_4: " << tvdiff(&tv_r_JOIN_INNER_4_759_s, &tv_r_JOIN_INNER_4_759_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(((l_suppkey#79 = s_suppkey#342) AND (c_nationkey#25 = s_nationkey#345))) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_5395_output.getNumRow() << " -> tbl_JOIN_INNER_TD_5395_output" 
     << std::endl << "      #COL: 4: " << "ListBuffer(c_nationkey#25, l_suppkey#79, l_extendedprice#82, l_discount#83)" 
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_5515_input.getNumRow() << " -> tbl_SerializeFromObject_TD_5515_input" 
     << std::endl << "      #COL: 2: " << "ListBuffer(s_suppkey#342, s_nationkey#345)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_4330_output.getNumRow() << " -> tbl_JOIN_INNER_TD_4330_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(l_extendedprice#82, l_discount#83, s_nationkey#345)" 
     << std::endl; 

    std::cout << "Filter_3: " << tvdiff(&tv_r_Filter_3_622_s, &tv_r_Filter_3_622_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer((isnotnull(r_name#175) AND (r_name#175 = AFRICA))) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_4247_input.getNumRow() << " -> tbl_SerializeFromObject_TD_4247_input" 
     << std::endl << "      #COL: 2: " << "ListBuffer(r_regionkey#174, r_name#175)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_3521_output.getNumRow() << " -> tbl_Filter_TD_3521_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(r_regionkey#174)" 
     << std::endl; 

    std::cout << "JOIN_INNER_3: " << tvdiff(&tv_r_JOIN_INNER_3_586_s, &tv_r_JOIN_INNER_3_586_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer((s_nationkey#345 = n_nationkey#144)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_4330_output.getNumRow() << " -> tbl_JOIN_INNER_TD_4330_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(l_extendedprice#82, l_discount#83, s_nationkey#345)" 
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_4785_input.getNumRow() << " -> tbl_SerializeFromObject_TD_4785_input" 
     << std::endl << "      #COL: 3: " << "ListBuffer(n_nationkey#144, n_name#145, n_regionkey#146)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_3337_output.getNumRow() << " -> tbl_JOIN_INNER_TD_3337_output" 
     << std::endl << "      #COL: 4: " << "ListBuffer(l_extendedprice#82, l_discount#83, n_name#145, n_regionkey#146)" 
     << std::endl; 

    std::cout << "JOIN_INNER_2: " << tvdiff(&tv_r_JOIN_INNER_2_306_s, &tv_r_JOIN_INNER_2_306_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer((n_regionkey#146 = r_regionkey#174)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_3337_output.getNumRow() << " -> tbl_JOIN_INNER_TD_3337_output" 
     << std::endl << "      #COL: 4: " << "ListBuffer(l_extendedprice#82, l_discount#83, n_name#145, n_regionkey#146)" 
     << std::endl << "      #ROW: " << tbl_Filter_TD_3521_output.getNumRow() << " -> tbl_Filter_TD_3521_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(r_regionkey#174)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_2170_output.getNumRow() << " -> tbl_JOIN_INNER_TD_2170_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(l_extendedprice#82, l_discount#83, n_name#145)" 
     << std::endl; 

    std::cout << "Aggregate_1: " << tvdiff(&tv_r_Aggregate_1_867_s, &tv_r_Aggregate_1_867_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(n_name#145, sum((l_extendedprice#82 * (100 - l_discount#83))) AS revenue#774L) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_2170_output.getNumRow() << " -> tbl_JOIN_INNER_TD_2170_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(l_extendedprice#82, l_discount#83, n_name#145)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_1889_output.getNumRow() << " -> tbl_Aggregate_TD_1889_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(n_name#145, revenue#774L)" 
     << std::endl; 

    std::cout << "Sort_0: " << tvdiff(&tv_r_Sort_0_224_s, &tv_r_Sort_0_224_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(revenue#774L DESC NULLS LAST) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_1889_output.getNumRow() << " -> tbl_Aggregate_TD_1889_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(n_name#145, revenue#774L)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Sort_TD_0927_output.getNumRow() << " -> tbl_Sort_TD_0927_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(n_name#145, revenue#774L)" 
     << std::endl; 

    std::cout << std::endl << " Total execution time: " << tvdiff(&tv_r_s, &tv_r_e) / 1000 << " ms"; 

    std::cout << std::endl << " Spark elapsed time: " << 26.86989 * 1000 << "ms" << std::endl; 
    return 0; 
}
