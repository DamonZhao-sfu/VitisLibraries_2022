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

#include "cfgFunc_q13.hpp" 
#include "q13.hpp" 

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
    std::cout << "NOTE:running query #13\n."; 
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
    Table tbl_Sort_TD_0688_output("tbl_Sort_TD_0688_output", 6100000, 2, "");
    tbl_Sort_TD_0688_output.allocateHost();
    Table tbl_Aggregate_TD_1692_output("tbl_Aggregate_TD_1692_output", 6100000, 2, "");
    tbl_Aggregate_TD_1692_output.allocateHost();
    Table tbl_Aggregate_TD_2851_output("tbl_Aggregate_TD_2851_output", 6100000, 1, "");
    tbl_Aggregate_TD_2851_output.allocateHost();
    Table tbl_JOIN_LEFTANTI_TD_3422_output("tbl_JOIN_LEFTANTI_TD_3422_output", 1531000, 4, "");
    tbl_JOIN_LEFTANTI_TD_3422_output.selectOverlayVersion(1);
    tbl_JOIN_LEFTANTI_TD_3422_output.allocateHost();
    Table tbl_JOIN_LEFTANTI_TD_3422_consolidated_output("tbl_JOIN_LEFTANTI_TD_3422_consolidated_output", 1531000, 2, "");
    tbl_JOIN_LEFTANTI_TD_3422_consolidated_output.selectOverlayVersion(1);
    tbl_JOIN_LEFTANTI_TD_3422_consolidated_output.allocateHost();
    Table tbl_JOIN_INNER_TD_0821_output("tbl_JOIN_INNER_TD_0821_output", 1480133, 4, "");
    tbl_JOIN_INNER_TD_0821_output.selectOverlayVersion(1);
    tbl_JOIN_INNER_TD_0821_output.allocateHost();
    Table tbl_JOIN_INNER_TD_0821_consolidated_output("tbl_JOIN_INNER_TD_0821_consolidated_output", 1480133, 3, "");
    tbl_JOIN_INNER_TD_0821_consolidated_output.selectOverlayVersion(1);
    tbl_JOIN_INNER_TD_0821_consolidated_output.allocateHost();
    Table tbl_SerializeFromObject_TD_4176_input;
    tbl_SerializeFromObject_TD_4176_input = Table("customer", customer_n, 1, in_dir);
    tbl_SerializeFromObject_TD_4176_input.selectOverlayVersion(1);
    tbl_SerializeFromObject_TD_4176_input.addCol("c_custkey", 4);
    tbl_SerializeFromObject_TD_4176_input.allocateHost();
    tbl_SerializeFromObject_TD_4176_input.loadHost();
    Table tbl_Filter_TD_4458_output("tbl_Filter_TD_4458_output", 6100000, 2, "");
    tbl_Filter_TD_4458_output.selectOverlayVersion(1);
    tbl_Filter_TD_4458_output.allocateHost();
    Table tbl_SerializeFromObject_TD_531_input;
    tbl_SerializeFromObject_TD_531_input = Table("order", order_n, 3, in_dir);
    tbl_SerializeFromObject_TD_531_input.addCol("o_orderkey", 4);
    tbl_SerializeFromObject_TD_531_input.addCol("o_custkey", 4);
    tbl_SerializeFromObject_TD_531_input.addCol("o_comment", TPCH_READ_O_CMNT_MAX+1);
    tbl_SerializeFromObject_TD_531_input.allocateHost();
    tbl_SerializeFromObject_TD_531_input.loadHost();
    // ********************** Allocate Device Buffer ******************** // 
    //tbl_JOIN_LEFTANTI_TD_3422_output.allocateDevBuffer(context_h, 32);
    //tbl_JOIN_INNER_TD_0821_output.allocateDevBuffer(context_h, 32);
    // tbl_SerializeFromObject_TD_4176_input.allocateDevBuffer(context_h, 32);
    //tbl_Filter_TD_4458_output.allocateDevBuffer(context_h, 32);
    // ****************************** Config **************************** // 
    
    xf::database::gqe::KernelCommand krn_cmd_build_279 = xf::database::gqe::KernelCommand();
    xf::database::gqe::KernelCommand krn_cmd_probe_279 = xf::database::gqe::KernelCommand();
    cfgCmd cfg_JOIN_LEFTANTI_TD_3422_cmds_build;
    cfg_JOIN_LEFTANTI_TD_3422_cmds_build.setup(1);
    cfg_JOIN_LEFTANTI_TD_3422_cmds_build.allocateHost();
    get_cfg_dat_JOIN_LEFTANTI_TD_3422_gqe_join_build(krn_cmd_build_279);
    cfg_JOIN_LEFTANTI_TD_3422_cmds_build.cmd = krn_cmd_build_279.getConfigBits();
    get_meta_info_JOIN_LEFTANTI_TD_3422_gqe_join_build(cfg_JOIN_LEFTANTI_TD_3422_cmds_build.meta_in, tbl_SerializeFromObject_TD_4176_input.nrow, 3);
    get_meta_info_JOIN_LEFTANTI_TD_3422_gqe_join_build(cfg_JOIN_LEFTANTI_TD_3422_cmds_build.meta_out, tbl_JOIN_LEFTANTI_TD_3422_output.nrow, 4);
    // cfg_JOIN_LEFTANTI_TD_3422_cmds_build.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_LEFTANTI_TD_3422_cmds_probe;
    cfg_JOIN_LEFTANTI_TD_3422_cmds_probe.setup(1);
    cfg_JOIN_LEFTANTI_TD_3422_cmds_probe.allocateHost();
    get_cfg_dat_JOIN_LEFTANTI_TD_3422_gqe_join_probe(krn_cmd_probe_279);
    cfg_JOIN_LEFTANTI_TD_3422_cmds_probe.cmd = krn_cmd_probe_279.getConfigBits();
    get_meta_info_JOIN_LEFTANTI_TD_3422_gqe_join_probe(cfg_JOIN_LEFTANTI_TD_3422_cmds_probe.meta_in, tbl_JOIN_INNER_TD_0821_consolidated_output.nrow, 3);
    get_meta_info_JOIN_LEFTANTI_TD_3422_gqe_join_probe(cfg_JOIN_LEFTANTI_TD_3422_cmds_probe.meta_out, tbl_JOIN_LEFTANTI_TD_3422_output.nrow, 3);
    // cfg_JOIN_LEFTANTI_TD_3422_cmds_probe.allocateDevBuffer(context_h, 32);
    cfg_JOIN_LEFTANTI_TD_3422_cmds_build.input_key_columns = {0, -1, -1};
    cfg_JOIN_LEFTANTI_TD_3422_cmds_probe.input_key_columns = {2, -1, -1};
    cfg_JOIN_LEFTANTI_TD_3422_cmds_build.output_key_columns = {0, 1, 2, 3};
    cfg_JOIN_LEFTANTI_TD_3422_cmds_probe.output_key_columns = {0, 1, 2, 3};
    
    xf::database::gqe::KernelCommand krn_cmd_build_507 = xf::database::gqe::KernelCommand();
    xf::database::gqe::KernelCommand krn_cmd_probe_507 = xf::database::gqe::KernelCommand();
    cfgCmd cfg_JOIN_INNER_TD_0821_cmds_build;
    cfg_JOIN_INNER_TD_0821_cmds_build.setup(1);
    cfg_JOIN_INNER_TD_0821_cmds_build.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_0821_gqe_join_build(krn_cmd_build_507);
    cfg_JOIN_INNER_TD_0821_cmds_build.cmd = krn_cmd_build_507.getConfigBits();
    get_meta_info_JOIN_INNER_TD_0821_gqe_join_build(cfg_JOIN_INNER_TD_0821_cmds_build.meta_in, tbl_SerializeFromObject_TD_4176_input.nrow, 3);
    get_meta_info_JOIN_INNER_TD_0821_gqe_join_build(cfg_JOIN_INNER_TD_0821_cmds_build.meta_out, tbl_JOIN_INNER_TD_0821_output.nrow, 4);
    // cfg_JOIN_INNER_TD_0821_cmds_build.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_0821_cmds_probe;
    cfg_JOIN_INNER_TD_0821_cmds_probe.setup(1);
    cfg_JOIN_INNER_TD_0821_cmds_probe.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_0821_gqe_join_probe(krn_cmd_probe_507);
    cfg_JOIN_INNER_TD_0821_cmds_probe.cmd = krn_cmd_probe_507.getConfigBits();
    get_meta_info_JOIN_INNER_TD_0821_gqe_join_probe(cfg_JOIN_INNER_TD_0821_cmds_probe.meta_in, tbl_Filter_TD_4458_output.nrow, 3);
    get_meta_info_JOIN_INNER_TD_0821_gqe_join_probe(cfg_JOIN_INNER_TD_0821_cmds_probe.meta_out, tbl_JOIN_INNER_TD_0821_output.nrow, 3);
    // cfg_JOIN_INNER_TD_0821_cmds_probe.allocateDevBuffer(context_h, 32);
    cfg_JOIN_INNER_TD_0821_cmds_build.input_key_columns = {0, -1, -1};
    cfg_JOIN_INNER_TD_0821_cmds_probe.input_key_columns = {1, -1, -1};
    cfg_JOIN_INNER_TD_0821_cmds_build.output_key_columns = {0, 1, 2, 3};
    cfg_JOIN_INNER_TD_0821_cmds_probe.output_key_columns = {0, 1, 2, 3};
    // *************************** Kernel Setup ************************* // 
    bufferTmp buftmp_h(context_h, 1); 
    buftmp_h.initBuffer(q_h); 
    std::cout << std::endl; 
    krnlEngine krnl_JOIN_LEFTANTI_TD_3422_build;
    krnl_JOIN_LEFTANTI_TD_3422_build = krnlEngine(program_h, q_h, context_h, "gqeKernel", 0);
    krnl_JOIN_LEFTANTI_TD_3422_build.setup(tbl_SerializeFromObject_TD_4176_input, tbl_JOIN_INNER_TD_0821_consolidated_output, tbl_JOIN_LEFTANTI_TD_3422_output, cfg_JOIN_LEFTANTI_TD_3422_cmds_build, buftmp_h);
    krnlEngine krnl_JOIN_LEFTANTI_TD_3422_probe;
    krnl_JOIN_LEFTANTI_TD_3422_probe = krnlEngine(program_h, q_h, context_h, "gqeKernel", 1);
    krnl_JOIN_LEFTANTI_TD_3422_probe.setup(tbl_JOIN_INNER_TD_0821_consolidated_output, tbl_SerializeFromObject_TD_4176_input, tbl_JOIN_LEFTANTI_TD_3422_output, cfg_JOIN_LEFTANTI_TD_3422_cmds_probe, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_0821_build;
    krnl_JOIN_INNER_TD_0821_build = krnlEngine(program_h, q_h, context_h, "gqeKernel", 0);
    krnl_JOIN_INNER_TD_0821_build.setup(tbl_SerializeFromObject_TD_4176_input, tbl_Filter_TD_4458_output, tbl_JOIN_INNER_TD_0821_output, cfg_JOIN_INNER_TD_0821_cmds_build, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_0821_probe;
    krnl_JOIN_INNER_TD_0821_probe = krnlEngine(program_h, q_h, context_h, "gqeKernel", 1);
    krnl_JOIN_INNER_TD_0821_probe.setup(tbl_Filter_TD_4458_output, tbl_SerializeFromObject_TD_4176_input, tbl_JOIN_INNER_TD_0821_output, cfg_JOIN_INNER_TD_0821_cmds_probe, buftmp_h);
    // ************************** Transfer Engine *********************** // 
    transEngine trans_JOIN_LEFTANTI_TD_3422;
    trans_JOIN_LEFTANTI_TD_3422.setq(q_h,1);
    trans_JOIN_LEFTANTI_TD_3422.add(&(cfg_JOIN_LEFTANTI_TD_3422_cmds_build));
    trans_JOIN_LEFTANTI_TD_3422.add(&(cfg_JOIN_LEFTANTI_TD_3422_cmds_probe));
    trans_JOIN_LEFTANTI_TD_3422.add(&(tbl_SerializeFromObject_TD_4176_input));
    transEngine trans_JOIN_LEFTANTI_TD_3422_out;
    trans_JOIN_LEFTANTI_TD_3422_out.setq(q_h,1);
    q_h.finish();
    transEngine trans_JOIN_INNER_TD_0821;
    trans_JOIN_INNER_TD_0821.setq(q_h,1);
    trans_JOIN_INNER_TD_0821.add(&(cfg_JOIN_INNER_TD_0821_cmds_build));
    trans_JOIN_INNER_TD_0821.add(&(cfg_JOIN_INNER_TD_0821_cmds_probe));
    trans_JOIN_INNER_TD_0821.add(&(tbl_SerializeFromObject_TD_4176_input));
    transEngine trans_JOIN_INNER_TD_0821_out;
    trans_JOIN_INNER_TD_0821_out.setq(q_h,1);
    q_h.finish();
    // ****************************** Events **************************** // 
    std::vector<cl::Event> events_h2d_wr_JOIN_LEFTANTI_TD_3422;
    std::vector<cl::Event> events_d2h_rd_JOIN_LEFTANTI_TD_3422;
    std::vector<cl::Event> events_JOIN_LEFTANTI_TD_3422;
    events_h2d_wr_JOIN_LEFTANTI_TD_3422.resize(1);
    events_d2h_rd_JOIN_LEFTANTI_TD_3422.resize(1);
    events_JOIN_LEFTANTI_TD_3422.resize(2);
    std::vector<cl::Event> events_grp_JOIN_LEFTANTI_TD_3422;
    std::vector<cl::Event> prev_events_grp_JOIN_LEFTANTI_TD_3422;
    std::vector<cl::Event> events_h2d_wr_JOIN_INNER_TD_0821;
    std::vector<cl::Event> events_d2h_rd_JOIN_INNER_TD_0821;
    std::vector<cl::Event> events_JOIN_INNER_TD_0821;
    events_h2d_wr_JOIN_INNER_TD_0821.resize(1);
    events_d2h_rd_JOIN_INNER_TD_0821.resize(1);
    events_JOIN_INNER_TD_0821.resize(2);
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_0821;
    std::vector<cl::Event> prev_events_grp_JOIN_INNER_TD_0821;
    // **************************** Operations ************************** // 
    struct timeval tv_r_s, tv_r_e; 
    gettimeofday(&tv_r_s, 0); 

    struct timeval tv_r_Filter_4_653_s, tv_r_Filter_4_653_e;
    gettimeofday(&tv_r_Filter_4_653_s, 0);
    SW_Filter_TD_4458(tbl_SerializeFromObject_TD_531_input, tbl_Filter_TD_4458_output);
    gettimeofday(&tv_r_Filter_4_653_e, 0);

    struct timeval tv_r_JOIN_INNER_0_47_s, tv_r_JOIN_INNER_0_47_e;
    gettimeofday(&tv_r_JOIN_INNER_0_47_s, 0);
    tbl_SerializeFromObject_TD_4176_input.tableToCol();
    tbl_Filter_TD_4458_output.tableToCol();
    krnl_JOIN_INNER_TD_0821_build.updateMeta(tbl_SerializeFromObject_TD_4176_input.getNumRow(), 3);
    krnl_JOIN_INNER_TD_0821_probe.updateMeta(tbl_Filter_TD_4458_output.getNumRow(), 3);
    trans_JOIN_INNER_TD_0821.add(&(tbl_Filter_TD_4458_output));
    trans_JOIN_INNER_TD_0821.host2dev(0, &(prev_events_grp_JOIN_INNER_TD_0821), &(events_h2d_wr_JOIN_INNER_TD_0821[0]));
    events_grp_JOIN_INNER_TD_0821.push_back(events_h2d_wr_JOIN_INNER_TD_0821[0]);
    krnl_JOIN_INNER_TD_0821_build.run(0, &(events_grp_JOIN_INNER_TD_0821), &(events_JOIN_INNER_TD_0821[0]));
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_0821_build_done;
    events_grp_JOIN_INNER_TD_0821_build_done.push_back(events_JOIN_INNER_TD_0821[0]);
    krnl_JOIN_INNER_TD_0821_probe.run(0, &(events_grp_JOIN_INNER_TD_0821_build_done), &(events_JOIN_INNER_TD_0821[1]));
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_0821_probe_done;
    events_grp_JOIN_INNER_TD_0821_probe_done.push_back(events_JOIN_INNER_TD_0821[1]);
    trans_JOIN_INNER_TD_0821_out.add(&(tbl_JOIN_INNER_TD_0821_output));
    trans_JOIN_INNER_TD_0821_out.add(&(cfg_JOIN_INNER_TD_0821_cmds_build));
    trans_JOIN_INNER_TD_0821_out.add(&(cfg_JOIN_INNER_TD_0821_cmds_probe));
    trans_JOIN_INNER_TD_0821_out.dev2host(0, &(events_grp_JOIN_INNER_TD_0821_probe_done), &(events_d2h_rd_JOIN_INNER_TD_0821[0]));
    q_h.flush();
    q_h.finish();
    tbl_JOIN_INNER_TD_0821_output.setNumRow((cfg_JOIN_INNER_TD_0821_cmds_probe.meta_out)->getColLen());
    tbl_JOIN_INNER_TD_0821_output.colToTable();
    SW_Consolidated_JOIN_INNER_TD_0821_output(tbl_SerializeFromObject_TD_4176_input, tbl_Filter_TD_4458_output, tbl_JOIN_INNER_TD_0821_output, tbl_JOIN_INNER_TD_0821_consolidated_output);
    gettimeofday(&tv_r_JOIN_INNER_0_47_e, 0);

    struct timeval tv_r_JOIN_LEFTANTI_3_17_s, tv_r_JOIN_LEFTANTI_3_17_e;
    gettimeofday(&tv_r_JOIN_LEFTANTI_3_17_s, 0);
    prev_events_grp_JOIN_LEFTANTI_TD_3422.push_back(events_h2d_wr_JOIN_INNER_TD_0821[0]);
    tbl_SerializeFromObject_TD_4176_input.tableToCol();
    tbl_JOIN_INNER_TD_0821_consolidated_output.tableToCol();
    krnl_JOIN_LEFTANTI_TD_3422_build.updateMeta(tbl_SerializeFromObject_TD_4176_input.getNumRow(), 3);
    krnl_JOIN_LEFTANTI_TD_3422_probe.updateMeta(tbl_JOIN_INNER_TD_0821_consolidated_output.getNumRow(), 3);
    trans_JOIN_LEFTANTI_TD_3422.host2dev(0, &(prev_events_grp_JOIN_LEFTANTI_TD_3422), &(events_h2d_wr_JOIN_LEFTANTI_TD_3422[0]));
    events_grp_JOIN_LEFTANTI_TD_3422.push_back(events_h2d_wr_JOIN_LEFTANTI_TD_3422[0]);
    events_grp_JOIN_LEFTANTI_TD_3422.push_back(events_JOIN_INNER_TD_0821[0]);
    krnl_JOIN_LEFTANTI_TD_3422_build.run(0, &(events_grp_JOIN_LEFTANTI_TD_3422), &(events_JOIN_LEFTANTI_TD_3422[0]));
    std::vector<cl::Event> events_grp_JOIN_LEFTANTI_TD_3422_build_done;
    events_grp_JOIN_LEFTANTI_TD_3422_build_done.push_back(events_JOIN_LEFTANTI_TD_3422[0]);
    krnl_JOIN_LEFTANTI_TD_3422_probe.run(0, &(events_grp_JOIN_LEFTANTI_TD_3422_build_done), &(events_JOIN_LEFTANTI_TD_3422[1]));
    std::vector<cl::Event> events_grp_JOIN_LEFTANTI_TD_3422_probe_done;
    events_grp_JOIN_LEFTANTI_TD_3422_probe_done.push_back(events_JOIN_LEFTANTI_TD_3422[1]);
    
    trans_JOIN_LEFTANTI_TD_3422_out.add(&(tbl_JOIN_LEFTANTI_TD_3422_output));
    trans_JOIN_LEFTANTI_TD_3422_out.add(&(tbl_JOIN_INNER_TD_0821_consolidated_output));
    trans_JOIN_LEFTANTI_TD_3422_out.dev2host(0, &(events_JOIN_LEFTANTI_TD_3422), &(events_d2h_rd_JOIN_LEFTANTI_TD_3422[0]));
    q_h.flush();
    q_h.finish();
    SW_JOIN_LEFTANTI_TD_3422_concatenate(tbl_JOIN_LEFTANTI_TD_3422_output, tbl_JOIN_INNER_TD_0821_consolidated_output);
    tbl_JOIN_LEFTANTI_TD_3422_output.copyTableData(&(tbl_JOIN_LEFTANTI_TD_3422_output));
    gettimeofday(&tv_r_JOIN_LEFTANTI_3_17_e, 0);

    struct timeval tv_r_Aggregate_2_890_s, tv_r_Aggregate_2_890_e;
    gettimeofday(&tv_r_Aggregate_2_890_s, 0);
    SW_Aggregate_TD_2851(tbl_JOIN_LEFTANTI_TD_3422_output, tbl_Aggregate_TD_2851_output);
    gettimeofday(&tv_r_Aggregate_2_890_e, 0);

    struct timeval tv_r_Aggregate_1_935_s, tv_r_Aggregate_1_935_e;
    gettimeofday(&tv_r_Aggregate_1_935_s, 0);
    SW_Aggregate_TD_1692(tbl_Aggregate_TD_2851_output, tbl_Aggregate_TD_1692_output);
    gettimeofday(&tv_r_Aggregate_1_935_e, 0);

    struct timeval tv_r_Sort_0_476_s, tv_r_Sort_0_476_e;
    gettimeofday(&tv_r_Sort_0_476_s, 0);
    SW_Sort_TD_0688(tbl_Aggregate_TD_1692_output, tbl_Sort_TD_0688_output);
    gettimeofday(&tv_r_Sort_0_476_e, 0);

std::cout << "CPU TIME: " << (tvdiff(&tv_r_Filter_4_653_s, &tv_r_Filter_4_653_e) + 
tvdiff(&tv_r_Aggregate_2_890_s, &tv_r_Aggregate_2_890_e) + 
tvdiff(&tv_r_Aggregate_1_935_s, &tv_r_Aggregate_1_935_e) + 
tvdiff(&tv_r_Sort_0_476_s, &tv_r_Sort_0_476_e) + 
1) / 1000.0 << std::endl;

std::cout << "FPGA TIME: " << (tvdiff(&tv_r_JOIN_INNER_0_47_s, &tv_r_JOIN_INNER_0_47_e) + 
tvdiff(&tv_r_JOIN_LEFTANTI_3_17_s, &tv_r_JOIN_LEFTANTI_3_17_e) + 
1) / 1000.0 << std::endl;

    gettimeofday(&tv_r_e, 0); 
    // **************************** Print Execution Time ************************** // 
    std::cout << std::endl; 
    std::cout << "Filter_4: " << tvdiff(&tv_r_Filter_4_653_s, &tv_r_Filter_4_653_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer((isnotnull(o_comment#215) AND NOT o_comment#215 LIKE %unusual%deposits%)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_531_input.getNumRow() << " -> tbl_SerializeFromObject_TD_531_input" 
     << std::endl << "      #COL: 3: " << "ListBuffer(o_orderkey#207, o_custkey#208, o_comment#215)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_4458_output.getNumRow() << " -> tbl_Filter_TD_4458_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(o_orderkey#207, o_custkey#208)" 
     << std::endl; 

    std::cout << "JOIN_INNER_0: " << tvdiff(&tv_r_JOIN_INNER_0_47_s, &tv_r_JOIN_INNER_0_47_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer((c_custkey#22 = o_custkey#208)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_4176_input.getNumRow() << " -> tbl_SerializeFromObject_TD_4176_input" 
     << std::endl << "      #COL: 1: " << "ListBuffer(c_custkey#22)" 
     << std::endl << "      #ROW: " << tbl_Filter_TD_4458_output.getNumRow() << " -> tbl_Filter_TD_4458_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(o_orderkey#207, o_custkey#208)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_0821_consolidated_output.getNumRow() << " -> tbl_JOIN_INNER_TD_0821_consolidated_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(c_custkey#22, o_orderkey#207, o_custkey#208)" 
     << std::endl; 

    std::cout << "JOIN_LEFTANTI_3: " << tvdiff(&tv_r_JOIN_LEFTANTI_3_17_s, &tv_r_JOIN_LEFTANTI_3_17_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer((c_custkey#22 = o_custkey#208)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_0821_consolidated_output.getNumRow() << " -> tbl_JOIN_INNER_TD_0821_consolidated_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(c_custkey#22, o_orderkey#207, o_custkey#208)" 
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_4176_input.getNumRow() << " -> tbl_SerializeFromObject_TD_4176_input" 
     << std::endl << "      #COL: 1: " << "ListBuffer(c_custkey#22)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_LEFTANTI_TD_3422_output.getNumRow() << " -> tbl_JOIN_LEFTANTI_TD_3422_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(c_custkey#22, o_orderkey#207)" 
     << std::endl; 

    std::cout << "Aggregate_2: " << tvdiff(&tv_r_Aggregate_2_890_s, &tv_r_Aggregate_2_890_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(c_custkey#22, count(o_orderkey#207) AS c_count#1365L) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_LEFTANTI_TD_3422_output.getNumRow() << " -> tbl_JOIN_LEFTANTI_TD_3422_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(c_custkey#22, o_orderkey#207)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_2851_output.getNumRow() << " -> tbl_Aggregate_TD_2851_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(c_count#1365L)" 
     << std::endl; 

    std::cout << "Aggregate_1: " << tvdiff(&tv_r_Aggregate_1_935_s, &tv_r_Aggregate_1_935_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(c_count#1365L, count(1) AS custdist#1360L) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_2851_output.getNumRow() << " -> tbl_Aggregate_TD_2851_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(c_count#1365L)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_1692_output.getNumRow() << " -> tbl_Aggregate_TD_1692_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(c_count#1365L, custdist#1360L)" 
     << std::endl; 

    std::cout << "Sort_0: " << tvdiff(&tv_r_Sort_0_476_s, &tv_r_Sort_0_476_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(custdist#1360L DESC NULLS LAST, c_count#1365L DESC NULLS LAST) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_1692_output.getNumRow() << " -> tbl_Aggregate_TD_1692_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(c_count#1365L, custdist#1360L)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Sort_TD_0688_output.getNumRow() << " -> tbl_Sort_TD_0688_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(c_count#1365L, custdist#1360L)" 
     << std::endl; 

    std::cout << std::endl << " Total execution time: " << tvdiff(&tv_r_s, &tv_r_e) / 1000 << " ms"; 

    std::cout << std::endl << " Spark elapsed time: " << 6.3078113 * 1000 << "ms" << std::endl; 
    return 0; 
}
