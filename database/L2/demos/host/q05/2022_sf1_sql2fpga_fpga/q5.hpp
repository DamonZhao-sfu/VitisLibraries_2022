#include <regex> 
#include <stdint.h> 

void SW_Filter_TD_7898(Table &tbl_SerializeFromObject_TD_8226_input, Table &tbl_Filter_TD_7898_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: Filter
    // Operation: ListBuffer(((o_orderdate#211 >= 19930101) AND (o_orderdate#211 < 19940101)))
    // Input: ListBuffer(o_orderkey#207, o_custkey#208, o_orderdate#211)
    // Output: ListBuffer(o_orderkey#207, o_custkey#208)
    int r = 0;
    int nrow1 = tbl_SerializeFromObject_TD_8226_input.getNumRow();
    for (int i = 0; i < nrow1; i++) {
        int32_t _o_orderdate211 = tbl_SerializeFromObject_TD_8226_input.getInt32(i, 2);
        if ((_o_orderdate211 >= 19930101) && (_o_orderdate211 < 19940101)) {
            int32_t _o_orderkey207_t = tbl_SerializeFromObject_TD_8226_input.getInt32(i, 0);
            tbl_Filter_TD_7898_output.setInt32(r, 0, _o_orderkey207_t);
            int32_t _o_custkey208_t = tbl_SerializeFromObject_TD_8226_input.getInt32(i, 1);
            tbl_Filter_TD_7898_output.setInt32(r, 1, _o_custkey208_t);
            r++;
        }
    }
    tbl_Filter_TD_7898_output.setNumRow(r);
    std::cout << "tbl_Filter_TD_7898_output #Row: " << tbl_Filter_TD_7898_output.getNumRow() << std::endl;
}

struct SW_JOIN_INNER_TD_6305_key_leftMajor {
    int32_t _c_custkey22;
    bool operator==(const SW_JOIN_INNER_TD_6305_key_leftMajor& other) const {
        return ((_c_custkey22 == other._c_custkey22));
    }
};
namespace std {
template <>
struct hash<SW_JOIN_INNER_TD_6305_key_leftMajor> {
    std::size_t operator() (const SW_JOIN_INNER_TD_6305_key_leftMajor& k) const {
        using std::size_t;
        using std::hash;
        using std::string;
        return (hash<int32_t>()(k._c_custkey22));
    }
};
}
struct SW_JOIN_INNER_TD_6305_payload_leftMajor {
    int32_t _c_custkey22;
    int32_t _c_nationkey25;
};
struct SW_JOIN_INNER_TD_6305_key_rightMajor {
    int32_t _o_custkey208;
    bool operator==(const SW_JOIN_INNER_TD_6305_key_rightMajor& other) const {
        return ((_o_custkey208 == other._o_custkey208));
    }
};
namespace std {
template <>
struct hash<SW_JOIN_INNER_TD_6305_key_rightMajor> {
    std::size_t operator() (const SW_JOIN_INNER_TD_6305_key_rightMajor& k) const {
        using std::size_t;
        using std::hash;
        using std::string;
        return (hash<int32_t>()(k._o_custkey208));
    }
};
}
struct SW_JOIN_INNER_TD_6305_payload_rightMajor {
    int32_t _o_orderkey207;
    int32_t _o_custkey208;
};
void SW_JOIN_INNER_TD_6305(Table &tbl_SerializeFromObject_TD_7970_input, Table &tbl_Filter_TD_7898_output, Table &tbl_JOIN_INNER_TD_6305_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: JOIN_INNER
    // Operation: ListBuffer((c_custkey#22 = o_custkey#208))
    // Left Table: ListBuffer(c_custkey#22, c_nationkey#25)
    // Right Table: ListBuffer(o_orderkey#207, o_custkey#208)
    // Output Table: ListBuffer(c_nationkey#25, o_orderkey#207)
    int left_nrow = tbl_SerializeFromObject_TD_7970_input.getNumRow();
    int right_nrow = tbl_Filter_TD_7898_output.getNumRow();
    if (left_nrow < right_nrow) { 
        std::unordered_multimap<SW_JOIN_INNER_TD_6305_key_leftMajor, SW_JOIN_INNER_TD_6305_payload_leftMajor> ht1;
        int nrow1 = tbl_SerializeFromObject_TD_7970_input.getNumRow();
        for (int i = 0; i < nrow1; i++) {
            int32_t _c_custkey22_k = tbl_SerializeFromObject_TD_7970_input.getInt32(i, 0);
            SW_JOIN_INNER_TD_6305_key_leftMajor keyA{_c_custkey22_k};
            int32_t _c_custkey22 = tbl_SerializeFromObject_TD_7970_input.getInt32(i, 0);
            int32_t _c_nationkey25 = tbl_SerializeFromObject_TD_7970_input.getInt32(i, 1);
            SW_JOIN_INNER_TD_6305_payload_leftMajor payloadA{_c_custkey22, _c_nationkey25};
            ht1.insert(std::make_pair(keyA, payloadA));
        }
        int r = 0;
        int nrow2 = tbl_Filter_TD_7898_output.getNumRow();
        for (int i = 0; i < nrow2; i++) {
            int32_t _o_custkey208_k = tbl_Filter_TD_7898_output.getInt32(i, 1);
            auto its = ht1.equal_range(SW_JOIN_INNER_TD_6305_key_leftMajor{_o_custkey208_k});
            auto it = its.first;
            while (it != its.second) {
                int32_t _c_custkey22 = (it->second)._c_custkey22;
                int32_t _c_nationkey25 = (it->second)._c_nationkey25;
                int32_t _o_orderkey207 = tbl_Filter_TD_7898_output.getInt32(i, 0);
                int32_t _o_custkey208 = tbl_Filter_TD_7898_output.getInt32(i, 1);
                tbl_JOIN_INNER_TD_6305_output.setInt32(r, 0, _c_nationkey25);
                tbl_JOIN_INNER_TD_6305_output.setInt32(r, 1, _o_orderkey207);
                it++;
                r++;
            }
        }
        tbl_JOIN_INNER_TD_6305_output.setNumRow(r);
    } else { 
        std::unordered_multimap<SW_JOIN_INNER_TD_6305_key_rightMajor, SW_JOIN_INNER_TD_6305_payload_rightMajor> ht1;
        int nrow1 = tbl_Filter_TD_7898_output.getNumRow();
        for (int i = 0; i < nrow1; i++) {
            int32_t _o_custkey208_k = tbl_Filter_TD_7898_output.getInt32(i, 1);
            SW_JOIN_INNER_TD_6305_key_rightMajor keyA{_o_custkey208_k};
            int32_t _o_orderkey207 = tbl_Filter_TD_7898_output.getInt32(i, 0);
            int32_t _o_custkey208 = tbl_Filter_TD_7898_output.getInt32(i, 1);
            SW_JOIN_INNER_TD_6305_payload_rightMajor payloadA{_o_orderkey207, _o_custkey208};
            ht1.insert(std::make_pair(keyA, payloadA));
        }
        int r = 0;
        int nrow2 = tbl_SerializeFromObject_TD_7970_input.getNumRow();
        for (int i = 0; i < nrow2; i++) {
            int32_t _c_custkey22_k = tbl_SerializeFromObject_TD_7970_input.getInt32(i, 0);
            auto its = ht1.equal_range(SW_JOIN_INNER_TD_6305_key_rightMajor{_c_custkey22_k});
            auto it = its.first;
            while (it != its.second) {
                int32_t _o_orderkey207 = (it->second)._o_orderkey207;
                int32_t _o_custkey208 = (it->second)._o_custkey208;
                int32_t _c_custkey22 = tbl_SerializeFromObject_TD_7970_input.getInt32(i, 0);
                int32_t _c_nationkey25 = tbl_SerializeFromObject_TD_7970_input.getInt32(i, 1);
                tbl_JOIN_INNER_TD_6305_output.setInt32(r, 1, _o_orderkey207);
                tbl_JOIN_INNER_TD_6305_output.setInt32(r, 0, _c_nationkey25);
                it++;
                r++;
            }
        }
        tbl_JOIN_INNER_TD_6305_output.setNumRow(r);
    } 
    std::cout << "tbl_JOIN_INNER_TD_6305_output #Row: " << tbl_JOIN_INNER_TD_6305_output.getNumRow() << std::endl;
}

