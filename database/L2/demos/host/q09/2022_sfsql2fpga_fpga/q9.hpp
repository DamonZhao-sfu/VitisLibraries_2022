#include <regex> 
#include <stdint.h> 

void SW_Filter_TD_8642(Table &tbl_SerializeFromObject_TD_9282_input, Table &tbl_Filter_TD_8642_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: Filter
    // Operation: ListBuffer((isnotnull(p_name#259) AND Contains(p_name#259, magenta)))
    // Input: ListBuffer(p_partkey#258, p_name#259)
    // Output: ListBuffer(p_partkey#258)
    int r = 0;
    int nrow1 = tbl_SerializeFromObject_TD_9282_input.getNumRow();
    for (int i = 0; i < nrow1; i++) {
        std::array<char, TPCH_READ_P_NAME_LEN + 1> _p_name259 = tbl_SerializeFromObject_TD_9282_input.getcharN<char, TPCH_READ_P_NAME_LEN + 1>(i, 1);
        if ((1) && (std::string(_p_name259.data()).find("magenta")!=std::string::npos)) {
            int32_t _p_partkey258_t = tbl_SerializeFromObject_TD_9282_input.getInt32(i, 0);
            tbl_Filter_TD_8642_output.setInt32(r, 0, _p_partkey258_t);
            r++;
        }
    }
    tbl_Filter_TD_8642_output.setNumRow(r);
    std::cout << "tbl_Filter_TD_8642_output #Row: " << tbl_Filter_TD_8642_output.getNumRow() << std::endl;
}

struct SW_JOIN_INNER_TD_7429_key_leftMajor {
    int32_t _p_partkey258;
    bool operator==(const SW_JOIN_INNER_TD_7429_key_leftMajor& other) const {
        return ((_p_partkey258 == other._p_partkey258));
    }
};
namespace std {
template <>
struct hash<SW_JOIN_INNER_TD_7429_key_leftMajor> {
    std::size_t operator() (const SW_JOIN_INNER_TD_7429_key_leftMajor& k) const {
        using std::size_t;
        using std::hash;
        using std::string;
        return (hash<int32_t>()(k._p_partkey258));
    }
};
}
struct SW_JOIN_INNER_TD_7429_payload_leftMajor {
    int32_t _p_partkey258;
};
struct SW_JOIN_INNER_TD_7429_key_rightMajor {
    int32_t _l_partkey78;
    bool operator==(const SW_JOIN_INNER_TD_7429_key_rightMajor& other) const {
        return ((_l_partkey78 == other._l_partkey78));
    }
};
namespace std {
template <>
struct hash<SW_JOIN_INNER_TD_7429_key_rightMajor> {
    std::size_t operator() (const SW_JOIN_INNER_TD_7429_key_rightMajor& k) const {
        using std::size_t;
        using std::hash;
        using std::string;
        return (hash<int32_t>()(k._l_partkey78));
    }
};
}
struct SW_JOIN_INNER_TD_7429_payload_rightMajor {
    int32_t _l_orderkey77;
    int32_t _l_partkey78;
    int32_t _l_suppkey79;
    int32_t _l_quantity81;
    int32_t _l_extendedprice82;
    int32_t _l_discount83;
};
void SW_JOIN_INNER_TD_7429(Table &tbl_Filter_TD_8642_output, Table &tbl_SerializeFromObject_TD_8960_input, Table &tbl_JOIN_INNER_TD_7429_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: JOIN_INNER
    // Operation: ListBuffer((p_partkey#258 = l_partkey#78))
    // Left Table: ListBuffer(p_partkey#258)
    // Right Table: ListBuffer(l_orderkey#77, l_partkey#78, l_suppkey#79, l_quantity#81, l_extendedprice#82, l_discount#83)
    // Output Table: ListBuffer(l_orderkey#77, l_partkey#78, l_suppkey#79, l_quantity#81, l_extendedprice#82, l_discount#83)
    int left_nrow = tbl_Filter_TD_8642_output.getNumRow();
    int right_nrow = tbl_SerializeFromObject_TD_8960_input.getNumRow();
    if (left_nrow < right_nrow) { 
        std::unordered_multimap<SW_JOIN_INNER_TD_7429_key_leftMajor, SW_JOIN_INNER_TD_7429_payload_leftMajor> ht1;
        int nrow1 = tbl_Filter_TD_8642_output.getNumRow();
        for (int i = 0; i < nrow1; i++) {
            int32_t _p_partkey258_k = tbl_Filter_TD_8642_output.getInt32(i, 0);
            SW_JOIN_INNER_TD_7429_key_leftMajor keyA{_p_partkey258_k};
            int32_t _p_partkey258 = tbl_Filter_TD_8642_output.getInt32(i, 0);
            SW_JOIN_INNER_TD_7429_payload_leftMajor payloadA{_p_partkey258};
            ht1.insert(std::make_pair(keyA, payloadA));
        }
        int r = 0;
        int nrow2 = tbl_SerializeFromObject_TD_8960_input.getNumRow();
        for (int i = 0; i < nrow2; i++) {
            int32_t _l_partkey78_k = tbl_SerializeFromObject_TD_8960_input.getInt32(i, 1);
            auto its = ht1.equal_range(SW_JOIN_INNER_TD_7429_key_leftMajor{_l_partkey78_k});
            auto it = its.first;
            while (it != its.second) {
                int32_t _p_partkey258 = (it->second)._p_partkey258;
                int32_t _l_orderkey77 = tbl_SerializeFromObject_TD_8960_input.getInt32(i, 0);
                int32_t _l_partkey78 = tbl_SerializeFromObject_TD_8960_input.getInt32(i, 1);
                int32_t _l_suppkey79 = tbl_SerializeFromObject_TD_8960_input.getInt32(i, 2);
                int32_t _l_quantity81 = tbl_SerializeFromObject_TD_8960_input.getInt32(i, 3);
                int32_t _l_extendedprice82 = tbl_SerializeFromObject_TD_8960_input.getInt32(i, 4);
                int32_t _l_discount83 = tbl_SerializeFromObject_TD_8960_input.getInt32(i, 5);
                tbl_JOIN_INNER_TD_7429_output.setInt32(r, 0, _l_orderkey77);
                tbl_JOIN_INNER_TD_7429_output.setInt32(r, 1, _l_partkey78);
                tbl_JOIN_INNER_TD_7429_output.setInt32(r, 2, _l_suppkey79);
                tbl_JOIN_INNER_TD_7429_output.setInt32(r, 3, _l_quantity81);
                tbl_JOIN_INNER_TD_7429_output.setInt32(r, 4, _l_extendedprice82);
                tbl_JOIN_INNER_TD_7429_output.setInt32(r, 5, _l_discount83);
                it++;
                r++;
            }
        }
        tbl_JOIN_INNER_TD_7429_output.setNumRow(r);
    } else { 
        std::unordered_multimap<SW_JOIN_INNER_TD_7429_key_rightMajor, SW_JOIN_INNER_TD_7429_payload_rightMajor> ht1;
        int nrow1 = tbl_SerializeFromObject_TD_8960_input.getNumRow();
        for (int i = 0; i < nrow1; i++) {
            int32_t _l_partkey78_k = tbl_SerializeFromObject_TD_8960_input.getInt32(i, 1);
            SW_JOIN_INNER_TD_7429_key_rightMajor keyA{_l_partkey78_k};
            int32_t _l_orderkey77 = tbl_SerializeFromObject_TD_8960_input.getInt32(i, 0);
            int32_t _l_partkey78 = tbl_SerializeFromObject_TD_8960_input.getInt32(i, 1);
            int32_t _l_suppkey79 = tbl_SerializeFromObject_TD_8960_input.getInt32(i, 2);
            int32_t _l_quantity81 = tbl_SerializeFromObject_TD_8960_input.getInt32(i, 3);
            int32_t _l_extendedprice82 = tbl_SerializeFromObject_TD_8960_input.getInt32(i, 4);
            int32_t _l_discount83 = tbl_SerializeFromObject_TD_8960_input.getInt32(i, 5);
            SW_JOIN_INNER_TD_7429_payload_rightMajor payloadA{_l_orderkey77, _l_partkey78, _l_suppkey79, _l_quantity81, _l_extendedprice82, _l_discount83};
            ht1.insert(std::make_pair(keyA, payloadA));
        }
        int r = 0;
        int nrow2 = tbl_Filter_TD_8642_output.getNumRow();
        for (int i = 0; i < nrow2; i++) {
            int32_t _p_partkey258_k = tbl_Filter_TD_8642_output.getInt32(i, 0);
            auto its = ht1.equal_range(SW_JOIN_INNER_TD_7429_key_rightMajor{_p_partkey258_k});
            auto it = its.first;
            while (it != its.second) {
                int32_t _l_orderkey77 = (it->second)._l_orderkey77;
                int32_t _l_partkey78 = (it->second)._l_partkey78;
                int32_t _l_suppkey79 = (it->second)._l_suppkey79;
                int32_t _l_quantity81 = (it->second)._l_quantity81;
                int32_t _l_extendedprice82 = (it->second)._l_extendedprice82;
                int32_t _l_discount83 = (it->second)._l_discount83;
                int32_t _p_partkey258 = tbl_Filter_TD_8642_output.getInt32(i, 0);
                tbl_JOIN_INNER_TD_7429_output.setInt32(r, 0, _l_orderkey77);
                tbl_JOIN_INNER_TD_7429_output.setInt32(r, 1, _l_partkey78);
                tbl_JOIN_INNER_TD_7429_output.setInt32(r, 2, _l_suppkey79);
                tbl_JOIN_INNER_TD_7429_output.setInt32(r, 3, _l_quantity81);
                tbl_JOIN_INNER_TD_7429_output.setInt32(r, 4, _l_extendedprice82);
                tbl_JOIN_INNER_TD_7429_output.setInt32(r, 5, _l_discount83);
                it++;
                r++;
            }
        }
        tbl_JOIN_INNER_TD_7429_output.setNumRow(r);
    } 
    std::cout << "tbl_JOIN_INNER_TD_7429_output #Row: " << tbl_JOIN_INNER_TD_7429_output.getNumRow() << std::endl;
}

