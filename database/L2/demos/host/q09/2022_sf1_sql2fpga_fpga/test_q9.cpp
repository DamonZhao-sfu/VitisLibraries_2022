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

#include "cfgFunc_q9.hpp" 
#include "q9.hpp" 

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
    std::cout << "NOTE:running query #9\n."; 
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
    // ****************************** Tables **************************** // 
    Table tbl_Sort_TD_0397_output("tbl_Sort_TD_0397_output", 6100000, 3, "");
    tbl_Sort_TD_0397_output.allocateHost();
    Table tbl_Aggregate_TD_1582_output("tbl_Aggregate_TD_1582_output", 6100000, 3, "");
    tbl_Aggregate_TD_1582_output.allocateHost();
    Table tbl_Project_TD_291_output("tbl_Project_TD_291_output", 6100000, 3, "");
    tbl_Project_TD_291_output.allocateHost();
    Table tbl_JOIN_INNER_TD_3618_output("tbl_JOIN_INNER_TD_3618_output", 6100000, 6, "");
    tbl_JOIN_INNER_TD_3618_output.allocateHost();
    Table tbl_JOIN_INNER_TD_4533_output("tbl_JOIN_INNER_TD_4533_output", 6100000, 6, "");
    tbl_JOIN_INNER_TD_4533_output.allocateHost();
    Table tbl_SerializeFromObject_TD_4284_input;
    tbl_SerializeFromObject_TD_4284_input = Table("nation", nation_n, 2, in_dir);
    tbl_SerializeFromObject_TD_4284_input.addCol("n_nationkey", 4);
    tbl_SerializeFromObject_TD_4284_input.addCol("n_name", TPCH_READ_NATION_LEN+1);
    tbl_SerializeFromObject_TD_4284_input.allocateHost();
    tbl_SerializeFromObject_TD_4284_input.loadHost();
    Table tbl_JOIN_INNER_TD_576_output("tbl_JOIN_INNER_TD_576_output", 6100000, 6, "");
    tbl_JOIN_INNER_TD_576_output.allocateHost();
    Table tbl_SerializeFromObject_TD_5322_input;
    tbl_SerializeFromObject_TD_5322_input = Table("order", order_n, 2, in_dir);
    tbl_SerializeFromObject_TD_5322_input.addCol("o_orderkey", 4);
    tbl_SerializeFromObject_TD_5322_input.addCol("o_orderdate", 4);
    tbl_SerializeFromObject_TD_5322_input.allocateHost();
    tbl_SerializeFromObject_TD_5322_input.loadHost();
    Table tbl_JOIN_INNER_TD_6667_output("tbl_JOIN_INNER_TD_6667_output", 6100000, 7, "");
    tbl_JOIN_INNER_TD_6667_output.allocateHost();
    Table tbl_SerializeFromObject_TD_6792_input;
    tbl_SerializeFromObject_TD_6792_input = Table("partsupp", partsupp_n, 3, in_dir);
    tbl_SerializeFromObject_TD_6792_input.addCol("ps_partkey", 4);
    tbl_SerializeFromObject_TD_6792_input.addCol("ps_suppkey", 4);
    tbl_SerializeFromObject_TD_6792_input.addCol("ps_supplycost", 4);
    tbl_SerializeFromObject_TD_6792_input.allocateHost();
    tbl_SerializeFromObject_TD_6792_input.loadHost();
    Table tbl_JOIN_INNER_TD_7358_output("tbl_JOIN_INNER_TD_7358_output", 6100000, 6, "");
    tbl_JOIN_INNER_TD_7358_output.allocateHost();
    Table tbl_SerializeFromObject_TD_7979_input;
    tbl_SerializeFromObject_TD_7979_input = Table("supplier", supplier_n, 2, in_dir);
    tbl_SerializeFromObject_TD_7979_input.addCol("s_suppkey", 4);
    tbl_SerializeFromObject_TD_7979_input.addCol("s_nationkey", 4);
    tbl_SerializeFromObject_TD_7979_input.allocateHost();
    tbl_SerializeFromObject_TD_7979_input.loadHost();
    Table tbl_Filter_TD_8919_output("tbl_Filter_TD_8919_output", 6100000, 1, "");
    tbl_Filter_TD_8919_output.allocateHost();
    Table tbl_SerializeFromObject_TD_8593_input;
    tbl_SerializeFromObject_TD_8593_input = Table("lineitem", lineitem_n, 6, in_dir);
    tbl_SerializeFromObject_TD_8593_input.addCol("l_orderkey", 4);
    tbl_SerializeFromObject_TD_8593_input.addCol("l_partkey", 4);
    tbl_SerializeFromObject_TD_8593_input.addCol("l_suppkey", 4);
    tbl_SerializeFromObject_TD_8593_input.addCol("l_quantity", 4);
    tbl_SerializeFromObject_TD_8593_input.addCol("l_extendedprice", 4);
    tbl_SerializeFromObject_TD_8593_input.addCol("l_discount", 4);
    tbl_SerializeFromObject_TD_8593_input.allocateHost();
    tbl_SerializeFromObject_TD_8593_input.loadHost();
    Table tbl_SerializeFromObject_TD_9593_input;
    tbl_SerializeFromObject_TD_9593_input = Table("part", part_n, 2, in_dir);
    tbl_SerializeFromObject_TD_9593_input.addCol("p_partkey", 4);
    tbl_SerializeFromObject_TD_9593_input.addCol("p_name", TPCH_READ_P_NAME_LEN+1);
    tbl_SerializeFromObject_TD_9593_input.allocateHost();
    tbl_SerializeFromObject_TD_9593_input.loadHost();
    // ********************** Allocate Device Buffer ******************** // 
    // ****************************** Config **************************** // 
    // *************************** Kernel Setup ************************* // 
    std::cout << std::endl; 
    // ************************** Transfer Engine *********************** // 
    // ****************************** Events **************************** // 
    // **************************** Operations ************************** // 
    struct timeval tv_r_s, tv_r_e; 
    gettimeofday(&tv_r_s, 0); 

    struct timeval tv_r_Filter_8_100_s, tv_r_Filter_8_100_e;
    gettimeofday(&tv_r_Filter_8_100_s, 0);
    SW_Filter_TD_8919(tbl_SerializeFromObject_TD_9593_input, tbl_Filter_TD_8919_output);
    gettimeofday(&tv_r_Filter_8_100_e, 0);

    struct timeval tv_r_JOIN_INNER_7_159_s, tv_r_JOIN_INNER_7_159_e;
    gettimeofday(&tv_r_JOIN_INNER_7_159_s, 0);
    SW_JOIN_INNER_TD_7358(tbl_Filter_TD_8919_output, tbl_SerializeFromObject_TD_8593_input, tbl_JOIN_INNER_TD_7358_output);
    gettimeofday(&tv_r_JOIN_INNER_7_159_e, 0);

    struct timeval tv_r_JOIN_INNER_6_551_s, tv_r_JOIN_INNER_6_551_e;
    gettimeofday(&tv_r_JOIN_INNER_6_551_s, 0);
    SW_JOIN_INNER_TD_6667(tbl_JOIN_INNER_TD_7358_output, tbl_SerializeFromObject_TD_7979_input, tbl_JOIN_INNER_TD_6667_output);
    gettimeofday(&tv_r_JOIN_INNER_6_551_e, 0);

    struct timeval tv_r_JOIN_INNER_5_910_s, tv_r_JOIN_INNER_5_910_e;
    gettimeofday(&tv_r_JOIN_INNER_5_910_s, 0);
    SW_JOIN_INNER_TD_576(tbl_JOIN_INNER_TD_6667_output, tbl_SerializeFromObject_TD_6792_input, tbl_JOIN_INNER_TD_576_output);
    gettimeofday(&tv_r_JOIN_INNER_5_910_e, 0);

    struct timeval tv_r_JOIN_INNER_4_39_s, tv_r_JOIN_INNER_4_39_e;
    gettimeofday(&tv_r_JOIN_INNER_4_39_s, 0);
    SW_JOIN_INNER_TD_4533(tbl_JOIN_INNER_TD_576_output, tbl_SerializeFromObject_TD_5322_input, tbl_JOIN_INNER_TD_4533_output);
    gettimeofday(&tv_r_JOIN_INNER_4_39_e, 0);

    struct timeval tv_r_JOIN_INNER_3_707_s, tv_r_JOIN_INNER_3_707_e;
    gettimeofday(&tv_r_JOIN_INNER_3_707_s, 0);
    SW_JOIN_INNER_TD_3618(tbl_JOIN_INNER_TD_4533_output, tbl_SerializeFromObject_TD_4284_input, tbl_JOIN_INNER_TD_3618_output);
    gettimeofday(&tv_r_JOIN_INNER_3_707_e, 0);

    struct timeval tv_r_Project_2_534_s, tv_r_Project_2_534_e;
    gettimeofday(&tv_r_Project_2_534_s, 0);
    SW_Project_TD_291(tbl_JOIN_INNER_TD_3618_output, tbl_Project_TD_291_output);
    gettimeofday(&tv_r_Project_2_534_e, 0);

    struct timeval tv_r_Aggregate_1_985_s, tv_r_Aggregate_1_985_e;
    gettimeofday(&tv_r_Aggregate_1_985_s, 0);
    SW_Aggregate_TD_1582(tbl_Project_TD_291_output, tbl_Aggregate_TD_1582_output);
    gettimeofday(&tv_r_Aggregate_1_985_e, 0);

    struct timeval tv_r_Sort_0_844_s, tv_r_Sort_0_844_e;
    gettimeofday(&tv_r_Sort_0_844_s, 0);
    SW_Sort_TD_0397(tbl_Aggregate_TD_1582_output, tbl_Sort_TD_0397_output);
    gettimeofday(&tv_r_Sort_0_844_e, 0);

