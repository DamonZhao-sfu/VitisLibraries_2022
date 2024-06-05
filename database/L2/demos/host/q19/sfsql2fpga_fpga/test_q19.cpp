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

#include "cfgFunc_q19.hpp" 
#include "q19.hpp" 

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
    std::cout << "NOTE:running query #19\n."; 
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
    Table tbl_Aggregate_TD_0482_output("tbl_Aggregate_TD_0482_output", 183000000, 1, "");
    tbl_Aggregate_TD_0482_output.allocateHost();
    Table tbl_JOIN_INNER_TD_1494_output("tbl_JOIN_INNER_TD_1494_output", 183000000, 2, "");
    tbl_JOIN_INNER_TD_1494_output.allocateHost();
    Table tbl_Filter_TD_2633_output("tbl_Filter_TD_2633_output", 183000000, 4, "");
    tbl_Filter_TD_2633_output.allocateHost();
    Table tbl_Filter_TD_2493_output("tbl_Filter_TD_2493_output", 183000000, 4, "");
    tbl_Filter_TD_2493_output.allocateHost();
    Table tbl_SerializeFromObject_TD_3247_input;
    tbl_SerializeFromObject_TD_3247_input = Table("lineitem", lineitem_n, 6, in_dir);
    tbl_SerializeFromObject_TD_3247_input.addCol("l_partkey", 4);
    tbl_SerializeFromObject_TD_3247_input.addCol("l_quantity", 4);
    tbl_SerializeFromObject_TD_3247_input.addCol("l_extendedprice", 4);
    tbl_SerializeFromObject_TD_3247_input.addCol("l_discount", 4);
    tbl_SerializeFromObject_TD_3247_input.addCol("l_shipinstruct", TPCH_READ_MAXAGG_LEN+1);
    tbl_SerializeFromObject_TD_3247_input.addCol("l_shipmode", TPCH_READ_MAXAGG_LEN+1);
    tbl_SerializeFromObject_TD_3247_input.allocateHost();
    tbl_SerializeFromObject_TD_3247_input.loadHost();
    Table tbl_SerializeFromObject_TD_3246_input;
    tbl_SerializeFromObject_TD_3246_input = Table("part", part_n, 4, in_dir);
    tbl_SerializeFromObject_TD_3246_input.addCol("p_partkey", 4);
    tbl_SerializeFromObject_TD_3246_input.addCol("p_brand", TPCH_READ_P_BRND_LEN+1);
    tbl_SerializeFromObject_TD_3246_input.addCol("p_size", 4);
    tbl_SerializeFromObject_TD_3246_input.addCol("p_container", TPCH_READ_P_CNTR_LEN+1);
    tbl_SerializeFromObject_TD_3246_input.allocateHost();
    tbl_SerializeFromObject_TD_3246_input.loadHost();
    // ********************** Allocate Device Buffer ******************** // 
    // ****************************** Config **************************** // 
    // *************************** Kernel Setup ************************* // 
    std::cout << std::endl; 
    // ************************** Transfer Engine *********************** // 
    // ****************************** Events **************************** // 
    // **************************** Operations ************************** // 
    struct timeval tv_r_s, tv_r_e; 
    gettimeofday(&tv_r_s, 0); 

    struct timeval tv_r_Filter_2_331_s, tv_r_Filter_2_331_e;
    gettimeofday(&tv_r_Filter_2_331_s, 0);
    SW_Filter_TD_2493(tbl_SerializeFromObject_TD_3246_input, tbl_Filter_TD_2493_output);
    gettimeofday(&tv_r_Filter_2_331_e, 0);

    struct timeval tv_r_Filter_2_874_s, tv_r_Filter_2_874_e;
    gettimeofday(&tv_r_Filter_2_874_s, 0);
    SW_Filter_TD_2633(tbl_SerializeFromObject_TD_3247_input, tbl_Filter_TD_2633_output);
    gettimeofday(&tv_r_Filter_2_874_e, 0);

    struct timeval tv_r_JOIN_INNER_1_459_s, tv_r_JOIN_INNER_1_459_e;
    gettimeofday(&tv_r_JOIN_INNER_1_459_s, 0);
    SW_JOIN_INNER_TD_1494(tbl_Filter_TD_2633_output, tbl_Filter_TD_2493_output, tbl_JOIN_INNER_TD_1494_output);
    gettimeofday(&tv_r_JOIN_INNER_1_459_e, 0);

    struct timeval tv_r_Aggregate_0_758_s, tv_r_Aggregate_0_758_e;
    gettimeofday(&tv_r_Aggregate_0_758_s, 0);
    SW_Aggregate_TD_0482(tbl_JOIN_INNER_TD_1494_output, tbl_Aggregate_TD_0482_output);
    gettimeofday(&tv_r_Aggregate_0_758_e, 0);

std::cout << "CPU TIME: " << (tvdiff(&tv_r_Filter_2_331_s, &tv_r_Filter_2_331_e) + 
tvdiff(&tv_r_Filter_2_874_s, &tv_r_Filter_2_874_e) + 
tvdiff(&tv_r_JOIN_INNER_1_459_s, &tv_r_JOIN_INNER_1_459_e) + 
tvdiff(&tv_r_Aggregate_0_758_s, &tv_r_Aggregate_0_758_e) + 
1) / 1000.0 << std::endl;

