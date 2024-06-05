// number of overlays (w/o fusion): 6 
// number of overlays (w/ fusion): 6 
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

#include "cfgFunc_q21.hpp" 
#include "q21.hpp" 

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
    std::cout << "NOTE:running query #21\n."; 
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
    Table tbl_Sort_TD_092_output("tbl_Sort_TD_092_output", 183000000, 2, "");
    tbl_Sort_TD_092_output.allocateHost();
    Table tbl_Aggregate_TD_1858_output("tbl_Aggregate_TD_1858_output", 183000000, 2, "");
    tbl_Aggregate_TD_1858_output.allocateHost();
    Table tbl_JOIN_INNER_TD_2412_output("tbl_JOIN_INNER_TD_2412_output", 25255, 1, "");
    tbl_JOIN_INNER_TD_2412_output.allocateHost(1.2, hpTimes_join);
    Table tbl_Filter_TD_375_output("tbl_Filter_TD_375_output", 183000000, 1, "");
    tbl_Filter_TD_375_output.selectOverlayVersion(0);
    tbl_Filter_TD_375_output.allocateHost();
    Table tbl_Filter_TD_375_output_partition("tbl_Filter_TD_375_output_partition", 183000000, 1, "");
    tbl_Filter_TD_375_output_partition.allocateHost(1.2, hpTimes_join);
    tbl_Filter_TD_375_output_partition.allocateDevBuffer(context_h, 32);
    Table tbl_Filter_TD_375_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_Filter_TD_375_output_partition_array[i] = tbl_Filter_TD_375_output_partition.createSubTable(i);
    }
    Table tbl_JOIN_INNER_TD_4400_output("tbl_JOIN_INNER_TD_4400_output", 52212, 2, "");
    tbl_JOIN_INNER_TD_4400_output.allocateHost(1.2, hpTimes_join);
    Table tbl_SerializeFromObject_TD_5565_input;
    tbl_SerializeFromObject_TD_5565_input = Table("order", order_n, 2, in_dir);
    tbl_SerializeFromObject_TD_5565_input.addCol("o_orderkey", 4);
    tbl_SerializeFromObject_TD_5565_input.addCol("o_orderstatus", 4);
    tbl_SerializeFromObject_TD_5565_input.allocateHost();
    tbl_SerializeFromObject_TD_5565_input.loadHost();
    Table tbl_JOIN_INNER_TD_3311_output("tbl_JOIN_INNER_TD_3311_output", 377, 2, "");
    tbl_JOIN_INNER_TD_3311_output.allocateHost(1.2, hpTimes_join);
    Table tbl_JOIN_LEFTANTI_TD_5941_output("tbl_JOIN_LEFTANTI_TD_5941_output", 1375555, 2, "");
    tbl_JOIN_LEFTANTI_TD_5941_output.allocateHost(1.2, hpTimes_join);
    Table tbl_SerializeFromObject_TD_5109_input_stringRowIDSubstitute;
    tbl_SerializeFromObject_TD_5109_input_stringRowIDSubstitute = Table("supplier", supplier_n, 3, in_dir);
    tbl_SerializeFromObject_TD_5109_input_stringRowIDSubstitute.addCol("s_suppkey", 4);
    tbl_SerializeFromObject_TD_5109_input_stringRowIDSubstitute.addCol("s_name", 4, 1, 0);
    tbl_SerializeFromObject_TD_5109_input_stringRowIDSubstitute.addCol("s_nationkey", 4);
    tbl_SerializeFromObject_TD_5109_input_stringRowIDSubstitute.allocateHost();
    tbl_SerializeFromObject_TD_5109_input_stringRowIDSubstitute.loadHost();
    Table tbl_SerializeFromObject_TD_5109_input_stringRowIDSubstitute_partition("tbl_SerializeFromObject_TD_5109_input_stringRowIDSubstitute_partition", supplier_n, 3, "");
    tbl_SerializeFromObject_TD_5109_input_stringRowIDSubstitute_partition.allocateHost(1.2, hpTimes_join);
    tbl_SerializeFromObject_TD_5109_input_stringRowIDSubstitute_partition.allocateDevBuffer(context_h, 32);
    Table tbl_SerializeFromObject_TD_5109_input_stringRowIDSubstitute_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_SerializeFromObject_TD_5109_input_stringRowIDSubstitute_partition_array[i] = tbl_SerializeFromObject_TD_5109_input_stringRowIDSubstitute_partition.createSubTable(i);
    }
    Table tbl_SerializeFromObject_TD_5109_input;
    tbl_SerializeFromObject_TD_5109_input = Table("supplier", supplier_n, 3, in_dir);
    tbl_SerializeFromObject_TD_5109_input.addCol("s_suppkey", 4);
    tbl_SerializeFromObject_TD_5109_input.addCol("s_name", TPCH_READ_S_NAME_LEN+1);
    tbl_SerializeFromObject_TD_5109_input.addCol("s_nationkey", 4);
    tbl_SerializeFromObject_TD_5109_input.allocateHost();
    tbl_SerializeFromObject_TD_5109_input.loadHost();
    Table tbl_Filter_TD_3386_output("tbl_Filter_TD_3386_output", 183000000, 1, "");
    tbl_Filter_TD_3386_output.selectOverlayVersion(0);
    tbl_Filter_TD_3386_output.allocateHost();
    Table tbl_Filter_TD_3386_output_partition("tbl_Filter_TD_3386_output_partition", 183000000, 1, "");
    tbl_Filter_TD_3386_output_partition.allocateHost(1.2, hpTimes_join);
    tbl_Filter_TD_3386_output_partition.allocateDevBuffer(context_h, 32);
    Table tbl_Filter_TD_3386_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_Filter_TD_3386_output_partition_array[i] = tbl_Filter_TD_3386_output_partition.createSubTable(i);
    }
    Table tbl_JOIN_LEFTSEMI_TD_654_output("tbl_JOIN_LEFTSEMI_TD_654_output", 2417632, 2, "");
    tbl_JOIN_LEFTSEMI_TD_654_output.allocateHost(1.2, hpTimes_join);
    Table tbl_SerializeFromObject_TD_4666_input;
    tbl_SerializeFromObject_TD_4666_input = Table("nation", nation_n, 2, in_dir);
    tbl_SerializeFromObject_TD_4666_input.addCol("n_nationkey", 4);
    tbl_SerializeFromObject_TD_4666_input.addCol("n_name", TPCH_READ_NATION_LEN+1);
    tbl_SerializeFromObject_TD_4666_input.allocateHost();
    tbl_SerializeFromObject_TD_4666_input.loadHost();
    Table tbl_JOIN_LEFTSEMI_TD_778_output("tbl_JOIN_LEFTSEMI_TD_778_output", 2844584, 2, "");
    tbl_JOIN_LEFTSEMI_TD_778_output.allocateHost(1.2, hpTimes_join);
    Table tbl_Filter_TD_7456_output("tbl_Filter_TD_7456_output", 183000000, 2, "");
    tbl_Filter_TD_7456_output.selectOverlayVersion(0);
    tbl_Filter_TD_7456_output.allocateHost();
    Table tbl_Filter_TD_7456_output_partition("tbl_Filter_TD_7456_output_partition", 183000000, 2, "");
    tbl_Filter_TD_7456_output_partition.allocateHost(1.2, hpTimes_join);
    tbl_Filter_TD_7456_output_partition.allocateDevBuffer(context_h, 32);
    Table tbl_Filter_TD_7456_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_Filter_TD_7456_output_partition_array[i] = tbl_Filter_TD_7456_output_partition.createSubTable(i);
    }
    Table tbl_Filter_TD_8678_output("tbl_Filter_TD_8678_output", 183000000, 2, "");
    tbl_Filter_TD_8678_output.selectOverlayVersion(0);
    tbl_Filter_TD_8678_output.allocateHost();
    Table tbl_Filter_TD_8678_output_partition("tbl_Filter_TD_8678_output_partition", 183000000, 2, "");
    tbl_Filter_TD_8678_output_partition.allocateHost(1.2, hpTimes_join);
    tbl_Filter_TD_8678_output_partition.allocateDevBuffer(context_h, 32);
    Table tbl_Filter_TD_8678_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_Filter_TD_8678_output_partition_array[i] = tbl_Filter_TD_8678_output_partition.createSubTable(i);
    }
    Table tbl_SerializeFromObject_TD_8924_input;
    tbl_SerializeFromObject_TD_8924_input = Table("lineitem", lineitem_n, 2, in_dir);
    tbl_SerializeFromObject_TD_8924_input.selectOverlayVersion(0);
    tbl_SerializeFromObject_TD_8924_input.addCol("l_orderkey", 4);
    tbl_SerializeFromObject_TD_8924_input.addCol("l_suppkey", 4);
    tbl_SerializeFromObject_TD_8924_input.allocateHost();
    tbl_SerializeFromObject_TD_8924_input.loadHost();
    Table tbl_SerializeFromObject_TD_8924_input_partition("tbl_SerializeFromObject_TD_8924_input_partition", lineitem_n, 2, "");
    tbl_SerializeFromObject_TD_8924_input_partition.allocateHost(1.2, hpTimes_join);
    tbl_SerializeFromObject_TD_8924_input_partition.allocateDevBuffer(context_h, 32);
    Table tbl_SerializeFromObject_TD_8924_input_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_SerializeFromObject_TD_8924_input_partition_array[i] = tbl_SerializeFromObject_TD_8924_input_partition.createSubTable(i);
    }
    Table tbl_SerializeFromObject_TD_934_input;
    tbl_SerializeFromObject_TD_934_input = Table("lineitem", lineitem_n, 4, in_dir);
    tbl_SerializeFromObject_TD_934_input.addCol("l_orderkey", 4);
    tbl_SerializeFromObject_TD_934_input.addCol("l_suppkey", 4);
    tbl_SerializeFromObject_TD_934_input.addCol("l_receiptdate", 4);
    tbl_SerializeFromObject_TD_934_input.addCol("l_commitdate", 4);
    tbl_SerializeFromObject_TD_934_input.allocateHost();
    tbl_SerializeFromObject_TD_934_input.loadHost();
    // ********************** Allocate Device Buffer ******************** // 
    tbl_JOIN_INNER_TD_2412_output.allocateDevBuffer(context_h, 32);
    Table tbl_JOIN_INNER_TD_2412_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_JOIN_INNER_TD_2412_output_partition_array[i] = tbl_JOIN_INNER_TD_2412_output.createSubTable(i);
    }
    tbl_Filter_TD_375_output.allocateDevBuffer(context_h, 33);
    tbl_JOIN_INNER_TD_4400_output.allocateDevBuffer(context_h, 32);
    Table tbl_JOIN_INNER_TD_4400_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_JOIN_INNER_TD_4400_output_partition_array[i] = tbl_JOIN_INNER_TD_4400_output.createSubTable(i);
    }
    tbl_JOIN_INNER_TD_3311_output.allocateDevBuffer(context_h, 32);
    Table tbl_JOIN_INNER_TD_3311_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_JOIN_INNER_TD_3311_output_partition_array[i] = tbl_JOIN_INNER_TD_3311_output.createSubTable(i);
    }
    tbl_JOIN_LEFTANTI_TD_5941_output.allocateDevBuffer(context_h, 32);
    Table tbl_JOIN_LEFTANTI_TD_5941_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_JOIN_LEFTANTI_TD_5941_output_partition_array[i] = tbl_JOIN_LEFTANTI_TD_5941_output.createSubTable(i);
    }
    tbl_SerializeFromObject_TD_5109_input_stringRowIDSubstitute.allocateDevBuffer(context_h, 33);
    tbl_Filter_TD_3386_output.allocateDevBuffer(context_h, 33);
    tbl_JOIN_LEFTSEMI_TD_654_output.allocateDevBuffer(context_h, 32);
    Table tbl_JOIN_LEFTSEMI_TD_654_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_JOIN_LEFTSEMI_TD_654_output_partition_array[i] = tbl_JOIN_LEFTSEMI_TD_654_output.createSubTable(i);
    }
    tbl_JOIN_LEFTSEMI_TD_778_output.allocateDevBuffer(context_h, 32);
    Table tbl_JOIN_LEFTSEMI_TD_778_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_JOIN_LEFTSEMI_TD_778_output_partition_array[i] = tbl_JOIN_LEFTSEMI_TD_778_output.createSubTable(i);
    }
    tbl_Filter_TD_7456_output.allocateDevBuffer(context_h, 33);
    tbl_Filter_TD_8678_output.allocateDevBuffer(context_h, 33);
    tbl_SerializeFromObject_TD_8924_input.allocateDevBuffer(context_h, 33);
    // ****************************** Config **************************** // 
    cfgCmd cfg_JOIN_INNER_TD_2412_cmds;
    cfg_JOIN_INNER_TD_2412_cmds.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_2412_gqe_join (cfg_JOIN_INNER_TD_2412_cmds.cmd);
    cfg_JOIN_INNER_TD_2412_cmds.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_2412_cmds_part;
    cfg_JOIN_INNER_TD_2412_cmds_part.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_2412_gqe_join_part (cfg_JOIN_INNER_TD_2412_cmds_part.cmd);
    cfg_JOIN_INNER_TD_2412_cmds_part.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_4400_cmds;
    cfg_JOIN_INNER_TD_4400_cmds.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_4400_gqe_join (cfg_JOIN_INNER_TD_4400_cmds.cmd);
    cfg_JOIN_INNER_TD_4400_cmds.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_3311_cmds;
    cfg_JOIN_INNER_TD_3311_cmds.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_3311_gqe_join (cfg_JOIN_INNER_TD_3311_cmds.cmd);
    cfg_JOIN_INNER_TD_3311_cmds.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_3311_cmds_part;
    cfg_JOIN_INNER_TD_3311_cmds_part.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_3311_gqe_join_part (cfg_JOIN_INNER_TD_3311_cmds_part.cmd);
    cfg_JOIN_INNER_TD_3311_cmds_part.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_LEFTANTI_TD_5941_cmds;
    cfg_JOIN_LEFTANTI_TD_5941_cmds.allocateHost();
    get_cfg_dat_JOIN_LEFTANTI_TD_5941_gqe_join (cfg_JOIN_LEFTANTI_TD_5941_cmds.cmd);
    cfg_JOIN_LEFTANTI_TD_5941_cmds.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_LEFTANTI_TD_5941_cmds_part;
    cfg_JOIN_LEFTANTI_TD_5941_cmds_part.allocateHost();
    get_cfg_dat_JOIN_LEFTANTI_TD_5941_gqe_join_part (cfg_JOIN_LEFTANTI_TD_5941_cmds_part.cmd);
    cfg_JOIN_LEFTANTI_TD_5941_cmds_part.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_LEFTSEMI_TD_654_cmds;
    cfg_JOIN_LEFTSEMI_TD_654_cmds.allocateHost();
    get_cfg_dat_JOIN_LEFTSEMI_TD_654_gqe_join (cfg_JOIN_LEFTSEMI_TD_654_cmds.cmd);
    cfg_JOIN_LEFTSEMI_TD_654_cmds.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_LEFTSEMI_TD_654_cmds_part;
    cfg_JOIN_LEFTSEMI_TD_654_cmds_part.allocateHost();
    get_cfg_dat_JOIN_LEFTSEMI_TD_654_gqe_join_part (cfg_JOIN_LEFTSEMI_TD_654_cmds_part.cmd);
    cfg_JOIN_LEFTSEMI_TD_654_cmds_part.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_LEFTSEMI_TD_778_cmds;
    cfg_JOIN_LEFTSEMI_TD_778_cmds.allocateHost();
    get_cfg_dat_JOIN_LEFTSEMI_TD_778_gqe_join (cfg_JOIN_LEFTSEMI_TD_778_cmds.cmd);
    cfg_JOIN_LEFTSEMI_TD_778_cmds.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_LEFTSEMI_TD_778_cmds_part;
    cfg_JOIN_LEFTSEMI_TD_778_cmds_part.allocateHost();
    get_cfg_dat_JOIN_LEFTSEMI_TD_778_gqe_join_part (cfg_JOIN_LEFTSEMI_TD_778_cmds_part.cmd);
    cfg_JOIN_LEFTSEMI_TD_778_cmds_part.allocateDevBuffer(context_h, 32);
    // *************************** Kernel Setup ************************* // 
    bufferTmp buftmp_h(context_h, 0); 
    buftmp_h.initBuffer(q_h); 
    std::cout << std::endl; 
    krnlEngine krnl_JOIN_INNER_TD_2412_part_right;
    krnl_JOIN_INNER_TD_2412_part_right = krnlEngine(program_h, q_h, context_h, "gqePart");
    krnl_JOIN_INNER_TD_2412_part_right.setup_hp(512, 1, power_of_hpTimes_join, tbl_Filter_TD_375_output, tbl_Filter_TD_375_output_partition, cfg_JOIN_INNER_TD_2412_cmds_part);
    krnlEngine krnl_JOIN_INNER_TD_2412[hpTimes_join];
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_INNER_TD_2412[i] = krnlEngine(program_h, q_h, "gqeJoin");
    }
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_INNER_TD_2412[i].setup(tbl_JOIN_INNER_TD_4400_output_partition_array[i], tbl_Filter_TD_375_output_partition_array[i], tbl_JOIN_INNER_TD_2412_output_partition_array[i], cfg_JOIN_INNER_TD_2412_cmds, buftmp_h);
    }
    krnlEngine krnl_JOIN_INNER_TD_2412_build;
    krnl_JOIN_INNER_TD_2412_build = krnlEngine(program_h, q_h, context_h, "gqeKernel", 0);
    krnl_JOIN_INNER_TD_2412_build.setup(tbl_JOIN_INNER_TD_4400_output, tbl_Filter_TD_375_output, tbl_JOIN_INNER_TD_2412_output, cfg_JOIN_INNER_TD_2412_cmds_build, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_2412_probe;
    krnl_JOIN_INNER_TD_2412_probe = krnlEngine(program_h, q_h, context_h, "gqeKernel", 1);
    krnl_JOIN_INNER_TD_2412_probe.setup(tbl_Filter_TD_375_output, tbl_JOIN_INNER_TD_4400_output, tbl_JOIN_INNER_TD_2412_output, cfg_JOIN_INNER_TD_2412_cmds_probe, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_4400[hpTimes_join];
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_INNER_TD_4400[i] = krnlEngine(program_h, q_h, "gqeJoin");
    }
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_INNER_TD_4400[i].setup(tbl_JOIN_INNER_TD_3311_output_partition_array[i], tbl_JOIN_LEFTANTI_TD_5941_output_partition_array[i], tbl_JOIN_INNER_TD_4400_output_partition_array[i], cfg_JOIN_INNER_TD_4400_cmds, buftmp_h);
    }
    krnlEngine krnl_JOIN_INNER_TD_4400_build;
    krnl_JOIN_INNER_TD_4400_build = krnlEngine(program_h, q_h, context_h, "gqeKernel", 0);
    krnl_JOIN_INNER_TD_4400_build.setup(tbl_JOIN_INNER_TD_3311_output, tbl_JOIN_LEFTANTI_TD_5941_output, tbl_JOIN_INNER_TD_4400_output, cfg_JOIN_INNER_TD_4400_cmds_build, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_4400_probe;
    krnl_JOIN_INNER_TD_4400_probe = krnlEngine(program_h, q_h, context_h, "gqeKernel", 1);
    krnl_JOIN_INNER_TD_4400_probe.setup(tbl_JOIN_LEFTANTI_TD_5941_output, tbl_JOIN_INNER_TD_3311_output, tbl_JOIN_INNER_TD_4400_output, cfg_JOIN_INNER_TD_4400_cmds_probe, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_3311_part_left;
    krnl_JOIN_INNER_TD_3311_part_left = krnlEngine(program_h, q_h, context_h, "gqePart");
    krnl_JOIN_INNER_TD_3311_part_left.setup_hp(512, 0, power_of_hpTimes_join, tbl_SerializeFromObject_TD_5109_input_stringRowIDSubstitute, tbl_SerializeFromObject_TD_5109_input_stringRowIDSubstitute_partition, cfg_JOIN_INNER_TD_3311_cmds_part);
    krnlEngine krnl_JOIN_INNER_TD_3311_part_right;
    krnl_JOIN_INNER_TD_3311_part_right = krnlEngine(program_h, q_h, context_h, "gqePart");
    krnl_JOIN_INNER_TD_3311_part_right.setup_hp(512, 1, power_of_hpTimes_join, tbl_Filter_TD_3386_output, tbl_Filter_TD_3386_output_partition, cfg_JOIN_INNER_TD_3311_cmds_part);
    krnlEngine krnl_JOIN_INNER_TD_3311[hpTimes_join];
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_INNER_TD_3311[i] = krnlEngine(program_h, q_h, "gqeJoin");
    }
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_INNER_TD_3311[i].setup(tbl_SerializeFromObject_TD_5109_input_stringRowIDSubstitute_partition_array[i], tbl_Filter_TD_3386_output_partition_array[i], tbl_JOIN_INNER_TD_3311_output_partition_array[i], cfg_JOIN_INNER_TD_3311_cmds, buftmp_h);
    }
    krnlEngine krnl_JOIN_INNER_TD_3311_build;
    krnl_JOIN_INNER_TD_3311_build = krnlEngine(program_h, q_h, context_h, "gqeKernel", 0);
    krnl_JOIN_INNER_TD_3311_build.setup(tbl_SerializeFromObject_TD_5109_input_stringRowIDSubstitute, tbl_Filter_TD_3386_output, tbl_JOIN_INNER_TD_3311_output, cfg_JOIN_INNER_TD_3311_cmds_build, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_3311_probe;
    krnl_JOIN_INNER_TD_3311_probe = krnlEngine(program_h, q_h, context_h, "gqeKernel", 1);
    krnl_JOIN_INNER_TD_3311_probe.setup(tbl_Filter_TD_3386_output, tbl_SerializeFromObject_TD_5109_input_stringRowIDSubstitute, tbl_JOIN_INNER_TD_3311_output, cfg_JOIN_INNER_TD_3311_cmds_probe, buftmp_h);
    krnlEngine krnl_JOIN_LEFTANTI_TD_5941_part_right;
    krnl_JOIN_LEFTANTI_TD_5941_part_right = krnlEngine(program_h, q_h, context_h, "gqePart");
    krnl_JOIN_LEFTANTI_TD_5941_part_right.setup_hp(512, 1, power_of_hpTimes_join, tbl_Filter_TD_7456_output, tbl_Filter_TD_7456_output_partition, cfg_JOIN_LEFTANTI_TD_5941_cmds_part);
    krnlEngine krnl_JOIN_LEFTANTI_TD_5941[hpTimes_join];
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_LEFTANTI_TD_5941[i] = krnlEngine(program_h, q_h, "gqeJoin");
    }
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_LEFTANTI_TD_5941[i].setup(tbl_JOIN_LEFTSEMI_TD_654_output_partition_array[i], tbl_Filter_TD_7456_output_partition_array[i], tbl_JOIN_LEFTANTI_TD_5941_output_partition_array[i], cfg_JOIN_LEFTANTI_TD_5941_cmds, buftmp_h);
    }
    krnlEngine krnl_JOIN_LEFTANTI_TD_5941_build;
    krnl_JOIN_LEFTANTI_TD_5941_build = krnlEngine(program_h, q_h, context_h, "gqeKernel", 0);
    krnl_JOIN_LEFTANTI_TD_5941_build.setup(tbl_JOIN_LEFTSEMI_TD_654_output, tbl_Filter_TD_7456_output, tbl_JOIN_LEFTANTI_TD_5941_output, cfg_JOIN_LEFTANTI_TD_5941_cmds_build, buftmp_h);
    krnlEngine krnl_JOIN_LEFTANTI_TD_5941_probe;
    krnl_JOIN_LEFTANTI_TD_5941_probe = krnlEngine(program_h, q_h, context_h, "gqeKernel", 1);
    krnl_JOIN_LEFTANTI_TD_5941_probe.setup(tbl_Filter_TD_7456_output, tbl_JOIN_LEFTSEMI_TD_654_output, tbl_JOIN_LEFTANTI_TD_5941_output, cfg_JOIN_LEFTANTI_TD_5941_cmds_probe, buftmp_h);
    krnlEngine krnl_JOIN_LEFTSEMI_TD_654_part_left;
    krnl_JOIN_LEFTSEMI_TD_654_part_left = krnlEngine(program_h, q_h, context_h, "gqePart");
    krnl_JOIN_LEFTSEMI_TD_654_part_left.setup_hp(512, 0, power_of_hpTimes_join, tbl_Filter_TD_7456_output, tbl_Filter_TD_7456_output_partition, cfg_JOIN_LEFTSEMI_TD_654_cmds_part);
    krnlEngine krnl_JOIN_LEFTSEMI_TD_654[hpTimes_join];
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_LEFTSEMI_TD_654[i] = krnlEngine(program_h, q_h, "gqeJoin");
    }
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_LEFTSEMI_TD_654[i].setup(tbl_Filter_TD_7456_output_partition_array[i], tbl_JOIN_LEFTSEMI_TD_778_output_partition_array[i], tbl_JOIN_LEFTSEMI_TD_654_output_partition_array[i], cfg_JOIN_LEFTSEMI_TD_654_cmds, buftmp_h);
    }
    krnlEngine krnl_JOIN_LEFTSEMI_TD_654_build;
    krnl_JOIN_LEFTSEMI_TD_654_build = krnlEngine(program_h, q_h, context_h, "gqeKernel", 0);
    krnl_JOIN_LEFTSEMI_TD_654_build.setup(tbl_Filter_TD_7456_output, tbl_JOIN_LEFTSEMI_TD_778_output, tbl_JOIN_LEFTSEMI_TD_654_output, cfg_JOIN_LEFTSEMI_TD_654_cmds_build, buftmp_h);
    krnlEngine krnl_JOIN_LEFTSEMI_TD_654_probe;
    krnl_JOIN_LEFTSEMI_TD_654_probe = krnlEngine(program_h, q_h, context_h, "gqeKernel", 1);
    krnl_JOIN_LEFTSEMI_TD_654_probe.setup(tbl_JOIN_LEFTSEMI_TD_778_output, tbl_Filter_TD_7456_output, tbl_JOIN_LEFTSEMI_TD_654_output, cfg_JOIN_LEFTSEMI_TD_654_cmds_probe, buftmp_h);
    krnlEngine krnl_JOIN_LEFTSEMI_TD_778_part_left;
    krnl_JOIN_LEFTSEMI_TD_778_part_left = krnlEngine(program_h, q_h, context_h, "gqePart");
    krnl_JOIN_LEFTSEMI_TD_778_part_left.setup_hp(512, 0, power_of_hpTimes_join, tbl_SerializeFromObject_TD_8924_input, tbl_SerializeFromObject_TD_8924_input_partition, cfg_JOIN_LEFTSEMI_TD_778_cmds_part);
    krnlEngine krnl_JOIN_LEFTSEMI_TD_778_part_right;
    krnl_JOIN_LEFTSEMI_TD_778_part_right = krnlEngine(program_h, q_h, context_h, "gqePart");
    krnl_JOIN_LEFTSEMI_TD_778_part_right.setup_hp(512, 1, power_of_hpTimes_join, tbl_Filter_TD_8678_output, tbl_Filter_TD_8678_output_partition, cfg_JOIN_LEFTSEMI_TD_778_cmds_part);
    krnlEngine krnl_JOIN_LEFTSEMI_TD_778[hpTimes_join];
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_LEFTSEMI_TD_778[i] = krnlEngine(program_h, q_h, "gqeJoin");
    }
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_LEFTSEMI_TD_778[i].setup(tbl_SerializeFromObject_TD_8924_input_partition_array[i], tbl_Filter_TD_8678_output_partition_array[i], tbl_JOIN_LEFTSEMI_TD_778_output_partition_array[i], cfg_JOIN_LEFTSEMI_TD_778_cmds, buftmp_h);
    }
    krnlEngine krnl_JOIN_LEFTSEMI_TD_778_build;
    krnl_JOIN_LEFTSEMI_TD_778_build = krnlEngine(program_h, q_h, context_h, "gqeKernel", 0);
    krnl_JOIN_LEFTSEMI_TD_778_build.setup(tbl_SerializeFromObject_TD_8924_input, tbl_Filter_TD_8678_output, tbl_JOIN_LEFTSEMI_TD_778_output, cfg_JOIN_LEFTSEMI_TD_778_cmds_build, buftmp_h);
    krnlEngine krnl_JOIN_LEFTSEMI_TD_778_probe;
    krnl_JOIN_LEFTSEMI_TD_778_probe = krnlEngine(program_h, q_h, context_h, "gqeKernel", 1);
    krnl_JOIN_LEFTSEMI_TD_778_probe.setup(tbl_Filter_TD_8678_output, tbl_SerializeFromObject_TD_8924_input, tbl_JOIN_LEFTSEMI_TD_778_output, cfg_JOIN_LEFTSEMI_TD_778_cmds_probe, buftmp_h);
    // ************************** Transfer Engine *********************** // 
    transEngine trans_JOIN_INNER_TD_2412;
    trans_JOIN_INNER_TD_2412.setq(q_h,0);
    trans_JOIN_INNER_TD_2412.add(&(cfg_JOIN_INNER_TD_2412_cmds_part));
    trans_JOIN_INNER_TD_2412.add(&(cfg_JOIN_INNER_TD_2412_cmds));
    transEngine trans_JOIN_INNER_TD_2412_out;
    trans_JOIN_INNER_TD_2412_out.setq(q_h,0);
    q_h.finish();
    transEngine trans_JOIN_INNER_TD_4400;
    trans_JOIN_INNER_TD_4400.setq(q_h,0);
    trans_JOIN_INNER_TD_4400.add(&(cfg_JOIN_INNER_TD_4400_cmds_part));
    trans_JOIN_INNER_TD_4400.add(&(cfg_JOIN_INNER_TD_4400_cmds));
    q_h.finish();
    transEngine trans_JOIN_INNER_TD_3311;
    trans_JOIN_INNER_TD_3311.setq(q_h,0);
    trans_JOIN_INNER_TD_3311.add(&(cfg_JOIN_INNER_TD_3311_cmds_part));
    trans_JOIN_INNER_TD_3311.add(&(cfg_JOIN_INNER_TD_3311_cmds));
    trans_JOIN_INNER_TD_3311.add(&(tbl_SerializeFromObject_TD_5109_input_stringRowIDSubstitute));
    q_h.finish();
    transEngine trans_JOIN_LEFTANTI_TD_5941;
    trans_JOIN_LEFTANTI_TD_5941.setq(q_h,0);
    trans_JOIN_LEFTANTI_TD_5941.add(&(cfg_JOIN_LEFTANTI_TD_5941_cmds_part));
    trans_JOIN_LEFTANTI_TD_5941.add(&(cfg_JOIN_LEFTANTI_TD_5941_cmds));
    q_h.finish();
    transEngine trans_JOIN_LEFTSEMI_TD_654;
    trans_JOIN_LEFTSEMI_TD_654.setq(q_h,0);
    trans_JOIN_LEFTSEMI_TD_654.add(&(cfg_JOIN_LEFTSEMI_TD_654_cmds_part));
    trans_JOIN_LEFTSEMI_TD_654.add(&(cfg_JOIN_LEFTSEMI_TD_654_cmds));
    q_h.finish();
    transEngine trans_JOIN_LEFTSEMI_TD_778;
    trans_JOIN_LEFTSEMI_TD_778.setq(q_h,0);
    trans_JOIN_LEFTSEMI_TD_778.add(&(cfg_JOIN_LEFTSEMI_TD_778_cmds_part));
    trans_JOIN_LEFTSEMI_TD_778.add(&(cfg_JOIN_LEFTSEMI_TD_778_cmds));
    trans_JOIN_LEFTSEMI_TD_778.add(&(tbl_SerializeFromObject_TD_8924_input));
    q_h.finish();
    // ****************************** Events **************************** // 
    std::vector<cl::Event> events_h2d_wr_JOIN_INNER_TD_2412;
    std::vector<cl::Event> events_d2h_rd_JOIN_INNER_TD_2412;
    std::vector<cl::Event> events_JOIN_INNER_TD_2412[2];
    events_h2d_wr_JOIN_INNER_TD_2412.resize(1);
    events_d2h_rd_JOIN_INNER_TD_2412.resize(1);
    events_JOIN_INNER_TD_2412[0].resize(1);
    events_JOIN_INNER_TD_2412[1].resize(hpTimes_join);
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_2412;
    std::vector<cl::Event> prev_events_grp_JOIN_INNER_TD_2412;
    std::vector<cl::Event> events_h2d_wr_JOIN_INNER_TD_4400;
    std::vector<cl::Event> events_d2h_rd_JOIN_INNER_TD_4400;
    std::vector<cl::Event> events_JOIN_INNER_TD_4400[2];
    events_h2d_wr_JOIN_INNER_TD_4400.resize(1);
    events_d2h_rd_JOIN_INNER_TD_4400.resize(1);
    events_JOIN_INNER_TD_4400[0].resize(0);
    events_JOIN_INNER_TD_4400[1].resize(hpTimes_join);
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_4400;
    std::vector<cl::Event> prev_events_grp_JOIN_INNER_TD_4400;
    std::vector<cl::Event> events_h2d_wr_JOIN_INNER_TD_3311;
    std::vector<cl::Event> events_d2h_rd_JOIN_INNER_TD_3311;
    std::vector<cl::Event> events_JOIN_INNER_TD_3311[2];
    events_h2d_wr_JOIN_INNER_TD_3311.resize(1);
    events_d2h_rd_JOIN_INNER_TD_3311.resize(1);
    events_JOIN_INNER_TD_3311[0].resize(2);
    events_JOIN_INNER_TD_3311[1].resize(hpTimes_join);
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_3311;
    std::vector<cl::Event> prev_events_grp_JOIN_INNER_TD_3311;
    std::vector<cl::Event> events_h2d_wr_JOIN_LEFTANTI_TD_5941;
    std::vector<cl::Event> events_d2h_rd_JOIN_LEFTANTI_TD_5941;
    std::vector<cl::Event> events_JOIN_LEFTANTI_TD_5941[2];
    events_h2d_wr_JOIN_LEFTANTI_TD_5941.resize(1);
    events_d2h_rd_JOIN_LEFTANTI_TD_5941.resize(1);
    events_JOIN_LEFTANTI_TD_5941[0].resize(1);
    events_JOIN_LEFTANTI_TD_5941[1].resize(hpTimes_join);
    std::vector<cl::Event> events_grp_JOIN_LEFTANTI_TD_5941;
    std::vector<cl::Event> prev_events_grp_JOIN_LEFTANTI_TD_5941;
    std::vector<cl::Event> events_h2d_wr_JOIN_LEFTSEMI_TD_654;
    std::vector<cl::Event> events_d2h_rd_JOIN_LEFTSEMI_TD_654;
    std::vector<cl::Event> events_JOIN_LEFTSEMI_TD_654[2];
    events_h2d_wr_JOIN_LEFTSEMI_TD_654.resize(1);
    events_d2h_rd_JOIN_LEFTSEMI_TD_654.resize(1);
    events_JOIN_LEFTSEMI_TD_654[0].resize(1);
    events_JOIN_LEFTSEMI_TD_654[1].resize(hpTimes_join);
    std::vector<cl::Event> events_grp_JOIN_LEFTSEMI_TD_654;
    std::vector<cl::Event> prev_events_grp_JOIN_LEFTSEMI_TD_654;
    std::vector<cl::Event> events_h2d_wr_JOIN_LEFTSEMI_TD_778;
    std::vector<cl::Event> events_d2h_rd_JOIN_LEFTSEMI_TD_778;
    std::vector<cl::Event> events_JOIN_LEFTSEMI_TD_778[2];
    events_h2d_wr_JOIN_LEFTSEMI_TD_778.resize(1);
    events_d2h_rd_JOIN_LEFTSEMI_TD_778.resize(1);
    events_JOIN_LEFTSEMI_TD_778[0].resize(2);
    events_JOIN_LEFTSEMI_TD_778[1].resize(hpTimes_join);
    std::vector<cl::Event> events_grp_JOIN_LEFTSEMI_TD_778;
    std::vector<cl::Event> prev_events_grp_JOIN_LEFTSEMI_TD_778;
    // **************************** Operations ************************** // 
    struct timeval tv_r_s, tv_r_e; 
    gettimeofday(&tv_r_s, 0); 

    struct timeval tv_r_Filter_8_219_s, tv_r_Filter_8_219_e;
    gettimeofday(&tv_r_Filter_8_219_s, 0);
    SW_Filter_TD_8678(tbl_SerializeFromObject_TD_934_input, tbl_Filter_TD_8678_output);
    gettimeofday(&tv_r_Filter_8_219_e, 0);

    struct timeval tv_r_Filter_7_445_s, tv_r_Filter_7_445_e;
    gettimeofday(&tv_r_Filter_7_445_s, 0);
    SW_Filter_TD_7456(tbl_SerializeFromObject_TD_934_input, tbl_Filter_TD_7456_output);
    gettimeofday(&tv_r_Filter_7_445_e, 0);

    struct timeval tv_r_JOIN_LEFTSEMI_7_834_s, tv_r_JOIN_LEFTSEMI_7_834_e;
    gettimeofday(&tv_r_JOIN_LEFTSEMI_7_834_s, 0);
    trans_JOIN_LEFTSEMI_TD_778.add(&(tbl_Filter_TD_8678_output));
    trans_JOIN_LEFTSEMI_TD_778.host2dev(0, &(prev_events_grp_JOIN_LEFTSEMI_TD_778), &(events_h2d_wr_JOIN_LEFTSEMI_TD_778[0]));
    events_grp_JOIN_LEFTSEMI_TD_778.push_back(events_h2d_wr_JOIN_LEFTSEMI_TD_778[0]);
    krnl_JOIN_LEFTSEMI_TD_778_part_left.run(0, &(events_grp_JOIN_LEFTSEMI_TD_778), &(events_JOIN_LEFTSEMI_TD_778[0][0]));
    krnl_JOIN_LEFTSEMI_TD_778_part_right.run(0, &(events_grp_JOIN_LEFTSEMI_TD_778), &(events_JOIN_LEFTSEMI_TD_778[0][1]));
    for (int i(0); i < hpTimes_join; ++i) {
        krnl_JOIN_LEFTSEMI_TD_778[i].run(0, &(events_JOIN_LEFTSEMI_TD_778[0]), &(events_JOIN_LEFTSEMI_TD_778[1][i]));
    }
    gettimeofday(&tv_r_JOIN_LEFTSEMI_7_834_e, 0);

    struct timeval tv_r_JOIN_LEFTSEMI_6_356_s, tv_r_JOIN_LEFTSEMI_6_356_e;
    gettimeofday(&tv_r_JOIN_LEFTSEMI_6_356_s, 0);
    prev_events_grp_JOIN_LEFTSEMI_TD_654.push_back(events_h2d_wr_JOIN_LEFTSEMI_TD_778[0]);
    trans_JOIN_LEFTSEMI_TD_654.add(&(tbl_Filter_TD_7456_output));
    trans_JOIN_LEFTSEMI_TD_654.host2dev(0, &(prev_events_grp_JOIN_LEFTSEMI_TD_654), &(events_h2d_wr_JOIN_LEFTSEMI_TD_654[0]));
    events_grp_JOIN_LEFTSEMI_TD_654.push_back(events_h2d_wr_JOIN_LEFTSEMI_TD_654[0]);
    events_grp_JOIN_LEFTSEMI_TD_654.insert(std::end(events_grp_JOIN_LEFTSEMI_TD_654), std::begin(events_JOIN_LEFTSEMI_TD_778[0]), std::end(events_JOIN_LEFTSEMI_TD_778[0]));
    events_grp_JOIN_LEFTSEMI_TD_654.insert(std::end(events_grp_JOIN_LEFTSEMI_TD_654), std::begin(events_JOIN_LEFTSEMI_TD_778[1]), std::end(events_JOIN_LEFTSEMI_TD_778[1]));
    krnl_JOIN_LEFTSEMI_TD_654_part_left.run(0, &(events_grp_JOIN_LEFTSEMI_TD_654), &(events_JOIN_LEFTSEMI_TD_654[0][0]));
    for (int i(0); i < hpTimes_join; ++i) {
        krnl_JOIN_LEFTSEMI_TD_654[i].run(0, &(events_JOIN_LEFTSEMI_TD_654[0]), &(events_JOIN_LEFTSEMI_TD_654[1][i]));
    }
    gettimeofday(&tv_r_JOIN_LEFTSEMI_6_356_e, 0);

    struct timeval tv_r_Filter_3_548_s, tv_r_Filter_3_548_e;
    gettimeofday(&tv_r_Filter_3_548_s, 0);
    SW_Filter_TD_3386(tbl_SerializeFromObject_TD_4666_input, tbl_Filter_TD_3386_output);
    gettimeofday(&tv_r_Filter_3_548_e, 0);

    struct timeval tv_r_JOIN_LEFTANTI_5_643_s, tv_r_JOIN_LEFTANTI_5_643_e;
    gettimeofday(&tv_r_JOIN_LEFTANTI_5_643_s, 0);
    prev_events_grp_JOIN_LEFTANTI_TD_5941.push_back(events_h2d_wr_JOIN_LEFTSEMI_TD_654[0]);
    trans_JOIN_LEFTANTI_TD_5941.add(&(tbl_Filter_TD_7456_output));
    trans_JOIN_LEFTANTI_TD_5941.host2dev(0, &(prev_events_grp_JOIN_LEFTANTI_TD_5941), &(events_h2d_wr_JOIN_LEFTANTI_TD_5941[0]));
    events_grp_JOIN_LEFTANTI_TD_5941.push_back(events_h2d_wr_JOIN_LEFTANTI_TD_5941[0]);
    events_grp_JOIN_LEFTANTI_TD_5941.insert(std::end(events_grp_JOIN_LEFTANTI_TD_5941), std::begin(events_JOIN_LEFTSEMI_TD_654[0]), std::end(events_JOIN_LEFTSEMI_TD_654[0]));
    events_grp_JOIN_LEFTANTI_TD_5941.insert(std::end(events_grp_JOIN_LEFTANTI_TD_5941), std::begin(events_JOIN_LEFTSEMI_TD_654[1]), std::end(events_JOIN_LEFTSEMI_TD_654[1]));
    krnl_JOIN_LEFTANTI_TD_5941_part_right.run(0, &(events_grp_JOIN_LEFTANTI_TD_5941), &(events_JOIN_LEFTANTI_TD_5941[0][0]));
    for (int i(0); i < hpTimes_join; ++i) {
        krnl_JOIN_LEFTANTI_TD_5941[i].run(0, &(events_grp_JOIN_LEFTANTI_TD_5941), &(events_JOIN_LEFTANTI_TD_5941[1][i]));
    }
    gettimeofday(&tv_r_JOIN_LEFTANTI_5_643_e, 0);

    struct timeval tv_r_JOIN_INNER_3_304_s, tv_r_JOIN_INNER_3_304_e;
    gettimeofday(&tv_r_JOIN_INNER_3_304_s, 0);
    trans_JOIN_INNER_TD_3311.add(&(tbl_Filter_TD_3386_output));
    trans_JOIN_INNER_TD_3311.host2dev(0, &(prev_events_grp_JOIN_INNER_TD_3311), &(events_h2d_wr_JOIN_INNER_TD_3311[0]));
    events_grp_JOIN_INNER_TD_3311.push_back(events_h2d_wr_JOIN_INNER_TD_3311[0]);
    krnl_JOIN_INNER_TD_3311_part_left.run(0, &(events_grp_JOIN_INNER_TD_3311), &(events_JOIN_INNER_TD_3311[0][0]));
    krnl_JOIN_INNER_TD_3311_part_right.run(0, &(events_grp_JOIN_INNER_TD_3311), &(events_JOIN_INNER_TD_3311[0][1]));
    for (int i(0); i < hpTimes_join; ++i) {
        krnl_JOIN_INNER_TD_3311[i].run(0, &(events_JOIN_INNER_TD_3311[0]), &(events_JOIN_INNER_TD_3311[1][i]));
    }
    gettimeofday(&tv_r_JOIN_INNER_3_304_e, 0);

    struct timeval tv_r_JOIN_INNER_4_726_s, tv_r_JOIN_INNER_4_726_e;
    gettimeofday(&tv_r_JOIN_INNER_4_726_s, 0);
    prev_events_grp_JOIN_INNER_TD_4400.push_back(events_h2d_wr_JOIN_INNER_TD_3311[0]);
    prev_events_grp_JOIN_INNER_TD_4400.push_back(events_h2d_wr_JOIN_LEFTANTI_TD_5941[0]);
    trans_JOIN_INNER_TD_4400.host2dev(0, &(prev_events_grp_JOIN_INNER_TD_4400), &(events_h2d_wr_JOIN_INNER_TD_4400[0]));
    events_grp_JOIN_INNER_TD_4400.push_back(events_h2d_wr_JOIN_INNER_TD_4400[0]);
    events_grp_JOIN_INNER_TD_4400.insert(std::end(events_grp_JOIN_INNER_TD_4400), std::begin(events_JOIN_INNER_TD_3311[0]), std::end(events_JOIN_INNER_TD_3311[0]));
    events_grp_JOIN_INNER_TD_4400.insert(std::end(events_grp_JOIN_INNER_TD_4400), std::begin(events_JOIN_INNER_TD_3311[1]), std::end(events_JOIN_INNER_TD_3311[1]));
    events_grp_JOIN_INNER_TD_4400.insert(std::end(events_grp_JOIN_INNER_TD_4400), std::begin(events_JOIN_LEFTANTI_TD_5941[0]), std::end(events_JOIN_LEFTANTI_TD_5941[0]));
    events_grp_JOIN_INNER_TD_4400.insert(std::end(events_grp_JOIN_INNER_TD_4400), std::begin(events_JOIN_LEFTANTI_TD_5941[1]), std::end(events_JOIN_LEFTANTI_TD_5941[1]));
    for (int i(0); i < hpTimes_join; ++i) {
        krnl_JOIN_INNER_TD_4400[i].run(0, &(events_grp_JOIN_INNER_TD_4400), &(events_JOIN_INNER_TD_4400[1][i]));
    }
    gettimeofday(&tv_r_JOIN_INNER_4_726_e, 0);

    struct timeval tv_r_Filter_3_72_s, tv_r_Filter_3_72_e;
    gettimeofday(&tv_r_Filter_3_72_s, 0);
    SW_Filter_TD_375(tbl_SerializeFromObject_TD_5565_input, tbl_Filter_TD_375_output);
    gettimeofday(&tv_r_Filter_3_72_e, 0);

    struct timeval tv_r_JOIN_INNER_2_515_s, tv_r_JOIN_INNER_2_515_e;
    gettimeofday(&tv_r_JOIN_INNER_2_515_s, 0);
    prev_events_grp_JOIN_INNER_TD_2412.push_back(events_h2d_wr_JOIN_INNER_TD_4400[0]);
    trans_JOIN_INNER_TD_2412.add(&(tbl_Filter_TD_375_output));
    trans_JOIN_INNER_TD_2412.host2dev(0, &(prev_events_grp_JOIN_INNER_TD_2412), &(events_h2d_wr_JOIN_INNER_TD_2412[0]));
    events_grp_JOIN_INNER_TD_2412.push_back(events_h2d_wr_JOIN_INNER_TD_2412[0]);
    events_grp_JOIN_INNER_TD_2412.insert(std::end(events_grp_JOIN_INNER_TD_2412), std::begin(events_JOIN_INNER_TD_4400[0]), std::end(events_JOIN_INNER_TD_4400[0]));
    events_grp_JOIN_INNER_TD_2412.insert(std::end(events_grp_JOIN_INNER_TD_2412), std::begin(events_JOIN_INNER_TD_4400[1]), std::end(events_JOIN_INNER_TD_4400[1]));
    krnl_JOIN_INNER_TD_2412_part_right.run(0, &(events_grp_JOIN_INNER_TD_2412), &(events_JOIN_INNER_TD_2412[0][0]));
    for (int i(0); i < hpTimes_join; ++i) {
        krnl_JOIN_INNER_TD_2412[i].run(0, &(events_grp_JOIN_INNER_TD_2412), &(events_JOIN_INNER_TD_2412[1][i]));
    }
    for (int i(0); i < hpTimes_join; ++i) {
        trans_JOIN_INNER_TD_2412_out.add(&(tbl_JOIN_INNER_TD_2412_output_partition_array[i]));
    }
    trans_JOIN_INNER_TD_2412_out.dev2host(0, &(events_JOIN_INNER_TD_2412[1]), &(events_d2h_rd_JOIN_INNER_TD_2412[0]));
    q_h.flush();
    q_h.finish();
    gettimeofday(&tv_r_JOIN_INNER_2_515_e, 0);

    struct timeval tv_r_Aggregate_1_32_s, tv_r_Aggregate_1_32_e;
    gettimeofday(&tv_r_Aggregate_1_32_s, 0);
    SW_Aggregate_TD_1858(tbl_JOIN_INNER_TD_2412_output_partition_array, tbl_SerializeFromObject_TD_5565_input, tbl_SerializeFromObject_TD_5109_input, tbl_SerializeFromObject_TD_4666_input, tbl_Aggregate_TD_1858_output, hpTimes_join);
    gettimeofday(&tv_r_Aggregate_1_32_e, 0);

    struct timeval tv_r_Sort_0_576_s, tv_r_Sort_0_576_e;
    gettimeofday(&tv_r_Sort_0_576_s, 0);
    SW_Sort_TD_092(tbl_Aggregate_TD_1858_output, tbl_Sort_TD_092_output);
    gettimeofday(&tv_r_Sort_0_576_e, 0);

