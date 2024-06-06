#include <regex> 
#include <stdint.h> 

void SW_Filter_TD_870(Table &tbl_SerializeFromObject_TD_96_input, Table &tbl_Filter_TD_870_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: Filter
    // Operation: ListBuffer(((l_shipdate#87 >= 19950101) AND (l_shipdate#87 <= 19961231)))
    // Input: ListBuffer(l_orderkey#77, l_suppkey#79, l_extendedprice#82, l_discount#83, l_shipdate#87)
    // Output: ListBuffer(l_orderkey#77, l_suppkey#79, l_extendedprice#82, l_discount#83, l_shipdate#87)
    int r = 0;
    int nrow1 = tbl_SerializeFromObject_TD_96_input.getNumRow();
    for (int i = 0; i < nrow1; i++) {
        int32_t _l_shipdate87 = tbl_SerializeFromObject_TD_96_input.getInt32(i, 4);
        if ((_l_shipdate87 >= 19950101) && (_l_shipdate87 <= 19961231)) {
            int32_t _l_orderkey77_t = tbl_SerializeFromObject_TD_96_input.getInt32(i, 0);
            tbl_Filter_TD_870_output.setInt32(r, 0, _l_orderkey77_t);
            int32_t _l_suppkey79_t = tbl_SerializeFromObject_TD_96_input.getInt32(i, 1);
            tbl_Filter_TD_870_output.setInt32(r, 1, _l_suppkey79_t);
            int32_t _l_extendedprice82_t = tbl_SerializeFromObject_TD_96_input.getInt32(i, 2);
            tbl_Filter_TD_870_output.setInt32(r, 2, _l_extendedprice82_t);
            int32_t _l_discount83_t = tbl_SerializeFromObject_TD_96_input.getInt32(i, 3);
            tbl_Filter_TD_870_output.setInt32(r, 3, _l_discount83_t);
            int32_t _l_shipdate87_t = tbl_SerializeFromObject_TD_96_input.getInt32(i, 4);
            tbl_Filter_TD_870_output.setInt32(r, 4, _l_shipdate87_t);
            r++;
        }
    }
    tbl_Filter_TD_870_output.setNumRow(r);
    std::cout << "tbl_Filter_TD_870_output #Row: " << tbl_Filter_TD_870_output.getNumRow() << std::endl;
}

void SW_Filter_TD_5344(Table &tbl_SerializeFromObject_TD_6527_input, Table &tbl_Filter_TD_5344_output) {
    // StringRowIDSubstitution: true StringRowIDBackSubstitution: false
    // Supported operation: Filter
    // Operation: ListBuffer(((n_name#145 = FRANCE) OR (n_name#145 = IRAQ)))
    // Input: ListBuffer(n_nationkey#144, n_name#145)
    // Output: ListBuffer(n_nationkey#144, n_name#145)
    int r = 0;
    int nrow1 = tbl_SerializeFromObject_TD_6527_input.getNumRow();
    for (int i = 0; i < nrow1; i++) {
        std::array<char, TPCH_READ_NATION_LEN + 1> _n_name145 = tbl_SerializeFromObject_TD_6527_input.getcharN<char, TPCH_READ_NATION_LEN + 1>(i, 1);
        if ((std::string(_n_name145.data()) == "FRANCE") || (std::string(_n_name145.data()) == "IRAQ")) {
            int32_t _n_nationkey144_t = tbl_SerializeFromObject_TD_6527_input.getInt32(i, 0);
            tbl_Filter_TD_5344_output.setInt32(r, 0, _n_nationkey144_t);
            tbl_Filter_TD_5344_output.setInt32(r, 1, i);
            r++;
        }
    }
    tbl_Filter_TD_5344_output.setNumRow(r);
    std::cout << "tbl_Filter_TD_5344_output #Row: " << tbl_Filter_TD_5344_output.getNumRow() << std::endl;
}

