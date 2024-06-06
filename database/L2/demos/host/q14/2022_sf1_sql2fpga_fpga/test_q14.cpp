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

#include "cfgFunc_q14.hpp" 
#include "q14.hpp" 

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
    std::cout << "NOTE:running query #14\n."; 
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
    Table tbl_Aggregate_TD_0120_output("tbl_Aggregate_TD_0120_output", 6100000, 1, "");
    tbl_Aggregate_TD_0120_output.allocateHost();
    Table tbl_JOIN_INNER_TD_1359_output("tbl_JOIN_INNER_TD_1359_output", 6100000, 3, "");
    tbl_JOIN_INNER_TD_1359_output.allocateHost();
    Table tbl_Filter_TD_2276_output("tbl_Filter_TD_2276_output", 6100000, 3, "");
    tbl_Filter_TD_2276_output.allocateHost();
    Table tbl_SerializeFromObject_TD_2698_input;
    tbl_SerializeFromObject_TD_2698_input = Table("part", part_n, 2, in_dir);
    tbl_SerializeFromObject_TD_2698_input.addCol("p_partkey", 4);
    tbl_SerializeFromObject_TD_2698_input.addCol("p_type", TPCH_READ_P_TYPE_LEN+1);
    tbl_SerializeFromObject_TD_2698_input.allocateHost();
    tbl_SerializeFromObject_TD_2698_input.loadHost();
    Table tbl_SerializeFromObject_TD_3202_input;
    tbl_SerializeFromObject_TD_3202_input = Table("lineitem", lineitem_n, 4, in_dir);
    tbl_SerializeFromObject_TD_3202_input.addCol("l_partkey", 4);
    tbl_SerializeFromObject_TD_3202_input.addCol("l_extendedprice", 4);
    tbl_SerializeFromObject_TD_3202_input.addCol("l_discount", 4);
    tbl_SerializeFromObject_TD_3202_input.addCol("l_shipdate", 4);
    tbl_SerializeFromObject_TD_3202_input.allocateHost();
    tbl_SerializeFromObject_TD_3202_input.loadHost();
    // ********************** Allocate Device Buffer ******************** // 
    // ****************************** Config **************************** // 
    // *************************** Kernel Setup ************************* // 
    std::cout << std::endl; 
    // ************************** Transfer Engine *********************** // 
    // ****************************** Events **************************** // 
    // **************************** Operations ************************** // 
    struct timeval tv_r_s, tv_r_e; 
    gettimeofday(&tv_r_s, 0); 

    struct timeval tv_r_Filter_2_549_s, tv_r_Filter_2_549_e;
    gettimeofday(&tv_r_Filter_2_549_s, 0);
    SW_Filter_TD_2276(tbl_SerializeFromObject_TD_3202_input, tbl_Filter_TD_2276_output);
    gettimeofday(&tv_r_Filter_2_549_e, 0);

    struct timeval tv_r_JOIN_INNER_1_670_s, tv_r_JOIN_INNER_1_670_e;
    gettimeofday(&tv_r_JOIN_INNER_1_670_s, 0);
    SW_JOIN_INNER_TD_1359(tbl_Filter_TD_2276_output, tbl_SerializeFromObject_TD_2698_input, tbl_JOIN_INNER_TD_1359_output);
    gettimeofday(&tv_r_JOIN_INNER_1_670_e, 0);

    struct timeval tv_r_Aggregate_0_967_s, tv_r_Aggregate_0_967_e;
    gettimeofday(&tv_r_Aggregate_0_967_s, 0);
    SW_Aggregate_TD_0120(tbl_JOIN_INNER_TD_1359_output, tbl_Aggregate_TD_0120_output);
    gettimeofday(&tv_r_Aggregate_0_967_e, 0);

std::cout << "CPU TIME: " << (tvdiff(&tv_r_Filter_2_549_s, &tv_r_Filter_2_549_e) + 
tvdiff(&tv_r_JOIN_INNER_1_670_s, &tv_r_JOIN_INNER_1_670_e) + 
tvdiff(&tv_r_Aggregate_0_967_s, &tv_r_Aggregate_0_967_e) + 
1) / 1000.0 << std::endl;

std::cout << "FPGA TIME: " << (1) / 1000.0 << std::endl;

    gettimeofday(&tv_r_e, 0); 
    // **************************** Print Execution Time ************************** // 
    std::cout << std::endl; 
    std::cout << "Filter_2: " << tvdiff(&tv_r_Filter_2_549_s, &tv_r_Filter_2_549_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(((l_shipdate#87 >= 19970301) AND (l_shipdate#87 < 19970401))) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_3202_input.getNumRow() << " -> tbl_SerializeFromObject_TD_3202_input" 
     << std::endl << "      #COL: 4: " << "ListBuffer(l_partkey#78, l_extendedprice#82, l_discount#83, l_shipdate#87)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_2276_output.getNumRow() << " -> tbl_Filter_TD_2276_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(l_partkey#78, l_extendedprice#82, l_discount#83)" 
     << std::endl; 

    std::cout << "JOIN_INNER_1: " << tvdiff(&tv_r_JOIN_INNER_1_670_s, &tv_r_JOIN_INNER_1_670_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer((l_partkey#78 = p_partkey#258)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_2276_output.getNumRow() << " -> tbl_Filter_TD_2276_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(l_partkey#78, l_extendedprice#82, l_discount#83)" 
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_2698_input.getNumRow() << " -> tbl_SerializeFromObject_TD_2698_input" 
     << std::endl << "      #COL: 2: " << "ListBuffer(p_partkey#258, p_type#262)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_1359_output.getNumRow() << " -> tbl_JOIN_INNER_TD_1359_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(l_extendedprice#82, l_discount#83, p_type#262)" 
     << std::endl; 

    std::cout << "Aggregate_0: " << tvdiff(&tv_r_Aggregate_0_967_s, &tv_r_Aggregate_0_967_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer((cast((100 * sum(CASE WHEN StartsWith(p_type#262, PROMO) THEN (l_extendedprice#82 * (100 - l_discount#83)) ELSE 0 END)) as double) / cast(sum((l_extendedprice#82 * (100 - l_discount#83))) as double)) AS promo_revenue#1407) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_1359_output.getNumRow() << " -> tbl_JOIN_INNER_TD_1359_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(l_extendedprice#82, l_discount#83, p_type#262)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_0120_output.getNumRow() << " -> tbl_Aggregate_TD_0120_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(promo_revenue#1407)" 
     << std::endl; 

    std::cout << std::endl << " Total execution time: " << tvdiff(&tv_r_s, &tv_r_e) / 1000 << " ms"; 

    std::cout << std::endl << " Spark elapsed time: " << 19.42474 * 1000 << "ms" << std::endl; 
    return 0; 
}
