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
    // *********************** Partition Infomation ********************* // 
    int hpTimes_join = 32; 
    int power_of_hpTimes_join = log2(hpTimes_join); 
    std::cout << "Number of partition (gqeJoin) is: " << hpTimes_join << std::endl; 
    int hpTimes_aggr = 256; 
    int power_of_hpTimes_aggr = log2(hpTimes_aggr); 
    std::cout << "Number of partition (gqeAggr) is: " << hpTimes_aggr << std::endl; 
    // ****************************** Tables **************************** // 
    Table tbl_Sort_TD_0502_output("tbl_Sort_TD_0502_output", 183000000, 4, "");
    tbl_Sort_TD_0502_output.allocateHost();
    Table tbl_Aggregate_TD_1343_output("tbl_Aggregate_TD_1343_output", 183000000, 4, "");
    tbl_Aggregate_TD_1343_output.allocateHost();
    Table tbl_Project_TD_288_output("tbl_Project_TD_288_output", 183000000, 4, "");
    tbl_Project_TD_288_output.allocateHost();
    Table tbl_JOIN_INNER_TD_3612_output("tbl_JOIN_INNER_TD_3612_output", 183000000, 6, "");
    tbl_JOIN_INNER_TD_3612_output.allocateHost();
    Table tbl_JOIN_INNER_TD_4852_output("tbl_JOIN_INNER_TD_4852_output", 153500, 5, "");
    tbl_JOIN_INNER_TD_4852_output.allocateHost(1.2, hpTimes_join);
    Table tbl_Filter_TD_4956_output("tbl_Filter_TD_4956_output", 183000000, 2, "");
    tbl_Filter_TD_4956_output.allocateHost();
    Table tbl_JOIN_INNER_TD_5573_output("tbl_JOIN_INNER_TD_5573_output", 153500, 6, "");
    tbl_JOIN_INNER_TD_5573_output.allocateHost(1.2, hpTimes_join);
    Table tbl_SerializeFromObject_TD_5893_input;
    tbl_SerializeFromObject_TD_5893_input = Table("customer", customer_n, 2, in_dir);
    tbl_SerializeFromObject_TD_5893_input.selectOverlayVersion(0);
    tbl_SerializeFromObject_TD_5893_input.addCol("c_custkey", 4);
    tbl_SerializeFromObject_TD_5893_input.addCol("c_nationkey", 4);
    tbl_SerializeFromObject_TD_5893_input.allocateHost();
    tbl_SerializeFromObject_TD_5893_input.loadHost();
    Table tbl_SerializeFromObject_TD_5893_input_partition("tbl_SerializeFromObject_TD_5893_input_partition", customer_n, 2, "");
    tbl_SerializeFromObject_TD_5893_input_partition.allocateHost(1.2, hpTimes_join);
    tbl_SerializeFromObject_TD_5893_input_partition.allocateDevBuffer(context_h, 32);
    Table tbl_SerializeFromObject_TD_5893_input_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_SerializeFromObject_TD_5893_input_partition_array[i] = tbl_SerializeFromObject_TD_5893_input_partition.createSubTable(i);
    }
    Table tbl_SerializeFromObject_TD_5740_input;
    tbl_SerializeFromObject_TD_5740_input = Table("nation", nation_n, 2, in_dir);
    tbl_SerializeFromObject_TD_5740_input.addCol("n_nationkey", 4);
    tbl_SerializeFromObject_TD_5740_input.addCol("n_name", TPCH_READ_NATION_LEN+1);
    tbl_SerializeFromObject_TD_5740_input.allocateHost();
    tbl_SerializeFromObject_TD_5740_input.loadHost();
    Table tbl_JOIN_INNER_TD_6771_output("tbl_JOIN_INNER_TD_6771_output", 153500, 6, "");
    tbl_JOIN_INNER_TD_6771_output.allocateHost(1.2, hpTimes_join);
    Table tbl_SerializeFromObject_TD_6468_input;
    tbl_SerializeFromObject_TD_6468_input = Table("order", order_n, 2, in_dir);
    tbl_SerializeFromObject_TD_6468_input.selectOverlayVersion(0);
    tbl_SerializeFromObject_TD_6468_input.addCol("o_orderkey", 4);
    tbl_SerializeFromObject_TD_6468_input.addCol("o_custkey", 4);
    tbl_SerializeFromObject_TD_6468_input.allocateHost();
    tbl_SerializeFromObject_TD_6468_input.loadHost();
    Table tbl_SerializeFromObject_TD_6468_input_partition("tbl_SerializeFromObject_TD_6468_input_partition", order_n, 2, "");
    tbl_SerializeFromObject_TD_6468_input_partition.allocateHost(1.2, hpTimes_join);
    tbl_SerializeFromObject_TD_6468_input_partition.allocateDevBuffer(context_h, 32);
    Table tbl_SerializeFromObject_TD_6468_input_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_SerializeFromObject_TD_6468_input_partition_array[i] = tbl_SerializeFromObject_TD_6468_input_partition.createSubTable(i);
    }
    Table tbl_JOIN_INNER_TD_7773_output("tbl_JOIN_INNER_TD_7773_output", 1829000, 5, "");
    tbl_JOIN_INNER_TD_7773_output.allocateHost(1.2, hpTimes_join);
    Table tbl_Filter_TD_5661_output("tbl_Filter_TD_5661_output", 183000000, 2, "");
    tbl_Filter_TD_5661_output.selectOverlayVersion(0);
    tbl_Filter_TD_5661_output.allocateHost();
    Table tbl_Filter_TD_5661_output_partition("tbl_Filter_TD_5661_output_partition", 183000000, 2, "");
    tbl_Filter_TD_5661_output_partition.allocateHost(1.2, hpTimes_join);
    tbl_Filter_TD_5661_output_partition.allocateDevBuffer(context_h, 32);
    Table tbl_Filter_TD_5661_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_Filter_TD_5661_output_partition_array[i] = tbl_Filter_TD_5661_output_partition.createSubTable(i);
    }
    Table tbl_SerializeFromObject_TD_8161_input;
    tbl_SerializeFromObject_TD_8161_input = Table("supplier", supplier_n, 2, in_dir);
    tbl_SerializeFromObject_TD_8161_input.selectOverlayVersion(0);
    tbl_SerializeFromObject_TD_8161_input.addCol("s_suppkey", 4);
    tbl_SerializeFromObject_TD_8161_input.addCol("s_nationkey", 4);
    tbl_SerializeFromObject_TD_8161_input.allocateHost();
    tbl_SerializeFromObject_TD_8161_input.loadHost();
    Table tbl_SerializeFromObject_TD_8161_input_partition("tbl_SerializeFromObject_TD_8161_input_partition", supplier_n, 2, "");
    tbl_SerializeFromObject_TD_8161_input_partition.allocateHost(1.2, hpTimes_join);
    tbl_SerializeFromObject_TD_8161_input_partition.allocateDevBuffer(context_h, 32);
    Table tbl_SerializeFromObject_TD_8161_input_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_SerializeFromObject_TD_8161_input_partition_array[i] = tbl_SerializeFromObject_TD_8161_input_partition.createSubTable(i);
    }
    Table tbl_Filter_TD_8761_output("tbl_Filter_TD_8761_output", 183000000, 5, "");
    tbl_Filter_TD_8761_output.selectOverlayVersion(0);
    tbl_Filter_TD_8761_output.allocateHost();
    Table tbl_Filter_TD_8761_output_partition("tbl_Filter_TD_8761_output_partition", 183000000, 5, "");
    tbl_Filter_TD_8761_output_partition.allocateHost(1.2, hpTimes_join);
    tbl_Filter_TD_8761_output_partition.allocateDevBuffer(context_h, 32);
    Table tbl_Filter_TD_8761_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_Filter_TD_8761_output_partition_array[i] = tbl_Filter_TD_8761_output_partition.createSubTable(i);
    }
    Table tbl_SerializeFromObject_TD_695_input;
    tbl_SerializeFromObject_TD_695_input = Table("nation", nation_n, 2, in_dir);
    tbl_SerializeFromObject_TD_695_input.addCol("n_nationkey", 4);
    tbl_SerializeFromObject_TD_695_input.addCol("n_name", TPCH_READ_NATION_LEN+1);
    tbl_SerializeFromObject_TD_695_input.allocateHost();
    tbl_SerializeFromObject_TD_695_input.loadHost();
    Table tbl_SerializeFromObject_TD_9263_input;
    tbl_SerializeFromObject_TD_9263_input = Table("lineitem", lineitem_n, 5, in_dir);
    tbl_SerializeFromObject_TD_9263_input.addCol("l_orderkey", 4);
    tbl_SerializeFromObject_TD_9263_input.addCol("l_suppkey", 4);
    tbl_SerializeFromObject_TD_9263_input.addCol("l_extendedprice", 4);
    tbl_SerializeFromObject_TD_9263_input.addCol("l_discount", 4);
    tbl_SerializeFromObject_TD_9263_input.addCol("l_shipdate", 4);
    tbl_SerializeFromObject_TD_9263_input.allocateHost();
    tbl_SerializeFromObject_TD_9263_input.loadHost();
    // ********************** Allocate Device Buffer ******************** // 
    tbl_JOIN_INNER_TD_4852_output.allocateDevBuffer(context_h, 32);
    Table tbl_JOIN_INNER_TD_4852_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_JOIN_INNER_TD_4852_output_partition_array[i] = tbl_JOIN_INNER_TD_4852_output.createSubTable(i);
    }
    tbl_JOIN_INNER_TD_5573_output.allocateDevBuffer(context_h, 32);
    Table tbl_JOIN_INNER_TD_5573_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_JOIN_INNER_TD_5573_output_partition_array[i] = tbl_JOIN_INNER_TD_5573_output.createSubTable(i);
    }
    tbl_SerializeFromObject_TD_5893_input.allocateDevBuffer(context_h, 33);
    tbl_JOIN_INNER_TD_6771_output.allocateDevBuffer(context_h, 32);
    Table tbl_JOIN_INNER_TD_6771_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_JOIN_INNER_TD_6771_output_partition_array[i] = tbl_JOIN_INNER_TD_6771_output.createSubTable(i);
    }
    tbl_SerializeFromObject_TD_6468_input.allocateDevBuffer(context_h, 33);
    tbl_JOIN_INNER_TD_7773_output.allocateDevBuffer(context_h, 32);
    Table tbl_JOIN_INNER_TD_7773_output_partition_array[hpTimes_join];
    for (int i(0); i < hpTimes_join; ++i) {
        tbl_JOIN_INNER_TD_7773_output_partition_array[i] = tbl_JOIN_INNER_TD_7773_output.createSubTable(i);
    }
    tbl_Filter_TD_5661_output.allocateDevBuffer(context_h, 33);
    tbl_SerializeFromObject_TD_8161_input.allocateDevBuffer(context_h, 33);
    tbl_Filter_TD_8761_output.allocateDevBuffer(context_h, 33);
    // ****************************** Config **************************** // 
    cfgCmd cfg_JOIN_INNER_TD_4852_cmds;
    cfg_JOIN_INNER_TD_4852_cmds.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_4852_gqe_join (cfg_JOIN_INNER_TD_4852_cmds.cmd);
    cfg_JOIN_INNER_TD_4852_cmds.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_4852_cmds_part;
    cfg_JOIN_INNER_TD_4852_cmds_part.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_4852_gqe_join_part (cfg_JOIN_INNER_TD_4852_cmds_part.cmd);
    cfg_JOIN_INNER_TD_4852_cmds_part.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_5573_cmds;
    cfg_JOIN_INNER_TD_5573_cmds.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_5573_gqe_join (cfg_JOIN_INNER_TD_5573_cmds.cmd);
    cfg_JOIN_INNER_TD_5573_cmds.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_5573_cmds_part;
    cfg_JOIN_INNER_TD_5573_cmds_part.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_5573_gqe_join_part (cfg_JOIN_INNER_TD_5573_cmds_part.cmd);
    cfg_JOIN_INNER_TD_5573_cmds_part.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_6771_cmds;
    cfg_JOIN_INNER_TD_6771_cmds.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_6771_gqe_join (cfg_JOIN_INNER_TD_6771_cmds.cmd);
    cfg_JOIN_INNER_TD_6771_cmds.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_6771_cmds_part;
    cfg_JOIN_INNER_TD_6771_cmds_part.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_6771_gqe_join_part (cfg_JOIN_INNER_TD_6771_cmds_part.cmd);
    cfg_JOIN_INNER_TD_6771_cmds_part.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_7773_cmds;
    cfg_JOIN_INNER_TD_7773_cmds.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_7773_gqe_join (cfg_JOIN_INNER_TD_7773_cmds.cmd);
    cfg_JOIN_INNER_TD_7773_cmds.allocateDevBuffer(context_h, 32);
    cfgCmd cfg_JOIN_INNER_TD_7773_cmds_part;
    cfg_JOIN_INNER_TD_7773_cmds_part.allocateHost();
    get_cfg_dat_JOIN_INNER_TD_7773_gqe_join_part (cfg_JOIN_INNER_TD_7773_cmds_part.cmd);
    cfg_JOIN_INNER_TD_7773_cmds_part.allocateDevBuffer(context_h, 32);
    // *************************** Kernel Setup ************************* // 
    bufferTmp buftmp_h(context_h, 0); 
    buftmp_h.initBuffer(q_h); 
    std::cout << std::endl; 
    krnlEngine krnl_JOIN_INNER_TD_4852_part_left;
    krnl_JOIN_INNER_TD_4852_part_left = krnlEngine(program_h, q_h, context_h, "gqePart");
    krnl_JOIN_INNER_TD_4852_part_left.setup_hp(512, 0, power_of_hpTimes_join, tbl_SerializeFromObject_TD_5893_input, tbl_SerializeFromObject_TD_5893_input_partition, cfg_JOIN_INNER_TD_4852_cmds_part);
    krnlEngine krnl_JOIN_INNER_TD_4852[hpTimes_join];
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_INNER_TD_4852[i] = krnlEngine(program_h, q_h, "gqeJoin");
    }
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_INNER_TD_4852[i].setup(tbl_SerializeFromObject_TD_5893_input_partition_array[i], tbl_JOIN_INNER_TD_5573_output_partition_array[i], tbl_JOIN_INNER_TD_4852_output_partition_array[i], cfg_JOIN_INNER_TD_4852_cmds, buftmp_h);
    }
    krnlEngine krnl_JOIN_INNER_TD_4852_build;
    krnl_JOIN_INNER_TD_4852_build = krnlEngine(program_h, q_h, context_h, "gqeKernel", 0);
    krnl_JOIN_INNER_TD_4852_build.setup(tbl_SerializeFromObject_TD_5893_input, tbl_JOIN_INNER_TD_5573_output, tbl_JOIN_INNER_TD_4852_output, cfg_JOIN_INNER_TD_4852_cmds_build, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_4852_probe;
    krnl_JOIN_INNER_TD_4852_probe = krnlEngine(program_h, q_h, context_h, "gqeKernel", 1);
    krnl_JOIN_INNER_TD_4852_probe.setup(tbl_JOIN_INNER_TD_5573_output, tbl_SerializeFromObject_TD_5893_input, tbl_JOIN_INNER_TD_4852_output, cfg_JOIN_INNER_TD_4852_cmds_probe, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_5573_part_right;
    krnl_JOIN_INNER_TD_5573_part_right = krnlEngine(program_h, q_h, context_h, "gqePart");
    krnl_JOIN_INNER_TD_5573_part_right.setup_hp(512, 1, power_of_hpTimes_join, tbl_SerializeFromObject_TD_6468_input, tbl_SerializeFromObject_TD_6468_input_partition, cfg_JOIN_INNER_TD_5573_cmds_part);
    krnlEngine krnl_JOIN_INNER_TD_5573[hpTimes_join];
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_INNER_TD_5573[i] = krnlEngine(program_h, q_h, "gqeJoin");
    }
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_INNER_TD_5573[i].setup(tbl_JOIN_INNER_TD_6771_output_partition_array[i], tbl_SerializeFromObject_TD_6468_input_partition_array[i], tbl_JOIN_INNER_TD_5573_output_partition_array[i], cfg_JOIN_INNER_TD_5573_cmds, buftmp_h);
    }
    krnlEngine krnl_JOIN_INNER_TD_5573_build;
    krnl_JOIN_INNER_TD_5573_build = krnlEngine(program_h, q_h, context_h, "gqeKernel", 0);
    krnl_JOIN_INNER_TD_5573_build.setup(tbl_JOIN_INNER_TD_6771_output, tbl_SerializeFromObject_TD_6468_input, tbl_JOIN_INNER_TD_5573_output, cfg_JOIN_INNER_TD_5573_cmds_build, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_5573_probe;
    krnl_JOIN_INNER_TD_5573_probe = krnlEngine(program_h, q_h, context_h, "gqeKernel", 1);
    krnl_JOIN_INNER_TD_5573_probe.setup(tbl_SerializeFromObject_TD_6468_input, tbl_JOIN_INNER_TD_6771_output, tbl_JOIN_INNER_TD_5573_output, cfg_JOIN_INNER_TD_5573_cmds_probe, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_6771_part_left;
    krnl_JOIN_INNER_TD_6771_part_left = krnlEngine(program_h, q_h, context_h, "gqePart");
    krnl_JOIN_INNER_TD_6771_part_left.setup_hp(512, 0, power_of_hpTimes_join, tbl_Filter_TD_5661_output, tbl_Filter_TD_5661_output_partition, cfg_JOIN_INNER_TD_6771_cmds_part);
    krnlEngine krnl_JOIN_INNER_TD_6771[hpTimes_join];
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_INNER_TD_6771[i] = krnlEngine(program_h, q_h, "gqeJoin");
    }
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_INNER_TD_6771[i].setup(tbl_Filter_TD_5661_output_partition_array[i], tbl_JOIN_INNER_TD_7773_output_partition_array[i], tbl_JOIN_INNER_TD_6771_output_partition_array[i], cfg_JOIN_INNER_TD_6771_cmds, buftmp_h);
    }
    krnlEngine krnl_JOIN_INNER_TD_6771_build;
    krnl_JOIN_INNER_TD_6771_build = krnlEngine(program_h, q_h, context_h, "gqeKernel", 0);
    krnl_JOIN_INNER_TD_6771_build.setup(tbl_Filter_TD_5661_output, tbl_JOIN_INNER_TD_7773_output, tbl_JOIN_INNER_TD_6771_output, cfg_JOIN_INNER_TD_6771_cmds_build, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_6771_probe;
    krnl_JOIN_INNER_TD_6771_probe = krnlEngine(program_h, q_h, context_h, "gqeKernel", 1);
    krnl_JOIN_INNER_TD_6771_probe.setup(tbl_JOIN_INNER_TD_7773_output, tbl_Filter_TD_5661_output, tbl_JOIN_INNER_TD_6771_output, cfg_JOIN_INNER_TD_6771_cmds_probe, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_7773_part_left;
    krnl_JOIN_INNER_TD_7773_part_left = krnlEngine(program_h, q_h, context_h, "gqePart");
    krnl_JOIN_INNER_TD_7773_part_left.setup_hp(512, 0, power_of_hpTimes_join, tbl_SerializeFromObject_TD_8161_input, tbl_SerializeFromObject_TD_8161_input_partition, cfg_JOIN_INNER_TD_7773_cmds_part);
    krnlEngine krnl_JOIN_INNER_TD_7773_part_right;
    krnl_JOIN_INNER_TD_7773_part_right = krnlEngine(program_h, q_h, context_h, "gqePart");
    krnl_JOIN_INNER_TD_7773_part_right.setup_hp(512, 1, power_of_hpTimes_join, tbl_Filter_TD_8761_output, tbl_Filter_TD_8761_output_partition, cfg_JOIN_INNER_TD_7773_cmds_part);
    krnlEngine krnl_JOIN_INNER_TD_7773[hpTimes_join];
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_INNER_TD_7773[i] = krnlEngine(program_h, q_h, "gqeJoin");
    }
    for (int i = 0; i < hpTimes_join; i++) {
        krnl_JOIN_INNER_TD_7773[i].setup(tbl_SerializeFromObject_TD_8161_input_partition_array[i], tbl_Filter_TD_8761_output_partition_array[i], tbl_JOIN_INNER_TD_7773_output_partition_array[i], cfg_JOIN_INNER_TD_7773_cmds, buftmp_h);
    }
    krnlEngine krnl_JOIN_INNER_TD_7773_build;
    krnl_JOIN_INNER_TD_7773_build = krnlEngine(program_h, q_h, context_h, "gqeKernel", 0);
    krnl_JOIN_INNER_TD_7773_build.setup(tbl_SerializeFromObject_TD_8161_input, tbl_Filter_TD_8761_output, tbl_JOIN_INNER_TD_7773_output, cfg_JOIN_INNER_TD_7773_cmds_build, buftmp_h);
    krnlEngine krnl_JOIN_INNER_TD_7773_probe;
    krnl_JOIN_INNER_TD_7773_probe = krnlEngine(program_h, q_h, context_h, "gqeKernel", 1);
    krnl_JOIN_INNER_TD_7773_probe.setup(tbl_Filter_TD_8761_output, tbl_SerializeFromObject_TD_8161_input, tbl_JOIN_INNER_TD_7773_output, cfg_JOIN_INNER_TD_7773_cmds_probe, buftmp_h);
    // ************************** Transfer Engine *********************** // 
    transEngine trans_JOIN_INNER_TD_4852;
    trans_JOIN_INNER_TD_4852.setq(q_h,0);
    trans_JOIN_INNER_TD_4852.add(&(cfg_JOIN_INNER_TD_4852_cmds_part));
    trans_JOIN_INNER_TD_4852.add(&(cfg_JOIN_INNER_TD_4852_cmds));
    trans_JOIN_INNER_TD_4852.add(&(tbl_SerializeFromObject_TD_5893_input));
    transEngine trans_JOIN_INNER_TD_4852_out;
    trans_JOIN_INNER_TD_4852_out.setq(q_h,0);
    q_h.finish();
    transEngine trans_JOIN_INNER_TD_5573;
    trans_JOIN_INNER_TD_5573.setq(q_h,0);
    trans_JOIN_INNER_TD_5573.add(&(cfg_JOIN_INNER_TD_5573_cmds_part));
    trans_JOIN_INNER_TD_5573.add(&(cfg_JOIN_INNER_TD_5573_cmds));
    trans_JOIN_INNER_TD_5573.add(&(tbl_SerializeFromObject_TD_6468_input));
    q_h.finish();
    transEngine trans_JOIN_INNER_TD_6771;
    trans_JOIN_INNER_TD_6771.setq(q_h,0);
    trans_JOIN_INNER_TD_6771.add(&(cfg_JOIN_INNER_TD_6771_cmds_part));
    trans_JOIN_INNER_TD_6771.add(&(cfg_JOIN_INNER_TD_6771_cmds));
    q_h.finish();
    transEngine trans_JOIN_INNER_TD_7773;
    trans_JOIN_INNER_TD_7773.setq(q_h,0);
    trans_JOIN_INNER_TD_7773.add(&(cfg_JOIN_INNER_TD_7773_cmds_part));
    trans_JOIN_INNER_TD_7773.add(&(cfg_JOIN_INNER_TD_7773_cmds));
    trans_JOIN_INNER_TD_7773.add(&(tbl_SerializeFromObject_TD_8161_input));
    q_h.finish();
    // ****************************** Events **************************** // 
    std::vector<cl::Event> events_h2d_wr_JOIN_INNER_TD_4852;
    std::vector<cl::Event> events_d2h_rd_JOIN_INNER_TD_4852;
    std::vector<cl::Event> events_JOIN_INNER_TD_4852[2];
    events_h2d_wr_JOIN_INNER_TD_4852.resize(1);
    events_d2h_rd_JOIN_INNER_TD_4852.resize(1);
    events_JOIN_INNER_TD_4852[0].resize(1);
    events_JOIN_INNER_TD_4852[1].resize(hpTimes_join);
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_4852;
    std::vector<cl::Event> prev_events_grp_JOIN_INNER_TD_4852;
    std::vector<cl::Event> events_h2d_wr_JOIN_INNER_TD_5573;
    std::vector<cl::Event> events_d2h_rd_JOIN_INNER_TD_5573;
    std::vector<cl::Event> events_JOIN_INNER_TD_5573[2];
    events_h2d_wr_JOIN_INNER_TD_5573.resize(1);
    events_d2h_rd_JOIN_INNER_TD_5573.resize(1);
    events_JOIN_INNER_TD_5573[0].resize(1);
    events_JOIN_INNER_TD_5573[1].resize(hpTimes_join);
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_5573;
    std::vector<cl::Event> prev_events_grp_JOIN_INNER_TD_5573;
    std::vector<cl::Event> events_h2d_wr_JOIN_INNER_TD_6771;
    std::vector<cl::Event> events_d2h_rd_JOIN_INNER_TD_6771;
    std::vector<cl::Event> events_JOIN_INNER_TD_6771[2];
    events_h2d_wr_JOIN_INNER_TD_6771.resize(1);
    events_d2h_rd_JOIN_INNER_TD_6771.resize(1);
    events_JOIN_INNER_TD_6771[0].resize(1);
    events_JOIN_INNER_TD_6771[1].resize(hpTimes_join);
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_6771;
    std::vector<cl::Event> prev_events_grp_JOIN_INNER_TD_6771;
    std::vector<cl::Event> events_h2d_wr_JOIN_INNER_TD_7773;
    std::vector<cl::Event> events_d2h_rd_JOIN_INNER_TD_7773;
    std::vector<cl::Event> events_JOIN_INNER_TD_7773[2];
    events_h2d_wr_JOIN_INNER_TD_7773.resize(1);
    events_d2h_rd_JOIN_INNER_TD_7773.resize(1);
    events_JOIN_INNER_TD_7773[0].resize(2);
    events_JOIN_INNER_TD_7773[1].resize(hpTimes_join);
    std::vector<cl::Event> events_grp_JOIN_INNER_TD_7773;
    std::vector<cl::Event> prev_events_grp_JOIN_INNER_TD_7773;
    // **************************** Operations ************************** // 
    struct timeval tv_r_s, tv_r_e; 
    gettimeofday(&tv_r_s, 0); 

    struct timeval tv_r_Filter_8_583_s, tv_r_Filter_8_583_e;
    gettimeofday(&tv_r_Filter_8_583_s, 0);
    SW_Filter_TD_8761(tbl_SerializeFromObject_TD_9263_input, tbl_Filter_TD_8761_output);
    gettimeofday(&tv_r_Filter_8_583_e, 0);

    struct timeval tv_r_Filter_5_171_s, tv_r_Filter_5_171_e;
    gettimeofday(&tv_r_Filter_5_171_s, 0);
    SW_Filter_TD_5661(tbl_SerializeFromObject_TD_695_input, tbl_Filter_TD_5661_output);
    gettimeofday(&tv_r_Filter_5_171_e, 0);

    struct timeval tv_r_JOIN_INNER_7_960_s, tv_r_JOIN_INNER_7_960_e;
    gettimeofday(&tv_r_JOIN_INNER_7_960_s, 0);
    trans_JOIN_INNER_TD_7773.add(&(tbl_Filter_TD_8761_output));
    trans_JOIN_INNER_TD_7773.host2dev(0, &(prev_events_grp_JOIN_INNER_TD_7773), &(events_h2d_wr_JOIN_INNER_TD_7773[0]));
    events_grp_JOIN_INNER_TD_7773.push_back(events_h2d_wr_JOIN_INNER_TD_7773[0]);
    krnl_JOIN_INNER_TD_7773_part_left.run(0, &(events_grp_JOIN_INNER_TD_7773), &(events_JOIN_INNER_TD_7773[0][0]));
    krnl_JOIN_INNER_TD_7773_part_right.run(0, &(events_grp_JOIN_INNER_TD_7773), &(events_JOIN_INNER_TD_7773[0][1]));
    for (int i(0); i < hpTimes_join; ++i) {
        krnl_JOIN_INNER_TD_7773[i].run(0, &(events_JOIN_INNER_TD_7773[0]), &(events_JOIN_INNER_TD_7773[1][i]));
    }
    gettimeofday(&tv_r_JOIN_INNER_7_960_e, 0);

    struct timeval tv_r_JOIN_INNER_6_289_s, tv_r_JOIN_INNER_6_289_e;
    gettimeofday(&tv_r_JOIN_INNER_6_289_s, 0);
    prev_events_grp_JOIN_INNER_TD_6771.push_back(events_h2d_wr_JOIN_INNER_TD_7773[0]);
    trans_JOIN_INNER_TD_6771.add(&(tbl_Filter_TD_5661_output));
    trans_JOIN_INNER_TD_6771.host2dev(0, &(prev_events_grp_JOIN_INNER_TD_6771), &(events_h2d_wr_JOIN_INNER_TD_6771[0]));
    events_grp_JOIN_INNER_TD_6771.push_back(events_h2d_wr_JOIN_INNER_TD_6771[0]);
    events_grp_JOIN_INNER_TD_6771.insert(std::end(events_grp_JOIN_INNER_TD_6771), std::begin(events_JOIN_INNER_TD_7773[0]), std::end(events_JOIN_INNER_TD_7773[0]));
    events_grp_JOIN_INNER_TD_6771.insert(std::end(events_grp_JOIN_INNER_TD_6771), std::begin(events_JOIN_INNER_TD_7773[1]), std::end(events_JOIN_INNER_TD_7773[1]));
    krnl_JOIN_INNER_TD_6771_part_left.run(0, &(events_grp_JOIN_INNER_TD_6771), &(events_JOIN_INNER_TD_6771[0][0]));
    for (int i(0); i < hpTimes_join; ++i) {
        krnl_JOIN_INNER_TD_6771[i].run(0, &(events_JOIN_INNER_TD_6771[0]), &(events_JOIN_INNER_TD_6771[1][i]));
    }
    gettimeofday(&tv_r_JOIN_INNER_6_289_e, 0);

    struct timeval tv_r_JOIN_INNER_5_456_s, tv_r_JOIN_INNER_5_456_e;
    gettimeofday(&tv_r_JOIN_INNER_5_456_s, 0);
    prev_events_grp_JOIN_INNER_TD_5573.push_back(events_h2d_wr_JOIN_INNER_TD_6771[0]);
    trans_JOIN_INNER_TD_5573.host2dev(0, &(prev_events_grp_JOIN_INNER_TD_5573), &(events_h2d_wr_JOIN_INNER_TD_5573[0]));
    events_grp_JOIN_INNER_TD_5573.push_back(events_h2d_wr_JOIN_INNER_TD_5573[0]);
    events_grp_JOIN_INNER_TD_5573.insert(std::end(events_grp_JOIN_INNER_TD_5573), std::begin(events_JOIN_INNER_TD_6771[0]), std::end(events_JOIN_INNER_TD_6771[0]));
    events_grp_JOIN_INNER_TD_5573.insert(std::end(events_grp_JOIN_INNER_TD_5573), std::begin(events_JOIN_INNER_TD_6771[1]), std::end(events_JOIN_INNER_TD_6771[1]));
    krnl_JOIN_INNER_TD_5573_part_right.run(0, &(events_grp_JOIN_INNER_TD_5573), &(events_JOIN_INNER_TD_5573[0][0]));
    for (int i(0); i < hpTimes_join; ++i) {
        krnl_JOIN_INNER_TD_5573[i].run(0, &(events_grp_JOIN_INNER_TD_5573), &(events_JOIN_INNER_TD_5573[1][i]));
    }
    gettimeofday(&tv_r_JOIN_INNER_5_456_e, 0);

    struct timeval tv_r_Filter_4_371_s, tv_r_Filter_4_371_e;
    gettimeofday(&tv_r_Filter_4_371_s, 0);
    SW_Filter_TD_4956(tbl_SerializeFromObject_TD_5740_input, tbl_Filter_TD_4956_output);
    gettimeofday(&tv_r_Filter_4_371_e, 0);

    struct timeval tv_r_JOIN_INNER_4_170_s, tv_r_JOIN_INNER_4_170_e;
    gettimeofday(&tv_r_JOIN_INNER_4_170_s, 0);
    prev_events_grp_JOIN_INNER_TD_4852.push_back(events_h2d_wr_JOIN_INNER_TD_5573[0]);
    trans_JOIN_INNER_TD_4852.host2dev(0, &(prev_events_grp_JOIN_INNER_TD_4852), &(events_h2d_wr_JOIN_INNER_TD_4852[0]));
    events_grp_JOIN_INNER_TD_4852.push_back(events_h2d_wr_JOIN_INNER_TD_4852[0]);
    events_grp_JOIN_INNER_TD_4852.insert(std::end(events_grp_JOIN_INNER_TD_4852), std::begin(events_JOIN_INNER_TD_5573[0]), std::end(events_JOIN_INNER_TD_5573[0]));
    events_grp_JOIN_INNER_TD_4852.insert(std::end(events_grp_JOIN_INNER_TD_4852), std::begin(events_JOIN_INNER_TD_5573[1]), std::end(events_JOIN_INNER_TD_5573[1]));
    krnl_JOIN_INNER_TD_4852_part_left.run(0, &(events_grp_JOIN_INNER_TD_4852), &(events_JOIN_INNER_TD_4852[0][0]));
    for (int i(0); i < hpTimes_join; ++i) {
        krnl_JOIN_INNER_TD_4852[i].run(0, &(events_JOIN_INNER_TD_4852[0]), &(events_JOIN_INNER_TD_4852[1][i]));
    }
    for (int i(0); i < hpTimes_join; ++i) {
        trans_JOIN_INNER_TD_4852_out.add(&(tbl_JOIN_INNER_TD_4852_output_partition_array[i]));
    }
    trans_JOIN_INNER_TD_4852_out.dev2host(0, &(events_JOIN_INNER_TD_4852[1]), &(events_d2h_rd_JOIN_INNER_TD_4852[0]));
    q_h.flush();
    q_h.finish();
    gettimeofday(&tv_r_JOIN_INNER_4_170_e, 0);

    struct timeval tv_r_JOIN_INNER_3_21_s, tv_r_JOIN_INNER_3_21_e;
    gettimeofday(&tv_r_JOIN_INNER_3_21_s, 0);
    SW_JOIN_INNER_TD_3612(tbl_JOIN_INNER_TD_4852_output_partition_array, tbl_Filter_TD_4956_output, tbl_SerializeFromObject_TD_695_input, tbl_JOIN_INNER_TD_3612_output, hpTimes_join);
    gettimeofday(&tv_r_JOIN_INNER_3_21_e, 0);

    struct timeval tv_r_Project_2_84_s, tv_r_Project_2_84_e;
    gettimeofday(&tv_r_Project_2_84_s, 0);
    SW_Project_TD_288(tbl_JOIN_INNER_TD_3612_output, tbl_Project_TD_288_output);
    gettimeofday(&tv_r_Project_2_84_e, 0);

    struct timeval tv_r_Aggregate_1_334_s, tv_r_Aggregate_1_334_e;
    gettimeofday(&tv_r_Aggregate_1_334_s, 0);
    SW_Aggregate_TD_1343(tbl_Project_TD_288_output, tbl_Aggregate_TD_1343_output);
    gettimeofday(&tv_r_Aggregate_1_334_e, 0);

    struct timeval tv_r_Sort_0_7_s, tv_r_Sort_0_7_e;
    gettimeofday(&tv_r_Sort_0_7_s, 0);
    SW_Sort_TD_0502(tbl_Aggregate_TD_1343_output, tbl_Sort_TD_0502_output);
    gettimeofday(&tv_r_Sort_0_7_e, 0);

std::cout << "CPU TIME: " << (tvdiff(&tv_r_Filter_8_583_s, &tv_r_Filter_8_583_e) + 
tvdiff(&tv_r_Filter_5_171_s, &tv_r_Filter_5_171_e) + 
tvdiff(&tv_r_Filter_4_371_s, &tv_r_Filter_4_371_e) + 
tvdiff(&tv_r_JOIN_INNER_3_21_s, &tv_r_JOIN_INNER_3_21_e) + 
tvdiff(&tv_r_Project_2_84_s, &tv_r_Project_2_84_e) + 
tvdiff(&tv_r_Aggregate_1_334_s, &tv_r_Aggregate_1_334_e) + 
tvdiff(&tv_r_Sort_0_7_s, &tv_r_Sort_0_7_e) + 
1) / 1000.0 << std::endl;

std::cout << "FPGA TIME: " << (tvdiff(&tv_r_JOIN_INNER_7_960_s, &tv_r_JOIN_INNER_7_960_e) + 
tvdiff(&tv_r_JOIN_INNER_6_289_s, &tv_r_JOIN_INNER_6_289_e) + 
tvdiff(&tv_r_JOIN_INNER_5_456_s, &tv_r_JOIN_INNER_5_456_e) + 
tvdiff(&tv_r_JOIN_INNER_4_170_s, &tv_r_JOIN_INNER_4_170_e) + 
1) / 1000.0 << std::endl;

    gettimeofday(&tv_r_e, 0); 
    // **************************** Print Execution Time ************************** // 
    std::cout << std::endl; 
    std::cout << "Filter_8: " << tvdiff(&tv_r_Filter_8_583_s, &tv_r_Filter_8_583_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(((l_shipdate#87 >= 19950101) AND (l_shipdate#87 <= 19961231))) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_9263_input.getNumRow() << " -> tbl_SerializeFromObject_TD_9263_input" 
     << std::endl << "      #COL: 5: " << "ListBuffer(l_orderkey#77, l_suppkey#79, l_extendedprice#82, l_discount#83, l_shipdate#87)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_8761_output.getNumRow() << " -> tbl_Filter_TD_8761_output" 
     << std::endl << "      #COL: 5: " << "ListBuffer(l_orderkey#77, l_suppkey#79, l_extendedprice#82, l_discount#83, l_shipdate#87)" 
     << std::endl; 

    std::cout << "Filter_5: " << tvdiff(&tv_r_Filter_5_171_s, &tv_r_Filter_5_171_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(((n_name#145 = FRANCE) OR (n_name#145 = IRAQ))) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_695_input.getNumRow() << " -> tbl_SerializeFromObject_TD_695_input" 
     << std::endl << "      #COL: 2: " << "ListBuffer(n_nationkey#144, n_name#145)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_5661_output.getNumRow() << " -> tbl_Filter_TD_5661_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(n_nationkey#144, n_name#145)" 
     << std::endl; 

    std::cout << "JOIN_INNER_7: " << tvdiff(&tv_r_JOIN_INNER_7_960_s, &tv_r_JOIN_INNER_7_960_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer((s_suppkey#342 = l_suppkey#79)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_8161_input.getNumRow() << " -> tbl_SerializeFromObject_TD_8161_input" 
     << std::endl << "      #COL: 2: " << "ListBuffer(s_suppkey#342, s_nationkey#345)" 
     << std::endl << "      #ROW: " << tbl_Filter_TD_8761_output.getNumRow() << " -> tbl_Filter_TD_8761_output" 
     << std::endl << "      #COL: 5: " << "ListBuffer(l_orderkey#77, l_suppkey#79, l_extendedprice#82, l_discount#83, l_shipdate#87)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_7773_output.getNumRow() << " -> tbl_JOIN_INNER_TD_7773_output" 
     << std::endl << "      #COL: 5: " << "ListBuffer(s_nationkey#345, l_orderkey#77, l_extendedprice#82, l_discount#83, l_shipdate#87)" 
     << std::endl; 

    std::cout << "JOIN_INNER_6: " << tvdiff(&tv_r_JOIN_INNER_6_289_s, &tv_r_JOIN_INNER_6_289_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer((s_nationkey#345 = n_nationkey#144)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_7773_output.getNumRow() << " -> tbl_JOIN_INNER_TD_7773_output" 
     << std::endl << "      #COL: 5: " << "ListBuffer(s_nationkey#345, l_orderkey#77, l_extendedprice#82, l_discount#83, l_shipdate#87)" 
     << std::endl << "      #ROW: " << tbl_Filter_TD_5661_output.getNumRow() << " -> tbl_Filter_TD_5661_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(n_nationkey#144, n_name#145)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_6771_output.getNumRow() << " -> tbl_JOIN_INNER_TD_6771_output" 
     << std::endl << "      #COL: 6: " << "ListBuffer(l_extendedprice#82, l_discount#83, l_shipdate#87, n_name#145, s_nationkey#345, l_orderkey#77)" 
     << std::endl; 

    std::cout << "JOIN_INNER_5: " << tvdiff(&tv_r_JOIN_INNER_5_456_s, &tv_r_JOIN_INNER_5_456_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer((o_orderkey#207 = l_orderkey#77)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_6771_output.getNumRow() << " -> tbl_JOIN_INNER_TD_6771_output" 
     << std::endl << "      #COL: 6: " << "ListBuffer(l_extendedprice#82, l_discount#83, l_shipdate#87, n_name#145, s_nationkey#345, l_orderkey#77)" 
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_6468_input.getNumRow() << " -> tbl_SerializeFromObject_TD_6468_input" 
     << std::endl << "      #COL: 2: " << "ListBuffer(o_orderkey#207, o_custkey#208)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_5573_output.getNumRow() << " -> tbl_JOIN_INNER_TD_5573_output" 
     << std::endl << "      #COL: 6: " << "ListBuffer(s_nationkey#345, l_extendedprice#82, l_discount#83, l_shipdate#87, o_custkey#208, n_name#145)" 
     << std::endl; 

    std::cout << "Filter_4: " << tvdiff(&tv_r_Filter_4_371_s, &tv_r_Filter_4_371_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(((n_name#871 = IRAQ) OR (n_name#871 = FRANCE))) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_5740_input.getNumRow() << " -> tbl_SerializeFromObject_TD_5740_input" 
     << std::endl << "      #COL: 2: " << "ListBuffer(n_nationkey#870, n_name#871)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Filter_TD_4956_output.getNumRow() << " -> tbl_Filter_TD_4956_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(n_nationkey#870, n_name#871)" 
     << std::endl; 

    std::cout << "JOIN_INNER_4: " << tvdiff(&tv_r_JOIN_INNER_4_170_s, &tv_r_JOIN_INNER_4_170_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 1 " 
     << std::endl << "    Operation: ListBuffer((c_custkey#22 = o_custkey#208)) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_5573_output.getNumRow() << " -> tbl_JOIN_INNER_TD_5573_output" 
     << std::endl << "      #COL: 6: " << "ListBuffer(s_nationkey#345, l_extendedprice#82, l_discount#83, l_shipdate#87, o_custkey#208, n_name#145)" 
     << std::endl << "      #ROW: " << tbl_SerializeFromObject_TD_5893_input.getNumRow() << " -> tbl_SerializeFromObject_TD_5893_input" 
     << std::endl << "      #COL: 2: " << "ListBuffer(c_custkey#22, c_nationkey#25)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_4852_output.getNumRow() << " -> tbl_JOIN_INNER_TD_4852_output" 
     << std::endl << "      #COL: 5: " << "ListBuffer(l_extendedprice#82, l_discount#83, l_shipdate#87, c_nationkey#25, n_name#145)" 
     << std::endl; 

    std::cout << "JOIN_INNER_3: " << tvdiff(&tv_r_JOIN_INNER_3_21_s, &tv_r_JOIN_INNER_3_21_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(((c_nationkey#25 = n_nationkey#870) AND (((n_name#145 = FRANCE) AND (n_name#871 = IRAQ)) OR ((n_name#145 = IRAQ) AND (n_name#871 = FRANCE))))) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_4852_output.getNumRow() << " -> tbl_JOIN_INNER_TD_4852_output" 
     << std::endl << "      #COL: 5: " << "ListBuffer(l_extendedprice#82, l_discount#83, l_shipdate#87, c_nationkey#25, n_name#145)" 
     << std::endl << "      #ROW: " << tbl_Filter_TD_4956_output.getNumRow() << " -> tbl_Filter_TD_4956_output" 
     << std::endl << "      #COL: 2: " << "ListBuffer(n_nationkey#870, n_name#871)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_3612_output.getNumRow() << " -> tbl_JOIN_INNER_TD_3612_output" 
     << std::endl << "      #COL: 6: " << "ListBuffer(n_name#145, n_name#871, l_shipdate#87, l_extendedprice#82, l_discount#83, c_nationkey#25)" 
     << std::endl; 

    std::cout << "Project_2: " << tvdiff(&tv_r_Project_2_84_s, &tv_r_Project_2_84_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(n_name#145 AS supp_nation#864, n_name#871 AS cust_nation#865, cast((cast(l_shipdate#87 as double) / 10000.0) as int) AS l_year#866, (l_extendedprice#82 * (100 - l_discount#83)) AS volume#867) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_JOIN_INNER_TD_3612_output.getNumRow() << " -> tbl_JOIN_INNER_TD_3612_output" 
     << std::endl << "      #COL: 6: " << "ListBuffer(n_name#145, n_name#871, l_shipdate#87, l_extendedprice#82, l_discount#83, c_nationkey#25)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Project_TD_288_output.getNumRow() << " -> tbl_Project_TD_288_output" 
     << std::endl << "      #COL: 4: " << "ListBuffer(supp_nation#864, cust_nation#865, l_year#866, volume#867)" 
     << std::endl; 

    std::cout << "Aggregate_1: " << tvdiff(&tv_r_Aggregate_1_334_s, &tv_r_Aggregate_1_334_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(supp_nation#864, cust_nation#865, l_year#866, sum(volume#867) AS revenue#868L) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_Project_TD_288_output.getNumRow() << " -> tbl_Project_TD_288_output" 
     << std::endl << "      #COL: 4: " << "ListBuffer(supp_nation#864, cust_nation#865, l_year#866, volume#867)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_1343_output.getNumRow() << " -> tbl_Aggregate_TD_1343_output" 
     << std::endl << "      #COL: 4: " << "ListBuffer(supp_nation#864, cust_nation#865, l_year#866, revenue#868L)" 
     << std::endl; 

    std::cout << "Sort_0: " << tvdiff(&tv_r_Sort_0_7_s, &tv_r_Sort_0_7_e) / 1000.0 << " ms " 
     << std::endl << "    CPUorFPGA: 0 " 
     << std::endl << "    Operation: ListBuffer(supp_nation#864 ASC NULLS FIRST, cust_nation#865 ASC NULLS FIRST, l_year#866 ASC NULLS FIRST) " 
     << std::endl << "    Input Tables:  "
     << std::endl << "      #ROW: " << tbl_Aggregate_TD_1343_output.getNumRow() << " -> tbl_Aggregate_TD_1343_output" 
     << std::endl << "      #COL: 4: " << "ListBuffer(supp_nation#864, cust_nation#865, l_year#866, revenue#868L)" 
     << std::endl << "    Output Table:  "
     << std::endl << "      #ROW: " << tbl_Sort_TD_0502_output.getNumRow() << " -> tbl_Sort_TD_0502_output" 
     << std::endl << "      #COL: 4: " << "ListBuffer(supp_nation#864, cust_nation#865, l_year#866, revenue#868L)" 
     << std::endl; 

    std::cout << std::endl << " Total execution time: " << tvdiff(&tv_r_s, &tv_r_e) / 1000 << " ms"; 

    std::cout << std::endl << " Spark elapsed time: " << 25.174908 * 1000 << "ms" << std::endl; 
    return 0; 
}