void SW_Consolidated_JOIN_INNER_TD_7285_output(Table& in1, Table&in2, Table& tbl_JOIN_INNER_TD_7285_output, Table& tbl_JOIN_INNER_TD_7285_consolidated_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: JOIN_INNER
    // Operation: ListBuffer((s_suppkey#342 = l_suppkey#79))
    // Left Table: ListBuffer(s_suppkey#342, s_nationkey#345)
    // Right Table: ListBuffer(l_orderkey#77, l_suppkey#79, l_extendedprice#82, l_discount#83, l_shipdate#87)
    // Output Table: ListBuffer(s_nationkey#345, l_orderkey#77, l_extendedprice#82, l_discount#83, l_shipdate#87)
    // Node Depth: 7
    int num_rows = tbl_JOIN_INNER_TD_7285_output.getNumRow();
        for (int j = 0; j < num_rows; j++) {
          int32_t rowid_1 = tbl_JOIN_INNER_TD_7285_output.getInt32(j, 0);
          int32_t rowid_2 = tbl_JOIN_INNER_TD_7285_output.getInt32(j, 1);
          int64_t key = tbl_JOIN_INNER_TD_7285_output.getInt32(j, 2);
          tbl_JOIN_INNER_TD_7285_consolidated_output.setInt32(j, 0, in1.getInt32(rowid_2 - 1, 1));
          tbl_JOIN_INNER_TD_7285_consolidated_output.setInt32(j, 1, in2.getInt32(rowid_1 - 1, 0));
          tbl_JOIN_INNER_TD_7285_consolidated_output.setInt32(j, 2, in2.getInt32(rowid_1 - 1, 2));
          tbl_JOIN_INNER_TD_7285_consolidated_output.setInt32(j, 3, in2.getInt32(rowid_1 - 1, 3));
          tbl_JOIN_INNER_TD_7285_consolidated_output.setInt32(j, 4, in2.getInt32(rowid_1 - 1, 4));
        }
        tbl_JOIN_INNER_TD_7285_consolidated_output.setNumRow(num_rows);
    std::cout << "tbl_JOIN_INNER_TD_7285_consolidated_output #Row: " << tbl_JOIN_INNER_TD_7285_consolidated_output.getNumRow() << std::endl;
}

void SW_Consolidated_JOIN_INNER_TD_6649_output(Table& in1, Table&in2, Table& tbl_JOIN_INNER_TD_6649_output, Table& tbl_JOIN_INNER_TD_6649_consolidated_output) {
    // StringRowIDSubstitution: true StringRowIDBackSubstitution: false
    // Supported operation: JOIN_INNER
    // Operation: ListBuffer((s_nationkey#345 = n_nationkey#144))
    // Left Table: ListBuffer(n_nationkey#144, n_name#145)
    // Right Table: ListBuffer(s_nationkey#345, l_orderkey#77, l_extendedprice#82, l_discount#83, l_shipdate#87)
    // Output Table: ListBuffer(l_extendedprice#82, l_discount#83, l_shipdate#87, n_name#145, s_nationkey#345, l_orderkey#77)
    // Node Depth: 6
    int num_rows = tbl_JOIN_INNER_TD_6649_output.getNumRow();
        for (int j = 0; j < num_rows; j++) {
          int32_t rowid_1 = tbl_JOIN_INNER_TD_6649_output.getInt32(j, 0);
          int32_t rowid_2 = tbl_JOIN_INNER_TD_6649_output.getInt32(j, 1);
          int64_t key = tbl_JOIN_INNER_TD_6649_output.getInt32(j, 2);
          tbl_JOIN_INNER_TD_6649_consolidated_output.setInt32(j, 0, in2.getInt32(rowid_1 - 1, 2));
          tbl_JOIN_INNER_TD_6649_consolidated_output.setInt32(j, 1, in2.getInt32(rowid_1 - 1, 3));
          tbl_JOIN_INNER_TD_6649_consolidated_output.setInt32(j, 2, in2.getInt32(rowid_1 - 1, 4));
          tbl_JOIN_INNER_TD_6649_consolidated_output.setInt32(j, 3, in1.getInt32(rowid_2 - 1, 1));
          tbl_JOIN_INNER_TD_6649_consolidated_output.setInt32(j, 4, in2.getInt32(rowid_1 - 1, 0));
          tbl_JOIN_INNER_TD_6649_consolidated_output.setInt32(j, 5, in2.getInt32(rowid_1 - 1, 1));
        }
        tbl_JOIN_INNER_TD_6649_consolidated_output.setNumRow(num_rows);
    std::cout << "tbl_JOIN_INNER_TD_6649_consolidated_output #Row: " << tbl_JOIN_INNER_TD_6649_consolidated_output.getNumRow() << std::endl;
}

