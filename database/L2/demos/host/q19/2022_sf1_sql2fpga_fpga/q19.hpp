#include <regex> 
#include <stdint.h> 

void SW_Filter_TD_2178(Table &tbl_SerializeFromObject_TD_3242_input, Table &tbl_Filter_TD_2178_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: Filter
    // Operation: ListBuffer(((p_size#263 >= 1) AND (((((p_brand#261 = Brand#51) AND p_container#264 IN (SM CASE,SM BOX,SM PACK,SM PKG)) AND (p_size#263 <= 5)) OR (((p_brand#261 = Brand#32) AND p_container#264 IN (MED BAG,MED BOX,MED PKG,MED PACK)) AND (p_size#263 <= 10))) OR (((p_brand#261 = Brand#12) AND p_container#264 IN (LG CASE,LG BOX,LG PACK,LG PKG)) AND (p_size#263 <= 15)))))
    // Input: ListBuffer(p_partkey#258, p_brand#261, p_size#263, p_container#264)
    // Output: ListBuffer(p_partkey#258, p_brand#261, p_size#263, p_container#264)
    int r = 0;
    int nrow1 = tbl_SerializeFromObject_TD_3242_input.getNumRow();
    for (int i = 0; i < nrow1; i++) {
        int32_t _p_size263 = tbl_SerializeFromObject_TD_3242_input.getInt32(i, 2);
        std::array<char, TPCH_READ_P_BRND_LEN + 1> _p_brand261 = tbl_SerializeFromObject_TD_3242_input.getcharN<char, TPCH_READ_P_BRND_LEN + 1>(i, 1);
        std::array<char, TPCH_READ_P_CNTR_LEN + 1> _p_container264 = tbl_SerializeFromObject_TD_3242_input.getcharN<char, TPCH_READ_P_CNTR_LEN + 1>(i, 3);
        auto reuse_col_str_560 = std::string(_p_container264.data());
        auto reuse_col_str_868 = std::string(_p_container264.data());
        auto reuse_col_str_916 = std::string(_p_container264.data());
        if ((_p_size263 >= 1) && (((((std::string(_p_brand261.data()) == "Brand#51") && ((reuse_col_str_560 == "SM CASE") || (reuse_col_str_560 == "SM BOX") || (reuse_col_str_560 == "SM PACK") || (reuse_col_str_560 == "SM PKG") || (0))) && (_p_size263 <= 5)) || (((std::string(_p_brand261.data()) == "Brand#32") && ((reuse_col_str_868 == "MED BAG") || (reuse_col_str_868 == "MED BOX") || (reuse_col_str_868 == "MED PKG") || (reuse_col_str_868 == "MED PACK") || (0))) && (_p_size263 <= 10))) || (((std::string(_p_brand261.data()) == "Brand#12") && ((reuse_col_str_916 == "LG CASE") || (reuse_col_str_916 == "LG BOX") || (reuse_col_str_916 == "LG PACK") || (reuse_col_str_916 == "LG PKG") || (0))) && (_p_size263 <= 15)))) {
            int32_t _p_partkey258_t = tbl_SerializeFromObject_TD_3242_input.getInt32(i, 0);
            tbl_Filter_TD_2178_output.setInt32(r, 0, _p_partkey258_t);
            std::array<char, TPCH_READ_P_BRND_LEN + 1> _p_brand261_t = tbl_SerializeFromObject_TD_3242_input.getcharN<char, TPCH_READ_P_BRND_LEN + 1>(i, 1);
            tbl_Filter_TD_2178_output.setcharN<char, TPCH_READ_P_BRND_LEN + 1>(r, 1, _p_brand261_t);
            int32_t _p_size263_t = tbl_SerializeFromObject_TD_3242_input.getInt32(i, 2);
            tbl_Filter_TD_2178_output.setInt32(r, 2, _p_size263_t);
            std::array<char, TPCH_READ_P_CNTR_LEN + 1> _p_container264_t = tbl_SerializeFromObject_TD_3242_input.getcharN<char, TPCH_READ_P_CNTR_LEN + 1>(i, 3);
            tbl_Filter_TD_2178_output.setcharN<char, TPCH_READ_P_CNTR_LEN + 1>(r, 3, _p_container264_t);
            r++;
        }
    }
    tbl_Filter_TD_2178_output.setNumRow(r);
    std::cout << "tbl_Filter_TD_2178_output #Row: " << tbl_Filter_TD_2178_output.getNumRow() << std::endl;
}

