// number of overlays (w/o fusion): 2 
// number of overlays (w/ fusion): 2 
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

#include "cfgFunc_q16.hpp" 
#include "q16.hpp" 

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
    std::cout << "NOTE:running query #16\n."; 
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
    Table tbl_Sort_TD_074_output("tbl_Sort_TD_074_output", 183000000, 4, "");
    tbl_Sort_TD_074_output.allocateHost();
    Table tbl_Aggregate_TD_1520_output("tbl_Aggregate_TD_1520_output", 183000000, 4, "");
    tbl_Aggregate_TD_1520_output.allocateHost();
    Table tbl_JOIN_INNER_TD_2163_output("tbl_JOIN_INNER_TD_2163_output", 120789, 4, "");
    tbl_JOIN_INNER_TD_2163_output.allocateHost(1.2, hpTimes_join);
    Table tbl_JOIN_INNER_TD_2163_consolidated_output("tbl_JOIN_INNER_TD_2163_consolidated_output", 120789, 4, "");
    tbl_JOIN_INNER_TD_2163_consolidated_output.selectOverlayVersion(1);
    tbl_JOIN_INNER_TD_2163_consolidated_output.allocateHost();
    Table tbl_JOIN_LEFTANTI_TD_3403_output("tbl_JOIN_LEFTANTI_TD_3403_output", 799680, 4, "");
    tbl_JOIN_LEFTANTI_TD_3403_output.allocateHost(1.2, hpTimes_join);
    Table tbl_JOIN_LEFTANTI_TD_3403_consolidated_output("tbl_JOIN_LEFTANTI_TD_3403_consolidated_output", 799680, 2, "");
    tbl_JOIN_LEFTANTI_TD_3403_consolidated_output.selectOverlayVersion(1);
    tbl_JOIN_LEFTANTI_TD_3403_consolidated_output.allocateHost();
    Table tbl_Filter_TD_3677_output("tbl_Filter_TD_3677_output", 183000000, 4, "");
    tbl_Filter_TD_3677_output.selectOverlayVersion(1);
    tbl_Filter_TD_3677_output.allocateHost();
    Table tbl_Filter_TD_3677_output_partition("tbl_Filter_TD_3677_output_partition", 183000000, 4, "");
    tbl_Filter_TD_3677_output_partition.allocateHost(1.2, hpTimes_join);
    tbl_Filter_TD_3677_output_partition.allocateDevBuffer(context_h, 32);
    Table tbl_Filter_TD_3677_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_Filter_TD_3677_output_partition_array[i] = tbl_Filter_TD_3677_output_partition.createSubTable(i);
    }
    Table tbl_SerializeFromObject_TD_4370_input;
    tbl_SerializeFromObject_TD_4370_input = Table("partsupp", partsupp_n, 2, in_dir);
    tbl_SerializeFromObject_TD_4370_input.selectOverlayVersion(1);
    tbl_SerializeFromObject_TD_4370_input.addCol("ps_partkey", 4);
    tbl_SerializeFromObject_TD_4370_input.addCol("ps_suppkey", 4);
    tbl_SerializeFromObject_TD_4370_input.allocateHost();
    tbl_SerializeFromObject_TD_4370_input.loadHost();
    Table tbl_SerializeFromObject_TD_4370_input_partition("tbl_SerializeFromObject_TD_4370_input_partition", partsupp_n, 2, "");
    tbl_SerializeFromObject_TD_4370_input_partition.allocateHost(1.2, hpTimes_join);
    tbl_SerializeFromObject_TD_4370_input_partition.allocateDevBuffer(context_h, 32);
    Table tbl_SerializeFromObject_TD_4370_input_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_SerializeFromObject_TD_4370_input_partition_array[i] = tbl_SerializeFromObject_TD_4370_input_partition.createSubTable(i);
    }
    Table tbl_Filter_TD_4606_output("tbl_Filter_TD_4606_output", 183000000, 1, "");
    tbl_Filter_TD_4606_output.selectOverlayVersion(1);
    tbl_Filter_TD_4606_output.allocateHost();
    Table tbl_Filter_TD_4606_output_partition("tbl_Filter_TD_4606_output_partition", 183000000, 1, "");
    tbl_Filter_TD_4606_output_partition.allocateHost(1.2, hpTimes_join);
    tbl_Filter_TD_4606_output_partition.allocateDevBuffer(context_h, 32);
    Table tbl_Filter_TD_4606_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_Filter_TD_4606_output_partition_array[i] = tbl_Filter_TD_4606_output_partition.createSubTable(i);
    }
    Table tbl_SerializeFromObject_TD_437_input;
    tbl_SerializeFromObject_TD_437_input = Table("part", part_n, 4, in_dir);
    tbl_SerializeFromObject_TD_437_input.addCol("p_partkey", 4);
    tbl_SerializeFromObject_TD_437_input.addCol("p_brand", TPCH_READ_P_BRND_LEN+1);
    tbl_SerializeFromObject_TD_437_input.addCol("p_type", TPCH_READ_P_TYPE_LEN+1);
    tbl_SerializeFromObject_TD_437_input.addCol("p_size", 4);
    tbl_SerializeFromObject_TD_437_input.allocateHost();
    tbl_SerializeFromObject_TD_437_input.loadHost();
    Table tbl_SerializeFromObject_TD_5994_input;
    tbl_SerializeFromObject_TD_5994_input = Table("supplier", supplier_n, 2, in_dir);
    tbl_SerializeFromObject_TD_5994_input.addCol("s_suppkey", 4);
    tbl_SerializeFromObject_TD_5994_input.addCol("s_comment", TPCH_READ_S_CMNT_MAX+1);
    tbl_SerializeFromObject_TD_5994_input.allocateHost();
    tbl_SerializeFromObject_TD_5994_input.loadHost();
    // ********************** Allocate Device Buffer ******************** // 
    tbl_JOIN_INNER_TD_2163_output.allocateDevBuffer(context_h, 32);
    Table tbl_JOIN_INNER_TD_2163_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_JOIN_INNER_TD_2163_output_partition_array[i] = tbl_JOIN_INNER_TD_2163_output.createSubTable(i);
    }
    tbl_JOIN_LEFTANTI_TD_3403_output.allocateDevBuffer(context_h, 32);
    Table tbl_JOIN_LEFTANTI_TD_3403_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_JOIN_LEFTANTI_TD_3403_output_partition_array[i] = tbl_JOIN_LEFTANTI_TD_3403_output.createSubTable(i);
    }
    tbl_Filter_TD_3677_output.allocateDevBuffer(context_h, 33);
    tbl_SerializeFromObject_TD_4370_input.allocateDevBuffer(context_h, 33);
    tbl_Filter_TD_4606_output.allocateDevBuffer(context_h, 33);
    // ****************************** Config **************************** // 
    
    xf::database::gqe::KernelCommand krn_cmd_build_822 = xf::database::gqe::KernelCommand();
    xf::database::gqe::KernelCommand krn_cmd_probe_822 = xf::database::gqe::KernelCommand();
    cfgCmd cfg_JOIN_INNER_TD_2163_cmds_build;
    cfg_JOIN_INNER_TD_2163_cmds_build.setup(1);
    cfg_JOIN_INNER_TD_2163_cmds_build.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_2163_gqe_join_build(krn_cmd_build_822);
    cfg_JOIN_INNER_TD_2163_cmds_build.cmd = krn_cmd_build_822.getConfigBits();
    get_meta_info_JOIN_INNER_TD_2163_gqe_join_build(cfg_JOIN_INNER_TD_2163_cmds_build.meta_in, tbl_Filter_TD_3677_output.nrow, 3);
    get_meta_info_JOIN_INNER_TD_2163_gqe_join_build(cfg_JOIN_INNER_TD_2163_cmds_build.meta_out, tbl_JOIN_INNER_TD_2163_output.nrow, 4);
    // cfg_JOIN_INNER_TD_2163_cmds_build.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_2163_cmds_probe;
    cfg_JOIN_INNER_TD_2163_cmds_probe.setup(1);
    cfg_JOIN_INNER_TD_2163_cmds_probe.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_2163_gqe_join_probe(krn_cmd_probe_822);
    cfg_JOIN_INNER_TD_2163_cmds_probe.cmd = krn_cmd_probe_822.getConfigBits();
    get_meta_info_JOIN_INNER_TD_2163_gqe_join_probe(cfg_JOIN_INNER_TD_2163_cmds_probe.meta_in, tbl_JOIN_LEFTANTI_TD_3403_consolidated_output.nrow, 3);
    get_meta_info_JOIN_INNER_TD_2163_gqe_join_probe(cfg_JOIN_INNER_TD_2163_cmds_probe.meta_out, tbl_JOIN_INNER_TD_2163_output.nrow, 3);
    // cfg_JOIN_INNER_TD_2163_cmds_probe.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_2163_cmds_part;
    cfg_JOIN_INNER_TD_2163_cmds_part.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_2163_gqe_join_part (cfg_JOIN_INNER_TD_2163_cmds_part.cmd);
    cfg_JOIN_INNER_TD_2163_cmds_part.allocateDevBuffer(context_h, 32);
    cfg_JOIN_INNER_TD_2163_cmds_build.input_key_columns = {0, -1, -1};
    cfg_JOIN_INNER_TD_2163_cmds_probe.input_key_columns = {0, -1, -1};
    cfg_JOIN_INNER_TD_2163_cmds_build.output_key_columns = {0, 1, 2, 3};
    cfg_JOIN_INNER_TD_2163_cmds_probe.output_key_columns = {0, 1, 2, 3};
    
    xf::database::gqe::KernelCommand krn_cmd_build_777 = xf::database::gqe::KernelCommand();
    xf::database::gqe::KernelCommand krn_cmd_probe_777 = xf::database::gqe::KernelCommand();
    cfgCmd cfg_JOIN_LEFTANTI_TD_3403_cmds_build;
    cfg_JOIN_LEFTANTI_TD_3403_cmds_build.setup(1);
    cfg_JOIN_LEFTANTI_TD_3403_cmds_build.allocateHost();
    get_cfg_dat_JOIN_LEFTANTI_TD_3403_gqe_join_build(krn_cmd_build_777);
    cfg_JOIN_LEFTANTI_TD_3403_cmds_build.cmd = krn_cmd_build_777.getConfigBits();
    get_meta_info_JOIN_LEFTANTI_TD_3403_gqe_join_build(cfg_JOIN_LEFTANTI_TD_3403_cmds_build.meta_in, tbl_Filter_TD_4606_output.nrow, 3);
    get_meta_info_JOIN_LEFTANTI_TD_3403_gqe_join_build(cfg_JOIN_LEFTANTI_TD_3403_cmds_build.meta_out, tbl_JOIN_LEFTANTI_TD_3403_output.nrow, 4);
    // cfg_JOIN_LEFTANTI_TD_3403_cmds_build.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_LEFTANTI_TD_3403_cmds_probe;
    cfg_JOIN_LEFTANTI_TD_3403_cmds_probe.setup(1);
    cfg_JOIN_LEFTANTI_TD_3403_cmds_probe.allocateHost();
    get_cfg_dat_JOIN_LEFTANTI_TD_3403_gqe_join_probe(krn_cmd_probe_777);
    cfg_JOIN_LEFTANTI_TD_3403_cmds_probe.cmd = krn_cmd_probe_777.getConfigBits();
    get_meta_info_JOIN_LEFTANTI_TD_3403_gqe_join_probe(cfg_JOIN_LEFTANTI_TD_3403_cmds_probe.meta_in, tbl_SerializeFromObject_TD_4370_input.nrow, 3);
    get_meta_info_JOIN_LEFTANTI_TD_3403_gqe_join_probe(cfg_JOIN_LEFTANTI_TD_3403_cmds_probe.meta_out, tbl_JOIN_LEFTANTI_TD_3403_output.nrow, 3);
    // cfg_JOIN_LEFTANTI_TD_3403_cmds_probe.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_LEFTANTI_TD_3403_cmds_part;
    cfg_JOIN_LEFTANTI_TD_3403_cmds_part.allocateHost();
    get_cfg_dat_JOIN_LEFTANTI_TD_3403_gqe_join_part (cfg_JOIN_LEFTANTI_TD_3403_cmds_part.cmd);
    cfg_JOIN_LEFTANTI_TD_3403_cmds_part.allocateDevBuffer(context_h, 32);
    cfg_JOIN_LEFTANTI_TD_3403_cmds_build.input_key_columns = {0, -1, -1};
    cfg_JOIN_LEFTANTI_TD_3403_cmds_probe.input_key_columns = {1, -1, -1};
    cfg_JOIN_LEFTANTI_TD_3403_cmds_build.output_key_columns = {0, 1, 2, 3};
    cfg_JOIN_LEFTANTI_TD_3403_cmds_probe.output_key_columns = {0, 1, 2, 3};
    // *************************** Kernel Setup ************************* // 
    bufferTmp buftmp_h(context_h, 1); 
    buftmp_h.initBuffer(q_h); 
    std::cout << std::endl; 
    krnlEngine krnl_JOIN_INNER_TD_2163_part_left;
    krnl_JOIN_INNER_TD_2163_part_left = krnlEngine(program_h, q_h, context_h, "gqePart");
    krnl_JOIN_INNER_TD_2163_part_left.setup_hp(512, 0, power_of_hpTimes_join, tbl_Filter_TD_3677_output, tbl_Filter_TD_3677_output_partition, cfg_JOIN_INNER_TD_2163_cmds_part);
    krnlEngine krnl_JOIN_INNER_TD_2163[hpTimes_join];
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_INNER_TD_2163[i] = krnlEngine(program_h, q_h, "gqeJoin");
    }
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_INNER_TD_2163[i].setup(tbl_Filter_TD_3677_output_partition_array[i], tbl_JOIN_LEFTANTI_TD_3403_consolidated_output_partition_array[i], tbl_JOIN_INNER_TD_2163_output_partition_array[i], , buftmp_h);
    }
    krnlEngine krnl_JOIN_INNER_TD_2163_build;
    krnl_JOIN_INNER_TD_2163_build = krnlEngine(program_h, q_h, context_h, "gqeKernel", 0);
    krnl_JOIN_INNER_TD_2163_build.setup(tbl_Filter_TD_3677_output, tbl_JOIN_LEFTANTI_TD_3403_consolidated_output, tbl_JOIN_INNER_TD_2163_output, cfg_JOIN_INNER_TD_2163_cmds_build, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_2163_probe;
    krnl_JOIN_INNER_TD_2163_probe = krnlEngine(program_h, q_h, context_h, "gqeKernel", 1);
    krnl_JOIN_INNER_TD_2163_probe.setup(tbl_JOIN_LEFTANTI_TD_3403_consolidated_output, tbl_Filter_TD_3677_output, tbl_JOIN_INNER_TD_2163_output, cfg_JOIN_INNER_TD_2163_cmds_probe, buftmp_h);
    krnlEngine krnl_JOIN_LEFTANTI_TD_3403_part_left;
    krnl_JOIN_LEFTANTI_TD_3403_part_left = krnlEngine(program_h, q_h, context_h, "gqePart");
    krnl_JOIN_LEFTANTI_TD_3403_part_left.setup_hp(512, 0, power_of_hpTimes_join, tbl_Filter_TD_4606_output, tbl_Filter_TD_4606_output_partition, cfg_JOIN_LEFTANTI_TD_3403_cmds_part);
    krnlEngine krnl_JOIN_LEFTANTI_TD_3403_part_right;
    krnl_JOIN_LEFTANTI_TD_3403_part_right = krnlEngine(program_h, q_h, context_h, "gqePart");
    krnl_JOIN_LEFTANTI_TD_3403_part_right.setup_hp(512, 1, power_of_hpTimes_join, tbl_SerializeFromObject_TD_4370_input, tbl_SerializeFromObject_TD_4370_input_partition, cfg_JOIN_LEFTANTI_TD_3403_cmds_part);
    krnlEngine krnl_JOIN_LEFTANTI_TD_3403[hpTimes_join];
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_LEFTANTI_TD_3403[i] = krnlEngine(program_h, q_h, "gqeJoin");
    }
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_LEFTANTI_TD_3403[i].setup(tbl_Filter_TD_4606_output_partition_array[i], tbl_SerializeFromObject_TD_4370_input_partition_array[i], tbl_JOIN_LEFTANTI_TD_3403_output_partition_array[i], , buftmp_h);
    }
    krnlEngine krnl_JOIN_LEFTANTI_TD_3403_build;
    krnl_JOIN_LEFTANTI_TD_3403_build = krnlEngine(program_h, q_h, context_h, "gqeKernel", 0);
    krnl_JOIN_LEFTANTI_TD_3403_build.setup(tbl_Filter_TD_4606_output, tbl_SerializeFromObject_TD_4370_input, tbl_JOIN_LEFTANTI_TD_3403_output, cfg_JOIN_LEFTANTI_TD_3403_cmds_build, buftmp_h);
    krnlEngine krnl_JOIN_LEFTANTI_TD_3403_probe;
    krnl_JOIN_LEFTANTI_TD_3403_probe = krnlEngine(program_h, q_h, context_h, "gqeKernel", 1);
    krnl_JOIN_LEFTANTI_TD_3403_probe.setup(tbl_SerializeFromObject_TD_4370_input, tbl_Filter_TD_4606_output, tbl_JOIN_LEFTANTI_TD_3403_output, cfg_JOIN_LEFTANTI_TD_3403_cmds_probe, buftmp_h);
    // ************************** Transfer Engine *********************** // 
    transEngine trans_JOIN_INNER_TD_2163;
    trans_JOIN_INNER_TD_2163.setq(q_h,1);
    trans_JOIN_INNER_TD_2163.add(&(cfg_JOIN_INNER_TD_2163_cmds_part));
    trans_JOIN_INNER_TD_2163.add(&(cfg_JOIN_INNER_TD_2163_cmds_build));
    trans_JOIN_INNER_TD_2163.add(&(cfg_JOIN_INNER_TD_2163_cmds_probe));
    transEngine trans_JOIN_INNER_TD_2163_out;
    trans_JOIN_INNER_TD_2163_out.setq(q_h,1);
    q_h.finish();
    transEngine trans_JOIN_LEFTANTI_TD_3403;
    trans_JOIN_LEFTANTI_TD_3403.setq(q_h,1);
    trans_JOIN_LEFTANTI_TD_3403.add(&(cfg_JOIN_LEFTANTI_TD_3403_cmds_part));
    trans_JOIN_LEFTANTI_TD_3403.add(&(cfg_JOIN_LEFTANTI_TD_3403_cmds_build));
    trans_JOIN_LEFTANTI_TD_3403.add(&(cfg_JOIN_LEFTANTI_TD_3403_cmds_probe));
    trans_JOIN_LEFTANTI_TD_3403.add(&(tbl_SerializeFromObject_TD_4370_input));
    transEngine trans_JOIN_LEFTANTI_TD_3403_out;
    trans_JOIN_LEFTANTI_TD_3403_out.setq(q_h,1);
    q_h.finish();
    // ****************************** Events **************************** // 
    std::vector<cl::Event> events_h2d_wr_JOIN_INNER_TD_2163;
    std::vector<cl::Event> events_d2h_rd_JOIN_INNER_TD_2163;
    std::vector<cl::Event> events_JOIN_INNER_TD_2163[2];
    events_h2d_wr_JOIN_INNER_TD_2163.resize(1);
    events_d2h_rd_JOIN_INNER_TD_2163.resize(1);
    events_JOIN_INNER_TD_2163[0].resize(1);
    events_JOIN_INNER_TD_2163[1].resize(hpTimes_join);
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_2163;
    std::vector<cl::Event> prev_events_grp_JOIN_INNER_TD_2163;
    std::vector<cl::Event> events_h2d_wr_JOIN_LEFTANTI_TD_3403;
    std::vector<cl::Event> events_d2h_rd_JOIN_LEFTANTI_TD_3403;
    std::vector<cl::Event> events_JOIN_LEFTANTI_TD_3403[2];
    events_h2d_wr_JOIN_LEFTANTI_TD_3403.resize(1);
    events_d2h_rd_JOIN_LEFTANTI_TD_3403.resize(1);
    events_JOIN_LEFTANTI_TD_3403[0].resize(2);
    events_JOIN_LEFTANTI_TD_3403[1].resize(hpTimes_join);
    std::vector<cl::Event> events_grp_JOIN_LEFTANTI_TD_3403;
    std::vector<cl::Event> prev_events_grp_JOIN_LEFTANTI_TD_3403;
    // **************************** Operations ************************** // 
    struct timeval tv_r_s, tv_r_e; 
    gettimeofday(&tv_r_s, 0); 

    struct timeval tv_r_Filter_4_507_s, tv_r_Filter_4_507_e;
    gettimeofday(&tv_r_Filter_4_507_s, 0);
    SW_Filter_TD_4606(tbl_SerializeFromObject_TD_5994_input, tbl_Filter_TD_4606_output);
    gettimeofday(&tv_r_Filter_4_507_e, 0);

    struct timeval tv_r_Filter_3_91_s, tv_r_Filter_3_91_e;
    gettimeofday(&tv_r_Filter_3_91_s, 0);
    SW_Filter_TD_3677(tbl_SerializeFromObject_TD_437_input, tbl_Filter_TD_3677_output);
    gettimeofday(&tv_r_Filter_3_91_e, 0);

    struct timeval tv_r_JOIN_LEFTANTI_3_602_s, tv_r_JOIN_LEFTANTI_3_602_e;
    gettimeofday(&tv_r_JOIN_LEFTANTI_3_602_s, 0);
    tbl_Filter_TD_4606_output.tableToCol();
    tbl_SerializeFromObject_TD_4370_input.tableToCol();
    krnl_JOIN_LEFTANTI_TD_3403_build.updateMeta(tbl_Filter_TD_4606_output.getNumRow(), 3);
    krnl_JOIN_LEFTANTI_TD_3403_probe.updateMeta(tbl_SerializeFromObject_TD_4370_input.getNumRow(), 3);
    trans_JOIN_LEFTANTI_TD_3403.add(&(tbl_Filter_TD_4606_output));
    trans_JOIN_LEFTANTI_TD_3403.host2dev(0, &(prev_events_grp_JOIN_LEFTANTI_TD_3403), &(events_h2d_wr_JOIN_LEFTANTI_TD_3403[0]));
    events_grp_JOIN_LEFTANTI_TD_3403.push_back(events_h2d_wr_JOIN_LEFTANTI_TD_3403[0]);
    krnl_JOIN_LEFTANTI_TD_3403_part_left.run(0, &(events_grp_JOIN_LEFTANTI_TD_3403), &(events_JOIN_LEFTANTI_TD_3403[0][0]));
    krnl_JOIN_LEFTANTI_TD_3403_part_right.run(0, &(events_grp_JOIN_LEFTANTI_TD_3403), &(events_JOIN_LEFTANTI_TD_3403[0][1]));
    for (int i(0); i < hpTimes_join; ++i) {
        krnl_JOIN_LEFTANTI_TD_3403[i].run(0, &(events_JOIN_LEFTANTI_TD_3403[0]), &(events_JOIN_LEFTANTI_TD_3403[1][i]));
    }
    std::vector<cl::Event> events_grp_JOIN_LEFTANTI_TD_3403_build_done;
    events_grp_JOIN_LEFTANTI_TD_3403_build_done.push_back(events_JOIN_LEFTANTI_TD_3403[0]);
    krnl_JOIN_LEFTANTI_TD_3403_part_left.run(0, &(events_grp_JOIN_LEFTANTI_TD_3403), &(events_JOIN_LEFTANTI_TD_3403[0][0]));
    krnl_JOIN_LEFTANTI_TD_3403_part_right.run(0, &(events_grp_JOIN_LEFTANTI_TD_3403), &(events_JOIN_LEFTANTI_TD_3403[0][1]));
    for (int i(0); i < hpTimes_join; ++i) {
        krnl_JOIN_LEFTANTI_TD_3403[i].run(0, &(events_JOIN_LEFTANTI_TD_3403[0]), &(events_JOIN_LEFTANTI_TD_3403[1][i]));
    }
    std::vector<cl::Event> events_grp_JOIN_LEFTANTI_TD_3403_probe_done;
    events_grp_JOIN_LEFTANTI_TD_3403_probe_done.push_back(events_JOIN_LEFTANTI_TD_3403[1]);
    for (int i(0); i < hpTimes_join; ++i) {
        trans_JOIN_LEFTANTI_TD_3403_out.add(&(tbl_JOIN_LEFTANTI_TD_3403_output_partition_array[i]));
    }
    trans_JOIN_LEFTANTI_TD_3403_out.dev2host(0, &(events_JOIN_LEFTANTI_TD_3403[1]), &(events_d2h_rd_JOIN_LEFTANTI_TD_3403[0]));
    q_h.flush();
    q_h.finish();
    tbl_JOIN_LEFTANTI_TD_3403_output.setNumRow((cfg_JOIN_LEFTANTI_TD_3403_cmds_probe.meta_out)->getColLen());
    tbl_JOIN_LEFTANTI_TD_3403_output.colToTable();
    SW_Consolidated_JOIN_LEFTANTI_TD_3403_output(tbl_Filter_TD_4606_output, tbl_SerializeFromObject_TD_4370_input, tbl_JOIN_LEFTANTI_TD_3403_output, tbl_JOIN_LEFTANTI_TD_3403_consolidated_output);
    gettimeofday(&tv_r_JOIN_LEFTANTI_3_602_e, 0);

    struct timeval tv_r_JOIN_INNER_2_595_s, tv_r_JOIN_INNER_2_595_e;
    gettimeofday(&tv_r_JOIN_INNER_2_595_s, 0);
    prev_events_grp_JOIN_INNER_TD_2163.push_back(events_h2d_wr_JOIN_LEFTANTI_TD_3403[0]);
    tbl_Filter_TD_3677_output.tableToCol();
    tbl_JOIN_LEFTANTI_TD_3403_consolidated_output.tableToCol();
    krnl_JOIN_INNER_TD_2163_build.updateMeta(tbl_Filter_TD_3677_output.getNumRow(), 3);
    krnl_JOIN_INNER_TD_2163_probe.updateMeta(tbl_JOIN_LEFTANTI_TD_3403_consolidated_output.getNumRow(), 3);
    trans_JOIN_INNER_TD_2163.add(&(tbl_Filter_TD_3677_output));
    trans_JOIN_INNER_TD_2163.host2dev(0, &(prev_events_grp_JOIN_INNER_TD_2163), &(events_h2d_wr_JOIN_INNER_TD_2163[0]));
    events_grp_JOIN_INNER_TD_2163.push_back(events_h2d_wr_JOIN_INNER_TD_2163[0]);
    events_grp_JOIN_INNER_TD_2163.insert(std::end(events_grp_JOIN_INNER_TD_2163), std::begin(events_JOIN_LEFTANTI_TD_3403[0]), std::end(events_JOIN_LEFTANTI_TD_3403[0]));
    events_grp_JOIN_INNER_TD_2163.insert(std::end(events_grp_JOIN_INNER_TD_2163), std::begin(events_JOIN_LEFTANTI_TD_3403[1]), std::end(events_JOIN_LEFTANTI_TD_3403[1]));
    krnl_JOIN_INNER_TD_2163_part_left.run(0, &(events_grp_JOIN_INNER_TD_2163), &(events_JOIN_INNER_TD_2163[0][0]));
    for (int i(0); i < hpTimes_join; ++i) {
        krnl_JOIN_INNER_TD_2163[i].run(0, &(events_JOIN_INNER_TD_2163[0]), &(events_JOIN_INNER_TD_2163[1][i]));
    }
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_2163_build_done;
    events_grp_JOIN_INNER_TD_2163_build_done.push_back(events_JOIN_INNER_TD_2163[0]);
    krnl_JOIN_INNER_TD_2163_part_left.run(0, &(events_grp_JOIN_INNER_TD_2163), &(events_JOIN_INNER_TD_2163[0][0]));
    for (int i(0); i < hpTimes_join; ++i) {
        krnl_JOIN_INNER_TD_2163[i].run(0, &(events_JOIN_INNER_TD_2163[0]), &(events_JOIN_INNER_TD_2163[1][i]));
    }
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_2163_probe_done;
    events_grp_JOIN_INNER_TD_2163_probe_done.push_back(events_JOIN_INNER_TD_2163[1]);
    for (int i(0); i < hpTimes_join; ++i) {
        trans_JOIN_INNER_TD_2163_out.add(&(tbl_JOIN_INNER_TD_2163_output_partition_array[i]));
    }
    trans_JOIN_INNER_TD_2163_out.dev2host(0, &(events_JOIN_INNER_TD_2163[1]), &(events_d2h_rd_JOIN_INNER_TD_2163[0]));
    q_h.flush();
    q_h.finish();
    tbl_JOIN_INNER_TD_2163_output.setNumRow((cfg_JOIN_INNER_TD_2163_cmds_probe.meta_out)->getColLen());
    tbl_JOIN_INNER_TD_2163_output.colToTable();
    SW_Consolidated_JOIN_INNER_TD_2163_output(tbl_Filter_TD_3677_output, tbl_JOIN_LEFTANTI_TD_3403_consolidated_output, tbl_JOIN_INNER_TD_2163_output, tbl_JOIN_INNER_TD_2163_consolidated_output);
    gettimeofday(&tv_r_JOIN_INNER_2_595_e, 0);

    struct timeval tv_r_Aggregate_1_462_s, tv_r_Aggregate_1_462_e;
    gettimeofday(&tv_r_Aggregate_1_462_s, 0);
    SW_Aggregate_TD_1520(tbl_JOIN_INNER_TD_2163_consolidated_output_partition_array, tbl_SerializeFromObject_TD_437_input, tbl_Aggregate_TD_1520_output, hpTimes_join);
    gettimeofday(&tv_r_Aggregate_1_462_e, 0);

    struct timeval tv_r_Sort_0_68_s, tv_r_Sort_0_68_e;
    gettimeofday(&tv_r_Sort_0_68_s, 0);
    SW_Sort_TD_074(tbl_Aggregate_TD_1520_output, tbl_Sort_TD_074_output);
    gettimeofday(&tv_r_Sort_0_68_e, 0);