struct SW_JOIN_INNER_TD_6541_key_leftMajor {
    int32_t _l_suppkey79;
    bool operator==(const SW_JOIN_INNER_TD_6541_key_leftMajor& other) const {
        return ((_l_suppkey79 == other._l_suppkey79));
    }
};
namespace std {
template <>
struct hash<SW_JOIN_INNER_TD_6541_key_leftMajor> {
    std::size_t operator() (const SW_JOIN_INNER_TD_6541_key_leftMajor& k) const {
        using std::size_t;
        using std::hash;
        using std::string;
        return (hash<int32_t>()(k._l_suppkey79));
    }
};
}
struct SW_JOIN_INNER_TD_6541_payload_leftMajor {
    int32_t _l_orderkey77;
    int32_t _l_partkey78;
    int32_t _l_suppkey79;
    int32_t _l_quantity81;
    int32_t _l_extendedprice82;
    int32_t _l_discount83;
};
struct SW_JOIN_INNER_TD_6541_key_rightMajor {
    int32_t _s_suppkey342;
    bool operator==(const SW_JOIN_INNER_TD_6541_key_rightMajor& other) const {
        return ((_s_suppkey342 == other._s_suppkey342));
    }
};
namespace std {
template <>
struct hash<SW_JOIN_INNER_TD_6541_key_rightMajor> {
    std::size_t operator() (const SW_JOIN_INNER_TD_6541_key_rightMajor& k) const {
        using std::size_t;
        using std::hash;
        using std::string;
        return (hash<int32_t>()(k._s_suppkey342));
    }
};
}
struct SW_JOIN_INNER_TD_6541_payload_rightMajor {
    int32_t _s_suppkey342;
    int32_t _s_nationkey345;
};
void SW_JOIN_INNER_TD_6541(Table &tbl_JOIN_INNER_TD_7429_output, Table &tbl_SerializeFromObject_TD_7602_input, Table &tbl_JOIN_INNER_TD_6541_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: JOIN_INNER
    // Operation: ListBuffer((s_suppkey#342 = l_suppkey#79))
    // Left Table: ListBuffer(l_orderkey#77, l_partkey#78, l_suppkey#79, l_quantity#81, l_extendedprice#82, l_discount#83)
    // Right Table: ListBuffer(s_suppkey#342, s_nationkey#345)
    // Output Table: ListBuffer(l_orderkey#77, l_partkey#78, l_suppkey#79, l_quantity#81, l_extendedprice#82, l_discount#83, s_nationkey#345)
    int left_nrow = tbl_JOIN_INNER_TD_7429_output.getNumRow();
    int right_nrow = tbl_SerializeFromObject_TD_7602_input.getNumRow();
    if (left_nrow < right_nrow) { 
        std::unordered_multimap<SW_JOIN_INNER_TD_6541_key_leftMajor, SW_JOIN_INNER_TD_6541_payload_leftMajor> ht1;
        int nrow1 = tbl_JOIN_INNER_TD_7429_output.getNumRow();
        for (int i = 0; i < nrow1; i++) {
            int32_t _l_suppkey79_k = tbl_JOIN_INNER_TD_7429_output.getInt32(i, 2);
            SW_JOIN_INNER_TD_6541_key_leftMajor keyA{_l_suppkey79_k};
            int32_t _l_orderkey77 = tbl_JOIN_INNER_TD_7429_output.getInt32(i, 0);
            int32_t _l_partkey78 = tbl_JOIN_INNER_TD_7429_output.getInt32(i, 1);
            int32_t _l_suppkey79 = tbl_JOIN_INNER_TD_7429_output.getInt32(i, 2);
            int32_t _l_quantity81 = tbl_JOIN_INNER_TD_7429_output.getInt32(i, 3);
            int32_t _l_extendedprice82 = tbl_JOIN_INNER_TD_7429_output.getInt32(i, 4);
            int32_t _l_discount83 = tbl_JOIN_INNER_TD_7429_output.getInt32(i, 5);
            SW_JOIN_INNER_TD_6541_payload_leftMajor payloadA{_l_orderkey77, _l_partkey78, _l_suppkey79, _l_quantity81, _l_extendedprice82, _l_discount83};
            ht1.insert(std::make_pair(keyA, payloadA));
        }
        int r = 0;
        int nrow2 = tbl_SerializeFromObject_TD_7602_input.getNumRow();
        for (int i = 0; i < nrow2; i++) {
            int32_t _s_suppkey342_k = tbl_SerializeFromObject_TD_7602_input.getInt32(i, 0);
            auto its = ht1.equal_range(SW_JOIN_INNER_TD_6541_key_leftMajor{_s_suppkey342_k});
            auto it = its.first;
            while (it != its.second) {
                int32_t _l_orderkey77 = (it->second)._l_orderkey77;
                int32_t _l_partkey78 = (it->second)._l_partkey78;
                int32_t _l_suppkey79 = (it->second)._l_suppkey79;
                int32_t _l_quantity81 = (it->second)._l_quantity81;
                int32_t _l_extendedprice82 = (it->second)._l_extendedprice82;
                int32_t _l_discount83 = (it->second)._l_discount83;
                int32_t _s_suppkey342 = tbl_SerializeFromObject_TD_7602_input.getInt32(i, 0);
                int32_t _s_nationkey345 = tbl_SerializeFromObject_TD_7602_input.getInt32(i, 1);
                tbl_JOIN_INNER_TD_6541_output.setInt32(r, 0, _l_orderkey77);
                tbl_JOIN_INNER_TD_6541_output.setInt32(r, 1, _l_partkey78);
                tbl_JOIN_INNER_TD_6541_output.setInt32(r, 2, _l_suppkey79);
                tbl_JOIN_INNER_TD_6541_output.setInt32(r, 3, _l_quantity81);
                tbl_JOIN_INNER_TD_6541_output.setInt32(r, 4, _l_extendedprice82);
                tbl_JOIN_INNER_TD_6541_output.setInt32(r, 5, _l_discount83);
                tbl_JOIN_INNER_TD_6541_output.setInt32(r, 6, _s_nationkey345);
                it++;
                r++;
            }
        }
        tbl_JOIN_INNER_TD_6541_output.setNumRow(r);
    } else { 
        std::unordered_multimap<SW_JOIN_INNER_TD_6541_key_rightMajor, SW_JOIN_INNER_TD_6541_payload_rightMajor> ht1;
        int nrow1 = tbl_SerializeFromObject_TD_7602_input.getNumRow();
        for (int i = 0; i < nrow1; i++) {
            int32_t _s_suppkey342_k = tbl_SerializeFromObject_TD_7602_input.getInt32(i, 0);
            SW_JOIN_INNER_TD_6541_key_rightMajor keyA{_s_suppkey342_k};
            int32_t _s_suppkey342 = tbl_SerializeFromObject_TD_7602_input.getInt32(i, 0);
            int32_t _s_nationkey345 = tbl_SerializeFromObject_TD_7602_input.getInt32(i, 1);
            SW_JOIN_INNER_TD_6541_payload_rightMajor payloadA{_s_suppkey342, _s_nationkey345};
            ht1.insert(std::make_pair(keyA, payloadA));
        }
        int r = 0;
        int nrow2 = tbl_JOIN_INNER_TD_7429_output.getNumRow();
        for (int i = 0; i < nrow2; i++) {
            int32_t _l_suppkey79_k = tbl_JOIN_INNER_TD_7429_output.getInt32(i, 2);
            auto its = ht1.equal_range(SW_JOIN_INNER_TD_6541_key_rightMajor{_l_suppkey79_k});
            auto it = its.first;
            while (it != its.second) {
                int32_t _s_suppkey342 = (it->second)._s_suppkey342;
                int32_t _s_nationkey345 = (it->second)._s_nationkey345;
                int32_t _l_orderkey77 = tbl_JOIN_INNER_TD_7429_output.getInt32(i, 0);
                int32_t _l_partkey78 = tbl_JOIN_INNER_TD_7429_output.getInt32(i, 1);
                int32_t _l_suppkey79 = tbl_JOIN_INNER_TD_7429_output.getInt32(i, 2);
                int32_t _l_quantity81 = tbl_JOIN_INNER_TD_7429_output.getInt32(i, 3);
                int32_t _l_extendedprice82 = tbl_JOIN_INNER_TD_7429_output.getInt32(i, 4);
                int32_t _l_discount83 = tbl_JOIN_INNER_TD_7429_output.getInt32(i, 5);
                tbl_JOIN_INNER_TD_6541_output.setInt32(r, 6, _s_nationkey345);
                tbl_JOIN_INNER_TD_6541_output.setInt32(r, 0, _l_orderkey77);
                tbl_JOIN_INNER_TD_6541_output.setInt32(r, 1, _l_partkey78);
                tbl_JOIN_INNER_TD_6541_output.setInt32(r, 2, _l_suppkey79);
                tbl_JOIN_INNER_TD_6541_output.setInt32(r, 3, _l_quantity81);
                tbl_JOIN_INNER_TD_6541_output.setInt32(r, 4, _l_extendedprice82);
                tbl_JOIN_INNER_TD_6541_output.setInt32(r, 5, _l_discount83);
                it++;
                r++;
            }
        }
        tbl_JOIN_INNER_TD_6541_output.setNumRow(r);
    } 
    std::cout << "tbl_JOIN_INNER_TD_6541_output #Row: " << tbl_JOIN_INNER_TD_6541_output.getNumRow() << std::endl;
}