void SW_Filter_TD_2413(Table &tbl_SerializeFromObject_TD_3474_input, Table &tbl_Filter_TD_2413_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: Filter
    // Operation: ListBuffer(((isnotnull(l_shipinstruct#90) AND (l_shipmode#91 IN (AIR,AIR REG) AND (l_shipinstruct#90 = DELIVER IN PERSON))) AND ((((l_quantity#81 >= 7) AND (l_quantity#81 <= 17)) OR ((l_quantity#81 >= 10) AND (l_quantity#81 <= 20))) OR ((l_quantity#81 >= 24) AND (l_quantity#81 <= 34)))))
    // Input: ListBuffer(l_partkey#78, l_quantity#81, l_extendedprice#82, l_discount#83, l_shipinstruct#90, l_shipmode#91)
    // Output: ListBuffer(l_partkey#78, l_quantity#81, l_extendedprice#82, l_discount#83)
    int r = 0;
    int nrow1 = tbl_SerializeFromObject_TD_3474_input.getNumRow();
    for (int i = 0; i < nrow1; i++) {
        std::array<char, TPCH_READ_MAXAGG_LEN + 1> _l_shipinstruct90 = tbl_SerializeFromObject_TD_3474_input.getcharN<char, TPCH_READ_MAXAGG_LEN + 1>(i, 4);
        std::array<char, TPCH_READ_MAXAGG_LEN + 1> _l_shipmode91 = tbl_SerializeFromObject_TD_3474_input.getcharN<char, TPCH_READ_MAXAGG_LEN + 1>(i, 5);
        int32_t _l_quantity81 = tbl_SerializeFromObject_TD_3474_input.getInt32(i, 1);
        auto reuse_col_str_206 = std::string(_l_shipmode91.data());
        if (((1) && (((reuse_col_str_206 == "AIR") || (reuse_col_str_206 == "AIR REG") || (0)) && (std::string(_l_shipinstruct90.data()) == "DELIVER IN PERSON"))) && ((((_l_quantity81 >= 7) && (_l_quantity81 <= 17)) || ((_l_quantity81 >= 10) && (_l_quantity81 <= 20))) || ((_l_quantity81 >= 24) && (_l_quantity81 <= 34)))) {
            int32_t _l_partkey78_t = tbl_SerializeFromObject_TD_3474_input.getInt32(i, 0);
            tbl_Filter_TD_2413_output.setInt32(r, 0, _l_partkey78_t);
            int32_t _l_quantity81_t = tbl_SerializeFromObject_TD_3474_input.getInt32(i, 1);
            tbl_Filter_TD_2413_output.setInt32(r, 1, _l_quantity81_t);
            int32_t _l_extendedprice82_t = tbl_SerializeFromObject_TD_3474_input.getInt32(i, 2);
            tbl_Filter_TD_2413_output.setInt32(r, 2, _l_extendedprice82_t);
            int32_t _l_discount83_t = tbl_SerializeFromObject_TD_3474_input.getInt32(i, 3);
            tbl_Filter_TD_2413_output.setInt32(r, 3, _l_discount83_t);
            r++;
        }
    }
    tbl_Filter_TD_2413_output.setNumRow(r);
    std::cout << "tbl_Filter_TD_2413_output #Row: " << tbl_Filter_TD_2413_output.getNumRow() << std::endl;
}

