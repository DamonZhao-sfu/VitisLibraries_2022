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

#include "cfgFunc_q17.hpp" 
#include "q17.hpp" 

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
    std::cout << "NOTE:running query #17\n."; 
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
    cl::Device device_h = devices[0]; 
    // Create context_h and command queue for selected device 
    cl::Context context_h(device_h); 
    cl::CommandQueue q_h(context_h, device_h, CL_QUEUE_PROFILING_ENABLE | CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE); 
    std::string devName_h = device_h.getInfo<CL_DEVICE_NAME>(); 
    std::cout << "Selected Device " << devName_h << "\n"; 
    cl::Program::Binaries xclBins_h = xcl::import_binary_file(xclbin_path_h); 
    std::vector<cl::Device> devices_h; 
    devices_h.push_back(device_h); 
    cl::Program program_h(context_h, devices_h, xclBins_h); 
    // *********************** Partition Infomation ********************* // 
    int hpTimes_join = 32; 
    int power_of_hpTimes_join = log2(hpTimes_join); 
    std::cout << "Number of partition (gqeJoin) is: " << hpTimes_join << std::endl; 
    int hpTimes_aggr = 256; 
    int power_of_hpTimes_aggr = log2(hpTimes_aggr); 
    std::cout << "Number of partition (gqeAggr) is: " << hpTimes_aggr << std::endl; 
    // ****************************** Tables **************************** // 
    Table tbl_Aggregate_TD_0599_output("tbl_Aggregate_TD_0599_output", 183000000, 1, "");
    tbl_Aggregate_TD_0599_output.allocateHost();
    Table tbl_JOIN_INNER_TD_1378_output("tbl_JOIN_INNER_TD_1378_output", 183000000, 1, "");
    tbl_JOIN_INNER_TD_1378_output.allocateHost();
    Table tbl_JOIN_INNER_TD_263_output("tbl_JOIN_INNER_TD_263_output", 1507, 4, "");
    tbl_JOIN_INNER_TD_263_output.allocateHost(1.2, hpTimes_join);
    Table tbl_JOIN_INNER_TD_263_consolidated_output("tbl_JOIN_INNER_TD_263_consolidated_output", 1507, 3, "");
    tbl_JOIN_INNER_TD_263_consolidated_output.selectOverlayVersion(1);
    tbl_JOIN_INNER_TD_263_consolidated_output.allocateHost();
    Table tbl_Aggregate_TD_3178_output("tbl_Aggregate_TD_3178_output", 183000000, 2, "");
    tbl_Aggregate_TD_3178_output.allocateHost();
    Table tbl_SerializeFromObject_TD_3504_input;
    tbl_SerializeFromObject_TD_3504_input = Table("lineitem", lineitem_n, 3, in_dir);
    tbl_SerializeFromObject_TD_3504_input.selectOverlayVersion(1);
    tbl_SerializeFromObject_TD_3504_input.addCol("l_partkey", 4);
    tbl_SerializeFromObject_TD_3504_input.addCol("l_quantity", 4);
    tbl_SerializeFromObject_TD_3504_input.addCol("l_extendedprice", 4);
    tbl_SerializeFromObject_TD_3504_input.allocateHost();
    tbl_SerializeFromObject_TD_3504_input.loadHost();
    Table tbl_SerializeFromObject_TD_3504_input_partition("tbl_SerializeFromObject_TD_3504_input_partition", lineitem_n, 3, "");
    tbl_SerializeFromObject_TD_3504_input_partition.allocateHost(1.2, hpTimes_join);
    tbl_SerializeFromObject_TD_3504_input_partition.allocateDevBuffer(context_h, 32);
    Table tbl_SerializeFromObject_TD_3504_input_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_SerializeFromObject_TD_3504_input_partition_array[i] = tbl_SerializeFromObject_TD_3504_input_partition.createSubTable(i);
    }
    Table tbl_Filter_TD_3657_output("tbl_Filter_TD_3657_output", 183000000, 1, "");
    tbl_Filter_TD_3657_output.selectOverlayVersion(1);
    tbl_Filter_TD_3657_output.allocateHost();
    Table tbl_Filter_TD_3657_output_partition("tbl_Filter_TD_3657_output_partition", 183000000, 1, "");
    tbl_Filter_TD_3657_output_partition.allocateHost(1.2, hpTimes_join);
    tbl_Filter_TD_3657_output_partition.allocateDevBuffer(context_h, 32);
    Table tbl_Filter_TD_3657_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_Filter_TD_3657_output_partition_array[i] = tbl_Filter_TD_3657_output_partition.createSubTable(i);
    }
    Table tbl_SerializeFromObject_TD_4495_input;
    tbl_SerializeFromObject_TD_4495_input = Table("lineitem", lineitem_n, 2, in_dir);
    tbl_SerializeFromObject_TD_4495_input.addCol("l_partkey", 4);
    tbl_SerializeFromObject_TD_4495_input.addCol("l_quantity", 4);
    tbl_SerializeFromObject_TD_4495_input.allocateHost();
    tbl_SerializeFromObject_TD_4495_input.loadHost();
    Table tbl_SerializeFromObject_TD_4480_input;
    tbl_SerializeFromObject_TD_4480_input = Table("part", part_n, 3, in_dir);
    tbl_SerializeFromObject_TD_4480_input.addCol("p_partkey", 4);
    tbl_SerializeFromObject_TD_4480_input.addCol("p_brand", TPCH_READ_P_BRND_LEN+1);
    tbl_SerializeFromObject_TD_4480_input.addCol("p_container", TPCH_READ_P_CNTR_LEN+1);
    tbl_SerializeFromObject_TD_4480_input.allocateHost();
    tbl_SerializeFromObject_TD_4480_input.loadHost();
    // ********************** Allocate Device Buffer ******************** // 
    tbl_JOIN_INNER_TD_263_output.allocateDevBuffer(context_h, 32);
    Table tbl_JOIN_INNER_TD_263_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_JOIN_INNER_TD_263_output_partition_array[i] = tbl_JOIN_INNER_TD_263_output.createSubTable(i);
    }
    tbl_SerializeFromObject_TD_3504_input.allocateDevBuffer(context_h, 33);
    tbl_Filter_TD_3657_output.allocateDevBuffer(context_h, 33);
    // ****************************** Config **************************** // 
    
    xf::database::gqe::KernelCommand krn_cmd_build_159 = xf::database::gqe::KernelCommand();
    xf::database::gqe::KernelCommand krn_cmd_probe_159 = xf::database::gqe::KernelCommand();
    cfgCmd cfg_JOIN_INNER_TD_263_cmds_build;
    cfg_JOIN_INNER_TD_263_cmds_build.setup(1);
    cfg_JOIN_INNER_TD_263_cmds_build.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_263_gqe_join_build(krn_cmd_build_159);
    cfg_JOIN_INNER_TD_263_cmds_build.cmd = krn_cmd_build_159.getConfigBits();
    get_meta_info_JOIN_INNER_TD_263_gqe_join_build(cfg_JOIN_INNER_TD_263_cmds_build.meta_in, tbl_Filter_TD_3657_output.nrow, 3);
    get_meta_info_JOIN_INNER_TD_263_gqe_join_build(cfg_JOIN_INNER_TD_263_cmds_build.meta_out, tbl_JOIN_INNER_TD_263_output.nrow, 4);
    // cfg_JOIN_INNER_TD_263_cmds_build.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_263_cmds_probe;
    cfg_JOIN_INNER_TD_263_cmds_probe.setup(1);
    cfg_JOIN_INNER_TD_263_cmds_probe.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_263_gqe_join_probe(krn_cmd_probe_159);
    cfg_JOIN_INNER_TD_263_cmds_probe.cmd = krn_cmd_probe_159.getConfigBits();
    get_meta_info_JOIN_INNER_TD_263_gqe_join_probe(cfg_JOIN_INNER_TD_263_cmds_probe.meta_in, tbl_SerializeFromObject_TD_3504_input.nrow, 3);
    get_meta_info_JOIN_INNER_TD_263_gqe_join_probe(cfg_JOIN_INNER_TD_263_cmds_probe.meta_out, tbl_JOIN_INNER_TD_263_output.nrow, 3);
    // cfg_JOIN_INNER_TD_263_cmds_probe.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_263_cmds_part;
    cfg_JOIN_INNER_TD_263_cmds_part.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_263_gqe_join_part (cfg_JOIN_INNER_TD_263_cmds_part.cmd);
    cfg_JOIN_INNER_TD_263_cmds_part.allocateDevBuffer(context_h, 32);
    cfg_JOIN_INNER_TD_263_cmds_build.input_key_columns = {0, -1, -1};
    cfg_JOIN_INNER_TD_263_cmds_probe.input_key_columns = {0, -1, -1};
    cfg_JOIN_INNER_TD_263_cmds_build.output_key_columns = {0, 1, 2, 3};
    cfg_JOIN_INNER_TD_263_cmds_probe.output_key_columns = {0, 1, 2, 3};
    // *************************** Kernel Setup ************************* // 
    bufferTmp buftmp_h(context_h, 1); 
    buftmp_h.initBuffer(q_h); 
    std::cout << std::endl; 
    krnlEngine krnl_JOIN_INNER_TD_263_part_left;
    krnl_JOIN_INNER_TD_263_part_left = krnlEngine(program_h, q_h, context_h, "gqePart");
    krnl_JOIN_INNER_TD_263_part_left.setup_hp(512, 0, power_of_hpTimes_join, tbl_Filter_TD_3657_output, tbl_Filter_TD_3657_output_partition, cfg_JOIN_INNER_TD_263_cmds_part);
    krnlEngine krnl_JOIN_INNER_TD_263_part_right;
    krnl_JOIN_INNER_TD_263_part_right = krnlEngine(program_h, q_h, context_h, "gqePart");
    krnl_JOIN_INNER_TD_263_part_right.setup_hp(512, 1, power_of_hpTimes_join, tbl_SerializeFromObject_TD_3504_input, tbl_SerializeFromObject_TD_3504_input_partition, cfg_JOIN_INNER_TD_263_cmds_part);
    krnlEngine krnl_JOIN_INNER_TD_263[hpTimes_join];
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_INNER_TD_263[i] = krnlEngine(program_h, q_h, "gqeJoin");
    }
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_INNER_TD_263[i].setup(tbl_Filter_TD_3657_output_partition_array[i], tbl_SerializeFromObject_TD_3504_input_partition_array[i], tbl_JOIN_INNER_TD_263_output_partition_array[i], , buftmp_h);
    }
    krnlEngine krnl_JOIN_INNER_TD_263_build;
    krnl_JOIN_INNER_TD_263_build = krnlEngine(program_h, q_h, context_h, "gqeKernel", 0);
    krnl_JOIN_INNER_TD_263_build.setup(tbl_Filter_TD_3657_output, tbl_SerializeFromObject_TD_3504_input, tbl_JOIN_INNER_TD_263_output, cfg_JOIN_INNER_TD_263_cmds_build, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_263_probe;
    krnl_JOIN_INNER_TD_263_probe = krnlEngine(program_h, q_h, context_h, "gqeKernel", 1);
    krnl_JOIN_INNER_TD_263_probe.setup(tbl_SerializeFromObject_TD_3504_input, tbl_Filter_TD_3657_output, tbl_JOIN_INNER_TD_263_output, cfg_JOIN_INNER_TD_263_cmds_probe, buftmp_h);
    // ************************** Transfer Engine *********************** // 
    transEngine trans_JOIN_INNER_TD_263;
    trans_JOIN_INNER_TD_263.setq(q_h,1);
    trans_JOIN_INNER_TD_263.add(&(cfg_JOIN_INNER_TD_263_cmds_part));
    trans_JOIN_INNER_TD_263.add(&(cfg_JOIN_INNER_TD_263_cmds_build));
    trans_JOIN_INNER_TD_263.add(&(cfg_JOIN_INNER_TD_263_cmds_probe));
    trans_JOIN_INNER_TD_263.add(&(tbl_SerializeFromObject_TD_3504_input));
    transEngine trans_JOIN_INNER_TD_263_out;
    trans_JOIN_INNER_TD_263_out.setq(q_h,1);
    q_h.finish();
    // ****************************** Events **************************** // 
    std::vector<cl::Event> events_h2d_wr_JOIN_INNER_TD_263;
    std::vector<cl::Event> events_d2h_rd_JOIN_INNER_TD_263;
    std::vector<cl::Event> events_JOIN_INNER_TD_263[2];
    events_h2d_wr_JOIN_INNER_TD_263.resize(1);
    events_d2h_rd_JOIN_INNER_TD_263.resize(1);
    events_JOIN_INNER_TD_263[0].resize(2);
    events_JOIN_INNER_TD_263[1].resize(hpTimes_join);
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_263;
    std::vector<cl::Event> prev_events_grp_JOIN_INNER_TD_263;
    // **************************** Operations ************************** // 
    struct timeval tv_r_s, tv_r_e; 
    gettimeofday(&tv_r_s, 0); 

    struct timeval tv_r_Filter_3_605_s, tv_r_Filter_3_605_e;
    gettimeofday(&tv_r_Filter_3_605_s, 0);
    SW_Filter_TD_3657(tbl_SerializeFromObject_TD_4480_input, tbl_Filter_TD_3657_output);
    gettimeofday(&tv_r_Filter_3_605_e, 0);

    struct timeval tv_r_Aggregate_3_383_s, tv_r_Aggregate_3_383_e;
    gettimeofday(&tv_r_Aggregate_3_383_s, 0);
    SW_Aggregate_TD_3178(tbl_SerializeFromObject_TD_4495_input, tbl_Aggregate_TD_3178_output);
    gettimeofday(&tv_r_Aggregate_3_383_e, 0);

    struct timeval tv_r_JOIN_INNER_2_208_s, tv_r_JOIN_INNER_2_208_e;
    gettimeofday(&tv_r_JOIN_INNER_2_208_s, 0);
    tbl_Filter_TD_3657_output.tableToCol();
    tbl_SerializeFromObject_TD_3504_input.tableToCol();
    krnl_JOIN_INNER_TD_263_build.updateMeta(tbl_Filter_TD_3657_output.getNumRow(), 3);
    krnl_JOIN_INNER_TD_263_probe.updateMeta(tbl_SerializeFromObject_TD_3504_input.getNumRow(), 3);
    trans_JOIN_INNER_TD_263.add(&(tbl_Filter_TD_3657_output));
    trans_JOIN_INNER_TD_263.host2dev(0, &(prev_events_grp_JOIN_INNER_TD_263), &(events_h2d_wr_JOIN_INNER_TD_263[0]));
    events_grp_JOIN_INNER_TD_263.push_back(events_h2d_wr_JOIN_INNER_TD_263[0]);
    krnl_JOIN_INNER_TD_263_part_left.run(0, &(events_grp_JOIN_INNER_TD_263), &(events_JOIN_INNER_TD_263[0][0]));
    krnl_JOIN_INNER_TD_263_part_right.run(0, &(events_grp_JOIN_INNER_TD_263), &(events_JOIN_INNER_TD_263[0][1]));
    for (int i(0); i < hpTimes_join; ++i) {
        krnl_JOIN_INNER_TD_263[i].run(0, &(events_JOIN_INNER_TD_263[0]), &(events_JOIN_INNER_TD_263[1][i]));
    }
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_263_build_done;
    events_grp_JOIN_INNER_TD_263_build_done.push_back(events_JOIN_INNER_TD_263[0]);
    krnl_JOIN_INNER_TD_263_part_left.run(0, &(events_grp_JOIN_INNER_TD_263), &(events_JOIN_INNER_TD_263[0][0]));
    krnl_JOIN_INNER_TD_263_part_right.run(0, &(events_grp_JOIN_INNER_TD_263), &(events_JOIN_INNER_TD_263[0][1]));
    for (int i(0); i < hpTimes_join; ++i) {
        krnl_JOIN_INNER_TD_263[i].run(0, &(events_JOIN_INNER_TD_263[0]), &(events_JOIN_INNER_TD_263[1][i]));
    }
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_263_probe_done;
    events_grp_JOIN_INNER_TD_263_probe_done.push_back(events_JOIN_INNER_TD_263[1]);
    for (int i(0); i < hpTimes_join; ++i) {
        trans_JOIN_INNER_TD_263_out.add(&(tbl_JOIN_INNER_TD_263_output_partition_array[i]));
    }
    trans_JOIN_INNER_TD_263_out.dev2host(0, &(events_JOIN_INNER_TD_263[1]), &(events_d2h_rd_JOIN_INNER_TD_263[0]));
    q_h.flush();
    q_h.finish();
    tbl_JOIN_INNER_TD_263_output.setNumRow((cfg_JOIN_INNER_TD_263_cmds_probe.meta_out)->getColLen());
    tbl_JOIN_INNER_TD_263_output.colToTable();
    SW_Consolidated_JOIN_INNER_TD_263_output(tbl_Filter_TD_3657_output, tbl_SerializeFromObject_TD_3504_input, tbl_JOIN_INNER_TD_263_output, tbl_JOIN_INNER_TD_263_consolidated_output);
    gettimeofday(&tv_r_JOIN_INNER_2_208_e, 0);

    struct timeval tv_r_JOIN_INNER_1_907_s, tv_r_JOIN_INNER_1_907_e;
    gettimeofday(&tv_r_JOIN_INNER_1_907_s, 0);
    SW_JOIN_INNER_TD_1378(tbl_JOIN_INNER_TD_263_consolidated_output_partition_array, tbl_Aggregate_TD_3178_output, tbl_JOIN_INNER_TD_1378_output, hpTimes_join);
    gettimeofday(&tv_r_JOIN_INNER_1_907_e, 0);

    struct timeval tv_r_Aggregate_0_866_s, tv_r_Aggregate_0_866_e;
    gettimeofday(&tv_r_Aggregate_0_866_s, 0);
    SW_Aggregate_TD_0599(tbl_JOIN_INNER_TD_1378_output, tbl_Aggregate_TD_0599_output);
    gettimeofday(&tv_r_Aggregate_0_866_e, 0);

std::cout << "CPU TIME: " << (tvdiff(&tv_r_Filter_3_605_s, &tv_r_Filter_3_605_e) + 
tvdiff(&tv_r_Aggregate_3_383_s, &tv_r_Aggregate_3_383_e) + 
tvdiff(&tv_r_JOIN_INNER_1_907_s, &tv_r_JOIN_INNER_1_907_e) + 
tvdiff(&tv_r_Aggregate_0_866_s, &tv_r_Aggregate_0_866_e) + 
1) / 1000.0 << std::endl;

std::cout << "FPGA TIME: " << (tvdiff(&tv_r_JOIN_INNER_2_208_s, &tv_r_JOIN_INNER_2_208_e) + 
1) / 1000.0 << std::endl;

    gettimeofday(&tv_r_e, 0); 
    // **************************** Print Execution Time ************************** // 
    std::cout << std::endl; 
    std::cout << "Filter_3: " << tvdiff(&tv_r_Filter_3_605_s, &tv_r_Filter_3_605_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(((isnotnull(p_brand#261) AND isnotnull(p_container#264)) AND ((p_brand#261 = Brand#21) AND (p_container#264 = WRAP BAG)))) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_4480_input.getNumRow() << " -> tbl_SerializeFromObject_TD_4480_input" 
     << std::endl << "      #COL: 3: " << "ListBuffer(p_partkey#258, p_brand#261, p_container#264)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_3657_output.getNumRow() << " -> tbl_Filter_TD_3657_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(p_partkey#258)" 
     << std::endl; 

    std::cout << "Aggregate_3: " << tvdiff(&tv_r_Aggregate_3_383_s, &tv_r_Aggregate_3_383_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(l_partkey#78, (0.2 * avg(l_quantity#81)) AS (0.2 * avg(l_quantity))#1640, l_partkey#78 AS l_partkey#78#1690) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_4495_input.getNumRow() << " -> tbl_SerializeFromObject_TD_4495_input" 
     << std::endl << "      #COL: 2: " << "ListBuffer(l_partkey#78, l_quantity#81)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_3178_output.getNumRow() << " -> tbl_Aggregate_TD_3178_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer((0.2 * avg(l_quantity))#1640, l_partkey#78#1690)" 
     << std::endl; 

    std::cout << "JOIN_INNER_2: " << tvdiff(&tv_r_JOIN_INNER_2_208_s, &tv_r_JOIN_INNER_2_208_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer((p_partkey#258 = l_partkey#78)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_3504_input.getNumRow() << " -> tbl_SerializeFromObject_TD_3504_input" 
     << std::endl << "      #COL: 3: " << "ListBuffer(l_partkey#78, l_quantity#81, l_extendedprice#82)" 
     << std::endl << "      #ROW: " << tbl_Filter_TD_3657_output.getNumRow() << " -> tbl_Filter_TD_3657_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(p_partkey#258)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_263_consolidated_output.getNumRow() << " -> tbl_JOIN_INNER_TD_263_consolidated_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(l_quantity#81, l_extendedprice#82, p_partkey#258)" 
     << std::endl; 

    std::cout << "JOIN_INNER_1: " << tvdiff(&tv_r_JOIN_INNER_1_907_s, &tv_r_JOIN_INNER_1_907_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(((cast(l_quantity#81 as double) < (0.2 * avg(l_quantity))#1640) AND (l_partkey#78#1690 = p_partkey#258))) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_263_consolidated_output.getNumRow() << " -> tbl_JOIN_INNER_TD_263_consolidated_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(l_quantity#81, l_extendedprice#82, p_partkey#258)" 
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_3178_output.getNumRow() << " -> tbl_Aggregate_TD_3178_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer((0.2 * avg(l_quantity))#1640, l_partkey#78#1690)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_1378_output.getNumRow() << " -> tbl_JOIN_INNER_TD_1378_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(l_extendedprice#82)" 
     << std::endl; 

    std::cout << "Aggregate_0: " << tvdiff(&tv_r_Aggregate_0_866_s, &tv_r_Aggregate_0_866_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(CheckOverflow((promote_precision(cast(cast(sum(l_extendedprice#82) as decimal(20,0)) as decimal(21,1))) / 7.0), DecimalType(27,6), true) AS avg_yearly#1637) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_1378_output.getNumRow() << " -> tbl_JOIN_INNER_TD_1378_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(l_extendedprice#82)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_0599_output.getNumRow() << " -> tbl_Aggregate_TD_0599_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(avg_yearly#1637)" 
     << std::endl; 

    std::cout << std::endl << " Total execution time: " << tvdiff(&tv_r_s, &tv_r_e) / 1000 << " ms"; 

    std::cout << std::endl << " Spark elapsed time: " << 41.429882 * 1000 << "ms" << std::endl; 
    return 0; 
}