void SW_Consolidated_JOIN_INNER_TD_5272_output(Table& in1, Table&in2, Table& tbl_JOIN_INNER_TD_5272_output, Table& tbl_JOIN_INNER_TD_5272_consolidated_output) {
    // StringRowIDSubstitution: true StringRowIDBackSubstitution: false
    // Supported operation: JOIN_INNER
    // Operation: ListBuffer((o_orderkey#207 = l_orderkey#77))
    // Left Table: ListBuffer(l_extendedprice#82, l_discount#83, l_shipdate#87, n_name#145, s_nationkey#345, l_orderkey#77)
    // Right Table: ListBuffer(o_orderkey#207, o_custkey#208)
    // Output Table: ListBuffer(s_nationkey#345, l_extendedprice#82, l_discount#83, l_shipdate#87, o_custkey#208, n_name#145)
    // Node Depth: 5
    int num_rows = tbl_JOIN_INNER_TD_5272_output.getNumRow();
        for (int j = 0; j < num_rows; j++) {
          int32_t rowid_1 = tbl_JOIN_INNER_TD_5272_output.getInt32(j, 0);
          int32_t rowid_2 = tbl_JOIN_INNER_TD_5272_output.getInt32(j, 1);
          int64_t key = tbl_JOIN_INNER_TD_5272_output.getInt32(j, 2);
          tbl_JOIN_INNER_TD_5272_consolidated_output.setInt32(j, 0, in1.getInt32(rowid_2 - 1, 4));
          tbl_JOIN_INNER_TD_5272_consolidated_output.setInt32(j, 1, in1.getInt32(rowid_2 - 1, 0));
          tbl_JOIN_INNER_TD_5272_consolidated_output.setInt32(j, 2, in1.getInt32(rowid_2 - 1, 1));
          tbl_JOIN_INNER_TD_5272_consolidated_output.setInt32(j, 3, in1.getInt32(rowid_2 - 1, 2));
          tbl_JOIN_INNER_TD_5272_consolidated_output.setInt32(j, 4, in2.getInt32(rowid_1 - 1, 1));
          tbl_JOIN_INNER_TD_5272_consolidated_output.setInt32(j, 5, in1.getInt32(rowid_2 - 1, 3));
        }
        tbl_JOIN_INNER_TD_5272_consolidated_output.setNumRow(num_rows);
    std::cout << "tbl_JOIN_INNER_TD_5272_consolidated_output #Row: " << tbl_JOIN_INNER_TD_5272_consolidated_output.getNumRow() << std::endl;
}

void SW_Filter_TD_4468(Table &tbl_SerializeFromObject_TD_5307_input, Table &tbl_Filter_TD_4468_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: Filter
    // Operation: ListBuffer(((n_name#871 = IRAQ) OR (n_name#871 = FRANCE)))
    // Input: ListBuffer(n_nationkey#870, n_name#871)
    // Output: ListBuffer(n_nationkey#870, n_name#871)
    int r = 0;
    int nrow1 = tbl_SerializeFromObject_TD_5307_input.getNumRow();
    for (int i = 0; i < nrow1; i++) {
        std::array<char, TPCH_READ_NATION_LEN + 1> _n_name871 = tbl_SerializeFromObject_TD_5307_input.getcharN<char, TPCH_READ_NATION_LEN + 1>(i, 1);
        if ((std::string(_n_name871.data()) == "IRAQ") || (std::string(_n_name871.data()) == "FRANCE")) {
            int32_t _n_nationkey870_t = tbl_SerializeFromObject_TD_5307_input.getInt32(i, 0);
            tbl_Filter_TD_4468_output.setInt32(r, 0, _n_nationkey870_t);
            std::array<char, TPCH_READ_NATION_LEN + 1> _n_name871_t = tbl_SerializeFromObject_TD_5307_input.getcharN<char, TPCH_READ_NATION_LEN + 1>(i, 1);
            tbl_Filter_TD_4468_output.setcharN<char, TPCH_READ_NATION_LEN + 1>(r, 1, _n_name871_t);
            r++;
        }
    }
    tbl_Filter_TD_4468_output.setNumRow(r);
    std::cout << "tbl_Filter_TD_4468_output #Row: " << tbl_Filter_TD_4468_output.getNumRow() << std::endl;
}

