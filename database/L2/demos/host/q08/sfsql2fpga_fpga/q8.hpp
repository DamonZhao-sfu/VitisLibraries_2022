#include <regex> 
#include <stdint.h> 

void SW_Filter_TD_10557(Table &tbl_SerializeFromObject_TD_11662_input, Table &tbl_Filter_TD_10557_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: Filter
    // Operation: ListBuffer((isnotnull(p_type#262) AND (p_type#262 = LARGE ANODIZED STEEL)))
    // Input: ListBuffer(p_partkey#258, p_type#262)
    // Output: ListBuffer(p_partkey#258)
    int r = 0;
    int nrow1 = tbl_SerializeFromObject_TD_11662_input.getNumRow();
    for (int i = 0; i < nrow1; i++) {
        std::array<char, TPCH_READ_P_TYPE_LEN + 1> _p_type262 = tbl_SerializeFromObject_TD_11662_input.getcharN<char, TPCH_READ_P_TYPE_LEN + 1>(i, 1);
        if ((1) && (std::string(_p_type262.data()) == "LARGE ANODIZED STEEL")) {
            int32_t _p_partkey258_t = tbl_SerializeFromObject_TD_11662_input.getInt32(i, 0);
            tbl_Filter_TD_10557_output.setInt32(r, 0, _p_partkey258_t);
            r++;
        }
    }
    tbl_Filter_TD_10557_output.setNumRow(r);
    std::cout << "tbl_Filter_TD_10557_output #Row: " << tbl_Filter_TD_10557_output.getNumRow() << std::endl;
}

struct SW_JOIN_INNER_TD_9616_key_leftMajor {
    int32_t _p_partkey258;
    bool operator==(const SW_JOIN_INNER_TD_9616_key_leftMajor& other) const {
        return ((_p_partkey258 == other._p_partkey258));
    }
};
namespace std {
template <>
struct hash<SW_JOIN_INNER_TD_9616_key_leftMajor> {
    std::size_t operator() (const SW_JOIN_INNER_TD_9616_key_leftMajor& k) const {
        using std::size_t;
        using std::hash;
        using std::string;
        return (hash<int32_t>()(k._p_partkey258));
    }
};
}
struct SW_JOIN_INNER_TD_9616_payload_leftMajor {
    int32_t _p_partkey258;
};
struct SW_JOIN_INNER_TD_9616_key_rightMajor {
    int32_t _l_partkey78;
    bool operator==(const SW_JOIN_INNER_TD_9616_key_rightMajor& other) const {
        return ((_l_partkey78 == other._l_partkey78));
    }
};
namespace std {
template <>
struct hash<SW_JOIN_INNER_TD_9616_key_rightMajor> {
    std::size_t operator() (const SW_JOIN_INNER_TD_9616_key_rightMajor& k) const {
        using std::size_t;
        using std::hash;
        using std::string;
        return (hash<int32_t>()(k._l_partkey78));
    }
};
}
struct SW_JOIN_INNER_TD_9616_payload_rightMajor {
    int32_t _l_orderkey77;
    int32_t _l_partkey78;
    int32_t _l_suppkey79;
    int32_t _l_extendedprice82;
    int32_t _l_discount83;
};
void SW_JOIN_INNER_TD_9616(Table &tbl_Filter_TD_10557_output, Table &tbl_SerializeFromObject_TD_10142_input, Table &tbl_JOIN_INNER_TD_9616_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: JOIN_INNER
    // Operation: ListBuffer((p_partkey#258 = l_partkey#78))
    // Left Table: ListBuffer(p_partkey#258)
    // Right Table: ListBuffer(l_orderkey#77, l_partkey#78, l_suppkey#79, l_extendedprice#82, l_discount#83)
    // Output Table: ListBuffer(l_orderkey#77, l_suppkey#79, l_extendedprice#82, l_discount#83)
    int left_nrow = tbl_Filter_TD_10557_output.getNumRow();
    int right_nrow = tbl_SerializeFromObject_TD_10142_input.getNumRow();
    if (left_nrow < right_nrow) { 
        std::unordered_multimap<SW_JOIN_INNER_TD_9616_key_leftMajor, SW_JOIN_INNER_TD_9616_payload_leftMajor> ht1;
        int nrow1 = tbl_Filter_TD_10557_output.getNumRow();
        for (int i = 0; i < nrow1; i++) {
            int32_t _p_partkey258_k = tbl_Filter_TD_10557_output.getInt32(i, 0);
            SW_JOIN_INNER_TD_9616_key_leftMajor keyA{_p_partkey258_k};
            int32_t _p_partkey258 = tbl_Filter_TD_10557_output.getInt32(i, 0);
            SW_JOIN_INNER_TD_9616_payload_leftMajor payloadA{_p_partkey258};
            ht1.insert(std::make_pair(keyA, payloadA));
        }
        int r = 0;
        int nrow2 = tbl_SerializeFromObject_TD_10142_input.getNumRow();
        for (int i = 0; i < nrow2; i++) {
            int32_t _l_partkey78_k = tbl_SerializeFromObject_TD_10142_input.getInt32(i, 1);
            auto its = ht1.equal_range(SW_JOIN_INNER_TD_9616_key_leftMajor{_l_partkey78_k});
            auto it = its.first;
            while (it != its.second) {
                int32_t _p_partkey258 = (it->second)._p_partkey258;
                int32_t _l_orderkey77 = tbl_SerializeFromObject_TD_10142_input.getInt32(i, 0);
                int32_t _l_partkey78 = tbl_SerializeFromObject_TD_10142_input.getInt32(i, 1);
                int32_t _l_suppkey79 = tbl_SerializeFromObject_TD_10142_input.getInt32(i, 2);
                int32_t _l_extendedprice82 = tbl_SerializeFromObject_TD_10142_input.getInt32(i, 3);
                int32_t _l_discount83 = tbl_SerializeFromObject_TD_10142_input.getInt32(i, 4);
                tbl_JOIN_INNER_TD_9616_output.setInt32(r, 0, _l_orderkey77);
                tbl_JOIN_INNER_TD_9616_output.setInt32(r, 1, _l_suppkey79);
                tbl_JOIN_INNER_TD_9616_output.setInt32(r, 2, _l_extendedprice82);
                tbl_JOIN_INNER_TD_9616_output.setInt32(r, 3, _l_discount83);
                it++;
                r++;
            }
        }
        tbl_JOIN_INNER_TD_9616_output.setNumRow(r);
    } else { 
        std::unordered_multimap<SW_JOIN_INNER_TD_9616_key_rightMajor, SW_JOIN_INNER_TD_9616_payload_rightMajor> ht1;
        int nrow1 = tbl_SerializeFromObject_TD_10142_input.getNumRow();
        for (int i = 0; i < nrow1; i++) {
            int32_t _l_partkey78_k = tbl_SerializeFromObject_TD_10142_input.getInt32(i, 1);
            SW_JOIN_INNER_TD_9616_key_rightMajor keyA{_l_partkey78_k};
            int32_t _l_orderkey77 = tbl_SerializeFromObject_TD_10142_input.getInt32(i, 0);
            int32_t _l_partkey78 = tbl_SerializeFromObject_TD_10142_input.getInt32(i, 1);
            int32_t _l_suppkey79 = tbl_SerializeFromObject_TD_10142_input.getInt32(i, 2);
            int32_t _l_extendedprice82 = tbl_SerializeFromObject_TD_10142_input.getInt32(i, 3);
            int32_t _l_discount83 = tbl_SerializeFromObject_TD_10142_input.getInt32(i, 4);
            SW_JOIN_INNER_TD_9616_payload_rightMajor payloadA{_l_orderkey77, _l_partkey78, _l_suppkey79, _l_extendedprice82, _l_discount83};
            ht1.insert(std::make_pair(keyA, payloadA));
        }
        int r = 0;
        int nrow2 = tbl_Filter_TD_10557_output.getNumRow();
        for (int i = 0; i < nrow2; i++) {
            int32_t _p_partkey258_k = tbl_Filter_TD_10557_output.getInt32(i, 0);
            auto its = ht1.equal_range(SW_JOIN_INNER_TD_9616_key_rightMajor{_p_partkey258_k});
            auto it = its.first;
            while (it != its.second) {
                int32_t _l_orderkey77 = (it->second)._l_orderkey77;
                int32_t _l_partkey78 = (it->second)._l_partkey78;
                int32_t _l_suppkey79 = (it->second)._l_suppkey79;
                int32_t _l_extendedprice82 = (it->second)._l_extendedprice82;
                int32_t _l_discount83 = (it->second)._l_discount83;
                int32_t _p_partkey258 = tbl_Filter_TD_10557_output.getInt32(i, 0);
                tbl_JOIN_INNER_TD_9616_output.setInt32(r, 0, _l_orderkey77);
                tbl_JOIN_INNER_TD_9616_output.setInt32(r, 1, _l_suppkey79);
                tbl_JOIN_INNER_TD_9616_output.setInt32(r, 2, _l_extendedprice82);
                tbl_JOIN_INNER_TD_9616_output.setInt32(r, 3, _l_discount83);
                it++;
                r++;
            }
        }
        tbl_JOIN_INNER_TD_9616_output.setNumRow(r);
    } 
    std::cout << "tbl_JOIN_INNER_TD_9616_output #Row: " << tbl_JOIN_INNER_TD_9616_output.getNumRow() << std::endl;
}

