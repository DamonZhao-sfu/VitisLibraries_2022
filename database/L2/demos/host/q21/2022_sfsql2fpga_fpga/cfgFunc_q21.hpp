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

void get_cfg_dat_JOIN_LEFTSEMI_TD_7346_gqe_join_build(xf::database::gqe::KernelCommand& krn_cmd) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: JOIN_LEFTSEMI
    // Operation: ListBuffer(((l_orderkey#77#1994 = l_orderkey#77) AND NOT (l_suppkey#79#1996 = l_suppkey#79)))
    // Left Table: ListBuffer(l_orderkey#77#1994, l_suppkey#79#1996)
    // Right Table: ListBuffer(l_orderkey#77, l_suppkey#79)
    // Output Table: ListBuffer(l_orderkey#77, l_suppkey#79)
    // Node Depth: 7
    krn_cmd.setJoinOn(1);
    krn_cmd.setJoinType(3);
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

void get_cfg_dat_JOIN_LEFTSEMI_TD_7346_gqe_join_probe(xf::database::gqe::KernelCommand& krn_cmd) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: JOIN_LEFTSEMI
    // Operation: ListBuffer(((l_orderkey#77#1994 = l_orderkey#77) AND NOT (l_suppkey#79#1996 = l_suppkey#79)))
    // Left Table: ListBuffer(l_orderkey#77#1994, l_suppkey#79#1996)
    // Right Table: ListBuffer(l_orderkey#77, l_suppkey#79)
    // Output Table: ListBuffer(l_orderkey#77, l_suppkey#79)
    // Node Depth: 7
    krn_cmd.setJoinOn(1);
    krn_cmd.setJoinOn(3);
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

void get_meta_info_JOIN_LEFTSEMI_TD_7346_gqe_join_build(MetaTable* meta, int nrow, int ncol) {
    meta->setSecID(0);
    meta->setColNum(ncol);
    for(int i=0; i<ncol; i++){
         meta->setCol(i,i,nrow);
    }
}

void get_meta_info_JOIN_LEFTSEMI_TD_7346_gqe_join_probe(MetaTable* meta, int nrow, int ncol) {
    meta->setSecID(0);
    meta->setColNum(ncol);
    for(int i=0; i<ncol; i++){
         meta->setCol(i,i,nrow);
    }
}
void get_cfg_dat_JOIN_LEFTSEMI_TD_7346_gqe_join_part(ap_uint<512>* hbuf) {
    ap_uint<512>* b = hbuf;
    memset(b, 0, sizeof(ap_uint<512>) * 9);
    ap_uint<512> t = 0;

    // input table a
    signed char id_a[] = {0,1,-1,-1,-1,-1,-1,-1};
    for (int c = 0; c < 8; ++c) {
        t.range(56 + 8 * c + 7, 56 + 8 * c) = id_a[c];
    }
    // filter tbl_a config
    uint32_t cfga[45];
    gen_pass_fcfg(cfga);
    memcpy(&b[3], cfga, sizeof(uint32_t) * 45);

    // input table b
    signed char id_b[] = {0,1,-1,-1,-1,-1,-1,-1};
    for (int c = 0; c < 8; ++c) {
        t.range(120 + 8 * c + 7, 120 + 8 * c) = id_b[c];
    }
    // filter tbl_b config
    uint32_t cfgb[45];
    gen_pass_fcfg(cfgb);
    memcpy(&b[6], cfgb, sizeof(uint32_t) * 45);

    // join config
    t.set_bit(0, 1);    // join
    t.set_bit(2, 0);    // dual-key
    t.range(5, 3) = 3;  // hash join flag = 0 for normal, 1 for semi, 2 for anti

    b[0] = t;
}

void get_cfg_dat_JOIN_LEFTSEMI_TD_6827_gqe_join_build(xf::database::gqe::KernelCommand& krn_cmd) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: JOIN_LEFTSEMI
    // Operation: ListBuffer(((l_orderkey#77#2010 = l_orderkey#77) AND NOT (l_suppkey#79#2012 = l_suppkey#79)))
    // Left Table: ListBuffer(l_orderkey#77#2010, l_suppkey#79#2012)
    // Right Table: ListBuffer(l_orderkey#77, l_suppkey#79)
    // Output Table: ListBuffer(l_orderkey#77, l_suppkey#79)
    // Node Depth: 6
    krn_cmd.setJoinOn(1);
    krn_cmd.setJoinType(3);
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

