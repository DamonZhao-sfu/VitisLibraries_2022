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

#include "cfgFunc_q7.hpp" 
#include "q7.hpp" 

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
    std::cout << "NOTE:running query #7\n."; 
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
    Table tbl_Sort_TD_0451_output("tbl_Sort_TD_0451_output", 6100000, 4, "");
    tbl_Sort_TD_0451_output.allocateHost();
    Table tbl_Aggregate_TD_1904_output("tbl_Aggregate_TD_1904_output", 6100000, 4, "");
    tbl_Aggregate_TD_1904_output.allocateHost();
    Table tbl_Project_TD_2215_output("tbl_Project_TD_2215_output", 6100000, 4, "");
    tbl_Project_TD_2215_output.allocateHost();
    Table tbl_JOIN_INNER_TD_3655_output("tbl_JOIN_INNER_TD_3655_output", 6100000, 6, "");
    tbl_JOIN_INNER_TD_3655_output.allocateHost();
    Table tbl_JOIN_INNER_TD_4287_output("tbl_JOIN_INNER_TD_4287_output", 153500, 4, "");
    tbl_JOIN_INNER_TD_4287_output.selectOverlayVersion(1);
    tbl_JOIN_INNER_TD_4287_output.allocateHost();
    Table tbl_JOIN_INNER_TD_4287_consolidated_output("tbl_JOIN_INNER_TD_4287_consolidated_output", 153500, 5, "");
    tbl_JOIN_INNER_TD_4287_consolidated_output.selectOverlayVersion(1);
    tbl_JOIN_INNER_TD_4287_consolidated_output.allocateHost();
    Table tbl_Filter_TD_4468_output("tbl_Filter_TD_4468_output", 6100000, 2, "");
    tbl_Filter_TD_4468_output.allocateHost();
    Table tbl_JOIN_INNER_TD_5272_output("tbl_JOIN_INNER_TD_5272_output", 153500, 4, "");
    tbl_JOIN_INNER_TD_5272_output.selectOverlayVersion(1);
    tbl_JOIN_INNER_TD_5272_output.allocateHost();
    Table tbl_JOIN_INNER_TD_5272_consolidated_output("tbl_JOIN_INNER_TD_5272_consolidated_output", 153500, 6, "");
    tbl_JOIN_INNER_TD_5272_consolidated_output.selectOverlayVersion(1);
    tbl_JOIN_INNER_TD_5272_consolidated_output.allocateHost();
    Table tbl_SerializeFromObject_TD_5489_input;
    tbl_SerializeFromObject_TD_5489_input = Table("customer", customer_n, 2, in_dir);
    tbl_SerializeFromObject_TD_5489_input.selectOverlayVersion(1);
    tbl_SerializeFromObject_TD_5489_input.addCol("c_custkey", 4);
    tbl_SerializeFromObject_TD_5489_input.addCol("c_nationkey", 4);
    tbl_SerializeFromObject_TD_5489_input.allocateHost();
    tbl_SerializeFromObject_TD_5489_input.loadHost();
    Table tbl_SerializeFromObject_TD_5307_input;
    tbl_SerializeFromObject_TD_5307_input = Table("nation", nation_n, 2, in_dir);
    tbl_SerializeFromObject_TD_5307_input.addCol("n_nationkey", 4);
    tbl_SerializeFromObject_TD_5307_input.addCol("n_name", TPCH_READ_NATION_LEN+1);
    tbl_SerializeFromObject_TD_5307_input.allocateHost();
    tbl_SerializeFromObject_TD_5307_input.loadHost();
    Table tbl_JOIN_INNER_TD_6649_output("tbl_JOIN_INNER_TD_6649_output", 153500, 4, "");
    tbl_JOIN_INNER_TD_6649_output.selectOverlayVersion(1);
    tbl_JOIN_INNER_TD_6649_output.allocateHost();
    Table tbl_JOIN_INNER_TD_6649_consolidated_output("tbl_JOIN_INNER_TD_6649_consolidated_output", 153500, 6, "");
    tbl_JOIN_INNER_TD_6649_consolidated_output.selectOverlayVersion(1);
    tbl_JOIN_INNER_TD_6649_consolidated_output.allocateHost();
    Table tbl_SerializeFromObject_TD_6245_input;
    tbl_SerializeFromObject_TD_6245_input = Table("order", order_n, 2, in_dir);
    tbl_SerializeFromObject_TD_6245_input.selectOverlayVersion(1);
    tbl_SerializeFromObject_TD_6245_input.addCol("o_orderkey", 4);
    tbl_SerializeFromObject_TD_6245_input.addCol("o_custkey", 4);
    tbl_SerializeFromObject_TD_6245_input.allocateHost();
    tbl_SerializeFromObject_TD_6245_input.loadHost();
    Table tbl_JOIN_INNER_TD_7285_output("tbl_JOIN_INNER_TD_7285_output", 1829000, 4, "");
    tbl_JOIN_INNER_TD_7285_output.selectOverlayVersion(1);
    tbl_JOIN_INNER_TD_7285_output.allocateHost();
    Table tbl_JOIN_INNER_TD_7285_consolidated_output("tbl_JOIN_INNER_TD_7285_consolidated_output", 1829000, 5, "");
    tbl_JOIN_INNER_TD_7285_consolidated_output.selectOverlayVersion(1);
    tbl_JOIN_INNER_TD_7285_consolidated_output.allocateHost();
    Table tbl_Filter_TD_5344_output("tbl_Filter_TD_5344_output", 6100000, 2, "");
    tbl_Filter_TD_5344_output.selectOverlayVersion(1);
    tbl_Filter_TD_5344_output.allocateHost();
    Table tbl_SerializeFromObject_TD_8859_input;
    tbl_SerializeFromObject_TD_8859_input = Table("supplier", supplier_n, 2, in_dir);
    tbl_SerializeFromObject_TD_8859_input.selectOverlayVersion(1);
    tbl_SerializeFromObject_TD_8859_input.addCol("s_suppkey", 4);
    tbl_SerializeFromObject_TD_8859_input.addCol("s_nationkey", 4);
    tbl_SerializeFromObject_TD_8859_input.allocateHost();
    tbl_SerializeFromObject_TD_8859_input.loadHost();
    Table tbl_Filter_TD_870_output("tbl_Filter_TD_870_output", 6100000, 5, "");
    tbl_Filter_TD_870_output.selectOverlayVersion(1);
    tbl_Filter_TD_870_output.allocateHost();
    Table tbl_SerializeFromObject_TD_6527_input;
    tbl_SerializeFromObject_TD_6527_input = Table("nation", nation_n, 2, in_dir);
    tbl_SerializeFromObject_TD_6527_input.addCol("n_nationkey", 4);
    tbl_SerializeFromObject_TD_6527_input.addCol("n_name", TPCH_READ_NATION_LEN+1);
    tbl_SerializeFromObject_TD_6527_input.allocateHost();
    tbl_SerializeFromObject_TD_6527_input.loadHost();
    Table tbl_SerializeFromObject_TD_96_input;
    tbl_SerializeFromObject_TD_96_input = Table("lineitem", lineitem_n, 5, in_dir);
    tbl_SerializeFromObject_TD_96_input.addCol("l_orderkey", 4);
    tbl_SerializeFromObject_TD_96_input.addCol("l_suppkey", 4);
    tbl_SerializeFromObject_TD_96_input.addCol("l_extendedprice", 4);
    tbl_SerializeFromObject_TD_96_input.addCol("l_discount", 4);
    tbl_SerializeFromObject_TD_96_input.addCol("l_shipdate", 4);
    tbl_SerializeFromObject_TD_96_input.allocateHost();
    tbl_SerializeFromObject_TD_96_input.loadHost();
    // ********************** Allocate Device Buffer ******************** // 
    //tbl_JOIN_INNER_TD_4287_output.allocateDevBuffer(context_h, 32);
    //tbl_JOIN_INNER_TD_5272_output.allocateDevBuffer(context_h, 32);
    // tbl_SerializeFromObject_TD_5489_input.allocateDevBuffer(context_h, 32);
    //tbl_JOIN_INNER_TD_6649_output.allocateDevBuffer(context_h, 32);
    // tbl_SerializeFromObject_TD_6245_input.allocateDevBuffer(context_h, 32);
    //tbl_JOIN_INNER_TD_7285_output.allocateDevBuffer(context_h, 32);
    //tbl_Filter_TD_5344_output.allocateDevBuffer(context_h, 32);
    // tbl_SerializeFromObject_TD_8859_input.allocateDevBuffer(context_h, 32);
    //tbl_Filter_TD_870_output.allocateDevBuffer(context_h, 32);
    // ****************************** Config **************************** // 
    
    xf::database::gqe::KernelCommand krn_cmd_build_551 = xf::database::gqe::KernelCommand();
    xf::database::gqe::KernelCommand krn_cmd_probe_551 = xf::database::gqe::KernelCommand();
    cfgCmd cfg_JOIN_INNER_TD_4287_cmds_build;
    cfg_JOIN_INNER_TD_4287_cmds_build.setup(1);
    cfg_JOIN_INNER_TD_4287_cmds_build.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_4287_gqe_join_build(krn_cmd_build_551);
    cfg_JOIN_INNER_TD_4287_cmds_build.cmd = krn_cmd_build_551.getConfigBits();
    get_meta_info_JOIN_INNER_TD_4287_gqe_join_build(cfg_JOIN_INNER_TD_4287_cmds_build.meta_in, tbl_SerializeFromObject_TD_5489_input.nrow, 3);
    get_meta_info_JOIN_INNER_TD_4287_gqe_join_build(cfg_JOIN_INNER_TD_4287_cmds_build.meta_out, tbl_JOIN_INNER_TD_4287_output.nrow, 4);
    // cfg_JOIN_INNER_TD_4287_cmds_build.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_4287_cmds_probe;
    cfg_JOIN_INNER_TD_4287_cmds_probe.setup(1);
    cfg_JOIN_INNER_TD_4287_cmds_probe.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_4287_gqe_join_probe(krn_cmd_probe_551);
    cfg_JOIN_INNER_TD_4287_cmds_probe.cmd = krn_cmd_probe_551.getConfigBits();
    get_meta_info_JOIN_INNER_TD_4287_gqe_join_probe(cfg_JOIN_INNER_TD_4287_cmds_probe.meta_in, tbl_JOIN_INNER_TD_5272_consolidated_output.nrow, 3);
    get_meta_info_JOIN_INNER_TD_4287_gqe_join_probe(cfg_JOIN_INNER_TD_4287_cmds_probe.meta_out, tbl_JOIN_INNER_TD_4287_output.nrow, 3);
    // cfg_JOIN_INNER_TD_4287_cmds_probe.allocateDevBuffer(context_h, 32);
    cfg_JOIN_INNER_TD_4287_cmds_build.input_key_columns = {0, -1, -1};
    cfg_JOIN_INNER_TD_4287_cmds_probe.input_key_columns = {4, -1, -1};
    cfg_JOIN_INNER_TD_4287_cmds_build.output_key_columns = {0, 1, 2, 3};
    cfg_JOIN_INNER_TD_4287_cmds_probe.output_key_columns = {0, 1, 2, 3};
    
    xf::database::gqe::KernelCommand krn_cmd_build_675 = xf::database::gqe::KernelCommand();
    xf::database::gqe::KernelCommand krn_cmd_probe_675 = xf::database::gqe::KernelCommand();
    cfgCmd cfg_JOIN_INNER_TD_5272_cmds_build;
    cfg_JOIN_INNER_TD_5272_cmds_build.setup(1);
    cfg_JOIN_INNER_TD_5272_cmds_build.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_5272_gqe_join_build(krn_cmd_build_675);
    cfg_JOIN_INNER_TD_5272_cmds_build.cmd = krn_cmd_build_675.getConfigBits();
    get_meta_info_JOIN_INNER_TD_5272_gqe_join_build(cfg_JOIN_INNER_TD_5272_cmds_build.meta_in, tbl_JOIN_INNER_TD_6649_consolidated_output.nrow, 3);
    get_meta_info_JOIN_INNER_TD_5272_gqe_join_build(cfg_JOIN_INNER_TD_5272_cmds_build.meta_out, tbl_JOIN_INNER_TD_5272_output.nrow, 4);
    // cfg_JOIN_INNER_TD_5272_cmds_build.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_5272_cmds_probe;
    cfg_JOIN_INNER_TD_5272_cmds_probe.setup(1);
    cfg_JOIN_INNER_TD_5272_cmds_probe.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_5272_gqe_join_probe(krn_cmd_probe_675);
    cfg_JOIN_INNER_TD_5272_cmds_probe.cmd = krn_cmd_probe_675.getConfigBits();
    get_meta_info_JOIN_INNER_TD_5272_gqe_join_probe(cfg_JOIN_INNER_TD_5272_cmds_probe.meta_in, tbl_SerializeFromObject_TD_6245_input.nrow, 3);
    get_meta_info_JOIN_INNER_TD_5272_gqe_join_probe(cfg_JOIN_INNER_TD_5272_cmds_probe.meta_out, tbl_JOIN_INNER_TD_5272_output.nrow, 3);
    // cfg_JOIN_INNER_TD_5272_cmds_probe.allocateDevBuffer(context_h, 32);
    cfg_JOIN_INNER_TD_5272_cmds_build.input_key_columns = {5, -1, -1};
    cfg_JOIN_INNER_TD_5272_cmds_probe.input_key_columns = {0, -1, -1};
    cfg_JOIN_INNER_TD_5272_cmds_build.output_key_columns = {0, 1, 2, 3};
    cfg_JOIN_INNER_TD_5272_cmds_probe.output_key_columns = {0, 1, 2, 3};
    
    xf::database::gqe::KernelCommand krn_cmd_build_639 = xf::database::gqe::KernelCommand();
    xf::database::gqe::KernelCommand krn_cmd_probe_639 = xf::database::gqe::KernelCommand();
    cfgCmd cfg_JOIN_INNER_TD_6649_cmds_build;
    cfg_JOIN_INNER_TD_6649_cmds_build.setup(1);
    cfg_JOIN_INNER_TD_6649_cmds_build.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_6649_gqe_join_build(krn_cmd_build_639);
    cfg_JOIN_INNER_TD_6649_cmds_build.cmd = krn_cmd_build_639.getConfigBits();
    get_meta_info_JOIN_INNER_TD_6649_gqe_join_build(cfg_JOIN_INNER_TD_6649_cmds_build.meta_in, tbl_Filter_TD_5344_output.nrow, 3);
    get_meta_info_JOIN_INNER_TD_6649_gqe_join_build(cfg_JOIN_INNER_TD_6649_cmds_build.meta_out, tbl_JOIN_INNER_TD_6649_output.nrow, 4);
    // cfg_JOIN_INNER_TD_6649_cmds_build.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_6649_cmds_probe;
    cfg_JOIN_INNER_TD_6649_cmds_probe.setup(1);
    cfg_JOIN_INNER_TD_6649_cmds_probe.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_6649_gqe_join_probe(krn_cmd_probe_639);
    cfg_JOIN_INNER_TD_6649_cmds_probe.cmd = krn_cmd_probe_639.getConfigBits();
    get_meta_info_JOIN_INNER_TD_6649_gqe_join_probe(cfg_JOIN_INNER_TD_6649_cmds_probe.meta_in, tbl_JOIN_INNER_TD_7285_consolidated_output.nrow, 3);
    get_meta_info_JOIN_INNER_TD_6649_gqe_join_probe(cfg_JOIN_INNER_TD_6649_cmds_probe.meta_out, tbl_JOIN_INNER_TD_6649_output.nrow, 3);
    // cfg_JOIN_INNER_TD_6649_cmds_probe.allocateDevBuffer(context_h, 32);
    cfg_JOIN_INNER_TD_6649_cmds_build.input_key_columns = {0, -1, -1};
    cfg_JOIN_INNER_TD_6649_cmds_probe.input_key_columns = {0, -1, -1};
    cfg_JOIN_INNER_TD_6649_cmds_build.output_key_columns = {0, 1, 2, 3};
    cfg_JOIN_INNER_TD_6649_cmds_probe.output_key_columns = {0, 1, 2, 3};
    
    xf::database::gqe::KernelCommand krn_cmd_build_418 = xf::database::gqe::KernelCommand();
    xf::database::gqe::KernelCommand krn_cmd_probe_418 = xf::database::gqe::KernelCommand();
    cfgCmd cfg_JOIN_INNER_TD_7285_cmds_build;
    cfg_JOIN_INNER_TD_7285_cmds_build.setup(1);
    cfg_JOIN_INNER_TD_7285_cmds_build.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_7285_gqe_join_build(krn_cmd_build_418);
    cfg_JOIN_INNER_TD_7285_cmds_build.cmd = krn_cmd_build_418.getConfigBits();
    get_meta_info_JOIN_INNER_TD_7285_gqe_join_build(cfg_JOIN_INNER_TD_7285_cmds_build.meta_in, tbl_SerializeFromObject_TD_8859_input.nrow, 3);
    get_meta_info_JOIN_INNER_TD_7285_gqe_join_build(cfg_JOIN_INNER_TD_7285_cmds_build.meta_out, tbl_JOIN_INNER_TD_7285_output.nrow, 4);
    // cfg_JOIN_INNER_TD_7285_cmds_build.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_7285_cmds_probe;
    cfg_JOIN_INNER_TD_7285_cmds_probe.setup(1);
    cfg_JOIN_INNER_TD_7285_cmds_probe.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_7285_gqe_join_probe(krn_cmd_probe_418);
    cfg_JOIN_INNER_TD_7285_cmds_probe.cmd = krn_cmd_probe_418.getConfigBits();
    get_meta_info_JOIN_INNER_TD_7285_gqe_join_probe(cfg_JOIN_INNER_TD_7285_cmds_probe.meta_in, tbl_Filter_TD_870_output.nrow, 3);
    get_meta_info_JOIN_INNER_TD_7285_gqe_join_probe(cfg_JOIN_INNER_TD_7285_cmds_probe.meta_out, tbl_JOIN_INNER_TD_7285_output.nrow, 3);
    // cfg_JOIN_INNER_TD_7285_cmds_probe.allocateDevBuffer(context_h, 32);
    cfg_JOIN_INNER_TD_7285_cmds_build.input_key_columns = {0, -1, -1};
    cfg_JOIN_INNER_TD_7285_cmds_probe.input_key_columns = {1, -1, -1};
    cfg_JOIN_INNER_TD_7285_cmds_build.output_key_columns = {0, 1, 2, 3};
    cfg_JOIN_INNER_TD_7285_cmds_probe.output_key_columns = {0, 1, 2, 3};
    // *************************** Kernel Setup ************************* // 
    bufferTmp buftmp_h(context_h, 1); 
    buftmp_h.initBuffer(q_h); 
    std::cout << std::endl; 
    krnlEngine krnl_JOIN_INNER_TD_4287_build;
    krnl_JOIN_INNER_TD_4287_build = krnlEngine(program_h, q_h, context_h, "gqeKernel", 0);
    krnl_JOIN_INNER_TD_4287_build.setup(tbl_SerializeFromObject_TD_5489_input, tbl_JOIN_INNER_TD_5272_consolidated_output, tbl_JOIN_INNER_TD_4287_output, cfg_JOIN_INNER_TD_4287_cmds_build, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_4287_probe;
    krnl_JOIN_INNER_TD_4287_probe = krnlEngine(program_h, q_h, context_h, "gqeKernel", 1);
    krnl_JOIN_INNER_TD_4287_probe.setup(tbl_JOIN_INNER_TD_5272_consolidated_output, tbl_SerializeFromObject_TD_5489_input, tbl_JOIN_INNER_TD_4287_output, cfg_JOIN_INNER_TD_4287_cmds_probe, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_5272_build;
    krnl_JOIN_INNER_TD_5272_build = krnlEngine(program_h, q_h, context_h, "gqeKernel", 0);
    krnl_JOIN_INNER_TD_5272_build.setup(tbl_JOIN_INNER_TD_6649_consolidated_output, tbl_SerializeFromObject_TD_6245_input, tbl_JOIN_INNER_TD_5272_output, cfg_JOIN_INNER_TD_5272_cmds_build, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_5272_probe;
    krnl_JOIN_INNER_TD_5272_probe = krnlEngine(program_h, q_h, context_h, "gqeKernel", 1);
    krnl_JOIN_INNER_TD_5272_probe.setup(tbl_SerializeFromObject_TD_6245_input, tbl_JOIN_INNER_TD_6649_consolidated_output, tbl_JOIN_INNER_TD_5272_output, cfg_JOIN_INNER_TD_5272_cmds_probe, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_6649_build;
    krnl_JOIN_INNER_TD_6649_build = krnlEngine(program_h, q_h, context_h, "gqeKernel", 0);
    krnl_JOIN_INNER_TD_6649_build.setup(tbl_Filter_TD_5344_output, tbl_JOIN_INNER_TD_7285_consolidated_output, tbl_JOIN_INNER_TD_6649_output, cfg_JOIN_INNER_TD_6649_cmds_build, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_6649_probe;
    krnl_JOIN_INNER_TD_6649_probe = krnlEngine(program_h, q_h, context_h, "gqeKernel", 1);
    krnl_JOIN_INNER_TD_6649_probe.setup(tbl_JOIN_INNER_TD_7285_consolidated_output, tbl_Filter_TD_5344_output, tbl_JOIN_INNER_TD_6649_output, cfg_JOIN_INNER_TD_6649_cmds_probe, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_7285_build;
    krnl_JOIN_INNER_TD_7285_build = krnlEngine(program_h, q_h, context_h, "gqeKernel", 0);
    krnl_JOIN_INNER_TD_7285_build.setup(tbl_SerializeFromObject_TD_8859_input, tbl_Filter_TD_870_output, tbl_JOIN_INNER_TD_7285_output, cfg_JOIN_INNER_TD_7285_cmds_build, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_7285_probe;
    krnl_JOIN_INNER_TD_7285_probe = krnlEngine(program_h, q_h, context_h, "gqeKernel", 1);
    krnl_JOIN_INNER_TD_7285_probe.setup(tbl_Filter_TD_870_output, tbl_SerializeFromObject_TD_8859_input, tbl_JOIN_INNER_TD_7285_output, cfg_JOIN_INNER_TD_7285_cmds_probe, buftmp_h);
    // ************************** Transfer Engine *********************** // 
    transEngine trans_JOIN_INNER_TD_4287;
    trans_JOIN_INNER_TD_4287.setq(q_h,1);
    trans_JOIN_INNER_TD_4287.add(&(cfg_JOIN_INNER_TD_4287_cmds_build));
    trans_JOIN_INNER_TD_4287.add(&(cfg_JOIN_INNER_TD_4287_cmds_probe));
    trans_JOIN_INNER_TD_4287.add(&(tbl_SerializeFromObject_TD_5489_input));
    transEngine trans_JOIN_INNER_TD_4287_out;
    trans_JOIN_INNER_TD_4287_out.setq(q_h,1);
    q_h.finish();
    transEngine trans_JOIN_INNER_TD_5272;
    trans_JOIN_INNER_TD_5272.setq(q_h,1);
    trans_JOIN_INNER_TD_5272.add(&(cfg_JOIN_INNER_TD_5272_cmds_build));
    trans_JOIN_INNER_TD_5272.add(&(cfg_JOIN_INNER_TD_5272_cmds_probe));
    trans_JOIN_INNER_TD_5272.add(&(tbl_SerializeFromObject_TD_6245_input));
    transEngine trans_JOIN_INNER_TD_5272_out;
    trans_JOIN_INNER_TD_5272_out.setq(q_h,1);
    q_h.finish();
    transEngine trans_JOIN_INNER_TD_6649;
    trans_JOIN_INNER_TD_6649.setq(q_h,1);
    trans_JOIN_INNER_TD_6649.add(&(cfg_JOIN_INNER_TD_6649_cmds_build));
    trans_JOIN_INNER_TD_6649.add(&(cfg_JOIN_INNER_TD_6649_cmds_probe));
    transEngine trans_JOIN_INNER_TD_6649_out;
    trans_JOIN_INNER_TD_6649_out.setq(q_h,1);
    q_h.finish();
    transEngine trans_JOIN_INNER_TD_7285;
    trans_JOIN_INNER_TD_7285.setq(q_h,1);
    trans_JOIN_INNER_TD_7285.add(&(cfg_JOIN_INNER_TD_7285_cmds_build));
    trans_JOIN_INNER_TD_7285.add(&(cfg_JOIN_INNER_TD_7285_cmds_probe));
    trans_JOIN_INNER_TD_7285.add(&(tbl_SerializeFromObject_TD_8859_input));
    transEngine trans_JOIN_INNER_TD_7285_out;
    trans_JOIN_INNER_TD_7285_out.setq(q_h,1);
    q_h.finish();
    // ****************************** Events **************************** // 
    std::vector<cl::Event> events_h2d_wr_JOIN_INNER_TD_4287;
    std::vector<cl::Event> events_d2h_rd_JOIN_INNER_TD_4287;
    std::vector<cl::Event> events_JOIN_INNER_TD_4287;
    events_h2d_wr_JOIN_INNER_TD_4287.resize(1);
    events_d2h_rd_JOIN_INNER_TD_4287.resize(1);
    events_JOIN_INNER_TD_4287.resize(2);
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_4287;
    std::vector<cl::Event> prev_events_grp_JOIN_INNER_TD_4287;
    std::vector<cl::Event> events_h2d_wr_JOIN_INNER_TD_5272;
    std::vector<cl::Event> events_d2h_rd_JOIN_INNER_TD_5272;
    std::vector<cl::Event> events_JOIN_INNER_TD_5272;
    events_h2d_wr_JOIN_INNER_TD_5272.resize(1);
    events_d2h_rd_JOIN_INNER_TD_5272.resize(1);
    events_JOIN_INNER_TD_5272.resize(2);
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_5272;
    std::vector<cl::Event> prev_events_grp_JOIN_INNER_TD_5272;
    std::vector<cl::Event> events_h2d_wr_JOIN_INNER_TD_6649;
    std::vector<cl::Event> events_d2h_rd_JOIN_INNER_TD_6649;
    std::vector<cl::Event> events_JOIN_INNER_TD_6649;
    events_h2d_wr_JOIN_INNER_TD_6649.resize(1);
    events_d2h_rd_JOIN_INNER_TD_6649.resize(1);
    events_JOIN_INNER_TD_6649.resize(2);
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_6649;
    std::vector<cl::Event> prev_events_grp_JOIN_INNER_TD_6649;
    std::vector<cl::Event> events_h2d_wr_JOIN_INNER_TD_7285;
    std::vector<cl::Event> events_d2h_rd_JOIN_INNER_TD_7285;
    std::vector<cl::Event> events_JOIN_INNER_TD_7285;
    events_h2d_wr_JOIN_INNER_TD_7285.resize(1);
    events_d2h_rd_JOIN_INNER_TD_7285.resize(1);
    events_JOIN_INNER_TD_7285.resize(2);
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_7285;
    std::vector<cl::Event> prev_events_grp_JOIN_INNER_TD_7285;
    // **************************** Operations ************************** // 
    struct timeval tv_r_s, tv_r_e; 
    gettimeofday(&tv_r_s, 0); 

    struct timeval tv_r_Filter_8_23_s, tv_r_Filter_8_23_e;
    gettimeofday(&tv_r_Filter_8_23_s, 0);
    SW_Filter_TD_870(tbl_SerializeFromObject_TD_96_input, tbl_Filter_TD_870_output);
    gettimeofday(&tv_r_Filter_8_23_e, 0);

    struct timeval tv_r_Filter_5_234_s, tv_r_Filter_5_234_e;
    gettimeofday(&tv_r_Filter_5_234_s, 0);
    SW_Filter_TD_5344(tbl_SerializeFromObject_TD_6527_input, tbl_Filter_TD_5344_output);
    gettimeofday(&tv_r_Filter_5_234_e, 0);

    struct timeval tv_r_JOIN_INNER_7_430_s, tv_r_JOIN_INNER_7_430_e;
    gettimeofday(&tv_r_JOIN_INNER_7_430_s, 0);
    tbl_SerializeFromObject_TD_8859_input.tableToCol();
    tbl_Filter_TD_870_output.tableToCol();
    krnl_JOIN_INNER_TD_7285_build.updateMeta(tbl_SerializeFromObject_TD_8859_input.getNumRow(), 3);
    krnl_JOIN_INNER_TD_7285_probe.updateMeta(tbl_Filter_TD_870_output.getNumRow(), 3);
    trans_JOIN_INNER_TD_7285.add(&(tbl_Filter_TD_870_output));
    trans_JOIN_INNER_TD_7285.host2dev(0, &(prev_events_grp_JOIN_INNER_TD_7285), &(events_h2d_wr_JOIN_INNER_TD_7285[0]));
    events_grp_JOIN_INNER_TD_7285.push_back(events_h2d_wr_JOIN_INNER_TD_7285[0]);
    krnl_JOIN_INNER_TD_7285_build.run(0, &(events_grp_JOIN_INNER_TD_7285), &(events_JOIN_INNER_TD_7285[0]));
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_7285_build_done;
    events_grp_JOIN_INNER_TD_7285_build_done.push_back(events_JOIN_INNER_TD_7285[0]);
    krnl_JOIN_INNER_TD_7285_probe.run(0, &(events_grp_JOIN_INNER_TD_7285_build_done), &(events_JOIN_INNER_TD_7285[1]));
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_7285_probe_done;
    events_grp_JOIN_INNER_TD_7285_probe_done.push_back(events_JOIN_INNER_TD_7285[1]);
    trans_JOIN_INNER_TD_7285_out.add(&(tbl_JOIN_INNER_TD_7285_output));
    trans_JOIN_INNER_TD_7285_out.add(&(cfg_JOIN_INNER_TD_7285_cmds_build));
    trans_JOIN_INNER_TD_7285_out.add(&(cfg_JOIN_INNER_TD_7285_cmds_probe));
    trans_JOIN_INNER_TD_7285_out.dev2host(0, &(events_grp_JOIN_INNER_TD_7285_probe_done), &(events_d2h_rd_JOIN_INNER_TD_7285[0]));
    q_h.flush();
    q_h.finish();
    tbl_JOIN_INNER_TD_7285_output.setNumRow((cfg_JOIN_INNER_TD_7285_cmds_probe.meta_out)->getColLen());
    tbl_JOIN_INNER_TD_7285_output.colToTable();
    SW_Consolidated_JOIN_INNER_TD_7285_output(tbl_SerializeFromObject_TD_8859_input, tbl_Filter_TD_870_output, tbl_JOIN_INNER_TD_7285_output, tbl_JOIN_INNER_TD_7285_consolidated_output);
    gettimeofday(&tv_r_JOIN_INNER_7_430_e, 0);

    struct timeval tv_r_JOIN_INNER_6_118_s, tv_r_JOIN_INNER_6_118_e;
    gettimeofday(&tv_r_JOIN_INNER_6_118_s, 0);
    prev_events_grp_JOIN_INNER_TD_6649.push_back(events_h2d_wr_JOIN_INNER_TD_7285[0]);
    tbl_Filter_TD_5344_output.tableToCol();
    tbl_JOIN_INNER_TD_7285_consolidated_output.tableToCol();
    krnl_JOIN_INNER_TD_6649_build.updateMeta(tbl_Filter_TD_5344_output.getNumRow(), 3);
    krnl_JOIN_INNER_TD_6649_probe.updateMeta(tbl_JOIN_INNER_TD_7285_consolidated_output.getNumRow(), 3);
    trans_JOIN_INNER_TD_6649.add(&(tbl_Filter_TD_5344_output));
    trans_JOIN_INNER_TD_6649.host2dev(0, &(prev_events_grp_JOIN_INNER_TD_6649), &(events_h2d_wr_JOIN_INNER_TD_6649[0]));
    events_grp_JOIN_INNER_TD_6649.push_back(events_h2d_wr_JOIN_INNER_TD_6649[0]);
    events_grp_JOIN_INNER_TD_6649.push_back(events_JOIN_INNER_TD_7285[0]);
    krnl_JOIN_INNER_TD_6649_build.run(0, &(events_grp_JOIN_INNER_TD_6649), &(events_JOIN_INNER_TD_6649[0]));
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_6649_build_done;
    events_grp_JOIN_INNER_TD_6649_build_done.push_back(events_JOIN_INNER_TD_6649[0]);
    krnl_JOIN_INNER_TD_6649_probe.run(0, &(events_grp_JOIN_INNER_TD_6649_build_done), &(events_JOIN_INNER_TD_6649[1]));
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_6649_probe_done;
    events_grp_JOIN_INNER_TD_6649_probe_done.push_back(events_JOIN_INNER_TD_6649[1]);
    trans_JOIN_INNER_TD_6649_out.add(&(tbl_JOIN_INNER_TD_6649_output));
    trans_JOIN_INNER_TD_6649_out.add(&(cfg_JOIN_INNER_TD_6649_cmds_build));
    trans_JOIN_INNER_TD_6649_out.add(&(cfg_JOIN_INNER_TD_6649_cmds_probe));
    trans_JOIN_INNER_TD_6649_out.dev2host(0, &(events_grp_JOIN_INNER_TD_6649_probe_done), &(events_d2h_rd_JOIN_INNER_TD_6649[0]));
    q_h.flush();
    q_h.finish();
    tbl_JOIN_INNER_TD_6649_output.setNumRow((cfg_JOIN_INNER_TD_6649_cmds_probe.meta_out)->getColLen());
    tbl_JOIN_INNER_TD_6649_output.colToTable();
    SW_Consolidated_JOIN_INNER_TD_6649_output(tbl_Filter_TD_5344_output, tbl_JOIN_INNER_TD_7285_consolidated_output, tbl_JOIN_INNER_TD_6649_output, tbl_JOIN_INNER_TD_6649_consolidated_output);
    gettimeofday(&tv_r_JOIN_INNER_6_118_e, 0);

    struct timeval tv_r_JOIN_INNER_5_352_s, tv_r_JOIN_INNER_5_352_e;
    gettimeofday(&tv_r_JOIN_INNER_5_352_s, 0);
    prev_events_grp_JOIN_INNER_TD_5272.push_back(events_h2d_wr_JOIN_INNER_TD_6649[0]);
    tbl_JOIN_INNER_TD_6649_consolidated_output.tableToCol();
    tbl_SerializeFromObject_TD_6245_input.tableToCol();
    krnl_JOIN_INNER_TD_5272_build.updateMeta(tbl_JOIN_INNER_TD_6649_consolidated_output.getNumRow(), 3);
    krnl_JOIN_INNER_TD_5272_probe.updateMeta(tbl_SerializeFromObject_TD_6245_input.getNumRow(), 3);
    trans_JOIN_INNER_TD_5272.host2dev(0, &(prev_events_grp_JOIN_INNER_TD_5272), &(events_h2d_wr_JOIN_INNER_TD_5272[0]));
    events_grp_JOIN_INNER_TD_5272.push_back(events_h2d_wr_JOIN_INNER_TD_5272[0]);
    events_grp_JOIN_INNER_TD_5272.push_back(events_JOIN_INNER_TD_6649[0]);
    krnl_JOIN_INNER_TD_5272_build.run(0, &(events_grp_JOIN_INNER_TD_5272), &(events_JOIN_INNER_TD_5272[0]));
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_5272_build_done;
    events_grp_JOIN_INNER_TD_5272_build_done.push_back(events_JOIN_INNER_TD_5272[0]);
    krnl_JOIN_INNER_TD_5272_probe.run(0, &(events_grp_JOIN_INNER_TD_5272_build_done), &(events_JOIN_INNER_TD_5272[1]));
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_5272_probe_done;
    events_grp_JOIN_INNER_TD_5272_probe_done.push_back(events_JOIN_INNER_TD_5272[1]);
    trans_JOIN_INNER_TD_5272_out.add(&(tbl_JOIN_INNER_TD_5272_output));
    trans_JOIN_INNER_TD_5272_out.add(&(cfg_JOIN_INNER_TD_5272_cmds_build));
    trans_JOIN_INNER_TD_5272_out.add(&(cfg_JOIN_INNER_TD_5272_cmds_probe));
    trans_JOIN_INNER_TD_5272_out.dev2host(0, &(events_grp_JOIN_INNER_TD_5272_probe_done), &(events_d2h_rd_JOIN_INNER_TD_5272[0]));
    q_h.flush();
    q_h.finish();
    tbl_JOIN_INNER_TD_5272_output.setNumRow((cfg_JOIN_INNER_TD_5272_cmds_probe.meta_out)->getColLen());
    tbl_JOIN_INNER_TD_5272_output.colToTable();
    SW_Consolidated_JOIN_INNER_TD_5272_output(tbl_JOIN_INNER_TD_6649_consolidated_output, tbl_SerializeFromObject_TD_6245_input, tbl_JOIN_INNER_TD_5272_output, tbl_JOIN_INNER_TD_5272_consolidated_output);
    gettimeofday(&tv_r_JOIN_INNER_5_352_e, 0);

    struct timeval tv_r_Filter_4_706_s, tv_r_Filter_4_706_e;
    gettimeofday(&tv_r_Filter_4_706_s, 0);
    SW_Filter_TD_4468(tbl_SerializeFromObject_TD_5307_input, tbl_Filter_TD_4468_output);
    gettimeofday(&tv_r_Filter_4_706_e, 0);

    struct timeval tv_r_JOIN_INNER_4_422_s, tv_r_JOIN_INNER_4_422_e;
    gettimeofday(&tv_r_JOIN_INNER_4_422_s, 0);
    prev_events_grp_JOIN_INNER_TD_4287.push_back(events_h2d_wr_JOIN_INNER_TD_5272[0]);
    tbl_SerializeFromObject_TD_5489_input.tableToCol();
    tbl_JOIN_INNER_TD_5272_consolidated_output.tableToCol();
    krnl_JOIN_INNER_TD_4287_build.updateMeta(tbl_SerializeFromObject_TD_5489_input.getNumRow(), 3);
    krnl_JOIN_INNER_TD_4287_probe.updateMeta(tbl_JOIN_INNER_TD_5272_consolidated_output.getNumRow(), 3);
    trans_JOIN_INNER_TD_4287.host2dev(0, &(prev_events_grp_JOIN_INNER_TD_4287), &(events_h2d_wr_JOIN_INNER_TD_4287[0]));
    events_grp_JOIN_INNER_TD_4287.push_back(events_h2d_wr_JOIN_INNER_TD_4287[0]);
    events_grp_JOIN_INNER_TD_4287.push_back(events_JOIN_INNER_TD_5272[0]);
    krnl_JOIN_INNER_TD_4287_build.run(0, &(events_grp_JOIN_INNER_TD_4287), &(events_JOIN_INNER_TD_4287[0]));
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_4287_build_done;
    events_grp_JOIN_INNER_TD_4287_build_done.push_back(events_JOIN_INNER_TD_4287[0]);
    krnl_JOIN_INNER_TD_4287_probe.run(0, &(events_grp_JOIN_INNER_TD_4287_build_done), &(events_JOIN_INNER_TD_4287[1]));
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_4287_probe_done;
    events_grp_JOIN_INNER_TD_4287_probe_done.push_back(events_JOIN_INNER_TD_4287[1]);
    trans_JOIN_INNER_TD_4287_out.add(&(tbl_JOIN_INNER_TD_4287_output));
    trans_JOIN_INNER_TD_4287_out.add(&(cfg_JOIN_INNER_TD_4287_cmds_build));
    trans_JOIN_INNER_TD_4287_out.add(&(cfg_JOIN_INNER_TD_4287_cmds_probe));
    trans_JOIN_INNER_TD_4287_out.dev2host(0, &(events_grp_JOIN_INNER_TD_4287_probe_done), &(events_d2h_rd_JOIN_INNER_TD_4287[0]));
    q_h.flush();
    q_h.finish();
    tbl_JOIN_INNER_TD_4287_output.setNumRow((cfg_JOIN_INNER_TD_4287_cmds_probe.meta_out)->getColLen());
    tbl_JOIN_INNER_TD_4287_output.colToTable();
    SW_Consolidated_JOIN_INNER_TD_4287_output(tbl_SerializeFromObject_TD_5489_input, tbl_JOIN_INNER_TD_5272_consolidated_output, tbl_JOIN_INNER_TD_4287_output, tbl_JOIN_INNER_TD_4287_consolidated_output);
    gettimeofday(&tv_r_JOIN_INNER_4_422_e, 0);

    struct timeval tv_r_JOIN_INNER_3_579_s, tv_r_JOIN_INNER_3_579_e;
    gettimeofday(&tv_r_JOIN_INNER_3_579_s, 0);
    SW_JOIN_INNER_TD_3655(tbl_JOIN_INNER_TD_4287_consolidated_output, tbl_Filter_TD_4468_output, tbl_SerializeFromObject_TD_6527_input, tbl_JOIN_INNER_TD_3655_output);
    gettimeofday(&tv_r_JOIN_INNER_3_579_e, 0);

    struct timeval tv_r_Project_2_330_s, tv_r_Project_2_330_e;
    gettimeofday(&tv_r_Project_2_330_s, 0);
    SW_Project_TD_2215(tbl_JOIN_INNER_TD_3655_output, tbl_Project_TD_2215_output);
    gettimeofday(&tv_r_Project_2_330_e, 0);

    struct timeval tv_r_Aggregate_1_146_s, tv_r_Aggregate_1_146_e;
    gettimeofday(&tv_r_Aggregate_1_146_s, 0);
    SW_Aggregate_TD_1904(tbl_Project_TD_2215_output, tbl_Aggregate_TD_1904_output);
    gettimeofday(&tv_r_Aggregate_1_146_e, 0);

    struct timeval tv_r_Sort_0_147_s, tv_r_Sort_0_147_e;
    gettimeofday(&tv_r_Sort_0_147_s, 0);
    SW_Sort_TD_0451(tbl_Aggregate_TD_1904_output, tbl_Sort_TD_0451_output);
    gettimeofday(&tv_r_Sort_0_147_e, 0);

std::cout << "CPU TIME: " << (tvdiff(&tv_r_Filter_8_23_s, &tv_r_Filter_8_23_e) + 
tvdiff(&tv_r_Filter_5_234_s, &tv_r_Filter_5_234_e) + 
tvdiff(&tv_r_Filter_4_706_s, &tv_r_Filter_4_706_e) + 
tvdiff(&tv_r_JOIN_INNER_3_579_s, &tv_r_JOIN_INNER_3_579_e) + 
tvdiff(&tv_r_Project_2_330_s, &tv_r_Project_2_330_e) + 
tvdiff(&tv_r_Aggregate_1_146_s, &tv_r_Aggregate_1_146_e) + 
tvdiff(&tv_r_Sort_0_147_s, &tv_r_Sort_0_147_e) + 
1) / 1000.0 << std::endl;

std::cout << "FPGA TIME: " << (tvdiff(&tv_r_JOIN_INNER_7_430_s, &tv_r_JOIN_INNER_7_430_e) + 
tvdiff(&tv_r_JOIN_INNER_6_118_s, &tv_r_JOIN_INNER_6_118_e) + 
tvdiff(&tv_r_JOIN_INNER_5_352_s, &tv_r_JOIN_INNER_5_352_e) + 
tvdiff(&tv_r_JOIN_INNER_4_422_s, &tv_r_JOIN_INNER_4_422_e) + 
1) / 1000.0 << std::endl;

    gettimeofday(&tv_r_e, 0); 
    // **************************** Print Execution Time ************************** // 
    std::cout << std::endl; 
    std::cout << "Filter_8: " << tvdiff(&tv_r_Filter_8_23_s, &tv_r_Filter_8_23_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(((l_shipdate#87 >= 19950101) AND (l_shipdate#87 <= 19961231))) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_96_input.getNumRow() << " -> tbl_SerializeFromObject_TD_96_input" 
     << std::endl << "      #COL: 5: " << "ListBuffer(l_orderkey#77, l_suppkey#79, l_extendedprice#82, l_discount#83, l_shipdate#87)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_870_output.getNumRow() << " -> tbl_Filter_TD_870_output" 
     << std::endl << "      #COL: 5: " << "ListBuffer(l_orderkey#77, l_suppkey#79, l_extendedprice#82, l_discount#83, l_shipdate#87)" 
     << std::endl; 

    std::cout << "Filter_5: " << tvdiff(&tv_r_Filter_5_234_s, &tv_r_Filter_5_234_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(((n_name#145 = FRANCE) OR (n_name#145 = IRAQ))) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_6527_input.getNumRow() << " -> tbl_SerializeFromObject_TD_6527_input" 
     << std::endl << "      #COL: 2: " << "ListBuffer(n_nationkey#144, n_name#145)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_5344_output.getNumRow() << " -> tbl_Filter_TD_5344_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(n_nationkey#144, n_name#145)" 
     << std::endl; 

    std::cout << "JOIN_INNER_7: " << tvdiff(&tv_r_JOIN_INNER_7_430_s, &tv_r_JOIN_INNER_7_430_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer((s_suppkey#342 = l_suppkey#79)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_8859_input.getNumRow() << " -> tbl_SerializeFromObject_TD_8859_input" 
     << std::endl << "      #COL: 2: " << "ListBuffer(s_suppkey#342, s_nationkey#345)" 
     << std::endl << "      #ROW: " << tbl_Filter_TD_870_output.getNumRow() << " -> tbl_Filter_TD_870_output" 
     << std::endl << "      #COL: 5: " << "ListBuffer(l_orderkey#77, l_suppkey#79, l_extendedprice#82, l_discount#83, l_shipdate#87)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_7285_consolidated_output.getNumRow() << " -> tbl_JOIN_INNER_TD_7285_consolidated_output" 
     << std::endl << "      #COL: 5: " << "ListBuffer(s_nationkey#345, l_orderkey#77, l_extendedprice#82, l_discount#83, l_shipdate#87)" 
     << std::endl; 

    std::cout << "JOIN_INNER_6: " << tvdiff(&tv_r_JOIN_INNER_6_118_s, &tv_r_JOIN_INNER_6_118_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer((s_nationkey#345 = n_nationkey#144)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_7285_consolidated_output.getNumRow() << " -> tbl_JOIN_INNER_TD_7285_consolidated_output" 
     << std::endl << "      #COL: 5: " << "ListBuffer(s_nationkey#345, l_orderkey#77, l_extendedprice#82, l_discount#83, l_shipdate#87)" 
     << std::endl << "      #ROW: " << tbl_Filter_TD_5344_output.getNumRow() << " -> tbl_Filter_TD_5344_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(n_nationkey#144, n_name#145)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_6649_consolidated_output.getNumRow() << " -> tbl_JOIN_INNER_TD_6649_consolidated_output" 
     << std::endl << "      #COL: 6: " << "ListBuffer(l_extendedprice#82, l_discount#83, l_shipdate#87, n_name#145, s_nationkey#345, l_orderkey#77)" 
     << std::endl; 

    std::cout << "JOIN_INNER_5: " << tvdiff(&tv_r_JOIN_INNER_5_352_s, &tv_r_JOIN_INNER_5_352_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer((o_orderkey#207 = l_orderkey#77)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_6649_consolidated_output.getNumRow() << " -> tbl_JOIN_INNER_TD_6649_consolidated_output" 
     << std::endl << "      #COL: 6: " << "ListBuffer(l_extendedprice#82, l_discount#83, l_shipdate#87, n_name#145, s_nationkey#345, l_orderkey#77)" 
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_6245_input.getNumRow() << " -> tbl_SerializeFromObject_TD_6245_input" 
     << std::endl << "      #COL: 2: " << "ListBuffer(o_orderkey#207, o_custkey#208)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_5272_consolidated_output.getNumRow() << " -> tbl_JOIN_INNER_TD_5272_consolidated_output" 
     << std::endl << "      #COL: 6: " << "ListBuffer(s_nationkey#345, l_extendedprice#82, l_discount#83, l_shipdate#87, o_custkey#208, n_name#145)" 
     << std::endl; 

    std::cout << "Filter_4: " << tvdiff(&tv_r_Filter_4_706_s, &tv_r_Filter_4_706_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(((n_name#871 = IRAQ) OR (n_name#871 = FRANCE))) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_5307_input.getNumRow() << " -> tbl_SerializeFromObject_TD_5307_input" 
     << std::endl << "      #COL: 2: " << "ListBuffer(n_nationkey#870, n_name#871)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_4468_output.getNumRow() << " -> tbl_Filter_TD_4468_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(n_nationkey#870, n_name#871)" 
     << std::endl; 

    std::cout << "JOIN_INNER_4: " << tvdiff(&tv_r_JOIN_INNER_4_422_s, &tv_r_JOIN_INNER_4_422_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer((c_custkey#22 = o_custkey#208)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_5272_consolidated_output.getNumRow() << " -> tbl_JOIN_INNER_TD_5272_consolidated_output" 
     << std::endl << "      #COL: 6: " << "ListBuffer(s_nationkey#345, l_extendedprice#82, l_discount#83, l_shipdate#87, o_custkey#208, n_name#145)" 
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_5489_input.getNumRow() << " -> tbl_SerializeFromObject_TD_5489_input" 
     << std::endl << "      #COL: 2: " << "ListBuffer(c_custkey#22, c_nationkey#25)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_4287_consolidated_output.getNumRow() << " -> tbl_JOIN_INNER_TD_4287_consolidated_output" 
     << std::endl << "      #COL: 5: " << "ListBuffer(l_extendedprice#82, l_discount#83, l_shipdate#87, c_nationkey#25, n_name#145)" 
     << std::endl; 

    std::cout << "JOIN_INNER_3: " << tvdiff(&tv_r_JOIN_INNER_3_579_s, &tv_r_JOIN_INNER_3_579_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(((c_nationkey#25 = n_nationkey#870) AND (((n_name#145 = FRANCE) AND (n_name#871 = IRAQ)) OR ((n_name#145 = IRAQ) AND (n_name#871 = FRANCE))))) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_4287_consolidated_output.getNumRow() << " -> tbl_JOIN_INNER_TD_4287_consolidated_output" 
     << std::endl << "      #COL: 5: " << "ListBuffer(l_extendedprice#82, l_discount#83, l_shipdate#87, c_nationkey#25, n_name#145)" 
     << std::endl << "      #ROW: " << tbl_Filter_TD_4468_output.getNumRow() << " -> tbl_Filter_TD_4468_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(n_nationkey#870, n_name#871)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_3655_output.getNumRow() << " -> tbl_JOIN_INNER_TD_3655_output" 
     << std::endl << "      #COL: 6: " << "ListBuffer(n_name#145, n_name#871, l_shipdate#87, l_extendedprice#82, l_discount#83, c_nationkey#25)" 
     << std::endl; 

    std::cout << "Project_2: " << tvdiff(&tv_r_Project_2_330_s, &tv_r_Project_2_330_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(n_name#145 AS supp_nation#864, n_name#871 AS cust_nation#865, cast((cast(l_shipdate#87 as double) / 10000.0) as int) AS l_year#866, (l_extendedprice#82 * (100 - l_discount#83)) AS volume#867) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_3655_output.getNumRow() << " -> tbl_JOIN_INNER_TD_3655_output" 
     << std::endl << "      #COL: 6: " << "ListBuffer(n_name#145, n_name#871, l_shipdate#87, l_extendedprice#82, l_discount#83, c_nationkey#25)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Project_TD_2215_output.getNumRow() << " -> tbl_Project_TD_2215_output" 
     << std::endl << "      #COL: 4: " << "ListBuffer(supp_nation#864, cust_nation#865, l_year#866, volume#867)" 
     << std::endl; 

    std::cout << "Aggregate_1: " << tvdiff(&tv_r_Aggregate_1_146_s, &tv_r_Aggregate_1_146_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(supp_nation#864, cust_nation#865, l_year#866, sum(volume#867) AS revenue#868L) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_Project_TD_2215_output.getNumRow() << " -> tbl_Project_TD_2215_output" 
     << std::endl << "      #COL: 4: " << "ListBuffer(supp_nation#864, cust_nation#865, l_year#866, volume#867)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_1904_output.getNumRow() << " -> tbl_Aggregate_TD_1904_output" 
     << std::endl << "      #COL: 4: " << "ListBuffer(supp_nation#864, cust_nation#865, l_year#866, revenue#868L)" 
     << std::endl; 

    std::cout << "Sort_0: " << tvdiff(&tv_r_Sort_0_147_s, &tv_r_Sort_0_147_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(supp_nation#864 ASC NULLS FIRST, cust_nation#865 ASC NULLS FIRST, l_year#866 ASC NULLS FIRST) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_1904_output.getNumRow() << " -> tbl_Aggregate_TD_1904_output" 
     << std::endl << "      #COL: 4: " << "ListBuffer(supp_nation#864, cust_nation#865, l_year#866, revenue#868L)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Sort_TD_0451_output.getNumRow() << " -> tbl_Sort_TD_0451_output" 
     << std::endl << "      #COL: 4: " << "ListBuffer(supp_nation#864, cust_nation#865, l_year#866, revenue#868L)" 
     << std::endl; 

    std::cout << std::endl << " Total execution time: " << tvdiff(&tv_r_s, &tv_r_e) / 1000 << " ms"; 

    std::cout << std::endl << " Spark elapsed time: " << 39.341377 * 1000 << "ms" << std::endl; 
    return 0; 
}
