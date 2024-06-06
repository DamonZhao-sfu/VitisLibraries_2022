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
    // ****************************** Tables **************************** // 
    Table tbl_Sort_TD_0621_output("tbl_Sort_TD_0621_output", 6100000, 5, "");
    tbl_Sort_TD_0621_output.allocateHost();
    Table tbl_JOIN_INNER_TD_1693_output("tbl_JOIN_INNER_TD_1693_output", 6100000, 5, "");
    tbl_JOIN_INNER_TD_1693_output.allocateHost();
    Table tbl_SerializeFromObject_TD_2224_input_stringRowIDSubstitute;
    tbl_SerializeFromObject_TD_2224_input_stringRowIDSubstitute = Table("supplier", supplier_n, 4, in_dir);
    tbl_SerializeFromObject_TD_2224_input_stringRowIDSubstitute.addCol("s_suppkey", 4);
    tbl_SerializeFromObject_TD_2224_input_stringRowIDSubstitute.addCol("s_name", 4, 1, 0);
    tbl_SerializeFromObject_TD_2224_input_stringRowIDSubstitute.addCol("s_address", 4, 1, 0);
    tbl_SerializeFromObject_TD_2224_input_stringRowIDSubstitute.addCol("s_phone", 4, 1, 0);
    tbl_SerializeFromObject_TD_2224_input_stringRowIDSubstitute.allocateHost();
    tbl_SerializeFromObject_TD_2224_input_stringRowIDSubstitute.loadHost();
    Table tbl_SerializeFromObject_TD_2224_input;
    tbl_SerializeFromObject_TD_2224_input = Table("supplier", supplier_n, 4, in_dir);
    tbl_SerializeFromObject_TD_2224_input.addCol("s_suppkey", 4);
    tbl_SerializeFromObject_TD_2224_input.addCol("s_name", TPCH_READ_S_NAME_LEN+1);
    tbl_SerializeFromObject_TD_2224_input.addCol("s_address", TPCH_READ_S_ADDR_MAX+1);
    tbl_SerializeFromObject_TD_2224_input.addCol("s_phone", TPCH_READ_PHONE_LEN+1);
    tbl_SerializeFromObject_TD_2224_input.allocateHost();
    tbl_SerializeFromObject_TD_2224_input.loadHost();
    Table tbl_Filter_TD_2923_output("tbl_Filter_TD_2923_output", 6100000, 2, "");
    tbl_Filter_TD_2923_output.allocateHost();
    Table tbl_Aggregate_TD_4283_output_preprocess("tbl_Aggregate_TD_4283_output_preprocess", 20000, 16, "");
    tbl_Aggregate_TD_4283_output_preprocess.selectOverlayVersion(0);
    tbl_Aggregate_TD_4283_output_preprocess.allocateHost();
    Table tbl_Aggregate_TD_4283_output("tbl_Aggregate_TD_4283_output", 20000, 2, "");
    tbl_Aggregate_TD_4283_output.allocateHost();
    Table tbl_Aggregate_TD_4258_output("tbl_Aggregate_TD_4258_output", 6100000, 1, "");
    tbl_Aggregate_TD_4258_output.allocateHost();
    Table tbl_Aggregate_TD_5224_output("tbl_Aggregate_TD_5224_output", 6100000, 1, "");
    tbl_Aggregate_TD_5224_output.allocateHost();
    Table tbl_Filter_TD_6714_output("tbl_Filter_TD_6714_output", 6100000, 3, "");
    tbl_Filter_TD_6714_output.selectOverlayVersion(0);
    tbl_Filter_TD_6714_output.allocateHost();
    Table tbl_SerializeFromObject_TD_7305_input;
    tbl_SerializeFromObject_TD_7305_input = Table("lineitem", lineitem_n, 4, in_dir);
    tbl_SerializeFromObject_TD_7305_input.addCol("l_suppkey", 4);
    tbl_SerializeFromObject_TD_7305_input.addCol("l_extendedprice", 4);
    tbl_SerializeFromObject_TD_7305_input.addCol("l_discount", 4);
    tbl_SerializeFromObject_TD_7305_input.addCol("l_shipdate", 4);
    tbl_SerializeFromObject_TD_7305_input.allocateHost();
    tbl_SerializeFromObject_TD_7305_input.loadHost();
    // ********************** Allocate Device Buffer ******************** // 
    tbl_Aggregate_TD_4283_output_preprocess.allocateDevBuffer(context_a, 33);
    tbl_Filter_TD_6714_output.allocateDevBuffer(context_a, 32);
    // ****************************** Config **************************** // 
    AggrCfgCmd cfg_Aggregate_TD_4283_cmds;
    cfg_Aggregate_TD_4283_cmds.allocateHost();
    get_cfg_dat_Aggregate_TD_4283_gqe_aggr(cfg_Aggregate_TD_4283_cmds.cmd);
    cfg_Aggregate_TD_4283_cmds.allocateDevBuffer(context_a, 32);
    AggrCfgCmd cfg_Aggregate_TD_4283_cmds_out;
    cfg_Aggregate_TD_4283_cmds_out.allocateHost();
    cfg_Aggregate_TD_4283_cmds_out.allocateDevBuffer(context_a, 33);
    // *************************** Kernel Setup ************************* // 
    AggrBufferTmp buftmp_a(context_a, 1); 
    buftmp_a.BufferInitial(q_a); 
    std::cout << std::endl; 
    AggrKrnlEngine krnl_Aggregate_TD_4283;
    krnl_Aggregate_TD_4283 = AggrKrnlEngine(program_a, q_a, context_a, "gqeAggr");
    krnl_Aggregate_TD_4283.setup(tbl_Filter_TD_6714_output, tbl_Aggregate_TD_4283_output_preprocess, cfg_Aggregate_TD_4283_cmds, cfg_Aggregate_TD_4283_cmds_out, buftmp_a);
    // ************************** Transfer Engine *********************** // 
    transEngine trans_Aggregate_TD_4283;
    trans_Aggregate_TD_4283.setq(q_a, 0);
    trans_Aggregate_TD_4283.add(&(cfg_Aggregate_TD_4283_cmds));
    trans_Aggregate_TD_4283.add(&(cfg_Aggregate_TD_4283_cmds_out));
    transEngine trans_Aggregate_TD_4283_out;
    trans_Aggregate_TD_4283_out.setq(q_a, 0);
    q_a.finish();
    // ****************************** Events **************************** // 
    std::vector<cl::Event> events_h2d_wr_Aggregate_TD_4283;
    std::vector<cl::Event> events_d2h_rd_Aggregate_TD_4283;
    std::vector<cl::Event> events_Aggregate_TD_4283;
    events_h2d_wr_Aggregate_TD_4283.resize(1);
    events_d2h_rd_Aggregate_TD_4283.resize(1);
    events_Aggregate_TD_4283.resize(1);
    std::vector<cl::Event> events_grp_Aggregate_TD_4283;
    std::vector<cl::Event> prev_events_grp_Aggregate_TD_4283;
    // **************************** Operations ************************** // 
    struct timeval tv_r_s, tv_r_e; 
    gettimeofday(&tv_r_s, 0); 

    struct timeval tv_r_Filter_6_716_s, tv_r_Filter_6_716_e;
    gettimeofday(&tv_r_Filter_6_716_s, 0);
    SW_Filter_TD_6714(tbl_SerializeFromObject_TD_7305_input, tbl_Filter_TD_6714_output);
    gettimeofday(&tv_r_Filter_6_716_e, 0);

    struct timeval tv_r_Aggregate_5_0_s, tv_r_Aggregate_5_0_e;
    gettimeofday(&tv_r_Aggregate_5_0_s, 0);
    SW_Aggregate_TD_5224(tbl_Filter_TD_6714_output, tbl_Aggregate_TD_5224_output);
    gettimeofday(&tv_r_Aggregate_5_0_e, 0);

    struct timeval tv_r_Aggregate_4_36_s, tv_r_Aggregate_4_36_e;
    gettimeofday(&tv_r_Aggregate_4_36_s, 0);
    SW_Aggregate_TD_4258(tbl_Aggregate_TD_5224_output, tbl_Aggregate_TD_4258_output);
    gettimeofday(&tv_r_Aggregate_4_36_e, 0);

    struct timeval tv_r_Aggregate_4_47_s, tv_r_Aggregate_4_47_e;
    gettimeofday(&tv_r_Aggregate_4_47_s, 0);
    trans_Aggregate_TD_4283.add(&(tbl_Filter_TD_6714_output));
    trans_Aggregate_TD_4283.host2dev(0, &(prev_events_grp_Aggregate_TD_4283), &(events_h2d_wr_Aggregate_TD_4283[0]));
    events_grp_Aggregate_TD_4283.push_back(events_h2d_wr_Aggregate_TD_4283[0]);
    krnl_Aggregate_TD_4283.run(0, &(events_grp_Aggregate_TD_4283), &(events_Aggregate_TD_4283[0]));
    
    trans_Aggregate_TD_4283_out.add(&(tbl_Aggregate_TD_4283_output_preprocess));
    trans_Aggregate_TD_4283_out.add(&(cfg_Aggregate_TD_4283_cmds_out));
    trans_Aggregate_TD_4283_out.dev2host(0, &(events_Aggregate_TD_4283), &(events_d2h_rd_Aggregate_TD_4283[0]));
    q_a.flush();
    q_a.finish();
    
    SW_Aggregate_TD_4283_consolidate(tbl_Aggregate_TD_4283_output_preprocess, tbl_Aggregate_TD_4283_output);
    gettimeofday(&tv_r_Aggregate_4_47_e, 0);

    struct timeval tv_r_Filter_2_274_s, tv_r_Filter_2_274_e;
    gettimeofday(&tv_r_Filter_2_274_s, 0);
    SW_Filter_TD_2923(tbl_Aggregate_TD_4283_output, tbl_Aggregate_TD_4258_output, tbl_Filter_TD_2923_output);
    gettimeofday(&tv_r_Filter_2_274_e, 0);

    struct timeval tv_r_JOIN_INNER_1_861_s, tv_r_JOIN_INNER_1_861_e;
    gettimeofday(&tv_r_JOIN_INNER_1_861_s, 0);
    SW_JOIN_INNER_TD_1693(tbl_SerializeFromObject_TD_2224_input_stringRowIDSubstitute, tbl_Filter_TD_2923_output, tbl_JOIN_INNER_TD_1693_output);
    gettimeofday(&tv_r_JOIN_INNER_1_861_e, 0);

    struct timeval tv_r_Sort_0_567_s, tv_r_Sort_0_567_e;
    gettimeofday(&tv_r_Sort_0_567_s, 0);
    SW_Sort_TD_0621(tbl_JOIN_INNER_TD_1693_output, tbl_SerializeFromObject_TD_2224_input, tbl_Sort_TD_0621_output);
    gettimeofday(&tv_r_Sort_0_567_e, 0);

