#include "ap_int.h" 
#include "xf_database/dynamic_alu_host.hpp"
#include "xf_database/enums.hpp"
#include <fstream> 
 
static void gen_pass_fcfg(uint32_t cfg[]) { 
    using namespace xf::database; 
    int n = 0; 
 
    // cond_1 
    cfg[n++] = (uint32_t)0UL; 
    cfg[n++] = (uint32_t)0UL; 
    cfg[n++] = 0UL | (FOP_DC << FilterOpWidth) | (FOP_DC); 
    // cond_2 
    cfg[n++] = (uint32_t)0UL; 
    cfg[n++] = (uint32_t)0UL; 
    cfg[n++] = 0UL | (FOP_DC << FilterOpWidth) | (FOP_DC); 
    // cond_3 
    cfg[n++] = (uint32_t)0UL; 
    cfg[n++] = (uint32_t)0UL; 
    cfg[n++] = 0UL | (FOP_DC << FilterOpWidth) | (FOP_DC); 
    // cond_4 
    cfg[n++] = (uint32_t)0UL; 
    cfg[n++] = (uint32_t)0UL; 
    cfg[n++] = 0UL | (FOP_DC << FilterOpWidth) | (FOP_DC); 
 
    uint32_t r = 0; 
    int sh = 0; 
    // cond_1 -- cond_2 
    r |= ((uint32_t)(FOP_DC << sh)); 
    sh += FilterOpWidth; 
    // cond_1 -- cond_3 
    r |= ((uint32_t)(FOP_DC << sh)); 
    sh += FilterOpWidth; 
    // cond_1 -- cond_4 
    r |= ((uint32_t)(FOP_DC << sh)); 
    sh += FilterOpWidth; 
 
    // cond_2 -- cond_3 
    r |= ((uint32_t)(FOP_DC << sh)); 
    sh += FilterOpWidth; 
    // cond_2 -- cond_4 
    r |= ((uint32_t)(FOP_DC << sh)); 
    sh += FilterOpWidth; 
 
    // cond_3 -- cond_4 
    r |= ((uint32_t)(FOP_DC << sh)); 
    sh += FilterOpWidth; 
 
    cfg[n++] = r; 
 
    // 4 true and 6 true 
    cfg[n++] = (uint32_t)0UL; 
    cfg[n++] = (uint32_t)0UL; 
    cfg[n++] = (uint32_t)0UL; 
    cfg[n++] = (uint32_t)0UL; 
    cfg[n++] = (uint32_t)0UL; 
    cfg[n++] = (uint32_t)0UL; 
    cfg[n++] = (uint32_t)0UL; 
    cfg[n++] = (uint32_t)0UL; 
    cfg[n++] = (uint32_t)0UL; 
    cfg[n++] = (uint32_t)0UL; 
    cfg[n++] = (uint32_t)0UL; 
    cfg[n++] = (uint32_t)0UL; 
    cfg[n++] = (uint32_t)0UL; 
    cfg[n++] = (uint32_t)0UL; 
    cfg[n++] = (uint32_t)0UL; 
    cfg[n++] = (uint32_t)0UL; 
    cfg[n++] = (uint32_t)0UL; 
    cfg[n++] = (uint32_t)0UL; 
    cfg[n++] = (uint32_t)0UL; 
    cfg[n++] = (uint32_t)0UL; 
    cfg[n++] = (uint32_t)0UL; 
    cfg[n++] = (uint32_t)0UL; 
    cfg[n++] = (uint32_t)0UL; 
    cfg[n++] = (uint32_t)0UL; 
    cfg[n++] = (uint32_t)0UL; 
    cfg[n++] = (uint32_t)0UL; 
    cfg[n++] = (uint32_t)0UL; 
    cfg[n++] = (uint32_t)0UL; 
    cfg[n++] = (uint32_t)0UL; 
    cfg[n++] = (uint32_t)0UL; 
    cfg[n++] = (uint32_t)0UL; 
    cfg[n++] = (uint32_t)(1UL << 31); 
} 