void SW_Filter_TD_8612(Table &tbl_SerializeFromObject_TD_953_input, Table &tbl_Filter_TD_8612_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: Filter
    // Operation: ListBuffer(((o_orderdate#211 >= 19950101) AND (o_orderdate#211 <= 19961231)))
    // Input: ListBuffer(o_orderkey#207, o_custkey#208, o_orderdate#211)
    // Output: ListBuffer(o_orderkey#207, o_custkey#208, o_orderdate#211)
    int r = 0;
    int nrow1 = tbl_SerializeFromObject_TD_953_input.getNumRow();
    for (int i = 0; i < nrow1; i++) {
        int32_t _o_orderdate211 = tbl_SerializeFromObject_TD_953_input.getInt32(i, 2);
        if ((_o_orderdate211 >= 19950101) && (_o_orderdate211 <= 19961231)) {
            int32_t _o_orderkey207_t = tbl_SerializeFromObject_TD_953_input.getInt32(i, 0);
            tbl_Filter_TD_8612_output.setInt32(r, 0, _o_orderkey207_t);
            int32_t _o_custkey208_t = tbl_SerializeFromObject_TD_953_input.getInt32(i, 1);
            tbl_Filter_TD_8612_output.setInt32(r, 1, _o_custkey208_t);
            int32_t _o_orderdate211_t = tbl_SerializeFromObject_TD_953_input.getInt32(i, 2);
            tbl_Filter_TD_8612_output.setInt32(r, 2, _o_orderdate211_t);
            r++;
        }
    }
    tbl_Filter_TD_8612_output.setNumRow(r);
    std::cout << "tbl_Filter_TD_8612_output #Row: " << tbl_Filter_TD_8612_output.getNumRow() << std::endl;
}

struct SW_JOIN_INNER_TD_8691_key_leftMajor {
    int32_t _l_suppkey79;
    bool operator==(const SW_JOIN_INNER_TD_8691_key_leftMajor& other) const {
        return ((_l_suppkey79 == other._l_suppkey79));
    }
};
namespace std {
template <>
struct hash<SW_JOIN_INNER_TD_8691_key_leftMajor> {
    std::size_t operator() (const SW_JOIN_INNER_TD_8691_key_leftMajor& k) const {
        using std::size_t;
        using std::hash;
        using std::string;
        return (hash<int32_t>()(k._l_suppkey79));
    }
};
}
struct SW_JOIN_INNER_TD_8691_payload_leftMajor {
    int32_t _l_orderkey77;
    int32_t _l_suppkey79;
    int32_t _l_extendedprice82;
    int32_t _l_discount83;
};
struct SW_JOIN_INNER_TD_8691_key_rightMajor {
    int32_t _s_suppkey342;
    bool operator==(const SW_JOIN_INNER_TD_8691_key_rightMajor& other) const {
        return ((_s_suppkey342 == other._s_suppkey342));
    }
};
namespace std {
template <>
struct hash<SW_JOIN_INNER_TD_8691_key_rightMajor> {
    std::size_t operator() (const SW_JOIN_INNER_TD_8691_key_rightMajor& k) const {
        using std::size_t;
        using std::hash;
        using std::string;
        return (hash<int32_t>()(k._s_suppkey342));
    }
};
}
struct SW_JOIN_INNER_TD_8691_payload_rightMajor {
    int32_t _s_suppkey342;
    int32_t _s_nationkey345;
};
void SW_JOIN_INNER_TD_8691(Table &tbl_JOIN_INNER_TD_9616_output, Table &tbl_SerializeFromObject_TD_961_input, Table &tbl_JOIN_INNER_TD_8691_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: JOIN_INNER
    // Operation: ListBuffer((s_suppkey#342 = l_suppkey#79))
    // Left Table: ListBuffer(l_orderkey#77, l_suppkey#79, l_extendedprice#82, l_discount#83)
    // Right Table: ListBuffer(s_suppkey#342, s_nationkey#345)
    // Output Table: ListBuffer(l_orderkey#77, l_extendedprice#82, l_discount#83, s_nationkey#345)
    int left_nrow = tbl_JOIN_INNER_TD_9616_output.getNumRow();
    int right_nrow = tbl_SerializeFromObject_TD_961_input.getNumRow();
    if (left_nrow < right_nrow) { 
        std::unordered_multimap<SW_JOIN_INNER_TD_8691_key_leftMajor, SW_JOIN_INNER_TD_8691_payload_leftMajor> ht1;
        int nrow1 = tbl_JOIN_INNER_TD_9616_output.getNumRow();
        for (int i = 0; i < nrow1; i++) {
            int32_t _l_suppkey79_k = tbl_JOIN_INNER_TD_9616_output.getInt32(i, 1);
            SW_JOIN_INNER_TD_8691_key_leftMajor keyA{_l_suppkey79_k};
            int32_t _l_orderkey77 = tbl_JOIN_INNER_TD_9616_output.getInt32(i, 0);
            int32_t _l_suppkey79 = tbl_JOIN_INNER_TD_9616_output.getInt32(i, 1);
            int32_t _l_extendedprice82 = tbl_JOIN_INNER_TD_9616_output.getInt32(i, 2);
            int32_t _l_discount83 = tbl_JOIN_INNER_TD_9616_output.getInt32(i, 3);
            SW_JOIN_INNER_TD_8691_payload_leftMajor payloadA{_l_orderkey77, _l_suppkey79, _l_extendedprice82, _l_discount83};
            ht1.insert(std::make_pair(keyA, payloadA));
        }
        int r = 0;
        int nrow2 = tbl_SerializeFromObject_TD_961_input.getNumRow();
        for (int i = 0; i < nrow2; i++) {
            int32_t _s_suppkey342_k = tbl_SerializeFromObject_TD_961_input.getInt32(i, 0);
            auto its = ht1.equal_range(SW_JOIN_INNER_TD_8691_key_leftMajor{_s_suppkey342_k});
            auto it = its.first;
            while (it != its.second) {
                int32_t _l_orderkey77 = (it->second)._l_orderkey77;
                int32_t _l_suppkey79 = (it->second)._l_suppkey79;
                int32_t _l_extendedprice82 = (it->second)._l_extendedprice82;
                int32_t _l_discount83 = (it->second)._l_discount83;
                int32_t _s_suppkey342 = tbl_SerializeFromObject_TD_961_input.getInt32(i, 0);
                int32_t _s_nationkey345 = tbl_SerializeFromObject_TD_961_input.getInt32(i, 1);
                tbl_JOIN_INNER_TD_8691_output.setInt32(r, 0, _l_orderkey77);
                tbl_JOIN_INNER_TD_8691_output.setInt32(r, 1, _l_extendedprice82);
                tbl_JOIN_INNER_TD_8691_output.setInt32(r, 2, _l_discount83);
                tbl_JOIN_INNER_TD_8691_output.setInt32(r, 3, _s_nationkey345);
                it++;
                r++;
            }
        }
        tbl_JOIN_INNER_TD_8691_output.setNumRow(r);
    } else { 
        std::unordered_multimap<SW_JOIN_INNER_TD_8691_key_rightMajor, SW_JOIN_INNER_TD_8691_payload_rightMajor> ht1;
        int nrow1 = tbl_SerializeFromObject_TD_961_input.getNumRow();
        for (int i = 0; i < nrow1; i++) {
            int32_t _s_suppkey342_k = tbl_SerializeFromObject_TD_961_input.getInt32(i, 0);
            SW_JOIN_INNER_TD_8691_key_rightMajor keyA{_s_suppkey342_k};
            int32_t _s_suppkey342 = tbl_SerializeFromObject_TD_961_input.getInt32(i, 0);
            int32_t _s_nationkey345 = tbl_SerializeFromObject_TD_961_input.getInt32(i, 1);
            SW_JOIN_INNER_TD_8691_payload_rightMajor payloadA{_s_suppkey342, _s_nationkey345};
            ht1.insert(std::make_pair(keyA, payloadA));
        }
        int r = 0;
        int nrow2 = tbl_JOIN_INNER_TD_9616_output.getNumRow();
        for (int i = 0; i < nrow2; i++) {
            int32_t _l_suppkey79_k = tbl_JOIN_INNER_TD_9616_output.getInt32(i, 1);
            auto its = ht1.equal_range(SW_JOIN_INNER_TD_8691_key_rightMajor{_l_suppkey79_k});
            auto it = its.first;
            while (it != its.second) {
                int32_t _s_suppkey342 = (it->second)._s_suppkey342;
                int32_t _s_nationkey345 = (it->second)._s_nationkey345;
                int32_t _l_orderkey77 = tbl_JOIN_INNER_TD_9616_output.getInt32(i, 0);
                int32_t _l_suppkey79 = tbl_JOIN_INNER_TD_9616_output.getInt32(i, 1);
                int32_t _l_extendedprice82 = tbl_JOIN_INNER_TD_9616_output.getInt32(i, 2);
                int32_t _l_discount83 = tbl_JOIN_INNER_TD_9616_output.getInt32(i, 3);
                tbl_JOIN_INNER_TD_8691_output.setInt32(r, 3, _s_nationkey345);
                tbl_JOIN_INNER_TD_8691_output.setInt32(r, 0, _l_orderkey77);
                tbl_JOIN_INNER_TD_8691_output.setInt32(r, 1, _l_extendedprice82);
                tbl_JOIN_INNER_TD_8691_output.setInt32(r, 2, _l_discount83);
                it++;
                r++;
            }
        }
        tbl_JOIN_INNER_TD_8691_output.setNumRow(r);
    } 
    std::cout << "tbl_JOIN_INNER_TD_8691_output #Row: " << tbl_JOIN_INNER_TD_8691_output.getNumRow() << std::endl;
}

