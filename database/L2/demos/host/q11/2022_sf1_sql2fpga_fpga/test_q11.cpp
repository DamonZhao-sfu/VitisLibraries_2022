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
    // ****************************** Tables **************************** // 
    Table tbl_Sort_TD_0476_output("tbl_Sort_TD_0476_output", 6100000, 2, "");
    tbl_Sort_TD_0476_output.allocateHost();
    Table tbl_Filter_TD_159_output("tbl_Filter_TD_159_output", 6100000, 2, "");
    tbl_Filter_TD_159_output.allocateHost();
    Table tbl_Aggregate_TD_2700_output("tbl_Aggregate_TD_2700_output", 6100000, 2, "");
    tbl_Aggregate_TD_2700_output.allocateHost();
    Table tbl_Aggregate_TD_2811_output("tbl_Aggregate_TD_2811_output", 6100000, 1, "");
    tbl_Aggregate_TD_2811_output.allocateHost();
    Table tbl_JOIN_INNER_TD_4820_output("tbl_JOIN_INNER_TD_4820_output", 31440, 4, "");
    tbl_JOIN_INNER_TD_4820_output.selectOverlayVersion(1);
    tbl_JOIN_INNER_TD_4820_output.allocateHost();
    Table tbl_JOIN_INNER_TD_4820_consolidated_output("tbl_JOIN_INNER_TD_4820_consolidated_output", 31440, 3, "");
    tbl_JOIN_INNER_TD_4820_consolidated_output.selectOverlayVersion(1);
    tbl_JOIN_INNER_TD_4820_consolidated_output.allocateHost();
    Table tbl_SerializeFromObject_TD_584_input;
    tbl_SerializeFromObject_TD_584_input = Table("partsupp", partsupp_n, 4, in_dir);
    tbl_SerializeFromObject_TD_584_input.selectOverlayVersion(1);
    tbl_SerializeFromObject_TD_584_input.addCol("ps_partkey", 4);
    tbl_SerializeFromObject_TD_584_input.addCol("ps_suppkey", 4);
    tbl_SerializeFromObject_TD_584_input.addCol("ps_availqty", 4);
    tbl_SerializeFromObject_TD_584_input.addCol("ps_supplycost", 4);
    tbl_SerializeFromObject_TD_584_input.allocateHost();
    tbl_SerializeFromObject_TD_584_input.loadHost();
    Table tbl_JOIN_INNER_TD_3883_output("tbl_JOIN_INNER_TD_3883_output", 393, 4, "");
    tbl_JOIN_INNER_TD_3883_output.selectOverlayVersion(1);
    tbl_JOIN_INNER_TD_3883_output.allocateHost();
    Table tbl_JOIN_INNER_TD_3883_consolidated_output("tbl_JOIN_INNER_TD_3883_consolidated_output", 393, 1, "");
    tbl_JOIN_INNER_TD_3883_consolidated_output.selectOverlayVersion(1);
    tbl_JOIN_INNER_TD_3883_consolidated_output.allocateHost();
    Table tbl_SerializeFromObject_TD_5178_input;
    tbl_SerializeFromObject_TD_5178_input = Table("supplier", supplier_n, 2, in_dir);
    tbl_SerializeFromObject_TD_5178_input.selectOverlayVersion(1);
    tbl_SerializeFromObject_TD_5178_input.addCol("s_suppkey", 4);
    tbl_SerializeFromObject_TD_5178_input.addCol("s_nationkey", 4);
    tbl_SerializeFromObject_TD_5178_input.allocateHost();
    tbl_SerializeFromObject_TD_5178_input.loadHost();
    Table tbl_Filter_TD_4676_output("tbl_Filter_TD_4676_output", 6100000, 1, "");
    tbl_Filter_TD_4676_output.selectOverlayVersion(1);
    tbl_Filter_TD_4676_output.allocateHost();
    Table tbl_SerializeFromObject_TD_5998_input;
    tbl_SerializeFromObject_TD_5998_input = Table("nation", nation_n, 2, in_dir);
    tbl_SerializeFromObject_TD_5998_input.addCol("n_nationkey", 4);
    tbl_SerializeFromObject_TD_5998_input.addCol("n_name", TPCH_READ_NATION_LEN+1);
    tbl_SerializeFromObject_TD_5998_input.allocateHost();
    tbl_SerializeFromObject_TD_5998_input.loadHost();
    // ********************** Allocate Device Buffer ******************** // 
    //tbl_JOIN_INNER_TD_4820_output.allocateDevBuffer(context_h, 32);
    // tbl_SerializeFromObject_TD_584_input.allocateDevBuffer(context_h, 32);
    //tbl_JOIN_INNER_TD_3883_output.allocateDevBuffer(context_h, 32);
    // tbl_SerializeFromObject_TD_5178_input.allocateDevBuffer(context_h, 32);
    //tbl_Filter_TD_4676_output.allocateDevBuffer(context_h, 32);
    // ****************************** Config **************************** // 
    
    xf::database::gqe::KernelCommand krn_cmd_build_878 = xf::database::gqe::KernelCommand();
    xf::database::gqe::KernelCommand krn_cmd_probe_878 = xf::database::gqe::KernelCommand();
    cfgCmd cfg_JOIN_INNER_TD_4820_cmds_build;
    cfg_JOIN_INNER_TD_4820_cmds_build.setup(1);
    cfg_JOIN_INNER_TD_4820_cmds_build.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_4820_gqe_join_build(krn_cmd_build_878);
    cfg_JOIN_INNER_TD_4820_cmds_build.cmd = krn_cmd_build_878.getConfigBits();
    get_meta_info_JOIN_INNER_TD_4820_gqe_join_build(cfg_JOIN_INNER_TD_4820_cmds_build.meta_in, tbl_JOIN_INNER_TD_3883_consolidated_output.nrow, 3);
    get_meta_info_JOIN_INNER_TD_4820_gqe_join_build(cfg_JOIN_INNER_TD_4820_cmds_build.meta_out, tbl_JOIN_INNER_TD_4820_output.nrow, 4);
    // cfg_JOIN_INNER_TD_4820_cmds_build.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_4820_cmds_probe;
    cfg_JOIN_INNER_TD_4820_cmds_probe.setup(1);
    cfg_JOIN_INNER_TD_4820_cmds_probe.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_4820_gqe_join_probe(krn_cmd_probe_878);
    cfg_JOIN_INNER_TD_4820_cmds_probe.cmd = krn_cmd_probe_878.getConfigBits();
    get_meta_info_JOIN_INNER_TD_4820_gqe_join_probe(cfg_JOIN_INNER_TD_4820_cmds_probe.meta_in, tbl_SerializeFromObject_TD_584_input.nrow, 3);
    get_meta_info_JOIN_INNER_TD_4820_gqe_join_probe(cfg_JOIN_INNER_TD_4820_cmds_probe.meta_out, tbl_JOIN_INNER_TD_4820_output.nrow, 3);
    // cfg_JOIN_INNER_TD_4820_cmds_probe.allocateDevBuffer(context_h, 32);
    cfg_JOIN_INNER_TD_4820_cmds_build.input_key_columns = {0, -1, -1};
    cfg_JOIN_INNER_TD_4820_cmds_probe.input_key_columns = {1, -1, -1};
    cfg_JOIN_INNER_TD_4820_cmds_build.output_key_columns = {0, 1, 2, 3};
    cfg_JOIN_INNER_TD_4820_cmds_probe.output_key_columns = {0, 1, 2, 3};
    
    xf::database::gqe::KernelCommand krn_cmd_build_254 = xf::database::gqe::KernelCommand();
    xf::database::gqe::KernelCommand krn_cmd_probe_254 = xf::database::gqe::KernelCommand();
    cfgCmd cfg_JOIN_INNER_TD_3883_cmds_build;
    cfg_JOIN_INNER_TD_3883_cmds_build.setup(1);
    cfg_JOIN_INNER_TD_3883_cmds_build.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_3883_gqe_join_build(krn_cmd_build_254);
    cfg_JOIN_INNER_TD_3883_cmds_build.cmd = krn_cmd_build_254.getConfigBits();
    get_meta_info_JOIN_INNER_TD_3883_gqe_join_build(cfg_JOIN_INNER_TD_3883_cmds_build.meta_in, tbl_Filter_TD_4676_output.nrow, 3);
    get_meta_info_JOIN_INNER_TD_3883_gqe_join_build(cfg_JOIN_INNER_TD_3883_cmds_build.meta_out, tbl_JOIN_INNER_TD_3883_output.nrow, 4);
    // cfg_JOIN_INNER_TD_3883_cmds_build.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_3883_cmds_probe;
    cfg_JOIN_INNER_TD_3883_cmds_probe.setup(1);
    cfg_JOIN_INNER_TD_3883_cmds_probe.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_3883_gqe_join_probe(krn_cmd_probe_254);
    cfg_JOIN_INNER_TD_3883_cmds_probe.cmd = krn_cmd_probe_254.getConfigBits();
    get_meta_info_JOIN_INNER_TD_3883_gqe_join_probe(cfg_JOIN_INNER_TD_3883_cmds_probe.meta_in, tbl_SerializeFromObject_TD_5178_input.nrow, 3);
    get_meta_info_JOIN_INNER_TD_3883_gqe_join_probe(cfg_JOIN_INNER_TD_3883_cmds_probe.meta_out, tbl_JOIN_INNER_TD_3883_output.nrow, 3);
    // cfg_JOIN_INNER_TD_3883_cmds_probe.allocateDevBuffer(context_h, 32);
    cfg_JOIN_INNER_TD_3883_cmds_build.input_key_columns = {0, -1, -1};
    cfg_JOIN_INNER_TD_3883_cmds_probe.input_key_columns = {1, -1, -1};
    cfg_JOIN_INNER_TD_3883_cmds_build.output_key_columns = {0, 1, 2, 3};
    cfg_JOIN_INNER_TD_3883_cmds_probe.output_key_columns = {0, 1, 2, 3};
    // *************************** Kernel Setup ************************* // 
    bufferTmp buftmp_h(context_h, 1); 
    buftmp_h.initBuffer(q_h); 
    std::cout << std::endl; 
    krnlEngine krnl_JOIN_INNER_TD_4820_build;
    krnl_JOIN_INNER_TD_4820_build = krnlEngine(program_h, q_h, context_h, "gqeKernel", 0);
    krnl_JOIN_INNER_TD_4820_build.setup(tbl_JOIN_INNER_TD_3883_consolidated_output, tbl_SerializeFromObject_TD_584_input, tbl_JOIN_INNER_TD_4820_output, cfg_JOIN_INNER_TD_4820_cmds_build, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_4820_probe;
    krnl_JOIN_INNER_TD_4820_probe = krnlEngine(program_h, q_h, context_h, "gqeKernel", 1);
    krnl_JOIN_INNER_TD_4820_probe.setup(tbl_SerializeFromObject_TD_584_input, tbl_JOIN_INNER_TD_3883_consolidated_output, tbl_JOIN_INNER_TD_4820_output, cfg_JOIN_INNER_TD_4820_cmds_probe, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_3883_build;
    krnl_JOIN_INNER_TD_3883_build = krnlEngine(program_h, q_h, context_h, "gqeKernel", 0);
    krnl_JOIN_INNER_TD_3883_build.setup(tbl_Filter_TD_4676_output, tbl_SerializeFromObject_TD_5178_input, tbl_JOIN_INNER_TD_3883_output, cfg_JOIN_INNER_TD_3883_cmds_build, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_3883_probe;
    krnl_JOIN_INNER_TD_3883_probe = krnlEngine(program_h, q_h, context_h, "gqeKernel", 1);
    krnl_JOIN_INNER_TD_3883_probe.setup(tbl_SerializeFromObject_TD_5178_input, tbl_Filter_TD_4676_output, tbl_JOIN_INNER_TD_3883_output, cfg_JOIN_INNER_TD_3883_cmds_probe, buftmp_h);
    // ************************** Transfer Engine *********************** // 
    transEngine trans_JOIN_INNER_TD_4820;
    trans_JOIN_INNER_TD_4820.setq(q_h,1);
    trans_JOIN_INNER_TD_4820.add(&(cfg_JOIN_INNER_TD_4820_cmds_build));
    trans_JOIN_INNER_TD_4820.add(&(cfg_JOIN_INNER_TD_4820_cmds_probe));
    trans_JOIN_INNER_TD_4820.add(&(tbl_SerializeFromObject_TD_584_input));
    transEngine trans_JOIN_INNER_TD_4820_out;
    trans_JOIN_INNER_TD_4820_out.setq(q_h,1);
    q_h.finish();
    transEngine trans_JOIN_INNER_TD_3883;
    trans_JOIN_INNER_TD_3883.setq(q_h,1);
    trans_JOIN_INNER_TD_3883.add(&(cfg_JOIN_INNER_TD_3883_cmds_build));
    trans_JOIN_INNER_TD_3883.add(&(cfg_JOIN_INNER_TD_3883_cmds_probe));
    trans_JOIN_INNER_TD_3883.add(&(tbl_SerializeFromObject_TD_5178_input));
    transEngine trans_JOIN_INNER_TD_3883_out;
    trans_JOIN_INNER_TD_3883_out.setq(q_h,1);
    q_h.finish();
    // ****************************** Events **************************** // 
    std::vector<cl::Event> events_h2d_wr_JOIN_INNER_TD_4820;
    std::vector<cl::Event> events_d2h_rd_JOIN_INNER_TD_4820;
    std::vector<cl::Event> events_JOIN_INNER_TD_4820;
    events_h2d_wr_JOIN_INNER_TD_4820.resize(1);
    events_d2h_rd_JOIN_INNER_TD_4820.resize(1);
    events_JOIN_INNER_TD_4820.resize(2);
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_4820;
    std::vector<cl::Event> prev_events_grp_JOIN_INNER_TD_4820;
    std::vector<cl::Event> events_h2d_wr_JOIN_INNER_TD_3883;
    std::vector<cl::Event> events_d2h_rd_JOIN_INNER_TD_3883;
    std::vector<cl::Event> events_JOIN_INNER_TD_3883;
    events_h2d_wr_JOIN_INNER_TD_3883.resize(1);
    events_d2h_rd_JOIN_INNER_TD_3883.resize(1);
    events_JOIN_INNER_TD_3883.resize(2);
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_3883;
    std::vector<cl::Event> prev_events_grp_JOIN_INNER_TD_3883;
    // **************************** Operations ************************** // 
    struct timeval tv_r_s, tv_r_e; 
    gettimeofday(&tv_r_s, 0); 

    struct timeval tv_r_Filter_4_505_s, tv_r_Filter_4_505_e;
    gettimeofday(&tv_r_Filter_4_505_s, 0);
    SW_Filter_TD_4676(tbl_SerializeFromObject_TD_5998_input, tbl_Filter_TD_4676_output);
    gettimeofday(&tv_r_Filter_4_505_e, 0);

    struct timeval tv_r_JOIN_INNER_3_392_s, tv_r_JOIN_INNER_3_392_e;
    gettimeofday(&tv_r_JOIN_INNER_3_392_s, 0);
    tbl_Filter_TD_4676_output.tableToCol();
    tbl_SerializeFromObject_TD_5178_input.tableToCol();
    krnl_JOIN_INNER_TD_3883_build.updateMeta(tbl_Filter_TD_4676_output.getNumRow(), 3);
    krnl_JOIN_INNER_TD_3883_probe.updateMeta(tbl_SerializeFromObject_TD_5178_input.getNumRow(), 3);
    trans_JOIN_INNER_TD_3883.add(&(tbl_Filter_TD_4676_output));
    trans_JOIN_INNER_TD_3883.host2dev(0, &(prev_events_grp_JOIN_INNER_TD_3883), &(events_h2d_wr_JOIN_INNER_TD_3883[0]));
    events_grp_JOIN_INNER_TD_3883.push_back(events_h2d_wr_JOIN_INNER_TD_3883[0]);
    krnl_JOIN_INNER_TD_3883_build.run(0, &(events_grp_JOIN_INNER_TD_3883), &(events_JOIN_INNER_TD_3883[0]));
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_3883_build_done;
    events_grp_JOIN_INNER_TD_3883_build_done.push_back(events_JOIN_INNER_TD_3883[0]);
    krnl_JOIN_INNER_TD_3883_probe.run(0, &(events_grp_JOIN_INNER_TD_3883_build_done), &(events_JOIN_INNER_TD_3883[1]));
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_3883_probe_done;
    events_grp_JOIN_INNER_TD_3883_probe_done.push_back(events_JOIN_INNER_TD_3883[1]);
    trans_JOIN_INNER_TD_3883_out.add(&(tbl_JOIN_INNER_TD_3883_output));
    trans_JOIN_INNER_TD_3883_out.add(&(cfg_JOIN_INNER_TD_3883_cmds_build));
    trans_JOIN_INNER_TD_3883_out.add(&(cfg_JOIN_INNER_TD_3883_cmds_probe));
    trans_JOIN_INNER_TD_3883_out.dev2host(0, &(events_grp_JOIN_INNER_TD_3883_probe_done), &(events_d2h_rd_JOIN_INNER_TD_3883[0]));
    q_h.flush();
    q_h.finish();
    tbl_JOIN_INNER_TD_3883_output.setNumRow((cfg_JOIN_INNER_TD_3883_cmds_probe.meta_out)->getColLen());
    tbl_JOIN_INNER_TD_3883_output.colToTable();
    SW_Consolidated_JOIN_INNER_TD_3883_output(tbl_Filter_TD_4676_output, tbl_SerializeFromObject_TD_5178_input, tbl_JOIN_INNER_TD_3883_output, tbl_JOIN_INNER_TD_3883_consolidated_output);
    gettimeofday(&tv_r_JOIN_INNER_3_392_e, 0);

    struct timeval tv_r_JOIN_INNER_4_572_s, tv_r_JOIN_INNER_4_572_e;
    gettimeofday(&tv_r_JOIN_INNER_4_572_s, 0);
    prev_events_grp_JOIN_INNER_TD_4820.push_back(events_h2d_wr_JOIN_INNER_TD_3883[0]);
    tbl_JOIN_INNER_TD_3883_consolidated_output.tableToCol();
    tbl_SerializeFromObject_TD_584_input.tableToCol();
    krnl_JOIN_INNER_TD_4820_build.updateMeta(tbl_JOIN_INNER_TD_3883_consolidated_output.getNumRow(), 3);
    krnl_JOIN_INNER_TD_4820_probe.updateMeta(tbl_SerializeFromObject_TD_584_input.getNumRow(), 3);
    trans_JOIN_INNER_TD_4820.host2dev(0, &(prev_events_grp_JOIN_INNER_TD_4820), &(events_h2d_wr_JOIN_INNER_TD_4820[0]));
    events_grp_JOIN_INNER_TD_4820.push_back(events_h2d_wr_JOIN_INNER_TD_4820[0]);
    events_grp_JOIN_INNER_TD_4820.push_back(events_JOIN_INNER_TD_3883[0]);
    krnl_JOIN_INNER_TD_4820_build.run(0, &(events_grp_JOIN_INNER_TD_4820), &(events_JOIN_INNER_TD_4820[0]));
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_4820_build_done;
    events_grp_JOIN_INNER_TD_4820_build_done.push_back(events_JOIN_INNER_TD_4820[0]);
    krnl_JOIN_INNER_TD_4820_probe.run(0, &(events_grp_JOIN_INNER_TD_4820_build_done), &(events_JOIN_INNER_TD_4820[1]));
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_4820_probe_done;
    events_grp_JOIN_INNER_TD_4820_probe_done.push_back(events_JOIN_INNER_TD_4820[1]);
    trans_JOIN_INNER_TD_4820_out.add(&(tbl_JOIN_INNER_TD_4820_output));
    trans_JOIN_INNER_TD_4820_out.add(&(cfg_JOIN_INNER_TD_4820_cmds_build));
    trans_JOIN_INNER_TD_4820_out.add(&(cfg_JOIN_INNER_TD_4820_cmds_probe));
    trans_JOIN_INNER_TD_4820_out.dev2host(0, &(events_grp_JOIN_INNER_TD_4820_probe_done), &(events_d2h_rd_JOIN_INNER_TD_4820[0]));
    q_h.flush();
    q_h.finish();
    tbl_JOIN_INNER_TD_4820_output.setNumRow((cfg_JOIN_INNER_TD_4820_cmds_probe.meta_out)->getColLen());
    tbl_JOIN_INNER_TD_4820_output.colToTable();
    SW_Consolidated_JOIN_INNER_TD_4820_output(tbl_JOIN_INNER_TD_3883_consolidated_output, tbl_SerializeFromObject_TD_584_input, tbl_JOIN_INNER_TD_4820_output, tbl_JOIN_INNER_TD_4820_consolidated_output);
    gettimeofday(&tv_r_JOIN_INNER_4_572_e, 0);

    struct timeval tv_r_Aggregate_2_489_s, tv_r_Aggregate_2_489_e;
    gettimeofday(&tv_r_Aggregate_2_489_s, 0);
    SW_Aggregate_TD_2811(tbl_JOIN_INNER_TD_4820_consolidated_output, tbl_Aggregate_TD_2811_output);
    gettimeofday(&tv_r_Aggregate_2_489_e, 0);

    struct timeval tv_r_Aggregate_2_281_s, tv_r_Aggregate_2_281_e;
    gettimeofday(&tv_r_Aggregate_2_281_s, 0);
    SW_Aggregate_TD_2700(tbl_JOIN_INNER_TD_4820_consolidated_output, tbl_Aggregate_TD_2700_output);
    gettimeofday(&tv_r_Aggregate_2_281_e, 0);

    struct timeval tv_r_Filter_1_709_s, tv_r_Filter_1_709_e;
    gettimeofday(&tv_r_Filter_1_709_s, 0);
    SW_Filter_TD_159(tbl_Aggregate_TD_2700_output, tbl_Aggregate_TD_2811_output, tbl_Filter_TD_159_output);
    gettimeofday(&tv_r_Filter_1_709_e, 0);

    struct timeval tv_r_Sort_0_780_s, tv_r_Sort_0_780_e;
    gettimeofday(&tv_r_Sort_0_780_s, 0);
    SW_Sort_TD_0476(tbl_Filter_TD_159_output, tbl_Sort_TD_0476_output);
    gettimeofday(&tv_r_Sort_0_780_e, 0);

std::cout << "CPU TIME: " << (tvdiff(&tv_r_Filter_4_505_s, &tv_r_Filter_4_505_e) + 
tvdiff(&tv_r_Aggregate_2_489_s, &tv_r_Aggregate_2_489_e) + 
tvdiff(&tv_r_Aggregate_2_281_s, &tv_r_Aggregate_2_281_e) + 
tvdiff(&tv_r_Filter_1_709_s, &tv_r_Filter_1_709_e) + 
tvdiff(&tv_r_Sort_0_780_s, &tv_r_Sort_0_780_e) + 
1) / 1000.0 << std::endl;

std::cout << "FPGA TIME: " << (tvdiff(&tv_r_JOIN_INNER_3_392_s, &tv_r_JOIN_INNER_3_392_e) + 
tvdiff(&tv_r_JOIN_INNER_4_572_s, &tv_r_JOIN_INNER_4_572_e) + 
1) / 1000.0 << std::endl;

    gettimeofday(&tv_r_e, 0); 
    // **************************** Print Execution Time ************************** // 
    std::cout << std::endl; 
    std::cout << "Filter_4: " << tvdiff(&tv_r_Filter_4_505_s, &tv_r_Filter_4_505_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer((isnotnull(n_name#145) AND (n_name#145 = IRAN))) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_5998_input.getNumRow() << " -> tbl_SerializeFromObject_TD_5998_input" 
     << std::endl << "      #COL: 2: " << "ListBuffer(n_nationkey#144, n_name#145)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_4676_output.getNumRow() << " -> tbl_Filter_TD_4676_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(n_nationkey#144)" 
     << std::endl; 

    std::cout << "JOIN_INNER_3: " << tvdiff(&tv_r_JOIN_INNER_3_392_s, &tv_r_JOIN_INNER_3_392_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer((s_nationkey#345 = n_nationkey#144)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_5178_input.getNumRow() << " -> tbl_SerializeFromObject_TD_5178_input" 
     << std::endl << "      #COL: 2: " << "ListBuffer(s_suppkey#342, s_nationkey#345)" 
     << std::endl << "      #ROW: " << tbl_Filter_TD_4676_output.getNumRow() << " -> tbl_Filter_TD_4676_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(n_nationkey#144)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_3883_consolidated_output.getNumRow() << " -> tbl_JOIN_INNER_TD_3883_consolidated_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(s_suppkey#342)" 
     << std::endl; 

    std::cout << "JOIN_INNER_4: " << tvdiff(&tv_r_JOIN_INNER_4_572_s, &tv_r_JOIN_INNER_4_572_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer((ps_suppkey#306 = s_suppkey#342)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_584_input.getNumRow() << " -> tbl_SerializeFromObject_TD_584_input" 
     << std::endl << "      #COL: 4: " << "ListBuffer(ps_partkey#305, ps_suppkey#306, ps_availqty#307, ps_supplycost#308)" 
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_3883_consolidated_output.getNumRow() << " -> tbl_JOIN_INNER_TD_3883_consolidated_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(s_suppkey#342)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_4820_consolidated_output.getNumRow() << " -> tbl_JOIN_INNER_TD_4820_consolidated_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(ps_partkey#305, ps_availqty#307, ps_supplycost#308)" 
     << std::endl; 

    std::cout << "Aggregate_2: " << tvdiff(&tv_r_Aggregate_2_489_s, &tv_r_Aggregate_2_489_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(CheckOverflow((promote_precision(cast(cast(sum((ps_supplycost#308 * ps_availqty#307)) as decimal(20,0)) as decimal(30,10))) * 0.0001000000), DecimalType(31,10), true) AS (sum((ps_supplycost * ps_availqty)) * 0.0001000000)#1204) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_4820_consolidated_output.getNumRow() << " -> tbl_JOIN_INNER_TD_4820_consolidated_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(ps_partkey#305, ps_availqty#307, ps_supplycost#308)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_2811_output.getNumRow() << " -> tbl_Aggregate_TD_2811_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer((sum((ps_supplycost * ps_availqty)) * 0.0001000000)#1204)" 
     << std::endl; 

    std::cout << "Aggregate_2: " << tvdiff(&tv_r_Aggregate_2_281_s, &tv_r_Aggregate_2_281_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(ps_partkey#305, sum((ps_supplycost#308 * ps_availqty#307)) AS value#1200L) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_4820_consolidated_output.getNumRow() << " -> tbl_JOIN_INNER_TD_4820_consolidated_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(ps_partkey#305, ps_availqty#307, ps_supplycost#308)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_2700_output.getNumRow() << " -> tbl_Aggregate_TD_2700_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(ps_partkey#305, value#1200L)" 
     << std::endl; 

    std::cout << "Filter_1: " << tvdiff(&tv_r_Filter_1_709_s, &tv_r_Filter_1_709_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer((isnotnull(value#1200L) AND (cast(cast(value#1200L as decimal(20,0)) as decimal(31,10)) > scalar-subquery#1201 []))) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_2700_output.getNumRow() << " -> tbl_Aggregate_TD_2700_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(ps_partkey#305, value#1200L)" 
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_2811_output.getNumRow() << " -> tbl_Aggregate_TD_2811_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer((sum((ps_supplycost * ps_availqty)) * 0.0001000000)#1204)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_159_output.getNumRow() << " -> tbl_Filter_TD_159_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(ps_partkey#305, value#1200L)" 
     << std::endl; 

    std::cout << "Sort_0: " << tvdiff(&tv_r_Sort_0_780_s, &tv_r_Sort_0_780_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(value#1200L DESC NULLS LAST) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_159_output.getNumRow() << " -> tbl_Filter_TD_159_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(ps_partkey#305, value#1200L)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Sort_TD_0476_output.getNumRow() << " -> tbl_Sort_TD_0476_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(ps_partkey#305, value#1200L)" 
     << std::endl; 

    std::cout << std::endl << " Total execution time: " << tvdiff(&tv_r_s, &tv_r_e) / 1000 << " ms"; 

    std::cout << std::endl << " Spark elapsed time: " << 2.4161582 * 1000 << "ms" << std::endl; 
    return 0; 
}
