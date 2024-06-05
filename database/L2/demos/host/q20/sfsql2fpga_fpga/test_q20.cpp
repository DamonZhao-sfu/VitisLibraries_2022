// number of overlays (w/o fusion): 5 
// number of overlays (w/ fusion): 5 
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

#include "cfgFunc_q20.hpp" 
#include "q20.hpp" 

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
    std::cout << "NOTE:running query #20\n."; 
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
    Table tbl_Sort_TD_0516_output("tbl_Sort_TD_0516_output", 183000000, 2, "");
    tbl_Sort_TD_0516_output.allocateHost();
    Table tbl_JOIN_INNER_TD_1646_output("tbl_JOIN_INNER_TD_1646_output", 210, 2, "");
    tbl_JOIN_INNER_TD_1646_output.allocateHost(1.2, hpTimes_join);
    Table tbl_JOIN_LEFTSEMI_TD_2734_output("tbl_JOIN_LEFTSEMI_TD_2734_output", 5366, 3, "");
    tbl_JOIN_LEFTSEMI_TD_2734_output.allocateHost(1.2, hpTimes_join);
    Table tbl_Filter_TD_2986_output("tbl_Filter_TD_2986_output", 183000000, 1, "");
    tbl_Filter_TD_2986_output.selectOverlayVersion(0);
    tbl_Filter_TD_2986_output.allocateHost();
    Table tbl_Filter_TD_2986_output_partition("tbl_Filter_TD_2986_output_partition", 183000000, 1, "");
    tbl_Filter_TD_2986_output_partition.allocateHost(1.2, hpTimes_join);
    tbl_Filter_TD_2986_output_partition.allocateDevBuffer(context_h, 32);
    Table tbl_Filter_TD_2986_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_Filter_TD_2986_output_partition_array[i] = tbl_Filter_TD_2986_output_partition.createSubTable(i);
    }
    Table tbl_SerializeFromObject_TD_3999_input_stringRowIDSubstitute;
    tbl_SerializeFromObject_TD_3999_input_stringRowIDSubstitute = Table("supplier", supplier_n, 4, in_dir);
    tbl_SerializeFromObject_TD_3999_input_stringRowIDSubstitute.addCol("s_suppkey", 4);
    tbl_SerializeFromObject_TD_3999_input_stringRowIDSubstitute.addCol("s_name", 4, 1, 0);
    tbl_SerializeFromObject_TD_3999_input_stringRowIDSubstitute.addCol("s_address", 4, 1, 0);
    tbl_SerializeFromObject_TD_3999_input_stringRowIDSubstitute.addCol("s_nationkey", 4);
    tbl_SerializeFromObject_TD_3999_input_stringRowIDSubstitute.allocateHost();
    tbl_SerializeFromObject_TD_3999_input_stringRowIDSubstitute.loadHost();
    Table tbl_SerializeFromObject_TD_3999_input_stringRowIDSubstitute_partition("tbl_SerializeFromObject_TD_3999_input_stringRowIDSubstitute_partition", supplier_n, 4, "");
    tbl_SerializeFromObject_TD_3999_input_stringRowIDSubstitute_partition.allocateHost(1.2, hpTimes_join);
    tbl_SerializeFromObject_TD_3999_input_stringRowIDSubstitute_partition.allocateDevBuffer(context_h, 32);
    Table tbl_SerializeFromObject_TD_3999_input_stringRowIDSubstitute_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_SerializeFromObject_TD_3999_input_stringRowIDSubstitute_partition_array[i] = tbl_SerializeFromObject_TD_3999_input_stringRowIDSubstitute_partition.createSubTable(i);
    }
    Table tbl_SerializeFromObject_TD_3999_input;
    tbl_SerializeFromObject_TD_3999_input = Table("supplier", supplier_n, 4, in_dir);
    tbl_SerializeFromObject_TD_3999_input.addCol("s_suppkey", 4);
    tbl_SerializeFromObject_TD_3999_input.addCol("s_name", TPCH_READ_S_NAME_LEN+1);
    tbl_SerializeFromObject_TD_3999_input.addCol("s_address", TPCH_READ_S_ADDR_MAX+1);
    tbl_SerializeFromObject_TD_3999_input.addCol("s_nationkey", 4);
    tbl_SerializeFromObject_TD_3999_input.allocateHost();
    tbl_SerializeFromObject_TD_3999_input.loadHost();
    Table tbl_JOIN_INNER_TD_3765_output("tbl_JOIN_INNER_TD_3765_output", 7670, 1, "");
    tbl_JOIN_INNER_TD_3765_output.allocateHost(1.2, hpTimes_join);
    Table tbl_SerializeFromObject_TD_3371_input;
    tbl_SerializeFromObject_TD_3371_input = Table("nation", nation_n, 2, in_dir);
    tbl_SerializeFromObject_TD_3371_input.addCol("n_nationkey", 4);
    tbl_SerializeFromObject_TD_3371_input.addCol("n_name", TPCH_READ_NATION_LEN+1);
    tbl_SerializeFromObject_TD_3371_input.allocateHost();
    tbl_SerializeFromObject_TD_3371_input.loadHost();
    Table tbl_JOIN_LEFTSEMI_TD_4447_output("tbl_JOIN_LEFTSEMI_TD_4447_output", 11160, 3, "");
    tbl_JOIN_LEFTSEMI_TD_4447_output.allocateHost(1.2, hpTimes_join);
    Table tbl_Filter_TD_4268_output("tbl_Filter_TD_4268_output", 183000000, 3, "");
    tbl_Filter_TD_4268_output.selectOverlayVersion(0);
    tbl_Filter_TD_4268_output.allocateHost();
    Table tbl_Filter_TD_4268_output_partition("tbl_Filter_TD_4268_output_partition", 183000000, 3, "");
    tbl_Filter_TD_4268_output_partition.allocateHost(1.2, hpTimes_join);
    tbl_Filter_TD_4268_output_partition.allocateDevBuffer(context_h, 32);
    Table tbl_Filter_TD_4268_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_Filter_TD_4268_output_partition_array[i] = tbl_Filter_TD_4268_output_partition.createSubTable(i);
    }
    Table tbl_SerializeFromObject_TD_5882_input;
    tbl_SerializeFromObject_TD_5882_input = Table("partsupp", partsupp_n, 3, in_dir);
    tbl_SerializeFromObject_TD_5882_input.selectOverlayVersion(0);
    tbl_SerializeFromObject_TD_5882_input.addCol("ps_partkey", 4);
    tbl_SerializeFromObject_TD_5882_input.addCol("ps_suppkey", 4);
    tbl_SerializeFromObject_TD_5882_input.addCol("ps_availqty", 4);
    tbl_SerializeFromObject_TD_5882_input.allocateHost();
    tbl_SerializeFromObject_TD_5882_input.loadHost();
    Table tbl_SerializeFromObject_TD_5882_input_partition("tbl_SerializeFromObject_TD_5882_input_partition", partsupp_n, 3, "");
    tbl_SerializeFromObject_TD_5882_input_partition.allocateHost(1.2, hpTimes_join);
    tbl_SerializeFromObject_TD_5882_input_partition.allocateDevBuffer(context_h, 32);
    Table tbl_SerializeFromObject_TD_5882_input_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_SerializeFromObject_TD_5882_input_partition_array[i] = tbl_SerializeFromObject_TD_5882_input_partition.createSubTable(i);
    }
    Table tbl_Filter_TD_5752_output("tbl_Filter_TD_5752_output", 183000000, 1, "");
    tbl_Filter_TD_5752_output.selectOverlayVersion(0);
    tbl_Filter_TD_5752_output.allocateHost();
    Table tbl_Filter_TD_5752_output_partition("tbl_Filter_TD_5752_output_partition", 183000000, 1, "");
    tbl_Filter_TD_5752_output_partition.allocateHost(1.2, hpTimes_join);
    tbl_Filter_TD_5752_output_partition.allocateDevBuffer(context_h, 32);
    Table tbl_Filter_TD_5752_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_Filter_TD_5752_output_partition_array[i] = tbl_Filter_TD_5752_output_partition.createSubTable(i);
    }
    Table tbl_Aggregate_TD_5897_output("tbl_Aggregate_TD_5897_output", 183000000, 3, "");
    tbl_Aggregate_TD_5897_output.allocateHost();
    Table tbl_SerializeFromObject_TD_6680_input;
    tbl_SerializeFromObject_TD_6680_input = Table("part", part_n, 2, in_dir);
    tbl_SerializeFromObject_TD_6680_input.addCol("p_partkey", 4);
    tbl_SerializeFromObject_TD_6680_input.addCol("p_name", TPCH_READ_P_NAME_LEN+1);
    tbl_SerializeFromObject_TD_6680_input.allocateHost();
    tbl_SerializeFromObject_TD_6680_input.loadHost();
    Table tbl_JOIN_LEFTSEMI_TD_6611_output("tbl_JOIN_LEFTSEMI_TD_6611_output", 12816, 3, "");
    tbl_JOIN_LEFTSEMI_TD_6611_output.allocateHost(1.2, hpTimes_join);
    Table tbl_Filter_TD_7742_output("tbl_Filter_TD_7742_output", 183000000, 3, "");
    tbl_Filter_TD_7742_output.selectOverlayVersion(0);
    tbl_Filter_TD_7742_output.allocateHost();
    Table tbl_Filter_TD_7742_output_partition("tbl_Filter_TD_7742_output_partition", 183000000, 3, "");
    tbl_Filter_TD_7742_output_partition.allocateHost(1.2, hpTimes_join);
    tbl_Filter_TD_7742_output_partition.allocateDevBuffer(context_h, 32);
    Table tbl_Filter_TD_7742_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_Filter_TD_7742_output_partition_array[i] = tbl_Filter_TD_7742_output_partition.createSubTable(i);
    }
    Table tbl_Filter_TD_7414_output("tbl_Filter_TD_7414_output", 183000000, 1, "");
    tbl_Filter_TD_7414_output.selectOverlayVersion(0);
    tbl_Filter_TD_7414_output.allocateHost();
    Table tbl_Filter_TD_7414_output_partition("tbl_Filter_TD_7414_output_partition", 183000000, 1, "");
    tbl_Filter_TD_7414_output_partition.allocateHost(1.2, hpTimes_join);
    tbl_Filter_TD_7414_output_partition.allocateDevBuffer(context_h, 32);
    Table tbl_Filter_TD_7414_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_Filter_TD_7414_output_partition_array[i] = tbl_Filter_TD_7414_output_partition.createSubTable(i);
    }
    Table tbl_SerializeFromObject_TD_8258_input;
    tbl_SerializeFromObject_TD_8258_input = Table("lineitem", lineitem_n, 4, in_dir);
    tbl_SerializeFromObject_TD_8258_input.addCol("l_partkey", 4);
    tbl_SerializeFromObject_TD_8258_input.addCol("l_suppkey", 4);
    tbl_SerializeFromObject_TD_8258_input.addCol("l_quantity", 4);
    tbl_SerializeFromObject_TD_8258_input.addCol("l_shipdate", 4);
    tbl_SerializeFromObject_TD_8258_input.allocateHost();
    tbl_SerializeFromObject_TD_8258_input.loadHost();
    Table tbl_SerializeFromObject_TD_820_input;
    tbl_SerializeFromObject_TD_820_input = Table("part", part_n, 2, in_dir);
    tbl_SerializeFromObject_TD_820_input.addCol("p_partkey", 4);
    tbl_SerializeFromObject_TD_820_input.addCol("p_name", TPCH_READ_P_NAME_LEN+1);
    tbl_SerializeFromObject_TD_820_input.allocateHost();
    tbl_SerializeFromObject_TD_820_input.loadHost();
    // ********************** Allocate Device Buffer ******************** // 
    tbl_JOIN_INNER_TD_1646_output.allocateDevBuffer(context_h, 32);
    Table tbl_JOIN_INNER_TD_1646_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_JOIN_INNER_TD_1646_output_partition_array[i] = tbl_JOIN_INNER_TD_1646_output.createSubTable(i);
    }
    tbl_JOIN_LEFTSEMI_TD_2734_output.allocateDevBuffer(context_h, 32);
    Table tbl_JOIN_LEFTSEMI_TD_2734_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_JOIN_LEFTSEMI_TD_2734_output_partition_array[i] = tbl_JOIN_LEFTSEMI_TD_2734_output.createSubTable(i);
    }
    tbl_Filter_TD_2986_output.allocateDevBuffer(context_h, 33);
    tbl_SerializeFromObject_TD_3999_input_stringRowIDSubstitute.allocateDevBuffer(context_h, 33);
    tbl_JOIN_INNER_TD_3765_output.allocateDevBuffer(context_h, 32);
    Table tbl_JOIN_INNER_TD_3765_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_JOIN_INNER_TD_3765_output_partition_array[i] = tbl_JOIN_INNER_TD_3765_output.createSubTable(i);
    }
    tbl_JOIN_LEFTSEMI_TD_4447_output.allocateDevBuffer(context_h, 32);
    Table tbl_JOIN_LEFTSEMI_TD_4447_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_JOIN_LEFTSEMI_TD_4447_output_partition_array[i] = tbl_JOIN_LEFTSEMI_TD_4447_output.createSubTable(i);
    }
    tbl_Filter_TD_4268_output.allocateDevBuffer(context_h, 33);
    tbl_SerializeFromObject_TD_5882_input.allocateDevBuffer(context_h, 33);
    tbl_Filter_TD_5752_output.allocateDevBuffer(context_h, 33);
    tbl_JOIN_LEFTSEMI_TD_6611_output.allocateDevBuffer(context_h, 32);
    Table tbl_JOIN_LEFTSEMI_TD_6611_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_JOIN_LEFTSEMI_TD_6611_output_partition_array[i] = tbl_JOIN_LEFTSEMI_TD_6611_output.createSubTable(i);
    }
    tbl_Filter_TD_7742_output.allocateDevBuffer(context_h, 33);
    tbl_Filter_TD_7414_output.allocateDevBuffer(context_h, 33);
    // ****************************** Config **************************** // 
    cfgCmd cfg_JOIN_INNER_TD_1646_cmds;
    cfg_JOIN_INNER_TD_1646_cmds.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_1646_gqe_join (cfg_JOIN_INNER_TD_1646_cmds.cmd);
    cfg_JOIN_INNER_TD_1646_cmds.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_1646_cmds_part;
    cfg_JOIN_INNER_TD_1646_cmds_part.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_1646_gqe_join_part (cfg_JOIN_INNER_TD_1646_cmds_part.cmd);
    cfg_JOIN_INNER_TD_1646_cmds_part.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_LEFTSEMI_TD_2734_cmds;
    cfg_JOIN_LEFTSEMI_TD_2734_cmds.allocateHost();
    get_cfg_dat_JOIN_LEFTSEMI_TD_2734_gqe_join (cfg_JOIN_LEFTSEMI_TD_2734_cmds.cmd);
    cfg_JOIN_LEFTSEMI_TD_2734_cmds.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_LEFTSEMI_TD_2734_cmds_part;
    cfg_JOIN_LEFTSEMI_TD_2734_cmds_part.allocateHost();
    get_cfg_dat_JOIN_LEFTSEMI_TD_2734_gqe_join_part (cfg_JOIN_LEFTSEMI_TD_2734_cmds_part.cmd);
    cfg_JOIN_LEFTSEMI_TD_2734_cmds_part.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_3765_cmds;
    cfg_JOIN_INNER_TD_3765_cmds.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_3765_gqe_join (cfg_JOIN_INNER_TD_3765_cmds.cmd);
    cfg_JOIN_INNER_TD_3765_cmds.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_3765_cmds_part;
    cfg_JOIN_INNER_TD_3765_cmds_part.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_3765_gqe_join_part (cfg_JOIN_INNER_TD_3765_cmds_part.cmd);
    cfg_JOIN_INNER_TD_3765_cmds_part.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_LEFTSEMI_TD_4447_cmds;
    cfg_JOIN_LEFTSEMI_TD_4447_cmds.allocateHost();
    get_cfg_dat_JOIN_LEFTSEMI_TD_4447_gqe_join (cfg_JOIN_LEFTSEMI_TD_4447_cmds.cmd);
    cfg_JOIN_LEFTSEMI_TD_4447_cmds.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_LEFTSEMI_TD_4447_cmds_part;
    cfg_JOIN_LEFTSEMI_TD_4447_cmds_part.allocateHost();
    get_cfg_dat_JOIN_LEFTSEMI_TD_4447_gqe_join_part (cfg_JOIN_LEFTSEMI_TD_4447_cmds_part.cmd);
    cfg_JOIN_LEFTSEMI_TD_4447_cmds_part.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_LEFTSEMI_TD_6611_cmds;
    cfg_JOIN_LEFTSEMI_TD_6611_cmds.allocateHost();
    get_cfg_dat_JOIN_LEFTSEMI_TD_6611_gqe_join (cfg_JOIN_LEFTSEMI_TD_6611_cmds.cmd);
    cfg_JOIN_LEFTSEMI_TD_6611_cmds.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_LEFTSEMI_TD_6611_cmds_part;
    cfg_JOIN_LEFTSEMI_TD_6611_cmds_part.allocateHost();
    get_cfg_dat_JOIN_LEFTSEMI_TD_6611_gqe_join_part (cfg_JOIN_LEFTSEMI_TD_6611_cmds_part.cmd);
    cfg_JOIN_LEFTSEMI_TD_6611_cmds_part.allocateDevBuffer(context_h, 32);
    // *************************** Kernel Setup ************************* // 
    bufferTmp buftmp_h(context_h, 0); 
    buftmp_h.initBuffer(q_h); 
    std::cout << std::endl; 
    krnlEngine krnl_JOIN_INNER_TD_1646_part_right;
    krnl_JOIN_INNER_TD_1646_part_right = krnlEngine(program_h, q_h, context_h, "gqePart");
    krnl_JOIN_INNER_TD_1646_part_right.setup_hp(512, 1, power_of_hpTimes_join, tbl_Filter_TD_2986_output, tbl_Filter_TD_2986_output_partition, cfg_JOIN_INNER_TD_1646_cmds_part);
    krnlEngine krnl_JOIN_INNER_TD_1646[hpTimes_join];
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_INNER_TD_1646[i] = krnlEngine(program_h, q_h, "gqeJoin");
    }
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_INNER_TD_1646[i].setup(tbl_JOIN_LEFTSEMI_TD_2734_output_partition_array[i], tbl_Filter_TD_2986_output_partition_array[i], tbl_JOIN_INNER_TD_1646_output_partition_array[i], cfg_JOIN_INNER_TD_1646_cmds, buftmp_h);
    }
    krnlEngine krnl_JOIN_INNER_TD_1646_build;
    krnl_JOIN_INNER_TD_1646_build = krnlEngine(program_h, q_h, context_h, "gqeKernel", 0);
    krnl_JOIN_INNER_TD_1646_build.setup(tbl_JOIN_LEFTSEMI_TD_2734_output, tbl_Filter_TD_2986_output, tbl_JOIN_INNER_TD_1646_output, cfg_JOIN_INNER_TD_1646_cmds_build, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_1646_probe;
    krnl_JOIN_INNER_TD_1646_probe = krnlEngine(program_h, q_h, context_h, "gqeKernel", 1);
    krnl_JOIN_INNER_TD_1646_probe.setup(tbl_Filter_TD_2986_output, tbl_JOIN_LEFTSEMI_TD_2734_output, tbl_JOIN_INNER_TD_1646_output, cfg_JOIN_INNER_TD_1646_cmds_probe, buftmp_h);
    krnlEngine krnl_JOIN_LEFTSEMI_TD_2734_part_right;
    krnl_JOIN_LEFTSEMI_TD_2734_part_right = krnlEngine(program_h, q_h, context_h, "gqePart");
    krnl_JOIN_LEFTSEMI_TD_2734_part_right.setup_hp(512, 1, power_of_hpTimes_join, tbl_SerializeFromObject_TD_3999_input_stringRowIDSubstitute, tbl_SerializeFromObject_TD_3999_input_stringRowIDSubstitute_partition, cfg_JOIN_LEFTSEMI_TD_2734_cmds_part);
    krnlEngine krnl_JOIN_LEFTSEMI_TD_2734[hpTimes_join];
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_LEFTSEMI_TD_2734[i] = krnlEngine(program_h, q_h, "gqeJoin");
    }
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_LEFTSEMI_TD_2734[i].setup(tbl_JOIN_INNER_TD_3765_output_partition_array[i], tbl_SerializeFromObject_TD_3999_input_stringRowIDSubstitute_partition_array[i], tbl_JOIN_LEFTSEMI_TD_2734_output_partition_array[i], cfg_JOIN_LEFTSEMI_TD_2734_cmds, buftmp_h);
    }
    krnlEngine krnl_JOIN_LEFTSEMI_TD_2734_build;
    krnl_JOIN_LEFTSEMI_TD_2734_build = krnlEngine(program_h, q_h, context_h, "gqeKernel", 0);
    krnl_JOIN_LEFTSEMI_TD_2734_build.setup(tbl_JOIN_INNER_TD_3765_output, tbl_SerializeFromObject_TD_3999_input_stringRowIDSubstitute, tbl_JOIN_LEFTSEMI_TD_2734_output, cfg_JOIN_LEFTSEMI_TD_2734_cmds_build, buftmp_h);
    krnlEngine krnl_JOIN_LEFTSEMI_TD_2734_probe;
    krnl_JOIN_LEFTSEMI_TD_2734_probe = krnlEngine(program_h, q_h, context_h, "gqeKernel", 1);
    krnl_JOIN_LEFTSEMI_TD_2734_probe.setup(tbl_SerializeFromObject_TD_3999_input_stringRowIDSubstitute, tbl_JOIN_INNER_TD_3765_output, tbl_JOIN_LEFTSEMI_TD_2734_output, cfg_JOIN_LEFTSEMI_TD_2734_cmds_probe, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_3765_part_right;
    krnl_JOIN_INNER_TD_3765_part_right = krnlEngine(program_h, q_h, context_h, "gqePart");
    krnl_JOIN_INNER_TD_3765_part_right.setup_hp(512, 1, power_of_hpTimes_join, tbl_Filter_TD_4268_output, tbl_Filter_TD_4268_output_partition, cfg_JOIN_INNER_TD_3765_cmds_part);
    krnlEngine krnl_JOIN_INNER_TD_3765[hpTimes_join];
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_INNER_TD_3765[i] = krnlEngine(program_h, q_h, "gqeJoin");
    }
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_INNER_TD_3765[i].setup(tbl_JOIN_LEFTSEMI_TD_4447_output_partition_array[i], tbl_Filter_TD_4268_output_partition_array[i], tbl_JOIN_INNER_TD_3765_output_partition_array[i], cfg_JOIN_INNER_TD_3765_cmds, buftmp_h);
    }
    krnlEngine krnl_JOIN_INNER_TD_3765_build;
    krnl_JOIN_INNER_TD_3765_build = krnlEngine(program_h, q_h, context_h, "gqeKernel", 0);
    krnl_JOIN_INNER_TD_3765_build.setup(tbl_JOIN_LEFTSEMI_TD_4447_output, tbl_Filter_TD_4268_output, tbl_JOIN_INNER_TD_3765_output, cfg_JOIN_INNER_TD_3765_cmds_build, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_3765_probe;
    krnl_JOIN_INNER_TD_3765_probe = krnlEngine(program_h, q_h, context_h, "gqeKernel", 1);
    krnl_JOIN_INNER_TD_3765_probe.setup(tbl_Filter_TD_4268_output, tbl_JOIN_LEFTSEMI_TD_4447_output, tbl_JOIN_INNER_TD_3765_output, cfg_JOIN_INNER_TD_3765_cmds_probe, buftmp_h);
    krnlEngine krnl_JOIN_LEFTSEMI_TD_4447_part_left;
    krnl_JOIN_LEFTSEMI_TD_4447_part_left = krnlEngine(program_h, q_h, context_h, "gqePart");
    krnl_JOIN_LEFTSEMI_TD_4447_part_left.setup_hp(512, 0, power_of_hpTimes_join, tbl_Filter_TD_5752_output, tbl_Filter_TD_5752_output_partition, cfg_JOIN_LEFTSEMI_TD_4447_cmds_part);
    krnlEngine krnl_JOIN_LEFTSEMI_TD_4447_part_right;
    krnl_JOIN_LEFTSEMI_TD_4447_part_right = krnlEngine(program_h, q_h, context_h, "gqePart");
    krnl_JOIN_LEFTSEMI_TD_4447_part_right.setup_hp(512, 1, power_of_hpTimes_join, tbl_SerializeFromObject_TD_5882_input, tbl_SerializeFromObject_TD_5882_input_partition, cfg_JOIN_LEFTSEMI_TD_4447_cmds_part);
    krnlEngine krnl_JOIN_LEFTSEMI_TD_4447[hpTimes_join];
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_LEFTSEMI_TD_4447[i] = krnlEngine(program_h, q_h, "gqeJoin");
    }
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_LEFTSEMI_TD_4447[i].setup(tbl_Filter_TD_5752_output_partition_array[i], tbl_SerializeFromObject_TD_5882_input_partition_array[i], tbl_JOIN_LEFTSEMI_TD_4447_output_partition_array[i], cfg_JOIN_LEFTSEMI_TD_4447_cmds, buftmp_h);
    }
    krnlEngine krnl_JOIN_LEFTSEMI_TD_4447_build;
    krnl_JOIN_LEFTSEMI_TD_4447_build = krnlEngine(program_h, q_h, context_h, "gqeKernel", 0);
    krnl_JOIN_LEFTSEMI_TD_4447_build.setup(tbl_Filter_TD_5752_output, tbl_SerializeFromObject_TD_5882_input, tbl_JOIN_LEFTSEMI_TD_4447_output, cfg_JOIN_LEFTSEMI_TD_4447_cmds_build, buftmp_h);
    krnlEngine krnl_JOIN_LEFTSEMI_TD_4447_probe;
    krnl_JOIN_LEFTSEMI_TD_4447_probe = krnlEngine(program_h, q_h, context_h, "gqeKernel", 1);
    krnl_JOIN_LEFTSEMI_TD_4447_probe.setup(tbl_SerializeFromObject_TD_5882_input, tbl_Filter_TD_5752_output, tbl_JOIN_LEFTSEMI_TD_4447_output, cfg_JOIN_LEFTSEMI_TD_4447_cmds_probe, buftmp_h);
    krnlEngine krnl_JOIN_LEFTSEMI_TD_6611_part_left;
    krnl_JOIN_LEFTSEMI_TD_6611_part_left = krnlEngine(program_h, q_h, context_h, "gqePart");
    krnl_JOIN_LEFTSEMI_TD_6611_part_left.setup_hp(512, 0, power_of_hpTimes_join, tbl_Filter_TD_7414_output, tbl_Filter_TD_7414_output_partition, cfg_JOIN_LEFTSEMI_TD_6611_cmds_part);
    krnlEngine krnl_JOIN_LEFTSEMI_TD_6611_part_right;
    krnl_JOIN_LEFTSEMI_TD_6611_part_right = krnlEngine(program_h, q_h, context_h, "gqePart");
    krnl_JOIN_LEFTSEMI_TD_6611_part_right.setup_hp(512, 1, power_of_hpTimes_join, tbl_Filter_TD_7742_output, tbl_Filter_TD_7742_output_partition, cfg_JOIN_LEFTSEMI_TD_6611_cmds_part);
    krnlEngine krnl_JOIN_LEFTSEMI_TD_6611[hpTimes_join];
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_LEFTSEMI_TD_6611[i] = krnlEngine(program_h, q_h, "gqeJoin");
    }
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_LEFTSEMI_TD_6611[i].setup(tbl_Filter_TD_7414_output_partition_array[i], tbl_Filter_TD_7742_output_partition_array[i], tbl_JOIN_LEFTSEMI_TD_6611_output_partition_array[i], cfg_JOIN_LEFTSEMI_TD_6611_cmds, buftmp_h);
    }
    krnlEngine krnl_JOIN_LEFTSEMI_TD_6611_build;
    krnl_JOIN_LEFTSEMI_TD_6611_build = krnlEngine(program_h, q_h, context_h, "gqeKernel", 0);
    krnl_JOIN_LEFTSEMI_TD_6611_build.setup(tbl_Filter_TD_7414_output, tbl_Filter_TD_7742_output, tbl_JOIN_LEFTSEMI_TD_6611_output, cfg_JOIN_LEFTSEMI_TD_6611_cmds_build, buftmp_h);
    krnlEngine krnl_JOIN_LEFTSEMI_TD_6611_probe;
    krnl_JOIN_LEFTSEMI_TD_6611_probe = krnlEngine(program_h, q_h, context_h, "gqeKernel", 1);
    krnl_JOIN_LEFTSEMI_TD_6611_probe.setup(tbl_Filter_TD_7742_output, tbl_Filter_TD_7414_output, tbl_JOIN_LEFTSEMI_TD_6611_output, cfg_JOIN_LEFTSEMI_TD_6611_cmds_probe, buftmp_h);
    // ************************** Transfer Engine *********************** // 
    transEngine trans_JOIN_INNER_TD_1646;
    trans_JOIN_INNER_TD_1646.setq(q_h,0);
    trans_JOIN_INNER_TD_1646.add(&(cfg_JOIN_INNER_TD_1646_cmds_part));
    trans_JOIN_INNER_TD_1646.add(&(cfg_JOIN_INNER_TD_1646_cmds));
    transEngine trans_JOIN_INNER_TD_1646_out;
    trans_JOIN_INNER_TD_1646_out.setq(q_h,0);
    q_h.finish();
    transEngine trans_JOIN_LEFTSEMI_TD_2734;
    trans_JOIN_LEFTSEMI_TD_2734.setq(q_h,0);
    trans_JOIN_LEFTSEMI_TD_2734.add(&(cfg_JOIN_LEFTSEMI_TD_2734_cmds_part));
    trans_JOIN_LEFTSEMI_TD_2734.add(&(cfg_JOIN_LEFTSEMI_TD_2734_cmds));
    trans_JOIN_LEFTSEMI_TD_2734.add(&(tbl_SerializeFromObject_TD_3999_input_stringRowIDSubstitute));
    q_h.finish();
    transEngine trans_JOIN_INNER_TD_3765;
    trans_JOIN_INNER_TD_3765.setq(q_h,0);
    trans_JOIN_INNER_TD_3765.add(&(cfg_JOIN_INNER_TD_3765_cmds_part));
    trans_JOIN_INNER_TD_3765.add(&(cfg_JOIN_INNER_TD_3765_cmds));
    q_h.finish();
    transEngine trans_JOIN_LEFTSEMI_TD_4447;
    trans_JOIN_LEFTSEMI_TD_4447.setq(q_h,0);
    trans_JOIN_LEFTSEMI_TD_4447.add(&(cfg_JOIN_LEFTSEMI_TD_4447_cmds_part));
    trans_JOIN_LEFTSEMI_TD_4447.add(&(cfg_JOIN_LEFTSEMI_TD_4447_cmds));
    trans_JOIN_LEFTSEMI_TD_4447.add(&(tbl_SerializeFromObject_TD_5882_input));
    q_h.finish();
    transEngine trans_JOIN_LEFTSEMI_TD_6611;
    trans_JOIN_LEFTSEMI_TD_6611.setq(q_h,0);
    trans_JOIN_LEFTSEMI_TD_6611.add(&(cfg_JOIN_LEFTSEMI_TD_6611_cmds));
    transEngine trans_JOIN_LEFTSEMI_TD_6611_out;
    trans_JOIN_LEFTSEMI_TD_6611_out.setq(q_h,0);
    q_h.finish();
    // ****************************** Events **************************** // 
    std::vector<cl::Event> events_h2d_wr_JOIN_INNER_TD_1646;
    std::vector<cl::Event> events_d2h_rd_JOIN_INNER_TD_1646;
    std::vector<cl::Event> events_JOIN_INNER_TD_1646[2];
    events_h2d_wr_JOIN_INNER_TD_1646.resize(1);
    events_d2h_rd_JOIN_INNER_TD_1646.resize(1);
    events_JOIN_INNER_TD_1646[0].resize(1);
    events_JOIN_INNER_TD_1646[1].resize(hpTimes_join);
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_1646;
    std::vector<cl::Event> prev_events_grp_JOIN_INNER_TD_1646;
    std::vector<cl::Event> events_h2d_wr_JOIN_LEFTSEMI_TD_2734;
    std::vector<cl::Event> events_d2h_rd_JOIN_LEFTSEMI_TD_2734;
    std::vector<cl::Event> events_JOIN_LEFTSEMI_TD_2734[2];
    events_h2d_wr_JOIN_LEFTSEMI_TD_2734.resize(1);
    events_d2h_rd_JOIN_LEFTSEMI_TD_2734.resize(1);
    events_JOIN_LEFTSEMI_TD_2734[0].resize(1);
    events_JOIN_LEFTSEMI_TD_2734[1].resize(hpTimes_join);
    std::vector<cl::Event> events_grp_JOIN_LEFTSEMI_TD_2734;
    std::vector<cl::Event> prev_events_grp_JOIN_LEFTSEMI_TD_2734;
    std::vector<cl::Event> events_h2d_wr_JOIN_INNER_TD_3765;
    std::vector<cl::Event> events_d2h_rd_JOIN_INNER_TD_3765;
    std::vector<cl::Event> events_JOIN_INNER_TD_3765[2];
    events_h2d_wr_JOIN_INNER_TD_3765.resize(1);
    events_d2h_rd_JOIN_INNER_TD_3765.resize(1);
    events_JOIN_INNER_TD_3765[0].resize(1);
    events_JOIN_INNER_TD_3765[1].resize(hpTimes_join);
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_3765;
    std::vector<cl::Event> prev_events_grp_JOIN_INNER_TD_3765;
    std::vector<cl::Event> events_h2d_wr_JOIN_LEFTSEMI_TD_4447;
    std::vector<cl::Event> events_d2h_rd_JOIN_LEFTSEMI_TD_4447;
    std::vector<cl::Event> events_JOIN_LEFTSEMI_TD_4447[2];
    events_h2d_wr_JOIN_LEFTSEMI_TD_4447.resize(1);
    events_d2h_rd_JOIN_LEFTSEMI_TD_4447.resize(1);
    events_JOIN_LEFTSEMI_TD_4447[0].resize(2);
    events_JOIN_LEFTSEMI_TD_4447[1].resize(hpTimes_join);
    std::vector<cl::Event> events_grp_JOIN_LEFTSEMI_TD_4447;
    std::vector<cl::Event> prev_events_grp_JOIN_LEFTSEMI_TD_4447;
    std::vector<cl::Event> events_h2d_wr_JOIN_LEFTSEMI_TD_6611;
    std::vector<cl::Event> events_d2h_rd_JOIN_LEFTSEMI_TD_6611;
    std::vector<cl::Event> events_JOIN_LEFTSEMI_TD_6611[2];
    events_h2d_wr_JOIN_LEFTSEMI_TD_6611.resize(1);
    events_d2h_rd_JOIN_LEFTSEMI_TD_6611.resize(1);
    events_JOIN_LEFTSEMI_TD_6611[0].resize(2);
    events_JOIN_LEFTSEMI_TD_6611[1].resize(hpTimes_join);
    std::vector<cl::Event> events_grp_JOIN_LEFTSEMI_TD_6611;
    std::vector<cl::Event> prev_events_grp_JOIN_LEFTSEMI_TD_6611;
    // **************************** Operations ************************** // 
    struct timeval tv_r_s, tv_r_e; 
    gettimeofday(&tv_r_s, 0); 

    struct timeval tv_r_Filter_7_807_s, tv_r_Filter_7_807_e;
    gettimeofday(&tv_r_Filter_7_807_s, 0);
    SW_Filter_TD_7414(tbl_SerializeFromObject_TD_820_input, tbl_Filter_TD_7414_output);
    gettimeofday(&tv_r_Filter_7_807_e, 0);

    struct timeval tv_r_Filter_7_689_s, tv_r_Filter_7_689_e;
    gettimeofday(&tv_r_Filter_7_689_s, 0);
    SW_Filter_TD_7742(tbl_SerializeFromObject_TD_8258_input, tbl_Filter_TD_7742_output);
    gettimeofday(&tv_r_Filter_7_689_e, 0);

    struct timeval tv_r_JOIN_LEFTSEMI_6_1_s, tv_r_JOIN_LEFTSEMI_6_1_e;
    gettimeofday(&tv_r_JOIN_LEFTSEMI_6_1_s, 0);
    trans_JOIN_LEFTSEMI_TD_6611.add(&(tbl_Filter_TD_7742_output));
    trans_JOIN_LEFTSEMI_TD_6611.add(&(tbl_Filter_TD_7414_output));
    trans_JOIN_LEFTSEMI_TD_6611.host2dev(0, &(prev_events_grp_JOIN_LEFTSEMI_TD_6611), &(events_h2d_wr_JOIN_LEFTSEMI_TD_6611[0]));
    events_grp_JOIN_LEFTSEMI_TD_6611.push_back(events_h2d_wr_JOIN_LEFTSEMI_TD_6611[0]);
    krnl_JOIN_LEFTSEMI_TD_6611_part_left.run(0, &(events_grp_JOIN_LEFTSEMI_TD_6611), &(events_JOIN_LEFTSEMI_TD_6611[0][0]));
    krnl_JOIN_LEFTSEMI_TD_6611_part_right.run(0, &(events_grp_JOIN_LEFTSEMI_TD_6611), &(events_JOIN_LEFTSEMI_TD_6611[0][1]));
    for (int i(0); i < hpTimes_join; ++i) {
        krnl_JOIN_LEFTSEMI_TD_6611[i].run(0, &(events_JOIN_LEFTSEMI_TD_6611[0]), &(events_JOIN_LEFTSEMI_TD_6611[1][i]));
    }
    for (int i(0); i < hpTimes_join; ++i) {
        trans_JOIN_LEFTSEMI_TD_6611_out.add(&(tbl_JOIN_LEFTSEMI_TD_6611_output_partition_array[i]));
    }
    trans_JOIN_LEFTSEMI_TD_6611_out.dev2host(0, &(events_JOIN_LEFTSEMI_TD_6611[1]), &(events_d2h_rd_JOIN_LEFTSEMI_TD_6611[0]));
    q_h.flush();
    q_h.finish();
    gettimeofday(&tv_r_JOIN_LEFTSEMI_6_1_e, 0);

    struct timeval tv_r_Aggregate_5_339_s, tv_r_Aggregate_5_339_e;
    gettimeofday(&tv_r_Aggregate_5_339_s, 0);
    SW_Aggregate_TD_5897(tbl_JOIN_LEFTSEMI_TD_6611_output_partition_array, tbl_Aggregate_TD_5897_output, hpTimes_join);
    gettimeofday(&tv_r_Aggregate_5_339_e, 0);

    struct timeval tv_r_Filter_5_930_s, tv_r_Filter_5_930_e;
    gettimeofday(&tv_r_Filter_5_930_s, 0);
    SW_Filter_TD_5752(tbl_SerializeFromObject_TD_6680_input, tbl_Filter_TD_5752_output);
    gettimeofday(&tv_r_Filter_5_930_e, 0);

    struct timeval tv_r_Filter_4_737_s, tv_r_Filter_4_737_e;
    gettimeofday(&tv_r_Filter_4_737_s, 0);
    SW_Filter_TD_4268(tbl_Aggregate_TD_5897_output, tbl_Filter_TD_4268_output);
    gettimeofday(&tv_r_Filter_4_737_e, 0);

    struct timeval tv_r_JOIN_LEFTSEMI_4_386_s, tv_r_JOIN_LEFTSEMI_4_386_e;
    gettimeofday(&tv_r_JOIN_LEFTSEMI_4_386_s, 0);
    trans_JOIN_LEFTSEMI_TD_4447.add(&(tbl_Filter_TD_5752_output));
    trans_JOIN_LEFTSEMI_TD_4447.host2dev(0, &(prev_events_grp_JOIN_LEFTSEMI_TD_4447), &(events_h2d_wr_JOIN_LEFTSEMI_TD_4447[0]));
    events_grp_JOIN_LEFTSEMI_TD_4447.push_back(events_h2d_wr_JOIN_LEFTSEMI_TD_4447[0]);
    krnl_JOIN_LEFTSEMI_TD_4447_part_left.run(0, &(events_grp_JOIN_LEFTSEMI_TD_4447), &(events_JOIN_LEFTSEMI_TD_4447[0][0]));
    krnl_JOIN_LEFTSEMI_TD_4447_part_right.run(0, &(events_grp_JOIN_LEFTSEMI_TD_4447), &(events_JOIN_LEFTSEMI_TD_4447[0][1]));
    for (int i(0); i < hpTimes_join; ++i) {
        krnl_JOIN_LEFTSEMI_TD_4447[i].run(0, &(events_JOIN_LEFTSEMI_TD_4447[0]), &(events_JOIN_LEFTSEMI_TD_4447[1][i]));
    }
    gettimeofday(&tv_r_JOIN_LEFTSEMI_4_386_e, 0);

    struct timeval tv_r_JOIN_INNER_3_825_s, tv_r_JOIN_INNER_3_825_e;
    gettimeofday(&tv_r_JOIN_INNER_3_825_s, 0);
    prev_events_grp_JOIN_INNER_TD_3765.push_back(events_h2d_wr_JOIN_LEFTSEMI_TD_4447[0]);
    trans_JOIN_INNER_TD_3765.add(&(tbl_Filter_TD_4268_output));
    trans_JOIN_INNER_TD_3765.host2dev(0, &(prev_events_grp_JOIN_INNER_TD_3765), &(events_h2d_wr_JOIN_INNER_TD_3765[0]));
    events_grp_JOIN_INNER_TD_3765.push_back(events_h2d_wr_JOIN_INNER_TD_3765[0]);
    events_grp_JOIN_INNER_TD_3765.insert(std::end(events_grp_JOIN_INNER_TD_3765), std::begin(events_JOIN_LEFTSEMI_TD_4447[0]), std::end(events_JOIN_LEFTSEMI_TD_4447[0]));
    events_grp_JOIN_INNER_TD_3765.insert(std::end(events_grp_JOIN_INNER_TD_3765), std::begin(events_JOIN_LEFTSEMI_TD_4447[1]), std::end(events_JOIN_LEFTSEMI_TD_4447[1]));
    krnl_JOIN_INNER_TD_3765_part_right.run(0, &(events_grp_JOIN_INNER_TD_3765), &(events_JOIN_INNER_TD_3765[0][0]));
    for (int i(0); i < hpTimes_join; ++i) {
        krnl_JOIN_INNER_TD_3765[i].run(0, &(events_grp_JOIN_INNER_TD_3765), &(events_JOIN_INNER_TD_3765[1][i]));
    }
    gettimeofday(&tv_r_JOIN_INNER_3_825_e, 0);

    struct timeval tv_r_Filter_2_316_s, tv_r_Filter_2_316_e;
    gettimeofday(&tv_r_Filter_2_316_s, 0);
    SW_Filter_TD_2986(tbl_SerializeFromObject_TD_3371_input, tbl_Filter_TD_2986_output);
    gettimeofday(&tv_r_Filter_2_316_e, 0);

    struct timeval tv_r_JOIN_LEFTSEMI_2_437_s, tv_r_JOIN_LEFTSEMI_2_437_e;
    gettimeofday(&tv_r_JOIN_LEFTSEMI_2_437_s, 0);
    prev_events_grp_JOIN_LEFTSEMI_TD_2734.push_back(events_h2d_wr_JOIN_INNER_TD_3765[0]);
    trans_JOIN_LEFTSEMI_TD_2734.host2dev(0, &(prev_events_grp_JOIN_LEFTSEMI_TD_2734), &(events_h2d_wr_JOIN_LEFTSEMI_TD_2734[0]));
    events_grp_JOIN_LEFTSEMI_TD_2734.push_back(events_h2d_wr_JOIN_LEFTSEMI_TD_2734[0]);
    events_grp_JOIN_LEFTSEMI_TD_2734.insert(std::end(events_grp_JOIN_LEFTSEMI_TD_2734), std::begin(events_JOIN_INNER_TD_3765[0]), std::end(events_JOIN_INNER_TD_3765[0]));
    events_grp_JOIN_LEFTSEMI_TD_2734.insert(std::end(events_grp_JOIN_LEFTSEMI_TD_2734), std::begin(events_JOIN_INNER_TD_3765[1]), std::end(events_JOIN_INNER_TD_3765[1]));
    krnl_JOIN_LEFTSEMI_TD_2734_part_right.run(0, &(events_grp_JOIN_LEFTSEMI_TD_2734), &(events_JOIN_LEFTSEMI_TD_2734[0][0]));
    for (int i(0); i < hpTimes_join; ++i) {
        krnl_JOIN_LEFTSEMI_TD_2734[i].run(0, &(events_grp_JOIN_LEFTSEMI_TD_2734), &(events_JOIN_LEFTSEMI_TD_2734[1][i]));
    }
    gettimeofday(&tv_r_JOIN_LEFTSEMI_2_437_e, 0);

    struct timeval tv_r_JOIN_INNER_1_483_s, tv_r_JOIN_INNER_1_483_e;
    gettimeofday(&tv_r_JOIN_INNER_1_483_s, 0);
    prev_events_grp_JOIN_INNER_TD_1646.push_back(events_h2d_wr_JOIN_LEFTSEMI_TD_2734[0]);
    trans_JOIN_INNER_TD_1646.add(&(tbl_Filter_TD_2986_output));
    trans_JOIN_INNER_TD_1646.host2dev(0, &(prev_events_grp_JOIN_INNER_TD_1646), &(events_h2d_wr_JOIN_INNER_TD_1646[0]));
    events_grp_JOIN_INNER_TD_1646.push_back(events_h2d_wr_JOIN_INNER_TD_1646[0]);
    events_grp_JOIN_INNER_TD_1646.insert(std::end(events_grp_JOIN_INNER_TD_1646), std::begin(events_JOIN_LEFTSEMI_TD_2734[0]), std::end(events_JOIN_LEFTSEMI_TD_2734[0]));
    events_grp_JOIN_INNER_TD_1646.insert(std::end(events_grp_JOIN_INNER_TD_1646), std::begin(events_JOIN_LEFTSEMI_TD_2734[1]), std::end(events_JOIN_LEFTSEMI_TD_2734[1]));
    krnl_JOIN_INNER_TD_1646_part_right.run(0, &(events_grp_JOIN_INNER_TD_1646), &(events_JOIN_INNER_TD_1646[0][0]));
    for (int i(0); i < hpTimes_join; ++i) {
        krnl_JOIN_INNER_TD_1646[i].run(0, &(events_grp_JOIN_INNER_TD_1646), &(events_JOIN_INNER_TD_1646[1][i]));
    }
    for (int i(0); i < hpTimes_join; ++i) {
        trans_JOIN_INNER_TD_1646_out.add(&(tbl_JOIN_INNER_TD_1646_output_partition_array[i]));
    }
    trans_JOIN_INNER_TD_1646_out.dev2host(0, &(events_JOIN_INNER_TD_1646[1]), &(events_d2h_rd_JOIN_INNER_TD_1646[0]));
    q_h.flush();
    q_h.finish();
    gettimeofday(&tv_r_JOIN_INNER_1_483_e, 0);

    struct timeval tv_r_Sort_0_110_s, tv_r_Sort_0_110_e;
    gettimeofday(&tv_r_Sort_0_110_s, 0);
    SW_Sort_TD_0516(tbl_JOIN_INNER_TD_1646_output_partition_array, tbl_SerializeFromObject_TD_3999_input, tbl_SerializeFromObject_TD_3371_input, tbl_Sort_TD_0516_output, hpTimes_join);
    gettimeofday(&tv_r_Sort_0_110_e, 0);

