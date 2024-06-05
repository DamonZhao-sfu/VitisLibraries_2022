// number of overlays (w/o fusion): 4 
// number of overlays (w/ fusion): 4 
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

#include "cfgFunc_q11.hpp" 
#include "q11.hpp" 

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
    std::cout << "NOTE:running query #11\n."; 
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
    Table tbl_Sort_TD_088_output("tbl_Sort_TD_088_output", 183000000, 2, "");
    tbl_Sort_TD_088_output.allocateHost();
    Table tbl_Filter_TD_1217_output("tbl_Filter_TD_1217_output", 183000000, 2, "");
    tbl_Filter_TD_1217_output.allocateHost();
    Table tbl_Aggregate_TD_2261_output("tbl_Aggregate_TD_2261_output", 183000000, 2, "");
    tbl_Aggregate_TD_2261_output.allocateHost();
    Table tbl_Aggregate_TD_2675_output("tbl_Aggregate_TD_2675_output", 183000000, 1, "");
    tbl_Aggregate_TD_2675_output.allocateHost();
    Table tbl_JOIN_INNER_TD_4140_output("tbl_JOIN_INNER_TD_4140_output", 31440, 4, "");
    tbl_JOIN_INNER_TD_4140_output.allocateHost(1.2, hpTimes_join);
    Table tbl_JOIN_INNER_TD_4140_consolidated_output("tbl_JOIN_INNER_TD_4140_consolidated_output", 31440, 3, "");
    tbl_JOIN_INNER_TD_4140_consolidated_output.selectOverlayVersion(1);
    tbl_JOIN_INNER_TD_4140_consolidated_output.allocateHost();
    Table tbl_SerializeFromObject_TD_5176_input;
    tbl_SerializeFromObject_TD_5176_input = Table("partsupp", partsupp_n, 4, in_dir);
    tbl_SerializeFromObject_TD_5176_input.selectOverlayVersion(1);
    tbl_SerializeFromObject_TD_5176_input.addCol("ps_partkey", 4);
    tbl_SerializeFromObject_TD_5176_input.addCol("ps_suppkey", 4);
    tbl_SerializeFromObject_TD_5176_input.addCol("ps_availqty", 4);
    tbl_SerializeFromObject_TD_5176_input.addCol("ps_supplycost", 4);
    tbl_SerializeFromObject_TD_5176_input.allocateHost();
    tbl_SerializeFromObject_TD_5176_input.loadHost();
    Table tbl_SerializeFromObject_TD_5176_input_partition("tbl_SerializeFromObject_TD_5176_input_partition", partsupp_n, 4, "");
    tbl_SerializeFromObject_TD_5176_input_partition.allocateHost(1.2, hpTimes_join);
    tbl_SerializeFromObject_TD_5176_input_partition.allocateDevBuffer(context_h, 32);
    Table tbl_SerializeFromObject_TD_5176_input_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_SerializeFromObject_TD_5176_input_partition_array[i] = tbl_SerializeFromObject_TD_5176_input_partition.createSubTable(i);
    }
    Table tbl_JOIN_INNER_TD_3524_output("tbl_JOIN_INNER_TD_3524_output", 393, 4, "");
    tbl_JOIN_INNER_TD_3524_output.allocateHost(1.2, hpTimes_join);
    Table tbl_JOIN_INNER_TD_3524_consolidated_output("tbl_JOIN_INNER_TD_3524_consolidated_output", 393, 1, "");
    tbl_JOIN_INNER_TD_3524_consolidated_output.selectOverlayVersion(1);
    tbl_JOIN_INNER_TD_3524_consolidated_output.allocateHost();
    Table tbl_SerializeFromObject_TD_5475_input;
    tbl_SerializeFromObject_TD_5475_input = Table("supplier", supplier_n, 2, in_dir);
    tbl_SerializeFromObject_TD_5475_input.selectOverlayVersion(1);
    tbl_SerializeFromObject_TD_5475_input.addCol("s_suppkey", 4);
    tbl_SerializeFromObject_TD_5475_input.addCol("s_nationkey", 4);
    tbl_SerializeFromObject_TD_5475_input.allocateHost();
    tbl_SerializeFromObject_TD_5475_input.loadHost();
    Table tbl_SerializeFromObject_TD_5475_input_partition("tbl_SerializeFromObject_TD_5475_input_partition", supplier_n, 2, "");
    tbl_SerializeFromObject_TD_5475_input_partition.allocateHost(1.2, hpTimes_join);
    tbl_SerializeFromObject_TD_5475_input_partition.allocateDevBuffer(context_h, 32);
    Table tbl_SerializeFromObject_TD_5475_input_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_SerializeFromObject_TD_5475_input_partition_array[i] = tbl_SerializeFromObject_TD_5475_input_partition.createSubTable(i);
    }
    Table tbl_Filter_TD_4760_output("tbl_Filter_TD_4760_output", 183000000, 1, "");
    tbl_Filter_TD_4760_output.selectOverlayVersion(1);
    tbl_Filter_TD_4760_output.allocateHost();
    Table tbl_Filter_TD_4760_output_partition("tbl_Filter_TD_4760_output_partition", 183000000, 1, "");
    tbl_Filter_TD_4760_output_partition.allocateHost(1.2, hpTimes_join);
    tbl_Filter_TD_4760_output_partition.allocateDevBuffer(context_h, 32);
    Table tbl_Filter_TD_4760_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_Filter_TD_4760_output_partition_array[i] = tbl_Filter_TD_4760_output_partition.createSubTable(i);
    }
    Table tbl_SerializeFromObject_TD_5284_input;
    tbl_SerializeFromObject_TD_5284_input = Table("nation", nation_n, 2, in_dir);
    tbl_SerializeFromObject_TD_5284_input.addCol("n_nationkey", 4);
    tbl_SerializeFromObject_TD_5284_input.addCol("n_name", TPCH_READ_NATION_LEN+1);
    tbl_SerializeFromObject_TD_5284_input.allocateHost();
    tbl_SerializeFromObject_TD_5284_input.loadHost();
    // ********************** Allocate Device Buffer ******************** // 
    tbl_JOIN_INNER_TD_4140_output.allocateDevBuffer(context_h, 32);
    Table tbl_JOIN_INNER_TD_4140_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_JOIN_INNER_TD_4140_output_partition_array[i] = tbl_JOIN_INNER_TD_4140_output.createSubTable(i);
    }
    tbl_SerializeFromObject_TD_5176_input.allocateDevBuffer(context_h, 33);
    tbl_JOIN_INNER_TD_3524_output.allocateDevBuffer(context_h, 32);
    Table tbl_JOIN_INNER_TD_3524_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_JOIN_INNER_TD_3524_output_partition_array[i] = tbl_JOIN_INNER_TD_3524_output.createSubTable(i);
    }
    tbl_SerializeFromObject_TD_5475_input.allocateDevBuffer(context_h, 33);
    tbl_Filter_TD_4760_output.allocateDevBuffer(context_h, 33);
    // ****************************** Config **************************** // 
    
    xf::database::gqe::KernelCommand krn_cmd_build_166 = xf::database::gqe::KernelCommand();
    xf::database::gqe::KernelCommand krn_cmd_probe_166 = xf::database::gqe::KernelCommand();
    cfgCmd cfg_JOIN_INNER_TD_4140_cmds_build;
    cfg_JOIN_INNER_TD_4140_cmds_build.setup(1);
    cfg_JOIN_INNER_TD_4140_cmds_build.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_4140_gqe_join_build(krn_cmd_build_166);
    cfg_JOIN_INNER_TD_4140_cmds_build.cmd = krn_cmd_build_166.getConfigBits();
    get_meta_info_JOIN_INNER_TD_4140_gqe_join_build(cfg_JOIN_INNER_TD_4140_cmds_build.meta_in, tbl_JOIN_INNER_TD_3524_consolidated_output.nrow, 3);
    get_meta_info_JOIN_INNER_TD_4140_gqe_join_build(cfg_JOIN_INNER_TD_4140_cmds_build.meta_out, tbl_JOIN_INNER_TD_4140_output.nrow, 4);
    // cfg_JOIN_INNER_TD_4140_cmds_build.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_4140_cmds_probe;
    cfg_JOIN_INNER_TD_4140_cmds_probe.setup(1);
    cfg_JOIN_INNER_TD_4140_cmds_probe.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_4140_gqe_join_probe(krn_cmd_probe_166);
    cfg_JOIN_INNER_TD_4140_cmds_probe.cmd = krn_cmd_probe_166.getConfigBits();
    get_meta_info_JOIN_INNER_TD_4140_gqe_join_probe(cfg_JOIN_INNER_TD_4140_cmds_probe.meta_in, tbl_SerializeFromObject_TD_5176_input.nrow, 3);
    get_meta_info_JOIN_INNER_TD_4140_gqe_join_probe(cfg_JOIN_INNER_TD_4140_cmds_probe.meta_out, tbl_JOIN_INNER_TD_4140_output.nrow, 3);
    // cfg_JOIN_INNER_TD_4140_cmds_probe.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_4140_cmds_part;
    cfg_JOIN_INNER_TD_4140_cmds_part.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_4140_gqe_join_part (cfg_JOIN_INNER_TD_4140_cmds_part.cmd);
    cfg_JOIN_INNER_TD_4140_cmds_part.allocateDevBuffer(context_h, 32);
    cfg_JOIN_INNER_TD_4140_cmds_build.input_key_columns = {0, -1, -1};
    cfg_JOIN_INNER_TD_4140_cmds_probe.input_key_columns = {1, -1, -1};
    cfg_JOIN_INNER_TD_4140_cmds_build.output_key_columns = {0, 1, 2, 3};
    cfg_JOIN_INNER_TD_4140_cmds_probe.output_key_columns = {0, 1, 2, 3};
    
    xf::database::gqe::KernelCommand krn_cmd_build_687 = xf::database::gqe::KernelCommand();
    xf::database::gqe::KernelCommand krn_cmd_probe_687 = xf::database::gqe::KernelCommand();
    cfgCmd cfg_JOIN_INNER_TD_3524_cmds_build;
    cfg_JOIN_INNER_TD_3524_cmds_build.setup(1);
    cfg_JOIN_INNER_TD_3524_cmds_build.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_3524_gqe_join_build(krn_cmd_build_687);
    cfg_JOIN_INNER_TD_3524_cmds_build.cmd = krn_cmd_build_687.getConfigBits();
    get_meta_info_JOIN_INNER_TD_3524_gqe_join_build(cfg_JOIN_INNER_TD_3524_cmds_build.meta_in, tbl_Filter_TD_4760_output.nrow, 3);
    get_meta_info_JOIN_INNER_TD_3524_gqe_join_build(cfg_JOIN_INNER_TD_3524_cmds_build.meta_out, tbl_JOIN_INNER_TD_3524_output.nrow, 4);
    // cfg_JOIN_INNER_TD_3524_cmds_build.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_3524_cmds_probe;
    cfg_JOIN_INNER_TD_3524_cmds_probe.setup(1);
    cfg_JOIN_INNER_TD_3524_cmds_probe.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_3524_gqe_join_probe(krn_cmd_probe_687);
    cfg_JOIN_INNER_TD_3524_cmds_probe.cmd = krn_cmd_probe_687.getConfigBits();
    get_meta_info_JOIN_INNER_TD_3524_gqe_join_probe(cfg_JOIN_INNER_TD_3524_cmds_probe.meta_in, tbl_SerializeFromObject_TD_5475_input.nrow, 3);
    get_meta_info_JOIN_INNER_TD_3524_gqe_join_probe(cfg_JOIN_INNER_TD_3524_cmds_probe.meta_out, tbl_JOIN_INNER_TD_3524_output.nrow, 3);
    // cfg_JOIN_INNER_TD_3524_cmds_probe.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_3524_cmds_part;
    cfg_JOIN_INNER_TD_3524_cmds_part.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_3524_gqe_join_part (cfg_JOIN_INNER_TD_3524_cmds_part.cmd);
    cfg_JOIN_INNER_TD_3524_cmds_part.allocateDevBuffer(context_h, 32);
    cfg_JOIN_INNER_TD_3524_cmds_build.input_key_columns = {0, -1, -1};
    cfg_JOIN_INNER_TD_3524_cmds_probe.input_key_columns = {1, -1, -1};
    cfg_JOIN_INNER_TD_3524_cmds_build.output_key_columns = {0, 1, 2, 3};
    cfg_JOIN_INNER_TD_3524_cmds_probe.output_key_columns = {0, 1, 2, 3};
    // *************************** Kernel Setup ************************* // 
    bufferTmp buftmp_h(context_h, 1); 
    buftmp_h.initBuffer(q_h); 
    std::cout << std::endl; 
    krnlEngine krnl_JOIN_INNER_TD_4140_part_right;
    krnl_JOIN_INNER_TD_4140_part_right = krnlEngine(program_h, q_h, context_h, "gqePart");
    krnl_JOIN_INNER_TD_4140_part_right.setup_hp(512, 1, power_of_hpTimes_join, tbl_SerializeFromObject_TD_5176_input, tbl_SerializeFromObject_TD_5176_input_partition, cfg_JOIN_INNER_TD_4140_cmds_part);
    krnlEngine krnl_JOIN_INNER_TD_4140[hpTimes_join];
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_INNER_TD_4140[i] = krnlEngine(program_h, q_h, "gqeJoin");
    }
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_INNER_TD_4140[i].setup(tbl_JOIN_INNER_TD_3524_consolidated_output_partition_array[i], tbl_SerializeFromObject_TD_5176_input_partition_array[i], tbl_JOIN_INNER_TD_4140_output_partition_array[i], , buftmp_h);
    }
    krnlEngine krnl_JOIN_INNER_TD_4140_build;
    krnl_JOIN_INNER_TD_4140_build = krnlEngine(program_h, q_h, context_h, "gqeKernel", 0);
    krnl_JOIN_INNER_TD_4140_build.setup(tbl_JOIN_INNER_TD_3524_consolidated_output, tbl_SerializeFromObject_TD_5176_input, tbl_JOIN_INNER_TD_4140_output, cfg_JOIN_INNER_TD_4140_cmds_build, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_4140_probe;
    krnl_JOIN_INNER_TD_4140_probe = krnlEngine(program_h, q_h, context_h, "gqeKernel", 1);
    krnl_JOIN_INNER_TD_4140_probe.setup(tbl_SerializeFromObject_TD_5176_input, tbl_JOIN_INNER_TD_3524_consolidated_output, tbl_JOIN_INNER_TD_4140_output, cfg_JOIN_INNER_TD_4140_cmds_probe, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_3524_part_left;
    krnl_JOIN_INNER_TD_3524_part_left = krnlEngine(program_h, q_h, context_h, "gqePart");
    krnl_JOIN_INNER_TD_3524_part_left.setup_hp(512, 0, power_of_hpTimes_join, tbl_Filter_TD_4760_output, tbl_Filter_TD_4760_output_partition, cfg_JOIN_INNER_TD_3524_cmds_part);
    krnlEngine krnl_JOIN_INNER_TD_3524_part_right;
    krnl_JOIN_INNER_TD_3524_part_right = krnlEngine(program_h, q_h, context_h, "gqePart");
    krnl_JOIN_INNER_TD_3524_part_right.setup_hp(512, 1, power_of_hpTimes_join, tbl_SerializeFromObject_TD_5475_input, tbl_SerializeFromObject_TD_5475_input_partition, cfg_JOIN_INNER_TD_3524_cmds_part);
    krnlEngine krnl_JOIN_INNER_TD_3524[hpTimes_join];
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_INNER_TD_3524[i] = krnlEngine(program_h, q_h, "gqeJoin");
    }
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_INNER_TD_3524[i].setup(tbl_Filter_TD_4760_output_partition_array[i], tbl_SerializeFromObject_TD_5475_input_partition_array[i], tbl_JOIN_INNER_TD_3524_output_partition_array[i], , buftmp_h);
    }
    krnlEngine krnl_JOIN_INNER_TD_3524_build;
    krnl_JOIN_INNER_TD_3524_build = krnlEngine(program_h, q_h, context_h, "gqeKernel", 0);
    krnl_JOIN_INNER_TD_3524_build.setup(tbl_Filter_TD_4760_output, tbl_SerializeFromObject_TD_5475_input, tbl_JOIN_INNER_TD_3524_output, cfg_JOIN_INNER_TD_3524_cmds_build, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_3524_probe;
    krnl_JOIN_INNER_TD_3524_probe = krnlEngine(program_h, q_h, context_h, "gqeKernel", 1);
    krnl_JOIN_INNER_TD_3524_probe.setup(tbl_SerializeFromObject_TD_5475_input, tbl_Filter_TD_4760_output, tbl_JOIN_INNER_TD_3524_output, cfg_JOIN_INNER_TD_3524_cmds_probe, buftmp_h);
    // ************************** Transfer Engine *********************** // 
    transEngine trans_JOIN_INNER_TD_4140;
    trans_JOIN_INNER_TD_4140.setq(q_h,1);
    trans_JOIN_INNER_TD_4140.add(&(cfg_JOIN_INNER_TD_4140_cmds_part));
    trans_JOIN_INNER_TD_4140.add(&(cfg_JOIN_INNER_TD_4140_cmds_build));
    trans_JOIN_INNER_TD_4140.add(&(cfg_JOIN_INNER_TD_4140_cmds_probe));
    trans_JOIN_INNER_TD_4140.add(&(tbl_SerializeFromObject_TD_5176_input));
    transEngine trans_JOIN_INNER_TD_4140_out;
    trans_JOIN_INNER_TD_4140_out.setq(q_h,1);
    q_h.finish();
    transEngine trans_JOIN_INNER_TD_3524;
    trans_JOIN_INNER_TD_3524.setq(q_h,1);
    trans_JOIN_INNER_TD_3524.add(&(cfg_JOIN_INNER_TD_3524_cmds_part));
    trans_JOIN_INNER_TD_3524.add(&(cfg_JOIN_INNER_TD_3524_cmds_build));
    trans_JOIN_INNER_TD_3524.add(&(cfg_JOIN_INNER_TD_3524_cmds_probe));
    trans_JOIN_INNER_TD_3524.add(&(tbl_SerializeFromObject_TD_5475_input));
    transEngine trans_JOIN_INNER_TD_3524_out;
    trans_JOIN_INNER_TD_3524_out.setq(q_h,1);
    q_h.finish();
    // ****************************** Events **************************** // 
    std::vector<cl::Event> events_h2d_wr_JOIN_INNER_TD_4140;
    std::vector<cl::Event> events_d2h_rd_JOIN_INNER_TD_4140;
    std::vector<cl::Event> events_JOIN_INNER_TD_4140[2];
    events_h2d_wr_JOIN_INNER_TD_4140.resize(1);
    events_d2h_rd_JOIN_INNER_TD_4140.resize(1);
    events_JOIN_INNER_TD_4140[0].resize(1);
    events_JOIN_INNER_TD_4140[1].resize(hpTimes_join);
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_4140;
    std::vector<cl::Event> prev_events_grp_JOIN_INNER_TD_4140;
    std::vector<cl::Event> events_h2d_wr_JOIN_INNER_TD_3524;
    std::vector<cl::Event> events_d2h_rd_JOIN_INNER_TD_3524;
    std::vector<cl::Event> events_JOIN_INNER_TD_3524[2];
    events_h2d_wr_JOIN_INNER_TD_3524.resize(1);
    events_d2h_rd_JOIN_INNER_TD_3524.resize(1);
    events_JOIN_INNER_TD_3524[0].resize(2);
    events_JOIN_INNER_TD_3524[1].resize(hpTimes_join);
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_3524;
    std::vector<cl::Event> prev_events_grp_JOIN_INNER_TD_3524;
    // **************************** Operations ************************** // 
    struct timeval tv_r_s, tv_r_e; 
    gettimeofday(&tv_r_s, 0); 

    struct timeval tv_r_Filter_4_886_s, tv_r_Filter_4_886_e;
    gettimeofday(&tv_r_Filter_4_886_s, 0);
    SW_Filter_TD_4760(tbl_SerializeFromObject_TD_5284_input, tbl_Filter_TD_4760_output);
    gettimeofday(&tv_r_Filter_4_886_e, 0);

    struct timeval tv_r_JOIN_INNER_3_438_s, tv_r_JOIN_INNER_3_438_e;
    gettimeofday(&tv_r_JOIN_INNER_3_438_s, 0);
    tbl_Filter_TD_4760_output.tableToCol();
    tbl_SerializeFromObject_TD_5475_input.tableToCol();
    krnl_JOIN_INNER_TD_3524_build.updateMeta(tbl_Filter_TD_4760_output.getNumRow(), 3);
    krnl_JOIN_INNER_TD_3524_probe.updateMeta(tbl_SerializeFromObject_TD_5475_input.getNumRow(), 3);
    trans_JOIN_INNER_TD_3524.add(&(tbl_Filter_TD_4760_output));
    trans_JOIN_INNER_TD_3524.host2dev(0, &(prev_events_grp_JOIN_INNER_TD_3524), &(events_h2d_wr_JOIN_INNER_TD_3524[0]));
    events_grp_JOIN_INNER_TD_3524.push_back(events_h2d_wr_JOIN_INNER_TD_3524[0]);
    krnl_JOIN_INNER_TD_3524_part_left.run(0, &(events_grp_JOIN_INNER_TD_3524), &(events_JOIN_INNER_TD_3524[0][0]));
    krnl_JOIN_INNER_TD_3524_part_right.run(0, &(events_grp_JOIN_INNER_TD_3524), &(events_JOIN_INNER_TD_3524[0][1]));
    for (int i(0); i < hpTimes_join; ++i) {
        krnl_JOIN_INNER_TD_3524[i].run(0, &(events_JOIN_INNER_TD_3524[0]), &(events_JOIN_INNER_TD_3524[1][i]));
    }
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_3524_build_done;
    events_grp_JOIN_INNER_TD_3524_build_done.push_back(events_JOIN_INNER_TD_3524[0]);
    krnl_JOIN_INNER_TD_3524_part_left.run(0, &(events_grp_JOIN_INNER_TD_3524), &(events_JOIN_INNER_TD_3524[0][0]));
    krnl_JOIN_INNER_TD_3524_part_right.run(0, &(events_grp_JOIN_INNER_TD_3524), &(events_JOIN_INNER_TD_3524[0][1]));
    for (int i(0); i < hpTimes_join; ++i) {
        krnl_JOIN_INNER_TD_3524[i].run(0, &(events_JOIN_INNER_TD_3524[0]), &(events_JOIN_INNER_TD_3524[1][i]));
    }
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_3524_probe_done;
    events_grp_JOIN_INNER_TD_3524_probe_done.push_back(events_JOIN_INNER_TD_3524[1]);
    for (int i(0); i < hpTimes_join; ++i) {
        trans_JOIN_INNER_TD_3524_out.add(&(tbl_JOIN_INNER_TD_3524_output_partition_array[i]));
    }
    trans_JOIN_INNER_TD_3524_out.dev2host(0, &(events_JOIN_INNER_TD_3524[1]), &(events_d2h_rd_JOIN_INNER_TD_3524[0]));
    q_h.flush();
    q_h.finish();
    tbl_JOIN_INNER_TD_3524_output.setNumRow((cfg_JOIN_INNER_TD_3524_cmds_probe.meta_out)->getColLen());
    tbl_JOIN_INNER_TD_3524_output.colToTable();
    SW_Consolidated_JOIN_INNER_TD_3524_output(tbl_Filter_TD_4760_output, tbl_SerializeFromObject_TD_5475_input, tbl_JOIN_INNER_TD_3524_output, tbl_JOIN_INNER_TD_3524_consolidated_output);
    gettimeofday(&tv_r_JOIN_INNER_3_438_e, 0);

    struct timeval tv_r_JOIN_INNER_4_103_s, tv_r_JOIN_INNER_4_103_e;
    gettimeofday(&tv_r_JOIN_INNER_4_103_s, 0);
    prev_events_grp_JOIN_INNER_TD_4140.push_back(events_h2d_wr_JOIN_INNER_TD_3524[0]);
    tbl_JOIN_INNER_TD_3524_consolidated_output.tableToCol();
    tbl_SerializeFromObject_TD_5176_input.tableToCol();
    krnl_JOIN_INNER_TD_4140_build.updateMeta(tbl_JOIN_INNER_TD_3524_consolidated_output.getNumRow(), 3);
    krnl_JOIN_INNER_TD_4140_probe.updateMeta(tbl_SerializeFromObject_TD_5176_input.getNumRow(), 3);
    trans_JOIN_INNER_TD_4140.host2dev(0, &(prev_events_grp_JOIN_INNER_TD_4140), &(events_h2d_wr_JOIN_INNER_TD_4140[0]));
    events_grp_JOIN_INNER_TD_4140.push_back(events_h2d_wr_JOIN_INNER_TD_4140[0]);
    events_grp_JOIN_INNER_TD_4140.insert(std::end(events_grp_JOIN_INNER_TD_4140), std::begin(events_JOIN_INNER_TD_3524[0]), std::end(events_JOIN_INNER_TD_3524[0]));
    events_grp_JOIN_INNER_TD_4140.insert(std::end(events_grp_JOIN_INNER_TD_4140), std::begin(events_JOIN_INNER_TD_3524[1]), std::end(events_JOIN_INNER_TD_3524[1]));
    krnl_JOIN_INNER_TD_4140_part_right.run(0, &(events_grp_JOIN_INNER_TD_4140), &(events_JOIN_INNER_TD_4140[0][0]));
    for (int i(0); i < hpTimes_join; ++i) {
        krnl_JOIN_INNER_TD_4140[i].run(0, &(events_grp_JOIN_INNER_TD_4140), &(events_JOIN_INNER_TD_4140[1][i]));
    }
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_4140_build_done;
    events_grp_JOIN_INNER_TD_4140_build_done.push_back(events_JOIN_INNER_TD_4140[0]);
    krnl_JOIN_INNER_TD_4140_part_right.run(0, &(events_grp_JOIN_INNER_TD_4140), &(events_JOIN_INNER_TD_4140[0][0]));
    for (int i(0); i < hpTimes_join; ++i) {
        krnl_JOIN_INNER_TD_4140[i].run(0, &(events_grp_JOIN_INNER_TD_4140), &(events_JOIN_INNER_TD_4140[1][i]));
    }
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_4140_probe_done;
    events_grp_JOIN_INNER_TD_4140_probe_done.push_back(events_JOIN_INNER_TD_4140[1]);
    for (int i(0); i < hpTimes_join; ++i) {
        trans_JOIN_INNER_TD_4140_out.add(&(tbl_JOIN_INNER_TD_4140_output_partition_array[i]));
    }
    trans_JOIN_INNER_TD_4140_out.dev2host(0, &(events_JOIN_INNER_TD_4140[1]), &(events_d2h_rd_JOIN_INNER_TD_4140[0]));
    q_h.flush();
    q_h.finish();
    tbl_JOIN_INNER_TD_4140_output.setNumRow((cfg_JOIN_INNER_TD_4140_cmds_probe.meta_out)->getColLen());
    tbl_JOIN_INNER_TD_4140_output.colToTable();
    SW_Consolidated_JOIN_INNER_TD_4140_output(tbl_JOIN_INNER_TD_3524_consolidated_output, tbl_SerializeFromObject_TD_5176_input, tbl_JOIN_INNER_TD_4140_output, tbl_JOIN_INNER_TD_4140_consolidated_output);
    gettimeofday(&tv_r_JOIN_INNER_4_103_e, 0);

    struct timeval tv_r_Aggregate_2_277_s, tv_r_Aggregate_2_277_e;
    gettimeofday(&tv_r_Aggregate_2_277_s, 0);
    SW_Aggregate_TD_2675(tbl_JOIN_INNER_TD_4140_consolidated_output_partition_array, tbl_Aggregate_TD_2675_output, hpTimes_join);
    gettimeofday(&tv_r_Aggregate_2_277_e, 0);

    struct timeval tv_r_Aggregate_2_683_s, tv_r_Aggregate_2_683_e;
    gettimeofday(&tv_r_Aggregate_2_683_s, 0);
    SW_Aggregate_TD_2261(tbl_JOIN_INNER_TD_4140_consolidated_output_partition_array, tbl_Aggregate_TD_2261_output, hpTimes_join);
    gettimeofday(&tv_r_Aggregate_2_683_e, 0);

    struct timeval tv_r_Filter_1_987_s, tv_r_Filter_1_987_e;
    gettimeofday(&tv_r_Filter_1_987_s, 0);
    SW_Filter_TD_1217(tbl_Aggregate_TD_2261_output, tbl_Aggregate_TD_2675_output, tbl_Filter_TD_1217_output);
    gettimeofday(&tv_r_Filter_1_987_e, 0);

    struct timeval tv_r_Sort_0_934_s, tv_r_Sort_0_934_e;
    gettimeofday(&tv_r_Sort_0_934_s, 0);
    SW_Sort_TD_088(tbl_Filter_TD_1217_output, tbl_Sort_TD_088_output);
    gettimeofday(&tv_r_Sort_0_934_e, 0);

std::cout << "CPU TIME: " << (tvdiff(&tv_r_Filter_4_886_s, &tv_r_Filter_4_886_e) + 
tvdiff(&tv_r_Aggregate_2_277_s, &tv_r_Aggregate_2_277_e) + 
tvdiff(&tv_r_Aggregate_2_683_s, &tv_r_Aggregate_2_683_e) + 
tvdiff(&tv_r_Filter_1_987_s, &tv_r_Filter_1_987_e) + 
tvdiff(&tv_r_Sort_0_934_s, &tv_r_Sort_0_934_e) + 
1) / 1000.0 << std::endl;

std::cout << "FPGA TIME: " << (tvdiff(&tv_r_JOIN_INNER_3_438_s, &tv_r_JOIN_INNER_3_438_e) + 
tvdiff(&tv_r_JOIN_INNER_4_103_s, &tv_r_JOIN_INNER_4_103_e) + 
1) / 1000.0 << std::endl;

    gettimeofday(&tv_r_e, 0); 
    // **************************** Print Execution Time ************************** // 
    std::cout << std::endl; 
    std::cout << "Filter_4: " << tvdiff(&tv_r_Filter_4_886_s, &tv_r_Filter_4_886_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer((isnotnull(n_name#145) AND (n_name#145 = IRAN))) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_5284_input.getNumRow() << " -> tbl_SerializeFromObject_TD_5284_input" 
     << std::endl << "      #COL: 2: " << "ListBuffer(n_nationkey#144, n_name#145)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_4760_output.getNumRow() << " -> tbl_Filter_TD_4760_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(n_nationkey#144)" 
     << std::endl; 

    std::cout << "JOIN_INNER_3: " << tvdiff(&tv_r_JOIN_INNER_3_438_s, &tv_r_JOIN_INNER_3_438_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer((s_nationkey#345 = n_nationkey#144)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_5475_input.getNumRow() << " -> tbl_SerializeFromObject_TD_5475_input" 
     << std::endl << "      #COL: 2: " << "ListBuffer(s_suppkey#342, s_nationkey#345)" 
     << std::endl << "      #ROW: " << tbl_Filter_TD_4760_output.getNumRow() << " -> tbl_Filter_TD_4760_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(n_nationkey#144)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_3524_consolidated_output.getNumRow() << " -> tbl_JOIN_INNER_TD_3524_consolidated_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(s_suppkey#342)" 
     << std::endl; 

    std::cout << "JOIN_INNER_4: " << tvdiff(&tv_r_JOIN_INNER_4_103_s, &tv_r_JOIN_INNER_4_103_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer((ps_suppkey#306 = s_suppkey#342)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_5176_input.getNumRow() << " -> tbl_SerializeFromObject_TD_5176_input" 
     << std::endl << "      #COL: 4: " << "ListBuffer(ps_partkey#305, ps_suppkey#306, ps_availqty#307, ps_supplycost#308)" 
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_3524_consolidated_output.getNumRow() << " -> tbl_JOIN_INNER_TD_3524_consolidated_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(s_suppkey#342)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_4140_consolidated_output.getNumRow() << " -> tbl_JOIN_INNER_TD_4140_consolidated_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(ps_partkey#305, ps_availqty#307, ps_supplycost#308)" 
     << std::endl; 

    std::cout << "Aggregate_2: " << tvdiff(&tv_r_Aggregate_2_277_s, &tv_r_Aggregate_2_277_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(CheckOverflow((promote_precision(cast(cast(sum((ps_supplycost#308 * ps_availqty#307)) as decimal(20,0)) as decimal(30,10))) * 0.0001000000), DecimalType(31,10), true) AS (sum((ps_supplycost * ps_availqty)) * 0.0001000000)#1201) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_4140_consolidated_output.getNumRow() << " -> tbl_JOIN_INNER_TD_4140_consolidated_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(ps_partkey#305, ps_availqty#307, ps_supplycost#308)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_2675_output.getNumRow() << " -> tbl_Aggregate_TD_2675_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer((sum((ps_supplycost * ps_availqty)) * 0.0001000000)#1201)" 
     << std::endl; 

    std::cout << "Aggregate_2: " << tvdiff(&tv_r_Aggregate_2_683_s, &tv_r_Aggregate_2_683_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(ps_partkey#305, sum((ps_supplycost#308 * ps_availqty#307)) AS value#1197L) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_4140_consolidated_output.getNumRow() << " -> tbl_JOIN_INNER_TD_4140_consolidated_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(ps_partkey#305, ps_availqty#307, ps_supplycost#308)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_2261_output.getNumRow() << " -> tbl_Aggregate_TD_2261_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(ps_partkey#305, value#1197L)" 
     << std::endl; 

    std::cout << "Filter_1: " << tvdiff(&tv_r_Filter_1_987_s, &tv_r_Filter_1_987_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer((isnotnull(value#1197L) AND (cast(cast(value#1197L as decimal(20,0)) as decimal(31,10)) > scalar-subquery#1198 []))) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_2261_output.getNumRow() << " -> tbl_Aggregate_TD_2261_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(ps_partkey#305, value#1197L)" 
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_2675_output.getNumRow() << " -> tbl_Aggregate_TD_2675_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer((sum((ps_supplycost * ps_availqty)) * 0.0001000000)#1201)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_1217_output.getNumRow() << " -> tbl_Filter_TD_1217_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(ps_partkey#305, value#1197L)" 
     << std::endl; 

    std::cout << "Sort_0: " << tvdiff(&tv_r_Sort_0_934_s, &tv_r_Sort_0_934_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(value#1197L DESC NULLS LAST) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_1217_output.getNumRow() << " -> tbl_Filter_TD_1217_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(ps_partkey#305, value#1197L)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Sort_TD_088_output.getNumRow() << " -> tbl_Sort_TD_088_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(ps_partkey#305, value#1197L)" 
     << std::endl; 

    std::cout << std::endl << " Total execution time: " << tvdiff(&tv_r_s, &tv_r_e) / 1000 << " ms"; 

    std::cout << std::endl << " Spark elapsed time: " << 2.4141476 * 1000 << "ms" << std::endl; 
    return 0; 
}