struct SW_JOIN_INNER_TD_5847_key_leftMajor {
    int32_t _l_suppkey79;
    int32_t _l_partkey78;
    bool operator==(const SW_JOIN_INNER_TD_5847_key_leftMajor& other) const {
        return ((_l_suppkey79 == other._l_suppkey79) && (_l_partkey78 == other._l_partkey78));
    }
};
namespace std {
template <>
struct hash<SW_JOIN_INNER_TD_5847_key_leftMajor> {
    std::size_t operator() (const SW_JOIN_INNER_TD_5847_key_leftMajor& k) const {
        using std::size_t;
        using std::hash;
        using std::string;
        return (hash<int32_t>()(k._l_suppkey79)) + (hash<int32_t>()(k._l_partkey78));
    }
};
}
struct SW_JOIN_INNER_TD_5847_payload_leftMajor {
    int32_t _l_orderkey77;
    int32_t _l_partkey78;
    int32_t _l_suppkey79;
    int32_t _l_quantity81;
    int32_t _l_extendedprice82;
    int32_t _l_discount83;
    int32_t _s_nationkey345;
};
struct SW_JOIN_INNER_TD_5847_key_rightMajor {
    int32_t _ps_suppkey306;
    int32_t _ps_partkey305;
    bool operator==(const SW_JOIN_INNER_TD_5847_key_rightMajor& other) const {
        return ((_ps_suppkey306 == other._ps_suppkey306) && (_ps_partkey305 == other._ps_partkey305));
    }
};
namespace std {
template <>
struct hash<SW_JOIN_INNER_TD_5847_key_rightMajor> {
    std::size_t operator() (const SW_JOIN_INNER_TD_5847_key_rightMajor& k) const {
        using std::size_t;
        using std::hash;
        using std::string;
        return (hash<int32_t>()(k._ps_suppkey306)) + (hash<int32_t>()(k._ps_partkey305));
    }
};
}
struct SW_JOIN_INNER_TD_5847_payload_rightMajor {
    int32_t _ps_partkey305;
    int32_t _ps_suppkey306;
    int32_t _ps_supplycost308;
};
void SW_JOIN_INNER_TD_5847(Table &tbl_JOIN_INNER_TD_6541_output, Table &tbl_SerializeFromObject_TD_6558_input, Table &tbl_JOIN_INNER_TD_5847_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: JOIN_INNER
    // Operation: ListBuffer(((ps_suppkey#306 = l_suppkey#79) AND (ps_partkey#305 = l_partkey#78)))
    // Left Table: ListBuffer(l_orderkey#77, l_partkey#78, l_suppkey#79, l_quantity#81, l_extendedprice#82, l_discount#83, s_nationkey#345)
    // Right Table: ListBuffer(ps_partkey#305, ps_suppkey#306, ps_supplycost#308)
    // Output Table: ListBuffer(l_orderkey#77, l_quantity#81, l_extendedprice#82, l_discount#83, s_nationkey#345, ps_supplycost#308)
    int left_nrow = tbl_JOIN_INNER_TD_6541_output.getNumRow();
    int right_nrow = tbl_SerializeFromObject_TD_6558_input.getNumRow();
    if (left_nrow < right_nrow) { 
        std::unordered_multimap<SW_JOIN_INNER_TD_5847_key_leftMajor, SW_JOIN_INNER_TD_5847_payload_leftMajor> ht1;
        int nrow1 = tbl_JOIN_INNER_TD_6541_output.getNumRow();
        for (int i = 0; i < nrow1; i++) {
            int32_t _l_suppkey79_k = tbl_JOIN_INNER_TD_6541_output.getInt32(i, 2);
            int32_t _l_partkey78_k = tbl_JOIN_INNER_TD_6541_output.getInt32(i, 1);
            SW_JOIN_INNER_TD_5847_key_leftMajor keyA{_l_suppkey79_k, _l_partkey78_k};
            int32_t _l_orderkey77 = tbl_JOIN_INNER_TD_6541_output.getInt32(i, 0);
            int32_t _l_partkey78 = tbl_JOIN_INNER_TD_6541_output.getInt32(i, 1);
            int32_t _l_suppkey79 = tbl_JOIN_INNER_TD_6541_output.getInt32(i, 2);
            int32_t _l_quantity81 = tbl_JOIN_INNER_TD_6541_output.getInt32(i, 3);
            int32_t _l_extendedprice82 = tbl_JOIN_INNER_TD_6541_output.getInt32(i, 4);
            int32_t _l_discount83 = tbl_JOIN_INNER_TD_6541_output.getInt32(i, 5);
            int32_t _s_nationkey345 = tbl_JOIN_INNER_TD_6541_output.getInt32(i, 6);
            SW_JOIN_INNER_TD_5847_payload_leftMajor payloadA{_l_orderkey77, _l_partkey78, _l_suppkey79, _l_quantity81, _l_extendedprice82, _l_discount83, _s_nationkey345};
            ht1.insert(std::make_pair(keyA, payloadA));
        }
        int r = 0;
        int nrow2 = tbl_SerializeFromObject_TD_6558_input.getNumRow();
        for (int i = 0; i < nrow2; i++) {
            int32_t _ps_suppkey306_k = tbl_SerializeFromObject_TD_6558_input.getInt32(i, 1);
            int32_t _ps_partkey305_k = tbl_SerializeFromObject_TD_6558_input.getInt32(i, 0);
            auto its = ht1.equal_range(SW_JOIN_INNER_TD_5847_key_leftMajor{_ps_suppkey306_k, _ps_partkey305_k});
            auto it = its.first;
            while (it != its.second) {
                int32_t _l_orderkey77 = (it->second)._l_orderkey77;
                int32_t _l_partkey78 = (it->second)._l_partkey78;
                int32_t _l_suppkey79 = (it->second)._l_suppkey79;
                int32_t _l_quantity81 = (it->second)._l_quantity81;
                int32_t _l_extendedprice82 = (it->second)._l_extendedprice82;
                int32_t _l_discount83 = (it->second)._l_discount83;
                int32_t _s_nationkey345 = (it->second)._s_nationkey345;
                int32_t _ps_partkey305 = tbl_SerializeFromObject_TD_6558_input.getInt32(i, 0);
                int32_t _ps_suppkey306 = tbl_SerializeFromObject_TD_6558_input.getInt32(i, 1);
                int32_t _ps_supplycost308 = tbl_SerializeFromObject_TD_6558_input.getInt32(i, 2);
                tbl_JOIN_INNER_TD_5847_output.setInt32(r, 0, _l_orderkey77);
                tbl_JOIN_INNER_TD_5847_output.setInt32(r, 1, _l_quantity81);
                tbl_JOIN_INNER_TD_5847_output.setInt32(r, 2, _l_extendedprice82);
                tbl_JOIN_INNER_TD_5847_output.setInt32(r, 3, _l_discount83);
                tbl_JOIN_INNER_TD_5847_output.setInt32(r, 4, _s_nationkey345);
                tbl_JOIN_INNER_TD_5847_output.setInt32(r, 5, _ps_supplycost308);
                it++;
                r++;
            }
        }
        tbl_JOIN_INNER_TD_5847_output.setNumRow(r);
    } else { 
        std::unordered_multimap<SW_JOIN_INNER_TD_5847_key_rightMajor, SW_JOIN_INNER_TD_5847_payload_rightMajor> ht1;
        int nrow1 = tbl_SerializeFromObject_TD_6558_input.getNumRow();
        for (int i = 0; i < nrow1; i++) {
            int32_t _ps_suppkey306_k = tbl_SerializeFromObject_TD_6558_input.getInt32(i, 1);
            int32_t _ps_partkey305_k = tbl_SerializeFromObject_TD_6558_input.getInt32(i, 0);
            SW_JOIN_INNER_TD_5847_key_rightMajor keyA{_ps_suppkey306_k, _ps_partkey305_k};
            int32_t _ps_partkey305 = tbl_SerializeFromObject_TD_6558_input.getInt32(i, 0);
            int32_t _ps_suppkey306 = tbl_SerializeFromObject_TD_6558_input.getInt32(i, 1);
            int32_t _ps_supplycost308 = tbl_SerializeFromObject_TD_6558_input.getInt32(i, 2);
            SW_JOIN_INNER_TD_5847_payload_rightMajor payloadA{_ps_partkey305, _ps_suppkey306, _ps_supplycost308};
            ht1.insert(std::make_pair(keyA, payloadA));
        }
        int r = 0;
        int nrow2 = tbl_JOIN_INNER_TD_6541_output.getNumRow();
        for (int i = 0; i < nrow2; i++) {
            int32_t _l_suppkey79_k = tbl_JOIN_INNER_TD_6541_output.getInt32(i, 2);
            int32_t _l_partkey78_k = tbl_JOIN_INNER_TD_6541_output.getInt32(i, 1);
            auto its = ht1.equal_range(SW_JOIN_INNER_TD_5847_key_rightMajor{_l_suppkey79_k, _l_partkey78_k});
            auto it = its.first;
            while (it != its.second) {
                int32_t _ps_partkey305 = (it->second)._ps_partkey305;
                int32_t _ps_suppkey306 = (it->second)._ps_suppkey306;
                int32_t _ps_supplycost308 = (it->second)._ps_supplycost308;
                int32_t _l_orderkey77 = tbl_JOIN_INNER_TD_6541_output.getInt32(i, 0);
                int32_t _l_partkey78 = tbl_JOIN_INNER_TD_6541_output.getInt32(i, 1);
                int32_t _l_suppkey79 = tbl_JOIN_INNER_TD_6541_output.getInt32(i, 2);
                int32_t _l_quantity81 = tbl_JOIN_INNER_TD_6541_output.getInt32(i, 3);
                int32_t _l_extendedprice82 = tbl_JOIN_INNER_TD_6541_output.getInt32(i, 4);
                int32_t _l_discount83 = tbl_JOIN_INNER_TD_6541_output.getInt32(i, 5);
                int32_t _s_nationkey345 = tbl_JOIN_INNER_TD_6541_output.getInt32(i, 6);
                tbl_JOIN_INNER_TD_5847_output.setInt32(r, 5, _ps_supplycost308);
                tbl_JOIN_INNER_TD_5847_output.setInt32(r, 0, _l_orderkey77);
                tbl_JOIN_INNER_TD_5847_output.setInt32(r, 1, _l_quantity81);
                tbl_JOIN_INNER_TD_5847_output.setInt32(r, 2, _l_extendedprice82);
                tbl_JOIN_INNER_TD_5847_output.setInt32(r, 3, _l_discount83);
                tbl_JOIN_INNER_TD_5847_output.setInt32(r, 4, _s_nationkey345);
                it++;
                r++;
            }
        }
        tbl_JOIN_INNER_TD_5847_output.setNumRow(r);
    } 
    std::cout << "tbl_JOIN_INNER_TD_5847_output #Row: " << tbl_JOIN_INNER_TD_5847_output.getNumRow() << std::endl;
}