std::cout << "CPU TIME: " << (tvdiff(&tv_r_Filter_7_807_s, &tv_r_Filter_7_807_e) + 
tvdiff(&tv_r_Filter_7_689_s, &tv_r_Filter_7_689_e) + 
tvdiff(&tv_r_Aggregate_5_339_s, &tv_r_Aggregate_5_339_e) + 
tvdiff(&tv_r_Filter_5_930_s, &tv_r_Filter_5_930_e) + 
tvdiff(&tv_r_Filter_4_737_s, &tv_r_Filter_4_737_e) + 
tvdiff(&tv_r_Filter_2_316_s, &tv_r_Filter_2_316_e) + 
tvdiff(&tv_r_Sort_0_110_s, &tv_r_Sort_0_110_e) + 
1) / 1000.0 << std::endl;

std::cout << "FPGA TIME: " << (tvdiff(&tv_r_JOIN_LEFTSEMI_6_1_s, &tv_r_JOIN_LEFTSEMI_6_1_e) + 
tvdiff(&tv_r_JOIN_LEFTSEMI_4_386_s, &tv_r_JOIN_LEFTSEMI_4_386_e) + 
tvdiff(&tv_r_JOIN_INNER_3_825_s, &tv_r_JOIN_INNER_3_825_e) + 
tvdiff(&tv_r_JOIN_LEFTSEMI_2_437_s, &tv_r_JOIN_LEFTSEMI_2_437_e) + 
tvdiff(&tv_r_JOIN_INNER_1_483_s, &tv_r_JOIN_INNER_1_483_e) + 
1) / 1000.0 << std::endl;

    gettimeofday(&tv_r_e, 0); 
    // **************************** Print Execution Time ************************** // 
    std::cout << std::endl; 
    std::cout << "Filter_7: " << tvdiff(&tv_r_Filter_7_807_s, &tv_r_Filter_7_807_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer((isnotnull(p_name#259) AND StartsWith(p_name#259, blush))) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_820_input.getNumRow() << " -> tbl_SerializeFromObject_TD_820_input" 
     << std::endl << "      #COL: 2: " << "ListBuffer(p_partkey#258, p_name#259)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_7414_output.getNumRow() << " -> tbl_Filter_TD_7414_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(p_partkey#258)" 
     << std::endl; 

    std::cout << "Filter_7: " << tvdiff(&tv_r_Filter_7_689_s, &tv_r_Filter_7_689_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(((l_shipdate#87 >= 19950101) AND (l_shipdate#87 < 19960101))) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_8258_input.getNumRow() << " -> tbl_SerializeFromObject_TD_8258_input" 
     << std::endl << "      #COL: 4: " << "ListBuffer(l_partkey#78, l_suppkey#79, l_quantity#81, l_shipdate#87)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_7742_output.getNumRow() << " -> tbl_Filter_TD_7742_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(l_partkey#78, l_suppkey#79, l_quantity#81)" 
     << std::endl; 

    std::cout << "JOIN_LEFTSEMI_6: " << tvdiff(&tv_r_JOIN_LEFTSEMI_6_1_s, &tv_r_JOIN_LEFTSEMI_6_1_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer((l_partkey#78 = p_partkey#258)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_7742_output.getNumRow() << " -> tbl_Filter_TD_7742_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(l_partkey#78, l_suppkey#79, l_quantity#81)" 
     << std::endl << "      #ROW: " << tbl_Filter_TD_7414_output.getNumRow() << " -> tbl_Filter_TD_7414_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(p_partkey#258)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_LEFTSEMI_TD_6611_output.getNumRow() << " -> tbl_JOIN_LEFTSEMI_TD_6611_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(l_partkey#78, l_suppkey#79, l_quantity#81)" 
     << std::endl; 

    std::cout << "Aggregate_5: " << tvdiff(&tv_r_Aggregate_5_339_s, &tv_r_Aggregate_5_339_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(l_partkey#78, l_suppkey#79, sum(l_quantity#81) AS sum(l_quantity)#1854L) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_LEFTSEMI_TD_6611_output.getNumRow() << " -> tbl_JOIN_LEFTSEMI_TD_6611_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(l_partkey#78, l_suppkey#79, l_quantity#81)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_5897_output.getNumRow() << " -> tbl_Aggregate_TD_5897_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(sum(l_quantity)#1854L, l_partkey#78, l_suppkey#79)" 
     << std::endl; 

    std::cout << "Filter_5: " << tvdiff(&tv_r_Filter_5_930_s, &tv_r_Filter_5_930_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer((isnotnull(p_name#259) AND StartsWith(p_name#259, blush))) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_6680_input.getNumRow() << " -> tbl_SerializeFromObject_TD_6680_input" 
     << std::endl << "      #COL: 2: " << "ListBuffer(p_partkey#258, p_name#259)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_5752_output.getNumRow() << " -> tbl_Filter_TD_5752_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(p_partkey#258)" 
     << std::endl; 

    std::cout << "Filter_4: " << tvdiff(&tv_r_Filter_4_737_s, &tv_r_Filter_4_737_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(isnotnull(sum(l_quantity)#1854L)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_5897_output.getNumRow() << " -> tbl_Aggregate_TD_5897_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(sum(l_quantity)#1854L, l_partkey#78, l_suppkey#79)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_4268_output.getNumRow() << " -> tbl_Filter_TD_4268_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(sum(l_quantity)#1854L, l_partkey#78, l_suppkey#79)" 
     << std::endl; 

    std::cout << "JOIN_LEFTSEMI_4: " << tvdiff(&tv_r_JOIN_LEFTSEMI_4_386_s, &tv_r_JOIN_LEFTSEMI_4_386_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer((ps_partkey#305 = p_partkey#258)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_5882_input.getNumRow() << " -> tbl_SerializeFromObject_TD_5882_input" 
     << std::endl << "      #COL: 3: " << "ListBuffer(ps_partkey#305, ps_suppkey#306, ps_availqty#307)" 
     << std::endl << "      #ROW: " << tbl_Filter_TD_5752_output.getNumRow() << " -> tbl_Filter_TD_5752_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(p_partkey#258)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_LEFTSEMI_TD_4447_output.getNumRow() << " -> tbl_JOIN_LEFTSEMI_TD_4447_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(ps_partkey#305, ps_suppkey#306, ps_availqty#307)" 
     << std::endl; 

    std::cout << "JOIN_INNER_3: " << tvdiff(&tv_r_JOIN_INNER_3_825_s, &tv_r_JOIN_INNER_3_825_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer((((cast(ps_availqty#307 as bigint) > sum(l_quantity)#1854L) AND (l_partkey#78 = ps_partkey#305)) AND (l_suppkey#79 = ps_suppkey#306))) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_LEFTSEMI_TD_4447_output.getNumRow() << " -> tbl_JOIN_LEFTSEMI_TD_4447_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(ps_partkey#305, ps_suppkey#306, ps_availqty#307)" 
     << std::endl << "      #ROW: " << tbl_Filter_TD_4268_output.getNumRow() << " -> tbl_Filter_TD_4268_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(sum(l_quantity)#1854L, l_partkey#78, l_suppkey#79)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_3765_output.getNumRow() << " -> tbl_JOIN_INNER_TD_3765_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(ps_suppkey#306)" 
     << std::endl; 

    std::cout << "Filter_2: " << tvdiff(&tv_r_Filter_2_316_s, &tv_r_Filter_2_316_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer((isnotnull(n_name#145) AND (n_name#145 = INDONESIA))) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_3371_input.getNumRow() << " -> tbl_SerializeFromObject_TD_3371_input" 
     << std::endl << "      #COL: 2: " << "ListBuffer(n_nationkey#144, n_name#145)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_2986_output.getNumRow() << " -> tbl_Filter_TD_2986_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(n_nationkey#144)" 
     << std::endl; 

    std::cout << "JOIN_LEFTSEMI_2: " << tvdiff(&tv_r_JOIN_LEFTSEMI_2_437_s, &tv_r_JOIN_LEFTSEMI_2_437_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer((s_suppkey#342 = ps_suppkey#306)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_3999_input_stringRowIDSubstitute.getNumRow() << " -> tbl_SerializeFromObject_TD_3999_input_stringRowIDSubstitute" 
     << std::endl << "      #COL: 4: " << "ListBuffer(s_suppkey#342, s_name#343, s_address#344, s_nationkey#345)" 
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_3765_output.getNumRow() << " -> tbl_JOIN_INNER_TD_3765_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(ps_suppkey#306)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_LEFTSEMI_TD_2734_output.getNumRow() << " -> tbl_JOIN_LEFTSEMI_TD_2734_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(s_name#343, s_address#344, s_nationkey#345)" 
     << std::endl; 

    std::cout << "JOIN_INNER_1: " << tvdiff(&tv_r_JOIN_INNER_1_483_s, &tv_r_JOIN_INNER_1_483_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer((s_nationkey#345 = n_nationkey#144)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_LEFTSEMI_TD_2734_output.getNumRow() << " -> tbl_JOIN_LEFTSEMI_TD_2734_output" 
     << std::endl << "      #COL: 3: " << "ListBuffer(s_name#343, s_address#344, s_nationkey#345)" 
     << std::endl << "      #ROW: " << tbl_Filter_TD_2986_output.getNumRow() << " -> tbl_Filter_TD_2986_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(n_nationkey#144)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_1646_output.getNumRow() << " -> tbl_JOIN_INNER_TD_1646_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(s_name#343, s_address#344)" 
     << std::endl; 

    std::cout << "Sort_0: " << tvdiff(&tv_r_Sort_0_110_s, &tv_r_Sort_0_110_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(s_name#343 ASC NULLS FIRST) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_1646_output.getNumRow() << " -> tbl_JOIN_INNER_TD_1646_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(s_name#343, s_address#344)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Sort_TD_0516_output.getNumRow() << " -> tbl_Sort_TD_0516_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(s_name#343, s_address#344)" 
     << std::endl; 

    std::cout << std::endl << " Total execution time: " << tvdiff(&tv_r_s, &tv_r_e) / 1000 << " ms"; 

    std::cout << std::endl << " Spark elapsed time: " << 21.283146 * 1000 << "ms" << std::endl; 
    return 0; 
}
