// number of overlays (w/o fusion): 3 
// number of overlays (w/ fusion): 3 
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

#include "cfgFunc_q3.hpp" 
#include "q3.hpp" 

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
    std::cout << "NOTE:running query #3\n."; 
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
    Table tbl_Sort_TD_0483_output("tbl_Sort_TD_0483_output", 183000000, 4, "");
    tbl_Sort_TD_0483_output.allocateHost();
    Table tbl_Aggregate_TD_133_output_preprocess("tbl_Aggregate_TD_133_output_preprocess", 24000, 16, "");
    tbl_Aggregate_TD_133_output_preprocess.allocateHost(1.2, hpTimes_aggr);
    Table tbl_Aggregate_TD_133_output("tbl_Aggregate_TD_133_output", 24000, 4, "");
    Table tbl_JOIN_INNER_TD_2246_output_preprocess("tbl_JOIN_INNER_TD_2246_output_preprocess", 30000, 5, "");
    tbl_JOIN_INNER_TD_2246_output_preprocess.selectOverlayVersion(0);
    tbl_JOIN_INNER_TD_2246_output_preprocess.allocateHost();
    Table tbl_JOIN_INNER_TD_2246_output("tbl_JOIN_INNER_TD_2246_output", 30000, 5, "");
    tbl_JOIN_INNER_TD_2246_output_preprocess.selectOverlayVersion(0);
    tbl_JOIN_INNER_TD_2246_output.allocateHost();
    Table tbl_Filter_TD_3612_output("tbl_Filter_TD_3612_output", 183000000, 3, "");
    tbl_Filter_TD_3612_output.selectOverlayVersion(1);
    tbl_Filter_TD_3612_output.allocateHost();
    Table tbl_Filter_TD_3612_output_partition("tbl_Filter_TD_3612_output_partition", 183000000, 3, "");
    tbl_Filter_TD_3612_output_partition.allocateHost(1.2, hpTimes_join);
    tbl_Filter_TD_3612_output_partition.allocateDevBuffer(context_h, 32);
    Table tbl_Filter_TD_3612_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_Filter_TD_3612_output_partition_array[i] = tbl_Filter_TD_3612_output_partition.createSubTable(i);
    }
    Table tbl_JOIN_INNER_TD_314_output("tbl_JOIN_INNER_TD_314_output", 150000, 4, "");
    tbl_JOIN_INNER_TD_314_output.allocateHost(1.2, hpTimes_join);
    Table tbl_JOIN_INNER_TD_314_consolidated_output("tbl_JOIN_INNER_TD_314_consolidated_output", 150000, 3, "");
    tbl_JOIN_INNER_TD_314_consolidated_output.selectOverlayVersion(1);
    tbl_JOIN_INNER_TD_314_consolidated_output.allocateHost();
    Table tbl_SerializeFromObject_TD_4597_input;
    tbl_SerializeFromObject_TD_4597_input = Table("lineitem", lineitem_n, 4, in_dir);
    tbl_SerializeFromObject_TD_4597_input.addCol("l_orderkey", 4);
    tbl_SerializeFromObject_TD_4597_input.addCol("l_extendedprice", 4);
    tbl_SerializeFromObject_TD_4597_input.addCol("l_discount", 4);
    tbl_SerializeFromObject_TD_4597_input.addCol("l_shipdate", 4);
    tbl_SerializeFromObject_TD_4597_input.allocateHost();
    tbl_SerializeFromObject_TD_4597_input.loadHost();
    Table tbl_Filter_TD_4727_output("tbl_Filter_TD_4727_output", 183000000, 1, "");
    tbl_Filter_TD_4727_output.selectOverlayVersion(1);
    tbl_Filter_TD_4727_output.allocateHost();
    Table tbl_Filter_TD_4727_output_partition("tbl_Filter_TD_4727_output_partition", 183000000, 1, "");
    tbl_Filter_TD_4727_output_partition.allocateHost(1.2, hpTimes_join);
    tbl_Filter_TD_4727_output_partition.allocateDevBuffer(context_h, 32);
    Table tbl_Filter_TD_4727_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_Filter_TD_4727_output_partition_array[i] = tbl_Filter_TD_4727_output_partition.createSubTable(i);
    }
    Table tbl_Filter_TD_426_output("tbl_Filter_TD_426_output", 183000000, 4, "");
    tbl_Filter_TD_426_output.selectOverlayVersion(1);
    tbl_Filter_TD_426_output.allocateHost();
    Table tbl_Filter_TD_426_output_partition("tbl_Filter_TD_426_output_partition", 183000000, 4, "");
    tbl_Filter_TD_426_output_partition.allocateHost(1.2, hpTimes_join);
    tbl_Filter_TD_426_output_partition.allocateDevBuffer(context_h, 32);
    Table tbl_Filter_TD_426_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_Filter_TD_426_output_partition_array[i] = tbl_Filter_TD_426_output_partition.createSubTable(i);
    }
    Table tbl_SerializeFromObject_TD_5928_input;
    tbl_SerializeFromObject_TD_5928_input = Table("customer", customer_n, 2, in_dir);
    tbl_SerializeFromObject_TD_5928_input.addCol("c_custkey", 4);
    tbl_SerializeFromObject_TD_5928_input.addCol("c_mktsegment", TPCH_READ_MAXAGG_LEN+1);
    tbl_SerializeFromObject_TD_5928_input.allocateHost();
    tbl_SerializeFromObject_TD_5928_input.loadHost();
    Table tbl_SerializeFromObject_TD_526_input;
    tbl_SerializeFromObject_TD_526_input = Table("order", order_n, 4, in_dir);
    tbl_SerializeFromObject_TD_526_input.addCol("o_orderkey", 4);
    tbl_SerializeFromObject_TD_526_input.addCol("o_custkey", 4);
    tbl_SerializeFromObject_TD_526_input.addCol("o_orderdate", 4);
    tbl_SerializeFromObject_TD_526_input.addCol("o_shippriority", 4);
    tbl_SerializeFromObject_TD_526_input.allocateHost();
    tbl_SerializeFromObject_TD_526_input.loadHost();
    // ********************** Allocate Device Buffer ******************** // 
    tbl_JOIN_INNER_TD_2246_output.allocateDevBuffer(context_a, 32);
    tbl_Aggregate_TD_133_output_preprocess.allocateDevBuffer(context_a, 33);
    Table tbl_Aggregate_TD_133_output_partition_array[hpTimes_aggr];
    for (int i(0); i < hpTimes_aggr; ++i) {
        tbl_Aggregate_TD_133_output_partition_array[i] = tbl_Aggregate_TD_133_output_preprocess.createSubTable(i);
    }
    tbl_JOIN_INNER_TD_2246_output_preprocess.allocateDevBuffer(context_h, 32);
    tbl_Filter_TD_3612_output.allocateDevBuffer(context_h, 33);
    tbl_JOIN_INNER_TD_314_output.allocateDevBuffer(context_h, 32);
    Table tbl_JOIN_INNER_TD_314_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_JOIN_INNER_TD_314_output_partition_array[i] = tbl_JOIN_INNER_TD_314_output.createSubTable(i);
    }
    tbl_Filter_TD_4727_output.allocateDevBuffer(context_h, 33);
    tbl_Filter_TD_426_output.allocateDevBuffer(context_h, 33);
    // ****************************** Config **************************** // 
    AggrCfgCmd cfg_Aggregate_TD_133_cmds;
    cfg_Aggregate_TD_133_cmds.allocateHost();
    get_cfg_dat_Aggregate_TD_133_gqe_aggr(cfg_Aggregate_TD_133_cmds.cmd);
    cfg_Aggregate_TD_133_cmds.allocateDevBuffer(context_a, 32);
    AggrCfgCmd cfg_Aggregate_TD_133_cmds_out;
    cfg_Aggregate_TD_133_cmds_out.allocateHost();
    cfg_Aggregate_TD_133_cmds_out.allocateDevBuffer(context_a, 33);
    cfgCmd cfg_JOIN_INNER_TD_2246_cmds;
    cfg_JOIN_INNER_TD_2246_cmds.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_2246_gqe_join (cfg_JOIN_INNER_TD_2246_cmds.cmd);
    cfg_JOIN_INNER_TD_2246_cmds.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_2246_cmds_part;
    cfg_JOIN_INNER_TD_2246_cmds_part.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_2246_gqe_join_part (cfg_JOIN_INNER_TD_2246_cmds_part.cmd);
    cfg_JOIN_INNER_TD_2246_cmds_part.allocateDevBuffer(context_h, 32);
    
    xf::database::gqe::KernelCommand krn_cmd_build_732 = xf::database::gqe::KernelCommand();
    xf::database::gqe::KernelCommand krn_cmd_probe_732 = xf::database::gqe::KernelCommand();
    cfgCmd cfg_JOIN_INNER_TD_314_cmds_build;
    cfg_JOIN_INNER_TD_314_cmds_build.setup(1);
    cfg_JOIN_INNER_TD_314_cmds_build.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_314_gqe_join_build(krn_cmd_build_732);
    cfg_JOIN_INNER_TD_314_cmds_build.cmd = krn_cmd_build_732.getConfigBits();
    get_meta_info_JOIN_INNER_TD_314_gqe_join_build(cfg_JOIN_INNER_TD_314_cmds_build.meta_in, tbl_Filter_TD_4727_output.nrow, 3);
    get_meta_info_JOIN_INNER_TD_314_gqe_join_build(cfg_JOIN_INNER_TD_314_cmds_build.meta_out, tbl_JOIN_INNER_TD_314_output.nrow, 4);
    // cfg_JOIN_INNER_TD_314_cmds_build.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_314_cmds_probe;
    cfg_JOIN_INNER_TD_314_cmds_probe.setup(1);
    cfg_JOIN_INNER_TD_314_cmds_probe.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_314_gqe_join_probe(krn_cmd_probe_732);
    cfg_JOIN_INNER_TD_314_cmds_probe.cmd = krn_cmd_probe_732.getConfigBits();
    get_meta_info_JOIN_INNER_TD_314_gqe_join_probe(cfg_JOIN_INNER_TD_314_cmds_probe.meta_in, tbl_Filter_TD_426_output.nrow, 3);
    get_meta_info_JOIN_INNER_TD_314_gqe_join_probe(cfg_JOIN_INNER_TD_314_cmds_probe.meta_out, tbl_JOIN_INNER_TD_314_output.nrow, 3);
    // cfg_JOIN_INNER_TD_314_cmds_probe.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_314_cmds_part;
    cfg_JOIN_INNER_TD_314_cmds_part.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_314_gqe_join_part (cfg_JOIN_INNER_TD_314_cmds_part.cmd);
    cfg_JOIN_INNER_TD_314_cmds_part.allocateDevBuffer(context_h, 32);
    cfg_JOIN_INNER_TD_314_cmds_build.input_key_columns = {0, -1, -1};
    cfg_JOIN_INNER_TD_314_cmds_probe.input_key_columns = {1, -1, -1};
    cfg_JOIN_INNER_TD_314_cmds_build.output_key_columns = {0, 1, 2, 3};
    cfg_JOIN_INNER_TD_314_cmds_probe.output_key_columns = {0, 1, 2, 3};
    // *************************** Kernel Setup ************************* // 
    bufferTmp buftmp_h(context_h, 1); 
    buftmp_h.initBuffer(q_h); 
    AggrBufferTmp buftmp_a(context_a, 1); 
    buftmp_a.BufferInitial(q_a); 
    std::cout << std::endl; 
    AggrKrnlEngine krnl_Aggregate_TD_133_part;
    krnl_Aggregate_TD_133_part = AggrKrnlEngine(program_a, q_a, "gqePart");
    krnl_Aggregate_TD_133_part.setup_hp(512, 0, power_of_hpTimes_aggr, tbl_JOIN_INNER_TD_2246_output, tbl_JOIN_INNER_TD_2246_output_partition, cfg_Aggregate_TD_133_cmds_part);
    AggrKrnlEngine krnl_Aggregate_TD_133[hpTimes_aggr];
    for (int i = 0; i < hpTimes_aggr; i++) {
        krnl_Aggregate_TD_133[i] = AggrKrnlEngine(program_a, q_a, "gqeAggr");
    }
    for (int i = 0; i < hpTimes_aggr; i++) {
        krnl_Aggregate_TD_133[i].setup(tbl_JOIN_INNER_TD_2246_output_partition_array[i], tbl_Aggregate_TD_133_output_partition_array[i], cfg_Aggregate_TD_133_cmds, cfg_Aggregate_TD_133_cmds_out, buftmp_a);
    }
    krnlEngine krnl_JOIN_INNER_TD_2246_part_right;
    krnl_JOIN_INNER_TD_2246_part_right = krnlEngine(program_h, q_h, context_h, "gqePart");
    krnl_JOIN_INNER_TD_2246_part_right.setup_hp(512, 1, power_of_hpTimes_join, tbl_Filter_TD_3612_output, tbl_Filter_TD_3612_output_partition, cfg_JOIN_INNER_TD_2246_cmds_part);
    krnlEngine krnl_JOIN_INNER_TD_2246[hpTimes_join];
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_INNER_TD_2246[i] = krnlEngine(program_h, q_h, "gqeJoin");
    }
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_INNER_TD_2246[i].setup(tbl_JOIN_INNER_TD_314_consolidated_output_partition_array[i], tbl_Filter_TD_3612_output_partition_array[i], tbl_JOIN_INNER_TD_2246_output_preprocess_partition_array[i], cfg_JOIN_INNER_TD_2246_cmds, buftmp_h);
    }
    krnlEngine krnl_JOIN_INNER_TD_2246_build;
    krnl_JOIN_INNER_TD_2246_build = krnlEngine(program_h, q_h, context_h, "gqeKernel", 0);
    krnl_JOIN_INNER_TD_2246_build.setup(tbl_JOIN_INNER_TD_314_consolidated_output, tbl_Filter_TD_3612_output, tbl_JOIN_INNER_TD_2246_output_preprocess, cfg_JOIN_INNER_TD_2246_cmds_build, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_2246_probe;
    krnl_JOIN_INNER_TD_2246_probe = krnlEngine(program_h, q_h, context_h, "gqeKernel", 1);
    krnl_JOIN_INNER_TD_2246_probe.setup(tbl_Filter_TD_3612_output, tbl_JOIN_INNER_TD_314_consolidated_output, tbl_JOIN_INNER_TD_2246_output_preprocess, cfg_JOIN_INNER_TD_2246_cmds_probe, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_314_part_left;
    krnl_JOIN_INNER_TD_314_part_left = krnlEngine(program_h, q_h, context_h, "gqePart");
    krnl_JOIN_INNER_TD_314_part_left.setup_hp(512, 0, power_of_hpTimes_join, tbl_Filter_TD_4727_output, tbl_Filter_TD_4727_output_partition, cfg_JOIN_INNER_TD_314_cmds_part);
    krnlEngine krnl_JOIN_INNER_TD_314_part_right;
    krnl_JOIN_INNER_TD_314_part_right = krnlEngine(program_h, q_h, context_h, "gqePart");
    krnl_JOIN_INNER_TD_314_part_right.setup_hp(512, 1, power_of_hpTimes_join, tbl_Filter_TD_426_output, tbl_Filter_TD_426_output_partition, cfg_JOIN_INNER_TD_314_cmds_part);
    krnlEngine krnl_JOIN_INNER_TD_314[hpTimes_join];
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_INNER_TD_314[i] = krnlEngine(program_h, q_h, "gqeJoin");
    }
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_INNER_TD_314[i].setup(tbl_Filter_TD_4727_output_partition_array[i], tbl_Filter_TD_426_output_partition_array[i], tbl_JOIN_INNER_TD_314_output_partition_array[i], , buftmp_h);
    }
    krnlEngine krnl_JOIN_INNER_TD_314_build;
    krnl_JOIN_INNER_TD_314_build = krnlEngine(program_h, q_h, context_h, "gqeKernel", 0);
    krnl_JOIN_INNER_TD_314_build.setup(tbl_Filter_TD_4727_output, tbl_Filter_TD_426_output, tbl_JOIN_INNER_TD_314_output, cfg_JOIN_INNER_TD_314_cmds_build, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_314_probe;
    krnl_JOIN_INNER_TD_314_probe = krnlEngine(program_h, q_h, context_h, "gqeKernel", 1);
    krnl_JOIN_INNER_TD_314_probe.setup(tbl_Filter_TD_426_output, tbl_Filter_TD_4727_output, tbl_JOIN_INNER_TD_314_output, cfg_JOIN_INNER_TD_314_cmds_probe, buftmp_h);
    // ************************** Transfer Engine *********************** // 
    transEngine trans_Aggregate_TD_133;
    trans_Aggregate_TD_133.setq(q_a, 0);
    trans_Aggregate_TD_133.add(&(cfg_Aggregate_TD_133_cmds_part));
    trans_Aggregate_TD_133.add(&(cfg_Aggregate_TD_133_cmds));
    trans_Aggregate_TD_133.add(&(cfg_Aggregate_TD_133_cmds_out));
    transEngine trans_Aggregate_TD_133_out;
    trans_Aggregate_TD_133_out.setq(q_a, 0);
    q_a.finish();
    transEngine trans_JOIN_INNER_TD_2246;
    trans_JOIN_INNER_TD_2246.setq(q_h,0);
    trans_JOIN_INNER_TD_2246.add(&(cfg_JOIN_INNER_TD_2246_cmds_part));
    trans_JOIN_INNER_TD_2246.add(&(cfg_JOIN_INNER_TD_2246_cmds));
    transEngine trans_JOIN_INNER_TD_2246_out;
    trans_JOIN_INNER_TD_2246_out.setq(q_h,0);
    q_h.finish();
    transEngine trans_JOIN_INNER_TD_314;
    trans_JOIN_INNER_TD_314.setq(q_h,1);
    trans_JOIN_INNER_TD_314.add(&(cfg_JOIN_INNER_TD_314_cmds_build));
    trans_JOIN_INNER_TD_314.add(&(cfg_JOIN_INNER_TD_314_cmds_probe));
    transEngine trans_JOIN_INNER_TD_314_out;
    trans_JOIN_INNER_TD_314_out.setq(q_h,1);
    q_h.finish();
    // ****************************** Events **************************** // 
    std::vector<cl::Event> events_h2d_wr_Aggregate_TD_133;
    std::vector<cl::Event> events_d2h_rd_Aggregate_TD_133;
    std::vector<cl::Event> events_Aggregate_TD_133[2];
    events_h2d_wr_Aggregate_TD_133.resize(1);
    events_d2h_rd_Aggregate_TD_133.resize(1);
    events_Aggregate_TD_133[0].resize(1);
    events_Aggregate_TD_133[1].resize(hpTimes_aggr);
    std::vector<cl::Event> events_grp_Aggregate_TD_133;
    std::vector<cl::Event> prev_events_grp_Aggregate_TD_133;
    std::vector<cl::Event> events_h2d_wr_JOIN_INNER_TD_2246;
    std::vector<cl::Event> events_d2h_rd_JOIN_INNER_TD_2246;
    std::vector<cl::Event> events_JOIN_INNER_TD_2246[2];
    events_h2d_wr_JOIN_INNER_TD_2246.resize(1);
    events_d2h_rd_JOIN_INNER_TD_2246.resize(1);
    events_JOIN_INNER_TD_2246[0].resize(1);
    events_JOIN_INNER_TD_2246[1].resize(hpTimes_join);
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_2246;
    std::vector<cl::Event> prev_events_grp_JOIN_INNER_TD_2246;
    std::vector<cl::Event> events_h2d_wr_JOIN_INNER_TD_314;
    std::vector<cl::Event> events_d2h_rd_JOIN_INNER_TD_314;
    std::vector<cl::Event> events_JOIN_INNER_TD_314[2];
    events_h2d_wr_JOIN_INNER_TD_314.resize(1);
    events_d2h_rd_JOIN_INNER_TD_314.resize(1);
    events_JOIN_INNER_TD_314[0].resize(2);
    events_JOIN_INNER_TD_314[1].resize(hpTimes_join);
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_314;
    std::vector<cl::Event> prev_events_grp_JOIN_INNER_TD_314;
    // **************************** Operations ************************** // 
    struct timeval tv_r_s, tv_r_e; 
    gettimeofday(&tv_r_s, 0); 

    struct timeval tv_r_Filter_4_340_s, tv_r_Filter_4_340_e;
    gettimeofday(&tv_r_Filter_4_340_s, 0);
    SW_Filter_TD_426(tbl_SerializeFromObject_TD_526_input, tbl_Filter_TD_426_output);
    gettimeofday(&tv_r_Filter_4_340_e, 0);

    struct timeval tv_r_Filter_4_569_s, tv_r_Filter_4_569_e;
    gettimeofday(&tv_r_Filter_4_569_s, 0);
    SW_Filter_TD_4727(tbl_SerializeFromObject_TD_5928_input, tbl_Filter_TD_4727_output);
    gettimeofday(&tv_r_Filter_4_569_e, 0);

    struct timeval tv_r_JOIN_INNER_3_470_s, tv_r_JOIN_INNER_3_470_e;
    gettimeofday(&tv_r_JOIN_INNER_3_470_s, 0);
    tbl_Filter_TD_4727_output.tableToCol();
    tbl_Filter_TD_426_output.tableToCol();
    krnl_JOIN_INNER_TD_314_build.updateMeta(tbl_Filter_TD_4727_output.getNumRow(), 3);
    krnl_JOIN_INNER_TD_314_probe.updateMeta(tbl_Filter_TD_426_output.getNumRow(), 3);
    trans_JOIN_INNER_TD_314.add(&(tbl_Filter_TD_4727_output));
    trans_JOIN_INNER_TD_314.add(&(tbl_Filter_TD_426_output));
    trans_JOIN_INNER_TD_314.host2dev(0, &(prev_events_grp_JOIN_INNER_TD_314), &(events_h2d_wr_JOIN_INNER_TD_314[0]));
    events_grp_JOIN_INNER_TD_314.push_back(events_h2d_wr_JOIN_INNER_TD_314[0]);
    krnl_JOIN_INNER_TD_314_part_left.run(0, &(events_grp_JOIN_INNER_TD_314), &(events_JOIN_INNER_TD_314[0][0]));
    krnl_JOIN_INNER_TD_314_part_right.run(0, &(events_grp_JOIN_INNER_TD_314), &(events_JOIN_INNER_TD_314[0][1]));
    for (int i(0); i < hpTimes_join; ++i) {
        krnl_JOIN_INNER_TD_314[i].run(0, &(events_JOIN_INNER_TD_314[0]), &(events_JOIN_INNER_TD_314[1][i]));
    }
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_314_build_done;
    events_grp_JOIN_INNER_TD_314_build_done.push_back(events_JOIN_INNER_TD_314[0]);
    krnl_JOIN_INNER_TD_314_part_left.run(0, &(events_grp_JOIN_INNER_TD_314), &(events_JOIN_INNER_TD_314[0][0]));
    krnl_JOIN_INNER_TD_314_part_right.run(0, &(events_grp_JOIN_INNER_TD_314), &(events_JOIN_INNER_TD_314[0][1]));
    for (int i(0); i < hpTimes_join; ++i) {
        krnl_JOIN_INNER_TD_314[i].run(0, &(events_JOIN_INNER_TD_314[0]), &(events_JOIN_INNER_TD_314[1][i]));
    }
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_314_probe_done;
    events_grp_JOIN_INNER_TD_314_probe_done.push_back(events_JOIN_INNER_TD_314[1]);
    for (int i(0); i < hpTimes_join; ++i) {
        trans_JOIN_INNER_TD_314_out.add(&(tbl_JOIN_INNER_TD_314_output_partition_array[i]));
    }
    trans_JOIN_INNER_TD_314_out.dev2host(0, &(events_JOIN_INNER_TD_314[1]), &(events_d2h_rd_JOIN_INNER_TD_314[0]));
    q_h.flush();
    q_h.finish();
    tbl_JOIN_INNER_TD_314_output.setNumRow((cfg_JOIN_INNER_TD_314_cmds_probe.meta_out)->getColLen());
    tbl_JOIN_INNER_TD_314_output.colToTable();
    SW_Consolidated_JOIN_INNER_TD_314_output(tbl_Filter_TD_4727_output, tbl_Filter_TD_426_output, tbl_JOIN_INNER_TD_314_output, tbl_JOIN_INNER_TD_314_consolidated_output);
    gettimeofday(&tv_r_JOIN_INNER_3_470_e, 0);

    struct timeval tv_r_Filter_3_871_s, tv_r_Filter_3_871_e;
    gettimeofday(&tv_r_Filter_3_871_s, 0);
    SW_Filter_TD_3612(tbl_SerializeFromObject_TD_4597_input, tbl_Filter_TD_3612_output);
    gettimeofday(&tv_r_Filter_3_871_e, 0);

    struct timeval tv_r_JOIN_INNER_2_98_s, tv_r_JOIN_INNER_2_98_e;
    gettimeofday(&tv_r_JOIN_INNER_2_98_s, 0);
    prev_events_grp_JOIN_INNER_TD_2246.push_back(events_h2d_wr_JOIN_INNER_TD_314[0]);
    trans_JOIN_INNER_TD_2246.add(&(tbl_Filter_TD_3612_output));
    trans_JOIN_INNER_TD_2246.host2dev(0, &(prev_events_grp_JOIN_INNER_TD_2246), &(events_h2d_wr_JOIN_INNER_TD_2246[0]));
    events_grp_JOIN_INNER_TD_2246.push_back(events_h2d_wr_JOIN_INNER_TD_2246[0]);
    events_grp_JOIN_INNER_TD_2246.insert(std::end(events_grp_JOIN_INNER_TD_2246), std::begin(events_JOIN_INNER_TD_314[0]), std::end(events_JOIN_INNER_TD_314[0]));
    events_grp_JOIN_INNER_TD_2246.insert(std::end(events_grp_JOIN_INNER_TD_2246), std::begin(events_JOIN_INNER_TD_314[1]), std::end(events_JOIN_INNER_TD_314[1]));
    krnl_JOIN_INNER_TD_2246_part_right.run(0, &(events_grp_JOIN_INNER_TD_2246), &(events_JOIN_INNER_TD_2246[0][0]));
    for (int i(0); i < hpTimes_join; ++i) {
        krnl_JOIN_INNER_TD_2246[i].run(0, &(events_grp_JOIN_INNER_TD_2246), &(events_JOIN_INNER_TD_2246[1][i]));
    }
    for (int i(0); i < hpTimes_join; ++i) {
        trans_JOIN_INNER_TD_2246_out.add(&(tbl_JOIN_INNER_TD_2246_output_preprocess_partition_array[i]));
    }
    trans_JOIN_INNER_TD_2246_out.dev2host(0, &(events_JOIN_INNER_TD_2246[1]), &(events_d2h_rd_JOIN_INNER_TD_2246[0]));
    q_h.flush();
    q_h.finish();
    WaitForEvents(events_d2h_rd_JOIN_INNER_TD_2246);
    tbl_JOIN_INNER_TD_2246_output_preprocess.copyTableData(&(tbl_JOIN_INNER_TD_2246_output));
    gettimeofday(&tv_r_JOIN_INNER_2_98_e, 0);

    struct timeval tv_r_Aggregate_1_309_s, tv_r_Aggregate_1_309_e;
    gettimeofday(&tv_r_Aggregate_1_309_s, 0);
    trans_Aggregate_TD_133.add(&(tbl_JOIN_INNER_TD_2246_output));
    trans_Aggregate_TD_133.host2dev(0, &(prev_events_grp_Aggregate_TD_133), &(events_h2d_wr_Aggregate_TD_133[0]));
    events_grp_Aggregate_TD_133.push_back(events_h2d_wr_Aggregate_TD_133[0]);
    krnl_Aggregate_TD_133_part.run(0, &(events_grp_Aggregate_TD_133), &(events_Aggregate_TD_133[0][0]));
    for (int i(0); i < hpTimes_aggr; ++i) {
        krnl_Aggregate_TD_133[i].run(0, &(events_Aggregate_TD_133[0]), &(events_Aggregate_TD_133[1][i]));
    }
    
    for (int i(0); i < hpTimes_aggr; ++i) {
        trans_Aggregate_TD_133_out.add(&(tbl_Aggregate_TD_133_output_partition_array[i]));
    }
    trans_Aggregate_TD_133_out.dev2host(0, &(events_Aggregate_TD_133[1]), &(events_d2h_rd_Aggregate_TD_133[0]));
    q_a.flush();
    q_a.finish();
    
    SW_Aggregate_TD_133_consolidate(tbl_Aggregate_TD_133_output_partition_array, tbl_Aggregate_TD_133_output, hpTimes_aggr);
    gettimeofday(&tv_r_Aggregate_1_309_e, 0);

    struct timeval tv_r_Sort_0_834_s, tv_r_Sort_0_834_e;
    gettimeofday(&tv_r_Sort_0_834_s, 0);
    SW_Sort_TD_0483(tbl_Aggregate_TD_133_output, tbl_Sort_TD_0483_output);
    gettimeofday(&tv_r_Sort_0_834_e, 0);

std::cout << "CPU TIME: " << (tvdiff(&tv_r_Filter_4_340_s, &tv_r_Filter_4_340_e) + 
tvdiff(&tv_r_Filter_4_569_s, &tv_r_Filter_4_569_e) + 
tvdiff(&tv_r_Filter_3_871_s, &tv_r_Filter_3_871_e) + 
tvdiff(&tv_r_Sort_0_834_s, &tv_r_Sort_0_834_e) + 
1) / 1000.0 << std::endl;

std::cout << "FPGA TIME: " << (tvdiff(&tv_r_JOIN_INNER_3_470_s, &tv_r_JOIN_INNER_3_470_e) + 
tvdiff(&tv_r_JOIN_INNER_2_98_s, &tv_r_JOIN_INNER_2_98_e) + 
tvdiff(&tv_r_Aggregate_1_309_s, &tv_r_Aggregate_1_309_e) + 
1) / 1000.0 << std::endl;

    gettimeofday(&tv_r_e, 0); 
    // **************************** Print Execution Time ************************** // 
    std::cout << std::endl; 
    std::cout << "Filter_4: " << tvdiff(&tv_r_Filter_4_340_s, &tv_r_Filter_4_340_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer((o_orderdate#211 < 19950307)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_526_input.getNumRow() << " -> tbl_SerializeFromObject_TD_526_input" 
     << std::endl << "      #COL: 4: " << "ListBuffer(o_orderkey#207, o_custkey#208, o_orderdate#211, o_shippriority#214)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_426_output.getNumRow() << " -> tbl_Filter_TD_426_output" 
     << std::endl << "      #COL: 4: " << "ListBuffer(o_orderkey#207, o_custkey#208, o_orderdate#211, o_shippriority#214)" 
     << std::endl; 

    std::cout << "Filter_4: " << tvdiff(&tv_r_Filter_4_569_s, &tv_r_Filter_4_569_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer((isnotnull(c_mktsegment#28) AND (c_mktsegment#28 = MACHINERY))) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_5928_input.getNumRow() << " -> tbl_SerializeFromObject_TD_5928_input" 
     << std::endl << "      #COL: 2: " << "ListBuffer(c_custkey#22, c_mktsegment#28)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_4727_output.getNumRow() << " -> tbl_Filter_TD_4727_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(c_custkey#22)" 
     << std::endl; 

    std::cout << "JOIN_INNER_3: " << tvdiff(&tv_r_JOIN_INNER_3_470_s, &tv_r_JOIN_INNER_3_470_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer((c_custkey#22 = o_custkey#208)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_4727_output.getNumRow() << " -> tbl_Filter_TD_4727_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(c_custkey#22)" 
     << std::endl << "      #ROW: " << tbl_Filter_TD_426_output.getNumRow() << " -> tbl_Filter_TD_426_output" 
     << std::endl << "      #COL: 4: " << "ListBuffer(o_orderkey#207, o_custkey#208, o_orderdate#211, o_shippriority#214)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_314_consolidated_output.getNumRow() << " -> tbl_JOIN_INNER_TD_314_consolidated_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(o_orderkey#207, o_orderdate#211, o_shippriority#214)" 
     << std::endl; 

    std::cout << "Filter_3: " << tvdiff(&tv_r_Filter_3_871_s, &tv_r_Filter_3_871_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer((l_shipdate#87 > 19950307)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_4597_input.getNumRow() << " -> tbl_SerializeFromObject_TD_4597_input" 
     << std::endl << "      #COL: 4: " << "ListBuffer(l_orderkey#77, l_extendedprice#82, l_discount#83, l_shipdate#87)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_3612_output.getNumRow() << " -> tbl_Filter_TD_3612_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(l_orderkey#77, l_extendedprice#82, l_discount#83)" 
     << std::endl; 

    std::cout << "JOIN_INNER_2: " << tvdiff(&tv_r_JOIN_INNER_2_98_s, &tv_r_JOIN_INNER_2_98_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer((l_orderkey#77 = o_orderkey#207)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_3612_output.getNumRow() << " -> tbl_Filter_TD_3612_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(l_orderkey#77, l_extendedprice#82, l_discount#83)" 
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_314_consolidated_output.getNumRow() << " -> tbl_JOIN_INNER_TD_314_consolidated_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(o_orderkey#207, o_orderdate#211, o_shippriority#214)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_2246_output.getNumRow() << " -> tbl_JOIN_INNER_TD_2246_output" 
     << std::endl << "      #COL: 5: " << "ListBuffer(o_orderdate#211, o_shippriority#214, l_orderkey#77, l_extendedprice#82, l_discount#83)" 
     << std::endl; 

    std::cout << "Aggregate_1: " << tvdiff(&tv_r_Aggregate_1_309_s, &tv_r_Aggregate_1_309_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer(l_orderkey#77, o_orderdate#211, o_shippriority#214, sum((l_extendedprice#82 * (100 - l_discount#83))) AS revenue#686L) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_2246_output.getNumRow() << " -> tbl_JOIN_INNER_TD_2246_output" 
     << std::endl << "      #COL: 5: " << "ListBuffer(o_orderdate#211, o_shippriority#214, l_orderkey#77, l_extendedprice#82, l_discount#83)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_133_output.getNumRow() << " -> tbl_Aggregate_TD_133_output" 
     << std::endl << "      #COL: 4: " << "ListBuffer(l_orderkey#77, revenue#686L, o_orderdate#211, o_shippriority#214)" 
     << std::endl; 

    std::cout << "Sort_0: " << tvdiff(&tv_r_Sort_0_834_s, &tv_r_Sort_0_834_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(revenue#686L DESC NULLS LAST, o_orderdate#211 ASC NULLS FIRST) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_133_output.getNumRow() << " -> tbl_Aggregate_TD_133_output" 
     << std::endl << "      #COL: 4: " << "ListBuffer(l_orderkey#77, revenue#686L, o_orderdate#211, o_shippriority#214)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Sort_TD_0483_output.getNumRow() << " -> tbl_Sort_TD_0483_output" 
     << std::endl << "      #COL: 4: " << "ListBuffer(l_orderkey#77, revenue#686L, o_orderdate#211, o_shippriority#214)" 
     << std::endl; 

    std::cout << std::endl << " Total execution time: " << tvdiff(&tv_r_s, &tv_r_e) / 1000 << " ms"; 

    std::cout << std::endl << " Spark elapsed time: " << 24.840988 * 1000 << "ms" << std::endl; 
    return 0; 
}