void get_cfg_dat_JOIN_LEFTSEMI_TD_6827_gqe_join_probe(xf::database::gqe::KernelCommand& krn_cmd) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: JOIN_LEFTSEMI
    // Operation: ListBuffer(((l_orderkey#77#2010 = l_orderkey#77) AND NOT (l_suppkey#79#2012 = l_suppkey#79)))
    // Left Table: ListBuffer(l_orderkey#77#2010, l_suppkey#79#2012)
    // Right Table: ListBuffer(l_orderkey#77, l_suppkey#79)
    // Output Table: ListBuffer(l_orderkey#77, l_suppkey#79)
    // Node Depth: 6
    krn_cmd.setJoinOn(1);
    krn_cmd.setJoinOn(3);
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

void get_meta_info_JOIN_LEFTSEMI_TD_6827_gqe_join_build(MetaTable* meta, int nrow, int ncol) {
    meta->setSecID(0);
    meta->setColNum(ncol);
    for(int i=0; i<ncol; i++){
         meta->setCol(i,i,nrow);
    }
}

void get_meta_info_JOIN_LEFTSEMI_TD_6827_gqe_join_probe(MetaTable* meta, int nrow, int ncol) {
    meta->setSecID(0);
    meta->setColNum(ncol);
    for(int i=0; i<ncol; i++){
         meta->setCol(i,i,nrow);
    }
}
void get_cfg_dat_JOIN_LEFTSEMI_TD_6827_gqe_join_part(ap_uint<512>* hbuf) {
    ap_uint<512>* b = hbuf;
    memset(b, 0, sizeof(ap_uint<512>) * 9);
    ap_uint<512> t = 0;

    // input table a
    signed char id_a[] = {0,1,-1,-1,-1,-1,-1,-1};
    for (int c = 0; c < 8; ++c) {
        t.range(56 + 8 * c + 7, 56 + 8 * c) = id_a[c];
    }
    // filter tbl_a config
    uint32_t cfga[45];
    gen_pass_fcfg(cfga);
    memcpy(&b[3], cfga, sizeof(uint32_t) * 45);

    // input table b
    signed char id_b[] = {0,1,-1,-1,-1,-1,-1,-1};
    for (int c = 0; c < 8; ++c) {
        t.range(120 + 8 * c + 7, 120 + 8 * c) = id_b[c];
    }
    // filter tbl_b config
    uint32_t cfgb[45];
    gen_pass_fcfg(cfgb);
    memcpy(&b[6], cfgb, sizeof(uint32_t) * 45);

    // join config
    t.set_bit(0, 1);    // join
    t.set_bit(2, 0);    // dual-key
    t.range(5, 3) = 3;  // hash join flag = 0 for normal, 1 for semi, 2 for anti

    b[0] = t;
}

void get_cfg_dat_JOIN_LEFTANTI_TD_5733_gqe_join_build(xf::database::gqe::KernelCommand& krn_cmd) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: JOIN_LEFTANTI
    // Operation: ListBuffer(((l_orderkey#77#2010 = l_orderkey#77) AND NOT (l_suppkey#79#2012 = l_suppkey#79)))
    // Left Table: ListBuffer(l_orderkey#77, l_suppkey#79)
    // Right Table: ListBuffer(l_orderkey#77#2010, l_suppkey#79#2012)
    // Output Table: ListBuffer(l_orderkey#77, l_suppkey#79)
    // Node Depth: 5
    krn_cmd.setJoinOn(1);
    krn_cmd.setJoinType(2);
    std::vector<int8_t> enable_A;
    enable_A.push_back(0);
    enable_A.push_back(1);
    enable_A.push_back(-1);
    krn_cmd.setScanColEnable(0, enable_A);

    std::vector<int8_t> enable_B;
    enable_B.push_back(0);
    enable_B.push_back(1);
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

