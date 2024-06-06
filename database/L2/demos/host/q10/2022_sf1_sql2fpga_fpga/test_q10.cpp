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

#include "cfgFunc_q10.hpp" 
#include "q10.hpp" 

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
    std::cout << "NOTE:running query #10\n."; 
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
    Table tbl_Sort_TD_0999_output("tbl_Sort_TD_0999_output", 6100000, 5, "");
    tbl_Sort_TD_0999_output.allocateHost();
    Table tbl_Aggregate_TD_155_output("tbl_Aggregate_TD_155_output", 6100000, 5, "");
    tbl_Aggregate_TD_155_output.allocateHost();
    Table tbl_JOIN_INNER_TD_2823_output("tbl_JOIN_INNER_TD_2823_output", 114347, 4, "");
    tbl_JOIN_INNER_TD_2823_output.selectOverlayVersion(1);
    tbl_JOIN_INNER_TD_2823_output.allocateHost();
    Table tbl_JOIN_INNER_TD_2823_consolidated_output("tbl_JOIN_INNER_TD_2823_consolidated_output", 114347, 6, "");
    tbl_JOIN_INNER_TD_2823_consolidated_output.selectOverlayVersion(1);
    tbl_JOIN_INNER_TD_2823_consolidated_output.allocateHost();
    Table tbl_JOIN_INNER_TD_3705_output("tbl_JOIN_INNER_TD_3705_output", 114347, 4, "");
    tbl_JOIN_INNER_TD_3705_output.selectOverlayVersion(1);
    tbl_JOIN_INNER_TD_3705_output.allocateHost();
    Table tbl_JOIN_INNER_TD_3705_consolidated_output("tbl_JOIN_INNER_TD_3705_consolidated_output", 114347, 6, "");
    tbl_JOIN_INNER_TD_3705_consolidated_output.selectOverlayVersion(1);
    tbl_JOIN_INNER_TD_3705_consolidated_output.allocateHost();
    Table tbl_SerializeFromObject_TD_3662_input_stringRowIDSubstitute;
    tbl_SerializeFromObject_TD_3662_input_stringRowIDSubstitute = Table("nation", nation_n, 2, in_dir);
    tbl_SerializeFromObject_TD_3662_input_stringRowIDSubstitute.addCol("n_nationkey", 4);
    tbl_SerializeFromObject_TD_3662_input_stringRowIDSubstitute.addCol("n_name", 4, 1, 0);
    tbl_SerializeFromObject_TD_3662_input_stringRowIDSubstitute.allocateHost();
    tbl_SerializeFromObject_TD_3662_input_stringRowIDSubstitute.loadHost();
    Table tbl_SerializeFromObject_TD_3662_input;
    tbl_SerializeFromObject_TD_3662_input = Table("nation", nation_n, 2, in_dir);
    tbl_SerializeFromObject_TD_3662_input.addCol("n_nationkey", 4);
    tbl_SerializeFromObject_TD_3662_input.addCol("n_name", TPCH_READ_NATION_LEN+1);
    tbl_SerializeFromObject_TD_3662_input.allocateHost();
    tbl_SerializeFromObject_TD_3662_input.loadHost();
    Table tbl_JOIN_INNER_TD_4407_output("tbl_JOIN_INNER_TD_4407_output", 57111, 4, "");
    tbl_JOIN_INNER_TD_4407_output.selectOverlayVersion(1);
    tbl_JOIN_INNER_TD_4407_output.allocateHost();
    Table tbl_JOIN_INNER_TD_4407_consolidated_output("tbl_JOIN_INNER_TD_4407_consolidated_output", 57111, 5, "");
    tbl_JOIN_INNER_TD_4407_consolidated_output.selectOverlayVersion(1);
    tbl_JOIN_INNER_TD_4407_consolidated_output.allocateHost();
    Table tbl_Filter_TD_4349_output("tbl_Filter_TD_4349_output", 6100000, 3, "");
    tbl_Filter_TD_4349_output.selectOverlayVersion(1);
    tbl_Filter_TD_4349_output.allocateHost();
    Table tbl_SerializeFromObject_TD_5309_input_stringRowIDSubstitute;
    tbl_SerializeFromObject_TD_5309_input_stringRowIDSubstitute = Table("customer", customer_n, 4, in_dir);
    tbl_SerializeFromObject_TD_5309_input_stringRowIDSubstitute.addCol("c_custkey", 4);
    tbl_SerializeFromObject_TD_5309_input_stringRowIDSubstitute.addCol("c_name", 4, 1, 0);
    tbl_SerializeFromObject_TD_5309_input_stringRowIDSubstitute.addCol("c_nationkey", 4);
    tbl_SerializeFromObject_TD_5309_input_stringRowIDSubstitute.addCol("c_acctbal", 4);
    tbl_SerializeFromObject_TD_5309_input_stringRowIDSubstitute.allocateHost();
    tbl_SerializeFromObject_TD_5309_input_stringRowIDSubstitute.loadHost();
    Table tbl_SerializeFromObject_TD_5309_input;
    tbl_SerializeFromObject_TD_5309_input = Table("customer", customer_n, 4, in_dir);
    tbl_SerializeFromObject_TD_5309_input.addCol("c_custkey", 4);
    tbl_SerializeFromObject_TD_5309_input.addCol("c_name", TPCH_READ_C_NAME_LEN+1);
    tbl_SerializeFromObject_TD_5309_input.addCol("c_nationkey", 4);
    tbl_SerializeFromObject_TD_5309_input.addCol("c_acctbal", 4);
    tbl_SerializeFromObject_TD_5309_input.allocateHost();
    tbl_SerializeFromObject_TD_5309_input.loadHost();
    Table tbl_Filter_TD_5576_output("tbl_Filter_TD_5576_output", 6100000, 2, "");
    tbl_Filter_TD_5576_output.selectOverlayVersion(1);
    tbl_Filter_TD_5576_output.allocateHost();
    Table tbl_SerializeFromObject_TD_5277_input;
    tbl_SerializeFromObject_TD_5277_input = Table("lineitem", lineitem_n, 4, in_dir);
    tbl_SerializeFromObject_TD_5277_input.addCol("l_orderkey", 4);
    tbl_SerializeFromObject_TD_5277_input.addCol("l_extendedprice", 4);
    tbl_SerializeFromObject_TD_5277_input.addCol("l_discount", 4);
    tbl_SerializeFromObject_TD_5277_input.addCol("l_returnflag", 4);
    tbl_SerializeFromObject_TD_5277_input.allocateHost();
    tbl_SerializeFromObject_TD_5277_input.loadHost();
    Table tbl_SerializeFromObject_TD_6366_input;
    tbl_SerializeFromObject_TD_6366_input = Table("order", order_n, 3, in_dir);
    tbl_SerializeFromObject_TD_6366_input.addCol("o_orderkey", 4);
    tbl_SerializeFromObject_TD_6366_input.addCol("o_custkey", 4);
    tbl_SerializeFromObject_TD_6366_input.addCol("o_orderdate", 4);
    tbl_SerializeFromObject_TD_6366_input.allocateHost();
    tbl_SerializeFromObject_TD_6366_input.loadHost();
    // ********************** Allocate Device Buffer ******************** // 
    //tbl_JOIN_INNER_TD_2823_output.allocateDevBuffer(context_h, 32);
    //tbl_JOIN_INNER_TD_3705_output.allocateDevBuffer(context_h, 32);
    tbl_SerializeFromObject_TD_3662_input_stringRowIDSubstitute.allocateDevBuffer(context_h, 32);
    //tbl_JOIN_INNER_TD_4407_output.allocateDevBuffer(context_h, 32);
    //tbl_Filter_TD_4349_output.allocateDevBuffer(context_h, 32);
    tbl_SerializeFromObject_TD_5309_input_stringRowIDSubstitute.allocateDevBuffer(context_h, 32);
    //tbl_Filter_TD_5576_output.allocateDevBuffer(context_h, 32);
    // ****************************** Config **************************** // 
    
    xf::database::gqe::KernelCommand krn_cmd_build_848 = xf::database::gqe::KernelCommand();
    xf::database::gqe::KernelCommand krn_cmd_probe_848 = xf::database::gqe::KernelCommand();
    cfgCmd cfg_JOIN_INNER_TD_2823_cmds_build;
    cfg_JOIN_INNER_TD_2823_cmds_build.setup(1);
    cfg_JOIN_INNER_TD_2823_cmds_build.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_2823_gqe_join_build(krn_cmd_build_848);
    cfg_JOIN_INNER_TD_2823_cmds_build.cmd = krn_cmd_build_848.getConfigBits();
    get_meta_info_JOIN_INNER_TD_2823_gqe_join_build(cfg_JOIN_INNER_TD_2823_cmds_build.meta_in, tbl_SerializeFromObject_TD_3662_input_stringRowIDSubstitute.nrow, 3);
    get_meta_info_JOIN_INNER_TD_2823_gqe_join_build(cfg_JOIN_INNER_TD_2823_cmds_build.meta_out, tbl_JOIN_INNER_TD_2823_output.nrow, 4);
    // cfg_JOIN_INNER_TD_2823_cmds_build.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_2823_cmds_probe;
    cfg_JOIN_INNER_TD_2823_cmds_probe.setup(1);
    cfg_JOIN_INNER_TD_2823_cmds_probe.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_2823_gqe_join_probe(krn_cmd_probe_848);
    cfg_JOIN_INNER_TD_2823_cmds_probe.cmd = krn_cmd_probe_848.getConfigBits();
    get_meta_info_JOIN_INNER_TD_2823_gqe_join_probe(cfg_JOIN_INNER_TD_2823_cmds_probe.meta_in, tbl_JOIN_INNER_TD_3705_consolidated_output.nrow, 3);
    get_meta_info_JOIN_INNER_TD_2823_gqe_join_probe(cfg_JOIN_INNER_TD_2823_cmds_probe.meta_out, tbl_JOIN_INNER_TD_2823_output.nrow, 3);
    // cfg_JOIN_INNER_TD_2823_cmds_probe.allocateDevBuffer(context_h, 32);
    cfg_JOIN_INNER_TD_2823_cmds_build.input_key_columns = {0, -1, -1};
    cfg_JOIN_INNER_TD_2823_cmds_probe.input_key_columns = {2, -1, -1};
    cfg_JOIN_INNER_TD_2823_cmds_build.output_key_columns = {0, 1, 2, 3};
    cfg_JOIN_INNER_TD_2823_cmds_probe.output_key_columns = {0, 1, 2, 3};
    
    xf::database::gqe::KernelCommand krn_cmd_build_238 = xf::database::gqe::KernelCommand();
    xf::database::gqe::KernelCommand krn_cmd_probe_238 = xf::database::gqe::KernelCommand();
    cfgCmd cfg_JOIN_INNER_TD_3705_cmds_build;
    cfg_JOIN_INNER_TD_3705_cmds_build.setup(1);
    cfg_JOIN_INNER_TD_3705_cmds_build.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_3705_gqe_join_build(krn_cmd_build_238);
    cfg_JOIN_INNER_TD_3705_cmds_build.cmd = krn_cmd_build_238.getConfigBits();
    get_meta_info_JOIN_INNER_TD_3705_gqe_join_build(cfg_JOIN_INNER_TD_3705_cmds_build.meta_in, tbl_Filter_TD_4349_output.nrow, 3);
    get_meta_info_JOIN_INNER_TD_3705_gqe_join_build(cfg_JOIN_INNER_TD_3705_cmds_build.meta_out, tbl_JOIN_INNER_TD_3705_output.nrow, 4);
    // cfg_JOIN_INNER_TD_3705_cmds_build.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_3705_cmds_probe;
    cfg_JOIN_INNER_TD_3705_cmds_probe.setup(1);
    cfg_JOIN_INNER_TD_3705_cmds_probe.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_3705_gqe_join_probe(krn_cmd_probe_238);
    cfg_JOIN_INNER_TD_3705_cmds_probe.cmd = krn_cmd_probe_238.getConfigBits();
    get_meta_info_JOIN_INNER_TD_3705_gqe_join_probe(cfg_JOIN_INNER_TD_3705_cmds_probe.meta_in, tbl_JOIN_INNER_TD_4407_consolidated_output.nrow, 3);
    get_meta_info_JOIN_INNER_TD_3705_gqe_join_probe(cfg_JOIN_INNER_TD_3705_cmds_probe.meta_out, tbl_JOIN_INNER_TD_3705_output.nrow, 3);
    // cfg_JOIN_INNER_TD_3705_cmds_probe.allocateDevBuffer(context_h, 32);
    cfg_JOIN_INNER_TD_3705_cmds_build.input_key_columns = {0, -1, -1};
    cfg_JOIN_INNER_TD_3705_cmds_probe.input_key_columns = {4, -1, -1};
    cfg_JOIN_INNER_TD_3705_cmds_build.output_key_columns = {0, 1, 2, 3};
    cfg_JOIN_INNER_TD_3705_cmds_probe.output_key_columns = {0, 1, 2, 3};
    
    xf::database::gqe::KernelCommand krn_cmd_build_425 = xf::database::gqe::KernelCommand();
    xf::database::gqe::KernelCommand krn_cmd_probe_425 = xf::database::gqe::KernelCommand();
    cfgCmd cfg_JOIN_INNER_TD_4407_cmds_build;
    cfg_JOIN_INNER_TD_4407_cmds_build.setup(1);
    cfg_JOIN_INNER_TD_4407_cmds_build.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_4407_gqe_join_build(krn_cmd_build_425);
    cfg_JOIN_INNER_TD_4407_cmds_build.cmd = krn_cmd_build_425.getConfigBits();
    get_meta_info_JOIN_INNER_TD_4407_gqe_join_build(cfg_JOIN_INNER_TD_4407_cmds_build.meta_in, tbl_Filter_TD_5576_output.nrow, 3);
    get_meta_info_JOIN_INNER_TD_4407_gqe_join_build(cfg_JOIN_INNER_TD_4407_cmds_build.meta_out, tbl_JOIN_INNER_TD_4407_output.nrow, 4);
    // cfg_JOIN_INNER_TD_4407_cmds_build.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_4407_cmds_probe;
    cfg_JOIN_INNER_TD_4407_cmds_probe.setup(1);
    cfg_JOIN_INNER_TD_4407_cmds_probe.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_4407_gqe_join_probe(krn_cmd_probe_425);
    cfg_JOIN_INNER_TD_4407_cmds_probe.cmd = krn_cmd_probe_425.getConfigBits();
    get_meta_info_JOIN_INNER_TD_4407_gqe_join_probe(cfg_JOIN_INNER_TD_4407_cmds_probe.meta_in, tbl_SerializeFromObject_TD_5309_input_stringRowIDSubstitute.nrow, 3);
    get_meta_info_JOIN_INNER_TD_4407_gqe_join_probe(cfg_JOIN_INNER_TD_4407_cmds_probe.meta_out, tbl_JOIN_INNER_TD_4407_output.nrow, 3);
    // cfg_JOIN_INNER_TD_4407_cmds_probe.allocateDevBuffer(context_h, 32);
    cfg_JOIN_INNER_TD_4407_cmds_build.input_key_columns = {1, -1, -1};
    cfg_JOIN_INNER_TD_4407_cmds_probe.input_key_columns = {0, -1, -1};
    cfg_JOIN_INNER_TD_4407_cmds_build.output_key_columns = {0, 1, 2, 3};
    cfg_JOIN_INNER_TD_4407_cmds_probe.output_key_columns = {0, 1, 2, 3};
    // *************************** Kernel Setup ************************* // 
    bufferTmp buftmp_h(context_h, 1); 
    buftmp_h.initBuffer(q_h); 
    std::cout << std::endl; 
    krnlEngine krnl_JOIN_INNER_TD_2823_build;
    krnl_JOIN_INNER_TD_2823_build = krnlEngine(program_h, q_h, context_h, "gqeKernel", 0);
    krnl_JOIN_INNER_TD_2823_build.setup(tbl_SerializeFromObject_TD_3662_input_stringRowIDSubstitute, tbl_JOIN_INNER_TD_3705_consolidated_output, tbl_JOIN_INNER_TD_2823_output, cfg_JOIN_INNER_TD_2823_cmds_build, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_2823_probe;
    krnl_JOIN_INNER_TD_2823_probe = krnlEngine(program_h, q_h, context_h, "gqeKernel", 1);
    krnl_JOIN_INNER_TD_2823_probe.setup(tbl_JOIN_INNER_TD_3705_consolidated_output, tbl_SerializeFromObject_TD_3662_input_stringRowIDSubstitute, tbl_JOIN_INNER_TD_2823_output, cfg_JOIN_INNER_TD_2823_cmds_probe, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_3705_build;
    krnl_JOIN_INNER_TD_3705_build = krnlEngine(program_h, q_h, context_h, "gqeKernel", 0);
    krnl_JOIN_INNER_TD_3705_build.setup(tbl_Filter_TD_4349_output, tbl_JOIN_INNER_TD_4407_consolidated_output, tbl_JOIN_INNER_TD_3705_output, cfg_JOIN_INNER_TD_3705_cmds_build, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_3705_probe;
    krnl_JOIN_INNER_TD_3705_probe = krnlEngine(program_h, q_h, context_h, "gqeKernel", 1);
    krnl_JOIN_INNER_TD_3705_probe.setup(tbl_JOIN_INNER_TD_4407_consolidated_output, tbl_Filter_TD_4349_output, tbl_JOIN_INNER_TD_3705_output, cfg_JOIN_INNER_TD_3705_cmds_probe, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_4407_build;
    krnl_JOIN_INNER_TD_4407_build = krnlEngine(program_h, q_h, context_h, "gqeKernel", 0);
    krnl_JOIN_INNER_TD_4407_build.setup(tbl_Filter_TD_5576_output, tbl_SerializeFromObject_TD_5309_input_stringRowIDSubstitute, tbl_JOIN_INNER_TD_4407_output, cfg_JOIN_INNER_TD_4407_cmds_build, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_4407_probe;
    krnl_JOIN_INNER_TD_4407_probe = krnlEngine(program_h, q_h, context_h, "gqeKernel", 1);
    krnl_JOIN_INNER_TD_4407_probe.setup(tbl_SerializeFromObject_TD_5309_input_stringRowIDSubstitute, tbl_Filter_TD_5576_output, tbl_JOIN_INNER_TD_4407_output, cfg_JOIN_INNER_TD_4407_cmds_probe, buftmp_h);
    // ************************** Transfer Engine *********************** // 
    transEngine trans_JOIN_INNER_TD_2823;
    trans_JOIN_INNER_TD_2823.setq(q_h,1);
    trans_JOIN_INNER_TD_2823.add(&(cfg_JOIN_INNER_TD_2823_cmds_build));
    trans_JOIN_INNER_TD_2823.add(&(cfg_JOIN_INNER_TD_2823_cmds_probe));
    trans_JOIN_INNER_TD_2823.add(&(tbl_SerializeFromObject_TD_3662_input_stringRowIDSubstitute));
    transEngine trans_JOIN_INNER_TD_2823_out;
    trans_JOIN_INNER_TD_2823_out.setq(q_h,1);
    q_h.finish();
    transEngine trans_JOIN_INNER_TD_3705;
    trans_JOIN_INNER_TD_3705.setq(q_h,1);
    trans_JOIN_INNER_TD_3705.add(&(cfg_JOIN_INNER_TD_3705_cmds_build));
    trans_JOIN_INNER_TD_3705.add(&(cfg_JOIN_INNER_TD_3705_cmds_probe));
    transEngine trans_JOIN_INNER_TD_3705_out;
    trans_JOIN_INNER_TD_3705_out.setq(q_h,1);
    q_h.finish();
    transEngine trans_JOIN_INNER_TD_4407;
    trans_JOIN_INNER_TD_4407.setq(q_h,1);
    trans_JOIN_INNER_TD_4407.add(&(cfg_JOIN_INNER_TD_4407_cmds_build));
    trans_JOIN_INNER_TD_4407.add(&(cfg_JOIN_INNER_TD_4407_cmds_probe));
    trans_JOIN_INNER_TD_4407.add(&(tbl_SerializeFromObject_TD_5309_input_stringRowIDSubstitute));
    transEngine trans_JOIN_INNER_TD_4407_out;
    trans_JOIN_INNER_TD_4407_out.setq(q_h,1);
    q_h.finish();
    // ****************************** Events **************************** // 
    std::vector<cl::Event> events_h2d_wr_JOIN_INNER_TD_2823;
    std::vector<cl::Event> events_d2h_rd_JOIN_INNER_TD_2823;
    std::vector<cl::Event> events_JOIN_INNER_TD_2823;
    events_h2d_wr_JOIN_INNER_TD_2823.resize(1);
    events_d2h_rd_JOIN_INNER_TD_2823.resize(1);
    events_JOIN_INNER_TD_2823.resize(2);
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_2823;
    std::vector<cl::Event> prev_events_grp_JOIN_INNER_TD_2823;
    std::vector<cl::Event> events_h2d_wr_JOIN_INNER_TD_3705;
    std::vector<cl::Event> events_d2h_rd_JOIN_INNER_TD_3705;
    std::vector<cl::Event> events_JOIN_INNER_TD_3705;
    events_h2d_wr_JOIN_INNER_TD_3705.resize(1);
    events_d2h_rd_JOIN_INNER_TD_3705.resize(1);
    events_JOIN_INNER_TD_3705.resize(2);
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_3705;
    std::vector<cl::Event> prev_events_grp_JOIN_INNER_TD_3705;
    std::vector<cl::Event> events_h2d_wr_JOIN_INNER_TD_4407;
    std::vector<cl::Event> events_d2h_rd_JOIN_INNER_TD_4407;
    std::vector<cl::Event> events_JOIN_INNER_TD_4407;
    events_h2d_wr_JOIN_INNER_TD_4407.resize(1);
    events_d2h_rd_JOIN_INNER_TD_4407.resize(1);
    events_JOIN_INNER_TD_4407.resize(2);
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_4407;
    std::vector<cl::Event> prev_events_grp_JOIN_INNER_TD_4407;
    // **************************** Operations ************************** // 
    struct timeval tv_r_s, tv_r_e; 
    gettimeofday(&tv_r_s, 0); 

    struct timeval tv_r_Filter_5_626_s, tv_r_Filter_5_626_e;
    gettimeofday(&tv_r_Filter_5_626_s, 0);
    SW_Filter_TD_5576(tbl_SerializeFromObject_TD_6366_input, tbl_Filter_TD_5576_output);
    gettimeofday(&tv_r_Filter_5_626_e, 0);

    struct timeval tv_r_Filter_4_329_s, tv_r_Filter_4_329_e;
    gettimeofday(&tv_r_Filter_4_329_s, 0);
    SW_Filter_TD_4349(tbl_SerializeFromObject_TD_5277_input, tbl_Filter_TD_4349_output);
    gettimeofday(&tv_r_Filter_4_329_e, 0);

    struct timeval tv_r_JOIN_INNER_4_121_s, tv_r_JOIN_INNER_4_121_e;
    gettimeofday(&tv_r_JOIN_INNER_4_121_s, 0);
    tbl_Filter_TD_5576_output.tableToCol();
    tbl_SerializeFromObject_TD_5309_input_stringRowIDSubstitute.tableToCol();
    krnl_JOIN_INNER_TD_4407_build.updateMeta(tbl_Filter_TD_5576_output.getNumRow(), 3);
    krnl_JOIN_INNER_TD_4407_probe.updateMeta(tbl_SerializeFromObject_TD_5309_input_stringRowIDSubstitute.getNumRow(), 3);
    trans_JOIN_INNER_TD_4407.add(&(tbl_Filter_TD_5576_output));
    trans_JOIN_INNER_TD_4407.host2dev(0, &(prev_events_grp_JOIN_INNER_TD_4407), &(events_h2d_wr_JOIN_INNER_TD_4407[0]));
    events_grp_JOIN_INNER_TD_4407.push_back(events_h2d_wr_JOIN_INNER_TD_4407[0]);
    krnl_JOIN_INNER_TD_4407_build.run(0, &(events_grp_JOIN_INNER_TD_4407), &(events_JOIN_INNER_TD_4407[0]));
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_4407_build_done;
    events_grp_JOIN_INNER_TD_4407_build_done.push_back(events_JOIN_INNER_TD_4407[0]);
    krnl_JOIN_INNER_TD_4407_probe.run(0, &(events_grp_JOIN_INNER_TD_4407_build_done), &(events_JOIN_INNER_TD_4407[1]));
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_4407_probe_done;
    events_grp_JOIN_INNER_TD_4407_probe_done.push_back(events_JOIN_INNER_TD_4407[1]);
    trans_JOIN_INNER_TD_4407_out.add(&(tbl_JOIN_INNER_TD_4407_output));
    trans_JOIN_INNER_TD_4407_out.add(&(cfg_JOIN_INNER_TD_4407_cmds_build));
    trans_JOIN_INNER_TD_4407_out.add(&(cfg_JOIN_INNER_TD_4407_cmds_probe));
    trans_JOIN_INNER_TD_4407_out.dev2host(0, &(events_grp_JOIN_INNER_TD_4407_probe_done), &(events_d2h_rd_JOIN_INNER_TD_4407[0]));
    q_h.flush();
    q_h.finish();
    tbl_JOIN_INNER_TD_4407_output.setNumRow((cfg_JOIN_INNER_TD_4407_cmds_probe.meta_out)->getColLen());
    tbl_JOIN_INNER_TD_4407_output.colToTable();
    SW_Consolidated_JOIN_INNER_TD_4407_output(tbl_Filter_TD_5576_output, tbl_SerializeFromObject_TD_5309_input_stringRowIDSubstitute, tbl_JOIN_INNER_TD_4407_output, tbl_JOIN_INNER_TD_4407_consolidated_output);
    gettimeofday(&tv_r_JOIN_INNER_4_121_e, 0);

    struct timeval tv_r_JOIN_INNER_3_670_s, tv_r_JOIN_INNER_3_670_e;
    gettimeofday(&tv_r_JOIN_INNER_3_670_s, 0);
    prev_events_grp_JOIN_INNER_TD_3705.push_back(events_h2d_wr_JOIN_INNER_TD_4407[0]);
    tbl_Filter_TD_4349_output.tableToCol();
    tbl_JOIN_INNER_TD_4407_consolidated_output.tableToCol();
    krnl_JOIN_INNER_TD_3705_build.updateMeta(tbl_Filter_TD_4349_output.getNumRow(), 3);
    krnl_JOIN_INNER_TD_3705_probe.updateMeta(tbl_JOIN_INNER_TD_4407_consolidated_output.getNumRow(), 3);
    trans_JOIN_INNER_TD_3705.add(&(tbl_Filter_TD_4349_output));
    trans_JOIN_INNER_TD_3705.host2dev(0, &(prev_events_grp_JOIN_INNER_TD_3705), &(events_h2d_wr_JOIN_INNER_TD_3705[0]));
    events_grp_JOIN_INNER_TD_3705.push_back(events_h2d_wr_JOIN_INNER_TD_3705[0]);
    events_grp_JOIN_INNER_TD_3705.push_back(events_JOIN_INNER_TD_4407[0]);
    krnl_JOIN_INNER_TD_3705_build.run(0, &(events_grp_JOIN_INNER_TD_3705), &(events_JOIN_INNER_TD_3705[0]));
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_3705_build_done;
    events_grp_JOIN_INNER_TD_3705_build_done.push_back(events_JOIN_INNER_TD_3705[0]);
    krnl_JOIN_INNER_TD_3705_probe.run(0, &(events_grp_JOIN_INNER_TD_3705_build_done), &(events_JOIN_INNER_TD_3705[1]));
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_3705_probe_done;
    events_grp_JOIN_INNER_TD_3705_probe_done.push_back(events_JOIN_INNER_TD_3705[1]);
    trans_JOIN_INNER_TD_3705_out.add(&(tbl_JOIN_INNER_TD_3705_output));
    trans_JOIN_INNER_TD_3705_out.add(&(cfg_JOIN_INNER_TD_3705_cmds_build));
    trans_JOIN_INNER_TD_3705_out.add(&(cfg_JOIN_INNER_TD_3705_cmds_probe));
    trans_JOIN_INNER_TD_3705_out.dev2host(0, &(events_grp_JOIN_INNER_TD_3705_probe_done), &(events_d2h_rd_JOIN_INNER_TD_3705[0]));
    q_h.flush();
    q_h.finish();
    tbl_JOIN_INNER_TD_3705_output.setNumRow((cfg_JOIN_INNER_TD_3705_cmds_probe.meta_out)->getColLen());
    tbl_JOIN_INNER_TD_3705_output.colToTable();
    SW_Consolidated_JOIN_INNER_TD_3705_output(tbl_Filter_TD_4349_output, tbl_JOIN_INNER_TD_4407_consolidated_output, tbl_JOIN_INNER_TD_3705_output, tbl_JOIN_INNER_TD_3705_consolidated_output);
    gettimeofday(&tv_r_JOIN_INNER_3_670_e, 0);

    struct timeval tv_r_JOIN_INNER_2_393_s, tv_r_JOIN_INNER_2_393_e;
    gettimeofday(&tv_r_JOIN_INNER_2_393_s, 0);
    prev_events_grp_JOIN_INNER_TD_2823.push_back(events_h2d_wr_JOIN_INNER_TD_3705[0]);
    tbl_SerializeFromObject_TD_3662_input_stringRowIDSubstitute.tableToCol();
    tbl_JOIN_INNER_TD_3705_consolidated_output.tableToCol();
    krnl_JOIN_INNER_TD_2823_build.updateMeta(tbl_SerializeFromObject_TD_3662_input_stringRowIDSubstitute.getNumRow(), 3);
    krnl_JOIN_INNER_TD_2823_probe.updateMeta(tbl_JOIN_INNER_TD_3705_consolidated_output.getNumRow(), 3);
    trans_JOIN_INNER_TD_2823.host2dev(0, &(prev_events_grp_JOIN_INNER_TD_2823), &(events_h2d_wr_JOIN_INNER_TD_2823[0]));
    events_grp_JOIN_INNER_TD_2823.push_back(events_h2d_wr_JOIN_INNER_TD_2823[0]);
    events_grp_JOIN_INNER_TD_2823.push_back(events_JOIN_INNER_TD_3705[0]);
    krnl_JOIN_INNER_TD_2823_build.run(0, &(events_grp_JOIN_INNER_TD_2823), &(events_JOIN_INNER_TD_2823[0]));
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_2823_build_done;
    events_grp_JOIN_INNER_TD_2823_build_done.push_back(events_JOIN_INNER_TD_2823[0]);
    krnl_JOIN_INNER_TD_2823_probe.run(0, &(events_grp_JOIN_INNER_TD_2823_build_done), &(events_JOIN_INNER_TD_2823[1]));
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_2823_probe_done;
    events_grp_JOIN_INNER_TD_2823_probe_done.push_back(events_JOIN_INNER_TD_2823[1]);
    trans_JOIN_INNER_TD_2823_out.add(&(tbl_JOIN_INNER_TD_2823_output));
    trans_JOIN_INNER_TD_2823_out.add(&(cfg_JOIN_INNER_TD_2823_cmds_build));
    trans_JOIN_INNER_TD_2823_out.add(&(cfg_JOIN_INNER_TD_2823_cmds_probe));
    trans_JOIN_INNER_TD_2823_out.dev2host(0, &(events_grp_JOIN_INNER_TD_2823_probe_done), &(events_d2h_rd_JOIN_INNER_TD_2823[0]));
    q_h.flush();
    q_h.finish();
    tbl_JOIN_INNER_TD_2823_output.setNumRow((cfg_JOIN_INNER_TD_2823_cmds_probe.meta_out)->getColLen());
    tbl_JOIN_INNER_TD_2823_output.colToTable();
    SW_Consolidated_JOIN_INNER_TD_2823_output(tbl_SerializeFromObject_TD_3662_input_stringRowIDSubstitute, tbl_JOIN_INNER_TD_3705_consolidated_output, tbl_JOIN_INNER_TD_2823_output, tbl_JOIN_INNER_TD_2823_consolidated_output);
    gettimeofday(&tv_r_JOIN_INNER_2_393_e, 0);

    struct timeval tv_r_Aggregate_1_788_s, tv_r_Aggregate_1_788_e;
    gettimeofday(&tv_r_Aggregate_1_788_s, 0);
    SW_Aggregate_TD_155(tbl_JOIN_INNER_TD_2823_consolidated_output, tbl_SerializeFromObject_TD_5309_input, tbl_SerializeFromObject_TD_6366_input, tbl_SerializeFromObject_TD_5277_input, tbl_SerializeFromObject_TD_3662_input, tbl_Aggregate_TD_155_output);
    gettimeofday(&tv_r_Aggregate_1_788_e, 0);

    struct timeval tv_r_Sort_0_198_s, tv_r_Sort_0_198_e;
    gettimeofday(&tv_r_Sort_0_198_s, 0);
    SW_Sort_TD_0999(tbl_Aggregate_TD_155_output, tbl_Sort_TD_0999_output);
    gettimeofday(&tv_r_Sort_0_198_e, 0);

std::cout << "CPU TIME: " << (tvdiff(&tv_r_Filter_5_626_s, &tv_r_Filter_5_626_e) + 
tvdiff(&tv_r_Filter_4_329_s, &tv_r_Filter_4_329_e) + 
tvdiff(&tv_r_Aggregate_1_788_s, &tv_r_Aggregate_1_788_e) + 
tvdiff(&tv_r_Sort_0_198_s, &tv_r_Sort_0_198_e) + 
1) / 1000.0 << std::endl;

std::cout << "FPGA TIME: " << (tvdiff(&tv_r_JOIN_INNER_4_121_s, &tv_r_JOIN_INNER_4_121_e) + 
tvdiff(&tv_r_JOIN_INNER_3_670_s, &tv_r_JOIN_INNER_3_670_e) + 
tvdiff(&tv_r_JOIN_INNER_2_393_s, &tv_r_JOIN_INNER_2_393_e) + 
1) / 1000.0 << std::endl;

    gettimeofday(&tv_r_e, 0); 
    // **************************** Print Execution Time ************************** // 
    std::cout << std::endl; 
    std::cout << "Filter_5: " << tvdiff(&tv_r_Filter_5_626_s, &tv_r_Filter_5_626_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(((o_orderdate#211 >= 19940801) AND (o_orderdate#211 < 19941101))) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_6366_input.getNumRow() << " -> tbl_SerializeFromObject_TD_6366_input" 
     << std::endl << "      #COL: 3: " << "ListBuffer(o_orderkey#207, o_custkey#208, o_orderdate#211)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_5576_output.getNumRow() << " -> tbl_Filter_TD_5576_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(o_orderkey#207, o_custkey#208)" 
     << std::endl; 

    std::cout << "Filter_4: " << tvdiff(&tv_r_Filter_4_329_s, &tv_r_Filter_4_329_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer((l_returnflag#85 = 82)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_5277_input.getNumRow() << " -> tbl_SerializeFromObject_TD_5277_input" 
     << std::endl << "      #COL: 4: " << "ListBuffer(l_orderkey#77, l_extendedprice#82, l_discount#83, l_returnflag#85)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_4349_output.getNumRow() << " -> tbl_Filter_TD_4349_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(l_orderkey#77, l_extendedprice#82, l_discount#83)" 
     << std::endl; 

    std::cout << "JOIN_INNER_4: " << tvdiff(&tv_r_JOIN_INNER_4_121_s, &tv_r_JOIN_INNER_4_121_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer((c_custkey#22 = o_custkey#208)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_5309_input_stringRowIDSubstitute.getNumRow() << " -> tbl_SerializeFromObject_TD_5309_input_stringRowIDSubstitute" 
     << std::endl << "      #COL: 4: " << "ListBuffer(c_custkey#22, c_name#23, c_nationkey#25, c_acctbal#27)" 
     << std::endl << "      #ROW: " << tbl_Filter_TD_5576_output.getNumRow() << " -> tbl_Filter_TD_5576_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(o_orderkey#207, o_custkey#208)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_4407_consolidated_output.getNumRow() << " -> tbl_JOIN_INNER_TD_4407_consolidated_output" 
     << std::endl << "      #COL: 5: " << "ListBuffer(c_custkey#22, c_name#23, c_nationkey#25, c_acctbal#27, o_orderkey#207)" 
     << std::endl; 

    std::cout << "JOIN_INNER_3: " << tvdiff(&tv_r_JOIN_INNER_3_670_s, &tv_r_JOIN_INNER_3_670_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer((l_orderkey#77 = o_orderkey#207)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_4407_consolidated_output.getNumRow() << " -> tbl_JOIN_INNER_TD_4407_consolidated_output" 
     << std::endl << "      #COL: 5: " << "ListBuffer(c_custkey#22, c_name#23, c_nationkey#25, c_acctbal#27, o_orderkey#207)" 
     << std::endl << "      #ROW: " << tbl_Filter_TD_4349_output.getNumRow() << " -> tbl_Filter_TD_4349_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(l_orderkey#77, l_extendedprice#82, l_discount#83)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_3705_consolidated_output.getNumRow() << " -> tbl_JOIN_INNER_TD_3705_consolidated_output" 
     << std::endl << "      #COL: 6: " << "ListBuffer(c_custkey#22, c_name#23, c_nationkey#25, c_acctbal#27, l_extendedprice#82, l_discount#83)" 
     << std::endl; 

    std::cout << "JOIN_INNER_2: " << tvdiff(&tv_r_JOIN_INNER_2_393_s, &tv_r_JOIN_INNER_2_393_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer((c_nationkey#25 = n_nationkey#144)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_3705_consolidated_output.getNumRow() << " -> tbl_JOIN_INNER_TD_3705_consolidated_output" 
     << std::endl << "      #COL: 6: " << "ListBuffer(c_custkey#22, c_name#23, c_nationkey#25, c_acctbal#27, l_extendedprice#82, l_discount#83)" 
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_3662_input_stringRowIDSubstitute.getNumRow() << " -> tbl_SerializeFromObject_TD_3662_input_stringRowIDSubstitute" 
     << std::endl << "      #COL: 2: " << "ListBuffer(n_nationkey#144, n_name#145)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_2823_consolidated_output.getNumRow() << " -> tbl_JOIN_INNER_TD_2823_consolidated_output" 
     << std::endl << "      #COL: 6: " << "ListBuffer(c_custkey#22, c_name#23, c_acctbal#27, l_extendedprice#82, l_discount#83, n_name#145)" 
     << std::endl; 

    std::cout << "Aggregate_1: " << tvdiff(&tv_r_Aggregate_1_788_s, &tv_r_Aggregate_1_788_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(c_custkey#22, c_name#23, c_acctbal#27, n_name#145, sum((l_extendedprice#82 * (100 - l_discount#83))) AS revenue#1130L) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_2823_consolidated_output.getNumRow() << " -> tbl_JOIN_INNER_TD_2823_consolidated_output" 
     << std::endl << "      #COL: 6: " << "ListBuffer(c_custkey#22, c_name#23, c_acctbal#27, l_extendedprice#82, l_discount#83, n_name#145)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_155_output.getNumRow() << " -> tbl_Aggregate_TD_155_output" 
     << std::endl << "      #COL: 5: " << "ListBuffer(c_custkey#22, c_name#23, revenue#1130L, c_acctbal#27, n_name#145)" 
     << std::endl; 

    std::cout << "Sort_0: " << tvdiff(&tv_r_Sort_0_198_s, &tv_r_Sort_0_198_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(revenue#1130L DESC NULLS LAST) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_155_output.getNumRow() << " -> tbl_Aggregate_TD_155_output" 
     << std::endl << "      #COL: 5: " << "ListBuffer(c_custkey#22, c_name#23, revenue#1130L, c_acctbal#27, n_name#145)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Sort_TD_0999_output.getNumRow() << " -> tbl_Sort_TD_0999_output" 
     << std::endl << "      #COL: 5: " << "ListBuffer(c_custkey#22, c_name#23, revenue#1130L, c_acctbal#27, n_name#145)" 
     << std::endl; 

    std::cout << std::endl << " Total execution time: " << tvdiff(&tv_r_s, &tv_r_e) / 1000 << " ms"; 

    std::cout << std::endl << " Spark elapsed time: " << 21.614927 * 1000 << "ms" << std::endl; 
    return 0; 
}
