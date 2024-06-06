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

#include "cfgFunc_q1.hpp" 
#include "q1.hpp" 

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
    std::cout << "NOTE:running query #1\n."; 
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
    // ****************************** Tables **************************** // 
    Table tbl_Sort_TD_0854_output("tbl_Sort_TD_0854_output", 6100000, 10, "");
    tbl_Sort_TD_0854_output.allocateHost();
    Table tbl_Aggregate_TD_1353_output_preprocess("tbl_Aggregate_TD_1353_output_preprocess", 10, 16, "");
    tbl_Aggregate_TD_1353_output_preprocess.selectOverlayVersion(0);
    tbl_Aggregate_TD_1353_output_preprocess.allocateHost();
    Table tbl_Aggregate_TD_1353_output("tbl_Aggregate_TD_1353_output", 10, 10, "");
    tbl_Aggregate_TD_1353_output.allocateHost();
    Table tbl_Filter_TD_2525_output("tbl_Filter_TD_2525_output", 6100000, 6, "");
    tbl_Filter_TD_2525_output.selectOverlayVersion(0);
    tbl_Filter_TD_2525_output.allocateHost();
    Table tbl_SerializeFromObject_TD_3718_input;
    tbl_SerializeFromObject_TD_3718_input = Table("lineitem", lineitem_n, 7, in_dir);
    tbl_SerializeFromObject_TD_3718_input.addCol("l_quantity", 4);
    tbl_SerializeFromObject_TD_3718_input.addCol("l_extendedprice", 4);
    tbl_SerializeFromObject_TD_3718_input.addCol("l_discount", 4);
    tbl_SerializeFromObject_TD_3718_input.addCol("l_tax", 4);
    tbl_SerializeFromObject_TD_3718_input.addCol("l_returnflag", 4);
    tbl_SerializeFromObject_TD_3718_input.addCol("l_linestatus", 4);
    tbl_SerializeFromObject_TD_3718_input.addCol("l_shipdate", 4);
    tbl_SerializeFromObject_TD_3718_input.allocateHost();
    tbl_SerializeFromObject_TD_3718_input.loadHost();
    // ********************** Allocate Device Buffer ******************** // 
    tbl_Aggregate_TD_1353_output_preprocess.allocateDevBuffer(context_a, 33);
    tbl_Filter_TD_2525_output.allocateDevBuffer(context_a, 32);
    // ****************************** Config **************************** // 
    AggrCfgCmd cfg_Aggregate_TD_1353_cmds;
    cfg_Aggregate_TD_1353_cmds.allocateHost();
    get_cfg_dat_Aggregate_TD_1353_gqe_aggr(cfg_Aggregate_TD_1353_cmds.cmd);
    cfg_Aggregate_TD_1353_cmds.allocateDevBuffer(context_a, 32);
    AggrCfgCmd cfg_Aggregate_TD_1353_cmds_out;
    cfg_Aggregate_TD_1353_cmds_out.allocateHost();
    cfg_Aggregate_TD_1353_cmds_out.allocateDevBuffer(context_a, 33);
    // *************************** Kernel Setup ************************* // 
    AggrBufferTmp buftmp_a(context_a, 1); 
    buftmp_a.BufferInitial(q_a); 
    std::cout << std::endl; 
    AggrKrnlEngine krnl_Aggregate_TD_1353;
    krnl_Aggregate_TD_1353 = AggrKrnlEngine(program_a, q_a, context_a, "gqeAggr");
    krnl_Aggregate_TD_1353.setup(tbl_Filter_TD_2525_output, tbl_Aggregate_TD_1353_output_preprocess, cfg_Aggregate_TD_1353_cmds, cfg_Aggregate_TD_1353_cmds_out, buftmp_a);
    // ************************** Transfer Engine *********************** // 
    transEngine trans_Aggregate_TD_1353;
    trans_Aggregate_TD_1353.setq(q_a, 0);
    trans_Aggregate_TD_1353.add(&(cfg_Aggregate_TD_1353_cmds));
    trans_Aggregate_TD_1353.add(&(cfg_Aggregate_TD_1353_cmds_out));
    transEngine trans_Aggregate_TD_1353_out;
    trans_Aggregate_TD_1353_out.setq(q_a, 0);
    q_a.finish();
    // ****************************** Events **************************** // 
    std::vector<cl::Event> events_h2d_wr_Aggregate_TD_1353;
    std::vector<cl::Event> events_d2h_rd_Aggregate_TD_1353;
    std::vector<cl::Event> events_Aggregate_TD_1353;
    events_h2d_wr_Aggregate_TD_1353.resize(1);
    events_d2h_rd_Aggregate_TD_1353.resize(1);
    events_Aggregate_TD_1353.resize(1);
    std::vector<cl::Event> events_grp_Aggregate_TD_1353;
    std::vector<cl::Event> prev_events_grp_Aggregate_TD_1353;
    // **************************** Operations ************************** // 
    struct timeval tv_r_s, tv_r_e; 
    gettimeofday(&tv_r_s, 0); 

    struct timeval tv_r_Filter_2_357_s, tv_r_Filter_2_357_e;
    gettimeofday(&tv_r_Filter_2_357_s, 0);
    SW_Filter_TD_2525(tbl_SerializeFromObject_TD_3718_input, tbl_Filter_TD_2525_output);
    gettimeofday(&tv_r_Filter_2_357_e, 0);

    struct timeval tv_r_Aggregate_1_188_s, tv_r_Aggregate_1_188_e;
    gettimeofday(&tv_r_Aggregate_1_188_s, 0);
    trans_Aggregate_TD_1353.add(&(tbl_Filter_TD_2525_output));
    trans_Aggregate_TD_1353.host2dev(0, &(prev_events_grp_Aggregate_TD_1353), &(events_h2d_wr_Aggregate_TD_1353[0]));
    events_grp_Aggregate_TD_1353.push_back(events_h2d_wr_Aggregate_TD_1353[0]);
    krnl_Aggregate_TD_1353.run(0, &(events_grp_Aggregate_TD_1353), &(events_Aggregate_TD_1353[0]));
    
    trans_Aggregate_TD_1353_out.add(&(tbl_Aggregate_TD_1353_output_preprocess));
    trans_Aggregate_TD_1353_out.add(&(cfg_Aggregate_TD_1353_cmds_out));
    trans_Aggregate_TD_1353_out.dev2host(0, &(events_Aggregate_TD_1353), &(events_d2h_rd_Aggregate_TD_1353[0]));
    q_a.flush();
    q_a.finish();
    
    SW_Aggregate_TD_1353_consolidate(tbl_Aggregate_TD_1353_output_preprocess, tbl_Aggregate_TD_1353_output);
    gettimeofday(&tv_r_Aggregate_1_188_e, 0);

    struct timeval tv_r_Sort_0_978_s, tv_r_Sort_0_978_e;
    gettimeofday(&tv_r_Sort_0_978_s, 0);
    SW_Sort_TD_0854(tbl_Aggregate_TD_1353_output, tbl_Sort_TD_0854_output);
    gettimeofday(&tv_r_Sort_0_978_e, 0);