void get_cfg_dat_JOIN_LEFTANTI_TD_3869_gqe_join_build(xf::database::gqe::KernelCommand& krn_cmd) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: JOIN_LEFTANTI
    // Operation: ListBuffer(((ps_suppkey#306 = s_suppkey#342) OR isnull((ps_suppkey#306 = s_suppkey#342))))
    // Left Table: ListBuffer(s_suppkey#342)
    // Right Table: ListBuffer(ps_partkey#305, ps_suppkey#306)
    // Output Table: ListBuffer(ps_partkey#305, ps_suppkey#306)
    // Node Depth: 3
    krn_cmd.setJoinOn(1);
    krn_cmd.setDualKeyOn(0);
    krn_cmd.setJoinType(2);
    krn_cmd.setJoinBuildProbe(0);
    krn_cmd.setRowIDValidEnable(0, 1, 0);
    std::vector<int8_t> enable_A;
    enable_A.push_back(0);
    enable_A.push_back(-1);
    enable_A.push_back(-1);
    krn_cmd.setScanColEnable(0, enable_A);

    std::vector<int8_t> enable_B;
    enable_B.push_back(0);
    enable_B.push_back(-1);
    enable_B.push_back(-1);
    krn_cmd.setScanColEnable(1, enable_B);
    std::vector<int8_t> enable_C;
    enable_C.push_back(0);
    enable_C.push_back(1);
    enable_C.push_back(2);
    enable_C.push_back(3);
    krn_cmd.setWriteColEnable(0, 1, enable_C);
    krn_cmd.setWriteColEnable(0, 0, enable_C);
}

void get_cfg_dat_JOIN_LEFTANTI_TD_3869_gqe_join_probe(xf::database::gqe::KernelCommand& krn_cmd) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: JOIN_LEFTANTI
    // Operation: ListBuffer(((ps_suppkey#306 = s_suppkey#342) OR isnull((ps_suppkey#306 = s_suppkey#342))))
    // Left Table: ListBuffer(s_suppkey#342)
    // Right Table: ListBuffer(ps_partkey#305, ps_suppkey#306)
    // Output Table: ListBuffer(ps_partkey#305, ps_suppkey#306)
    // Node Depth: 3
    krn_cmd.setJoinOn(1);
    krn_cmd.setDualKeyOn(0);
    krn_cmd.setJoinType(2);
    krn_cmd.setJoinBuildProbe(1);
    krn_cmd.setRowIDValidEnable(1, 1, 0);
    std::vector<int8_t> enable_A;
    enable_A.push_back(0);
    enable_A.push_back(-1);
    enable_A.push_back(-1);
    krn_cmd.setScanColEnable(0, enable_A);

    std::vector<int8_t> enable_B;
    enable_B.push_back(0);
    enable_B.push_back(-1);
    enable_B.push_back(-1);
    krn_cmd.setScanColEnable(1, enable_B);
    std::vector<int8_t> enable_C;
    enable_C.push_back(0);
    enable_C.push_back(1);
    enable_C.push_back(2);
    enable_C.push_back(3);
    krn_cmd.setWriteColEnable(0, 1, enable_C);
    krn_cmd.setWriteColEnable(0, 0, enable_C);
}

void get_meta_info_JOIN_LEFTANTI_TD_3869_gqe_join_build(MetaTable* meta, int nrow, int ncol) {
    meta->setSecID(0);
    meta->setColNum(ncol);
    for(int i=0; i<ncol; i++){
         meta->setCol(i,i,nrow);
    }
}

void get_meta_info_JOIN_LEFTANTI_TD_3869_gqe_join_probe(MetaTable* meta, int nrow, int ncol) {
    meta->setSecID(0);
    meta->setColNum(ncol);
    for(int i=0; i<ncol; i++){
         meta->setCol(i,i,nrow);
    }
}