std::cout << "CPU TIME: " << (tvdiff(&tv_r_Filter_8_219_s, &tv_r_Filter_8_219_e) + 
tvdiff(&tv_r_Filter_7_445_s, &tv_r_Filter_7_445_e) + 
tvdiff(&tv_r_Filter_3_548_s, &tv_r_Filter_3_548_e) + 
tvdiff(&tv_r_Filter_3_72_s, &tv_r_Filter_3_72_e) + 
tvdiff(&tv_r_Aggregate_1_32_s, &tv_r_Aggregate_1_32_e) + 
tvdiff(&tv_r_Sort_0_576_s, &tv_r_Sort_0_576_e) + 
1) / 1000.0 << std::endl;

std::cout << "FPGA TIME: " << (tvdiff(&tv_r_JOIN_LEFTSEMI_7_834_s, &tv_r_JOIN_LEFTSEMI_7_834_e) + 
tvdiff(&tv_r_JOIN_LEFTSEMI_6_356_s, &tv_r_JOIN_LEFTSEMI_6_356_e) + 
tvdiff(&tv_r_JOIN_LEFTANTI_5_643_s, &tv_r_JOIN_LEFTANTI_5_643_e) + 
tvdiff(&tv_r_JOIN_INNER_3_304_s, &tv_r_JOIN_INNER_3_304_e) + 
tvdiff(&tv_r_JOIN_INNER_4_726_s, &tv_r_JOIN_INNER_4_726_e) + 
tvdiff(&tv_r_JOIN_INNER_2_515_s, &tv_r_JOIN_INNER_2_515_e) + 
1) / 1000.0 << std::endl;

    gettimeofday(&tv_r_e, 0); 
    // **************************** Print Execution Time ************************** // 
    std::cout << std::endl; 
    std::cout << "Filter_8: " << tvdiff(&tv_r_Filter_8_219_s, &tv_r_Filter_8_219_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer((l_receiptdate#89 > l_commitdate#88)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_934_input.getNumRow() << " -> tbl_SerializeFromObject_TD_934_input" 
     << std::endl << "      #COL: 4: " << "ListBuffer(l_orderkey#77, l_suppkey#79, l_receiptdate#89, l_commitdate#88)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_8678_output.getNumRow() << " -> tbl_Filter_TD_8678_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(l_orderkey#77, l_suppkey#79)" 
     << std::endl; 

    std::cout << "Filter_7: " << tvdiff(&tv_r_Filter_7_445_s, &tv_r_Filter_7_445_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer((l_receiptdate#89 > l_commitdate#88)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_934_input.getNumRow() << " -> tbl_SerializeFromObject_TD_934_input" 
     << std::endl << "      #COL: 4: " << "ListBuffer(l_orderkey#77, l_suppkey#79, l_receiptdate#89, l_commitdate#88)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_7456_output.getNumRow() << " -> tbl_Filter_TD_7456_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(l_orderkey#77#2016, l_suppkey#79#2018)" 
     << std::endl; 

    std::cout << "JOIN_LEFTSEMI_7: " << tvdiff(&tv_r_JOIN_LEFTSEMI_7_834_s, &tv_r_JOIN_LEFTSEMI_7_834_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer(((l_orderkey#77#2000 = l_orderkey#77) AND NOT (l_suppkey#79#2002 = l_suppkey#79))) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_8678_output.getNumRow() << " -> tbl_Filter_TD_8678_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(l_orderkey#77, l_suppkey#79)" 
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_8924_input.getNumRow() << " -> tbl_SerializeFromObject_TD_8924_input" 
     << std::endl << "      #COL: 2: " << "ListBuffer(l_orderkey#77#2000, l_suppkey#79#2002)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_LEFTSEMI_TD_778_output.getNumRow() << " -> tbl_JOIN_LEFTSEMI_TD_778_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(l_orderkey#77, l_suppkey#79)" 
     << std::endl; 

    std::cout << "JOIN_LEFTSEMI_6: " << tvdiff(&tv_r_JOIN_LEFTSEMI_6_356_s, &tv_r_JOIN_LEFTSEMI_6_356_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer(((l_orderkey#77#2016 = l_orderkey#77) AND NOT (l_suppkey#79#2018 = l_suppkey#79))) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_LEFTSEMI_TD_778_output.getNumRow() << " -> tbl_JOIN_LEFTSEMI_TD_778_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(l_orderkey#77, l_suppkey#79)" 
     << std::endl << "      #ROW: " << tbl_Filter_TD_7456_output.getNumRow() << " -> tbl_Filter_TD_7456_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(l_orderkey#77#2016, l_suppkey#79#2018)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_LEFTSEMI_TD_654_output.getNumRow() << " -> tbl_JOIN_LEFTSEMI_TD_654_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(l_orderkey#77, l_suppkey#79)" 
     << std::endl; 

    std::cout << "Filter_3: " << tvdiff(&tv_r_Filter_3_548_s, &tv_r_Filter_3_548_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer((isnotnull(n_name#145) AND (n_name#145 = JAPAN))) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_4666_input.getNumRow() << " -> tbl_SerializeFromObject_TD_4666_input" 
     << std::endl << "      #COL: 2: " << "ListBuffer(n_nationkey#144, n_name#145)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_3386_output.getNumRow() << " -> tbl_Filter_TD_3386_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(n_nationkey#144)" 
     << std::endl; 

    std::cout << "JOIN_LEFTANTI_5: " << tvdiff(&tv_r_JOIN_LEFTANTI_5_643_s, &tv_r_JOIN_LEFTANTI_5_643_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer(((l_orderkey#77#2016 = l_orderkey#77) AND NOT (l_suppkey#79#2018 = l_suppkey#79))) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_7456_output.getNumRow() << " -> tbl_Filter_TD_7456_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(l_orderkey#77#2016, l_suppkey#79#2018)" 
     << std::endl << "      #ROW: " << tbl_JOIN_LEFTSEMI_TD_654_output.getNumRow() << " -> tbl_JOIN_LEFTSEMI_TD_654_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(l_orderkey#77, l_suppkey#79)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_LEFTANTI_TD_5941_output.getNumRow() << " -> tbl_JOIN_LEFTANTI_TD_5941_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(l_orderkey#77, l_suppkey#79)" 
     << std::endl; 

    std::cout << "JOIN_INNER_3: " << tvdiff(&tv_r_JOIN_INNER_3_304_s, &tv_r_JOIN_INNER_3_304_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer((s_nationkey#345 = n_nationkey#144)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_5109_input_stringRowIDSubstitute.getNumRow() << " -> tbl_SerializeFromObject_TD_5109_input_stringRowIDSubstitute" 
     << std::endl << "      #COL: 3: " << "ListBuffer(s_suppkey#342, s_name#343, s_nationkey#345)" 
     << std::endl << "      #ROW: " << tbl_Filter_TD_3386_output.getNumRow() << " -> tbl_Filter_TD_3386_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(n_nationkey#144)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_3311_output.getNumRow() << " -> tbl_JOIN_INNER_TD_3311_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(s_suppkey#342, s_name#343)" 
     << std::endl; 

    std::cout << "JOIN_INNER_4: " << tvdiff(&tv_r_JOIN_INNER_4_726_s, &tv_r_JOIN_INNER_4_726_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer((s_suppkey#342 = l_suppkey#79)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_3311_output.getNumRow() << " -> tbl_JOIN_INNER_TD_3311_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(s_suppkey#342, s_name#343)" 
     << std::endl << "      #ROW: " << tbl_JOIN_LEFTANTI_TD_5941_output.getNumRow() << " -> tbl_JOIN_LEFTANTI_TD_5941_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(l_orderkey#77, l_suppkey#79)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_4400_output.getNumRow() << " -> tbl_JOIN_INNER_TD_4400_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(s_name#343, l_orderkey#77)" 
     << std::endl; 

    std::cout << "Filter_3: " << tvdiff(&tv_r_Filter_3_72_s, &tv_r_Filter_3_72_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer((o_orderstatus#209 = 70)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_5565_input.getNumRow() << " -> tbl_SerializeFromObject_TD_5565_input" 
     << std::endl << "      #COL: 2: " << "ListBuffer(o_orderkey#207, o_orderstatus#209)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_375_output.getNumRow() << " -> tbl_Filter_TD_375_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(o_orderkey#207)" 
     << std::endl; 

    std::cout << "JOIN_INNER_2: " << tvdiff(&tv_r_JOIN_INNER_2_515_s, &tv_r_JOIN_INNER_2_515_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer((o_orderkey#207 = l_orderkey#77)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_375_output.getNumRow() << " -> tbl_Filter_TD_375_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(o_orderkey#207)" 
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_4400_output.getNumRow() << " -> tbl_JOIN_INNER_TD_4400_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(s_name#343, l_orderkey#77)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_2412_output.getNumRow() << " -> tbl_JOIN_INNER_TD_2412_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(s_name#343)" 
     << std::endl; 

    std::cout << "Aggregate_1: " << tvdiff(&tv_r_Aggregate_1_32_s, &tv_r_Aggregate_1_32_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(s_name#343, count(1) AS numwait#1903L) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_2412_output.getNumRow() << " -> tbl_JOIN_INNER_TD_2412_output" 
     << std::endl << "      #COL: 1: " << "ListBuffer(s_name#343)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_1858_output.getNumRow() << " -> tbl_Aggregate_TD_1858_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(s_name#343, numwait#1903L)" 
     << std::endl; 

    std::cout << "Sort_0: " << tvdiff(&tv_r_Sort_0_576_s, &tv_r_Sort_0_576_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(numwait#1903L DESC NULLS LAST, s_name#343 ASC NULLS FIRST) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_1858_output.getNumRow() << " -> tbl_Aggregate_TD_1858_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(s_name#343, numwait#1903L)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Sort_TD_092_output.getNumRow() << " -> tbl_Sort_TD_092_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(s_name#343, numwait#1903L)" 
     << std::endl; 

    std::cout << std::endl << " Total execution time: " << tvdiff(&tv_r_s, &tv_r_e) / 1000 << " ms"; 

    std::cout << std::endl << " Spark elapsed time: " << 70.53126 * 1000 << "ms" << std::endl; 
    return 0; 
}