void SW_Consolidated_JOIN_INNER_TD_4287_output(Table& in1, Table&in2, Table& tbl_JOIN_INNER_TD_4287_output, Table& tbl_JOIN_INNER_TD_4287_consolidated_output) {
    // StringRowIDSubstitution: true StringRowIDBackSubstitution: false
    // Supported operation: JOIN_INNER
    // Operation: ListBuffer((c_custkey#22 = o_custkey#208))
    // Left Table: ListBuffer(c_custkey#22, c_nationkey#25)
    // Right Table: ListBuffer(s_nationkey#345, l_extendedprice#82, l_discount#83, l_shipdate#87, o_custkey#208, n_name#145)
    // Output Table: ListBuffer(l_extendedprice#82, l_discount#83, l_shipdate#87, c_nationkey#25, n_name#145)
    // Node Depth: 4
    int num_rows = tbl_JOIN_INNER_TD_4287_output.getNumRow();
        for (int j = 0; j < num_rows; j++) {
          int32_t rowid_1 = tbl_JOIN_INNER_TD_4287_output.getInt32(j, 0);
          int32_t rowid_2 = tbl_JOIN_INNER_TD_4287_output.getInt32(j, 1);
          int64_t key = tbl_JOIN_INNER_TD_4287_output.getInt32(j, 2);
          tbl_JOIN_INNER_TD_4287_consolidated_output.setInt32(j, 0, in2.getInt32(rowid_1 - 1, 1));
          tbl_JOIN_INNER_TD_4287_consolidated_output.setInt32(j, 1, in2.getInt32(rowid_1 - 1, 2));
          tbl_JOIN_INNER_TD_4287_consolidated_output.setInt32(j, 2, in2.getInt32(rowid_1 - 1, 3));
          tbl_JOIN_INNER_TD_4287_consolidated_output.setInt32(j, 3, in1.getInt32(rowid_2 - 1, 1));
          tbl_JOIN_INNER_TD_4287_consolidated_output.setInt32(j, 4, in2.getInt32(rowid_1 - 1, 5));
        }
        tbl_JOIN_INNER_TD_4287_consolidated_output.setNumRow(num_rows);
    std::cout << "tbl_JOIN_INNER_TD_4287_consolidated_output #Row: " << tbl_JOIN_INNER_TD_4287_consolidated_output.getNumRow() << std::endl;
}

