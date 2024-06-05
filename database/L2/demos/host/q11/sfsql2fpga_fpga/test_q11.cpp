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
    Table tbl_Sort_TD_0318_output("tbl_Sort_TD_0318_output", 183000000, 2, "");
    tbl_Sort_TD_0318_output.allocateHost();
    Table tbl_Filter_TD_1648_output("tbl_Filter_TD_1648_output", 183000000, 2, "");
    tbl_Filter_TD_1648_output.allocateHost();
    Table tbl_Aggregate_TD_2685_output("tbl_Aggregate_TD_2685_output", 183000000, 2, "");
    tbl_Aggregate_TD_2685_output.allocateHost();
    Table tbl_Aggregate_TD_2214_output("tbl_Aggregate_TD_2214_output", 183000000, 1, "");
    tbl_Aggregate_TD_2214_output.allocateHost();
    Table tbl_JOIN_INNER_TD_4851_output("tbl_JOIN_INNER_TD_4851_output", 31440, 3, "");
    tbl_JOIN_INNER_TD_4851_output.allocateHost(1.2, hpTimes_join);
    Table tbl_SerializeFromObject_TD_5833_input;
    tbl_SerializeFromObject_TD_5833_input = Table("partsupp", partsupp_n, 4, in_dir);
    tbl_SerializeFromObject_TD_5833_input.selectOverlayVersion(0);
    tbl_SerializeFromObject_TD_5833_input.addCol("ps_partkey", 4);
    tbl_SerializeFromObject_TD_5833_input.addCol("ps_suppkey", 4);
    tbl_SerializeFromObject_TD_5833_input.addCol("ps_availqty", 4);
    tbl_SerializeFromObject_TD_5833_input.addCol("ps_supplycost", 4);
    tbl_SerializeFromObject_TD_5833_input.allocateHost();
    tbl_SerializeFromObject_TD_5833_input.loadHost();
    Table tbl_SerializeFromObject_TD_5833_input_partition("tbl_SerializeFromObject_TD_5833_input_partition", partsupp_n, 4, "");
    tbl_SerializeFromObject_TD_5833_input_partition.allocateHost(1.2, hpTimes_join);
    tbl_SerializeFromObject_TD_5833_input_partition.allocateDevBuffer(context_h, 32);
    Table tbl_SerializeFromObject_TD_5833_input_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_SerializeFromObject_TD_5833_input_partition_array[i] = tbl_SerializeFromObject_TD_5833_input_partition.createSubTable(i);
    }
    Table tbl_JOIN_INNER_TD_3217_output("tbl_JOIN_INNER_TD_3217_output", 393, 1, "");
    tbl_JOIN_INNER_TD_3217_output.allocateHost(1.2, hpTimes_join);
    Table tbl_SerializeFromObject_TD_5507_input;
    tbl_SerializeFromObject_TD_5507_input = Table("supplier", supplier_n, 2, in_dir);
    tbl_SerializeFromObject_TD_5507_input.selectOverlayVersion(0);
    tbl_SerializeFromObject_TD_5507_input.addCol("s_suppkey", 4);
    tbl_SerializeFromObject_TD_5507_input.addCol("s_nationkey", 4);
    tbl_SerializeFromObject_TD_5507_input.allocateHost();
    tbl_SerializeFromObject_TD_5507_input.loadHost();
    Table tbl_SerializeFromObject_TD_5507_input_partition("tbl_SerializeFromObject_TD_5507_input_partition", supplier_n, 2, "");
    tbl_SerializeFromObject_TD_5507_input_partition.allocateHost(1.2, hpTimes_join);
    tbl_SerializeFromObject_TD_5507_input_partition.allocateDevBuffer(context_h, 32);
    Table tbl_SerializeFromObject_TD_5507_input_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_SerializeFromObject_TD_5507_input_partition_array[i] = tbl_SerializeFromObject_TD_5507_input_partition.createSubTable(i);
    }
    Table tbl_Filter_TD_4366_output("tbl_Filter_TD_4366_output", 183000000, 1, "");
    tbl_Filter_TD_4366_output.selectOverlayVersion(0);
    tbl_Filter_TD_4366_output.allocateHost();
    Table tbl_Filter_TD_4366_output_partition("tbl_Filter_TD_4366_output_partition", 183000000, 1, "");
    tbl_Filter_TD_4366_output_partition.allocateHost(1.2, hpTimes_join);
    tbl_Filter_TD_4366_output_partition.allocateDevBuffer(context_h, 32);
    Table tbl_Filter_TD_4366_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_Filter_TD_4366_output_partition_array[i] = tbl_Filter_TD_4366_output_partition.createSubTable(i);
    }
    Table tbl_SerializeFromObject_TD_5693_input;
    tbl_SerializeFromObject_TD_5693_input = Table("nation", nation_n, 2, in_dir);
    tbl_SerializeFromObject_TD_5693_input.addCol("n_nationkey", 4);
    tbl_SerializeFromObject_TD_5693_input.addCol("n_name", TPCH_READ_NATION_LEN+1);
    tbl_SerializeFromObject_TD_5693_input.allocateHost();
    tbl_SerializeFromObject_TD_5693_input.loadHost();
    // ********************** Allocate Device Buffer ******************** // 
    tbl_JOIN_INNER_TD_4851_output.allocateDevBuffer(context_h, 32);
    Table tbl_JOIN_INNER_TD_4851_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_JOIN_INNER_TD_4851_output_partition_array[i] = tbl_JOIN_INNER_TD_4851_output.createSubTable(i);
    }
    tbl_SerializeFromObject_TD_5833_input.allocateDevBuffer(context_h, 33);
    tbl_JOIN_INNER_TD_3217_output.allocateDevBuffer(context_h, 32);
    Table tbl_JOIN_INNER_TD_3217_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_JOIN_INNER_TD_3217_output_partition_array[i] = tbl_JOIN_INNER_TD_3217_output.createSubTable(i);
    }
    tbl_SerializeFromObject_TD_5507_input.allocateDevBuffer(context_h, 33);
    tbl_Filter_TD_4366_output.allocateDevBuffer(context_h, 33);
    // ****************************** Config **************************** // 
    cfgCmd cfg_JOIN_INNER_TD_4851_cmds;
    cfg_JOIN_INNER_TD_4851_cmds.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_4851_gqe_join (cfg_JOIN_INNER_TD_4851_cmds.cmd);
    cfg_JOIN_INNER_TD_4851_cmds.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_4851_cmds_part;
    cfg_JOIN_INNER_TD_4851_cmds_part.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_4851_gqe_join_part (cfg_JOIN_INNER_TD_4851_cmds_part.cmd);
    cfg_JOIN_INNER_TD_4851_cmds_part.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_3217_cmds;
    cfg_JOIN_INNER_TD_3217_cmds.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_3217_gqe_join (cfg_JOIN_INNER_TD_3217_cmds.cmd);
    cfg_JOIN_INNER_TD_3217_cmds.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_3217_cmds_part;
    cfg_JOIN_INNER_TD_3217_cmds_part.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_3217_gqe_join_part (cfg_JOIN_INNER_TD_3217_cmds_part.cmd);
    cfg_JOIN_INNER_TD_3217_cmds_part.allocateDevBuffer(context_h, 32);
    // *************************** Kernel Setup ************************* // 
    bufferTmp buftmp_h(context_h, 0); 
    buftmp_h.initBuffer(q_h); 
    std::cout << std::endl; 
    krnlEngine krnl_JOIN_INNER_TD_4851_part_right;
    krnl_JOIN_INNER_TD_4851_part_right = krnlEngine(program_h, q_h, context_h, "gqePart");
    krnl_JOIN_INNER_TD_4851_part_right.setup_hp(512, 1, power_of_hpTimes_join, tbl_SerializeFromObject_TD_5833_input, tbl_SerializeFromObject_TD_5833_input_partition, cfg_JOIN_INNER_TD_4851_cmds_part);
    krnlEngine krnl_JOIN_INNER_TD_4851[hpTimes_join];
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_INNER_TD_4851[i] = krnlEngine(program_h, q_h, "gqeJoin");
    }
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_INNER_TD_4851[i].setup(tbl_JOIN_INNER_TD_3217_output_partition_array[i], tbl_SerializeFromObject_TD_5833_input_partition_array[i], tbl_JOIN_INNER_TD_4851_output_partition_array[i], cfg_JOIN_INNER_TD_4851_cmds, buftmp_h);
    }
    krnlEngine krnl_JOIN_INNER_TD_4851_build;
    krnl_JOIN_INNER_TD_4851_build = krnlEngine(program_h, q_h, context_h, "gqeKernel", 0);
    krnl_JOIN_INNER_TD_4851_build.setup(tbl_JOIN_INNER_TD_3217_output, tbl_SerializeFromObject_TD_5833_input, tbl_JOIN_INNER_TD_4851_output, cfg_JOIN_INNER_TD_4851_cmds_build, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_4851_probe;
    krnl_JOIN_INNER_TD_4851_probe = krnlEngine(program_h, q_h, context_h, "gqeKernel", 1);
    krnl_JOIN_INNER_TD_4851_probe.setup(tbl_SerializeFromObject_TD_5833_input, tbl_JOIN_INNER_TD_3217_output, tbl_JOIN_INNER_TD_4851_output, cfg_JOIN_INNER_TD_4851_cmds_probe, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_3217_part_left;
    krnl_JOIN_INNER_TD_3217_part_left = krnlEngine(program_h, q_h, context_h, "gqePart");
    krnl_JOIN_INNER_TD_3217_part_left.setup_hp(512, 0, power_of_hpTimes_join, tbl_Filter_TD_4366_output, tbl_Filter_TD_4366_output_partition, cfg_JOIN_INNER_TD_3217_cmds_part);
    krnlEngine krnl_JOIN_INNER_TD_3217_part_right;
    krnl_JOIN_INNER_TD_3217_part_right = krnlEngine(program_h, q_h, context_h, "gqePart");
    krnl_JOIN_INNER_TD_3217_part_right.setup_hp(512, 1, power_of_hpTimes_join, tbl_SerializeFromObject_TD_5507_input, tbl_SerializeFromObject_TD_5507_input_partition, cfg_JOIN_INNER_TD_3217_cmds_part);
    krnlEngine krnl_JOIN_INNER_TD_3217[hpTimes_join];
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_INNER_TD_3217[i] = krnlEngine(program_h, q_h, "gqeJoin");
    }
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_INNER_TD_3217[i].setup(tbl_Filter_TD_4366_output_partition_array[i], tbl_SerializeFromObject_TD_5507_input_partition_array[i], tbl_JOIN_INNER_TD_3217_output_partition_array[i], cfg_JOIN_INNER_TD_3217_cmds, buftmp_h);
    }
    krnlEngine krnl_JOIN_INNER_TD_3217_build;
    krnl_JOIN_INNER_TD_3217_build = krnlEngine(program_h, q_h, context_h, "gqeKernel", 0);
    krnl_JOIN_INNER_TD_3217_build.setup(tbl_Filter_TD_4366_output, tbl_SerializeFromObject_TD_5507_input, tbl_JOIN_INNER_TD_3217_output, cfg_JOIN_INNER_TD_3217_cmds_build, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_3217_probe;
    krnl_JOIN_INNER_TD_3217_probe = krnlEngine(program_h, q_h, context_h, "gqeKernel", 1);
    krnl_JOIN_INNER_TD_3217_probe.setup(tbl_SerializeFromObject_TD_5507_input, tbl_Filter_TD_4366_output, tbl_JOIN_INNER_TD_3217_output, cfg_JOIN_INNER_TD_3217_cmds_probe, buftmp_h);
    // ************************** Transfer Engine *********************** // 
    transEngine trans_JOIN_INNER_TD_4851;
    trans_JOIN_INNER_TD_4851.setq(q_h,0);
    trans_JOIN_INNER_TD_4851.add(&(cfg_JOIN_INNER_TD_4851_cmds_part));
    trans_JOIN_INNER_TD_4851.add(&(cfg_JOIN_INNER_TD_4851_cmds));
    trans_JOIN_INNER_TD_4851.add(&(tbl_SerializeFromObject_TD_5833_input));
    transEngine trans_JOIN_INNER_TD_4851_out;
    trans_JOIN_INNER_TD_4851_out.setq(q_h,0);
    q_h.finish();
    transEngine trans_JOIN_INNER_TD_3217;
    trans_JOIN_INNER_TD_3217.setq(q_h,0);
    trans_JOIN_INNER_TD_3217.add(&(cfg_JOIN_INNER_TD_3217_cmds_part));
    trans_JOIN_INNER_TD_3217.add(&(cfg_JOIN_INNER_TD_3217_cmds));
    trans_JOIN_INNER_TD_3217.add(&(tbl_SerializeFromObject_TD_5507_input));
    q_h.finish();
    // ****************************** Events **************************** // 
    std::vector<cl::Event> events_h2d_wr_JOIN_INNER_TD_4851;
    std::vector<cl::Event> events_d2h_rd_JOIN_INNER_TD_4851;
    std::vector<cl::Event> events_JOIN_INNER_TD_4851[2];
    events_h2d_wr_JOIN_INNER_TD_4851.resize(1);
    events_d2h_rd_JOIN_INNER_TD_4851.resize(1);
    events_JOIN_INNER_TD_4851[0].resize(1);
    events_JOIN_INNER_TD_4851[1].resize(hpTimes_join);
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_4851;
    std::vector<cl::Event> prev_events_grp_JOIN_INNER_TD_4851;
    std::vector<cl::Event> events_h2d_wr_JOIN_INNER_TD_3217;
    std::vector<cl::Event> events_d2h_rd_JOIN_INNER_TD_3217;
    std::vector<cl::Event> events_JOIN_INNER_TD_3217[2];
    events_h2d_wr_JOIN_INNER_TD_3217.resize(1);
    events_d2h_rd_JOIN_INNER_TD_3217.resize(1);
    events_JOIN_INNER_TD_3217[0].resize(2);
    events_JOIN_INNER_TD_3217[1].resize(hpTimes_join);
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_3217;
    std::vector<cl::Event> prev_events_grp_JOIN_INNER_TD_3217;
    // **************************** Operations ************************** // 
    struct timeval tv_r_s, tv_r_e; 
    gettimeofday(&tv_r_s, 0); 

    struct timeval tv_r_Filter_4_126_s, tv_r_Filter_4_126_e;
    gettimeofday(&tv_r_Filter_4_126_s, 0);
    SW_Filter_TD_4366(tbl_SerializeFromObject_TD_5693_input, tbl_Filter_TD_4366_output);
    gettimeofday(&tv_r_Filter_4_126_e, 0);

    struct timeval tv_r_JOIN_INNER_3_750_s, tv_r_JOIN_INNER_3_750_e;
    gettimeofday(&tv_r_JOIN_INNER_3_750_s, 0);
    trans_JOIN_INNER_TD_3217.add(&(tbl_Filter_TD_4366_output));
    trans_JOIN_INNER_TD_3217.host2dev(0, &(prev_events_grp_JOIN_INNER_TD_3217), &(events_h2d_wr_JOIN_INNER_TD_3217[0]));
    events_grp_JOIN_INNER_TD_3217.push_back(events_h2d_wr_JOIN_INNER_TD_3217[0]);
    krnl_JOIN_INNER_TD_3217_part_left.run(0, &(events_grp_JOIN_INNER_TD_3217), &(events_JOIN_INNER_TD_3217[0][0]));
    krnl_JOIN_INNER_TD_3217_part_right.run(0, &(events_grp_JOIN_INNER_TD_3217), &(events_JOIN_INNER_TD_3217[0][1]));
    for (int i(0); i < hpTimes_join; ++i) {
        krnl_JOIN_INNER_TD_3217[i].run(0, &(events_JOIN_INNER_TD_3217[0]), &(events_JOIN_INNER_TD_3217[1][i]));
    }
    gettimeofday(&tv_r_JOIN_INNER_3_750_e, 0);

    struct timeval tv_r_JOIN_INNER_4_798_s, tv_r_JOIN_INNER_4_798_e;
    gettimeofday(&tv_r_JOIN_INNER_4_798_s, 0);
    prev_events_grp_JOIN_INNER_TD_4851.push_back(events_h2d_wr_JOIN_INNER_TD_3217[0]);
    trans_JOIN_INNER_TD_4851.host2dev(0, &(prev_events_grp_JOIN_INNER_TD_4851), &(events_h2d_wr_JOIN_INNER_TD_4851[0]));
    events_grp_JOIN_INNER_TD_4851.push_back(events_h2d_wr_JOIN_INNER_TD_4851[0]);
    events_grp_JOIN_INNER_TD_4851.insert(std::end(events_grp_JOIN_INNER_TD_4851), std::begin(events_JOIN_INNER_TD_3217[0]), std::end(events_JOIN_INNER_TD_3217[0]));
    events_grp_JOIN_INNER_TD_4851.insert(std::end(events_grp_JOIN_INNER_TD_4851), std::begin(events_JOIN_INNER_TD_3217[1]), std::end(events_JOIN_INNER_TD_3217[1]));
    krnl_JOIN_INNER_TD_4851_part_right.run(0, &(events_grp_JOIN_INNER_TD_4851), &(events_JOIN_INNER_TD_4851[0][0]));
    for (int i(0); i < hpTimes_join; ++i) {
        krnl_JOIN_INNER_TD_4851[i].run(0, &(events_grp_JOIN_INNER_TD_4851), &(events_JOIN_INNER_TD_4851[1][i]));
    }
    for (int i(0); i < hpTimes_join; ++i) {
        trans_JOIN_INNER_TD_4851_out.add(&(tbl_JOIN_INNER_TD_4851_output_partition_array[i]));
    }
    trans_JOIN_INNER_TD_4851_out.dev2host(0, &(events_JOIN_INNER_TD_4851[1]), &(events_d2h_rd_JOIN_INNER_TD_4851[0]));
    q_h.flush();
    q_h.finish();
    gettimeofday(&tv_r_JOIN_INNER_4_798_e, 0);

    struct timeval tv_r_Aggregate_2_356_s, tv_r_Aggregate_2_356_e;
    gettimeofday(&tv_r_Aggregate_2_356_s, 0);
    SW_Aggregate_TD_2214(tbl_JOIN_INNER_TD_4851_output_partition_array, tbl_Aggregate_TD_2214_output, hpTimes_join);
    gettimeofday(&tv_r_Aggregate_2_356_e, 0);

    struct timeval tv_r_Aggregate_2_998_s, tv_r_Aggregate_2_998_e;
    gettimeofday(&tv_r_Aggregate_2_998_s, 0);
    SW_Aggregate_TD_2685(tbl_JOIN_INNER_TD_4851_output_partition_array, tbl_Aggregate_TD_2685_output, hpTimes_join);
    gettimeofday(&tv_r_Aggregate_2_998_e, 0);

    struct timeval tv_r_Filter_1_49_s, tv_r_Filter_1_49_e;
    gettimeofday(&tv_r_Filter_1_49_s, 0);
    SW_Filter_TD_1648(tbl_Aggregate_TD_2685_output, tbl_Aggregate_TD_2214_output, tbl_Filter_TD_1648_output);
    gettimeofday(&tv_r_Filter_1_49_e, 0);

    struct timeval tv_r_Sort_0_750_s, tv_r_Sort_0_750_e;
    gettimeofday(&tv_r_Sort_0_750_s, 0);
    SW_Sort_TD_0318(tbl_Filter_TD_1648_output, tbl_Sort_TD_0318_output);
    gettimeofday(&tv_r_Sort_0_750_e, 0);

std::cout << "CPU TIME: " << (tvdiff(&tv_r_Filter_4_126_s, &tv_r_Filter_4_126_e) + 
tvdiff(&tv_r_Aggregate_2_356_s, &tv_r_Aggregate_2_356_e) + 
tvdiff(&tv_r_Aggregate_2_998_s, &tv_r_Aggregate_2_998_e) + 
tvdiff(&tv_r_Filter_1_49_s, &tv_r_Filter_1_49_e) + 
tvdiff(&tv_r_Sort_0_750_s, &tv_r_Sort_0_750_e) + 
1) / 1000.0 << std::endl;

std::cout << "FPGA TIME: " << (tvdiff(&tv_r_JOIN_INNER_3_750_s, &tv_r_JOIN_INNER_3_750_e) + 
tvdiff(&tv_r_JOIN_INNER_4_798_s, &tv_r_JOIN_INNER_4_798_e) + 
1) / 1000.0 << std::endl;

    gettimeofday(&tv_r_e, 0); 
    // **************************** Print Execution Time ************************** // 
    std::cout << std::endl; 
    std::cout << "Filter_4: " << tvdiff(&tv_r_Filter_4_126_s, &tv_r_Filter_4_126_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer((isnotnull(n_name#145) AND (n_name#145 = IRAN))) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_5693_input.getNumRow() << " -> tbl_SerializeFromObject_TD_5693_input" 
     << std::endl << "      #COL: 2: " << "ListBuffer(n_nationkey#144, n_name#145)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_4366_output.getNumRow() << " -> tbl_Filter_TD_4366_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(n_nationkey#144)" 
     << std::endl; 

    std::cout << "JOIN_INNER_3: " << tvdiff(&tv_r_JOIN_INNER_3_750_s, &tv_r_JOIN_INNER_3_750_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer((s_nationkey#345 = n_nationkey#144)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_5507_input.getNumRow() << " -> tbl_SerializeFromObject_TD_5507_input" 
     << std::endl << "      #COL: 2: " << "ListBuffer(s_suppkey#342, s_nationkey#345)" 
     << std::endl << "      #ROW: " << tbl_Filter_TD_4366_output.getNumRow() << " -> tbl_Filter_TD_4366_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(n_nationkey#144)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_3217_output.getNumRow() << " -> tbl_JOIN_INNER_TD_3217_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(s_suppkey#342)" 
     << std::endl; 

    std::cout << "JOIN_INNER_4: " << tvdiff(&tv_r_JOIN_INNER_4_798_s, &tv_r_JOIN_INNER_4_798_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer((ps_suppkey#306 = s_suppkey#342)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_5833_input.getNumRow() << " -> tbl_SerializeFromObject_TD_5833_input" 
     << std::endl << "      #COL: 4: " << "ListBuffer(ps_partkey#305, ps_suppkey#306, ps_availqty#307, ps_supplycost#308)" 
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_3217_output.getNumRow() << " -> tbl_JOIN_INNER_TD_3217_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(s_suppkey#342)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_4851_output.getNumRow() << " -> tbl_JOIN_INNER_TD_4851_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(ps_partkey#305, ps_availqty#307, ps_supplycost#308)" 
     << std::endl; 

    std::cout << "Aggregate_2: " << tvdiff(&tv_r_Aggregate_2_356_s, &tv_r_Aggregate_2_356_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(CheckOverflow((promote_precision(cast(cast(sum((ps_supplycost#308 * ps_availqty#307)) as decimal(20,0)) as decimal(30,10))) * 0.0001000000), DecimalType(31,10), true) AS (sum((ps_supplycost * ps_availqty)) * 0.0001000000)#1206) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_4851_output.getNumRow() << " -> tbl_JOIN_INNER_TD_4851_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(ps_partkey#305, ps_availqty#307, ps_supplycost#308)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_2214_output.getNumRow() << " -> tbl_Aggregate_TD_2214_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer((sum((ps_supplycost * ps_availqty)) * 0.0001000000)#1206)" 
     << std::endl; 

    std::cout << "Aggregate_2: " << tvdiff(&tv_r_Aggregate_2_998_s, &tv_r_Aggregate_2_998_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(ps_partkey#305, sum((ps_supplycost#308 * ps_availqty#307)) AS value#1202L) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_4851_output.getNumRow() << " -> tbl_JOIN_INNER_TD_4851_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(ps_partkey#305, ps_availqty#307, ps_supplycost#308)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_2685_output.getNumRow() << " -> tbl_Aggregate_TD_2685_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(ps_partkey#305, value#1202L)" 
     << std::endl; 

    std::cout << "Filter_1: " << tvdiff(&tv_r_Filter_1_49_s, &tv_r_Filter_1_49_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer((isnotnull(value#1202L) AND (cast(cast(value#1202L as decimal(20,0)) as decimal(31,10)) > scalar-subquery#1203 []))) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_2685_output.getNumRow() << " -> tbl_Aggregate_TD_2685_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(ps_partkey#305, value#1202L)" 
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_2214_output.getNumRow() << " -> tbl_Aggregate_TD_2214_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer((sum((ps_supplycost * ps_availqty)) * 0.0001000000)#1206)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_1648_output.getNumRow() << " -> tbl_Filter_TD_1648_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(ps_partkey#305, value#1202L)" 
     << std::endl; 

    std::cout << "Sort_0: " << tvdiff(&tv_r_Sort_0_750_s, &tv_r_Sort_0_750_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(value#1202L DESC NULLS LAST) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_1648_output.getNumRow() << " -> tbl_Filter_TD_1648_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(ps_partkey#305, value#1202L)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Sort_TD_0318_output.getNumRow() << " -> tbl_Sort_TD_0318_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(ps_partkey#305, value#1202L)" 
     << std::endl; 

    std::cout << std::endl << " Total execution time: " << tvdiff(&tv_r_s, &tv_r_e) / 1000 << " ms"; 

    std::cout << std::endl << " Spark elapsed time: " << 2.363161 * 1000 << "ms" << std::endl; 
    return 0; 
}
