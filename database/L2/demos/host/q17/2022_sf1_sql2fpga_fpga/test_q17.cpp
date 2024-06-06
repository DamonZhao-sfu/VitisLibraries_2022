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
    // ****************************** Tables **************************** // 
    Table tbl_Aggregate_TD_0554_output("tbl_Aggregate_TD_0554_output", 6100000, 1, "");
    tbl_Aggregate_TD_0554_output.allocateHost();
    Table tbl_JOIN_INNER_TD_1119_output("tbl_JOIN_INNER_TD_1119_output", 6100000, 1, "");
    tbl_JOIN_INNER_TD_1119_output.allocateHost();
    Table tbl_JOIN_INNER_TD_2230_output("tbl_JOIN_INNER_TD_2230_output", 1507, 4, "");
    tbl_JOIN_INNER_TD_2230_output.selectOverlayVersion(1);
    tbl_JOIN_INNER_TD_2230_output.allocateHost();
    Table tbl_JOIN_INNER_TD_2230_consolidated_output("tbl_JOIN_INNER_TD_2230_consolidated_output", 1507, 3, "");
    tbl_JOIN_INNER_TD_2230_consolidated_output.selectOverlayVersion(1);
    tbl_JOIN_INNER_TD_2230_consolidated_output.allocateHost();
    Table tbl_Aggregate_TD_3417_output("tbl_Aggregate_TD_3417_output", 6100000, 2, "");
    tbl_Aggregate_TD_3417_output.allocateHost();
    Table tbl_SerializeFromObject_TD_3569_input;
    tbl_SerializeFromObject_TD_3569_input = Table("lineitem", lineitem_n, 3, in_dir);
    tbl_SerializeFromObject_TD_3569_input.selectOverlayVersion(1);
    tbl_SerializeFromObject_TD_3569_input.addCol("l_partkey", 4);
    tbl_SerializeFromObject_TD_3569_input.addCol("l_quantity", 4);
    tbl_SerializeFromObject_TD_3569_input.addCol("l_extendedprice", 4);
    tbl_SerializeFromObject_TD_3569_input.allocateHost();
    tbl_SerializeFromObject_TD_3569_input.loadHost();
    Table tbl_Filter_TD_3792_output("tbl_Filter_TD_3792_output", 6100000, 1, "");
    tbl_Filter_TD_3792_output.selectOverlayVersion(1);
    tbl_Filter_TD_3792_output.allocateHost();
    Table tbl_SerializeFromObject_TD_4932_input;
    tbl_SerializeFromObject_TD_4932_input = Table("lineitem", lineitem_n, 2, in_dir);
    tbl_SerializeFromObject_TD_4932_input.addCol("l_partkey", 4);
    tbl_SerializeFromObject_TD_4932_input.addCol("l_quantity", 4);
    tbl_SerializeFromObject_TD_4932_input.allocateHost();
    tbl_SerializeFromObject_TD_4932_input.loadHost();
    Table tbl_SerializeFromObject_TD_4103_input;
    tbl_SerializeFromObject_TD_4103_input = Table("part", part_n, 3, in_dir);
    tbl_SerializeFromObject_TD_4103_input.addCol("p_partkey", 4);
    tbl_SerializeFromObject_TD_4103_input.addCol("p_brand", TPCH_READ_P_BRND_LEN+1);
    tbl_SerializeFromObject_TD_4103_input.addCol("p_container", TPCH_READ_P_CNTR_LEN+1);
    tbl_SerializeFromObject_TD_4103_input.allocateHost();
    tbl_SerializeFromObject_TD_4103_input.loadHost();
    // ********************** Allocate Device Buffer ******************** // 
    //tbl_JOIN_INNER_TD_2230_output.allocateDevBuffer(context_h, 32);
    // tbl_SerializeFromObject_TD_3569_input.allocateDevBuffer(context_h, 32);
    //tbl_Filter_TD_3792_output.allocateDevBuffer(context_h, 32);
    // ****************************** Config **************************** // 
    
    xf::database::gqe::KernelCommand krn_cmd_build_904 = xf::database::gqe::KernelCommand();
    xf::database::gqe::KernelCommand krn_cmd_probe_904 = xf::database::gqe::KernelCommand();
    cfgCmd cfg_JOIN_INNER_TD_2230_cmds_build;
    cfg_JOIN_INNER_TD_2230_cmds_build.setup(1);
    cfg_JOIN_INNER_TD_2230_cmds_build.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_2230_gqe_join_build(krn_cmd_build_904);
    cfg_JOIN_INNER_TD_2230_cmds_build.cmd = krn_cmd_build_904.getConfigBits();
    get_meta_info_JOIN_INNER_TD_2230_gqe_join_build(cfg_JOIN_INNER_TD_2230_cmds_build.meta_in, tbl_Filter_TD_3792_output.nrow, 3);
    get_meta_info_JOIN_INNER_TD_2230_gqe_join_build(cfg_JOIN_INNER_TD_2230_cmds_build.meta_out, tbl_JOIN_INNER_TD_2230_output.nrow, 4);
    // cfg_JOIN_INNER_TD_2230_cmds_build.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_2230_cmds_probe;
    cfg_JOIN_INNER_TD_2230_cmds_probe.setup(1);
    cfg_JOIN_INNER_TD_2230_cmds_probe.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_2230_gqe_join_probe(krn_cmd_probe_904);
    cfg_JOIN_INNER_TD_2230_cmds_probe.cmd = krn_cmd_probe_904.getConfigBits();
    get_meta_info_JOIN_INNER_TD_2230_gqe_join_probe(cfg_JOIN_INNER_TD_2230_cmds_probe.meta_in, tbl_SerializeFromObject_TD_3569_input.nrow, 3);
    get_meta_info_JOIN_INNER_TD_2230_gqe_join_probe(cfg_JOIN_INNER_TD_2230_cmds_probe.meta_out, tbl_JOIN_INNER_TD_2230_output.nrow, 3);
    // cfg_JOIN_INNER_TD_2230_cmds_probe.allocateDevBuffer(context_h, 32);
    cfg_JOIN_INNER_TD_2230_cmds_build.input_key_columns = {0, -1, -1};
    cfg_JOIN_INNER_TD_2230_cmds_probe.input_key_columns = {0, -1, -1};
    cfg_JOIN_INNER_TD_2230_cmds_build.output_key_columns = {0, 1, 2, 3};
    cfg_JOIN_INNER_TD_2230_cmds_probe.output_key_columns = {0, 1, 2, 3};
    // *************************** Kernel Setup ************************* // 
    bufferTmp buftmp_h(context_h, 1); 
    buftmp_h.initBuffer(q_h); 
    std::cout << std::endl; 
    krnlEngine krnl_JOIN_INNER_TD_2230_build;
    krnl_JOIN_INNER_TD_2230_build = krnlEngine(program_h, q_h, context_h, "gqeKernel", 0);
    krnl_JOIN_INNER_TD_2230_build.setup(tbl_Filter_TD_3792_output, tbl_SerializeFromObject_TD_3569_input, tbl_JOIN_INNER_TD_2230_output, cfg_JOIN_INNER_TD_2230_cmds_build, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_2230_probe;
    krnl_JOIN_INNER_TD_2230_probe = krnlEngine(program_h, q_h, context_h, "gqeKernel", 1);
    krnl_JOIN_INNER_TD_2230_probe.setup(tbl_SerializeFromObject_TD_3569_input, tbl_Filter_TD_3792_output, tbl_JOIN_INNER_TD_2230_output, cfg_JOIN_INNER_TD_2230_cmds_probe, buftmp_h);
    // ************************** Transfer Engine *********************** // 
    transEngine trans_JOIN_INNER_TD_2230;
    trans_JOIN_INNER_TD_2230.setq(q_h,1);
    trans_JOIN_INNER_TD_2230.add(&(cfg_JOIN_INNER_TD_2230_cmds_build));
    trans_JOIN_INNER_TD_2230.add(&(cfg_JOIN_INNER_TD_2230_cmds_probe));
    trans_JOIN_INNER_TD_2230.add(&(tbl_SerializeFromObject_TD_3569_input));
    transEngine trans_JOIN_INNER_TD_2230_out;
    trans_JOIN_INNER_TD_2230_out.setq(q_h,1);
    q_h.finish();
    // ****************************** Events **************************** // 
    std::vector<cl::Event> events_h2d_wr_JOIN_INNER_TD_2230;
    std::vector<cl::Event> events_d2h_rd_JOIN_INNER_TD_2230;
    std::vector<cl::Event> events_JOIN_INNER_TD_2230;
    events_h2d_wr_JOIN_INNER_TD_2230.resize(1);
    events_d2h_rd_JOIN_INNER_TD_2230.resize(1);
    events_JOIN_INNER_TD_2230.resize(2);
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_2230;
    std::vector<cl::Event> prev_events_grp_JOIN_INNER_TD_2230;
    // **************************** Operations ************************** // 
    struct timeval tv_r_s, tv_r_e; 
    gettimeofday(&tv_r_s, 0); 

    struct timeval tv_r_Filter_3_32_s, tv_r_Filter_3_32_e;
    gettimeofday(&tv_r_Filter_3_32_s, 0);
    SW_Filter_TD_3792(tbl_SerializeFromObject_TD_4103_input, tbl_Filter_TD_3792_output);
    gettimeofday(&tv_r_Filter_3_32_e, 0);

    struct timeval tv_r_Aggregate_3_468_s, tv_r_Aggregate_3_468_e;
    gettimeofday(&tv_r_Aggregate_3_468_s, 0);
    SW_Aggregate_TD_3417(tbl_SerializeFromObject_TD_4932_input, tbl_Aggregate_TD_3417_output);
    gettimeofday(&tv_r_Aggregate_3_468_e, 0);

    struct timeval tv_r_JOIN_INNER_2_653_s, tv_r_JOIN_INNER_2_653_e;
    gettimeofday(&tv_r_JOIN_INNER_2_653_s, 0);
    tbl_Filter_TD_3792_output.tableToCol();
    tbl_SerializeFromObject_TD_3569_input.tableToCol();
    krnl_JOIN_INNER_TD_2230_build.updateMeta(tbl_Filter_TD_3792_output.getNumRow(), 3);
    krnl_JOIN_INNER_TD_2230_probe.updateMeta(tbl_SerializeFromObject_TD_3569_input.getNumRow(), 3);
    trans_JOIN_INNER_TD_2230.add(&(tbl_Filter_TD_3792_output));
    trans_JOIN_INNER_TD_2230.host2dev(0, &(prev_events_grp_JOIN_INNER_TD_2230), &(events_h2d_wr_JOIN_INNER_TD_2230[0]));
    events_grp_JOIN_INNER_TD_2230.push_back(events_h2d_wr_JOIN_INNER_TD_2230[0]);
    krnl_JOIN_INNER_TD_2230_build.run(0, &(events_grp_JOIN_INNER_TD_2230), &(events_JOIN_INNER_TD_2230[0]));
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_2230_build_done;
    events_grp_JOIN_INNER_TD_2230_build_done.push_back(events_JOIN_INNER_TD_2230[0]);
    krnl_JOIN_INNER_TD_2230_probe.run(0, &(events_grp_JOIN_INNER_TD_2230_build_done), &(events_JOIN_INNER_TD_2230[1]));
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_2230_probe_done;
    events_grp_JOIN_INNER_TD_2230_probe_done.push_back(events_JOIN_INNER_TD_2230[1]);
    trans_JOIN_INNER_TD_2230_out.add(&(tbl_JOIN_INNER_TD_2230_output));
    trans_JOIN_INNER_TD_2230_out.add(&(cfg_JOIN_INNER_TD_2230_cmds_build));
    trans_JOIN_INNER_TD_2230_out.add(&(cfg_JOIN_INNER_TD_2230_cmds_probe));
    trans_JOIN_INNER_TD_2230_out.dev2host(0, &(events_grp_JOIN_INNER_TD_2230_probe_done), &(events_d2h_rd_JOIN_INNER_TD_2230[0]));
    q_h.flush();
    q_h.finish();
    tbl_JOIN_INNER_TD_2230_output.setNumRow((cfg_JOIN_INNER_TD_2230_cmds_probe.meta_out)->getColLen());
    tbl_JOIN_INNER_TD_2230_output.colToTable();
    SW_Consolidated_JOIN_INNER_TD_2230_output(tbl_Filter_TD_3792_output, tbl_SerializeFromObject_TD_3569_input, tbl_JOIN_INNER_TD_2230_output, tbl_JOIN_INNER_TD_2230_consolidated_output);
    gettimeofday(&tv_r_JOIN_INNER_2_653_e, 0);

    struct timeval tv_r_JOIN_INNER_1_421_s, tv_r_JOIN_INNER_1_421_e;
    gettimeofday(&tv_r_JOIN_INNER_1_421_s, 0);
    SW_JOIN_INNER_TD_1119(tbl_JOIN_INNER_TD_2230_consolidated_output, tbl_Aggregate_TD_3417_output, tbl_JOIN_INNER_TD_1119_output);
    gettimeofday(&tv_r_JOIN_INNER_1_421_e, 0);

    struct timeval tv_r_Aggregate_0_717_s, tv_r_Aggregate_0_717_e;
    gettimeofday(&tv_r_Aggregate_0_717_s, 0);
    SW_Aggregate_TD_0554(tbl_JOIN_INNER_TD_1119_output, tbl_Aggregate_TD_0554_output);
    gettimeofday(&tv_r_Aggregate_0_717_e, 0);

std::cout << "CPU TIME: " << (tvdiff(&tv_r_Filter_3_32_s, &tv_r_Filter_3_32_e) + 
tvdiff(&tv_r_Aggregate_3_468_s, &tv_r_Aggregate_3_468_e) + 
tvdiff(&tv_r_JOIN_INNER_1_421_s, &tv_r_JOIN_INNER_1_421_e) + 
tvdiff(&tv_r_Aggregate_0_717_s, &tv_r_Aggregate_0_717_e) + 
1) / 1000.0 << std::endl;

std::cout << "FPGA TIME: " << (tvdiff(&tv_r_JOIN_INNER_2_653_s, &tv_r_JOIN_INNER_2_653_e) + 
1) / 1000.0 << std::endl;

    gettimeofday(&tv_r_e, 0); 
    // **************************** Print Execution Time ************************** // 
    std::cout << std::endl; 
    std::cout << "Filter_3: " << tvdiff(&tv_r_Filter_3_32_s, &tv_r_Filter_3_32_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(((isnotnull(p_brand#261) AND isnotnull(p_container#264)) AND ((p_brand#261 = Brand#21) AND (p_container#264 = WRAP BAG)))) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_4103_input.getNumRow() << " -> tbl_SerializeFromObject_TD_4103_input" 
     << std::endl << "      #COL: 3: " << "ListBuffer(p_partkey#258, p_brand#261, p_container#264)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_3792_output.getNumRow() << " -> tbl_Filter_TD_3792_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(p_partkey#258)" 
     << std::endl; 

    std::cout << "Aggregate_3: " << tvdiff(&tv_r_Aggregate_3_468_s, &tv_r_Aggregate_3_468_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(l_partkey#78, (0.2 * avg(l_quantity#81)) AS (0.2 * avg(l_quantity))#1643, l_partkey#78 AS l_partkey#78#1693) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_4932_input.getNumRow() << " -> tbl_SerializeFromObject_TD_4932_input" 
     << std::endl << "      #COL: 2: " << "ListBuffer(l_partkey#78, l_quantity#81)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_3417_output.getNumRow() << " -> tbl_Aggregate_TD_3417_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer((0.2 * avg(l_quantity))#1643, l_partkey#78#1693)" 
     << std::endl; 

    std::cout << "JOIN_INNER_2: " << tvdiff(&tv_r_JOIN_INNER_2_653_s, &tv_r_JOIN_INNER_2_653_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer((p_partkey#258 = l_partkey#78)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_3569_input.getNumRow() << " -> tbl_SerializeFromObject_TD_3569_input" 
     << std::endl << "      #COL: 3: " << "ListBuffer(l_partkey#78, l_quantity#81, l_extendedprice#82)" 
     << std::endl << "      #ROW: " << tbl_Filter_TD_3792_output.getNumRow() << " -> tbl_Filter_TD_3792_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(p_partkey#258)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_2230_consolidated_output.getNumRow() << " -> tbl_JOIN_INNER_TD_2230_consolidated_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(l_quantity#81, l_extendedprice#82, p_partkey#258)" 
     << std::endl; 

    std::cout << "JOIN_INNER_1: " << tvdiff(&tv_r_JOIN_INNER_1_421_s, &tv_r_JOIN_INNER_1_421_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(((cast(l_quantity#81 as double) < (0.2 * avg(l_quantity))#1643) AND (l_partkey#78#1693 = p_partkey#258))) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_2230_consolidated_output.getNumRow() << " -> tbl_JOIN_INNER_TD_2230_consolidated_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(l_quantity#81, l_extendedprice#82, p_partkey#258)" 
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_3417_output.getNumRow() << " -> tbl_Aggregate_TD_3417_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer((0.2 * avg(l_quantity))#1643, l_partkey#78#1693)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_1119_output.getNumRow() << " -> tbl_JOIN_INNER_TD_1119_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(l_extendedprice#82)" 
     << std::endl; 

    std::cout << "Aggregate_0: " << tvdiff(&tv_r_Aggregate_0_717_s, &tv_r_Aggregate_0_717_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(CheckOverflow((promote_precision(cast(cast(sum(l_extendedprice#82) as decimal(20,0)) as decimal(21,1))) / 7.0), DecimalType(27,6), true) AS avg_yearly#1640) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_1119_output.getNumRow() << " -> tbl_JOIN_INNER_TD_1119_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(l_extendedprice#82)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_0554_output.getNumRow() << " -> tbl_Aggregate_TD_0554_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(avg_yearly#1640)" 
     << std::endl; 

    std::cout << std::endl << " Total execution time: " << tvdiff(&tv_r_s, &tv_r_e) / 1000 << " ms"; 

    std::cout << std::endl << " Spark elapsed time: " << 41.657513 * 1000 << "ms" << std::endl; 
    return 0; 
}