struct SW_JOIN_INNER_TD_5328_key_leftMajor {
    int32_t _o_orderkey207;
    bool operator==(const SW_JOIN_INNER_TD_5328_key_leftMajor& other) const {
        return ((_o_orderkey207 == other._o_orderkey207));
    }
};
namespace std {
template <>
struct hash<SW_JOIN_INNER_TD_5328_key_leftMajor> {
    std::size_t operator() (const SW_JOIN_INNER_TD_5328_key_leftMajor& k) const {
        using std::size_t;
        using std::hash;
        using std::string;
        return (hash<int32_t>()(k._o_orderkey207));
    }
};
}
struct SW_JOIN_INNER_TD_5328_payload_leftMajor {
    int32_t _c_nationkey25;
    int32_t _o_orderkey207;
};
struct SW_JOIN_INNER_TD_5328_key_rightMajor {
    int32_t _l_orderkey77;
    bool operator==(const SW_JOIN_INNER_TD_5328_key_rightMajor& other) const {
        return ((_l_orderkey77 == other._l_orderkey77));
    }
};
namespace std {
template <>
struct hash<SW_JOIN_INNER_TD_5328_key_rightMajor> {
    std::size_t operator() (const SW_JOIN_INNER_TD_5328_key_rightMajor& k) const {
        using std::size_t;
        using std::hash;
        using std::string;
        return (hash<int32_t>()(k._l_orderkey77));
    }
};
}
struct SW_JOIN_INNER_TD_5328_payload_rightMajor {
    int32_t _l_orderkey77;
    int32_t _l_suppkey79;
    int32_t _l_extendedprice82;
    int32_t _l_discount83;
};
void SW_JOIN_INNER_TD_5328(Table &tbl_JOIN_INNER_TD_6305_output, Table &tbl_SerializeFromObject_TD_6954_input, Table &tbl_JOIN_INNER_TD_5328_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: JOIN_INNER
    // Operation: ListBuffer((l_orderkey#77 = o_orderkey#207))
    // Left Table: ListBuffer(c_nationkey#25, o_orderkey#207)
    // Right Table: ListBuffer(l_orderkey#77, l_suppkey#79, l_extendedprice#82, l_discount#83)
    // Output Table: ListBuffer(c_nationkey#25, l_suppkey#79, l_extendedprice#82, l_discount#83)
    int left_nrow = tbl_JOIN_INNER_TD_6305_output.getNumRow();
    int right_nrow = tbl_SerializeFromObject_TD_6954_input.getNumRow();
    if (left_nrow < right_nrow) { 
        std::unordered_multimap<SW_JOIN_INNER_TD_5328_key_leftMajor, SW_JOIN_INNER_TD_5328_payload_leftMajor> ht1;
        int nrow1 = tbl_JOIN_INNER_TD_6305_output.getNumRow();
        for (int i = 0; i < nrow1; i++) {
            int32_t _o_orderkey207_k = tbl_JOIN_INNER_TD_6305_output.getInt32(i, 1);
            SW_JOIN_INNER_TD_5328_key_leftMajor keyA{_o_orderkey207_k};
            int32_t _c_nationkey25 = tbl_JOIN_INNER_TD_6305_output.getInt32(i, 0);
            int32_t _o_orderkey207 = tbl_JOIN_INNER_TD_6305_output.getInt32(i, 1);
            SW_JOIN_INNER_TD_5328_payload_leftMajor payloadA{_c_nationkey25, _o_orderkey207};
            ht1.insert(std::make_pair(keyA, payloadA));
        }
        int r = 0;
        int nrow2 = tbl_SerializeFromObject_TD_6954_input.getNumRow();
        for (int i = 0; i < nrow2; i++) {
            int32_t _l_orderkey77_k = tbl_SerializeFromObject_TD_6954_input.getInt32(i, 0);
            auto its = ht1.equal_range(SW_JOIN_INNER_TD_5328_key_leftMajor{_l_orderkey77_k});
            auto it = its.first;
            while (it != its.second) {
                int32_t _c_nationkey25 = (it->second)._c_nationkey25;
                int32_t _o_orderkey207 = (it->second)._o_orderkey207;
                int32_t _l_orderkey77 = tbl_SerializeFromObject_TD_6954_input.getInt32(i, 0);
                int32_t _l_suppkey79 = tbl_SerializeFromObject_TD_6954_input.getInt32(i, 1);
                int32_t _l_extendedprice82 = tbl_SerializeFromObject_TD_6954_input.getInt32(i, 2);
                int32_t _l_discount83 = tbl_SerializeFromObject_TD_6954_input.getInt32(i, 3);
                tbl_JOIN_INNER_TD_5328_output.setInt32(r, 0, _c_nationkey25);
                tbl_JOIN_INNER_TD_5328_output.setInt32(r, 1, _l_suppkey79);
                tbl_JOIN_INNER_TD_5328_output.setInt32(r, 2, _l_extendedprice82);
                tbl_JOIN_INNER_TD_5328_output.setInt32(r, 3, _l_discount83);
                it++;
                r++;
            }
        }
        tbl_JOIN_INNER_TD_5328_output.setNumRow(r);
    } else { 
        std::unordered_multimap<SW_JOIN_INNER_TD_5328_key_rightMajor, SW_JOIN_INNER_TD_5328_payload_rightMajor> ht1;
        int nrow1 = tbl_SerializeFromObject_TD_6954_input.getNumRow();
        for (int i = 0; i < nrow1; i++) {
            int32_t _l_orderkey77_k = tbl_SerializeFromObject_TD_6954_input.getInt32(i, 0);
            SW_JOIN_INNER_TD_5328_key_rightMajor keyA{_l_orderkey77_k};
            int32_t _l_orderkey77 = tbl_SerializeFromObject_TD_6954_input.getInt32(i, 0);
            int32_t _l_suppkey79 = tbl_SerializeFromObject_TD_6954_input.getInt32(i, 1);
            int32_t _l_extendedprice82 = tbl_SerializeFromObject_TD_6954_input.getInt32(i, 2);
            int32_t _l_discount83 = tbl_SerializeFromObject_TD_6954_input.getInt32(i, 3);
            SW_JOIN_INNER_TD_5328_payload_rightMajor payloadA{_l_orderkey77, _l_suppkey79, _l_extendedprice82, _l_discount83};
            ht1.insert(std::make_pair(keyA, payloadA));
        }
        int r = 0;
        int nrow2 = tbl_JOIN_INNER_TD_6305_output.getNumRow();
        for (int i = 0; i < nrow2; i++) {
            int32_t _o_orderkey207_k = tbl_JOIN_INNER_TD_6305_output.getInt32(i, 1);
            auto its = ht1.equal_range(SW_JOIN_INNER_TD_5328_key_rightMajor{_o_orderkey207_k});
            auto it = its.first;
            while (it != its.second) {
                int32_t _l_orderkey77 = (it->second)._l_orderkey77;
                int32_t _l_suppkey79 = (it->second)._l_suppkey79;
                int32_t _l_extendedprice82 = (it->second)._l_extendedprice82;
                int32_t _l_discount83 = (it->second)._l_discount83;
                int32_t _c_nationkey25 = tbl_JOIN_INNER_TD_6305_output.getInt32(i, 0);
                int32_t _o_orderkey207 = tbl_JOIN_INNER_TD_6305_output.getInt32(i, 1);
                tbl_JOIN_INNER_TD_5328_output.setInt32(r, 1, _l_suppkey79);
                tbl_JOIN_INNER_TD_5328_output.setInt32(r, 2, _l_extendedprice82);
                tbl_JOIN_INNER_TD_5328_output.setInt32(r, 3, _l_discount83);
                tbl_JOIN_INNER_TD_5328_output.setInt32(r, 0, _c_nationkey25);
                it++;
                r++;
            }
        }
        tbl_JOIN_INNER_TD_5328_output.setNumRow(r);
    } 
    std::cout << "tbl_JOIN_INNER_TD_5328_output #Row: " << tbl_JOIN_INNER_TD_5328_output.getNumRow() << std::endl;
}