struct SW_JOIN_INNER_TD_1169_key_leftMajor {
    int32_t _l_partkey78;
    bool operator==(const SW_JOIN_INNER_TD_1169_key_leftMajor& other) const {
        return ((_l_partkey78 == other._l_partkey78));
    }
};
namespace std {
template <>
struct hash<SW_JOIN_INNER_TD_1169_key_leftMajor> {
    std::size_t operator() (const SW_JOIN_INNER_TD_1169_key_leftMajor& k) const {
        using std::size_t;
        using std::hash;
        using std::string;
        return (hash<int32_t>()(k._l_partkey78));
    }
};
}
struct SW_JOIN_INNER_TD_1169_payload_leftMajor {
    int32_t _l_partkey78;
    int32_t _l_quantity81;
    int32_t _l_extendedprice82;
    int32_t _l_discount83;
};
struct SW_JOIN_INNER_TD_1169_key_rightMajor {
    int32_t _p_partkey258;
    bool operator==(const SW_JOIN_INNER_TD_1169_key_rightMajor& other) const {
        return ((_p_partkey258 == other._p_partkey258));
    }
};
namespace std {
template <>
struct hash<SW_JOIN_INNER_TD_1169_key_rightMajor> {
    std::size_t operator() (const SW_JOIN_INNER_TD_1169_key_rightMajor& k) const {
        using std::size_t;
        using std::hash;
        using std::string;
        return (hash<int32_t>()(k._p_partkey258));
    }
};
}
struct SW_JOIN_INNER_TD_1169_payload_rightMajor {
    int32_t _p_partkey258;
    std::string _p_brand261;
    int32_t _p_size263;
    std::string _p_container264;
};
void SW_JOIN_INNER_TD_1169(Table &tbl_Filter_TD_2413_output, Table &tbl_Filter_TD_2178_output, Table &tbl_JOIN_INNER_TD_1169_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: JOIN_INNER
    // Operation: ListBuffer(((p_partkey#258 = l_partkey#78) AND (((((((p_brand#261 = Brand#51) AND p_container#264 IN (SM CASE,SM BOX,SM PACK,SM PKG)) AND (l_quantity#81 >= 7)) AND (l_quantity#81 <= 17)) AND (p_size#263 <= 5)) OR (((((p_brand#261 = Brand#32) AND p_container#264 IN (MED BAG,MED BOX,MED PKG,MED PACK)) AND (l_quantity#81 >= 10)) AND (l_quantity#81 <= 20)) AND (p_size#263 <= 10))) OR (((((p_brand#261 = Brand#12) AND p_container#264 IN (LG CASE,LG BOX,LG PACK,LG PKG)) AND (l_quantity#81 >= 24)) AND (l_quantity#81 <= 34)) AND (p_size#263 <= 15)))))
    // Left Table: ListBuffer(l_partkey#78, l_quantity#81, l_extendedprice#82, l_discount#83)
    // Right Table: ListBuffer(p_partkey#258, p_brand#261, p_size#263, p_container#264)
    // Output Table: ListBuffer(l_extendedprice#82, l_discount#83)
    int left_nrow = tbl_Filter_TD_2413_output.getNumRow();
    int right_nrow = tbl_Filter_TD_2178_output.getNumRow();
    if (left_nrow < right_nrow) { 
        std::unordered_multimap<SW_JOIN_INNER_TD_1169_key_leftMajor, SW_JOIN_INNER_TD_1169_payload_leftMajor> ht1;
        int nrow1 = tbl_Filter_TD_2413_output.getNumRow();
        for (int i = 0; i < nrow1; i++) {
            int32_t _l_partkey78_k = tbl_Filter_TD_2413_output.getInt32(i, 0);
            SW_JOIN_INNER_TD_1169_key_leftMajor keyA{_l_partkey78_k};
            int32_t _l_partkey78 = tbl_Filter_TD_2413_output.getInt32(i, 0);
            int32_t _l_quantity81 = tbl_Filter_TD_2413_output.getInt32(i, 1);
            int32_t _l_extendedprice82 = tbl_Filter_TD_2413_output.getInt32(i, 2);
            int32_t _l_discount83 = tbl_Filter_TD_2413_output.getInt32(i, 3);
            SW_JOIN_INNER_TD_1169_payload_leftMajor payloadA{_l_partkey78, _l_quantity81, _l_extendedprice82, _l_discount83};
            ht1.insert(std::make_pair(keyA, payloadA));
        }
        int r = 0;
        int nrow2 = tbl_Filter_TD_2178_output.getNumRow();
        for (int i = 0; i < nrow2; i++) {
            int32_t _p_partkey258_k = tbl_Filter_TD_2178_output.getInt32(i, 0);
            auto its = ht1.equal_range(SW_JOIN_INNER_TD_1169_key_leftMajor{_p_partkey258_k});
            auto it = its.first;
            while (it != its.second) {
                int32_t _l_partkey78 = (it->second)._l_partkey78;
                int32_t _l_quantity81 = (it->second)._l_quantity81;
                int32_t _l_extendedprice82 = (it->second)._l_extendedprice82;
                int32_t _l_discount83 = (it->second)._l_discount83;
                int32_t _p_partkey258 = tbl_Filter_TD_2178_output.getInt32(i, 0);
                std::array<char, TPCH_READ_P_BRND_LEN + 1> _p_brand261_n = tbl_Filter_TD_2178_output.getcharN<char, TPCH_READ_P_BRND_LEN + 1>(i, 1);
                std::string _p_brand261 = std::string(_p_brand261_n.data());
                int32_t _p_size263 = tbl_Filter_TD_2178_output.getInt32(i, 2);
                std::array<char, TPCH_READ_P_CNTR_LEN + 1> _p_container264_n = tbl_Filter_TD_2178_output.getcharN<char, TPCH_READ_P_CNTR_LEN + 1>(i, 3);
                std::string _p_container264 = std::string(_p_container264_n.data());
                if (((((((_p_brand261 == "Brand#51") && ((_p_container264 == "SM CASE") || (_p_container264 == "SM BOX") || (_p_container264 == "SM PACK") || (_p_container264 == "SM PKG") || (0))) && (_l_quantity81 >= 7)) && (_l_quantity81 <= 17)) && (_p_size263 <= 5)) || (((((_p_brand261 == "Brand#32") && ((_p_container264 == "MED BAG") || (_p_container264 == "MED BOX") || (_p_container264 == "MED PKG") || (_p_container264 == "MED PACK") || (0))) && (_l_quantity81 >= 10)) && (_l_quantity81 <= 20)) && (_p_size263 <= 10))) || (((((_p_brand261 == "Brand#12") && ((_p_container264 == "LG CASE") || (_p_container264 == "LG BOX") || (_p_container264 == "LG PACK") || (_p_container264 == "LG PKG") || (0))) && (_l_quantity81 >= 24)) && (_l_quantity81 <= 34)) && (_p_size263 <= 15))) {
                    tbl_JOIN_INNER_TD_1169_output.setInt32(r, 0, _l_extendedprice82);
                    tbl_JOIN_INNER_TD_1169_output.setInt32(r, 1, _l_discount83);
                    r++;
                }
                it++;
            }
        }
        tbl_JOIN_INNER_TD_1169_output.setNumRow(r);
    } else { 
        std::unordered_multimap<SW_JOIN_INNER_TD_1169_key_rightMajor, SW_JOIN_INNER_TD_1169_payload_rightMajor> ht1;
        int nrow1 = tbl_Filter_TD_2178_output.getNumRow();
        for (int i = 0; i < nrow1; i++) {
            int32_t _p_partkey258_k = tbl_Filter_TD_2178_output.getInt32(i, 0);
            SW_JOIN_INNER_TD_1169_key_rightMajor keyA{_p_partkey258_k};
            int32_t _p_partkey258 = tbl_Filter_TD_2178_output.getInt32(i, 0);
            std::array<char, TPCH_READ_P_BRND_LEN + 1> _p_brand261_n = tbl_Filter_TD_2178_output.getcharN<char, TPCH_READ_P_BRND_LEN + 1>(i, 1);
            std::string _p_brand261 = std::string(_p_brand261_n.data());
            int32_t _p_size263 = tbl_Filter_TD_2178_output.getInt32(i, 2);
            std::array<char, TPCH_READ_P_CNTR_LEN + 1> _p_container264_n = tbl_Filter_TD_2178_output.getcharN<char, TPCH_READ_P_CNTR_LEN + 1>(i, 3);
            std::string _p_container264 = std::string(_p_container264_n.data());
            SW_JOIN_INNER_TD_1169_payload_rightMajor payloadA{_p_partkey258, _p_brand261, _p_size263, _p_container264};
            ht1.insert(std::make_pair(keyA, payloadA));
        }
        int r = 0;
        int nrow2 = tbl_Filter_TD_2413_output.getNumRow();
        for (int i = 0; i < nrow2; i++) {
            int32_t _l_partkey78_k = tbl_Filter_TD_2413_output.getInt32(i, 0);
            auto its = ht1.equal_range(SW_JOIN_INNER_TD_1169_key_rightMajor{_l_partkey78_k});
            auto it = its.first;
            while (it != its.second) {
                int32_t _p_partkey258 = (it->second)._p_partkey258;
                std::string _p_brand261 = (it->second)._p_brand261;
                std::array<char, TPCH_READ_P_BRND_LEN + 1> _p_brand261_n{};
                memcpy(_p_brand261_n.data(), (_p_brand261).data(), (_p_brand261).length());
                int32_t _p_size263 = (it->second)._p_size263;
                std::string _p_container264 = (it->second)._p_container264;
                std::array<char, TPCH_READ_P_CNTR_LEN + 1> _p_container264_n{};
                memcpy(_p_container264_n.data(), (_p_container264).data(), (_p_container264).length());
                int32_t _l_partkey78 = tbl_Filter_TD_2413_output.getInt32(i, 0);
                int32_t _l_quantity81 = tbl_Filter_TD_2413_output.getInt32(i, 1);
                int32_t _l_extendedprice82 = tbl_Filter_TD_2413_output.getInt32(i, 2);
                int32_t _l_discount83 = tbl_Filter_TD_2413_output.getInt32(i, 3);
                if (((((((_p_brand261 == "Brand#51") && ((_p_container264 == "SM CASE") || (_p_container264 == "SM BOX") || (_p_container264 == "SM PACK") || (_p_container264 == "SM PKG") || (0))) && (_l_quantity81 >= 7)) && (_l_quantity81 <= 17)) && (_p_size263 <= 5)) || (((((_p_brand261 == "Brand#32") && ((_p_container264 == "MED BAG") || (_p_container264 == "MED BOX") || (_p_container264 == "MED PKG") || (_p_container264 == "MED PACK") || (0))) && (_l_quantity81 >= 10)) && (_l_quantity81 <= 20)) && (_p_size263 <= 10))) || (((((_p_brand261 == "Brand#12") && ((_p_container264 == "LG CASE") || (_p_container264 == "LG BOX") || (_p_container264 == "LG PACK") || (_p_container264 == "LG PKG") || (0))) && (_l_quantity81 >= 24)) && (_l_quantity81 <= 34)) && (_p_size263 <= 15))) {
                    tbl_JOIN_INNER_TD_1169_output.setInt32(r, 0, _l_extendedprice82);
                    tbl_JOIN_INNER_TD_1169_output.setInt32(r, 1, _l_discount83);
                    r++;
                }
                it++;
            }
        }
        tbl_JOIN_INNER_TD_1169_output.setNumRow(r);
    } 
    std::cout << "tbl_JOIN_INNER_TD_1169_output #Row: " << tbl_JOIN_INNER_TD_1169_output.getNumRow() << std::endl;
}

