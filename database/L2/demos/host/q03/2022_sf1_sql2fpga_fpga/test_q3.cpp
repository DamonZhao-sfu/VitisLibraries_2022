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
    // ****************************** Tables **************************** // 
    Table tbl_Sort_TD_0903_output("tbl_Sort_TD_0903_output", 6100000, 4, "");
    tbl_Sort_TD_0903_output.allocateHost();
    Table tbl_Aggregate_TD_1493_output_preprocess("tbl_Aggregate_TD_1493_output_preprocess", 24000, 16, "");
    tbl_Aggregate_TD_1493_output_preprocess.selectOverlayVersion(0);
    tbl_Aggregate_TD_1493_output_preprocess.allocateHost();
    Table tbl_Aggregate_TD_1493_output("tbl_Aggregate_TD_1493_output", 24000, 4, "");
    tbl_Aggregate_TD_1493_output.allocateHost();
    Table tbl_JOIN_INNER_TD_2119_output_preprocess("tbl_JOIN_INNER_TD_2119_output_preprocess", 30000, 5, "");
    tbl_JOIN_INNER_TD_2119_output_preprocess.selectOverlayVersion(0);
    tbl_JOIN_INNER_TD_2119_output_preprocess.allocateHost();
    Table tbl_JOIN_INNER_TD_2119_output("tbl_JOIN_INNER_TD_2119_output", 30000, 5, "");
    tbl_JOIN_INNER_TD_2119_output_preprocess.selectOverlayVersion(0);
    tbl_JOIN_INNER_TD_2119_output.allocateHost();
    Table tbl_Filter_TD_3261_output("tbl_Filter_TD_3261_output", 6100000, 3, "");
    tbl_Filter_TD_3261_output.selectOverlayVersion(1);
    tbl_Filter_TD_3261_output.allocateHost();
    Table tbl_JOIN_INNER_TD_3142_output("tbl_JOIN_INNER_TD_3142_output", 150000, 4, "");
    tbl_JOIN_INNER_TD_3142_output.selectOverlayVersion(1);
    tbl_JOIN_INNER_TD_3142_output.allocateHost();
    Table tbl_JOIN_INNER_TD_3142_consolidated_output("tbl_JOIN_INNER_TD_3142_consolidated_output", 150000, 3, "");
    tbl_JOIN_INNER_TD_3142_consolidated_output.selectOverlayVersion(1);
    tbl_JOIN_INNER_TD_3142_consolidated_output.allocateHost();
    Table tbl_SerializeFromObject_TD_4873_input;
    tbl_SerializeFromObject_TD_4873_input = Table("lineitem", lineitem_n, 4, in_dir);
    tbl_SerializeFromObject_TD_4873_input.addCol("l_orderkey", 4);
    tbl_SerializeFromObject_TD_4873_input.addCol("l_extendedprice", 4);
    tbl_SerializeFromObject_TD_4873_input.addCol("l_discount", 4);
    tbl_SerializeFromObject_TD_4873_input.addCol("l_shipdate", 4);
    tbl_SerializeFromObject_TD_4873_input.allocateHost();
    tbl_SerializeFromObject_TD_4873_input.loadHost();
    Table tbl_Filter_TD_4568_output("tbl_Filter_TD_4568_output", 6100000, 1, "");
    tbl_Filter_TD_4568_output.selectOverlayVersion(1);
    tbl_Filter_TD_4568_output.allocateHost();
    Table tbl_Filter_TD_4933_output("tbl_Filter_TD_4933_output", 6100000, 4, "");
    tbl_Filter_TD_4933_output.selectOverlayVersion(1);
    tbl_Filter_TD_4933_output.allocateHost();
    Table tbl_SerializeFromObject_TD_5692_input;
    tbl_SerializeFromObject_TD_5692_input = Table("customer", customer_n, 2, in_dir);
    tbl_SerializeFromObject_TD_5692_input.addCol("c_custkey", 4);
    tbl_SerializeFromObject_TD_5692_input.addCol("c_mktsegment", TPCH_READ_MAXAGG_LEN+1);
    tbl_SerializeFromObject_TD_5692_input.allocateHost();
    tbl_SerializeFromObject_TD_5692_input.loadHost();
    Table tbl_SerializeFromObject_TD_5174_input;
    tbl_SerializeFromObject_TD_5174_input = Table("order", order_n, 4, in_dir);
    tbl_SerializeFromObject_TD_5174_input.addCol("o_orderkey", 4);
    tbl_SerializeFromObject_TD_5174_input.addCol("o_custkey", 4);
    tbl_SerializeFromObject_TD_5174_input.addCol("o_orderdate", 4);
    tbl_SerializeFromObject_TD_5174_input.addCol("o_shippriority", 4);
    tbl_SerializeFromObject_TD_5174_input.allocateHost();
    tbl_SerializeFromObject_TD_5174_input.loadHost();
    // ********************** Allocate Device Buffer ******************** // 
    tbl_JOIN_INNER_TD_2119_output.allocateDevBuffer(context_a, 32);
    tbl_Aggregate_TD_1493_output_preprocess.allocateDevBuffer(context_a, 33);
    tbl_JOIN_INNER_TD_2119_output_preprocess.allocateDevBuffer(context_h, 32);
    //tbl_Filter_TD_3261_output.allocateDevBuffer(context_h, 32);
    //tbl_JOIN_INNER_TD_3142_output.allocateDevBuffer(context_h, 32);
    //tbl_Filter_TD_4568_output.allocateDevBuffer(context_h, 32);
    //tbl_Filter_TD_4933_output.allocateDevBuffer(context_h, 32);
    // ****************************** Config **************************** // 
    AggrCfgCmd cfg_Aggregate_TD_1493_cmds;
    cfg_Aggregate_TD_1493_cmds.allocateHost();
    get_cfg_dat_Aggregate_TD_1493_gqe_aggr(cfg_Aggregate_TD_1493_cmds.cmd);
    cfg_Aggregate_TD_1493_cmds.allocateDevBuffer(context_a, 32);
    AggrCfgCmd cfg_Aggregate_TD_1493_cmds_out;
    cfg_Aggregate_TD_1493_cmds_out.allocateHost();
    cfg_Aggregate_TD_1493_cmds_out.allocateDevBuffer(context_a, 33);
    cfgCmd cfg_JOIN_INNER_TD_2119_cmds;
    cfg_JOIN_INNER_TD_2119_cmds.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_2119_gqe_join (cfg_JOIN_INNER_TD_2119_cmds.cmd);
    cfg_JOIN_INNER_TD_2119_cmds.allocateDevBuffer(context_h, 32);
    
    xf::database::gqe::KernelCommand krn_cmd_build_29 = xf::database::gqe::KernelCommand();
    xf::database::gqe::KernelCommand krn_cmd_probe_29 = xf::database::gqe::KernelCommand();
    cfgCmd cfg_JOIN_INNER_TD_3142_cmds_build;
    cfg_JOIN_INNER_TD_3142_cmds_build.setup(1);
    cfg_JOIN_INNER_TD_3142_cmds_build.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_3142_gqe_join_build(krn_cmd_build_29);
    cfg_JOIN_INNER_TD_3142_cmds_build.cmd = krn_cmd_build_29.getConfigBits();
    get_meta_info_JOIN_INNER_TD_3142_gqe_join_build(cfg_JOIN_INNER_TD_3142_cmds_build.meta_in, tbl_Filter_TD_4568_output.nrow, 3);
    get_meta_info_JOIN_INNER_TD_3142_gqe_join_build(cfg_JOIN_INNER_TD_3142_cmds_build.meta_out, tbl_JOIN_INNER_TD_3142_output.nrow, 4);
    // cfg_JOIN_INNER_TD_3142_cmds_build.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_3142_cmds_probe;
    cfg_JOIN_INNER_TD_3142_cmds_probe.setup(1);
    cfg_JOIN_INNER_TD_3142_cmds_probe.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_3142_gqe_join_probe(krn_cmd_probe_29);
    cfg_JOIN_INNER_TD_3142_cmds_probe.cmd = krn_cmd_probe_29.getConfigBits();
    get_meta_info_JOIN_INNER_TD_3142_gqe_join_probe(cfg_JOIN_INNER_TD_3142_cmds_probe.meta_in, tbl_Filter_TD_4933_output.nrow, 3);
    get_meta_info_JOIN_INNER_TD_3142_gqe_join_probe(cfg_JOIN_INNER_TD_3142_cmds_probe.meta_out, tbl_JOIN_INNER_TD_3142_output.nrow, 3);
    // cfg_JOIN_INNER_TD_3142_cmds_probe.allocateDevBuffer(context_h, 32);
    cfg_JOIN_INNER_TD_3142_cmds_build.input_key_columns = {0, -1, -1};
    cfg_JOIN_INNER_TD_3142_cmds_probe.input_key_columns = {1, -1, -1};
    cfg_JOIN_INNER_TD_3142_cmds_build.output_key_columns = {0, 1, 2, 3};
    cfg_JOIN_INNER_TD_3142_cmds_probe.output_key_columns = {0, 1, 2, 3};
    // *************************** Kernel Setup ************************* // 
    bufferTmp buftmp_h(context_h, 1); 
    buftmp_h.initBuffer(q_h); 
    AggrBufferTmp buftmp_a(context_a, 1); 
    buftmp_a.BufferInitial(q_a); 
    std::cout << std::endl; 
    AggrKrnlEngine krnl_Aggregate_TD_1493;
    krnl_Aggregate_TD_1493 = AggrKrnlEngine(program_a, q_a, context_a, "gqeAggr");
    krnl_Aggregate_TD_1493.setup(tbl_JOIN_INNER_TD_2119_output, tbl_Aggregate_TD_1493_output_preprocess, cfg_Aggregate_TD_1493_cmds, cfg_Aggregate_TD_1493_cmds_out, buftmp_a);
    krnlEngine krnl_JOIN_INNER_TD_2119;
    krnl_JOIN_INNER_TD_2119 = krnlEngine(program_h, q_h, context_h, "gqeJoin");
    krnl_JOIN_INNER_TD_2119.setup(tbl_JOIN_INNER_TD_3142_consolidated_output, tbl_Filter_TD_3261_output, tbl_JOIN_INNER_TD_2119_output_preprocess, cfg_JOIN_INNER_TD_2119_cmds, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_3142_build;
    krnl_JOIN_INNER_TD_3142_build = krnlEngine(program_h, q_h, context_h, "gqeKernel", 0);
    krnl_JOIN_INNER_TD_3142_build.setup(tbl_Filter_TD_4568_output, tbl_Filter_TD_4933_output, tbl_JOIN_INNER_TD_3142_output, cfg_JOIN_INNER_TD_3142_cmds_build, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_3142_probe;
    krnl_JOIN_INNER_TD_3142_probe = krnlEngine(program_h, q_h, context_h, "gqeKernel", 1);
    krnl_JOIN_INNER_TD_3142_probe.setup(tbl_Filter_TD_4933_output, tbl_Filter_TD_4568_output, tbl_JOIN_INNER_TD_3142_output, cfg_JOIN_INNER_TD_3142_cmds_probe, buftmp_h);
    // ************************** Transfer Engine *********************** // 
    transEngine trans_Aggregate_TD_1493;
    trans_Aggregate_TD_1493.setq(q_a, 0);
    trans_Aggregate_TD_1493.add(&(cfg_Aggregate_TD_1493_cmds));
    trans_Aggregate_TD_1493.add(&(cfg_Aggregate_TD_1493_cmds_out));
    transEngine trans_Aggregate_TD_1493_out;
    trans_Aggregate_TD_1493_out.setq(q_a, 0);
    q_a.finish();
    transEngine trans_JOIN_INNER_TD_2119;
    trans_JOIN_INNER_TD_2119.setq(q_h,0);
    trans_JOIN_INNER_TD_2119.add(&(cfg_JOIN_INNER_TD_2119_cmds));
    transEngine trans_JOIN_INNER_TD_2119_out;
    trans_JOIN_INNER_TD_2119_out.setq(q_h,0);
    q_h.finish();
    transEngine trans_JOIN_INNER_TD_3142;
    trans_JOIN_INNER_TD_3142.setq(q_h,1);
    trans_JOIN_INNER_TD_3142.add(&(cfg_JOIN_INNER_TD_3142_cmds_build));
    trans_JOIN_INNER_TD_3142.add(&(cfg_JOIN_INNER_TD_3142_cmds_probe));
    transEngine trans_JOIN_INNER_TD_3142_out;
    trans_JOIN_INNER_TD_3142_out.setq(q_h,1);
    q_h.finish();
    // ****************************** Events **************************** // 
    std::vector<cl::Event> events_h2d_wr_Aggregate_TD_1493;
    std::vector<cl::Event> events_d2h_rd_Aggregate_TD_1493;
    std::vector<cl::Event> events_Aggregate_TD_1493;
    events_h2d_wr_Aggregate_TD_1493.resize(1);
    events_d2h_rd_Aggregate_TD_1493.resize(1);
    events_Aggregate_TD_1493.resize(1);
    std::vector<cl::Event> events_grp_Aggregate_TD_1493;
    std::vector<cl::Event> prev_events_grp_Aggregate_TD_1493;
    std::vector<cl::Event> events_h2d_wr_JOIN_INNER_TD_2119;
    std::vector<cl::Event> events_d2h_rd_JOIN_INNER_TD_2119;
    std::vector<cl::Event> events_JOIN_INNER_TD_2119;
    events_h2d_wr_JOIN_INNER_TD_2119.resize(1);
    events_d2h_rd_JOIN_INNER_TD_2119.resize(1);
    events_JOIN_INNER_TD_2119.resize(1);
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_2119;
    std::vector<cl::Event> prev_events_grp_JOIN_INNER_TD_2119;
    std::vector<cl::Event> events_h2d_wr_JOIN_INNER_TD_3142;
    std::vector<cl::Event> events_d2h_rd_JOIN_INNER_TD_3142;
    std::vector<cl::Event> events_JOIN_INNER_TD_3142;
    events_h2d_wr_JOIN_INNER_TD_3142.resize(1);
    events_d2h_rd_JOIN_INNER_TD_3142.resize(1);
    events_JOIN_INNER_TD_3142.resize(2);
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_3142;
    std::vector<cl::Event> prev_events_grp_JOIN_INNER_TD_3142;
    // **************************** Operations ************************** // 
    struct timeval tv_r_s, tv_r_e; 
    gettimeofday(&tv_r_s, 0); 

    struct timeval tv_r_Filter_4_422_s, tv_r_Filter_4_422_e;
    gettimeofday(&tv_r_Filter_4_422_s, 0);
    SW_Filter_TD_4933(tbl_SerializeFromObject_TD_5174_input, tbl_Filter_TD_4933_output);
    gettimeofday(&tv_r_Filter_4_422_e, 0);

    struct timeval tv_r_Filter_4_758_s, tv_r_Filter_4_758_e;
    gettimeofday(&tv_r_Filter_4_758_s, 0);
    SW_Filter_TD_4568(tbl_SerializeFromObject_TD_5692_input, tbl_Filter_TD_4568_output);
    gettimeofday(&tv_r_Filter_4_758_e, 0);

    struct timeval tv_r_JOIN_INNER_3_141_s, tv_r_JOIN_INNER_3_141_e;
    gettimeofday(&tv_r_JOIN_INNER_3_141_s, 0);
    tbl_Filter_TD_4568_output.tableToCol();
    tbl_Filter_TD_4933_output.tableToCol();
    krnl_JOIN_INNER_TD_3142_build.updateMeta(tbl_Filter_TD_4568_output.getNumRow(), 3);
    krnl_JOIN_INNER_TD_3142_probe.updateMeta(tbl_Filter_TD_4933_output.getNumRow(), 3);
    trans_JOIN_INNER_TD_3142.add(&(tbl_Filter_TD_4568_output));
    trans_JOIN_INNER_TD_3142.add(&(tbl_Filter_TD_4933_output));
    trans_JOIN_INNER_TD_3142.host2dev(0, &(prev_events_grp_JOIN_INNER_TD_3142), &(events_h2d_wr_JOIN_INNER_TD_3142[0]));
    events_grp_JOIN_INNER_TD_3142.push_back(events_h2d_wr_JOIN_INNER_TD_3142[0]);
    krnl_JOIN_INNER_TD_3142_build.run(0, &(events_grp_JOIN_INNER_TD_3142), &(events_JOIN_INNER_TD_3142[0]));
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_3142_build_done;
    events_grp_JOIN_INNER_TD_3142_build_done.push_back(events_JOIN_INNER_TD_3142[0]);
    krnl_JOIN_INNER_TD_3142_probe.run(0, &(events_grp_JOIN_INNER_TD_3142_build_done), &(events_JOIN_INNER_TD_3142[1]));
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_3142_probe_done;
    events_grp_JOIN_INNER_TD_3142_probe_done.push_back(events_JOIN_INNER_TD_3142[1]);
    trans_JOIN_INNER_TD_3142_out.add(&(tbl_JOIN_INNER_TD_3142_output));
    trans_JOIN_INNER_TD_3142_out.add(&(cfg_JOIN_INNER_TD_3142_cmds_build));
    trans_JOIN_INNER_TD_3142_out.add(&(cfg_JOIN_INNER_TD_3142_cmds_probe));
    trans_JOIN_INNER_TD_3142_out.dev2host(0, &(events_grp_JOIN_INNER_TD_3142_probe_done), &(events_d2h_rd_JOIN_INNER_TD_3142[0]));
    q_h.flush();
    q_h.finish();
    tbl_JOIN_INNER_TD_3142_output.setNumRow((cfg_JOIN_INNER_TD_3142_cmds_probe.meta_out)->getColLen());
    tbl_JOIN_INNER_TD_3142_output.colToTable();
    SW_Consolidated_JOIN_INNER_TD_3142_output(tbl_Filter_TD_4568_output, tbl_Filter_TD_4933_output, tbl_JOIN_INNER_TD_3142_output, tbl_JOIN_INNER_TD_3142_consolidated_output);
    gettimeofday(&tv_r_JOIN_INNER_3_141_e, 0);

    struct timeval tv_r_Filter_3_972_s, tv_r_Filter_3_972_e;
    gettimeofday(&tv_r_Filter_3_972_s, 0);
    SW_Filter_TD_3261(tbl_SerializeFromObject_TD_4873_input, tbl_Filter_TD_3261_output);
    gettimeofday(&tv_r_Filter_3_972_e, 0);

    struct timeval tv_r_JOIN_INNER_2_457_s, tv_r_JOIN_INNER_2_457_e;
    gettimeofday(&tv_r_JOIN_INNER_2_457_s, 0);
    prev_events_grp_JOIN_INNER_TD_2119.push_back(events_h2d_wr_JOIN_INNER_TD_3142[0]);
    trans_JOIN_INNER_TD_2119.add(&(tbl_Filter_TD_3261_output));
    trans_JOIN_INNER_TD_2119.host2dev(0, &(prev_events_grp_JOIN_INNER_TD_2119), &(events_h2d_wr_JOIN_INNER_TD_2119[0]));
    events_grp_JOIN_INNER_TD_2119.push_back(events_h2d_wr_JOIN_INNER_TD_2119[0]);
    events_grp_JOIN_INNER_TD_2119.push_back(events_JOIN_INNER_TD_3142[0]);
    krnl_JOIN_INNER_TD_2119.run(0, &(events_grp_JOIN_INNER_TD_2119), &(events_JOIN_INNER_TD_2119[0]));
    trans_JOIN_INNER_TD_2119_out.add(&(tbl_JOIN_INNER_TD_2119_output_preprocess));
    trans_JOIN_INNER_TD_2119_out.dev2host(0, &(events_JOIN_INNER_TD_2119), &(events_d2h_rd_JOIN_INNER_TD_2119[0]));
    q_h.flush();
    q_h.finish();
    WaitForEvents(events_d2h_rd_JOIN_INNER_TD_2119);
    tbl_JOIN_INNER_TD_2119_output_preprocess.copyTableData(&(tbl_JOIN_INNER_TD_2119_output));
    gettimeofday(&tv_r_JOIN_INNER_2_457_e, 0);

    struct timeval tv_r_Aggregate_1_326_s, tv_r_Aggregate_1_326_e;
    gettimeofday(&tv_r_Aggregate_1_326_s, 0);
    trans_Aggregate_TD_1493.add(&(tbl_JOIN_INNER_TD_2119_output));
    trans_Aggregate_TD_1493.host2dev(0, &(prev_events_grp_Aggregate_TD_1493), &(events_h2d_wr_Aggregate_TD_1493[0]));
    events_grp_Aggregate_TD_1493.push_back(events_h2d_wr_Aggregate_TD_1493[0]);
    krnl_Aggregate_TD_1493.run(0, &(events_grp_Aggregate_TD_1493), &(events_Aggregate_TD_1493[0]));
    
    trans_Aggregate_TD_1493_out.add(&(tbl_Aggregate_TD_1493_output_preprocess));
    trans_Aggregate_TD_1493_out.add(&(cfg_Aggregate_TD_1493_cmds_out));
    trans_Aggregate_TD_1493_out.dev2host(0, &(events_Aggregate_TD_1493), &(events_d2h_rd_Aggregate_TD_1493[0]));
    q_a.flush();
    q_a.finish();
    
    SW_Aggregate_TD_1493_consolidate(tbl_Aggregate_TD_1493_output_preprocess, tbl_Aggregate_TD_1493_output);
    gettimeofday(&tv_r_Aggregate_1_326_e, 0);

    struct timeval tv_r_Sort_0_653_s, tv_r_Sort_0_653_e;
    gettimeofday(&tv_r_Sort_0_653_s, 0);
    SW_Sort_TD_0903(tbl_Aggregate_TD_1493_output, tbl_Sort_TD_0903_output);
    gettimeofday(&tv_r_Sort_0_653_e, 0);

std::cout << "CPU TIME: " << (tvdiff(&tv_r_Filter_4_422_s, &tv_r_Filter_4_422_e) + 
tvdiff(&tv_r_Filter_4_758_s, &tv_r_Filter_4_758_e) + 
tvdiff(&tv_r_Filter_3_972_s, &tv_r_Filter_3_972_e) + 
tvdiff(&tv_r_Sort_0_653_s, &tv_r_Sort_0_653_e) + 
1) / 1000.0 << std::endl;

std::cout << "FPGA TIME: " << (tvdiff(&tv_r_JOIN_INNER_3_141_s, &tv_r_JOIN_INNER_3_141_e) + 
tvdiff(&tv_r_JOIN_INNER_2_457_s, &tv_r_JOIN_INNER_2_457_e) + 
tvdiff(&tv_r_Aggregate_1_326_s, &tv_r_Aggregate_1_326_e) + 
1) / 1000.0 << std::endl;

    gettimeofday(&tv_r_e, 0); 
    // **************************** Print Execution Time ************************** // 
    std::cout << std::endl; 
    std::cout << "Filter_4: " << tvdiff(&tv_r_Filter_4_422_s, &tv_r_Filter_4_422_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer((o_orderdate#211 < 19950307)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_5174_input.getNumRow() << " -> tbl_SerializeFromObject_TD_5174_input" 
     << std::endl << "      #COL: 4: " << "ListBuffer(o_orderkey#207, o_custkey#208, o_orderdate#211, o_shippriority#214)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_4933_output.getNumRow() << " -> tbl_Filter_TD_4933_output" 
     << std::endl << "      #COL: 4: " << "ListBuffer(o_orderkey#207, o_custkey#208, o_orderdate#211, o_shippriority#214)" 
     << std::endl; 

    std::cout << "Filter_4: " << tvdiff(&tv_r_Filter_4_758_s, &tv_r_Filter_4_758_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer((isnotnull(c_mktsegment#28) AND (c_mktsegment#28 = MACHINERY))) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_5692_input.getNumRow() << " -> tbl_SerializeFromObject_TD_5692_input" 
     << std::endl << "      #COL: 2: " << "ListBuffer(c_custkey#22, c_mktsegment#28)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_4568_output.getNumRow() << " -> tbl_Filter_TD_4568_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(c_custkey#22)" 
     << std::endl; 

    std::cout << "JOIN_INNER_3: " << tvdiff(&tv_r_JOIN_INNER_3_141_s, &tv_r_JOIN_INNER_3_141_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer((c_custkey#22 = o_custkey#208)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_4568_output.getNumRow() << " -> tbl_Filter_TD_4568_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(c_custkey#22)" 
     << std::endl << "      #ROW: " << tbl_Filter_TD_4933_output.getNumRow() << " -> tbl_Filter_TD_4933_output" 
     << std::endl << "      #COL: 4: " << "ListBuffer(o_orderkey#207, o_custkey#208, o_orderdate#211, o_shippriority#214)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_3142_consolidated_output.getNumRow() << " -> tbl_JOIN_INNER_TD_3142_consolidated_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(o_orderkey#207, o_orderdate#211, o_shippriority#214)" 
     << std::endl; 

    std::cout << "Filter_3: " << tvdiff(&tv_r_Filter_3_972_s, &tv_r_Filter_3_972_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer((l_shipdate#87 > 19950307)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_4873_input.getNumRow() << " -> tbl_SerializeFromObject_TD_4873_input" 
     << std::endl << "      #COL: 4: " << "ListBuffer(l_orderkey#77, l_extendedprice#82, l_discount#83, l_shipdate#87)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_3261_output.getNumRow() << " -> tbl_Filter_TD_3261_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(l_orderkey#77, l_extendedprice#82, l_discount#83)" 
     << std::endl; 

    std::cout << "JOIN_INNER_2: " << tvdiff(&tv_r_JOIN_INNER_2_457_s, &tv_r_JOIN_INNER_2_457_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer((l_orderkey#77 = o_orderkey#207)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_3261_output.getNumRow() << " -> tbl_Filter_TD_3261_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(l_orderkey#77, l_extendedprice#82, l_discount#83)" 
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_3142_consolidated_output.getNumRow() << " -> tbl_JOIN_INNER_TD_3142_consolidated_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(o_orderkey#207, o_orderdate#211, o_shippriority#214)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_2119_output.getNumRow() << " -> tbl_JOIN_INNER_TD_2119_output" 
     << std::endl << "      #COL: 5: " << "ListBuffer(o_orderdate#211, o_shippriority#214, l_orderkey#77, l_extendedprice#82, l_discount#83)" 
     << std::endl; 

    std::cout << "Aggregate_1: " << tvdiff(&tv_r_Aggregate_1_326_s, &tv_r_Aggregate_1_326_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer(l_orderkey#77, o_orderdate#211, o_shippriority#214, sum((l_extendedprice#82 * (100 - l_discount#83))) AS revenue#686L) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_2119_output.getNumRow() << " -> tbl_JOIN_INNER_TD_2119_output" 
     << std::endl << "      #COL: 5: " << "ListBuffer(o_orderdate#211, o_shippriority#214, l_orderkey#77, l_extendedprice#82, l_discount#83)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_1493_output.getNumRow() << " -> tbl_Aggregate_TD_1493_output" 
     << std::endl << "      #COL: 4: " << "ListBuffer(l_orderkey#77, revenue#686L, o_orderdate#211, o_shippriority#214)" 
     << std::endl; 

    std::cout << "Sort_0: " << tvdiff(&tv_r_Sort_0_653_s, &tv_r_Sort_0_653_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(revenue#686L DESC NULLS LAST, o_orderdate#211 ASC NULLS FIRST) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_1493_output.getNumRow() << " -> tbl_Aggregate_TD_1493_output" 
     << std::endl << "      #COL: 4: " << "ListBuffer(l_orderkey#77, revenue#686L, o_orderdate#211, o_shippriority#214)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Sort_TD_0903_output.getNumRow() << " -> tbl_Sort_TD_0903_output" 
     << std::endl << "      #COL: 4: " << "ListBuffer(l_orderkey#77, revenue#686L, o_orderdate#211, o_shippriority#214)" 
     << std::endl; 

    std::cout << std::endl << " Total execution time: " << tvdiff(&tv_r_s, &tv_r_e) / 1000 << " ms"; 

    std::cout << std::endl << " Spark elapsed time: " << 25.563742 * 1000 << "ms" << std::endl; 
    return 0; 
}
