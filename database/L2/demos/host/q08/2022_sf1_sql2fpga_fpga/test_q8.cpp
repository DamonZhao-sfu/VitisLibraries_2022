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
    // ****************************** Tables **************************** // 
    Table tbl_Sort_TD_0588_output("tbl_Sort_TD_0588_output", 6100000, 2, "");
    tbl_Sort_TD_0588_output.allocateHost();
    Table tbl_Aggregate_TD_1212_output("tbl_Aggregate_TD_1212_output", 6100000, 2, "");
    tbl_Aggregate_TD_1212_output.allocateHost();
    Table tbl_Project_TD_2446_output("tbl_Project_TD_2446_output", 6100000, 3, "");
    tbl_Project_TD_2446_output.allocateHost();
    Table tbl_JOIN_INNER_TD_3699_output("tbl_JOIN_INNER_TD_3699_output", 6100000, 4, "");
    tbl_JOIN_INNER_TD_3699_output.allocateHost();
    Table tbl_JOIN_INNER_TD_4754_output("tbl_JOIN_INNER_TD_4754_output", 6100000, 5, "");
    tbl_JOIN_INNER_TD_4754_output.allocateHost();
    Table tbl_SerializeFromObject_TD_4527_input;
    tbl_SerializeFromObject_TD_4527_input = Table("nation", nation_n, 2, in_dir);
    tbl_SerializeFromObject_TD_4527_input.addCol("n_nationkey", 4);
    tbl_SerializeFromObject_TD_4527_input.addCol("n_name", TPCH_READ_NATION_LEN+1);
    tbl_SerializeFromObject_TD_4527_input.allocateHost();
    tbl_SerializeFromObject_TD_4527_input.loadHost();
    Table tbl_JOIN_INNER_TD_5728_output("tbl_JOIN_INNER_TD_5728_output", 6100000, 5, "");
    tbl_JOIN_INNER_TD_5728_output.allocateHost();
    Table tbl_Filter_TD_4938_output("tbl_Filter_TD_4938_output", 6100000, 1, "");
    tbl_Filter_TD_4938_output.allocateHost();
    Table tbl_JOIN_INNER_TD_6195_output("tbl_JOIN_INNER_TD_6195_output", 6100000, 5, "");
    tbl_JOIN_INNER_TD_6195_output.allocateHost();
    Table tbl_SerializeFromObject_TD_6357_input;
    tbl_SerializeFromObject_TD_6357_input = Table("nation", nation_n, 2, in_dir);
    tbl_SerializeFromObject_TD_6357_input.addCol("n_nationkey", 4);
    tbl_SerializeFromObject_TD_6357_input.addCol("n_regionkey", 4);
    tbl_SerializeFromObject_TD_6357_input.allocateHost();
    tbl_SerializeFromObject_TD_6357_input.loadHost();
    Table tbl_SerializeFromObject_TD_5477_input;
    tbl_SerializeFromObject_TD_5477_input = Table("region", region_n, 2, in_dir);
    tbl_SerializeFromObject_TD_5477_input.addCol("r_regionkey", 4);
    tbl_SerializeFromObject_TD_5477_input.addCol("r_name", TPCH_READ_REGION_LEN+1);
    tbl_SerializeFromObject_TD_5477_input.allocateHost();
    tbl_SerializeFromObject_TD_5477_input.loadHost();
    Table tbl_JOIN_INNER_TD_7329_output("tbl_JOIN_INNER_TD_7329_output", 6100000, 5, "");
    tbl_JOIN_INNER_TD_7329_output.allocateHost();
    Table tbl_SerializeFromObject_TD_714_input;
    tbl_SerializeFromObject_TD_714_input = Table("customer", customer_n, 2, in_dir);
    tbl_SerializeFromObject_TD_714_input.addCol("c_custkey", 4);
    tbl_SerializeFromObject_TD_714_input.addCol("c_nationkey", 4);
    tbl_SerializeFromObject_TD_714_input.allocateHost();
    tbl_SerializeFromObject_TD_714_input.loadHost();
    Table tbl_JOIN_INNER_TD_8599_output("tbl_JOIN_INNER_TD_8599_output", 6100000, 4, "");
    tbl_JOIN_INNER_TD_8599_output.allocateHost();
    Table tbl_Filter_TD_8928_output("tbl_Filter_TD_8928_output", 6100000, 3, "");
    tbl_Filter_TD_8928_output.allocateHost();
    Table tbl_JOIN_INNER_TD_9216_output("tbl_JOIN_INNER_TD_9216_output", 6100000, 4, "");
    tbl_JOIN_INNER_TD_9216_output.allocateHost();
    Table tbl_SerializeFromObject_TD_9254_input;
    tbl_SerializeFromObject_TD_9254_input = Table("supplier", supplier_n, 2, in_dir);
    tbl_SerializeFromObject_TD_9254_input.addCol("s_suppkey", 4);
    tbl_SerializeFromObject_TD_9254_input.addCol("s_nationkey", 4);
    tbl_SerializeFromObject_TD_9254_input.allocateHost();
    tbl_SerializeFromObject_TD_9254_input.loadHost();
    Table tbl_SerializeFromObject_TD_9677_input;
    tbl_SerializeFromObject_TD_9677_input = Table("order", order_n, 3, in_dir);
    tbl_SerializeFromObject_TD_9677_input.addCol("o_orderkey", 4);
    tbl_SerializeFromObject_TD_9677_input.addCol("o_custkey", 4);
    tbl_SerializeFromObject_TD_9677_input.addCol("o_orderdate", 4);
    tbl_SerializeFromObject_TD_9677_input.allocateHost();
    tbl_SerializeFromObject_TD_9677_input.loadHost();
    Table tbl_Filter_TD_10670_output("tbl_Filter_TD_10670_output", 6100000, 1, "");
    tbl_Filter_TD_10670_output.allocateHost();
    Table tbl_SerializeFromObject_TD_10184_input;
    tbl_SerializeFromObject_TD_10184_input = Table("lineitem", lineitem_n, 5, in_dir);
    tbl_SerializeFromObject_TD_10184_input.addCol("l_orderkey", 4);
    tbl_SerializeFromObject_TD_10184_input.addCol("l_partkey", 4);
    tbl_SerializeFromObject_TD_10184_input.addCol("l_suppkey", 4);
    tbl_SerializeFromObject_TD_10184_input.addCol("l_extendedprice", 4);
    tbl_SerializeFromObject_TD_10184_input.addCol("l_discount", 4);
    tbl_SerializeFromObject_TD_10184_input.allocateHost();
    tbl_SerializeFromObject_TD_10184_input.loadHost();
    Table tbl_SerializeFromObject_TD_11628_input;
    tbl_SerializeFromObject_TD_11628_input = Table("part", part_n, 2, in_dir);
    tbl_SerializeFromObject_TD_11628_input.addCol("p_partkey", 4);
    tbl_SerializeFromObject_TD_11628_input.addCol("p_type", TPCH_READ_P_TYPE_LEN+1);
    tbl_SerializeFromObject_TD_11628_input.allocateHost();
    tbl_SerializeFromObject_TD_11628_input.loadHost();
    // ********************** Allocate Device Buffer ******************** // 
    // ****************************** Config **************************** // 
    // *************************** Kernel Setup ************************* // 
    std::cout << std::endl; 
    // ************************** Transfer Engine *********************** // 
    // ****************************** Events **************************** // 
    // **************************** Operations ************************** // 
    struct timeval tv_r_s, tv_r_e; 
    gettimeofday(&tv_r_s, 0); 

    struct timeval tv_r_Filter_10_278_s, tv_r_Filter_10_278_e;
    gettimeofday(&tv_r_Filter_10_278_s, 0);
    SW_Filter_TD_10670(tbl_SerializeFromObject_TD_11628_input, tbl_Filter_TD_10670_output);
    gettimeofday(&tv_r_Filter_10_278_e, 0);

    struct timeval tv_r_JOIN_INNER_9_601_s, tv_r_JOIN_INNER_9_601_e;
    gettimeofday(&tv_r_JOIN_INNER_9_601_s, 0);
    SW_JOIN_INNER_TD_9216(tbl_Filter_TD_10670_output, tbl_SerializeFromObject_TD_10184_input, tbl_JOIN_INNER_TD_9216_output);
    gettimeofday(&tv_r_JOIN_INNER_9_601_e, 0);

    struct timeval tv_r_Filter_8_507_s, tv_r_Filter_8_507_e;
    gettimeofday(&tv_r_Filter_8_507_s, 0);
    SW_Filter_TD_8928(tbl_SerializeFromObject_TD_9677_input, tbl_Filter_TD_8928_output);
    gettimeofday(&tv_r_Filter_8_507_e, 0);

    struct timeval tv_r_JOIN_INNER_8_436_s, tv_r_JOIN_INNER_8_436_e;
    gettimeofday(&tv_r_JOIN_INNER_8_436_s, 0);
    SW_JOIN_INNER_TD_8599(tbl_JOIN_INNER_TD_9216_output, tbl_SerializeFromObject_TD_9254_input, tbl_JOIN_INNER_TD_8599_output);
    gettimeofday(&tv_r_JOIN_INNER_8_436_e, 0);

    struct timeval tv_r_JOIN_INNER_7_559_s, tv_r_JOIN_INNER_7_559_e;
    gettimeofday(&tv_r_JOIN_INNER_7_559_s, 0);
    SW_JOIN_INNER_TD_7329(tbl_JOIN_INNER_TD_8599_output, tbl_Filter_TD_8928_output, tbl_JOIN_INNER_TD_7329_output);
    gettimeofday(&tv_r_JOIN_INNER_7_559_e, 0);

    struct timeval tv_r_JOIN_INNER_6_312_s, tv_r_JOIN_INNER_6_312_e;
    gettimeofday(&tv_r_JOIN_INNER_6_312_s, 0);
    SW_JOIN_INNER_TD_6195(tbl_JOIN_INNER_TD_7329_output, tbl_SerializeFromObject_TD_714_input, tbl_JOIN_INNER_TD_6195_output);
    gettimeofday(&tv_r_JOIN_INNER_6_312_e, 0);

    struct timeval tv_r_Filter_4_814_s, tv_r_Filter_4_814_e;
    gettimeofday(&tv_r_Filter_4_814_s, 0);
    SW_Filter_TD_4938(tbl_SerializeFromObject_TD_5477_input, tbl_Filter_TD_4938_output);
    gettimeofday(&tv_r_Filter_4_814_e, 0);

    struct timeval tv_r_JOIN_INNER_5_131_s, tv_r_JOIN_INNER_5_131_e;
    gettimeofday(&tv_r_JOIN_INNER_5_131_s, 0);
    SW_JOIN_INNER_TD_5728(tbl_JOIN_INNER_TD_6195_output, tbl_SerializeFromObject_TD_6357_input, tbl_JOIN_INNER_TD_5728_output);
    gettimeofday(&tv_r_JOIN_INNER_5_131_e, 0);

    struct timeval tv_r_JOIN_INNER_4_55_s, tv_r_JOIN_INNER_4_55_e;
    gettimeofday(&tv_r_JOIN_INNER_4_55_s, 0);
    SW_JOIN_INNER_TD_4754(tbl_JOIN_INNER_TD_5728_output, tbl_Filter_TD_4938_output, tbl_JOIN_INNER_TD_4754_output);
    gettimeofday(&tv_r_JOIN_INNER_4_55_e, 0);

    struct timeval tv_r_JOIN_INNER_3_989_s, tv_r_JOIN_INNER_3_989_e;
    gettimeofday(&tv_r_JOIN_INNER_3_989_s, 0);
    SW_JOIN_INNER_TD_3699(tbl_JOIN_INNER_TD_4754_output, tbl_SerializeFromObject_TD_4527_input, tbl_JOIN_INNER_TD_3699_output);
    gettimeofday(&tv_r_JOIN_INNER_3_989_e, 0);

    struct timeval tv_r_Project_2_580_s, tv_r_Project_2_580_e;
    gettimeofday(&tv_r_Project_2_580_s, 0);
    SW_Project_TD_2446(tbl_JOIN_INNER_TD_3699_output, tbl_Project_TD_2446_output);
    gettimeofday(&tv_r_Project_2_580_e, 0);

    struct timeval tv_r_Aggregate_1_470_s, tv_r_Aggregate_1_470_e;
    gettimeofday(&tv_r_Aggregate_1_470_s, 0);
    SW_Aggregate_TD_1212(tbl_Project_TD_2446_output, tbl_Aggregate_TD_1212_output);
    gettimeofday(&tv_r_Aggregate_1_470_e, 0);

    struct timeval tv_r_Sort_0_232_s, tv_r_Sort_0_232_e;
    gettimeofday(&tv_r_Sort_0_232_s, 0);
    SW_Sort_TD_0588(tbl_Aggregate_TD_1212_output, tbl_Sort_TD_0588_output);
    gettimeofday(&tv_r_Sort_0_232_e, 0);