struct SW_JOIN_INNER_TD_4813_key_leftMajor {
    int32_t _l_suppkey79;
    int32_t _c_nationkey25;
    bool operator==(const SW_JOIN_INNER_TD_4813_key_leftMajor& other) const {
        return ((_l_suppkey79 == other._l_suppkey79) && (_c_nationkey25 == other._c_nationkey25));
    }
};
namespace std {
template <>
struct hash<SW_JOIN_INNER_TD_4813_key_leftMajor> {
    std::size_t operator() (const SW_JOIN_INNER_TD_4813_key_leftMajor& k) const {
        using std::size_t;
        using std::hash;
        using std::string;
        return (hash<int32_t>()(k._l_suppkey79)) + (hash<int32_t>()(k._c_nationkey25));
    }
};
}
struct SW_JOIN_INNER_TD_4813_payload_leftMajor {
    int32_t _c_nationkey25;
    int32_t _l_suppkey79;
    int32_t _l_extendedprice82;
    int32_t _l_discount83;
};
struct SW_JOIN_INNER_TD_4813_key_rightMajor {
    int32_t _s_suppkey342;
    int32_t _s_nationkey345;
    bool operator==(const SW_JOIN_INNER_TD_4813_key_rightMajor& other) const {
        return ((_s_suppkey342 == other._s_suppkey342) && (_s_nationkey345 == other._s_nationkey345));
    }
};
namespace std {
template <>
struct hash<SW_JOIN_INNER_TD_4813_key_rightMajor> {
    std::size_t operator() (const SW_JOIN_INNER_TD_4813_key_rightMajor& k) const {
        using std::size_t;
        using std::hash;
        using std::string;
        return (hash<int32_t>()(k._s_suppkey342)) + (hash<int32_t>()(k._s_nationkey345));
    }
};
}
struct SW_JOIN_INNER_TD_4813_payload_rightMajor {
    int32_t _s_suppkey342;
    int32_t _s_nationkey345;
};
void SW_JOIN_INNER_TD_4813(Table &tbl_JOIN_INNER_TD_5328_output, Table &tbl_SerializeFromObject_TD_5371_input, Table &tbl_JOIN_INNER_TD_4813_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: JOIN_INNER
    // Operation: ListBuffer(((l_suppkey#79 = s_suppkey#342) AND (c_nationkey#25 = s_nationkey#345)))
    // Left Table: ListBuffer(c_nationkey#25, l_suppkey#79, l_extendedprice#82, l_discount#83)
    // Right Table: ListBuffer(s_suppkey#342, s_nationkey#345)
    // Output Table: ListBuffer(l_extendedprice#82, l_discount#83, s_nationkey#345)
    int left_nrow = tbl_JOIN_INNER_TD_5328_output.getNumRow();
    int right_nrow = tbl_SerializeFromObject_TD_5371_input.getNumRow();
    if (left_nrow < right_nrow) { 
        std::unordered_multimap<SW_JOIN_INNER_TD_4813_key_leftMajor, SW_JOIN_INNER_TD_4813_payload_leftMajor> ht1;
        int nrow1 = tbl_JOIN_INNER_TD_5328_output.getNumRow();
        for (int i = 0; i < nrow1; i++) {
            int32_t _l_suppkey79_k = tbl_JOIN_INNER_TD_5328_output.getInt32(i, 1);
            int32_t _c_nationkey25_k = tbl_JOIN_INNER_TD_5328_output.getInt32(i, 0);
            SW_JOIN_INNER_TD_4813_key_leftMajor keyA{_l_suppkey79_k, _c_nationkey25_k};
            int32_t _c_nationkey25 = tbl_JOIN_INNER_TD_5328_output.getInt32(i, 0);
            int32_t _l_suppkey79 = tbl_JOIN_INNER_TD_5328_output.getInt32(i, 1);
            int32_t _l_extendedprice82 = tbl_JOIN_INNER_TD_5328_output.getInt32(i, 2);
            int32_t _l_discount83 = tbl_JOIN_INNER_TD_5328_output.getInt32(i, 3);
            SW_JOIN_INNER_TD_4813_payload_leftMajor payloadA{_c_nationkey25, _l_suppkey79, _l_extendedprice82, _l_discount83};
            ht1.insert(std::make_pair(keyA, payloadA));
        }
        int r = 0;
        int nrow2 = tbl_SerializeFromObject_TD_5371_input.getNumRow();
        for (int i = 0; i < nrow2; i++) {
            int32_t _s_suppkey342_k = tbl_SerializeFromObject_TD_5371_input.getInt32(i, 0);
            int32_t _s_nationkey345_k = tbl_SerializeFromObject_TD_5371_input.getInt32(i, 1);
            auto its = ht1.equal_range(SW_JOIN_INNER_TD_4813_key_leftMajor{_s_suppkey342_k, _s_nationkey345_k});
            auto it = its.first;
            while (it != its.second) {
                int32_t _c_nationkey25 = (it->second)._c_nationkey25;
                int32_t _l_suppkey79 = (it->second)._l_suppkey79;
                int32_t _l_extendedprice82 = (it->second)._l_extendedprice82;
                int32_t _l_discount83 = (it->second)._l_discount83;
                int32_t _s_suppkey342 = tbl_SerializeFromObject_TD_5371_input.getInt32(i, 0);
                int32_t _s_nationkey345 = tbl_SerializeFromObject_TD_5371_input.getInt32(i, 1);
                tbl_JOIN_INNER_TD_4813_output.setInt32(r, 0, _l_extendedprice82);
                tbl_JOIN_INNER_TD_4813_output.setInt32(r, 1, _l_discount83);
                tbl_JOIN_INNER_TD_4813_output.setInt32(r, 2, _s_nationkey345);
                it++;
                r++;
            }
        }
        tbl_JOIN_INNER_TD_4813_output.setNumRow(r);
    } else { 
        std::unordered_multimap<SW_JOIN_INNER_TD_4813_key_rightMajor, SW_JOIN_INNER_TD_4813_payload_rightMajor> ht1;
        int nrow1 = tbl_SerializeFromObject_TD_5371_input.getNumRow();
        for (int i = 0; i < nrow1; i++) {
            int32_t _s_suppkey342_k = tbl_SerializeFromObject_TD_5371_input.getInt32(i, 0);
            int32_t _s_nationkey345_k = tbl_SerializeFromObject_TD_5371_input.getInt32(i, 1);
            SW_JOIN_INNER_TD_4813_key_rightMajor keyA{_s_suppkey342_k, _s_nationkey345_k};
            int32_t _s_suppkey342 = tbl_SerializeFromObject_TD_5371_input.getInt32(i, 0);
            int32_t _s_nationkey345 = tbl_SerializeFromObject_TD_5371_input.getInt32(i, 1);
            SW_JOIN_INNER_TD_4813_payload_rightMajor payloadA{_s_suppkey342, _s_nationkey345};
            ht1.insert(std::make_pair(keyA, payloadA));
        }
        int r = 0;
        int nrow2 = tbl_JOIN_INNER_TD_5328_output.getNumRow();
        for (int i = 0; i < nrow2; i++) {
            int32_t _l_suppkey79_k = tbl_JOIN_INNER_TD_5328_output.getInt32(i, 1);
            int32_t _c_nationkey25_k = tbl_JOIN_INNER_TD_5328_output.getInt32(i, 0);
            auto its = ht1.equal_range(SW_JOIN_INNER_TD_4813_key_rightMajor{_l_suppkey79_k, _c_nationkey25_k});
            auto it = its.first;
            while (it != its.second) {
                int32_t _s_suppkey342 = (it->second)._s_suppkey342;
                int32_t _s_nationkey345 = (it->second)._s_nationkey345;
                int32_t _c_nationkey25 = tbl_JOIN_INNER_TD_5328_output.getInt32(i, 0);
                int32_t _l_suppkey79 = tbl_JOIN_INNER_TD_5328_output.getInt32(i, 1);
                int32_t _l_extendedprice82 = tbl_JOIN_INNER_TD_5328_output.getInt32(i, 2);
                int32_t _l_discount83 = tbl_JOIN_INNER_TD_5328_output.getInt32(i, 3);
                tbl_JOIN_INNER_TD_4813_output.setInt32(r, 2, _s_nationkey345);
                tbl_JOIN_INNER_TD_4813_output.setInt32(r, 0, _l_extendedprice82);
                tbl_JOIN_INNER_TD_4813_output.setInt32(r, 1, _l_discount83);
                it++;
                r++;
            }
        }
        tbl_JOIN_INNER_TD_4813_output.setNumRow(r);
    } 
    std::cout << "tbl_JOIN_INNER_TD_4813_output #Row: " << tbl_JOIN_INNER_TD_4813_output.getNumRow() << std::endl;
}