void SW_Aggregate_TD_0391(Table &tbl_JOIN_INNER_TD_1169_output, Table &tbl_Aggregate_TD_0391_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: Aggregate
    // Operation: ListBuffer(sum((l_extendedprice#82 * (100 - l_discount#83))) AS revenue#1819L)
    // Input: ListBuffer(l_extendedprice#82, l_discount#83)
    // Output: ListBuffer(revenue#1819L)
    int64_t sum_0 = 0;
    int nrow1 = tbl_JOIN_INNER_TD_1169_output.getNumRow();
    for (int i = 0; i < nrow1; i++) {
        int32_t _l_extendedprice82 = tbl_JOIN_INNER_TD_1169_output.getInt32(i, 0);
        int32_t _l_discount83 = tbl_JOIN_INNER_TD_1169_output.getInt32(i, 1);
        int64_t _revenue1819L_sum_0 = (_l_extendedprice82 * (100 - _l_discount83));
        sum_0 += _revenue1819L_sum_0;
    }
    int r = 0;
    int64_t _revenue1819L = sum_0;
    tbl_Aggregate_TD_0391_output.setInt64(r++, 0, _revenue1819L);
    tbl_Aggregate_TD_0391_output.setNumRow(r);
    std::cout << "tbl_Aggregate_TD_0391_output #Row: " << tbl_Aggregate_TD_0391_output.getNumRow() << std::endl;
}