void get_cfg_dat_JOIN_LEFTANTI_TD_5733_gqe_join_probe(xf::database::gqe::KernelCommand& krn_cmd) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: JOIN_LEFTANTI
    // Operation: ListBuffer(((l_orderkey#77#2010 = l_orderkey#77) AND NOT (l_suppkey#79#2012 = l_suppkey#79)))
    // Left Table: ListBuffer(l_orderkey#77, l_suppkey#79)
    // Right Table: ListBuffer(l_orderkey#77#2010, l_suppkey#79#2012)
    // Output Table: ListBuffer(l_orderkey#77, l_suppkey#79)
    // Node Depth: 5
    krn_cmd.setJoinOn(1);
    krn_cmd.setJoinOn(2);
    std::vector<int8_t> enable_A;
    enable_A.push_back(0);
    enable_A.push_back(1);
    enable_A.push_back(-1);
    krn_cmd.setScanColEnable(0, enable_A);

    std::vector<int8_t> enable_B;
    enable_B.push_back(0);
    enable_B.push_back(1);
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

void get_meta_info_JOIN_LEFTANTI_TD_5733_gqe_join_build(MetaTable* meta, int nrow, int ncol) {
    meta->setSecID(0);
    meta->setColNum(ncol);
    for(int i=0; i<ncol; i++){
         meta->setCol(i,i,nrow);
    }
}

void get_meta_info_JOIN_LEFTANTI_TD_5733_gqe_join_probe(MetaTable* meta, int nrow, int ncol) {
    meta->setSecID(0);
    meta->setColNum(ncol);
    for(int i=0; i<ncol; i++){
         meta->setCol(i,i,nrow);
    }
}
void get_cfg_dat_JOIN_LEFTANTI_TD_5733_gqe_join_part(ap_uint<512>* hbuf) {
    ap_uint<512>* b = hbuf;
    memset(b, 0, sizeof(ap_uint<512>) * 9);
    ap_uint<512> t = 0;

    // input table a
    signed char id_a[] = {0,1,-1,-1,-1,-1,-1,-1};
    for (int c = 0; c < 8; ++c) {
        t.range(56 + 8 * c + 7, 56 + 8 * c) = id_a[c];
    }
    // filter tbl_a config
    uint32_t cfga[45];
    gen_pass_fcfg(cfga);
    memcpy(&b[3], cfga, sizeof(uint32_t) * 45);

    // input table b
    signed char id_b[] = {0,1,-1,-1,-1,-1,-1,-1};
    for (int c = 0; c < 8; ++c) {
        t.range(120 + 8 * c + 7, 120 + 8 * c) = id_b[c];
    }
    // filter tbl_b config
    uint32_t cfgb[45];
    gen_pass_fcfg(cfgb);
    memcpy(&b[6], cfgb, sizeof(uint32_t) * 45);

    // join config
    t.set_bit(0, 1);    // join
    t.set_bit(2, 1);    // dual-key
    t.range(5, 3) = 2;  // hash join flag = 0 for normal, 1 for semi, 2 for anti

    b[0] = t;
}