struct SW_JOIN_INNER_TD_4845_key_leftMajor {
    int32_t _l_orderkey77;
    bool operator==(const SW_JOIN_INNER_TD_4845_key_leftMajor& other) const {
        return ((_l_orderkey77 == other._l_orderkey77));
    }
};
namespace std {
template <>
struct hash<SW_JOIN_INNER_TD_4845_key_leftMajor> {
    std::size_t operator() (const SW_JOIN_INNER_TD_4845_key_leftMajor& k) const {
        using std::size_t;
        using std::hash;
        using std::string;
        return (hash<int32_t>()(k._l_orderkey77));
    }
};
}
struct SW_JOIN_INNER_TD_4845_payload_leftMajor {
    int32_t _l_orderkey77;
    int32_t _l_quantity81;
    int32_t _l_extendedprice82;
    int32_t _l_discount83;
    int32_t _s_nationkey345;
    int32_t _ps_supplycost308;
};
struct SW_JOIN_INNER_TD_4845_key_rightMajor {
    int32_t _o_orderkey207;
    bool operator==(const SW_JOIN_INNER_TD_4845_key_rightMajor& other) const {
        return ((_o_orderkey207 == other._o_orderkey207));
    }
};
namespace std {
template <>
struct hash<SW_JOIN_INNER_TD_4845_key_rightMajor> {
    std::size_t operator() (const SW_JOIN_INNER_TD_4845_key_rightMajor& k) const {
        using std::size_t;
        using std::hash;
        using std::string;
        return (hash<int32_t>()(k._o_orderkey207));
    }
};
}
struct SW_JOIN_INNER_TD_4845_payload_rightMajor {
    int32_t _o_orderkey207;
    int32_t _o_orderdate211;
};
void SW_JOIN_INNER_TD_4845(Table &tbl_JOIN_INNER_TD_5847_output, Table &tbl_SerializeFromObject_TD_590_input, Table &tbl_JOIN_INNER_TD_4845_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: JOIN_INNER
    // Operation: ListBuffer((o_orderkey#207 = l_orderkey#77))
    // Left Table: ListBuffer(l_orderkey#77, l_quantity#81, l_extendedprice#82, l_discount#83, s_nationkey#345, ps_supplycost#308)
    // Right Table: ListBuffer(o_orderkey#207, o_orderdate#211)
    // Output Table: ListBuffer(l_quantity#81, l_extendedprice#82, l_discount#83, s_nationkey#345, ps_supplycost#308, o_orderdate#211)
    int left_nrow = tbl_JOIN_INNER_TD_5847_output.getNumRow();
    int right_nrow = tbl_SerializeFromObject_TD_590_input.getNumRow();
    if (left_nrow < right_nrow) { 
        std::unordered_multimap<SW_JOIN_INNER_TD_4845_key_leftMajor, SW_JOIN_INNER_TD_4845_payload_leftMajor> ht1;
        int nrow1 = tbl_JOIN_INNER_TD_5847_output.getNumRow();
        for (int i = 0; i < nrow1; i++) {
            int32_t _l_orderkey77_k = tbl_JOIN_INNER_TD_5847_output.getInt32(i, 0);
            SW_JOIN_INNER_TD_4845_key_leftMajor keyA{_l_orderkey77_k};
            int32_t _l_orderkey77 = tbl_JOIN_INNER_TD_5847_output.getInt32(i, 0);
            int32_t _l_quantity81 = tbl_JOIN_INNER_TD_5847_output.getInt32(i, 1);
            int32_t _l_extendedprice82 = tbl_JOIN_INNER_TD_5847_output.getInt32(i, 2);
            int32_t _l_discount83 = tbl_JOIN_INNER_TD_5847_output.getInt32(i, 3);
            int32_t _s_nationkey345 = tbl_JOIN_INNER_TD_5847_output.getInt32(i, 4);
            int32_t _ps_supplycost308 = tbl_JOIN_INNER_TD_5847_output.getInt32(i, 5);
            SW_JOIN_INNER_TD_4845_payload_leftMajor payloadA{_l_orderkey77, _l_quantity81, _l_extendedprice82, _l_discount83, _s_nationkey345, _ps_supplycost308};
            ht1.insert(std::make_pair(keyA, payloadA));
        }
        int r = 0;
        int nrow2 = tbl_SerializeFromObject_TD_590_input.getNumRow();
        for (int i = 0; i < nrow2; i++) {
            int32_t _o_orderkey207_k = tbl_SerializeFromObject_TD_590_input.getInt32(i, 0);
            auto its = ht1.equal_range(SW_JOIN_INNER_TD_4845_key_leftMajor{_o_orderkey207_k});
            auto it = its.first;
            while (it != its.second) {
                int32_t _l_orderkey77 = (it->second)._l_orderkey77;
                int32_t _l_quantity81 = (it->second)._l_quantity81;
                int32_t _l_extendedprice82 = (it->second)._l_extendedprice82;
                int32_t _l_discount83 = (it->second)._l_discount83;
                int32_t _s_nationkey345 = (it->second)._s_nationkey345;
                int32_t _ps_supplycost308 = (it->second)._ps_supplycost308;
                int32_t _o_orderkey207 = tbl_SerializeFromObject_TD_590_input.getInt32(i, 0);
                int32_t _o_orderdate211 = tbl_SerializeFromObject_TD_590_input.getInt32(i, 1);
                tbl_JOIN_INNER_TD_4845_output.setInt32(r, 0, _l_quantity81);
                tbl_JOIN_INNER_TD_4845_output.setInt32(r, 1, _l_extendedprice82);
                tbl_JOIN_INNER_TD_4845_output.setInt32(r, 2, _l_discount83);
                tbl_JOIN_INNER_TD_4845_output.setInt32(r, 3, _s_nationkey345);
                tbl_JOIN_INNER_TD_4845_output.setInt32(r, 4, _ps_supplycost308);
                tbl_JOIN_INNER_TD_4845_output.setInt32(r, 5, _o_orderdate211);
                it++;
                r++;
            }
        }
        tbl_JOIN_INNER_TD_4845_output.setNumRow(r);
    } else { 
        std::unordered_multimap<SW_JOIN_INNER_TD_4845_key_rightMajor, SW_JOIN_INNER_TD_4845_payload_rightMajor> ht1;
        int nrow1 = tbl_SerializeFromObject_TD_590_input.getNumRow();
        for (int i = 0; i < nrow1; i++) {
            int32_t _o_orderkey207_k = tbl_SerializeFromObject_TD_590_input.getInt32(i, 0);
            SW_JOIN_INNER_TD_4845_key_rightMajor keyA{_o_orderkey207_k};
            int32_t _o_orderkey207 = tbl_SerializeFromObject_TD_590_input.getInt32(i, 0);
            int32_t _o_orderdate211 = tbl_SerializeFromObject_TD_590_input.getInt32(i, 1);
            SW_JOIN_INNER_TD_4845_payload_rightMajor payloadA{_o_orderkey207, _o_orderdate211};
            ht1.insert(std::make_pair(keyA, payloadA));
        }
        int r = 0;
        int nrow2 = tbl_JOIN_INNER_TD_5847_output.getNumRow();
        for (int i = 0; i < nrow2; i++) {
            int32_t _l_orderkey77_k = tbl_JOIN_INNER_TD_5847_output.getInt32(i, 0);
            auto its = ht1.equal_range(SW_JOIN_INNER_TD_4845_key_rightMajor{_l_orderkey77_k});
            auto it = its.first;
            while (it != its.second) {
                int32_t _o_orderkey207 = (it->second)._o_orderkey207;
                int32_t _o_orderdate211 = (it->second)._o_orderdate211;
                int32_t _l_orderkey77 = tbl_JOIN_INNER_TD_5847_output.getInt32(i, 0);
                int32_t _l_quantity81 = tbl_JOIN_INNER_TD_5847_output.getInt32(i, 1);
                int32_t _l_extendedprice82 = tbl_JOIN_INNER_TD_5847_output.getInt32(i, 2);
                int32_t _l_discount83 = tbl_JOIN_INNER_TD_5847_output.getInt32(i, 3);
                int32_t _s_nationkey345 = tbl_JOIN_INNER_TD_5847_output.getInt32(i, 4);
                int32_t _ps_supplycost308 = tbl_JOIN_INNER_TD_5847_output.getInt32(i, 5);
                tbl_JOIN_INNER_TD_4845_output.setInt32(r, 5, _o_orderdate211);
                tbl_JOIN_INNER_TD_4845_output.setInt32(r, 0, _l_quantity81);
                tbl_JOIN_INNER_TD_4845_output.setInt32(r, 1, _l_extendedprice82);
                tbl_JOIN_INNER_TD_4845_output.setInt32(r, 2, _l_discount83);
                tbl_JOIN_INNER_TD_4845_output.setInt32(r, 3, _s_nationkey345);
                tbl_JOIN_INNER_TD_4845_output.setInt32(r, 4, _ps_supplycost308);
                it++;
                r++;
            }
        }
        tbl_JOIN_INNER_TD_4845_output.setNumRow(r);
    } 
    std::cout << "tbl_JOIN_INNER_TD_4845_output #Row: " << tbl_JOIN_INNER_TD_4845_output.getNumRow() << std::endl;
}