struct SW_JOIN_INNER_TD_7769_key_leftMajor {
    int32_t _l_orderkey77;
    bool operator==(const SW_JOIN_INNER_TD_7769_key_leftMajor& other) const {
        return ((_l_orderkey77 == other._l_orderkey77));
    }
};
namespace std {
template <>
struct hash<SW_JOIN_INNER_TD_7769_key_leftMajor> {
    std::size_t operator() (const SW_JOIN_INNER_TD_7769_key_leftMajor& k) const {
        using std::size_t;
        using std::hash;
        using std::string;
        return (hash<int32_t>()(k._l_orderkey77));
    }
};
}
struct SW_JOIN_INNER_TD_7769_payload_leftMajor {
    int32_t _l_orderkey77;
    int32_t _l_extendedprice82;
    int32_t _l_discount83;
    int32_t _s_nationkey345;
};
struct SW_JOIN_INNER_TD_7769_key_rightMajor {
    int32_t _o_orderkey207;
    bool operator==(const SW_JOIN_INNER_TD_7769_key_rightMajor& other) const {
        return ((_o_orderkey207 == other._o_orderkey207));
    }
};
namespace std {
template <>
struct hash<SW_JOIN_INNER_TD_7769_key_rightMajor> {
    std::size_t operator() (const SW_JOIN_INNER_TD_7769_key_rightMajor& k) const {
        using std::size_t;
        using std::hash;
        using std::string;
        return (hash<int32_t>()(k._o_orderkey207));
    }
};
}
struct SW_JOIN_INNER_TD_7769_payload_rightMajor {
    int32_t _o_orderkey207;
    int32_t _o_custkey208;
    int32_t _o_orderdate211;
};
void SW_JOIN_INNER_TD_7769(Table &tbl_JOIN_INNER_TD_8691_output, Table &tbl_Filter_TD_8612_output, Table &tbl_JOIN_INNER_TD_7769_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: JOIN_INNER
    // Operation: ListBuffer((l_orderkey#77 = o_orderkey#207))
    // Left Table: ListBuffer(l_orderkey#77, l_extendedprice#82, l_discount#83, s_nationkey#345)
    // Right Table: ListBuffer(o_orderkey#207, o_custkey#208, o_orderdate#211)
    // Output Table: ListBuffer(l_extendedprice#82, l_discount#83, s_nationkey#345, o_custkey#208, o_orderdate#211)
    int left_nrow = tbl_JOIN_INNER_TD_8691_output.getNumRow();
    int right_nrow = tbl_Filter_TD_8612_output.getNumRow();
    if (left_nrow < right_nrow) { 
        std::unordered_multimap<SW_JOIN_INNER_TD_7769_key_leftMajor, SW_JOIN_INNER_TD_7769_payload_leftMajor> ht1;
        int nrow1 = tbl_JOIN_INNER_TD_8691_output.getNumRow();
        for (int i = 0; i < nrow1; i++) {
            int32_t _l_orderkey77_k = tbl_JOIN_INNER_TD_8691_output.getInt32(i, 0);
            SW_JOIN_INNER_TD_7769_key_leftMajor keyA{_l_orderkey77_k};
            int32_t _l_orderkey77 = tbl_JOIN_INNER_TD_8691_output.getInt32(i, 0);
            int32_t _l_extendedprice82 = tbl_JOIN_INNER_TD_8691_output.getInt32(i, 1);
            int32_t _l_discount83 = tbl_JOIN_INNER_TD_8691_output.getInt32(i, 2);
            int32_t _s_nationkey345 = tbl_JOIN_INNER_TD_8691_output.getInt32(i, 3);
            SW_JOIN_INNER_TD_7769_payload_leftMajor payloadA{_l_orderkey77, _l_extendedprice82, _l_discount83, _s_nationkey345};
            ht1.insert(std::make_pair(keyA, payloadA));
        }
        int r = 0;
        int nrow2 = tbl_Filter_TD_8612_output.getNumRow();
        for (int i = 0; i < nrow2; i++) {
            int32_t _o_orderkey207_k = tbl_Filter_TD_8612_output.getInt32(i, 0);
            auto its = ht1.equal_range(SW_JOIN_INNER_TD_7769_key_leftMajor{_o_orderkey207_k});
            auto it = its.first;
            while (it != its.second) {
                int32_t _l_orderkey77 = (it->second)._l_orderkey77;
                int32_t _l_extendedprice82 = (it->second)._l_extendedprice82;
                int32_t _l_discount83 = (it->second)._l_discount83;
                int32_t _s_nationkey345 = (it->second)._s_nationkey345;
                int32_t _o_orderkey207 = tbl_Filter_TD_8612_output.getInt32(i, 0);
                int32_t _o_custkey208 = tbl_Filter_TD_8612_output.getInt32(i, 1);
                int32_t _o_orderdate211 = tbl_Filter_TD_8612_output.getInt32(i, 2);
                tbl_JOIN_INNER_TD_7769_output.setInt32(r, 0, _l_extendedprice82);
                tbl_JOIN_INNER_TD_7769_output.setInt32(r, 1, _l_discount83);
                tbl_JOIN_INNER_TD_7769_output.setInt32(r, 2, _s_nationkey345);
                tbl_JOIN_INNER_TD_7769_output.setInt32(r, 3, _o_custkey208);
                tbl_JOIN_INNER_TD_7769_output.setInt32(r, 4, _o_orderdate211);
                it++;
                r++;
            }
        }
        tbl_JOIN_INNER_TD_7769_output.setNumRow(r);
    } else { 
        std::unordered_multimap<SW_JOIN_INNER_TD_7769_key_rightMajor, SW_JOIN_INNER_TD_7769_payload_rightMajor> ht1;
        int nrow1 = tbl_Filter_TD_8612_output.getNumRow();
        for (int i = 0; i < nrow1; i++) {
            int32_t _o_orderkey207_k = tbl_Filter_TD_8612_output.getInt32(i, 0);
            SW_JOIN_INNER_TD_7769_key_rightMajor keyA{_o_orderkey207_k};
            int32_t _o_orderkey207 = tbl_Filter_TD_8612_output.getInt32(i, 0);
            int32_t _o_custkey208 = tbl_Filter_TD_8612_output.getInt32(i, 1);
            int32_t _o_orderdate211 = tbl_Filter_TD_8612_output.getInt32(i, 2);
            SW_JOIN_INNER_TD_7769_payload_rightMajor payloadA{_o_orderkey207, _o_custkey208, _o_orderdate211};
            ht1.insert(std::make_pair(keyA, payloadA));
        }
        int r = 0;
        int nrow2 = tbl_JOIN_INNER_TD_8691_output.getNumRow();
        for (int i = 0; i < nrow2; i++) {
            int32_t _l_orderkey77_k = tbl_JOIN_INNER_TD_8691_output.getInt32(i, 0);
            auto its = ht1.equal_range(SW_JOIN_INNER_TD_7769_key_rightMajor{_l_orderkey77_k});
            auto it = its.first;
            while (it != its.second) {
                int32_t _o_orderkey207 = (it->second)._o_orderkey207;
                int32_t _o_custkey208 = (it->second)._o_custkey208;
                int32_t _o_orderdate211 = (it->second)._o_orderdate211;
                int32_t _l_orderkey77 = tbl_JOIN_INNER_TD_8691_output.getInt32(i, 0);
                int32_t _l_extendedprice82 = tbl_JOIN_INNER_TD_8691_output.getInt32(i, 1);
                int32_t _l_discount83 = tbl_JOIN_INNER_TD_8691_output.getInt32(i, 2);
                int32_t _s_nationkey345 = tbl_JOIN_INNER_TD_8691_output.getInt32(i, 3);
                tbl_JOIN_INNER_TD_7769_output.setInt32(r, 3, _o_custkey208);
                tbl_JOIN_INNER_TD_7769_output.setInt32(r, 4, _o_orderdate211);
                tbl_JOIN_INNER_TD_7769_output.setInt32(r, 0, _l_extendedprice82);
                tbl_JOIN_INNER_TD_7769_output.setInt32(r, 1, _l_discount83);
                tbl_JOIN_INNER_TD_7769_output.setInt32(r, 2, _s_nationkey345);
                it++;
                r++;
            }
        }
        tbl_JOIN_INNER_TD_7769_output.setNumRow(r);
    } 
    std::cout << "tbl_JOIN_INNER_TD_7769_output #Row: " << tbl_JOIN_INNER_TD_7769_output.getNumRow() << std::endl;
}

