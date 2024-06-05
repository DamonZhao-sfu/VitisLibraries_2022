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
    // *********************** Partition Infomation ********************* // 
    int hpTimes_join = 32; 
    int power_of_hpTimes_join = log2(hpTimes_join); 
    std::cout << "Number of partition (gqeJoin) is: " << hpTimes_join << std::endl; 
    int hpTimes_aggr = 256; 
    int power_of_hpTimes_aggr = log2(hpTimes_aggr); 
    std::cout << "Number of partition (gqeAggr) is: " << hpTimes_aggr << std::endl; 
    // ****************************** Tables **************************** // 
    Table tbl_Sort_TD_0538_output("tbl_Sort_TD_0538_output", 183000000, 10, "");
    tbl_Sort_TD_0538_output.allocateHost();
    Table tbl_Aggregate_TD_127_output_preprocess("tbl_Aggregate_TD_127_output_preprocess", 10, 16, "");
    tbl_Aggregate_TD_127_output_preprocess.allocateHost(1.2, hpTimes_aggr);
    Table tbl_Aggregate_TD_127_output("tbl_Aggregate_TD_127_output", 10, 10, "");
    Table tbl_Filter_TD_284_output("tbl_Filter_TD_284_output", 183000000, 6, "");
    tbl_Filter_TD_284_output.selectOverlayVersion(0);
    tbl_Filter_TD_284_output.allocateHost();
    Table tbl_Filter_TD_284_output_partition("tbl_Filter_TD_284_output_partition", 183000000, 6, "");
    tbl_Filter_TD_284_output_partition.allocateHost(1.2, hpTimes_aggr);
    tbl_Filter_TD_284_output_partition.allocateDevBuffer(context_a, 32);
    Table tbl_Filter_TD_284_output_partition_array[hpTimes_aggr];
    for (int i(0); i < hpTimes_aggr; ++i) {
        tbl_Filter_TD_284_output_partition_array[i] = tbl_Filter_TD_284_output_partition.createSubTable(i);
    }
    Table tbl_SerializeFromObject_TD_3310_input;
    tbl_SerializeFromObject_TD_3310_input = Table("lineitem", lineitem_n, 7, in_dir);
    tbl_SerializeFromObject_TD_3310_input.addCol("l_quantity", 4);
    tbl_SerializeFromObject_TD_3310_input.addCol("l_extendedprice", 4);
    tbl_SerializeFromObject_TD_3310_input.addCol("l_discount", 4);
    tbl_SerializeFromObject_TD_3310_input.addCol("l_tax", 4);
    tbl_SerializeFromObject_TD_3310_input.addCol("l_returnflag", 4);
    tbl_SerializeFromObject_TD_3310_input.addCol("l_linestatus", 4);
    tbl_SerializeFromObject_TD_3310_input.addCol("l_shipdate", 4);
    tbl_SerializeFromObject_TD_3310_input.allocateHost();
    tbl_SerializeFromObject_TD_3310_input.loadHost();
    // ********************** Allocate Device Buffer ******************** // 
    tbl_Aggregate_TD_127_output_preprocess.allocateDevBuffer(context_a, 33);
    Table tbl_Aggregate_TD_127_output_partition_array[hpTimes_aggr];
    for (int i(0); i < hpTimes_aggr; ++i) {
        tbl_Aggregate_TD_127_output_partition_array[i] = tbl_Aggregate_TD_127_output_preprocess.createSubTable(i);
    }
    tbl_Filter_TD_284_output.allocateDevBuffer(context_a, 33);
    // ****************************** Config **************************** // 
    AggrCfgCmd cfg_Aggregate_TD_127_cmds;
    cfg_Aggregate_TD_127_cmds.allocateHost();
    get_cfg_dat_Aggregate_TD_127_gqe_aggr(cfg_Aggregate_TD_127_cmds.cmd);
    cfg_Aggregate_TD_127_cmds.allocateDevBuffer(context_a, 32);
    AggrCfgCmd cfg_Aggregate_TD_127_cmds_out;
    cfg_Aggregate_TD_127_cmds_out.allocateHost();
    cfg_Aggregate_TD_127_cmds_out.allocateDevBuffer(context_a, 33);
    // *************************** Kernel Setup ************************* // 
    AggrBufferTmp buftmp_a(context_a, 1); 
    buftmp_a.BufferInitial(q_a); 
    std::cout << std::endl; 
    AggrKrnlEngine krnl_Aggregate_TD_127_part;
    krnl_Aggregate_TD_127_part = AggrKrnlEngine(program_a, q_a, "gqePart");
    krnl_Aggregate_TD_127_part.setup_hp(512, 0, power_of_hpTimes_aggr, tbl_Filter_TD_284_output, tbl_Filter_TD_284_output_partition, cfg_Aggregate_TD_127_cmds_part);
    AggrKrnlEngine krnl_Aggregate_TD_127[hpTimes_aggr];
    for (int i = 0; i < hpTimes_aggr; i++) {
        krnl_Aggregate_TD_127[i] = AggrKrnlEngine(program_a, q_a, "gqeAggr");
    }
    for (int i = 0; i < hpTimes_aggr; i++) {
        krnl_Aggregate_TD_127[i].setup(tbl_Filter_TD_284_output_partition_array[i], tbl_Aggregate_TD_127_output_partition_array[i], cfg_Aggregate_TD_127_cmds, cfg_Aggregate_TD_127_cmds_out, buftmp_a);
    }
    // ************************** Transfer Engine *********************** // 
    transEngine trans_Aggregate_TD_127;
    trans_Aggregate_TD_127.setq(q_a, 0);
    trans_Aggregate_TD_127.add(&(cfg_Aggregate_TD_127_cmds_part));
    trans_Aggregate_TD_127.add(&(cfg_Aggregate_TD_127_cmds));
    trans_Aggregate_TD_127.add(&(cfg_Aggregate_TD_127_cmds_out));
    transEngine trans_Aggregate_TD_127_out;
    trans_Aggregate_TD_127_out.setq(q_a, 0);
    q_a.finish();
    // ****************************** Events **************************** // 
    std::vector<cl::Event> events_h2d_wr_Aggregate_TD_127;
    std::vector<cl::Event> events_d2h_rd_Aggregate_TD_127;
    std::vector<cl::Event> events_Aggregate_TD_127[2];
    events_h2d_wr_Aggregate_TD_127.resize(1);
    events_d2h_rd_Aggregate_TD_127.resize(1);
    events_Aggregate_TD_127[0].resize(1);
    events_Aggregate_TD_127[1].resize(hpTimes_aggr);
    std::vector<cl::Event> events_grp_Aggregate_TD_127;
    std::vector<cl::Event> prev_events_grp_Aggregate_TD_127;
    // **************************** Operations ************************** // 
    struct timeval tv_r_s, tv_r_e; 
    gettimeofday(&tv_r_s, 0); 

    struct timeval tv_r_Filter_2_564_s, tv_r_Filter_2_564_e;
    gettimeofday(&tv_r_Filter_2_564_s, 0);
    SW_Filter_TD_284(tbl_SerializeFromObject_TD_3310_input, tbl_Filter_TD_284_output);
    gettimeofday(&tv_r_Filter_2_564_e, 0);

    struct timeval tv_r_Aggregate_1_881_s, tv_r_Aggregate_1_881_e;
    gettimeofday(&tv_r_Aggregate_1_881_s, 0);
    trans_Aggregate_TD_127.add(&(tbl_Filter_TD_284_output));
    trans_Aggregate_TD_127.host2dev(0, &(prev_events_grp_Aggregate_TD_127), &(events_h2d_wr_Aggregate_TD_127[0]));
    events_grp_Aggregate_TD_127.push_back(events_h2d_wr_Aggregate_TD_127[0]);
    krnl_Aggregate_TD_127_part.run(0, &(events_grp_Aggregate_TD_127), &(events_Aggregate_TD_127[0][0]));
    for (int i(0); i < hpTimes_aggr; ++i) {
        krnl_Aggregate_TD_127[i].run(0, &(events_Aggregate_TD_127[0]), &(events_Aggregate_TD_127[1][i]));
    }
    
    for (int i(0); i < hpTimes_aggr; ++i) {
        trans_Aggregate_TD_127_out.add(&(tbl_Aggregate_TD_127_output_partition_array[i]));
    }
    trans_Aggregate_TD_127_out.dev2host(0, &(events_Aggregate_TD_127[1]), &(events_d2h_rd_Aggregate_TD_127[0]));
    q_a.flush();
    q_a.finish();
    
    SW_Aggregate_TD_127_consolidate(tbl_Aggregate_TD_127_output_partition_array, tbl_Aggregate_TD_127_output, hpTimes_aggr);
    gettimeofday(&tv_r_Aggregate_1_881_e, 0);

    struct timeval tv_r_Sort_0_457_s, tv_r_Sort_0_457_e;
    gettimeofday(&tv_r_Sort_0_457_s, 0);
    SW_Sort_TD_0538(tbl_Aggregate_TD_127_output, tbl_Sort_TD_0538_output);
    gettimeofday(&tv_r_Sort_0_457_e, 0);