struct SW_JOIN_INNER_TD_3655_key_leftMajor {
    int32_t _c_nationkey25;
    bool operator==(const SW_JOIN_INNER_TD_3655_key_leftMajor& other) const {
        return ((_c_nationkey25 == other._c_nationkey25));
    }
};
namespace std {
template <>
struct hash<SW_JOIN_INNER_TD_3655_key_leftMajor> {
    std::size_t operator() (const SW_JOIN_INNER_TD_3655_key_leftMajor& k) const {
        using std::size_t;
        using std::hash;
        using std::string;
        return (hash<int32_t>()(k._c_nationkey25));
    }
};
}
struct SW_JOIN_INNER_TD_3655_payload_leftMajor {
    int32_t _l_extendedprice82;
    int32_t _l_discount83;
    int32_t _l_shipdate87;
    int32_t _c_nationkey25;
    std::string _n_name145;
};
struct SW_JOIN_INNER_TD_3655_key_rightMajor {
    int32_t _n_nationkey870;
    bool operator==(const SW_JOIN_INNER_TD_3655_key_rightMajor& other) const {
        return ((_n_nationkey870 == other._n_nationkey870));
    }
};
namespace std {
template <>
struct hash<SW_JOIN_INNER_TD_3655_key_rightMajor> {
    std::size_t operator() (const SW_JOIN_INNER_TD_3655_key_rightMajor& k) const {
        using std::size_t;
        using std::hash;
        using std::string;
        return (hash<int32_t>()(k._n_nationkey870));
    }
};
}
struct SW_JOIN_INNER_TD_3655_payload_rightMajor {
    int32_t _n_nationkey870;
    std::string _n_name871;
};
void SW_JOIN_INNER_TD_3655(Table &tbl_JOIN_INNER_TD_4287_consolidated_output, Table &tbl_Filter_TD_4468_output, Table &tbl_SerializeFromObject_TD_6527_input, Table &tbl_JOIN_INNER_TD_3655_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: true
    // Supported operation: JOIN_INNER
    // Operation: ListBuffer(((c_nationkey#25 = n_nationkey#870) AND (((n_name#145 = FRANCE) AND (n_name#871 = IRAQ)) OR ((n_name#145 = IRAQ) AND (n_name#871 = FRANCE)))))
    // Left Table: ListBuffer(l_extendedprice#82, l_discount#83, l_shipdate#87, c_nationkey#25, n_name#145)
    // Right Table: ListBuffer(n_nationkey#870, n_name#871)
    // Output Table: ListBuffer(n_name#145, n_name#871, l_shipdate#87, l_extendedprice#82, l_discount#83, c_nationkey#25)
    int left_nrow = tbl_JOIN_INNER_TD_4287_consolidated_output.getNumRow();
    int right_nrow = tbl_Filter_TD_4468_output.getNumRow();
    if (left_nrow < right_nrow) { 
        std::unordered_multimap<SW_JOIN_INNER_TD_3655_key_leftMajor, SW_JOIN_INNER_TD_3655_payload_leftMajor> ht1;
        int nrow1 = tbl_JOIN_INNER_TD_4287_consolidated_output.getNumRow();
        for (int i = 0; i < nrow1; i++) {
            int32_t _c_nationkey25_k = tbl_JOIN_INNER_TD_4287_consolidated_output.getInt32(i, 3);
            SW_JOIN_INNER_TD_3655_key_leftMajor keyA{_c_nationkey25_k};
            int32_t _l_extendedprice82 = tbl_JOIN_INNER_TD_4287_consolidated_output.getInt32(i, 0);
            int32_t _l_discount83 = tbl_JOIN_INNER_TD_4287_consolidated_output.getInt32(i, 1);
            int32_t _l_shipdate87 = tbl_JOIN_INNER_TD_4287_consolidated_output.getInt32(i, 2);
            int32_t _c_nationkey25 = tbl_JOIN_INNER_TD_4287_consolidated_output.getInt32(i, 3);
            std::array<char, TPCH_READ_NATION_LEN + 1> _n_name145_n = tbl_SerializeFromObject_TD_6527_input.getcharN<char, TPCH_READ_NATION_LEN + 1>(tbl_JOIN_INNER_TD_4287_consolidated_output.getInt32(i, 4), 1);
            std::string _n_name145 = std::string(_n_name145_n.data());
            SW_JOIN_INNER_TD_3655_payload_leftMajor payloadA{_l_extendedprice82, _l_discount83, _l_shipdate87, _c_nationkey25, _n_name145};
            ht1.insert(std::make_pair(keyA, payloadA));
        }
        int r = 0;
        int nrow2 = tbl_Filter_TD_4468_output.getNumRow();
        for (int i = 0; i < nrow2; i++) {
            int32_t _n_nationkey870_k = tbl_Filter_TD_4468_output.getInt32(i, 0);
            auto its = ht1.equal_range(SW_JOIN_INNER_TD_3655_key_leftMajor{_n_nationkey870_k});
            auto it = its.first;
            while (it != its.second) {
                int32_t _l_extendedprice82 = (it->second)._l_extendedprice82;
                int32_t _l_discount83 = (it->second)._l_discount83;
                int32_t _l_shipdate87 = (it->second)._l_shipdate87;
                int32_t _c_nationkey25 = (it->second)._c_nationkey25;
                std::string _n_name145 = (it->second)._n_name145;
                std::array<char, TPCH_READ_NATION_LEN + 1> _n_name145_n{};
                memcpy(_n_name145_n.data(), (_n_name145).data(), (_n_name145).length());
                int32_t _n_nationkey870 = tbl_Filter_TD_4468_output.getInt32(i, 0);
                std::array<char, TPCH_READ_NATION_LEN + 1> _n_name871_n = tbl_Filter_TD_4468_output.getcharN<char, TPCH_READ_NATION_LEN + 1>(i, 1);
                std::string _n_name871 = std::string(_n_name871_n.data());
                if (((_n_name145 == "FRANCE") && (_n_name871 == "IRAQ")) || ((_n_name145 == "IRAQ") && (_n_name871 == "FRANCE"))) {
                    tbl_JOIN_INNER_TD_3655_output.setInt32(r, 3, _l_extendedprice82);
                    tbl_JOIN_INNER_TD_3655_output.setInt32(r, 4, _l_discount83);
                    tbl_JOIN_INNER_TD_3655_output.setInt32(r, 2, _l_shipdate87);
                    tbl_JOIN_INNER_TD_3655_output.setInt32(r, 5, _c_nationkey25);
                    tbl_JOIN_INNER_TD_3655_output.setcharN<char, TPCH_READ_NATION_LEN + 1>(r, 0, _n_name145_n);
                    tbl_JOIN_INNER_TD_3655_output.setcharN<char, TPCH_READ_NATION_LEN + 1>(r, 1, _n_name871_n);
                    r++;
                }
                it++;
            }
        }
        tbl_JOIN_INNER_TD_3655_output.setNumRow(r);
    } else { 
        std::unordered_multimap<SW_JOIN_INNER_TD_3655_key_rightMajor, SW_JOIN_INNER_TD_3655_payload_rightMajor> ht1;
        int nrow1 = tbl_Filter_TD_4468_output.getNumRow();
        for (int i = 0; i < nrow1; i++) {
            int32_t _n_nationkey870_k = tbl_Filter_TD_4468_output.getInt32(i, 0);
            SW_JOIN_INNER_TD_3655_key_rightMajor keyA{_n_nationkey870_k};
            int32_t _n_nationkey870 = tbl_Filter_TD_4468_output.getInt32(i, 0);
            std::array<char, TPCH_READ_NATION_LEN + 1> _n_name871_n = tbl_Filter_TD_4468_output.getcharN<char, TPCH_READ_NATION_LEN + 1>(i, 1);
            std::string _n_name871 = std::string(_n_name871_n.data());
            SW_JOIN_INNER_TD_3655_payload_rightMajor payloadA{_n_nationkey870, _n_name871};
            ht1.insert(std::make_pair(keyA, payloadA));
        }
        int r = 0;
        int nrow2 = tbl_JOIN_INNER_TD_4287_consolidated_output.getNumRow();
        for (int i = 0; i < nrow2; i++) {
            int32_t _c_nationkey25_k = tbl_JOIN_INNER_TD_4287_consolidated_output.getInt32(i, 3);
            auto its = ht1.equal_range(SW_JOIN_INNER_TD_3655_key_rightMajor{_c_nationkey25_k});
            auto it = its.first;
            while (it != its.second) {
                int32_t _n_nationkey870 = (it->second)._n_nationkey870;
                std::string _n_name871 = (it->second)._n_name871;
                std::array<char, TPCH_READ_NATION_LEN + 1> _n_name871_n{};
                memcpy(_n_name871_n.data(), (_n_name871).data(), (_n_name871).length());
                int32_t _l_extendedprice82 = tbl_JOIN_INNER_TD_4287_consolidated_output.getInt32(i, 0);
                int32_t _l_discount83 = tbl_JOIN_INNER_TD_4287_consolidated_output.getInt32(i, 1);
                int32_t _l_shipdate87 = tbl_JOIN_INNER_TD_4287_consolidated_output.getInt32(i, 2);
                int32_t _c_nationkey25 = tbl_JOIN_INNER_TD_4287_consolidated_output.getInt32(i, 3);
            std::array<char, TPCH_READ_NATION_LEN + 1> _n_name145_n = tbl_SerializeFromObject_TD_6527_input.getcharN<char, TPCH_READ_NATION_LEN + 1>(tbl_JOIN_INNER_TD_4287_consolidated_output.getInt32(i, 4), 1);
            std::string _n_name145 = std::string(_n_name145_n.data());
                if (((_n_name145 == "FRANCE") && (_n_name871 == "IRAQ")) || ((_n_name145 == "IRAQ") && (_n_name871 == "FRANCE"))) {
                    tbl_JOIN_INNER_TD_3655_output.setcharN<char, TPCH_READ_NATION_LEN + 1>(r, 1, _n_name871_n);
                    tbl_JOIN_INNER_TD_3655_output.setInt32(r, 3, _l_extendedprice82);
                    tbl_JOIN_INNER_TD_3655_output.setInt32(r, 4, _l_discount83);
                    tbl_JOIN_INNER_TD_3655_output.setInt32(r, 2, _l_shipdate87);
                    tbl_JOIN_INNER_TD_3655_output.setInt32(r, 5, _c_nationkey25);
                    tbl_JOIN_INNER_TD_3655_output.setcharN<char, TPCH_READ_NATION_LEN + 1>(r, 0, _n_name145_n);
                    r++;
                }
                it++;
            }
        }
        tbl_JOIN_INNER_TD_3655_output.setNumRow(r);
    } 
    std::cout << "tbl_JOIN_INNER_TD_3655_output #Row: " << tbl_JOIN_INNER_TD_3655_output.getNumRow() << std::endl;
}