struct SW_JOIN_INNER_TD_6783_key_leftMajor {
    int32_t _o_custkey208;
    bool operator==(const SW_JOIN_INNER_TD_6783_key_leftMajor& other) const {
        return ((_o_custkey208 == other._o_custkey208));
    }
};
namespace std {
template <>
struct hash<SW_JOIN_INNER_TD_6783_key_leftMajor> {
    std::size_t operator() (const SW_JOIN_INNER_TD_6783_key_leftMajor& k) const {
        using std::size_t;
        using std::hash;
        using std::string;
        return (hash<int32_t>()(k._o_custkey208));
    }
};
}
struct SW_JOIN_INNER_TD_6783_payload_leftMajor {
    int32_t _l_extendedprice82;
    int32_t _l_discount83;
    int32_t _s_nationkey345;
    int32_t _o_custkey208;
    int32_t _o_orderdate211;
};
struct SW_JOIN_INNER_TD_6783_key_rightMajor {
    int32_t _c_custkey22;
    bool operator==(const SW_JOIN_INNER_TD_6783_key_rightMajor& other) const {
        return ((_c_custkey22 == other._c_custkey22));
    }
};
namespace std {
template <>
struct hash<SW_JOIN_INNER_TD_6783_key_rightMajor> {
    std::size_t operator() (const SW_JOIN_INNER_TD_6783_key_rightMajor& k) const {
        using std::size_t;
        using std::hash;
        using std::string;
        return (hash<int32_t>()(k._c_custkey22));
    }
};
}
struct SW_JOIN_INNER_TD_6783_payload_rightMajor {
    int32_t _c_custkey22;
    int32_t _c_nationkey25;
};
void SW_JOIN_INNER_TD_6783(Table &tbl_JOIN_INNER_TD_7769_output, Table &tbl_SerializeFromObject_TD_7173_input, Table &tbl_JOIN_INNER_TD_6783_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: JOIN_INNER
    // Operation: ListBuffer((o_custkey#208 = c_custkey#22))
    // Left Table: ListBuffer(l_extendedprice#82, l_discount#83, s_nationkey#345, o_custkey#208, o_orderdate#211)
    // Right Table: ListBuffer(c_custkey#22, c_nationkey#25)
    // Output Table: ListBuffer(l_extendedprice#82, l_discount#83, s_nationkey#345, o_orderdate#211, c_nationkey#25)
    int left_nrow = tbl_JOIN_INNER_TD_7769_output.getNumRow();
    int right_nrow = tbl_SerializeFromObject_TD_7173_input.getNumRow();
    if (left_nrow < right_nrow) { 
        std::unordered_multimap<SW_JOIN_INNER_TD_6783_key_leftMajor, SW_JOIN_INNER_TD_6783_payload_leftMajor> ht1;
        int nrow1 = tbl_JOIN_INNER_TD_7769_output.getNumRow();
        for (int i = 0; i < nrow1; i++) {
            int32_t _o_custkey208_k = tbl_JOIN_INNER_TD_7769_output.getInt32(i, 3);
            SW_JOIN_INNER_TD_6783_key_leftMajor keyA{_o_custkey208_k};
            int32_t _l_extendedprice82 = tbl_JOIN_INNER_TD_7769_output.getInt32(i, 0);
            int32_t _l_discount83 = tbl_JOIN_INNER_TD_7769_output.getInt32(i, 1);
            int32_t _s_nationkey345 = tbl_JOIN_INNER_TD_7769_output.getInt32(i, 2);
            int32_t _o_custkey208 = tbl_JOIN_INNER_TD_7769_output.getInt32(i, 3);
            int32_t _o_orderdate211 = tbl_JOIN_INNER_TD_7769_output.getInt32(i, 4);
            SW_JOIN_INNER_TD_6783_payload_leftMajor payloadA{_l_extendedprice82, _l_discount83, _s_nationkey345, _o_custkey208, _o_orderdate211};
            ht1.insert(std::make_pair(keyA, payloadA));
        }
        int r = 0;
        int nrow2 = tbl_SerializeFromObject_TD_7173_input.getNumRow();
        for (int i = 0; i < nrow2; i++) {
            int32_t _c_custkey22_k = tbl_SerializeFromObject_TD_7173_input.getInt32(i, 0);
            auto its = ht1.equal_range(SW_JOIN_INNER_TD_6783_key_leftMajor{_c_custkey22_k});
            auto it = its.first;
            while (it != its.second) {
                int32_t _l_extendedprice82 = (it->second)._l_extendedprice82;
                int32_t _l_discount83 = (it->second)._l_discount83;
                int32_t _s_nationkey345 = (it->second)._s_nationkey345;
                int32_t _o_custkey208 = (it->second)._o_custkey208;
                int32_t _o_orderdate211 = (it->second)._o_orderdate211;
                int32_t _c_custkey22 = tbl_SerializeFromObject_TD_7173_input.getInt32(i, 0);
                int32_t _c_nationkey25 = tbl_SerializeFromObject_TD_7173_input.getInt32(i, 1);
                tbl_JOIN_INNER_TD_6783_output.setInt32(r, 0, _l_extendedprice82);
                tbl_JOIN_INNER_TD_6783_output.setInt32(r, 1, _l_discount83);
                tbl_JOIN_INNER_TD_6783_output.setInt32(r, 2, _s_nationkey345);
                tbl_JOIN_INNER_TD_6783_output.setInt32(r, 3, _o_orderdate211);
                tbl_JOIN_INNER_TD_6783_output.setInt32(r, 4, _c_nationkey25);
                it++;
                r++;
            }
        }
        tbl_JOIN_INNER_TD_6783_output.setNumRow(r);
    } else { 
        std::unordered_multimap<SW_JOIN_INNER_TD_6783_key_rightMajor, SW_JOIN_INNER_TD_6783_payload_rightMajor> ht1;
        int nrow1 = tbl_SerializeFromObject_TD_7173_input.getNumRow();
        for (int i = 0; i < nrow1; i++) {
            int32_t _c_custkey22_k = tbl_SerializeFromObject_TD_7173_input.getInt32(i, 0);
            SW_JOIN_INNER_TD_6783_key_rightMajor keyA{_c_custkey22_k};
            int32_t _c_custkey22 = tbl_SerializeFromObject_TD_7173_input.getInt32(i, 0);
            int32_t _c_nationkey25 = tbl_SerializeFromObject_TD_7173_input.getInt32(i, 1);
            SW_JOIN_INNER_TD_6783_payload_rightMajor payloadA{_c_custkey22, _c_nationkey25};
            ht1.insert(std::make_pair(keyA, payloadA));
        }
        int r = 0;
        int nrow2 = tbl_JOIN_INNER_TD_7769_output.getNumRow();
        for (int i = 0; i < nrow2; i++) {
            int32_t _o_custkey208_k = tbl_JOIN_INNER_TD_7769_output.getInt32(i, 3);
            auto its = ht1.equal_range(SW_JOIN_INNER_TD_6783_key_rightMajor{_o_custkey208_k});
            auto it = its.first;
            while (it != its.second) {
                int32_t _c_custkey22 = (it->second)._c_custkey22;
                int32_t _c_nationkey25 = (it->second)._c_nationkey25;
                int32_t _l_extendedprice82 = tbl_JOIN_INNER_TD_7769_output.getInt32(i, 0);
                int32_t _l_discount83 = tbl_JOIN_INNER_TD_7769_output.getInt32(i, 1);
                int32_t _s_nationkey345 = tbl_JOIN_INNER_TD_7769_output.getInt32(i, 2);
                int32_t _o_custkey208 = tbl_JOIN_INNER_TD_7769_output.getInt32(i, 3);
                int32_t _o_orderdate211 = tbl_JOIN_INNER_TD_7769_output.getInt32(i, 4);
                tbl_JOIN_INNER_TD_6783_output.setInt32(r, 4, _c_nationkey25);
                tbl_JOIN_INNER_TD_6783_output.setInt32(r, 0, _l_extendedprice82);
                tbl_JOIN_INNER_TD_6783_output.setInt32(r, 1, _l_discount83);
                tbl_JOIN_INNER_TD_6783_output.setInt32(r, 2, _s_nationkey345);
                tbl_JOIN_INNER_TD_6783_output.setInt32(r, 3, _o_orderdate211);
                it++;
                r++;
            }
        }
        tbl_JOIN_INNER_TD_6783_output.setNumRow(r);
    } 
    std::cout << "tbl_JOIN_INNER_TD_6783_output #Row: " << tbl_JOIN_INNER_TD_6783_output.getNumRow() << std::endl;
}

