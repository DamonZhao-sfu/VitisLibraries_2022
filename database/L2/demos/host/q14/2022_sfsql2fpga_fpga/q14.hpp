#include <regex> 
#include <stdint.h> 

void SW_Filter_TD_2281(Table &tbl_SerializeFromObject_TD_3532_input, Table &tbl_Filter_TD_2281_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: Filter
    // Operation: ListBuffer(((l_shipdate#87 >= 19970301) AND (l_shipdate#87 < 19970401)))
    // Input: ListBuffer(l_partkey#78, l_extendedprice#82, l_discount#83, l_shipdate#87)
    // Output: ListBuffer(l_partkey#78, l_extendedprice#82, l_discount#83)
    int r = 0;
    int nrow1 = tbl_SerializeFromObject_TD_3532_input.getNumRow();
    for (int i = 0; i < nrow1; i++) {
        int32_t _l_shipdate87 = tbl_SerializeFromObject_TD_3532_input.getInt32(i, 3);
        if ((_l_shipdate87 >= 19970301) && (_l_shipdate87 < 19970401)) {
            int32_t _l_partkey78_t = tbl_SerializeFromObject_TD_3532_input.getInt32(i, 0);
            tbl_Filter_TD_2281_output.setInt32(r, 0, _l_partkey78_t);
            int32_t _l_extendedprice82_t = tbl_SerializeFromObject_TD_3532_input.getInt32(i, 1);
            tbl_Filter_TD_2281_output.setInt32(r, 1, _l_extendedprice82_t);
            int32_t _l_discount83_t = tbl_SerializeFromObject_TD_3532_input.getInt32(i, 2);
            tbl_Filter_TD_2281_output.setInt32(r, 2, _l_discount83_t);
            r++;
        }
    }
    tbl_Filter_TD_2281_output.setNumRow(r);
    std::cout << "tbl_Filter_TD_2281_output #Row: " << tbl_Filter_TD_2281_output.getNumRow() << std::endl;
}