void SW_Project_TD_2215(Table &tbl_JOIN_INNER_TD_3655_output, Table &tbl_Project_TD_2215_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: Project
    // Operation: ListBuffer(n_name#145 AS supp_nation#864, n_name#871 AS cust_nation#865, cast((cast(l_shipdate#87 as double) / 10000.0) as int) AS l_year#866, (l_extendedprice#82 * (100 - l_discount#83)) AS volume#867)
    // Input: ListBuffer(n_name#145, n_name#871, l_shipdate#87, l_extendedprice#82, l_discount#83, c_nationkey#25)
    // Output: ListBuffer(supp_nation#864, cust_nation#865, l_year#866, volume#867)
    int nrow1 = tbl_JOIN_INNER_TD_3655_output.getNumRow();
    for (int i = 0; i < nrow1; i++) {
        std::array<char, TPCH_READ_NATION_LEN + 1> _n_name145 = tbl_JOIN_INNER_TD_3655_output.getcharN<char, TPCH_READ_NATION_LEN + 1>(i, 0);
        std::array<char, TPCH_READ_NATION_LEN + 1> _n_name871 = tbl_JOIN_INNER_TD_3655_output.getcharN<char, TPCH_READ_NATION_LEN + 1>(i, 1);
        int32_t _l_shipdate87 = tbl_JOIN_INNER_TD_3655_output.getInt32(i, 2);
        int32_t _l_extendedprice82 = tbl_JOIN_INNER_TD_3655_output.getInt32(i, 3);
        int32_t _l_discount83 = tbl_JOIN_INNER_TD_3655_output.getInt32(i, 4);
        int32_t _c_nationkey25 = tbl_JOIN_INNER_TD_3655_output.getInt32(i, 5);
        std::array<char, TPCH_READ_NATION_LEN + 1> _supp_nation864 = _n_name145;
        tbl_Project_TD_2215_output.setcharN<char, TPCH_READ_NATION_LEN + 1>(i, 0, _supp_nation864);
        std::array<char, TPCH_READ_NATION_LEN + 1> _cust_nation865 = _n_name871;
        tbl_Project_TD_2215_output.setcharN<char, TPCH_READ_NATION_LEN + 1>(i, 1, _cust_nation865);
        int32_t _l_year866 = (_l_shipdate87 / 10000.0);
        tbl_Project_TD_2215_output.setInt32(i, 2, _l_year866);
        int32_t _volume867 = (_l_extendedprice82 * (100 - _l_discount83));
        tbl_Project_TD_2215_output.setInt32(i, 3, _volume867);
    }
    tbl_Project_TD_2215_output.setNumRow(nrow1);
    std::cout << "tbl_Project_TD_2215_output #Row: " << tbl_Project_TD_2215_output.getNumRow() << std::endl;
}