struct SW_JOIN_INNER_TD_382_key_leftMajor {
    int32_t _s_nationkey345;
    bool operator==(const SW_JOIN_INNER_TD_382_key_leftMajor& other) const {
        return ((_s_nationkey345 == other._s_nationkey345));
    }
};
namespace std {
template <>
struct hash<SW_JOIN_INNER_TD_382_key_leftMajor> {
    std::size_t operator() (const SW_JOIN_INNER_TD_382_key_leftMajor& k) const {
        using std::size_t;
        using std::hash;
        using std::string;
        return (hash<int32_t>()(k._s_nationkey345));
    }
};
}
struct SW_JOIN_INNER_TD_382_payload_leftMajor {
    int32_t _l_quantity81;
    int32_t _l_extendedprice82;
    int32_t _l_discount83;
    int32_t _s_nationkey345;
    int32_t _ps_supplycost308;
    int32_t _o_orderdate211;
};
struct SW_JOIN_INNER_TD_382_key_rightMajor {
    int32_t _n_nationkey144;
    bool operator==(const SW_JOIN_INNER_TD_382_key_rightMajor& other) const {
        return ((_n_nationkey144 == other._n_nationkey144));
    }
};
namespace std {
template <>
struct hash<SW_JOIN_INNER_TD_382_key_rightMajor> {
    std::size_t operator() (const SW_JOIN_INNER_TD_382_key_rightMajor& k) const {
        using std::size_t;
        using std::hash;
        using std::string;
        return (hash<int32_t>()(k._n_nationkey144));
    }
};
}
struct SW_JOIN_INNER_TD_382_payload_rightMajor {
    int32_t _n_nationkey144;
    std::string _n_name145;
};
void SW_JOIN_INNER_TD_382(Table &tbl_JOIN_INNER_TD_4845_output, Table &tbl_SerializeFromObject_TD_435_input, Table &tbl_JOIN_INNER_TD_382_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: JOIN_INNER
    // Operation: ListBuffer((s_nationkey#345 = n_nationkey#144))
    // Left Table: ListBuffer(l_quantity#81, l_extendedprice#82, l_discount#83, s_nationkey#345, ps_supplycost#308, o_orderdate#211)
    // Right Table: ListBuffer(n_nationkey#144, n_name#145)
    // Output Table: ListBuffer(n_name#145, o_orderdate#211, l_extendedprice#82, l_discount#83, ps_supplycost#308, l_quantity#81)
    int left_nrow = tbl_JOIN_INNER_TD_4845_output.getNumRow();
    int right_nrow = tbl_SerializeFromObject_TD_435_input.getNumRow();
    if (left_nrow < right_nrow) { 
        std::unordered_multimap<SW_JOIN_INNER_TD_382_key_leftMajor, SW_JOIN_INNER_TD_382_payload_leftMajor> ht1;
        int nrow1 = tbl_JOIN_INNER_TD_4845_output.getNumRow();
        for (int i = 0; i < nrow1; i++) {
            int32_t _s_nationkey345_k = tbl_JOIN_INNER_TD_4845_output.getInt32(i, 3);
            SW_JOIN_INNER_TD_382_key_leftMajor keyA{_s_nationkey345_k};
            int32_t _l_quantity81 = tbl_JOIN_INNER_TD_4845_output.getInt32(i, 0);
            int32_t _l_extendedprice82 = tbl_JOIN_INNER_TD_4845_output.getInt32(i, 1);
            int32_t _l_discount83 = tbl_JOIN_INNER_TD_4845_output.getInt32(i, 2);
            int32_t _s_nationkey345 = tbl_JOIN_INNER_TD_4845_output.getInt32(i, 3);
            int32_t _ps_supplycost308 = tbl_JOIN_INNER_TD_4845_output.getInt32(i, 4);
            int32_t _o_orderdate211 = tbl_JOIN_INNER_TD_4845_output.getInt32(i, 5);
            SW_JOIN_INNER_TD_382_payload_leftMajor payloadA{_l_quantity81, _l_extendedprice82, _l_discount83, _s_nationkey345, _ps_supplycost308, _o_orderdate211};
            ht1.insert(std::make_pair(keyA, payloadA));
        }
        int r = 0;
        int nrow2 = tbl_SerializeFromObject_TD_435_input.getNumRow();
        for (int i = 0; i < nrow2; i++) {
            int32_t _n_nationkey144_k = tbl_SerializeFromObject_TD_435_input.getInt32(i, 0);
            auto its = ht1.equal_range(SW_JOIN_INNER_TD_382_key_leftMajor{_n_nationkey144_k});
            auto it = its.first;
            while (it != its.second) {
                int32_t _l_quantity81 = (it->second)._l_quantity81;
                int32_t _l_extendedprice82 = (it->second)._l_extendedprice82;
                int32_t _l_discount83 = (it->second)._l_discount83;
                int32_t _s_nationkey345 = (it->second)._s_nationkey345;
                int32_t _ps_supplycost308 = (it->second)._ps_supplycost308;
                int32_t _o_orderdate211 = (it->second)._o_orderdate211;
                int32_t _n_nationkey144 = tbl_SerializeFromObject_TD_435_input.getInt32(i, 0);
                std::array<char, TPCH_READ_NATION_LEN + 1> _n_name145_n = tbl_SerializeFromObject_TD_435_input.getcharN<char, TPCH_READ_NATION_LEN + 1>(i, 1);
                std::string _n_name145 = std::string(_n_name145_n.data());
                tbl_JOIN_INNER_TD_382_output.setInt32(r, 5, _l_quantity81);
                tbl_JOIN_INNER_TD_382_output.setInt32(r, 2, _l_extendedprice82);
                tbl_JOIN_INNER_TD_382_output.setInt32(r, 3, _l_discount83);
                tbl_JOIN_INNER_TD_382_output.setInt32(r, 4, _ps_supplycost308);
                tbl_JOIN_INNER_TD_382_output.setInt32(r, 1, _o_orderdate211);
                tbl_JOIN_INNER_TD_382_output.setcharN<char, TPCH_READ_NATION_LEN + 1>(r, 0, _n_name145_n);
                it++;
                r++;
            }
        }
        tbl_JOIN_INNER_TD_382_output.setNumRow(r);
    } else { 
        std::unordered_multimap<SW_JOIN_INNER_TD_382_key_rightMajor, SW_JOIN_INNER_TD_382_payload_rightMajor> ht1;
        int nrow1 = tbl_SerializeFromObject_TD_435_input.getNumRow();
        for (int i = 0; i < nrow1; i++) {
            int32_t _n_nationkey144_k = tbl_SerializeFromObject_TD_435_input.getInt32(i, 0);
            SW_JOIN_INNER_TD_382_key_rightMajor keyA{_n_nationkey144_k};
            int32_t _n_nationkey144 = tbl_SerializeFromObject_TD_435_input.getInt32(i, 0);
            std::array<char, TPCH_READ_NATION_LEN + 1> _n_name145_n = tbl_SerializeFromObject_TD_435_input.getcharN<char, TPCH_READ_NATION_LEN + 1>(i, 1);
            std::string _n_name145 = std::string(_n_name145_n.data());
            SW_JOIN_INNER_TD_382_payload_rightMajor payloadA{_n_nationkey144, _n_name145};
            ht1.insert(std::make_pair(keyA, payloadA));
        }
        int r = 0;
        int nrow2 = tbl_JOIN_INNER_TD_4845_output.getNumRow();
        for (int i = 0; i < nrow2; i++) {
            int32_t _s_nationkey345_k = tbl_JOIN_INNER_TD_4845_output.getInt32(i, 3);
            auto its = ht1.equal_range(SW_JOIN_INNER_TD_382_key_rightMajor{_s_nationkey345_k});
            auto it = its.first;
            while (it != its.second) {
                int32_t _n_nationkey144 = (it->second)._n_nationkey144;
                std::string _n_name145 = (it->second)._n_name145;
                std::array<char, TPCH_READ_NATION_LEN + 1> _n_name145_n{};
                memcpy(_n_name145_n.data(), (_n_name145).data(), (_n_name145).length());
                int32_t _l_quantity81 = tbl_JOIN_INNER_TD_4845_output.getInt32(i, 0);
                int32_t _l_extendedprice82 = tbl_JOIN_INNER_TD_4845_output.getInt32(i, 1);
                int32_t _l_discount83 = tbl_JOIN_INNER_TD_4845_output.getInt32(i, 2);
                int32_t _s_nationkey345 = tbl_JOIN_INNER_TD_4845_output.getInt32(i, 3);
                int32_t _ps_supplycost308 = tbl_JOIN_INNER_TD_4845_output.getInt32(i, 4);
                int32_t _o_orderdate211 = tbl_JOIN_INNER_TD_4845_output.getInt32(i, 5);
                tbl_JOIN_INNER_TD_382_output.setcharN<char, TPCH_READ_NATION_LEN + 1>(r, 0, _n_name145_n);
                tbl_JOIN_INNER_TD_382_output.setInt32(r, 5, _l_quantity81);
                tbl_JOIN_INNER_TD_382_output.setInt32(r, 2, _l_extendedprice82);
                tbl_JOIN_INNER_TD_382_output.setInt32(r, 3, _l_discount83);
                tbl_JOIN_INNER_TD_382_output.setInt32(r, 4, _ps_supplycost308);
                tbl_JOIN_INNER_TD_382_output.setInt32(r, 1, _o_orderdate211);
                it++;
                r++;
            }
        }
        tbl_JOIN_INNER_TD_382_output.setNumRow(r);
    } 
    std::cout << "tbl_JOIN_INNER_TD_382_output #Row: " << tbl_JOIN_INNER_TD_382_output.getNumRow() << std::endl;
}