std::cout << "CPU TIME: " << (tvdiff(&tv_r_Filter_10_278_s, &tv_r_Filter_10_278_e) + 
tvdiff(&tv_r_JOIN_INNER_9_601_s, &tv_r_JOIN_INNER_9_601_e) + 
tvdiff(&tv_r_Filter_8_507_s, &tv_r_Filter_8_507_e) + 
tvdiff(&tv_r_JOIN_INNER_8_436_s, &tv_r_JOIN_INNER_8_436_e) + 
tvdiff(&tv_r_JOIN_INNER_7_559_s, &tv_r_JOIN_INNER_7_559_e) + 
tvdiff(&tv_r_JOIN_INNER_6_312_s, &tv_r_JOIN_INNER_6_312_e) + 
tvdiff(&tv_r_Filter_4_814_s, &tv_r_Filter_4_814_e) + 
tvdiff(&tv_r_JOIN_INNER_5_131_s, &tv_r_JOIN_INNER_5_131_e) + 
tvdiff(&tv_r_JOIN_INNER_4_55_s, &tv_r_JOIN_INNER_4_55_e) + 
tvdiff(&tv_r_JOIN_INNER_3_989_s, &tv_r_JOIN_INNER_3_989_e) + 
tvdiff(&tv_r_Project_2_580_s, &tv_r_Project_2_580_e) + 
tvdiff(&tv_r_Aggregate_1_470_s, &tv_r_Aggregate_1_470_e) + 
tvdiff(&tv_r_Sort_0_232_s, &tv_r_Sort_0_232_e) + 
1) / 1000.0 << std::endl;