void SW_Filter_TD_3312(Table &tbl_SerializeFromObject_TD_414_input, Table &tbl_Filter_TD_3312_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: Filter
    // Operation: ListBuffer((isnotnull(r_name#175) AND (r_name#175 = AFRICA)))
    // Input: ListBuffer(r_regionkey#174, r_name#175)
    // Output: ListBuffer(r_regionkey#174)
    int r = 0;
    int nrow1 = tbl_SerializeFromObject_TD_414_input.getNumRow();
    for (int i = 0; i < nrow1; i++) {
        std::array<char, TPCH_READ_REGION_LEN + 1> _r_name175 = tbl_SerializeFromObject_TD_414_input.getcharN<char, TPCH_READ_REGION_LEN + 1>(i, 1);
        if ((1) && (std::string(_r_name175.data()) == "AFRICA")) {
            int32_t _r_regionkey174_t = tbl_SerializeFromObject_TD_414_input.getInt32(i, 0);
            tbl_Filter_TD_3312_output.setInt32(r, 0, _r_regionkey174_t);
            r++;
        }
    }
    tbl_Filter_TD_3312_output.setNumRow(r);
    std::cout << "tbl_Filter_TD_3312_output #Row: " << tbl_Filter_TD_3312_output.getNumRow() << std::endl;
}