std::cout << "CPU TIME: " << (tvdiff(&tv_r_Filter_2_357_s, &tv_r_Filter_2_357_e) + 
tvdiff(&tv_r_Sort_0_978_s, &tv_r_Sort_0_978_e) + 
1) / 1000.0 << std::endl;

std::cout << "FPGA TIME: " << (tvdiff(&tv_r_Aggregate_1_188_s, &tv_r_Aggregate_1_188_e) + 
1) / 1000.0 << std::endl;

    gettimeofday(&tv_r_e, 0); 
    // **************************** Print Execution Time ************************** // 
    std::cout << std::endl; 
    std::cout << "Filter_2: " << tvdiff(&tv_r_Filter_2_357_s, &tv_r_Filter_2_357_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer((l_shipdate#87 <= 19980803)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_3718_input.getNumRow() << " -> tbl_SerializeFromObject_TD_3718_input" 
     << std::endl << "      #COL: 7: " << "ListBuffer(l_quantity#81, l_extendedprice#82, l_discount#83, l_tax#84, l_returnflag#85, l_linestatus#86, l_shipdate#87)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_2525_output.getNumRow() << " -> tbl_Filter_TD_2525_output" 
     << std::endl << "      #COL: 6: " << "ListBuffer(l_quantity#81, l_extendedprice#82, l_discount#83, l_tax#84, l_returnflag#85, l_linestatus#86)" 
     << std::endl; 

    std::cout << "Aggregate_1: " << tvdiff(&tv_r_Aggregate_1_188_s, &tv_r_Aggregate_1_188_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer(l_returnflag#85, l_linestatus#86, sum(l_quantity#81) AS sum_qty#364L, sum(l_extendedprice#82) AS sum_base_price#365L, sum((l_extendedprice#82 * (100 - l_discount#83))) AS sum_disc_price#366L, sum(((l_extendedprice#82 * (100 - l_discount#83)) * (100 + l_tax#84))) AS sum_charge#367L, avg(l_quantity#81) AS avg_qty#368, avg(l_extendedprice#82) AS avg_price#369, avg(l_discount#83) AS avg_disc#370, count(1) AS count_order#371L) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_2525_output.getNumRow() << " -> tbl_Filter_TD_2525_output" 
     << std::endl << "      #COL: 6: " << "ListBuffer(l_quantity#81, l_extendedprice#82, l_discount#83, l_tax#84, l_returnflag#85, l_linestatus#86)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_1353_output.getNumRow() << " -> tbl_Aggregate_TD_1353_output" 
     << std::endl << "      #COL: 10: " << "ListBuffer(l_returnflag#85, l_linestatus#86, sum_qty#364L, sum_base_price#365L, sum_disc_price#366L, sum_charge#367L, avg_qty#368, avg_price#369, avg_disc#370, count_order#371L)" 
     << std::endl; 

    std::cout << "Sort_0: " << tvdiff(&tv_r_Sort_0_978_s, &tv_r_Sort_0_978_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(l_returnflag#85 ASC NULLS FIRST, l_linestatus#86 ASC NULLS FIRST) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_1353_output.getNumRow() << " -> tbl_Aggregate_TD_1353_output" 
     << std::endl << "      #COL: 10: " << "ListBuffer(l_returnflag#85, l_linestatus#86, sum_qty#364L, sum_base_price#365L, sum_disc_price#366L, sum_charge#367L, avg_qty#368, avg_price#369, avg_disc#370, count_order#371L)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Sort_TD_0854_output.getNumRow() << " -> tbl_Sort_TD_0854_output" 
     << std::endl << "      #COL: 10: " << "ListBuffer(l_returnflag#85, l_linestatus#86, sum_qty#364L, sum_base_price#365L, sum_disc_price#366L, sum_charge#367L, avg_qty#368, avg_price#369, avg_disc#370, count_order#371L)" 
     << std::endl; 

    std::cout << std::endl << " Total execution time: " << tvdiff(&tv_r_s, &tv_r_e) / 1000 << " ms"; 

    std::cout << std::endl << " Spark elapsed time: " << 23.636158 * 1000 << "ms" << std::endl; 
    return 0; 
}