struct SW_Aggregate_TD_1904_key {
    std::string _supp_nation864;
    std::string _cust_nation865;
    int32_t _l_year866;
    bool operator==(const SW_Aggregate_TD_1904_key& other) const { return (_supp_nation864 == other._supp_nation864) && (_cust_nation865 == other._cust_nation865) && (_l_year866 == other._l_year866); }
};
namespace std {
template <>
struct hash<SW_Aggregate_TD_1904_key> {
    std::size_t operator() (const SW_Aggregate_TD_1904_key& k) const {
        using std::size_t;
        using std::hash;
        using std::string;
        return (hash<string>()(k._supp_nation864)) + (hash<string>()(k._cust_nation865)) + (hash<int32_t>()(k._l_year866));
    }
};
}
struct SW_Aggregate_TD_1904_payload {
    int64_t _revenue868L_sum_0;
};
void SW_Aggregate_TD_1904(Table &tbl_Project_TD_2215_output, Table &tbl_Aggregate_TD_1904_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: Aggregate
    // Operation: ListBuffer(supp_nation#864, cust_nation#865, l_year#866, sum(volume#867) AS revenue#868L)
    // Input: ListBuffer(supp_nation#864, cust_nation#865, l_year#866, volume#867)
    // Output: ListBuffer(supp_nation#864, cust_nation#865, l_year#866, revenue#868L)
    std::unordered_map<SW_Aggregate_TD_1904_key, SW_Aggregate_TD_1904_payload> ht1;
    int nrow1 = tbl_Project_TD_2215_output.getNumRow();
    for (int i = 0; i < nrow1; i++) {
        std::array<char, TPCH_READ_NATION_LEN + 1> _supp_nation864 = tbl_Project_TD_2215_output.getcharN<char, TPCH_READ_NATION_LEN + 1>(i, 0);
        std::array<char, TPCH_READ_NATION_LEN + 1> _cust_nation865 = tbl_Project_TD_2215_output.getcharN<char, TPCH_READ_NATION_LEN + 1>(i, 1);
        int32_t _l_year866 = tbl_Project_TD_2215_output.getInt32(i, 2);
        int32_t _volume867 = tbl_Project_TD_2215_output.getInt32(i, 3);
        SW_Aggregate_TD_1904_key k{std::string(_supp_nation864.data()), std::string(_cust_nation865.data()), _l_year866};
        int64_t _revenue868L_sum_0 = _volume867;
        SW_Aggregate_TD_1904_payload p{_revenue868L_sum_0};
        auto it = ht1.find(k);
        if (it != ht1.end()) {
            int64_t sum_0 = (it->second)._revenue868L_sum_0 + _revenue868L_sum_0;
            p._revenue868L_sum_0 = sum_0;
            ht1[k] = p;
        } else { 
            ht1.insert(std::make_pair(k, p));
        }
    }
    int r = 0;
    for (auto& it : ht1) {
        std::array<char, TPCH_READ_NATION_LEN + 1> _supp_nation864{};
        memcpy(_supp_nation864.data(), ((it.first)._supp_nation864).data(), ((it.first)._supp_nation864).length());
        tbl_Aggregate_TD_1904_output.setcharN<char, TPCH_READ_NATION_LEN + 1>(r, 0, _supp_nation864);
        std::array<char, TPCH_READ_NATION_LEN + 1> _cust_nation865{};
        memcpy(_cust_nation865.data(), ((it.first)._cust_nation865).data(), ((it.first)._cust_nation865).length());
        tbl_Aggregate_TD_1904_output.setcharN<char, TPCH_READ_NATION_LEN + 1>(r, 1, _cust_nation865);
        tbl_Aggregate_TD_1904_output.setInt32(r, 2, (it.first)._l_year866);
        int64_t _revenue868L = (it.second)._revenue868L_sum_0;
        tbl_Aggregate_TD_1904_output.setInt64(r, 3, _revenue868L);
        ++r;
    }
    tbl_Aggregate_TD_1904_output.setNumRow(r);
    std::cout << "tbl_Aggregate_TD_1904_output #Row: " << tbl_Aggregate_TD_1904_output.getNumRow() << std::endl;
}