std::cout << "FPGA TIME: " << (1) / 1000.0 << std::endl;

    gettimeofday(&tv_r_e, 0); 
    // **************************** Print Execution Time ************************** // 
    std::cout << std::endl; 
    std::cout << "Filter_2: " << tvdiff(&tv_r_Filter_2_331_s, &tv_r_Filter_2_331_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(((p_size#263 >= 1) AND (((((p_brand#261 = Brand#51) AND p_container#264 IN (SM CASE,SM BOX,SM PACK,SM PKG)) AND (p_size#263 <= 5)) OR (((p_brand#261 = Brand#32) AND p_container#264 IN (MED BAG,MED BOX,MED PKG,MED PACK)) AND (p_size#263 <= 10))) OR (((p_brand#261 = Brand#12) AND p_container#264 IN (LG CASE,LG BOX,LG PACK,LG PKG)) AND (p_size#263 <= 15))))) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_3246_input.getNumRow() << " -> tbl_SerializeFromObject_TD_3246_input" 
     << std::endl << "      #COL: 4: " << "ListBuffer(p_partkey#258, p_brand#261, p_size#263, p_container#264)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_2493_output.getNumRow() << " -> tbl_Filter_TD_2493_output" 
     << std::endl << "      #COL: 4: " << "ListBuffer(p_partkey#258, p_brand#261, p_size#263, p_container#264)" 
     << std::endl; 

    std::cout << "Filter_2: " << tvdiff(&tv_r_Filter_2_874_s, &tv_r_Filter_2_874_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(((isnotnull(l_shipinstruct#90) AND (l_shipmode#91 IN (AIR,AIR REG) AND (l_shipinstruct#90 = DELIVER IN PERSON))) AND ((((l_quantity#81 >= 7) AND (l_quantity#81 <= 17)) OR ((l_quantity#81 >= 10) AND (l_quantity#81 <= 20))) OR ((l_quantity#81 >= 24) AND (l_quantity#81 <= 34))))) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_3247_input.getNumRow() << " -> tbl_SerializeFromObject_TD_3247_input" 
     << std::endl << "      #COL: 6: " << "ListBuffer(l_partkey#78, l_quantity#81, l_extendedprice#82, l_discount#83, l_shipinstruct#90, l_shipmode#91)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_2633_output.getNumRow() << " -> tbl_Filter_TD_2633_output" 
     << std::endl << "      #COL: 4: " << "ListBuffer(l_partkey#78, l_quantity#81, l_extendedprice#82, l_discount#83)" 
     << std::endl; 

    std::cout << "JOIN_INNER_1: " << tvdiff(&tv_r_JOIN_INNER_1_459_s, &tv_r_JOIN_INNER_1_459_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(((p_partkey#258 = l_partkey#78) AND (((((((p_brand#261 = Brand#51) AND p_container#264 IN (SM CASE,SM BOX,SM PACK,SM PKG)) AND (l_quantity#81 >= 7)) AND (l_quantity#81 <= 17)) AND (p_size#263 <= 5)) OR (((((p_brand#261 = Brand#32) AND p_container#264 IN (MED BAG,MED BOX,MED PKG,MED PACK)) AND (l_quantity#81 >= 10)) AND (l_quantity#81 <= 20)) AND (p_size#263 <= 10))) OR (((((p_brand#261 = Brand#12) AND p_container#264 IN (LG CASE,LG BOX,LG PACK,LG PKG)) AND (l_quantity#81 >= 24)) AND (l_quantity#81 <= 34)) AND (p_size#263 <= 15))))) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_2633_output.getNumRow() << " -> tbl_Filter_TD_2633_output" 
     << std::endl << "      #COL: 4: " << "ListBuffer(l_partkey#78, l_quantity#81, l_extendedprice#82, l_discount#83)" 
     << std::endl << "      #ROW: " << tbl_Filter_TD_2493_output.getNumRow() << " -> tbl_Filter_TD_2493_output" 
     << std::endl << "      #COL: 4: " << "ListBuffer(p_partkey#258, p_brand#261, p_size#263, p_container#264)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_1494_output.getNumRow() << " -> tbl_JOIN_INNER_TD_1494_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(l_extendedprice#82, l_discount#83)" 
     << std::endl; 

    std::cout << "Aggregate_0: " << tvdiff(&tv_r_Aggregate_0_758_s, &tv_r_Aggregate_0_758_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(sum((l_extendedprice#82 * (100 - l_discount#83))) AS revenue#1822L) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_1494_output.getNumRow() << " -> tbl_JOIN_INNER_TD_1494_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(l_extendedprice#82, l_discount#83)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_0482_output.getNumRow() << " -> tbl_Aggregate_TD_0482_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(revenue#1822L)" 
     << std::endl; 

    std::cout << std::endl << " Total execution time: " << tvdiff(&tv_r_s, &tv_r_e) / 1000 << " ms"; 

    std::cout << std::endl << " Spark elapsed time: " << 24.293932 * 1000 << "ms" << std::endl; 
    return 0; 
}