std::cout << "CPU TIME: " << (tvdiff(&tv_r_Filter_4_507_s, &tv_r_Filter_4_507_e) + 
tvdiff(&tv_r_Filter_3_91_s, &tv_r_Filter_3_91_e) + 
tvdiff(&tv_r_Aggregate_1_462_s, &tv_r_Aggregate_1_462_e) + 
tvdiff(&tv_r_Sort_0_68_s, &tv_r_Sort_0_68_e) + 
1) / 1000.0 << std::endl;

std::cout << "FPGA TIME: " << (tvdiff(&tv_r_JOIN_LEFTANTI_3_602_s, &tv_r_JOIN_LEFTANTI_3_602_e) + 
tvdiff(&tv_r_JOIN_INNER_2_595_s, &tv_r_JOIN_INNER_2_595_e) + 
1) / 1000.0 << std::endl;

    gettimeofday(&tv_r_e, 0); 
    // **************************** Print Execution Time ************************** // 
    std::cout << std::endl; 
    std::cout << "Filter_4: " << tvdiff(&tv_r_Filter_4_507_s, &tv_r_Filter_4_507_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer((isnotnull(s_comment#348) AND s_comment#348 LIKE %Customer%Complaints%)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_5994_input.getNumRow() << " -> tbl_SerializeFromObject_TD_5994_input" 
     << std::endl << "      #COL: 2: " << "ListBuffer(s_suppkey#342, s_comment#348)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_4606_output.getNumRow() << " -> tbl_Filter_TD_4606_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(s_suppkey#342)" 
     << std::endl; 

    std::cout << "Filter_3: " << tvdiff(&tv_r_Filter_3_91_s, &tv_r_Filter_3_91_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(((isnotnull(p_brand#261) AND isnotnull(p_type#262)) AND ((NOT (p_brand#261 = Brand#32) AND NOT StartsWith(p_type#262, SMALL ANODIZED)) AND p_size#263 IN (43,7,27,21,5,15,36,11)))) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_437_input.getNumRow() << " -> tbl_SerializeFromObject_TD_437_input" 
     << std::endl << "      #COL: 4: " << "ListBuffer(p_partkey#258, p_brand#261, p_type#262, p_size#263)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_3677_output.getNumRow() << " -> tbl_Filter_TD_3677_output" 
     << std::endl << "      #COL: 4: " << "ListBuffer(p_partkey#258, p_brand#261, p_type#262, p_size#263)" 
     << std::endl; 

    std::cout << "JOIN_LEFTANTI_3: " << tvdiff(&tv_r_JOIN_LEFTANTI_3_602_s, &tv_r_JOIN_LEFTANTI_3_602_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer(((ps_suppkey#306 = s_suppkey#342) OR isnull((ps_suppkey#306 = s_suppkey#342)))) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_4370_input.getNumRow() << " -> tbl_SerializeFromObject_TD_4370_input" 
     << std::endl << "      #COL: 2: " << "ListBuffer(ps_partkey#305, ps_suppkey#306)" 
     << std::endl << "      #ROW: " << tbl_Filter_TD_4606_output.getNumRow() << " -> tbl_Filter_TD_4606_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(s_suppkey#342)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_LEFTANTI_TD_3403_consolidated_output.getNumRow() << " -> tbl_JOIN_LEFTANTI_TD_3403_consolidated_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(ps_partkey#305, ps_suppkey#306)" 
     << std::endl; 

    std::cout << "JOIN_INNER_2: " << tvdiff(&tv_r_JOIN_INNER_2_595_s, &tv_r_JOIN_INNER_2_595_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer((p_partkey#258 = ps_partkey#305)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_LEFTANTI_TD_3403_consolidated_output.getNumRow() << " -> tbl_JOIN_LEFTANTI_TD_3403_consolidated_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(ps_partkey#305, ps_suppkey#306)" 
     << std::endl << "      #ROW: " << tbl_Filter_TD_3677_output.getNumRow() << " -> tbl_Filter_TD_3677_output" 
     << std::endl << "      #COL: 4: " << "ListBuffer(p_partkey#258, p_brand#261, p_type#262, p_size#263)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_2163_consolidated_output.getNumRow() << " -> tbl_JOIN_INNER_TD_2163_consolidated_output" 
     << std::endl << "      #COL: 4: " << "ListBuffer(ps_suppkey#306, p_brand#261, p_type#262, p_size#263)" 
     << std::endl; 

    std::cout << "Aggregate_1: " << tvdiff(&tv_r_Aggregate_1_462_s, &tv_r_Aggregate_1_462_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(p_brand#261, p_type#262, p_size#263, count(distinct ps_suppkey#306) AS supplier_cnt#1548L) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_2163_consolidated_output.getNumRow() << " -> tbl_JOIN_INNER_TD_2163_consolidated_output" 
     << std::endl << "      #COL: 4: " << "ListBuffer(ps_suppkey#306, p_brand#261, p_type#262, p_size#263)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_1520_output.getNumRow() << " -> tbl_Aggregate_TD_1520_output" 
     << std::endl << "      #COL: 4: " << "ListBuffer(p_brand#261, p_type#262, p_size#263, supplier_cnt#1548L)" 
     << std::endl; 

    std::cout << "Sort_0: " << tvdiff(&tv_r_Sort_0_68_s, &tv_r_Sort_0_68_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(supplier_cnt#1548L DESC NULLS LAST, p_brand#261 ASC NULLS FIRST, p_type#262 ASC NULLS FIRST, p_size#263 ASC NULLS FIRST) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_1520_output.getNumRow() << " -> tbl_Aggregate_TD_1520_output" 
     << std::endl << "      #COL: 4: " << "ListBuffer(p_brand#261, p_type#262, p_size#263, supplier_cnt#1548L)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Sort_TD_074_output.getNumRow() << " -> tbl_Sort_TD_074_output" 
     << std::endl << "      #COL: 4: " << "ListBuffer(p_brand#261, p_type#262, p_size#263, supplier_cnt#1548L)" 
     << std::endl; 

    std::cout << std::endl << " Total execution time: " << tvdiff(&tv_r_s, &tv_r_e) / 1000 << " ms"; 

    std::cout << std::endl << " Spark elapsed time: " << 2.0409825 * 1000 << "ms" << std::endl; 
    return 0; 
}
