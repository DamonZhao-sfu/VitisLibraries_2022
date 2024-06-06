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
    // ****************************** Tables **************************** // 
    Table tbl_Sort_TD_0259_output("tbl_Sort_TD_0259_output", 6100000, 2, "");
    tbl_Sort_TD_0259_output.allocateHost();
    Table tbl_Aggregate_TD_1568_output("tbl_Aggregate_TD_1568_output", 6100000, 2, "");
    tbl_Aggregate_TD_1568_output.allocateHost();
    Table tbl_JOIN_INNER_TD_2878_output("tbl_JOIN_INNER_TD_2878_output", 6100000, 3, "");
    tbl_JOIN_INNER_TD_2878_output.allocateHost();
    Table tbl_JOIN_INNER_TD_3489_output("tbl_JOIN_INNER_TD_3489_output", 6100000, 4, "");
    tbl_JOIN_INNER_TD_3489_output.allocateHost();
    Table tbl_Filter_TD_3312_output("tbl_Filter_TD_3312_output", 6100000, 1, "");
    tbl_Filter_TD_3312_output.allocateHost();
    Table tbl_JOIN_INNER_TD_4813_output("tbl_JOIN_INNER_TD_4813_output", 6100000, 3, "");
    tbl_JOIN_INNER_TD_4813_output.allocateHost();
    Table tbl_SerializeFromObject_TD_4697_input;
    tbl_SerializeFromObject_TD_4697_input = Table("nation", nation_n, 3, in_dir);
    tbl_SerializeFromObject_TD_4697_input.addCol("n_nationkey", 4);
    tbl_SerializeFromObject_TD_4697_input.addCol("n_name", TPCH_READ_NATION_LEN+1);
    tbl_SerializeFromObject_TD_4697_input.addCol("n_regionkey", 4);
    tbl_SerializeFromObject_TD_4697_input.allocateHost();
    tbl_SerializeFromObject_TD_4697_input.loadHost();
    Table tbl_SerializeFromObject_TD_414_input;
    tbl_SerializeFromObject_TD_414_input = Table("region", region_n, 2, in_dir);
    tbl_SerializeFromObject_TD_414_input.addCol("r_regionkey", 4);
    tbl_SerializeFromObject_TD_414_input.addCol("r_name", TPCH_READ_REGION_LEN+1);
    tbl_SerializeFromObject_TD_414_input.allocateHost();
    tbl_SerializeFromObject_TD_414_input.loadHost();
    Table tbl_JOIN_INNER_TD_5328_output("tbl_JOIN_INNER_TD_5328_output", 6100000, 4, "");
    tbl_JOIN_INNER_TD_5328_output.allocateHost();
    Table tbl_SerializeFromObject_TD_5371_input;
    tbl_SerializeFromObject_TD_5371_input = Table("supplier", supplier_n, 2, in_dir);
    tbl_SerializeFromObject_TD_5371_input.addCol("s_suppkey", 4);
    tbl_SerializeFromObject_TD_5371_input.addCol("s_nationkey", 4);
    tbl_SerializeFromObject_TD_5371_input.allocateHost();
    tbl_SerializeFromObject_TD_5371_input.loadHost();
    Table tbl_JOIN_INNER_TD_6305_output("tbl_JOIN_INNER_TD_6305_output", 6100000, 2, "");
    tbl_JOIN_INNER_TD_6305_output.allocateHost();
    Table tbl_SerializeFromObject_TD_6954_input;
    tbl_SerializeFromObject_TD_6954_input = Table("lineitem", lineitem_n, 4, in_dir);
    tbl_SerializeFromObject_TD_6954_input.addCol("l_orderkey", 4);
    tbl_SerializeFromObject_TD_6954_input.addCol("l_suppkey", 4);
    tbl_SerializeFromObject_TD_6954_input.addCol("l_extendedprice", 4);
    tbl_SerializeFromObject_TD_6954_input.addCol("l_discount", 4);
    tbl_SerializeFromObject_TD_6954_input.allocateHost();
    tbl_SerializeFromObject_TD_6954_input.loadHost();
    Table tbl_SerializeFromObject_TD_7970_input;
    tbl_SerializeFromObject_TD_7970_input = Table("customer", customer_n, 2, in_dir);
    tbl_SerializeFromObject_TD_7970_input.addCol("c_custkey", 4);
    tbl_SerializeFromObject_TD_7970_input.addCol("c_nationkey", 4);
    tbl_SerializeFromObject_TD_7970_input.allocateHost();
    tbl_SerializeFromObject_TD_7970_input.loadHost();
    Table tbl_Filter_TD_7898_output("tbl_Filter_TD_7898_output", 6100000, 2, "");
    tbl_Filter_TD_7898_output.allocateHost();
    Table tbl_SerializeFromObject_TD_8226_input;
    tbl_SerializeFromObject_TD_8226_input = Table("order", order_n, 3, in_dir);
    tbl_SerializeFromObject_TD_8226_input.addCol("o_orderkey", 4);
    tbl_SerializeFromObject_TD_8226_input.addCol("o_custkey", 4);
    tbl_SerializeFromObject_TD_8226_input.addCol("o_orderdate", 4);
    tbl_SerializeFromObject_TD_8226_input.allocateHost();
    tbl_SerializeFromObject_TD_8226_input.loadHost();
    // ********************** Allocate Device Buffer ******************** // 
    // ****************************** Config **************************** // 
    // *************************** Kernel Setup ************************* // 
    std::cout << std::endl; 
    // ************************** Transfer Engine *********************** // 
    // ****************************** Events **************************** // 
    // **************************** Operations ************************** // 
    struct timeval tv_r_s, tv_r_e; 
    gettimeofday(&tv_r_s, 0); 

    struct timeval tv_r_Filter_7_221_s, tv_r_Filter_7_221_e;
    gettimeofday(&tv_r_Filter_7_221_s, 0);
    SW_Filter_TD_7898(tbl_SerializeFromObject_TD_8226_input, tbl_Filter_TD_7898_output);
    gettimeofday(&tv_r_Filter_7_221_e, 0);

    struct timeval tv_r_JOIN_INNER_6_293_s, tv_r_JOIN_INNER_6_293_e;
    gettimeofday(&tv_r_JOIN_INNER_6_293_s, 0);
    SW_JOIN_INNER_TD_6305(tbl_SerializeFromObject_TD_7970_input, tbl_Filter_TD_7898_output, tbl_JOIN_INNER_TD_6305_output);
    gettimeofday(&tv_r_JOIN_INNER_6_293_e, 0);

    struct timeval tv_r_JOIN_INNER_5_968_s, tv_r_JOIN_INNER_5_968_e;
    gettimeofday(&tv_r_JOIN_INNER_5_968_s, 0);
    SW_JOIN_INNER_TD_5328(tbl_JOIN_INNER_TD_6305_output, tbl_SerializeFromObject_TD_6954_input, tbl_JOIN_INNER_TD_5328_output);
    gettimeofday(&tv_r_JOIN_INNER_5_968_e, 0);

    struct timeval tv_r_JOIN_INNER_4_957_s, tv_r_JOIN_INNER_4_957_e;
    gettimeofday(&tv_r_JOIN_INNER_4_957_s, 0);
    SW_JOIN_INNER_TD_4813(tbl_JOIN_INNER_TD_5328_output, tbl_SerializeFromObject_TD_5371_input, tbl_JOIN_INNER_TD_4813_output);
    gettimeofday(&tv_r_JOIN_INNER_4_957_e, 0);

    struct timeval tv_r_Filter_3_450_s, tv_r_Filter_3_450_e;
    gettimeofday(&tv_r_Filter_3_450_s, 0);
    SW_Filter_TD_3312(tbl_SerializeFromObject_TD_414_input, tbl_Filter_TD_3312_output);
    gettimeofday(&tv_r_Filter_3_450_e, 0);

    struct timeval tv_r_JOIN_INNER_3_532_s, tv_r_JOIN_INNER_3_532_e;
    gettimeofday(&tv_r_JOIN_INNER_3_532_s, 0);
    SW_JOIN_INNER_TD_3489(tbl_JOIN_INNER_TD_4813_output, tbl_SerializeFromObject_TD_4697_input, tbl_JOIN_INNER_TD_3489_output);
    gettimeofday(&tv_r_JOIN_INNER_3_532_e, 0);

    struct timeval tv_r_JOIN_INNER_2_388_s, tv_r_JOIN_INNER_2_388_e;
    gettimeofday(&tv_r_JOIN_INNER_2_388_s, 0);
    SW_JOIN_INNER_TD_2878(tbl_JOIN_INNER_TD_3489_output, tbl_Filter_TD_3312_output, tbl_JOIN_INNER_TD_2878_output);
    gettimeofday(&tv_r_JOIN_INNER_2_388_e, 0);

    struct timeval tv_r_Aggregate_1_195_s, tv_r_Aggregate_1_195_e;
    gettimeofday(&tv_r_Aggregate_1_195_s, 0);
    SW_Aggregate_TD_1568(tbl_JOIN_INNER_TD_2878_output, tbl_Aggregate_TD_1568_output);
    gettimeofday(&tv_r_Aggregate_1_195_e, 0);

    struct timeval tv_r_Sort_0_685_s, tv_r_Sort_0_685_e;
    gettimeofday(&tv_r_Sort_0_685_s, 0);
    SW_Sort_TD_0259(tbl_Aggregate_TD_1568_output, tbl_Sort_TD_0259_output);
    gettimeofday(&tv_r_Sort_0_685_e, 0);