void SW_Filter_TD_467(Table &tbl_SerializeFromObject_TD_5797_input, Table &tbl_Filter_TD_467_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: Filter
    // Operation: ListBuffer((isnotnull(r_name#175) AND (r_name#175 = MIDDLE EAST)))
    // Input: ListBuffer(r_regionkey#174, r_name#175)
    // Output: ListBuffer(r_regionkey#174)
    int r = 0;
    int nrow1 = tbl_SerializeFromObject_TD_5797_input.getNumRow();
    for (int i = 0; i < nrow1; i++) {
        std::array<char, TPCH_READ_REGION_LEN + 1> _r_name175 = tbl_SerializeFromObject_TD_5797_input.getcharN<char, TPCH_READ_REGION_LEN + 1>(i, 1);
        if ((1) && (std::string(_r_name175.data()) == "MIDDLE EAST")) {
            int32_t _r_regionkey174_t = tbl_SerializeFromObject_TD_5797_input.getInt32(i, 0);
            tbl_Filter_TD_467_output.setInt32(r, 0, _r_regionkey174_t);
            r++;
        }
    }
    tbl_Filter_TD_467_output.setNumRow(r);
    std::cout << "tbl_Filter_TD_467_output #Row: " << tbl_Filter_TD_467_output.getNumRow() << std::endl;
}

struct SW_JOIN_INNER_TD_5534_key_leftMajor {
    int32_t _c_nationkey25;
    bool operator==(const SW_JOIN_INNER_TD_5534_key_leftMajor& other) const {
        return ((_c_nationkey25 == other._c_nationkey25));
    }
};
namespace std {
template <>
struct hash<SW_JOIN_INNER_TD_5534_key_leftMajor> {
    std::size_t operator() (const SW_JOIN_INNER_TD_5534_key_leftMajor& k) const {
        using std::size_t;
        using std::hash;
        using std::string;
        return (hash<int32_t>()(k._c_nationkey25));
    }
};
}
struct SW_JOIN_INNER_TD_5534_payload_leftMajor {
    int32_t _l_extendedprice82;
    int32_t _l_discount83;
    int32_t _s_nationkey345;
    int32_t _o_orderdate211;
    int32_t _c_nationkey25;
};
struct SW_JOIN_INNER_TD_5534_key_rightMajor {
    int32_t _n_nationkey144;
    bool operator==(const SW_JOIN_INNER_TD_5534_key_rightMajor& other) const {
        return ((_n_nationkey144 == other._n_nationkey144));
    }
};
namespace std {
template <>
struct hash<SW_JOIN_INNER_TD_5534_key_rightMajor> {
    std::size_t operator() (const SW_JOIN_INNER_TD_5534_key_rightMajor& k) const {
        using std::size_t;
        using std::hash;
        using std::string;
        return (hash<int32_t>()(k._n_nationkey144));
    }
};
}
struct SW_JOIN_INNER_TD_5534_payload_rightMajor {
    int32_t _n_nationkey144;
    int32_t _n_regionkey146;
};
void SW_JOIN_INNER_TD_5534(Table &tbl_JOIN_INNER_TD_6783_output, Table &tbl_SerializeFromObject_TD_6932_input, Table &tbl_JOIN_INNER_TD_5534_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: JOIN_INNER
    // Operation: ListBuffer((c_nationkey#25 = n_nationkey#144))
    // Left Table: ListBuffer(l_extendedprice#82, l_discount#83, s_nationkey#345, o_orderdate#211, c_nationkey#25)
    // Right Table: ListBuffer(n_nationkey#144, n_regionkey#146)
    // Output Table: ListBuffer(l_extendedprice#82, l_discount#83, s_nationkey#345, o_orderdate#211, n_regionkey#146)
    int left_nrow = tbl_JOIN_INNER_TD_6783_output.getNumRow();
    int right_nrow = tbl_SerializeFromObject_TD_6932_input.getNumRow();
    if (left_nrow < right_nrow) { 
        std::unordered_multimap<SW_JOIN_INNER_TD_5534_key_leftMajor, SW_JOIN_INNER_TD_5534_payload_leftMajor> ht1;
        int nrow1 = tbl_JOIN_INNER_TD_6783_output.getNumRow();
        for (int i = 0; i < nrow1; i++) {
            int32_t _c_nationkey25_k = tbl_JOIN_INNER_TD_6783_output.getInt32(i, 4);
            SW_JOIN_INNER_TD_5534_key_leftMajor keyA{_c_nationkey25_k};
            int32_t _l_extendedprice82 = tbl_JOIN_INNER_TD_6783_output.getInt32(i, 0);
            int32_t _l_discount83 = tbl_JOIN_INNER_TD_6783_output.getInt32(i, 1);
            int32_t _s_nationkey345 = tbl_JOIN_INNER_TD_6783_output.getInt32(i, 2);
            int32_t _o_orderdate211 = tbl_JOIN_INNER_TD_6783_output.getInt32(i, 3);
            int32_t _c_nationkey25 = tbl_JOIN_INNER_TD_6783_output.getInt32(i, 4);
            SW_JOIN_INNER_TD_5534_payload_leftMajor payloadA{_l_extendedprice82, _l_discount83, _s_nationkey345, _o_orderdate211, _c_nationkey25};
            ht1.insert(std::make_pair(keyA, payloadA));
        }
        int r = 0;
        int nrow2 = tbl_SerializeFromObject_TD_6932_input.getNumRow();
        for (int i = 0; i < nrow2; i++) {
            int32_t _n_nationkey144_k = tbl_SerializeFromObject_TD_6932_input.getInt32(i, 0);
            auto its = ht1.equal_range(SW_JOIN_INNER_TD_5534_key_leftMajor{_n_nationkey144_k});
            auto it = its.first;
            while (it != its.second) {
                int32_t _l_extendedprice82 = (it->second)._l_extendedprice82;
                int32_t _l_discount83 = (it->second)._l_discount83;
                int32_t _s_nationkey345 = (it->second)._s_nationkey345;
                int32_t _o_orderdate211 = (it->second)._o_orderdate211;
                int32_t _c_nationkey25 = (it->second)._c_nationkey25;
                int32_t _n_nationkey144 = tbl_SerializeFromObject_TD_6932_input.getInt32(i, 0);
                int32_t _n_regionkey146 = tbl_SerializeFromObject_TD_6932_input.getInt32(i, 1);
                tbl_JOIN_INNER_TD_5534_output.setInt32(r, 0, _l_extendedprice82);
                tbl_JOIN_INNER_TD_5534_output.setInt32(r, 1, _l_discount83);
                tbl_JOIN_INNER_TD_5534_output.setInt32(r, 2, _s_nationkey345);
                tbl_JOIN_INNER_TD_5534_output.setInt32(r, 3, _o_orderdate211);
                tbl_JOIN_INNER_TD_5534_output.setInt32(r, 4, _n_regionkey146);
                it++;
                r++;
            }
        }
        tbl_JOIN_INNER_TD_5534_output.setNumRow(r);
    } else { 
        std::unordered_multimap<SW_JOIN_INNER_TD_5534_key_rightMajor, SW_JOIN_INNER_TD_5534_payload_rightMajor> ht1;
        int nrow1 = tbl_SerializeFromObject_TD_6932_input.getNumRow();
        for (int i = 0; i < nrow1; i++) {
            int32_t _n_nationkey144_k = tbl_SerializeFromObject_TD_6932_input.getInt32(i, 0);
            SW_JOIN_INNER_TD_5534_key_rightMajor keyA{_n_nationkey144_k};
            int32_t _n_nationkey144 = tbl_SerializeFromObject_TD_6932_input.getInt32(i, 0);
            int32_t _n_regionkey146 = tbl_SerializeFromObject_TD_6932_input.getInt32(i, 1);
            SW_JOIN_INNER_TD_5534_payload_rightMajor payloadA{_n_nationkey144, _n_regionkey146};
            ht1.insert(std::make_pair(keyA, payloadA));
        }
        int r = 0;
        int nrow2 = tbl_JOIN_INNER_TD_6783_output.getNumRow();
        for (int i = 0; i < nrow2; i++) {
            int32_t _c_nationkey25_k = tbl_JOIN_INNER_TD_6783_output.getInt32(i, 4);
            auto its = ht1.equal_range(SW_JOIN_INNER_TD_5534_key_rightMajor{_c_nationkey25_k});
            auto it = its.first;
            while (it != its.second) {
                int32_t _n_nationkey144 = (it->second)._n_nationkey144;
                int32_t _n_regionkey146 = (it->second)._n_regionkey146;
                int32_t _l_extendedprice82 = tbl_JOIN_INNER_TD_6783_output.getInt32(i, 0);
                int32_t _l_discount83 = tbl_JOIN_INNER_TD_6783_output.getInt32(i, 1);
                int32_t _s_nationkey345 = tbl_JOIN_INNER_TD_6783_output.getInt32(i, 2);
                int32_t _o_orderdate211 = tbl_JOIN_INNER_TD_6783_output.getInt32(i, 3);
                int32_t _c_nationkey25 = tbl_JOIN_INNER_TD_6783_output.getInt32(i, 4);
                tbl_JOIN_INNER_TD_5534_output.setInt32(r, 4, _n_regionkey146);
                tbl_JOIN_INNER_TD_5534_output.setInt32(r, 0, _l_extendedprice82);
                tbl_JOIN_INNER_TD_5534_output.setInt32(r, 1, _l_discount83);
                tbl_JOIN_INNER_TD_5534_output.setInt32(r, 2, _s_nationkey345);
                tbl_JOIN_INNER_TD_5534_output.setInt32(r, 3, _o_orderdate211);
                it++;
                r++;
            }
        }
        tbl_JOIN_INNER_TD_5534_output.setNumRow(r);
    } 
    std::cout << "tbl_JOIN_INNER_TD_5534_output #Row: " << tbl_JOIN_INNER_TD_5534_output.getNumRow() << std::endl;
}