std::cout << "FPGA TIME: " << (1) / 1000.0 << std::endl;

    gettimeofday(&tv_r_e, 0); 
    // **************************** Print Execution Time ************************** // 
    std::cout << std::endl; 
    std::cout << "Filter_10: " << tvdiff(&tv_r_Filter_10_278_s, &tv_r_Filter_10_278_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer((isnotnull(p_type#262) AND (p_type#262 = LARGE ANODIZED STEEL))) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_11628_input.getNumRow() << " -> tbl_SerializeFromObject_TD_11628_input" 
     << std::endl << "      #COL: 2: " << "ListBuffer(p_partkey#258, p_type#262)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_10670_output.getNumRow() << " -> tbl_Filter_TD_10670_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(p_partkey#258)" 
     << std::endl; 

    std::cout << "JOIN_INNER_9: " << tvdiff(&tv_r_JOIN_INNER_9_601_s, &tv_r_JOIN_INNER_9_601_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer((p_partkey#258 = l_partkey#78)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_10670_output.getNumRow() << " -> tbl_Filter_TD_10670_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(p_partkey#258)" 
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_10184_input.getNumRow() << " -> tbl_SerializeFromObject_TD_10184_input" 
     << std::endl << "      #COL: 5: " << "ListBuffer(l_orderkey#77, l_partkey#78, l_suppkey#79, l_extendedprice#82, l_discount#83)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_9216_output.getNumRow() << " -> tbl_JOIN_INNER_TD_9216_output" 
     << std::endl << "      #COL: 4: " << "ListBuffer(l_orderkey#77, l_suppkey#79, l_extendedprice#82, l_discount#83)" 
     << std::endl; 

    std::cout << "Filter_8: " << tvdiff(&tv_r_Filter_8_507_s, &tv_r_Filter_8_507_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(((o_orderdate#211 >= 19950101) AND (o_orderdate#211 <= 19961231))) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_9677_input.getNumRow() << " -> tbl_SerializeFromObject_TD_9677_input" 
     << std::endl << "      #COL: 3: " << "ListBuffer(o_orderkey#207, o_custkey#208, o_orderdate#211)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_8928_output.getNumRow() << " -> tbl_Filter_TD_8928_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(o_orderkey#207, o_custkey#208, o_orderdate#211)" 
     << std::endl; 

    std::cout << "JOIN_INNER_8: " << tvdiff(&tv_r_JOIN_INNER_8_436_s, &tv_r_JOIN_INNER_8_436_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer((s_suppkey#342 = l_suppkey#79)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_9216_output.getNumRow() << " -> tbl_JOIN_INNER_TD_9216_output" 
     << std::endl << "      #COL: 4: " << "ListBuffer(l_orderkey#77, l_suppkey#79, l_extendedprice#82, l_discount#83)" 
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_9254_input.getNumRow() << " -> tbl_SerializeFromObject_TD_9254_input" 
     << std::endl << "      #COL: 2: " << "ListBuffer(s_suppkey#342, s_nationkey#345)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_8599_output.getNumRow() << " -> tbl_JOIN_INNER_TD_8599_output" 
     << std::endl << "      #COL: 4: " << "ListBuffer(l_orderkey#77, l_extendedprice#82, l_discount#83, s_nationkey#345)" 
     << std::endl; 

    std::cout << "JOIN_INNER_7: " << tvdiff(&tv_r_JOIN_INNER_7_559_s, &tv_r_JOIN_INNER_7_559_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer((l_orderkey#77 = o_orderkey#207)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_8599_output.getNumRow() << " -> tbl_JOIN_INNER_TD_8599_output" 
     << std::endl << "      #COL: 4: " << "ListBuffer(l_orderkey#77, l_extendedprice#82, l_discount#83, s_nationkey#345)" 
     << std::endl << "      #ROW: " << tbl_Filter_TD_8928_output.getNumRow() << " -> tbl_Filter_TD_8928_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(o_orderkey#207, o_custkey#208, o_orderdate#211)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_7329_output.getNumRow() << " -> tbl_JOIN_INNER_TD_7329_output" 
     << std::endl << "      #COL: 5: " << "ListBuffer(l_extendedprice#82, l_discount#83, s_nationkey#345, o_custkey#208, o_orderdate#211)" 
     << std::endl; 

    std::cout << "JOIN_INNER_6: " << tvdiff(&tv_r_JOIN_INNER_6_312_s, &tv_r_JOIN_INNER_6_312_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer((o_custkey#208 = c_custkey#22)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_7329_output.getNumRow() << " -> tbl_JOIN_INNER_TD_7329_output" 
     << std::endl << "      #COL: 5: " << "ListBuffer(l_extendedprice#82, l_discount#83, s_nationkey#345, o_custkey#208, o_orderdate#211)" 
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_714_input.getNumRow() << " -> tbl_SerializeFromObject_TD_714_input" 
     << std::endl << "      #COL: 2: " << "ListBuffer(c_custkey#22, c_nationkey#25)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_6195_output.getNumRow() << " -> tbl_JOIN_INNER_TD_6195_output" 
     << std::endl << "      #COL: 5: " << "ListBuffer(l_extendedprice#82, l_discount#83, s_nationkey#345, o_orderdate#211, c_nationkey#25)" 
     << std::endl; 

    std::cout << "Filter_4: " << tvdiff(&tv_r_Filter_4_814_s, &tv_r_Filter_4_814_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer((isnotnull(r_name#175) AND (r_name#175 = MIDDLE EAST))) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_5477_input.getNumRow() << " -> tbl_SerializeFromObject_TD_5477_input" 
     << std::endl << "      #COL: 2: " << "ListBuffer(r_regionkey#174, r_name#175)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_4938_output.getNumRow() << " -> tbl_Filter_TD_4938_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(r_regionkey#174)" 
     << std::endl; 

    std::cout << "JOIN_INNER_5: " << tvdiff(&tv_r_JOIN_INNER_5_131_s, &tv_r_JOIN_INNER_5_131_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer((c_nationkey#25 = n_nationkey#144)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_6195_output.getNumRow() << " -> tbl_JOIN_INNER_TD_6195_output" 
     << std::endl << "      #COL: 5: " << "ListBuffer(l_extendedprice#82, l_discount#83, s_nationkey#345, o_orderdate#211, c_nationkey#25)" 
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_6357_input.getNumRow() << " -> tbl_SerializeFromObject_TD_6357_input" 
     << std::endl << "      #COL: 2: " << "ListBuffer(n_nationkey#144, n_regionkey#146)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_5728_output.getNumRow() << " -> tbl_JOIN_INNER_TD_5728_output" 
     << std::endl << "      #COL: 5: " << "ListBuffer(l_extendedprice#82, l_discount#83, s_nationkey#345, o_orderdate#211, n_regionkey#146)" 
     << std::endl; 

    std::cout << "JOIN_INNER_4: " << tvdiff(&tv_r_JOIN_INNER_4_55_s, &tv_r_JOIN_INNER_4_55_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer((n_regionkey#146 = r_regionkey#174)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_5728_output.getNumRow() << " -> tbl_JOIN_INNER_TD_5728_output" 
     << std::endl << "      #COL: 5: " << "ListBuffer(l_extendedprice#82, l_discount#83, s_nationkey#345, o_orderdate#211, n_regionkey#146)" 
     << std::endl << "      #ROW: " << tbl_Filter_TD_4938_output.getNumRow() << " -> tbl_Filter_TD_4938_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(r_regionkey#174)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_4754_output.getNumRow() << " -> tbl_JOIN_INNER_TD_4754_output" 
     << std::endl << "      #COL: 5: " << "ListBuffer(o_orderdate#211, l_extendedprice#82, l_discount#83, s_nationkey#345, n_regionkey#146)" 
     << std::endl; 

    std::cout << "JOIN_INNER_3: " << tvdiff(&tv_r_JOIN_INNER_3_989_s, &tv_r_JOIN_INNER_3_989_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer((s_nationkey#345 = n_nationkey#951)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_4754_output.getNumRow() << " -> tbl_JOIN_INNER_TD_4754_output" 
     << std::endl << "      #COL: 5: " << "ListBuffer(o_orderdate#211, l_extendedprice#82, l_discount#83, s_nationkey#345, n_regionkey#146)" 
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_4527_input.getNumRow() << " -> tbl_SerializeFromObject_TD_4527_input" 
     << std::endl << "      #COL: 2: " << "ListBuffer(n_nationkey#951, n_name#952)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_3699_output.getNumRow() << " -> tbl_JOIN_INNER_TD_3699_output" 
     << std::endl << "      #COL: 4: " << "ListBuffer(o_orderdate#211, l_extendedprice#82, l_discount#83, n_name#952)" 
     << std::endl; 

    std::cout << "Project_2: " << tvdiff(&tv_r_Project_2_580_s, &tv_r_Project_2_580_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(cast((cast(o_orderdate#211 as double) / 10000.0) as int) AS o_year#946, (l_extendedprice#82 * (100 - l_discount#83)) AS volume#947, n_name#952 AS nation#948) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_3699_output.getNumRow() << " -> tbl_JOIN_INNER_TD_3699_output" 
     << std::endl << "      #COL: 4: " << "ListBuffer(o_orderdate#211, l_extendedprice#82, l_discount#83, n_name#952)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Project_TD_2446_output.getNumRow() << " -> tbl_Project_TD_2446_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(o_year#946, volume#947, nation#948)" 
     << std::endl; 

    std::cout << "Aggregate_1: " << tvdiff(&tv_r_Aggregate_1_470_s, &tv_r_Aggregate_1_470_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(o_year#946, (cast(sum(CASE WHEN (nation#948 = IRAQ) THEN volume#947 ELSE 0 END) as double) / cast(sum(volume#947) as double)) AS mkt_share#949) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_Project_TD_2446_output.getNumRow() << " -> tbl_Project_TD_2446_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(o_year#946, volume#947, nation#948)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_1212_output.getNumRow() << " -> tbl_Aggregate_TD_1212_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(o_year#946, mkt_share#949)" 
     << std::endl; 

    std::cout << "Sort_0: " << tvdiff(&tv_r_Sort_0_232_s, &tv_r_Sort_0_232_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(o_year#946 ASC NULLS FIRST) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_1212_output.getNumRow() << " -> tbl_Aggregate_TD_1212_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(o_year#946, mkt_share#949)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Sort_TD_0588_output.getNumRow() << " -> tbl_Sort_TD_0588_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(o_year#946, mkt_share#949)" 
     << std::endl; 

    std::cout << std::endl << " Total execution time: " << tvdiff(&tv_r_s, &tv_r_e) / 1000 << " ms"; 

    std::cout << std::endl << " Spark elapsed time: " << 22.90656 * 1000 << "ms" << std::endl; 
    return 0; 
}