struct SW_JOIN_INNER_TD_1733_key_leftMajor {
    int32_t _l_partkey78;
    bool operator==(const SW_JOIN_INNER_TD_1733_key_leftMajor& other) const {
        return ((_l_partkey78 == other._l_partkey78));
    }
};
namespace std {
template <>
struct hash<SW_JOIN_INNER_TD_1733_key_leftMajor> {
    std::size_t operator() (const SW_JOIN_INNER_TD_1733_key_leftMajor& k) const {
        using std::size_t;
        using std::hash;
        using std::string;
        return (hash<int32_t>()(k._l_partkey78));
    }
};
}
struct SW_JOIN_INNER_TD_1733_payload_leftMajor {
    int32_t _l_partkey78;
    int32_t _l_extendedprice82;
    int32_t _l_discount83;
};
struct SW_JOIN_INNER_TD_1733_key_rightMajor {
    int32_t _p_partkey258;
    bool operator==(const SW_JOIN_INNER_TD_1733_key_rightMajor& other) const {
        return ((_p_partkey258 == other._p_partkey258));
    }
};
namespace std {
template <>
struct hash<SW_JOIN_INNER_TD_1733_key_rightMajor> {
    std::size_t operator() (const SW_JOIN_INNER_TD_1733_key_rightMajor& k) const {
        using std::size_t;
        using std::hash;
        using std::string;
        return (hash<int32_t>()(k._p_partkey258));
    }
};
}
struct SW_JOIN_INNER_TD_1733_payload_rightMajor {
    int32_t _p_partkey258;
    std::string _p_type262;
};
void SW_JOIN_INNER_TD_1733(Table &tbl_Filter_TD_2281_output, Table &tbl_SerializeFromObject_TD_2963_input, Table &tbl_JOIN_INNER_TD_1733_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: JOIN_INNER
    // Operation: ListBuffer((l_partkey#78 = p_partkey#258))
    // Left Table: ListBuffer(l_partkey#78, l_extendedprice#82, l_discount#83)
    // Right Table: ListBuffer(p_partkey#258, p_type#262)
    // Output Table: ListBuffer(l_extendedprice#82, l_discount#83, p_type#262)
    int left_nrow = tbl_Filter_TD_2281_output.getNumRow();
    int right_nrow = tbl_SerializeFromObject_TD_2963_input.getNumRow();
    if (left_nrow < right_nrow) { 
        std::unordered_multimap<SW_JOIN_INNER_TD_1733_key_leftMajor, SW_JOIN_INNER_TD_1733_payload_leftMajor> ht1;
        int nrow1 = tbl_Filter_TD_2281_output.getNumRow();
        for (int i = 0; i < nrow1; i++) {
            int32_t _l_partkey78_k = tbl_Filter_TD_2281_output.getInt32(i, 0);
            SW_JOIN_INNER_TD_1733_key_leftMajor keyA{_l_partkey78_k};
            int32_t _l_partkey78 = tbl_Filter_TD_2281_output.getInt32(i, 0);
            int32_t _l_extendedprice82 = tbl_Filter_TD_2281_output.getInt32(i, 1);
            int32_t _l_discount83 = tbl_Filter_TD_2281_output.getInt32(i, 2);
            SW_JOIN_INNER_TD_1733_payload_leftMajor payloadA{_l_partkey78, _l_extendedprice82, _l_discount83};
            ht1.insert(std::make_pair(keyA, payloadA));
        }
        int r = 0;
        int nrow2 = tbl_SerializeFromObject_TD_2963_input.getNumRow();
        for (int i = 0; i < nrow2; i++) {
            int32_t _p_partkey258_k = tbl_SerializeFromObject_TD_2963_input.getInt32(i, 0);
            auto its = ht1.equal_range(SW_JOIN_INNER_TD_1733_key_leftMajor{_p_partkey258_k});
            auto it = its.first;
            while (it != its.second) {
                int32_t _l_partkey78 = (it->second)._l_partkey78;
                int32_t _l_extendedprice82 = (it->second)._l_extendedprice82;
                int32_t _l_discount83 = (it->second)._l_discount83;
                int32_t _p_partkey258 = tbl_SerializeFromObject_TD_2963_input.getInt32(i, 0);
                std::array<char, TPCH_READ_P_TYPE_LEN + 1> _p_type262_n = tbl_SerializeFromObject_TD_2963_input.getcharN<char, TPCH_READ_P_TYPE_LEN + 1>(i, 1);
                std::string _p_type262 = std::string(_p_type262_n.data());
                tbl_JOIN_INNER_TD_1733_output.setInt32(r, 0, _l_extendedprice82);
                tbl_JOIN_INNER_TD_1733_output.setInt32(r, 1, _l_discount83);
                tbl_JOIN_INNER_TD_1733_output.setcharN<char, TPCH_READ_P_TYPE_LEN + 1>(r, 2, _p_type262_n);
                it++;
                r++;
            }
        }
        tbl_JOIN_INNER_TD_1733_output.setNumRow(r);
    } else { 
        std::unordered_multimap<SW_JOIN_INNER_TD_1733_key_rightMajor, SW_JOIN_INNER_TD_1733_payload_rightMajor> ht1;
        int nrow1 = tbl_SerializeFromObject_TD_2963_input.getNumRow();
        for (int i = 0; i < nrow1; i++) {
            int32_t _p_partkey258_k = tbl_SerializeFromObject_TD_2963_input.getInt32(i, 0);
            SW_JOIN_INNER_TD_1733_key_rightMajor keyA{_p_partkey258_k};
            int32_t _p_partkey258 = tbl_SerializeFromObject_TD_2963_input.getInt32(i, 0);
            std::array<char, TPCH_READ_P_TYPE_LEN + 1> _p_type262_n = tbl_SerializeFromObject_TD_2963_input.getcharN<char, TPCH_READ_P_TYPE_LEN + 1>(i, 1);
            std::string _p_type262 = std::string(_p_type262_n.data());
            SW_JOIN_INNER_TD_1733_payload_rightMajor payloadA{_p_partkey258, _p_type262};
            ht1.insert(std::make_pair(keyA, payloadA));
        }
        int r = 0;
        int nrow2 = tbl_Filter_TD_2281_output.getNumRow();
        for (int i = 0; i < nrow2; i++) {
            int32_t _l_partkey78_k = tbl_Filter_TD_2281_output.getInt32(i, 0);
            auto its = ht1.equal_range(SW_JOIN_INNER_TD_1733_key_rightMajor{_l_partkey78_k});
            auto it = its.first;
            while (it != its.second) {
                int32_t _p_partkey258 = (it->second)._p_partkey258;
                std::string _p_type262 = (it->second)._p_type262;
                std::array<char, TPCH_READ_P_TYPE_LEN + 1> _p_type262_n{};
                memcpy(_p_type262_n.data(), (_p_type262).data(), (_p_type262).length());
                int32_t _l_partkey78 = tbl_Filter_TD_2281_output.getInt32(i, 0);
                int32_t _l_extendedprice82 = tbl_Filter_TD_2281_output.getInt32(i, 1);
                int32_t _l_discount83 = tbl_Filter_TD_2281_output.getInt32(i, 2);
                tbl_JOIN_INNER_TD_1733_output.setcharN<char, TPCH_READ_P_TYPE_LEN + 1>(r, 2, _p_type262_n);
                tbl_JOIN_INNER_TD_1733_output.setInt32(r, 0, _l_extendedprice82);
                tbl_JOIN_INNER_TD_1733_output.setInt32(r, 1, _l_discount83);
                it++;
                r++;
            }
        }
        tbl_JOIN_INNER_TD_1733_output.setNumRow(r);
    } 
    std::cout << "tbl_JOIN_INNER_TD_1733_output #Row: " << tbl_JOIN_INNER_TD_1733_output.getNumRow() << std::endl;
}