struct SW_JOIN_INNER_TD_4186_key_leftMajor {
    int32_t _n_regionkey146;
    bool operator==(const SW_JOIN_INNER_TD_4186_key_leftMajor& other) const {
        return ((_n_regionkey146 == other._n_regionkey146));
    }
};
namespace std {
template <>
struct hash<SW_JOIN_INNER_TD_4186_key_leftMajor> {
    std::size_t operator() (const SW_JOIN_INNER_TD_4186_key_leftMajor& k) const {
        using std::size_t;
        using std::hash;
        using std::string;
        return (hash<int32_t>()(k._n_regionkey146));
    }
};
}
struct SW_JOIN_INNER_TD_4186_payload_leftMajor {
    int32_t _l_extendedprice82;
    int32_t _l_discount83;
    int32_t _s_nationkey345;
    int32_t _o_orderdate211;
    int32_t _n_regionkey146;
};
struct SW_JOIN_INNER_TD_4186_key_rightMajor {
    int32_t _r_regionkey174;
    bool operator==(const SW_JOIN_INNER_TD_4186_key_rightMajor& other) const {
        return ((_r_regionkey174 == other._r_regionkey174));
    }
};
namespace std {
template <>
struct hash<SW_JOIN_INNER_TD_4186_key_rightMajor> {
    std::size_t operator() (const SW_JOIN_INNER_TD_4186_key_rightMajor& k) const {
        using std::size_t;
        using std::hash;
        using std::string;
        return (hash<int32_t>()(k._r_regionkey174));
    }
};
}
struct SW_JOIN_INNER_TD_4186_payload_rightMajor {
    int32_t _r_regionkey174;
};
void SW_JOIN_INNER_TD_4186(Table &tbl_JOIN_INNER_TD_5534_output, Table &tbl_Filter_TD_467_output, Table &tbl_JOIN_INNER_TD_4186_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: JOIN_INNER
    // Operation: ListBuffer((n_regionkey#146 = r_regionkey#174))
    // Left Table: ListBuffer(l_extendedprice#82, l_discount#83, s_nationkey#345, o_orderdate#211, n_regionkey#146)
    // Right Table: ListBuffer(r_regionkey#174)
    // Output Table: ListBuffer(o_orderdate#211, l_extendedprice#82, l_discount#83, s_nationkey#345, n_regionkey#146)
    int left_nrow = tbl_JOIN_INNER_TD_5534_output.getNumRow();
    int right_nrow = tbl_Filter_TD_467_output.getNumRow();
    if (left_nrow < right_nrow) { 
        std::unordered_multimap<SW_JOIN_INNER_TD_4186_key_leftMajor, SW_JOIN_INNER_TD_4186_payload_leftMajor> ht1;
        int nrow1 = tbl_JOIN_INNER_TD_5534_output.getNumRow();
        for (int i = 0; i < nrow1; i++) {
            int32_t _n_regionkey146_k = tbl_JOIN_INNER_TD_5534_output.getInt32(i, 4);
            SW_JOIN_INNER_TD_4186_key_leftMajor keyA{_n_regionkey146_k};
            int32_t _l_extendedprice82 = tbl_JOIN_INNER_TD_5534_output.getInt32(i, 0);
            int32_t _l_discount83 = tbl_JOIN_INNER_TD_5534_output.getInt32(i, 1);
            int32_t _s_nationkey345 = tbl_JOIN_INNER_TD_5534_output.getInt32(i, 2);
            int32_t _o_orderdate211 = tbl_JOIN_INNER_TD_5534_output.getInt32(i, 3);
            int32_t _n_regionkey146 = tbl_JOIN_INNER_TD_5534_output.getInt32(i, 4);
            SW_JOIN_INNER_TD_4186_payload_leftMajor payloadA{_l_extendedprice82, _l_discount83, _s_nationkey345, _o_orderdate211, _n_regionkey146};
            ht1.insert(std::make_pair(keyA, payloadA));
        }
        int r = 0;
        int nrow2 = tbl_Filter_TD_467_output.getNumRow();
        for (int i = 0; i < nrow2; i++) {
            int32_t _r_regionkey174_k = tbl_Filter_TD_467_output.getInt32(i, 0);
            auto its = ht1.equal_range(SW_JOIN_INNER_TD_4186_key_leftMajor{_r_regionkey174_k});
            auto it = its.first;
            while (it != its.second) {
                int32_t _l_extendedprice82 = (it->second)._l_extendedprice82;
                int32_t _l_discount83 = (it->second)._l_discount83;
                int32_t _s_nationkey345 = (it->second)._s_nationkey345;
                int32_t _o_orderdate211 = (it->second)._o_orderdate211;
                int32_t _n_regionkey146 = (it->second)._n_regionkey146;
                int32_t _r_regionkey174 = tbl_Filter_TD_467_output.getInt32(i, 0);
                tbl_JOIN_INNER_TD_4186_output.setInt32(r, 1, _l_extendedprice82);
                tbl_JOIN_INNER_TD_4186_output.setInt32(r, 2, _l_discount83);
                tbl_JOIN_INNER_TD_4186_output.setInt32(r, 3, _s_nationkey345);
                tbl_JOIN_INNER_TD_4186_output.setInt32(r, 0, _o_orderdate211);
                tbl_JOIN_INNER_TD_4186_output.setInt32(r, 4, _n_regionkey146);
                it++;
                r++;
            }
        }
        tbl_JOIN_INNER_TD_4186_output.setNumRow(r);
    } else { 
        std::unordered_multimap<SW_JOIN_INNER_TD_4186_key_rightMajor, SW_JOIN_INNER_TD_4186_payload_rightMajor> ht1;
        int nrow1 = tbl_Filter_TD_467_output.getNumRow();
        for (int i = 0; i < nrow1; i++) {
            int32_t _r_regionkey174_k = tbl_Filter_TD_467_output.getInt32(i, 0);
            SW_JOIN_INNER_TD_4186_key_rightMajor keyA{_r_regionkey174_k};
            int32_t _r_regionkey174 = tbl_Filter_TD_467_output.getInt32(i, 0);
            SW_JOIN_INNER_TD_4186_payload_rightMajor payloadA{_r_regionkey174};
            ht1.insert(std::make_pair(keyA, payloadA));
        }
        int r = 0;
        int nrow2 = tbl_JOIN_INNER_TD_5534_output.getNumRow();
        for (int i = 0; i < nrow2; i++) {
            int32_t _n_regionkey146_k = tbl_JOIN_INNER_TD_5534_output.getInt32(i, 4);
            auto its = ht1.equal_range(SW_JOIN_INNER_TD_4186_key_rightMajor{_n_regionkey146_k});
            auto it = its.first;
            while (it != its.second) {
                int32_t _r_regionkey174 = (it->second)._r_regionkey174;
                int32_t _l_extendedprice82 = tbl_JOIN_INNER_TD_5534_output.getInt32(i, 0);
                int32_t _l_discount83 = tbl_JOIN_INNER_TD_5534_output.getInt32(i, 1);
                int32_t _s_nationkey345 = tbl_JOIN_INNER_TD_5534_output.getInt32(i, 2);
                int32_t _o_orderdate211 = tbl_JOIN_INNER_TD_5534_output.getInt32(i, 3);
                int32_t _n_regionkey146 = tbl_JOIN_INNER_TD_5534_output.getInt32(i, 4);
                tbl_JOIN_INNER_TD_4186_output.setInt32(r, 1, _l_extendedprice82);
                tbl_JOIN_INNER_TD_4186_output.setInt32(r, 2, _l_discount83);
                tbl_JOIN_INNER_TD_4186_output.setInt32(r, 3, _s_nationkey345);
                tbl_JOIN_INNER_TD_4186_output.setInt32(r, 0, _o_orderdate211);
                tbl_JOIN_INNER_TD_4186_output.setInt32(r, 4, _n_regionkey146);
                it++;
                r++;
            }
        }
        tbl_JOIN_INNER_TD_4186_output.setNumRow(r);
    } 
    std::cout << "tbl_JOIN_INNER_TD_4186_output #Row: " << tbl_JOIN_INNER_TD_4186_output.getNumRow() << std::endl;
}