struct SW_JOIN_INNER_TD_3489_key_leftMajor {
    int32_t _s_nationkey345;
    bool operator==(const SW_JOIN_INNER_TD_3489_key_leftMajor& other) const {
        return ((_s_nationkey345 == other._s_nationkey345));
    }
};
namespace std {
template <>
struct hash<SW_JOIN_INNER_TD_3489_key_leftMajor> {
    std::size_t operator() (const SW_JOIN_INNER_TD_3489_key_leftMajor& k) const {
        using std::size_t;
        using std::hash;
        using std::string;
        return (hash<int32_t>()(k._s_nationkey345));
    }
};
}
struct SW_JOIN_INNER_TD_3489_payload_leftMajor {
    int32_t _l_extendedprice82;
    int32_t _l_discount83;
    int32_t _s_nationkey345;
};
struct SW_JOIN_INNER_TD_3489_key_rightMajor {
    int32_t _n_nationkey144;
    bool operator==(const SW_JOIN_INNER_TD_3489_key_rightMajor& other) const {
        return ((_n_nationkey144 == other._n_nationkey144));
    }
};
namespace std {
template <>
struct hash<SW_JOIN_INNER_TD_3489_key_rightMajor> {
    std::size_t operator() (const SW_JOIN_INNER_TD_3489_key_rightMajor& k) const {
        using std::size_t;
        using std::hash;
        using std::string;
        return (hash<int32_t>()(k._n_nationkey144));
    }
};
}
struct SW_JOIN_INNER_TD_3489_payload_rightMajor {
    int32_t _n_nationkey144;
    std::string _n_name145;
    int32_t _n_regionkey146;
};
void SW_JOIN_INNER_TD_3489(Table &tbl_JOIN_INNER_TD_4813_output, Table &tbl_SerializeFromObject_TD_4697_input, Table &tbl_JOIN_INNER_TD_3489_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: JOIN_INNER
    // Operation: ListBuffer((s_nationkey#345 = n_nationkey#144))
    // Left Table: ListBuffer(l_extendedprice#82, l_discount#83, s_nationkey#345)
    // Right Table: ListBuffer(n_nationkey#144, n_name#145, n_regionkey#146)
    // Output Table: ListBuffer(l_extendedprice#82, l_discount#83, n_name#145, n_regionkey#146)
    int left_nrow = tbl_JOIN_INNER_TD_4813_output.getNumRow();
    int right_nrow = tbl_SerializeFromObject_TD_4697_input.getNumRow();
    if (left_nrow < right_nrow) { 
        std::unordered_multimap<SW_JOIN_INNER_TD_3489_key_leftMajor, SW_JOIN_INNER_TD_3489_payload_leftMajor> ht1;
        int nrow1 = tbl_JOIN_INNER_TD_4813_output.getNumRow();
        for (int i = 0; i < nrow1; i++) {
            int32_t _s_nationkey345_k = tbl_JOIN_INNER_TD_4813_output.getInt32(i, 2);
            SW_JOIN_INNER_TD_3489_key_leftMajor keyA{_s_nationkey345_k};
            int32_t _l_extendedprice82 = tbl_JOIN_INNER_TD_4813_output.getInt32(i, 0);
            int32_t _l_discount83 = tbl_JOIN_INNER_TD_4813_output.getInt32(i, 1);
            int32_t _s_nationkey345 = tbl_JOIN_INNER_TD_4813_output.getInt32(i, 2);
            SW_JOIN_INNER_TD_3489_payload_leftMajor payloadA{_l_extendedprice82, _l_discount83, _s_nationkey345};
            ht1.insert(std::make_pair(keyA, payloadA));
        }
        int r = 0;
        int nrow2 = tbl_SerializeFromObject_TD_4697_input.getNumRow();
        for (int i = 0; i < nrow2; i++) {
            int32_t _n_nationkey144_k = tbl_SerializeFromObject_TD_4697_input.getInt32(i, 0);
            auto its = ht1.equal_range(SW_JOIN_INNER_TD_3489_key_leftMajor{_n_nationkey144_k});
            auto it = its.first;
            while (it != its.second) {
                int32_t _l_extendedprice82 = (it->second)._l_extendedprice82;
                int32_t _l_discount83 = (it->second)._l_discount83;
                int32_t _s_nationkey345 = (it->second)._s_nationkey345;
                int32_t _n_nationkey144 = tbl_SerializeFromObject_TD_4697_input.getInt32(i, 0);
                std::array<char, TPCH_READ_NATION_LEN + 1> _n_name145_n = tbl_SerializeFromObject_TD_4697_input.getcharN<char, TPCH_READ_NATION_LEN + 1>(i, 1);
                std::string _n_name145 = std::string(_n_name145_n.data());
                int32_t _n_regionkey146 = tbl_SerializeFromObject_TD_4697_input.getInt32(i, 2);
                tbl_JOIN_INNER_TD_3489_output.setInt32(r, 0, _l_extendedprice82);
                tbl_JOIN_INNER_TD_3489_output.setInt32(r, 1, _l_discount83);
                tbl_JOIN_INNER_TD_3489_output.setcharN<char, TPCH_READ_NATION_LEN + 1>(r, 2, _n_name145_n);
                tbl_JOIN_INNER_TD_3489_output.setInt32(r, 3, _n_regionkey146);
                it++;
                r++;
            }
        }
        tbl_JOIN_INNER_TD_3489_output.setNumRow(r);
    } else { 
        std::unordered_multimap<SW_JOIN_INNER_TD_3489_key_rightMajor, SW_JOIN_INNER_TD_3489_payload_rightMajor> ht1;
        int nrow1 = tbl_SerializeFromObject_TD_4697_input.getNumRow();
        for (int i = 0; i < nrow1; i++) {
            int32_t _n_nationkey144_k = tbl_SerializeFromObject_TD_4697_input.getInt32(i, 0);
            SW_JOIN_INNER_TD_3489_key_rightMajor keyA{_n_nationkey144_k};
            int32_t _n_nationkey144 = tbl_SerializeFromObject_TD_4697_input.getInt32(i, 0);
            std::array<char, TPCH_READ_NATION_LEN + 1> _n_name145_n = tbl_SerializeFromObject_TD_4697_input.getcharN<char, TPCH_READ_NATION_LEN + 1>(i, 1);
            std::string _n_name145 = std::string(_n_name145_n.data());
            int32_t _n_regionkey146 = tbl_SerializeFromObject_TD_4697_input.getInt32(i, 2);
            SW_JOIN_INNER_TD_3489_payload_rightMajor payloadA{_n_nationkey144, _n_name145, _n_regionkey146};
            ht1.insert(std::make_pair(keyA, payloadA));
        }
        int r = 0;
        int nrow2 = tbl_JOIN_INNER_TD_4813_output.getNumRow();
        for (int i = 0; i < nrow2; i++) {
            int32_t _s_nationkey345_k = tbl_JOIN_INNER_TD_4813_output.getInt32(i, 2);
            auto its = ht1.equal_range(SW_JOIN_INNER_TD_3489_key_rightMajor{_s_nationkey345_k});
            auto it = its.first;
            while (it != its.second) {
                int32_t _n_nationkey144 = (it->second)._n_nationkey144;
                std::string _n_name145 = (it->second)._n_name145;
                std::array<char, TPCH_READ_NATION_LEN + 1> _n_name145_n{};
                memcpy(_n_name145_n.data(), (_n_name145).data(), (_n_name145).length());
                int32_t _n_regionkey146 = (it->second)._n_regionkey146;
                int32_t _l_extendedprice82 = tbl_JOIN_INNER_TD_4813_output.getInt32(i, 0);
                int32_t _l_discount83 = tbl_JOIN_INNER_TD_4813_output.getInt32(i, 1);
                int32_t _s_nationkey345 = tbl_JOIN_INNER_TD_4813_output.getInt32(i, 2);
                tbl_JOIN_INNER_TD_3489_output.setcharN<char, TPCH_READ_NATION_LEN + 1>(r, 2, _n_name145_n);
                tbl_JOIN_INNER_TD_3489_output.setInt32(r, 3, _n_regionkey146);
                tbl_JOIN_INNER_TD_3489_output.setInt32(r, 0, _l_extendedprice82);
                tbl_JOIN_INNER_TD_3489_output.setInt32(r, 1, _l_discount83);
                it++;
                r++;
            }
        }
        tbl_JOIN_INNER_TD_3489_output.setNumRow(r);
    } 
    std::cout << "tbl_JOIN_INNER_TD_3489_output #Row: " << tbl_JOIN_INNER_TD_3489_output.getNumRow() << std::endl;
}