std::cout << "CPU TIME: " << (tvdiff(&tv_r_Filter_8_100_s, &tv_r_Filter_8_100_e) + 
tvdiff(&tv_r_JOIN_INNER_7_159_s, &tv_r_JOIN_INNER_7_159_e) + 
tvdiff(&tv_r_JOIN_INNER_6_551_s, &tv_r_JOIN_INNER_6_551_e) + 
tvdiff(&tv_r_JOIN_INNER_5_910_s, &tv_r_JOIN_INNER_5_910_e) + 
tvdiff(&tv_r_JOIN_INNER_4_39_s, &tv_r_JOIN_INNER_4_39_e) + 
tvdiff(&tv_r_JOIN_INNER_3_707_s, &tv_r_JOIN_INNER_3_707_e) + 
tvdiff(&tv_r_Project_2_534_s, &tv_r_Project_2_534_e) + 
tvdiff(&tv_r_Aggregate_1_985_s, &tv_r_Aggregate_1_985_e) + 
tvdiff(&tv_r_Sort_0_844_s, &tv_r_Sort_0_844_e) + 
1) / 1000.0 << std::endl;

std::cout << "FPGA TIME: " << (1) / 1000.0 << std::endl;

    gettimeofday(&tv_r_e, 0); 
    // **************************** Print Execution Time ************************** // 
    std::cout << std::endl; 
    std::cout << "Filter_8: " << tvdiff(&tv_r_Filter_8_100_s, &tv_r_Filter_8_100_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer((isnotnull(p_name#259) AND Contains(p_name#259, magenta))) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_9593_input.getNumRow() << " -> tbl_SerializeFromObject_TD_9593_input" 
     << std::endl << "      #COL: 2: " << "ListBuffer(p_partkey#258, p_name#259)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_8919_output.getNumRow() << " -> tbl_Filter_TD_8919_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(p_partkey#258)" 
     << std::endl; 

    std::cout << "JOIN_INNER_7: " << tvdiff(&tv_r_JOIN_INNER_7_159_s, &tv_r_JOIN_INNER_7_159_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer((p_partkey#258 = l_partkey#78)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_8919_output.getNumRow() << " -> tbl_Filter_TD_8919_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(p_partkey#258)" 
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_8593_input.getNumRow() << " -> tbl_SerializeFromObject_TD_8593_input" 
     << std::endl << "      #COL: 6: " << "ListBuffer(l_orderkey#77, l_partkey#78, l_suppkey#79, l_quantity#81, l_extendedprice#82, l_discount#83)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_7358_output.getNumRow() << " -> tbl_JOIN_INNER_TD_7358_output" 
     << std::endl << "      #COL: 6: " << "ListBuffer(l_orderkey#77, l_partkey#78, l_suppkey#79, l_quantity#81, l_extendedprice#82, l_discount#83)" 
     << std::endl; 

    std::cout << "JOIN_INNER_6: " << tvdiff(&tv_r_JOIN_INNER_6_551_s, &tv_r_JOIN_INNER_6_551_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer((s_suppkey#342 = l_suppkey#79)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_7358_output.getNumRow() << " -> tbl_JOIN_INNER_TD_7358_output" 
     << std::endl << "      #COL: 6: " << "ListBuffer(l_orderkey#77, l_partkey#78, l_suppkey#79, l_quantity#81, l_extendedprice#82, l_discount#83)" 
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_7979_input.getNumRow() << " -> tbl_SerializeFromObject_TD_7979_input" 
     << std::endl << "      #COL: 2: " << "ListBuffer(s_suppkey#342, s_nationkey#345)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_6667_output.getNumRow() << " -> tbl_JOIN_INNER_TD_6667_output" 
     << std::endl << "      #COL: 7: " << "ListBuffer(l_orderkey#77, l_partkey#78, l_suppkey#79, l_quantity#81, l_extendedprice#82, l_discount#83, s_nationkey#345)" 
     << std::endl; 

    std::cout << "JOIN_INNER_5: " << tvdiff(&tv_r_JOIN_INNER_5_910_s, &tv_r_JOIN_INNER_5_910_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(((ps_suppkey#306 = l_suppkey#79) AND (ps_partkey#305 = l_partkey#78))) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_6667_output.getNumRow() << " -> tbl_JOIN_INNER_TD_6667_output" 
     << std::endl << "      #COL: 7: " << "ListBuffer(l_orderkey#77, l_partkey#78, l_suppkey#79, l_quantity#81, l_extendedprice#82, l_discount#83, s_nationkey#345)" 
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_6792_input.getNumRow() << " -> tbl_SerializeFromObject_TD_6792_input" 
     << std::endl << "      #COL: 3: " << "ListBuffer(ps_partkey#305, ps_suppkey#306, ps_supplycost#308)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_576_output.getNumRow() << " -> tbl_JOIN_INNER_TD_576_output" 
     << std::endl << "      #COL: 6: " << "ListBuffer(l_orderkey#77, l_quantity#81, l_extendedprice#82, l_discount#83, s_nationkey#345, ps_supplycost#308)" 
     << std::endl; 

    std::cout << "JOIN_INNER_4: " << tvdiff(&tv_r_JOIN_INNER_4_39_s, &tv_r_JOIN_INNER_4_39_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer((o_orderkey#207 = l_orderkey#77)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_576_output.getNumRow() << " -> tbl_JOIN_INNER_TD_576_output" 
     << std::endl << "      #COL: 6: " << "ListBuffer(l_orderkey#77, l_quantity#81, l_extendedprice#82, l_discount#83, s_nationkey#345, ps_supplycost#308)" 
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_5322_input.getNumRow() << " -> tbl_SerializeFromObject_TD_5322_input" 
     << std::endl << "      #COL: 2: " << "ListBuffer(o_orderkey#207, o_orderdate#211)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_4533_output.getNumRow() << " -> tbl_JOIN_INNER_TD_4533_output" 
     << std::endl << "      #COL: 6: " << "ListBuffer(l_quantity#81, l_extendedprice#82, l_discount#83, s_nationkey#345, ps_supplycost#308, o_orderdate#211)" 
     << std::endl; 

    std::cout << "JOIN_INNER_3: " << tvdiff(&tv_r_JOIN_INNER_3_707_s, &tv_r_JOIN_INNER_3_707_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer((s_nationkey#345 = n_nationkey#144)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_4533_output.getNumRow() << " -> tbl_JOIN_INNER_TD_4533_output" 
     << std::endl << "      #COL: 6: " << "ListBuffer(l_quantity#81, l_extendedprice#82, l_discount#83, s_nationkey#345, ps_supplycost#308, o_orderdate#211)" 
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_4284_input.getNumRow() << " -> tbl_SerializeFromObject_TD_4284_input" 
     << std::endl << "      #COL: 2: " << "ListBuffer(n_nationkey#144, n_name#145)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_3618_output.getNumRow() << " -> tbl_JOIN_INNER_TD_3618_output" 
     << std::endl << "      #COL: 6: " << "ListBuffer(n_name#145, o_orderdate#211, l_extendedprice#82, l_discount#83, ps_supplycost#308, l_quantity#81)" 
     << std::endl; 

    std::cout << "Project_2: " << tvdiff(&tv_r_Project_2_534_s, &tv_r_Project_2_534_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(n_name#145 AS nation#1063, cast((cast(o_orderdate#211 as double) / 10000.0) as int) AS o_year#1064, ((l_extendedprice#82 * (1 - l_discount#83)) - (ps_supplycost#308 * l_quantity#81)) AS amount#1065) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_3618_output.getNumRow() << " -> tbl_JOIN_INNER_TD_3618_output" 
     << std::endl << "      #COL: 6: " << "ListBuffer(n_name#145, o_orderdate#211, l_extendedprice#82, l_discount#83, ps_supplycost#308, l_quantity#81)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Project_TD_291_output.getNumRow() << " -> tbl_Project_TD_291_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(nation#1063, o_year#1064, amount#1065)" 
     << std::endl; 

    std::cout << "Aggregate_1: " << tvdiff(&tv_r_Aggregate_1_985_s, &tv_r_Aggregate_1_985_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(nation#1063, o_year#1064, sum(amount#1065) AS sum_profit#1066L) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_Project_TD_291_output.getNumRow() << " -> tbl_Project_TD_291_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(nation#1063, o_year#1064, amount#1065)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_1582_output.getNumRow() << " -> tbl_Aggregate_TD_1582_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(nation#1063, o_year#1064, sum_profit#1066L)" 
     << std::endl; 

    std::cout << "Sort_0: " << tvdiff(&tv_r_Sort_0_844_s, &tv_r_Sort_0_844_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(nation#1063 ASC NULLS FIRST, o_year#1064 DESC NULLS LAST) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_1582_output.getNumRow() << " -> tbl_Aggregate_TD_1582_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(nation#1063, o_year#1064, sum_profit#1066L)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Sort_TD_0397_output.getNumRow() << " -> tbl_Sort_TD_0397_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(nation#1063, o_year#1064, sum_profit#1066L)" 
     << std::endl; 

    std::cout << std::endl << " Total execution time: " << tvdiff(&tv_r_s, &tv_r_e) / 1000 << " ms"; 

    std::cout << std::endl << " Spark elapsed time: " << 27.019144 * 1000 << "ms" << std::endl; 
    return 0; 
}