struct SW_JOIN_INNER_TD_3778_key_leftMajor {
    int32_t _s_nationkey345;
    bool operator==(const SW_JOIN_INNER_TD_3778_key_leftMajor& other) const {
        return ((_s_nationkey345 == other._s_nationkey345));
    }
};
namespace std {
template <>
struct hash<SW_JOIN_INNER_TD_3778_key_leftMajor> {
    std::size_t operator() (const SW_JOIN_INNER_TD_3778_key_leftMajor& k) const {
        using std::size_t;
        using std::hash;
        using std::string;
        return (hash<int32_t>()(k._s_nationkey345));
    }
};
}
struct SW_JOIN_INNER_TD_3778_payload_leftMajor {
    int32_t _o_orderdate211;
    int32_t _l_extendedprice82;
    int32_t _l_discount83;
    int32_t _s_nationkey345;
    int32_t _n_regionkey146;
};
struct SW_JOIN_INNER_TD_3778_key_rightMajor {
    int32_t _n_nationkey951;
    bool operator==(const SW_JOIN_INNER_TD_3778_key_rightMajor& other) const {
        return ((_n_nationkey951 == other._n_nationkey951));
    }
};
namespace std {
template <>
struct hash<SW_JOIN_INNER_TD_3778_key_rightMajor> {
    std::size_t operator() (const SW_JOIN_INNER_TD_3778_key_rightMajor& k) const {
        using std::size_t;
        using std::hash;
        using std::string;
        return (hash<int32_t>()(k._n_nationkey951));
    }
};
}
struct SW_JOIN_INNER_TD_3778_payload_rightMajor {
    int32_t _n_nationkey951;
    std::string _n_name952;
};
void SW_JOIN_INNER_TD_3778(Table &tbl_JOIN_INNER_TD_4186_output, Table &tbl_SerializeFromObject_TD_4136_input, Table &tbl_JOIN_INNER_TD_3778_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: JOIN_INNER
    // Operation: ListBuffer((s_nationkey#345 = n_nationkey#951))
    // Left Table: ListBuffer(o_orderdate#211, l_extendedprice#82, l_discount#83, s_nationkey#345, n_regionkey#146)
    // Right Table: ListBuffer(n_nationkey#951, n_name#952)
    // Output Table: ListBuffer(o_orderdate#211, l_extendedprice#82, l_discount#83, n_name#952)
    int left_nrow = tbl_JOIN_INNER_TD_4186_output.getNumRow();
    int right_nrow = tbl_SerializeFromObject_TD_4136_input.getNumRow();
    if (left_nrow < right_nrow) { 
        std::unordered_multimap<SW_JOIN_INNER_TD_3778_key_leftMajor, SW_JOIN_INNER_TD_3778_payload_leftMajor> ht1;
        int nrow1 = tbl_JOIN_INNER_TD_4186_output.getNumRow();
        for (int i = 0; i < nrow1; i++) {
            int32_t _s_nationkey345_k = tbl_JOIN_INNER_TD_4186_output.getInt32(i, 3);
            SW_JOIN_INNER_TD_3778_key_leftMajor keyA{_s_nationkey345_k};
            int32_t _o_orderdate211 = tbl_JOIN_INNER_TD_4186_output.getInt32(i, 0);
            int32_t _l_extendedprice82 = tbl_JOIN_INNER_TD_4186_output.getInt32(i, 1);
            int32_t _l_discount83 = tbl_JOIN_INNER_TD_4186_output.getInt32(i, 2);
            int32_t _s_nationkey345 = tbl_JOIN_INNER_TD_4186_output.getInt32(i, 3);
            int32_t _n_regionkey146 = tbl_JOIN_INNER_TD_4186_output.getInt32(i, 4);
            SW_JOIN_INNER_TD_3778_payload_leftMajor payloadA{_o_orderdate211, _l_extendedprice82, _l_discount83, _s_nationkey345, _n_regionkey146};
            ht1.insert(std::make_pair(keyA, payloadA));
        }
        int r = 0;
        int nrow2 = tbl_SerializeFromObject_TD_4136_input.getNumRow();
        for (int i = 0; i < nrow2; i++) {
            int32_t _n_nationkey951_k = tbl_SerializeFromObject_TD_4136_input.getInt32(i, 0);
            auto its = ht1.equal_range(SW_JOIN_INNER_TD_3778_key_leftMajor{_n_nationkey951_k});
            auto it = its.first;
            while (it != its.second) {
                int32_t _o_orderdate211 = (it->second)._o_orderdate211;
                int32_t _l_extendedprice82 = (it->second)._l_extendedprice82;
                int32_t _l_discount83 = (it->second)._l_discount83;
                int32_t _s_nationkey345 = (it->second)._s_nationkey345;
                int32_t _n_regionkey146 = (it->second)._n_regionkey146;
                int32_t _n_nationkey951 = tbl_SerializeFromObject_TD_4136_input.getInt32(i, 0);
                std::array<char, TPCH_READ_NATION_LEN + 1> _n_name952_n = tbl_SerializeFromObject_TD_4136_input.getcharN<char, TPCH_READ_NATION_LEN + 1>(i, 1);
                std::string _n_name952 = std::string(_n_name952_n.data());
                tbl_JOIN_INNER_TD_3778_output.setInt32(r, 0, _o_orderdate211);
                tbl_JOIN_INNER_TD_3778_output.setInt32(r, 1, _l_extendedprice82);
                tbl_JOIN_INNER_TD_3778_output.setInt32(r, 2, _l_discount83);
                tbl_JOIN_INNER_TD_3778_output.setcharN<char, TPCH_READ_NATION_LEN + 1>(r, 3, _n_name952_n);
                it++;
                r++;
            }
        }
        tbl_JOIN_INNER_TD_3778_output.setNumRow(r);
    } else { 
        std::unordered_multimap<SW_JOIN_INNER_TD_3778_key_rightMajor, SW_JOIN_INNER_TD_3778_payload_rightMajor> ht1;
        int nrow1 = tbl_SerializeFromObject_TD_4136_input.getNumRow();
        for (int i = 0; i < nrow1; i++) {
            int32_t _n_nationkey951_k = tbl_SerializeFromObject_TD_4136_input.getInt32(i, 0);
            SW_JOIN_INNER_TD_3778_key_rightMajor keyA{_n_nationkey951_k};
            int32_t _n_nationkey951 = tbl_SerializeFromObject_TD_4136_input.getInt32(i, 0);
            std::array<char, TPCH_READ_NATION_LEN + 1> _n_name952_n = tbl_SerializeFromObject_TD_4136_input.getcharN<char, TPCH_READ_NATION_LEN + 1>(i, 1);
            std::string _n_name952 = std::string(_n_name952_n.data());
            SW_JOIN_INNER_TD_3778_payload_rightMajor payloadA{_n_nationkey951, _n_name952};
            ht1.insert(std::make_pair(keyA, payloadA));
        }
        int r = 0;
        int nrow2 = tbl_JOIN_INNER_TD_4186_output.getNumRow();
        for (int i = 0; i < nrow2; i++) {
            int32_t _s_nationkey345_k = tbl_JOIN_INNER_TD_4186_output.getInt32(i, 3);
            auto its = ht1.equal_range(SW_JOIN_INNER_TD_3778_key_rightMajor{_s_nationkey345_k});
            auto it = its.first;
            while (it != its.second) {
                int32_t _n_nationkey951 = (it->second)._n_nationkey951;
                std::string _n_name952 = (it->second)._n_name952;
                std::array<char, TPCH_READ_NATION_LEN + 1> _n_name952_n{};
                memcpy(_n_name952_n.data(), (_n_name952).data(), (_n_name952).length());
                int32_t _o_orderdate211 = tbl_JOIN_INNER_TD_4186_output.getInt32(i, 0);
                int32_t _l_extendedprice82 = tbl_JOIN_INNER_TD_4186_output.getInt32(i, 1);
                int32_t _l_discount83 = tbl_JOIN_INNER_TD_4186_output.getInt32(i, 2);
                int32_t _s_nationkey345 = tbl_JOIN_INNER_TD_4186_output.getInt32(i, 3);
                int32_t _n_regionkey146 = tbl_JOIN_INNER_TD_4186_output.getInt32(i, 4);
                tbl_JOIN_INNER_TD_3778_output.setcharN<char, TPCH_READ_NATION_LEN + 1>(r, 3, _n_name952_n);
                tbl_JOIN_INNER_TD_3778_output.setInt32(r, 0, _o_orderdate211);
                tbl_JOIN_INNER_TD_3778_output.setInt32(r, 1, _l_extendedprice82);
                tbl_JOIN_INNER_TD_3778_output.setInt32(r, 2, _l_discount83);
                it++;
                r++;
            }
        }
        tbl_JOIN_INNER_TD_3778_output.setNumRow(r);
    } 
    std::cout << "tbl_JOIN_INNER_TD_3778_output #Row: " << tbl_JOIN_INNER_TD_3778_output.getNumRow() << std::endl;
}