struct SW_JOIN_INNER_TD_2878_key_leftMajor {
    int32_t _n_regionkey146;
    bool operator==(const SW_JOIN_INNER_TD_2878_key_leftMajor& other) const {
        return ((_n_regionkey146 == other._n_regionkey146));
    }
};
namespace std {
template <>
struct hash<SW_JOIN_INNER_TD_2878_key_leftMajor> {
    std::size_t operator() (const SW_JOIN_INNER_TD_2878_key_leftMajor& k) const {
        using std::size_t;
        using std::hash;
        using std::string;
        return (hash<int32_t>()(k._n_regionkey146));
    }
};
}
struct SW_JOIN_INNER_TD_2878_payload_leftMajor {
    int32_t _l_extendedprice82;
    int32_t _l_discount83;
    std::string _n_name145;
    int32_t _n_regionkey146;
};
struct SW_JOIN_INNER_TD_2878_key_rightMajor {
    int32_t _r_regionkey174;
    bool operator==(const SW_JOIN_INNER_TD_2878_key_rightMajor& other) const {
        return ((_r_regionkey174 == other._r_regionkey174));
    }
};
namespace std {
template <>
struct hash<SW_JOIN_INNER_TD_2878_key_rightMajor> {
    std::size_t operator() (const SW_JOIN_INNER_TD_2878_key_rightMajor& k) const {
        using std::size_t;
        using std::hash;
        using std::string;
        return (hash<int32_t>()(k._r_regionkey174));
    }
};
}
struct SW_JOIN_INNER_TD_2878_payload_rightMajor {
    int32_t _r_regionkey174;
};
void SW_JOIN_INNER_TD_2878(Table &tbl_JOIN_INNER_TD_3489_output, Table &tbl_Filter_TD_3312_output, Table &tbl_JOIN_INNER_TD_2878_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: JOIN_INNER
    // Operation: ListBuffer((n_regionkey#146 = r_regionkey#174))
    // Left Table: ListBuffer(l_extendedprice#82, l_discount#83, n_name#145, n_regionkey#146)
    // Right Table: ListBuffer(r_regionkey#174)
    // Output Table: ListBuffer(l_extendedprice#82, l_discount#83, n_name#145)
    int left_nrow = tbl_JOIN_INNER_TD_3489_output.getNumRow();
    int right_nrow = tbl_Filter_TD_3312_output.getNumRow();
    if (left_nrow < right_nrow) { 
        std::unordered_multimap<SW_JOIN_INNER_TD_2878_key_leftMajor, SW_JOIN_INNER_TD_2878_payload_leftMajor> ht1;
        int nrow1 = tbl_JOIN_INNER_TD_3489_output.getNumRow();
        for (int i = 0; i < nrow1; i++) {
            int32_t _n_regionkey146_k = tbl_JOIN_INNER_TD_3489_output.getInt32(i, 3);
            SW_JOIN_INNER_TD_2878_key_leftMajor keyA{_n_regionkey146_k};
            int32_t _l_extendedprice82 = tbl_JOIN_INNER_TD_3489_output.getInt32(i, 0);
            int32_t _l_discount83 = tbl_JOIN_INNER_TD_3489_output.getInt32(i, 1);
            std::array<char, TPCH_READ_NATION_LEN + 1> _n_name145_n = tbl_JOIN_INNER_TD_3489_output.getcharN<char, TPCH_READ_NATION_LEN + 1>(i, 2);
            std::string _n_name145 = std::string(_n_name145_n.data());
            int32_t _n_regionkey146 = tbl_JOIN_INNER_TD_3489_output.getInt32(i, 3);
            SW_JOIN_INNER_TD_2878_payload_leftMajor payloadA{_l_extendedprice82, _l_discount83, _n_name145, _n_regionkey146};
            ht1.insert(std::make_pair(keyA, payloadA));
        }
        int r = 0;
        int nrow2 = tbl_Filter_TD_3312_output.getNumRow();
        for (int i = 0; i < nrow2; i++) {
            int32_t _r_regionkey174_k = tbl_Filter_TD_3312_output.getInt32(i, 0);
            auto its = ht1.equal_range(SW_JOIN_INNER_TD_2878_key_leftMajor{_r_regionkey174_k});
            auto it = its.first;
            while (it != its.second) {
                int32_t _l_extendedprice82 = (it->second)._l_extendedprice82;
                int32_t _l_discount83 = (it->second)._l_discount83;
                std::string _n_name145 = (it->second)._n_name145;
                std::array<char, TPCH_READ_NATION_LEN + 1> _n_name145_n{};
                memcpy(_n_name145_n.data(), (_n_name145).data(), (_n_name145).length());
                int32_t _n_regionkey146 = (it->second)._n_regionkey146;
                int32_t _r_regionkey174 = tbl_Filter_TD_3312_output.getInt32(i, 0);
                tbl_JOIN_INNER_TD_2878_output.setInt32(r, 0, _l_extendedprice82);
                tbl_JOIN_INNER_TD_2878_output.setInt32(r, 1, _l_discount83);
                tbl_JOIN_INNER_TD_2878_output.setcharN<char, TPCH_READ_NATION_LEN + 1>(r, 2, _n_name145_n);
                it++;
                r++;
            }
        }
        tbl_JOIN_INNER_TD_2878_output.setNumRow(r);
    } else { 
        std::unordered_multimap<SW_JOIN_INNER_TD_2878_key_rightMajor, SW_JOIN_INNER_TD_2878_payload_rightMajor> ht1;
        int nrow1 = tbl_Filter_TD_3312_output.getNumRow();
        for (int i = 0; i < nrow1; i++) {
            int32_t _r_regionkey174_k = tbl_Filter_TD_3312_output.getInt32(i, 0);
            SW_JOIN_INNER_TD_2878_key_rightMajor keyA{_r_regionkey174_k};
            int32_t _r_regionkey174 = tbl_Filter_TD_3312_output.getInt32(i, 0);
            SW_JOIN_INNER_TD_2878_payload_rightMajor payloadA{_r_regionkey174};
            ht1.insert(std::make_pair(keyA, payloadA));
        }
        int r = 0;
        int nrow2 = tbl_JOIN_INNER_TD_3489_output.getNumRow();
        for (int i = 0; i < nrow2; i++) {
            int32_t _n_regionkey146_k = tbl_JOIN_INNER_TD_3489_output.getInt32(i, 3);
            auto its = ht1.equal_range(SW_JOIN_INNER_TD_2878_key_rightMajor{_n_regionkey146_k});
            auto it = its.first;
            while (it != its.second) {
                int32_t _r_regionkey174 = (it->second)._r_regionkey174;
                int32_t _l_extendedprice82 = tbl_JOIN_INNER_TD_3489_output.getInt32(i, 0);
                int32_t _l_discount83 = tbl_JOIN_INNER_TD_3489_output.getInt32(i, 1);
                std::array<char, TPCH_READ_NATION_LEN + 1> _n_name145_n = tbl_JOIN_INNER_TD_3489_output.getcharN<char, TPCH_READ_NATION_LEN + 1>(i, 2);
                std::string _n_name145 = std::string(_n_name145_n.data());
                int32_t _n_regionkey146 = tbl_JOIN_INNER_TD_3489_output.getInt32(i, 3);
                tbl_JOIN_INNER_TD_2878_output.setInt32(r, 0, _l_extendedprice82);
                tbl_JOIN_INNER_TD_2878_output.setInt32(r, 1, _l_discount83);
                tbl_JOIN_INNER_TD_2878_output.setcharN<char, TPCH_READ_NATION_LEN + 1>(r, 2, _n_name145_n);
                it++;
                r++;
            }
        }
        tbl_JOIN_INNER_TD_2878_output.setNumRow(r);
    } 
    std::cout << "tbl_JOIN_INNER_TD_2878_output #Row: " << tbl_JOIN_INNER_TD_2878_output.getNumRow() << std::endl;
}