std::cout << "CPU TIME: " << (tvdiff(&tv_r_Filter_6_716_s, &tv_r_Filter_6_716_e) + 
tvdiff(&tv_r_Aggregate_5_0_s, &tv_r_Aggregate_5_0_e) + 
tvdiff(&tv_r_Aggregate_4_36_s, &tv_r_Aggregate_4_36_e) + 
tvdiff(&tv_r_Filter_2_274_s, &tv_r_Filter_2_274_e) + 
tvdiff(&tv_r_JOIN_INNER_1_861_s, &tv_r_JOIN_INNER_1_861_e) + 
tvdiff(&tv_r_Sort_0_567_s, &tv_r_Sort_0_567_e) + 
1) / 1000.0 << std::endl;

std::cout << "FPGA TIME: " << (tvdiff(&tv_r_Aggregate_4_47_s, &tv_r_Aggregate_4_47_e) + 
1) / 1000.0 << std::endl;

    gettimeofday(&tv_r_e, 0); 
    // **************************** Print Execution Time ************************** // 
    std::cout << std::endl; 
    std::cout << "Filter_6: " << tvdiff(&tv_r_Filter_6_716_s, &tv_r_Filter_6_716_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(((l_shipdate#87 >= 19960101) AND (l_shipdate#87 < 19960401))) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_7305_input.getNumRow() << " -> tbl_SerializeFromObject_TD_7305_input" 
     << std::endl << "      #COL: 4: " << "ListBuffer(l_suppkey#79, l_extendedprice#82, l_discount#83, l_shipdate#87)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_6714_output.getNumRow() << " -> tbl_Filter_TD_6714_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(l_suppkey#79, l_extendedprice#82, l_discount#83)" 
     << std::endl; 

    std::cout << "Aggregate_5: " << tvdiff(&tv_r_Aggregate_5_0_s, &tv_r_Aggregate_5_0_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(l_suppkey#79, sum((l_extendedprice#82 * (1 - l_discount#83))) AS total_revenue#1461L) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_6714_output.getNumRow() << " -> tbl_Filter_TD_6714_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(l_suppkey#79, l_extendedprice#82, l_discount#83)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_5224_output.getNumRow() << " -> tbl_Aggregate_TD_5224_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(total_revenue#1461L)" 
     << std::endl; 

    std::cout << "Aggregate_4: " << tvdiff(&tv_r_Aggregate_4_36_s, &tv_r_Aggregate_4_36_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(max(total_revenue#1461L) AS max(total_revenue)#1463L) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_5224_output.getNumRow() << " -> tbl_Aggregate_TD_5224_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(total_revenue#1461L)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_4258_output.getNumRow() << " -> tbl_Aggregate_TD_4258_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(max(total_revenue)#1463L)" 
     << std::endl; 

    std::cout << "Aggregate_4: " << tvdiff(&tv_r_Aggregate_4_47_s, &tv_r_Aggregate_4_47_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer(l_suppkey#79, sum((l_extendedprice#82 * (1 - l_discount#83))) AS sum((l_extendedprice * (1 - l_discount)))#1459L) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_6714_output.getNumRow() << " -> tbl_Filter_TD_6714_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(l_suppkey#79, l_extendedprice#82, l_discount#83)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_4283_output.getNumRow() << " -> tbl_Aggregate_TD_4283_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(l_suppkey#79, sum((l_extendedprice * (1 - l_discount)))#1459L)" 
     << std::endl; 

    std::cout << "Filter_2: " << tvdiff(&tv_r_Filter_2_274_s, &tv_r_Filter_2_274_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer((isnotnull(sum((l_extendedprice * (1 - l_discount)))#1459L) AND (sum((l_extendedprice * (1 - l_discount)))#1459L = scalar-subquery#1457 []))) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_4283_output.getNumRow() << " -> tbl_Aggregate_TD_4283_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(l_suppkey#79, sum((l_extendedprice * (1 - l_discount)))#1459L)" 
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_4258_output.getNumRow() << " -> tbl_Aggregate_TD_4258_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(max(total_revenue)#1463L)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_2923_output.getNumRow() << " -> tbl_Filter_TD_2923_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(supplier_no#1460, total_revenue#1461L)" 
     << std::endl; 

    std::cout << "JOIN_INNER_1: " << tvdiff(&tv_r_JOIN_INNER_1_861_s, &tv_r_JOIN_INNER_1_861_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer((s_suppkey#342 = supplier_no#1460)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_2224_input_stringRowIDSubstitute.getNumRow() << " -> tbl_SerializeFromObject_TD_2224_input_stringRowIDSubstitute" 
     << std::endl << "      #COL: 4: " << "ListBuffer(s_suppkey#342, s_name#343, s_address#344, s_phone#346)" 
     << std::endl << "      #ROW: " << tbl_Filter_TD_2923_output.getNumRow() << " -> tbl_Filter_TD_2923_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(supplier_no#1460, total_revenue#1461L)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_1693_output.getNumRow() << " -> tbl_JOIN_INNER_TD_1693_output" 
     << std::endl << "      #COL: 5: " << "ListBuffer(s_suppkey#342, s_name#343, s_address#344, s_phone#346, total_revenue#1461L)" 
     << std::endl; 

    std::cout << "Sort_0: " << tvdiff(&tv_r_Sort_0_567_s, &tv_r_Sort_0_567_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(s_suppkey#342 ASC NULLS FIRST) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_1693_output.getNumRow() << " -> tbl_JOIN_INNER_TD_1693_output" 
     << std::endl << "      #COL: 5: " << "ListBuffer(s_suppkey#342, s_name#343, s_address#344, s_phone#346, total_revenue#1461L)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Sort_TD_0621_output.getNumRow() << " -> tbl_Sort_TD_0621_output" 
     << std::endl << "      #COL: 5: " << "ListBuffer(s_suppkey#342, s_name#343, s_address#344, s_phone#346, total_revenue#1461L)" 
     << std::endl; 

    std::cout << std::endl << " Total execution time: " << tvdiff(&tv_r_s, &tv_r_e) / 1000 << " ms"; 

    std::cout << std::endl << " Spark elapsed time: " << 19.699545 * 1000 << "ms" << std::endl; 
    return 0; 
}