void get_cfg_dat_JOIN_INNER_TD_2624_gqe_join_build(xf::database::gqe::KernelCommand& krn_cmd) {
    // StringRowIDSubstitution: true StringRowIDBackSubstitution: false
    // Supported operation: JOIN_INNER
    // Operation: ListBuffer((p_partkey#258 = ps_partkey#305))
    // Left Table: ListBuffer(p_partkey#258, p_brand#261, p_type#262, p_size#263)
    // Right Table: ListBuffer(ps_partkey#305, ps_suppkey#306)
    // Output Table: ListBuffer(ps_suppkey#306, p_brand#261, p_type#262, p_size#263)
    // Node Depth: 2
    krn_cmd.setJoinOn(1);
    krn_cmd.setDualKeyOn(0);
    krn_cmd.setJoinType(0);
    krn_cmd.setJoinBuildProbe(0);
    krn_cmd.setRowIDValidEnable(0, 1, 0);
    std::vector<int8_t> enable_A;
    enable_A.push_back(0);
    enable_A.push_back(-1);
    enable_A.push_back(-1);
    krn_cmd.setScanColEnable(0, enable_A);

    std::vector<int8_t> enable_B;
    enable_B.push_back(0);
    enable_B.push_back(-1);
    enable_B.push_back(-1);
    krn_cmd.setScanColEnable(1, enable_B);
    std::vector<int8_t> enable_C;
    enable_C.push_back(0);
    enable_C.push_back(1);
    enable_C.push_back(2);
    enable_C.push_back(3);
    krn_cmd.setWriteColEnable(0, 1, enable_C);
    krn_cmd.setWriteColEnable(0, 0, enable_C);
}

void get_cfg_dat_JOIN_INNER_TD_2624_gqe_join_probe(xf::database::gqe::KernelCommand& krn_cmd) {
    // StringRowIDSubstitution: true StringRowIDBackSubstitution: false
    // Supported operation: JOIN_INNER
    // Operation: ListBuffer((p_partkey#258 = ps_partkey#305))
    // Left Table: ListBuffer(p_partkey#258, p_brand#261, p_type#262, p_size#263)
    // Right Table: ListBuffer(ps_partkey#305, ps_suppkey#306)
    // Output Table: ListBuffer(ps_suppkey#306, p_brand#261, p_type#262, p_size#263)
    // Node Depth: 2
    krn_cmd.setJoinOn(1);
    krn_cmd.setDualKeyOn(0);
    krn_cmd.setJoinType(0);
    krn_cmd.setJoinBuildProbe(1);
    krn_cmd.setRowIDValidEnable(1, 1, 0);
    std::vector<int8_t> enable_A;
    enable_A.push_back(0);
    enable_A.push_back(-1);
    enable_A.push_back(-1);
    krn_cmd.setScanColEnable(0, enable_A);

    std::vector<int8_t> enable_B;
    enable_B.push_back(0);
    enable_B.push_back(-1);
    enable_B.push_back(-1);
    krn_cmd.setScanColEnable(1, enable_B);
    std::vector<int8_t> enable_C;
    enable_C.push_back(0);
    enable_C.push_back(1);
    enable_C.push_back(2);
    enable_C.push_back(3);
    krn_cmd.setWriteColEnable(0, 1, enable_C);
    krn_cmd.setWriteColEnable(0, 0, enable_C);
}

void get_meta_info_JOIN_INNER_TD_2624_gqe_join_build(MetaTable* meta, int nrow, int ncol) {
    meta->setSecID(0);
    meta->setColNum(ncol);
    for(int i=0; i<ncol; i++){
         meta->setCol(i,i,nrow);
    }
}

void get_meta_info_JOIN_INNER_TD_2624_gqe_join_probe(MetaTable* meta, int nrow, int ncol) {
    meta->setSecID(0);
    meta->setColNum(ncol);
    for(int i=0; i<ncol; i++){
         meta->setCol(i,i,nrow);
    }
}