void SW_Sort_TD_0451(Table &tbl_Aggregate_TD_1904_output, Table &tbl_Sort_TD_0451_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: Sort
    // Operation: ListBuffer(supp_nation#864 ASC NULLS FIRST, cust_nation#865 ASC NULLS FIRST, l_year#866 ASC NULLS FIRST)
    // Input: ListBuffer(supp_nation#864, cust_nation#865, l_year#866, revenue#868L)
    // Output: ListBuffer(supp_nation#864, cust_nation#865, l_year#866, revenue#868L)
    struct SW_Sort_TD_0451Row {
        std::string _supp_nation864;
        std::string _cust_nation865;
        int32_t _l_year866;
        int64_t _revenue868L;
    }; 

    struct {
        bool operator()(const SW_Sort_TD_0451Row& a, const SW_Sort_TD_0451Row& b) const { return 
 (a._supp_nation864 < b._supp_nation864) || 
 ((a._supp_nation864 == b._supp_nation864) && (a._cust_nation865 < b._cust_nation865)) || 
 ((a._supp_nation864 == b._supp_nation864) && (a._cust_nation865 == b._cust_nation865) && (a._l_year866 < b._l_year866)); 
}
    }SW_Sort_TD_0451_order; 

    int nrow1 = tbl_Aggregate_TD_1904_output.getNumRow();
    std::vector<SW_Sort_TD_0451Row> rows;
    for (int i = 0; i < nrow1; i++) {
        std::array<char, TPCH_READ_NATION_LEN + 1> _supp_nation864 = tbl_Aggregate_TD_1904_output.getcharN<char, TPCH_READ_NATION_LEN +1>(i, 0);
        std::array<char, TPCH_READ_NATION_LEN + 1> _cust_nation865 = tbl_Aggregate_TD_1904_output.getcharN<char, TPCH_READ_NATION_LEN +1>(i, 1);
        int32_t _l_year866 = tbl_Aggregate_TD_1904_output.getInt32(i, 2);
        int64_t _revenue868L = tbl_Aggregate_TD_1904_output.getInt64(i, 3);
        SW_Sort_TD_0451Row t = {std::string(_supp_nation864.data()),std::string(_cust_nation865.data()),_l_year866,_revenue868L};
        rows.push_back(t);
    }
    std::sort(rows.begin(), rows.end(), SW_Sort_TD_0451_order);
    int r = 0;
    for (auto& it : rows) {
        std::array<char, TPCH_READ_NATION_LEN + 1> _supp_nation864{};
        memcpy(_supp_nation864.data(), (it._supp_nation864).data(), (it._supp_nation864).length());
        tbl_Sort_TD_0451_output.setcharN<char, TPCH_READ_NATION_LEN +1>(r, 0, _supp_nation864);
        std::array<char, TPCH_READ_NATION_LEN + 1> _cust_nation865{};
        memcpy(_cust_nation865.data(), (it._cust_nation865).data(), (it._cust_nation865).length());
        tbl_Sort_TD_0451_output.setcharN<char, TPCH_READ_NATION_LEN +1>(r, 1, _cust_nation865);
        tbl_Sort_TD_0451_output.setInt32(r, 2, it._l_year866);
        tbl_Sort_TD_0451_output.setInt64(r, 3, it._revenue868L);
        if (r < 10) {
            std::cout << (it._supp_nation864).data() << " " << (it._cust_nation865).data() << " " << it._l_year866 << " " << it._revenue868L << " " << std::endl;
        }
        ++r;
    }
    tbl_Sort_TD_0451_output.setNumRow(r);
    std::cout << "tbl_Sort_TD_0451_output #Row: " << tbl_Sort_TD_0451_output.getNumRow() << std::endl;
}