typedef std::string SW_Aggregate_TD_1568_key;
struct SW_Aggregate_TD_1568_payload {
    int64_t _revenue774L_sum_0;
};
void SW_Aggregate_TD_1568(Table &tbl_JOIN_INNER_TD_2878_output, Table &tbl_Aggregate_TD_1568_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: Aggregate
    // Operation: ListBuffer(n_name#145, sum((l_extendedprice#82 * (100 - l_discount#83))) AS revenue#774L)
    // Input: ListBuffer(l_extendedprice#82, l_discount#83, n_name#145)
    // Output: ListBuffer(n_name#145, revenue#774L)
    std::unordered_map<SW_Aggregate_TD_1568_key, SW_Aggregate_TD_1568_payload> ht1;
    int nrow1 = tbl_JOIN_INNER_TD_2878_output.getNumRow();
    for (int i = 0; i < nrow1; i++) {
        int32_t _l_extendedprice82 = tbl_JOIN_INNER_TD_2878_output.getInt32(i, 0);
        int32_t _l_discount83 = tbl_JOIN_INNER_TD_2878_output.getInt32(i, 1);
        std::array<char, TPCH_READ_NATION_LEN + 1> _n_name145 = tbl_JOIN_INNER_TD_2878_output.getcharN<char, TPCH_READ_NATION_LEN + 1>(i, 2);
        SW_Aggregate_TD_1568_key k = std::string(_n_name145.data());
        int64_t _revenue774L_sum_0 = (_l_extendedprice82 * (100 - _l_discount83));
        SW_Aggregate_TD_1568_payload p{_revenue774L_sum_0};
        auto it = ht1.find(k);
        if (it != ht1.end()) {
            int64_t sum_0 = (it->second)._revenue774L_sum_0 + _revenue774L_sum_0;
            p._revenue774L_sum_0 = sum_0;
            ht1[k] = p;
        } else { 
            ht1.insert(std::make_pair(k, p));
        }
    }
    int r = 0;
    for (auto& it : ht1) {
        std::array<char, TPCH_READ_NATION_LEN + 1> _n_name145{};
        memcpy(_n_name145.data(), (it.first).data(), (it.first).length());
        tbl_Aggregate_TD_1568_output.setcharN<char, TPCH_READ_NATION_LEN + 1>(r, 0, _n_name145);
        int64_t _revenue774L = (it.second)._revenue774L_sum_0;
        tbl_Aggregate_TD_1568_output.setInt64(r, 1, _revenue774L);
        ++r;
    }
    tbl_Aggregate_TD_1568_output.setNumRow(r);
    std::cout << "tbl_Aggregate_TD_1568_output #Row: " << tbl_Aggregate_TD_1568_output.getNumRow() << std::endl;
}