void SW_Project_TD_2893(Table &tbl_JOIN_INNER_TD_3778_output, Table &tbl_Project_TD_2893_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: Project
    // Operation: ListBuffer(cast((cast(o_orderdate#211 as double) / 10000.0) as int) AS o_year#946, (l_extendedprice#82 * (100 - l_discount#83)) AS volume#947, n_name#952 AS nation#948)
    // Input: ListBuffer(o_orderdate#211, l_extendedprice#82, l_discount#83, n_name#952)
    // Output: ListBuffer(o_year#946, volume#947, nation#948)
    int nrow1 = tbl_JOIN_INNER_TD_3778_output.getNumRow();
    for (int i = 0; i < nrow1; i++) {
        int32_t _o_orderdate211 = tbl_JOIN_INNER_TD_3778_output.getInt32(i, 0);
        int32_t _l_extendedprice82 = tbl_JOIN_INNER_TD_3778_output.getInt32(i, 1);
        int32_t _l_discount83 = tbl_JOIN_INNER_TD_3778_output.getInt32(i, 2);
        std::array<char, TPCH_READ_NATION_LEN + 1> _n_name952 = tbl_JOIN_INNER_TD_3778_output.getcharN<char, TPCH_READ_NATION_LEN + 1>(i, 3);
        int32_t _o_year946 = (_o_orderdate211 / 10000.0);
        tbl_Project_TD_2893_output.setInt32(i, 0, _o_year946);
        int32_t _volume947 = (_l_extendedprice82 * (100 - _l_discount83));
        tbl_Project_TD_2893_output.setInt32(i, 1, _volume947);
        std::array<char, TPCH_READ_NATION_LEN + 1> _nation948 = _n_name952;
        tbl_Project_TD_2893_output.setcharN<char, TPCH_READ_NATION_LEN + 1>(i, 2, _nation948);
    }
    tbl_Project_TD_2893_output.setNumRow(nrow1);
    std::cout << "tbl_Project_TD_2893_output #Row: " << tbl_Project_TD_2893_output.getNumRow() << std::endl;
}

typedef int32_t SW_Aggregate_TD_1178_key;
struct SW_Aggregate_TD_1178_payload {
    int64_t _mkt_share949_sum_0;
    int64_t _mkt_share949_sum_1;
};
void SW_Aggregate_TD_1178(Table &tbl_Project_TD_2893_output, Table &tbl_Aggregate_TD_1178_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: Aggregate
    // Operation: ListBuffer(o_year#946, (cast(sum(CASE WHEN (nation#948 = IRAQ) THEN volume#947 ELSE 0 END) as double) / cast(sum(volume#947) as double)) AS mkt_share#949)
    // Input: ListBuffer(o_year#946, volume#947, nation#948)
    // Output: ListBuffer(o_year#946, mkt_share#949)
    std::unordered_map<SW_Aggregate_TD_1178_key, SW_Aggregate_TD_1178_payload> ht1;
    int nrow1 = tbl_Project_TD_2893_output.getNumRow();
    for (int i = 0; i < nrow1; i++) {
        int32_t _o_year946 = tbl_Project_TD_2893_output.getInt32(i, 0);
        int32_t _volume947 = tbl_Project_TD_2893_output.getInt32(i, 1);
        std::array<char, TPCH_READ_NATION_LEN + 1> _nation948 = tbl_Project_TD_2893_output.getcharN<char, TPCH_READ_NATION_LEN + 1>(i, 2);
        SW_Aggregate_TD_1178_key k = _o_year946;
        int64_t _mkt_share949_sum_0 = (std::string(_nation948.data()) == "IRAQ") ? _volume947 : 0;
        int64_t _mkt_share949_sum_1 = _volume947;
        SW_Aggregate_TD_1178_payload p{_mkt_share949_sum_0, _mkt_share949_sum_1};
        auto it = ht1.find(k);
        if (it != ht1.end()) {
            int64_t sum_0 = (it->second)._mkt_share949_sum_0 + _mkt_share949_sum_0;
            p._mkt_share949_sum_0 = sum_0;
            int64_t sum_1 = (it->second)._mkt_share949_sum_1 + _mkt_share949_sum_1;
            p._mkt_share949_sum_1 = sum_1;
            ht1[k] = p;
        } else { 
            ht1.insert(std::make_pair(k, p));
        }
    }
    int r = 0;
    for (auto& it : ht1) {
        tbl_Aggregate_TD_1178_output.setInt32(r, 0, (it.first));
        int64_t _mkt_share949 = ((it.second)._mkt_share949_sum_0 / (it.second)._mkt_share949_sum_1);
        tbl_Aggregate_TD_1178_output.setInt64(r, 1, _mkt_share949);
        ++r;
    }
    tbl_Aggregate_TD_1178_output.setNumRow(r);
    std::cout << "tbl_Aggregate_TD_1178_output #Row: " << tbl_Aggregate_TD_1178_output.getNumRow() << std::endl;
}

void SW_Sort_TD_0398(Table &tbl_Aggregate_TD_1178_output, Table &tbl_Sort_TD_0398_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: Sort
    // Operation: ListBuffer(o_year#946 ASC NULLS FIRST)
    // Input: ListBuffer(o_year#946, mkt_share#949)
    // Output: ListBuffer(o_year#946, mkt_share#949)
    struct SW_Sort_TD_0398Row {
        int32_t _o_year946;
        int64_t _mkt_share949;
    }; 

    struct {
        bool operator()(const SW_Sort_TD_0398Row& a, const SW_Sort_TD_0398Row& b) const { return 
 (a._o_year946 < b._o_year946); 
}
    }SW_Sort_TD_0398_order; 

    int nrow1 = tbl_Aggregate_TD_1178_output.getNumRow();
    std::vector<SW_Sort_TD_0398Row> rows;
    for (int i = 0; i < nrow1; i++) {
        int32_t _o_year946 = tbl_Aggregate_TD_1178_output.getInt32(i, 0);
        int64_t _mkt_share949 = tbl_Aggregate_TD_1178_output.getInt64(i, 1);
        SW_Sort_TD_0398Row t = {_o_year946,_mkt_share949};
        rows.push_back(t);
    }
    std::sort(rows.begin(), rows.end(), SW_Sort_TD_0398_order);
    int r = 0;
    for (auto& it : rows) {
        tbl_Sort_TD_0398_output.setInt32(r, 0, it._o_year946);
        tbl_Sort_TD_0398_output.setInt64(r, 1, it._mkt_share949);
        if (r < 10) {
            std::cout << it._o_year946 << " " << it._mkt_share949 << " " << std::endl;
        }
        ++r;
    }
    tbl_Sort_TD_0398_output.setNumRow(r);
    std::cout << "tbl_Sort_TD_0398_output #Row: " << tbl_Sort_TD_0398_output.getNumRow() << std::endl;
}