void SW_Project_TD_2613(Table &tbl_JOIN_INNER_TD_382_output, Table &tbl_Project_TD_2613_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: Project
    // Operation: ListBuffer(n_name#145 AS nation#1060, cast((cast(o_orderdate#211 as double) / 10000.0) as int) AS o_year#1061, ((l_extendedprice#82 * (1 - l_discount#83)) - (ps_supplycost#308 * l_quantity#81)) AS amount#1062)
    // Input: ListBuffer(n_name#145, o_orderdate#211, l_extendedprice#82, l_discount#83, ps_supplycost#308, l_quantity#81)
    // Output: ListBuffer(nation#1060, o_year#1061, amount#1062)
    int nrow1 = tbl_JOIN_INNER_TD_382_output.getNumRow();
    for (int i = 0; i < nrow1; i++) {
        std::array<char, TPCH_READ_NATION_LEN + 1> _n_name145 = tbl_JOIN_INNER_TD_382_output.getcharN<char, TPCH_READ_NATION_LEN + 1>(i, 0);
        int32_t _o_orderdate211 = tbl_JOIN_INNER_TD_382_output.getInt32(i, 1);
        int32_t _l_extendedprice82 = tbl_JOIN_INNER_TD_382_output.getInt32(i, 2);
        int32_t _l_discount83 = tbl_JOIN_INNER_TD_382_output.getInt32(i, 3);
        int32_t _ps_supplycost308 = tbl_JOIN_INNER_TD_382_output.getInt32(i, 4);
        int32_t _l_quantity81 = tbl_JOIN_INNER_TD_382_output.getInt32(i, 5);
        std::array<char, TPCH_READ_NATION_LEN + 1> _nation1060 = _n_name145;
        tbl_Project_TD_2613_output.setcharN<char, TPCH_READ_NATION_LEN + 1>(i, 0, _nation1060);
        int32_t _o_year1061 = (_o_orderdate211 / 10000.0);
        tbl_Project_TD_2613_output.setInt32(i, 1, _o_year1061);
        int32_t _amount1062 = ((_l_extendedprice82 * (1 - _l_discount83)) - (_ps_supplycost308 * _l_quantity81));
        tbl_Project_TD_2613_output.setInt32(i, 2, _amount1062);
    }
    tbl_Project_TD_2613_output.setNumRow(nrow1);
    std::cout << "tbl_Project_TD_2613_output #Row: " << tbl_Project_TD_2613_output.getNumRow() << std::endl;
}