void get_cfg_dat_JOIN_INNER_TD_3653_gqe_join_build(xf::database::gqe::KernelCommand& krn_cmd) {
    // StringRowIDSubstitution: true StringRowIDBackSubstitution: false
    // Supported operation: JOIN_INNER
    // Operation: ListBuffer((s_nationkey#345 = n_nationkey#144))
    // Left Table: ListBuffer(s_suppkey#342, s_name#343, s_nationkey#345)
    // Right Table: ListBuffer(n_nationkey#144)
    // Output Table: ListBuffer(s_suppkey#342, s_name#343)
    // Node Depth: 3
    krn_cmd.setJoinOn(1);
    krn_cmd.setJoinType(0);
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

void get_cfg_dat_JOIN_INNER_TD_3653_gqe_join_probe(xf::database::gqe::KernelCommand& krn_cmd) {
    // StringRowIDSubstitution: true StringRowIDBackSubstitution: false
    // Supported operation: JOIN_INNER
    // Operation: ListBuffer((s_nationkey#345 = n_nationkey#144))
    // Left Table: ListBuffer(s_suppkey#342, s_name#343, s_nationkey#345)
    // Right Table: ListBuffer(n_nationkey#144)
    // Output Table: ListBuffer(s_suppkey#342, s_name#343)
    // Node Depth: 3
    krn_cmd.setJoinOn(1);
    krn_cmd.setJoinOn(0);
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

void get_meta_info_JOIN_INNER_TD_3653_gqe_join_build(MetaTable* meta, int nrow, int ncol) {
    meta->setSecID(0);
    meta->setColNum(ncol);
    for(int i=0; i<ncol; i++){
         meta->setCol(i,i,nrow);
    }
}

void get_meta_info_JOIN_INNER_TD_3653_gqe_join_probe(MetaTable* meta, int nrow, int ncol) {
    meta->setSecID(0);
    meta->setColNum(ncol);
    for(int i=0; i<ncol; i++){
         meta->setCol(i,i,nrow);
    }
}
void get_cfg_dat_JOIN_INNER_TD_3653_gqe_join_part(ap_uint<512>* hbuf) {
    ap_uint<512>* b = hbuf;
    memset(b, 0, sizeof(ap_uint<512>) * 9);
    ap_uint<512> t = 0;

    // input table a
    signed char id_a[] = {2,1,0,-1,-1,-1,-1,-1};
    for (int c = 0; c < 8; ++c) {
        t.range(56 + 8 * c + 7, 56 + 8 * c) = id_a[c];
    }
    // filter tbl_a config
    uint32_t cfga[45];
    gen_pass_fcfg(cfga);
    memcpy(&b[3], cfga, sizeof(uint32_t) * 45);

    // input table b
    signed char id_b[] = {0,-1,-1,-1,-1,-1,-1,-1};
    for (int c = 0; c < 8; ++c) {
        t.range(120 + 8 * c + 7, 120 + 8 * c) = id_b[c];
    }
    // filter tbl_b config
    uint32_t cfgb[45];
    gen_pass_fcfg(cfgb);
    memcpy(&b[6], cfgb, sizeof(uint32_t) * 45);

    // join config
    t.set_bit(0, 1);    // join
    t.set_bit(2, 0);    // dual-key
    t.range(5, 3) = 0;  // hash join flag = 0 for normal, 1 for semi, 2 for anti

    b[0] = t;
}

void get_cfg_dat_JOIN_INNER_TD_4616_gqe_join_build(xf::database::gqe::KernelCommand& krn_cmd) {
    // StringRowIDSubstitution: true StringRowIDBackSubstitution: false
    // Supported operation: JOIN_INNER
    // Operation: ListBuffer((s_suppkey#342 = l_suppkey#79))
    // Left Table: ListBuffer(s_suppkey#342, s_name#343)
    // Right Table: ListBuffer(l_orderkey#77, l_suppkey#79)
    // Output Table: ListBuffer(s_name#343, l_orderkey#77)
    // Node Depth: 4
    krn_cmd.setJoinOn(1);
    krn_cmd.setJoinType(0);
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

void get_cfg_dat_JOIN_INNER_TD_4616_gqe_join_probe(xf::database::gqe::KernelCommand& krn_cmd) {
    // StringRowIDSubstitution: true StringRowIDBackSubstitution: false
    // Supported operation: JOIN_INNER
    // Operation: ListBuffer((s_suppkey#342 = l_suppkey#79))
    // Left Table: ListBuffer(s_suppkey#342, s_name#343)
    // Right Table: ListBuffer(l_orderkey#77, l_suppkey#79)
    // Output Table: ListBuffer(s_name#343, l_orderkey#77)
    // Node Depth: 4
    krn_cmd.setJoinOn(1);
    krn_cmd.setJoinOn(0);
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

void get_meta_info_JOIN_INNER_TD_4616_gqe_join_build(MetaTable* meta, int nrow, int ncol) {
    meta->setSecID(0);
    meta->setColNum(ncol);
    for(int i=0; i<ncol; i++){
         meta->setCol(i,i,nrow);
    }
}

void get_meta_info_JOIN_INNER_TD_4616_gqe_join_probe(MetaTable* meta, int nrow, int ncol) {
    meta->setSecID(0);
    meta->setColNum(ncol);
    for(int i=0; i<ncol; i++){
         meta->setCol(i,i,nrow);
    }
}
void get_cfg_dat_JOIN_INNER_TD_4616_gqe_join_part(ap_uint<512>* hbuf) {
    ap_uint<512>* b = hbuf;
    memset(b, 0, sizeof(ap_uint<512>) * 9);
    ap_uint<512> t = 0;

    // input table a
    signed char id_a[] = {0,1,-1,-1,-1,-1,-1,-1};
    for (int c = 0; c < 8; ++c) {
        t.range(56 + 8 * c + 7, 56 + 8 * c) = id_a[c];
    }
    // filter tbl_a config
    uint32_t cfga[45];
    gen_pass_fcfg(cfga);
    memcpy(&b[3], cfga, sizeof(uint32_t) * 45);

    // input table b
    signed char id_b[] = {1,0,-1,-1,-1,-1,-1,-1};
    for (int c = 0; c < 8; ++c) {
        t.range(120 + 8 * c + 7, 120 + 8 * c) = id_b[c];
    }
    // filter tbl_b config
    uint32_t cfgb[45];
    gen_pass_fcfg(cfgb);
    memcpy(&b[6], cfgb, sizeof(uint32_t) * 45);

    // join config
    t.set_bit(0, 1);    // join
    t.set_bit(2, 0);    // dual-key
    t.range(5, 3) = 0;  // hash join flag = 0 for normal, 1 for semi, 2 for anti

    b[0] = t;
}

void get_cfg_dat_JOIN_INNER_TD_2684_gqe_join_build(xf::database::gqe::KernelCommand& krn_cmd) {
    // StringRowIDSubstitution: true StringRowIDBackSubstitution: false
    // Supported operation: JOIN_INNER
    // Operation: ListBuffer((o_orderkey#207 = l_orderkey#77))
    // Left Table: ListBuffer(s_name#343, l_orderkey#77)
    // Right Table: ListBuffer(o_orderkey#207)
    // Output Table: ListBuffer(s_name#343)
    // Node Depth: 2
    krn_cmd.setJoinOn(1);
    krn_cmd.setJoinType(0);
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

void get_cfg_dat_JOIN_INNER_TD_2684_gqe_join_probe(xf::database::gqe::KernelCommand& krn_cmd) {
    // StringRowIDSubstitution: true StringRowIDBackSubstitution: false
    // Supported operation: JOIN_INNER
    // Operation: ListBuffer((o_orderkey#207 = l_orderkey#77))
    // Left Table: ListBuffer(s_name#343, l_orderkey#77)
    // Right Table: ListBuffer(o_orderkey#207)
    // Output Table: ListBuffer(s_name#343)
    // Node Depth: 2
    krn_cmd.setJoinOn(1);
    krn_cmd.setJoinOn(0);
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

void get_meta_info_JOIN_INNER_TD_2684_gqe_join_build(MetaTable* meta, int nrow, int ncol) {
    meta->setSecID(0);
    meta->setColNum(ncol);
    for(int i=0; i<ncol; i++){
         meta->setCol(i,i,nrow);
    }
}

void get_meta_info_JOIN_INNER_TD_2684_gqe_join_probe(MetaTable* meta, int nrow, int ncol) {
    meta->setSecID(0);
    meta->setColNum(ncol);
    for(int i=0; i<ncol; i++){
         meta->setCol(i,i,nrow);
    }
}
void get_cfg_dat_JOIN_INNER_TD_2684_gqe_join_part(ap_uint<512>* hbuf) {
    ap_uint<512>* b = hbuf;
    memset(b, 0, sizeof(ap_uint<512>) * 9);
    ap_uint<512> t = 0;

    // input table a
    signed char id_a[] = {1,0,-1,-1,-1,-1,-1,-1};
    for (int c = 0; c < 8; ++c) {
        t.range(56 + 8 * c + 7, 56 + 8 * c) = id_a[c];
    }
    // filter tbl_a config
    uint32_t cfga[45];
    gen_pass_fcfg(cfga);
    memcpy(&b[3], cfga, sizeof(uint32_t) * 45);

    // input table b
    signed char id_b[] = {0,-1,-1,-1,-1,-1,-1,-1};
    for (int c = 0; c < 8; ++c) {
        t.range(120 + 8 * c + 7, 120 + 8 * c) = id_b[c];
    }
    // filter tbl_b config
    uint32_t cfgb[45];
    gen_pass_fcfg(cfgb);
    memcpy(&b[6], cfgb, sizeof(uint32_t) * 45);

    // join config
    t.set_bit(0, 1);    // join
    t.set_bit(2, 0);    // dual-key
    t.range(5, 3) = 0;  // hash join flag = 0 for normal, 1 for semi, 2 for anti

    b[0] = t;
}