void SW_Sort_TD_0259(Table &tbl_Aggregate_TD_1568_output, Table &tbl_Sort_TD_0259_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: Sort
    // Operation: ListBuffer(revenue#774L DESC NULLS LAST)
    // Input: ListBuffer(n_name#145, revenue#774L)
    // Output: ListBuffer(n_name#145, revenue#774L)
    struct SW_Sort_TD_0259Row {
        std::string _n_name145;
        int64_t _revenue774L;
    }; 

    struct {
        bool operator()(const SW_Sort_TD_0259Row& a, const SW_Sort_TD_0259Row& b) const { return 
 (a._revenue774L > b._revenue774L); 
}
    }SW_Sort_TD_0259_order; 

    int nrow1 = tbl_Aggregate_TD_1568_output.getNumRow();
    std::vector<SW_Sort_TD_0259Row> rows;
    for (int i = 0; i < nrow1; i++) {
        std::array<char, TPCH_READ_NATION_LEN + 1> _n_name145 = tbl_Aggregate_TD_1568_output.getcharN<char, TPCH_READ_NATION_LEN +1>(i, 0);
        int64_t _revenue774L = tbl_Aggregate_TD_1568_output.getInt64(i, 1);
        SW_Sort_TD_0259Row t = {std::string(_n_name145.data()),_revenue774L};
        rows.push_back(t);
    }
    std::sort(rows.begin(), rows.end(), SW_Sort_TD_0259_order);
    int r = 0;
    for (auto& it : rows) {
        std::array<char, TPCH_READ_NATION_LEN + 1> _n_name145{};
        memcpy(_n_name145.data(), (it._n_name145).data(), (it._n_name145).length());
        tbl_Sort_TD_0259_output.setcharN<char, TPCH_READ_NATION_LEN +1>(r, 0, _n_name145);
        tbl_Sort_TD_0259_output.setInt64(r, 1, it._revenue774L);
        if (r < 10) {
            std::cout << (it._n_name145).data() << " " << it._revenue774L << " " << std::endl;
        }
        ++r;
    }
    tbl_Sort_TD_0259_output.setNumRow(r);
    std::cout << "tbl_Sort_TD_0259_output #Row: " << tbl_Sort_TD_0259_output.getNumRow() << std::endl;
}