std::cout << "CPU TIME: " << (tvdiff(&tv_r_Filter_2_564_s, &tv_r_Filter_2_564_e) + 
tvdiff(&tv_r_Sort_0_457_s, &tv_r_Sort_0_457_e) + 
1) / 1000.0 << std::endl;

std::cout << "FPGA TIME: " << (tvdiff(&tv_r_Aggregate_1_881_s, &tv_r_Aggregate_1_881_e) + 
1) / 1000.0 << std::endl;

    gettimeofday(&tv_r_e, 0); 
    // **************************** Print Execution Time ************************** // 
    std::cout << std::endl; 
    std::cout << "Filter_2: " << tvdiff(&tv_r_Filter_2_564_s, &tv_r_Filter_2_564_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer((l_shipdate#87 <= 19980803)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_3310_input.getNumRow() << " -> tbl_SerializeFromObject_TD_3310_input" 
     << std::endl << "      #COL: 7: " << "ListBuffer(l_quantity#81, l_extendedprice#82, l_discount#83, l_tax#84, l_returnflag#85, l_linestatus#86, l_shipdate#87)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_284_output.getNumRow() << " -> tbl_Filter_TD_284_output" 
     << std::endl << "      #COL: 6: " << "ListBuffer(l_quantity#81, l_extendedprice#82, l_discount#83, l_tax#84, l_returnflag#85, l_linestatus#86)" 
     << std::endl; 

    std::cout << "Aggregate_1: " << tvdiff(&tv_r_Aggregate_1_881_s, &tv_r_Aggregate_1_881_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer(l_returnflag#85, l_linestatus#86, sum(l_quantity#81) AS sum_qty#364L, sum(l_extendedprice#82) AS sum_base_price#365L, sum((l_extendedprice#82 * (100 - l_discount#83))) AS sum_disc_price#366L, sum(((l_extendedprice#82 * (100 - l_discount#83)) * (100 + l_tax#84))) AS sum_charge#367L, avg(l_quantity#81) AS avg_qty#368, avg(l_extendedprice#82) AS avg_price#369, avg(l_discount#83) AS avg_disc#370, count(1) AS count_order#371L) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_284_output.getNumRow() << " -> tbl_Filter_TD_284_output" 
     << std::endl << "      #COL: 6: " << "ListBuffer(l_quantity#81, l_extendedprice#82, l_discount#83, l_tax#84, l_returnflag#85, l_linestatus#86)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_127_output.getNumRow() << " -> tbl_Aggregate_TD_127_output" 
     << std::endl << "      #COL: 10: " << "ListBuffer(l_returnflag#85, l_linestatus#86, sum_qty#364L, sum_base_price#365L, sum_disc_price#366L, sum_charge#367L, avg_qty#368, avg_price#369, avg_disc#370, count_order#371L)" 
     << std::endl; 

    std::cout << "Sort_0: " << tvdiff(&tv_r_Sort_0_457_s, &tv_r_Sort_0_457_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(l_returnflag#85 ASC NULLS FIRST, l_linestatus#86 ASC NULLS FIRST) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_127_output.getNumRow() << " -> tbl_Aggregate_TD_127_output" 
     << std::endl << "      #COL: 10: " << "ListBuffer(l_returnflag#85, l_linestatus#86, sum_qty#364L, sum_base_price#365L, sum_disc_price#366L, sum_charge#367L, avg_qty#368, avg_price#369, avg_disc#370, count_order#371L)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Sort_TD_0538_output.getNumRow() << " -> tbl_Sort_TD_0538_output" 
     << std::endl << "      #COL: 10: " << "ListBuffer(l_returnflag#85, l_linestatus#86, sum_qty#364L, sum_base_price#365L, sum_disc_price#366L, sum_charge#367L, avg_qty#368, avg_price#369, avg_disc#370, count_order#371L)" 
     << std::endl; 

    std::cout << std::endl << " Total execution time: " << tvdiff(&tv_r_s, &tv_r_e) / 1000 << " ms"; 

    std::cout << std::endl << " Spark elapsed time: " << 24.0525 * 1000 << "ms" << std::endl; 
    return 0; 
}