struct SW_Aggregate_TD_1779_key {
    std::string _nation1060;
    int32_t _o_year1061;
    bool operator==(const SW_Aggregate_TD_1779_key& other) const { return (_nation1060 == other._nation1060) && (_o_year1061 == other._o_year1061); }
};
namespace std {
template <>
struct hash<SW_Aggregate_TD_1779_key> {
    std::size_t operator() (const SW_Aggregate_TD_1779_key& k) const {
        using std::size_t;
        using std::hash;
        using std::string;
        return (hash<string>()(k._nation1060)) + (hash<int32_t>()(k._o_year1061));
    }
};
}
struct SW_Aggregate_TD_1779_payload {
    int64_t _sum_profit1063L_sum_0;
};
void SW_Aggregate_TD_1779(Table &tbl_Project_TD_2613_output, Table &tbl_Aggregate_TD_1779_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: Aggregate
    // Operation: ListBuffer(nation#1060, o_year#1061, sum(amount#1062) AS sum_profit#1063L)
    // Input: ListBuffer(nation#1060, o_year#1061, amount#1062)
    // Output: ListBuffer(nation#1060, o_year#1061, sum_profit#1063L)
    std::unordered_map<SW_Aggregate_TD_1779_key, SW_Aggregate_TD_1779_payload> ht1;
    int nrow1 = tbl_Project_TD_2613_output.getNumRow();
    for (int i = 0; i < nrow1; i++) {
        std::array<char, TPCH_READ_NATION_LEN + 1> _nation1060 = tbl_Project_TD_2613_output.getcharN<char, TPCH_READ_NATION_LEN + 1>(i, 0);
        int32_t _o_year1061 = tbl_Project_TD_2613_output.getInt32(i, 1);
        int32_t _amount1062 = tbl_Project_TD_2613_output.getInt32(i, 2);
        SW_Aggregate_TD_1779_key k{std::string(_nation1060.data()), _o_year1061};
        int64_t _sum_profit1063L_sum_0 = _amount1062;
        SW_Aggregate_TD_1779_payload p{_sum_profit1063L_sum_0};
        auto it = ht1.find(k);
        if (it != ht1.end()) {
            int64_t sum_0 = (it->second)._sum_profit1063L_sum_0 + _sum_profit1063L_sum_0;
            p._sum_profit1063L_sum_0 = sum_0;
            ht1[k] = p;
        } else { 
            ht1.insert(std::make_pair(k, p));
        }
    }
    int r = 0;
    for (auto& it : ht1) {
        std::array<char, TPCH_READ_NATION_LEN + 1> _nation1060{};
        memcpy(_nation1060.data(), ((it.first)._nation1060).data(), ((it.first)._nation1060).length());
        tbl_Aggregate_TD_1779_output.setcharN<char, TPCH_READ_NATION_LEN + 1>(r, 0, _nation1060);
        tbl_Aggregate_TD_1779_output.setInt32(r, 1, (it.first)._o_year1061);
        int64_t _sum_profit1063L = (it.second)._sum_profit1063L_sum_0;
        tbl_Aggregate_TD_1779_output.setInt64(r, 2, _sum_profit1063L);
        ++r;
    }
    tbl_Aggregate_TD_1779_output.setNumRow(r);
    std::cout << "tbl_Aggregate_TD_1779_output #Row: " << tbl_Aggregate_TD_1779_output.getNumRow() << std::endl;
}