void SW_Aggregate_TD_0738(Table &tbl_JOIN_INNER_TD_1733_output, Table &tbl_Aggregate_TD_0738_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: Aggregate
    // Operation: ListBuffer((cast((100 * sum(CASE WHEN StartsWith(p_type#262, PROMO) THEN (l_extendedprice#82 * (100 - l_discount#83)) ELSE 0 END)) as double) / cast(sum((l_extendedprice#82 * (100 - l_discount#83))) as double)) AS promo_revenue#1404)
    // Input: ListBuffer(l_extendedprice#82, l_discount#83, p_type#262)
    // Output: ListBuffer(promo_revenue#1404)
    int64_t sum_0 = 0;
    int64_t sum_1 = 0;
    int nrow1 = tbl_JOIN_INNER_TD_1733_output.getNumRow();
    for (int i = 0; i < nrow1; i++) {
        int32_t _l_extendedprice82 = tbl_JOIN_INNER_TD_1733_output.getInt32(i, 0);
        int32_t _l_discount83 = tbl_JOIN_INNER_TD_1733_output.getInt32(i, 1);
        std::array<char, TPCH_READ_P_TYPE_LEN + 1> _p_type262 = tbl_JOIN_INNER_TD_1733_output.getcharN<char, TPCH_READ_P_TYPE_LEN + 1>(i, 2);
        int64_t _promo_revenue1404_sum_0 = (std::string(_p_type262.data()).find("PROMO")==0) ? (_l_extendedprice82 * (100 - _l_discount83)) : 0;
        int64_t _promo_revenue1404_sum_1 = (_l_extendedprice82 * (100 - _l_discount83));
        sum_0 += _promo_revenue1404_sum_0;
        sum_1 += _promo_revenue1404_sum_1;
    }
    int r = 0;
    int64_t _promo_revenue1404 = ((100 * sum_0) / sum_1);
    tbl_Aggregate_TD_0738_output.setInt64(r++, 0, _promo_revenue1404);
    tbl_Aggregate_TD_0738_output.setNumRow(r);
    std::cout << "tbl_Aggregate_TD_0738_output #Row: " << tbl_Aggregate_TD_0738_output.getNumRow() << std::endl;
}