std::cout << "CPU TIME: " << (tvdiff(&tv_r_Filter_7_221_s, &tv_r_Filter_7_221_e) + 
tvdiff(&tv_r_JOIN_INNER_6_293_s, &tv_r_JOIN_INNER_6_293_e) + 
tvdiff(&tv_r_JOIN_INNER_5_968_s, &tv_r_JOIN_INNER_5_968_e) + 
tvdiff(&tv_r_JOIN_INNER_4_957_s, &tv_r_JOIN_INNER_4_957_e) + 
tvdiff(&tv_r_Filter_3_450_s, &tv_r_Filter_3_450_e) + 
tvdiff(&tv_r_JOIN_INNER_3_532_s, &tv_r_JOIN_INNER_3_532_e) + 
tvdiff(&tv_r_JOIN_INNER_2_388_s, &tv_r_JOIN_INNER_2_388_e) + 
tvdiff(&tv_r_Aggregate_1_195_s, &tv_r_Aggregate_1_195_e) + 
tvdiff(&tv_r_Sort_0_685_s, &tv_r_Sort_0_685_e) + 
1) / 1000.0 << std::endl;

std::cout << "FPGA TIME: " << (1) / 1000.0 << std::endl;

    gettimeofday(&tv_r_e, 0); 
    // **************************** Print Execution Time ************************** // 
    std::cout << std::endl; 
    std::cout << "Filter_7: " << tvdiff(&tv_r_Filter_7_221_s, &tv_r_Filter_7_221_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(((o_orderdate#211 >= 19930101) AND (o_orderdate#211 < 19940101))) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_8226_input.getNumRow() << " -> tbl_SerializeFromObject_TD_8226_input" 
     << std::endl << "      #COL: 3: " << "ListBuffer(o_orderkey#207, o_custkey#208, o_orderdate#211)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_7898_output.getNumRow() << " -> tbl_Filter_TD_7898_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(o_orderkey#207, o_custkey#208)" 
     << std::endl; 

    std::cout << "JOIN_INNER_6: " << tvdiff(&tv_r_JOIN_INNER_6_293_s, &tv_r_JOIN_INNER_6_293_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer((c_custkey#22 = o_custkey#208)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_7970_input.getNumRow() << " -> tbl_SerializeFromObject_TD_7970_input" 
     << std::endl << "      #COL: 2: " << "ListBuffer(c_custkey#22, c_nationkey#25)" 
     << std::endl << "      #ROW: " << tbl_Filter_TD_7898_output.getNumRow() << " -> tbl_Filter_TD_7898_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(o_orderkey#207, o_custkey#208)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_6305_output.getNumRow() << " -> tbl_JOIN_INNER_TD_6305_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(c_nationkey#25, o_orderkey#207)" 
     << std::endl; 

    std::cout << "JOIN_INNER_5: " << tvdiff(&tv_r_JOIN_INNER_5_968_s, &tv_r_JOIN_INNER_5_968_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer((l_orderkey#77 = o_orderkey#207)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_6305_output.getNumRow() << " -> tbl_JOIN_INNER_TD_6305_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(c_nationkey#25, o_orderkey#207)" 
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_6954_input.getNumRow() << " -> tbl_SerializeFromObject_TD_6954_input" 
     << std::endl << "      #COL: 4: " << "ListBuffer(l_orderkey#77, l_suppkey#79, l_extendedprice#82, l_discount#83)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_5328_output.getNumRow() << " -> tbl_JOIN_INNER_TD_5328_output" 
     << std::endl << "      #COL: 4: " << "ListBuffer(c_nationkey#25, l_suppkey#79, l_extendedprice#82, l_discount#83)" 
     << std::endl; 

    std::cout << "JOIN_INNER_4: " << tvdiff(&tv_r_JOIN_INNER_4_957_s, &tv_r_JOIN_INNER_4_957_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(((l_suppkey#79 = s_suppkey#342) AND (c_nationkey#25 = s_nationkey#345))) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_5328_output.getNumRow() << " -> tbl_JOIN_INNER_TD_5328_output" 
     << std::endl << "      #COL: 4: " << "ListBuffer(c_nationkey#25, l_suppkey#79, l_extendedprice#82, l_discount#83)" 
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_5371_input.getNumRow() << " -> tbl_SerializeFromObject_TD_5371_input" 
     << std::endl << "      #COL: 2: " << "ListBuffer(s_suppkey#342, s_nationkey#345)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_4813_output.getNumRow() << " -> tbl_JOIN_INNER_TD_4813_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(l_extendedprice#82, l_discount#83, s_nationkey#345)" 
     << std::endl; 

    std::cout << "Filter_3: " << tvdiff(&tv_r_Filter_3_450_s, &tv_r_Filter_3_450_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer((isnotnull(r_name#175) AND (r_name#175 = AFRICA))) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_414_input.getNumRow() << " -> tbl_SerializeFromObject_TD_414_input" 
     << std::endl << "      #COL: 2: " << "ListBuffer(r_regionkey#174, r_name#175)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_3312_output.getNumRow() << " -> tbl_Filter_TD_3312_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(r_regionkey#174)" 
     << std::endl; 

    std::cout << "JOIN_INNER_3: " << tvdiff(&tv_r_JOIN_INNER_3_532_s, &tv_r_JOIN_INNER_3_532_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer((s_nationkey#345 = n_nationkey#144)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_4813_output.getNumRow() << " -> tbl_JOIN_INNER_TD_4813_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(l_extendedprice#82, l_discount#83, s_nationkey#345)" 
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_4697_input.getNumRow() << " -> tbl_SerializeFromObject_TD_4697_input" 
     << std::endl << "      #COL: 3: " << "ListBuffer(n_nationkey#144, n_name#145, n_regionkey#146)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_3489_output.getNumRow() << " -> tbl_JOIN_INNER_TD_3489_output" 
     << std::endl << "      #COL: 4: " << "ListBuffer(l_extendedprice#82, l_discount#83, n_name#145, n_regionkey#146)" 
     << std::endl; 

    std::cout << "JOIN_INNER_2: " << tvdiff(&tv_r_JOIN_INNER_2_388_s, &tv_r_JOIN_INNER_2_388_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer((n_regionkey#146 = r_regionkey#174)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_3489_output.getNumRow() << " -> tbl_JOIN_INNER_TD_3489_output" 
     << std::endl << "      #COL: 4: " << "ListBuffer(l_extendedprice#82, l_discount#83, n_name#145, n_regionkey#146)" 
     << std::endl << "      #ROW: " << tbl_Filter_TD_3312_output.getNumRow() << " -> tbl_Filter_TD_3312_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(r_regionkey#174)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_2878_output.getNumRow() << " -> tbl_JOIN_INNER_TD_2878_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(l_extendedprice#82, l_discount#83, n_name#145)" 
     << std::endl; 

    std::cout << "Aggregate_1: " << tvdiff(&tv_r_Aggregate_1_195_s, &tv_r_Aggregate_1_195_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(n_name#145, sum((l_extendedprice#82 * (100 - l_discount#83))) AS revenue#774L) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_2878_output.getNumRow() << " -> tbl_JOIN_INNER_TD_2878_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(l_extendedprice#82, l_discount#83, n_name#145)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_1568_output.getNumRow() << " -> tbl_Aggregate_TD_1568_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(n_name#145, revenue#774L)" 
     << std::endl; 

    std::cout << "Sort_0: " << tvdiff(&tv_r_Sort_0_685_s, &tv_r_Sort_0_685_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(revenue#774L DESC NULLS LAST) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_1568_output.getNumRow() << " -> tbl_Aggregate_TD_1568_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(n_name#145, revenue#774L)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Sort_TD_0259_output.getNumRow() << " -> tbl_Sort_TD_0259_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(n_name#145, revenue#774L)" 
     << std::endl; 

    std::cout << std::endl << " Total execution time: " << tvdiff(&tv_r_s, &tv_r_e) / 1000 << " ms"; 

    std::cout << std::endl << " Spark elapsed time: " << 26.155636 * 1000 << "ms" << std::endl; 
    return 0; 
}