void SW_Sort_TD_0901(Table &tbl_Aggregate_TD_1779_output, Table &tbl_Sort_TD_0901_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: Sort
    // Operation: ListBuffer(nation#1060 ASC NULLS FIRST, o_year#1061 DESC NULLS LAST)
    // Input: ListBuffer(nation#1060, o_year#1061, sum_profit#1063L)
    // Output: ListBuffer(nation#1060, o_year#1061, sum_profit#1063L)
    struct SW_Sort_TD_0901Row {
        std::string _nation1060;
        int32_t _o_year1061;
        int64_t _sum_profit1063L;
    }; 

    struct {
        bool operator()(const SW_Sort_TD_0901Row& a, const SW_Sort_TD_0901Row& b) const { return 
 (a._nation1060 < b._nation1060) || 
 ((a._nation1060 == b._nation1060) && (a._o_year1061 > b._o_year1061)); 
}
    }SW_Sort_TD_0901_order; 

    int nrow1 = tbl_Aggregate_TD_1779_output.getNumRow();
    std::vector<SW_Sort_TD_0901Row> rows;
    for (int i = 0; i < nrow1; i++) {
        std::array<char, TPCH_READ_NATION_LEN + 1> _nation1060 = tbl_Aggregate_TD_1779_output.getcharN<char, TPCH_READ_NATION_LEN +1>(i, 0);
        int32_t _o_year1061 = tbl_Aggregate_TD_1779_output.getInt32(i, 1);
        int64_t _sum_profit1063L = tbl_Aggregate_TD_1779_output.getInt64(i, 2);
        SW_Sort_TD_0901Row t = {std::string(_nation1060.data()),_o_year1061,_sum_profit1063L};
        rows.push_back(t);
    }
    std::sort(rows.begin(), rows.end(), SW_Sort_TD_0901_order);
    int r = 0;
    for (auto& it : rows) {
        std::array<char, TPCH_READ_NATION_LEN + 1> _nation1060{};
        memcpy(_nation1060.data(), (it._nation1060).data(), (it._nation1060).length());
        tbl_Sort_TD_0901_output.setcharN<char, TPCH_READ_NATION_LEN +1>(r, 0, _nation1060);
        tbl_Sort_TD_0901_output.setInt32(r, 1, it._o_year1061);
        tbl_Sort_TD_0901_output.setInt64(r, 2, it._sum_profit1063L);
        if (r < 10) {
            std::cout << (it._nation1060).data() << " " << it._o_year1061 << " " << it._sum_profit1063L << " " << std::endl;
        }
        ++r;
    }
    tbl_Sort_TD_0901_output.setNumRow(r);
    std::cout << "tbl_Sort_TD_0901_output #Row: " << tbl_Sort_TD_0901_output.getNumRow() << std::endl;
}

