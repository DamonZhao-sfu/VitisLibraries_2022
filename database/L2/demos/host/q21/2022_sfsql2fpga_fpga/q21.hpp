#include <regex> 
#include <stdint.h> 

void SW_Filter_TD_8644(Table &tbl_SerializeFromObject_TD_9935_input, Table &tbl_Filter_TD_8644_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: Filter
    // Operation: ListBuffer((l_receiptdate#89 > l_commitdate#88))
    // Input: ListBuffer(l_orderkey#77, l_suppkey#79, l_receiptdate#89, l_commitdate#88)
    // Output: ListBuffer(l_orderkey#77, l_suppkey#79)
    int r = 0;
    int nrow1 = tbl_SerializeFromObject_TD_9935_input.getNumRow();
    for (int i = 0; i < nrow1; i++) {
        int32_t _l_receiptdate89 = tbl_SerializeFromObject_TD_9935_input.getInt32(i, 2);
        int32_t _l_commitdate88 = tbl_SerializeFromObject_TD_9935_input.getInt32(i, 3);
        if (_l_receiptdate89 > _l_commitdate88) {
            int32_t _l_orderkey77_t = tbl_SerializeFromObject_TD_9935_input.getInt32(i, 0);
            tbl_Filter_TD_8644_output.setInt32(r, 0, _l_orderkey77_t);
            int32_t _l_suppkey79_t = tbl_SerializeFromObject_TD_9935_input.getInt32(i, 1);
            tbl_Filter_TD_8644_output.setInt32(r, 1, _l_suppkey79_t);
            r++;
        }
    }
    tbl_Filter_TD_8644_output.setNumRow(r);
    std::cout << "tbl_Filter_TD_8644_output #Row: " << tbl_Filter_TD_8644_output.getNumRow() << std::endl;
}

void SW_Filter_TD_7203(Table &tbl_SerializeFromObject_TD_9935_input, Table &tbl_Filter_TD_7203_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: Filter
    // Operation: ListBuffer((l_receiptdate#89 > l_commitdate#88))
    // Input: ListBuffer(l_orderkey#77, l_suppkey#79, l_receiptdate#89, l_commitdate#88)
    // Output: ListBuffer(l_orderkey#77#2010, l_suppkey#79#2012)
    int r = 0;
    int nrow1 = tbl_SerializeFromObject_TD_9935_input.getNumRow();
    for (int i = 0; i < nrow1; i++) {
        int32_t _l_receiptdate89 = tbl_SerializeFromObject_TD_9935_input.getInt32(i, 2);
        int32_t _l_commitdate88 = tbl_SerializeFromObject_TD_9935_input.getInt32(i, 3);
        if (_l_receiptdate89 > _l_commitdate88) {
            int32_t _l_orderkey772010_t = tbl_SerializeFromObject_TD_9935_input.getInt32(i, 0);
            tbl_Filter_TD_7203_output.setInt32(r, 0, _l_orderkey772010_t);
            int32_t _l_suppkey792012_t = tbl_SerializeFromObject_TD_9935_input.getInt32(i, 1);
            tbl_Filter_TD_7203_output.setInt32(r, 1, _l_suppkey792012_t);
            r++;
        }
    }
    tbl_Filter_TD_7203_output.setNumRow(r);
    std::cout << "tbl_Filter_TD_7203_output #Row: " << tbl_Filter_TD_7203_output.getNumRow() << std::endl;
}

void SW_Consolidated_JOIN_LEFTSEMI_TD_7346_output(Table& in1, Table&in2, Table& tbl_JOIN_LEFTSEMI_TD_7346_output, Table& tbl_JOIN_LEFTSEMI_TD_7346_consolidated_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: JOIN_LEFTSEMI
    // Operation: ListBuffer(((l_orderkey#77#1994 = l_orderkey#77) AND NOT (l_suppkey#79#1996 = l_suppkey#79)))
    // Left Table: ListBuffer(l_orderkey#77#1994, l_suppkey#79#1996)
    // Right Table: ListBuffer(l_orderkey#77, l_suppkey#79)
    // Output Table: ListBuffer(l_orderkey#77, l_suppkey#79)
    // Node Depth: 7
    int num_rows = tbl_JOIN_LEFTSEMI_TD_7346_output.getNumRow();
        for (int j = 0; j < num_rows; j++) {
          int32_t rowid_1 = tbl_JOIN_LEFTSEMI_TD_7346_output.getInt32(j, 0);
          int32_t rowid_2 = tbl_JOIN_LEFTSEMI_TD_7346_output.getInt32(j, 1);
          int64_t key = tbl_JOIN_LEFTSEMI_TD_7346_output.getInt32(j, 2);
          tbl_JOIN_LEFTSEMI_TD_7346_consolidated_output.setInt32(j, 0, in2.getInt32(rowid_1 - 1, 0));
          tbl_JOIN_LEFTSEMI_TD_7346_consolidated_output.setInt32(j, 1, in2.getInt32(rowid_1 - 1, 1));
        }
        tbl_JOIN_LEFTSEMI_TD_7346_consolidated_output.setNumRow(num_rows);
    std::cout << "tbl_JOIN_LEFTSEMI_TD_7346_consolidated_output #Row: " << tbl_JOIN_LEFTSEMI_TD_7346_consolidated_output.getNumRow() << std::endl;
}

void SW_Consolidated_JOIN_LEFTSEMI_TD_6827_output(Table& in1, Table&in2, Table& tbl_JOIN_LEFTSEMI_TD_6827_output, Table& tbl_JOIN_LEFTSEMI_TD_6827_consolidated_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: JOIN_LEFTSEMI
    // Operation: ListBuffer(((l_orderkey#77#2010 = l_orderkey#77) AND NOT (l_suppkey#79#2012 = l_suppkey#79)))
    // Left Table: ListBuffer(l_orderkey#77#2010, l_suppkey#79#2012)
    // Right Table: ListBuffer(l_orderkey#77, l_suppkey#79)
    // Output Table: ListBuffer(l_orderkey#77, l_suppkey#79)
    // Node Depth: 6
    int num_rows = tbl_JOIN_LEFTSEMI_TD_6827_output.getNumRow();
        for (int j = 0; j < num_rows; j++) {
          int32_t rowid_1 = tbl_JOIN_LEFTSEMI_TD_6827_output.getInt32(j, 0);
          int32_t rowid_2 = tbl_JOIN_LEFTSEMI_TD_6827_output.getInt32(j, 1);
          int64_t key = tbl_JOIN_LEFTSEMI_TD_6827_output.getInt32(j, 2);
          tbl_JOIN_LEFTSEMI_TD_6827_consolidated_output.setInt32(j, 0, in2.getInt32(rowid_1 - 1, 0));
          tbl_JOIN_LEFTSEMI_TD_6827_consolidated_output.setInt32(j, 1, in2.getInt32(rowid_1 - 1, 1));
        }
        tbl_JOIN_LEFTSEMI_TD_6827_consolidated_output.setNumRow(num_rows);
    std::cout << "tbl_JOIN_LEFTSEMI_TD_6827_consolidated_output #Row: " << tbl_JOIN_LEFTSEMI_TD_6827_consolidated_output.getNumRow() << std::endl;
}

void SW_Filter_TD_3611(Table &tbl_SerializeFromObject_TD_4935_input, Table &tbl_Filter_TD_3611_output) {
    // StringRowIDSubstitution: true StringRowIDBackSubstitution: false
    // Supported operation: Filter
    // Operation: ListBuffer((isnotnull(n_name#145) AND (n_name#145 = JAPAN)))
    // Input: ListBuffer(n_nationkey#144, n_name#145)
    // Output: ListBuffer(n_nationkey#144)
    int r = 0;
    int nrow1 = tbl_SerializeFromObject_TD_4935_input.getNumRow();
    for (int i = 0; i < nrow1; i++) {
        std::array<char, TPCH_READ_NATION_LEN + 1> _n_name145 = tbl_SerializeFromObject_TD_4935_input.getcharN<char, TPCH_READ_NATION_LEN + 1>(i, 1);
        if ((1) && (std::string(_n_name145.data()) == "JAPAN")) {
            int32_t _n_nationkey144_t = tbl_SerializeFromObject_TD_4935_input.getInt32(i, 0);
            tbl_Filter_TD_3611_output.setInt32(r, 0, _n_nationkey144_t);
            r++;
        }
    }
    tbl_Filter_TD_3611_output.setNumRow(r);
    std::cout << "tbl_Filter_TD_3611_output #Row: " << tbl_Filter_TD_3611_output.getNumRow() << std::endl;
}

void SW_Consolidated_JOIN_LEFTANTI_TD_5733_output(Table& in1, Table&in2, Table& tbl_JOIN_LEFTANTI_TD_5733_output, Table& tbl_JOIN_LEFTANTI_TD_5733_consolidated_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: JOIN_LEFTANTI
    // Operation: ListBuffer(((l_orderkey#77#2010 = l_orderkey#77) AND NOT (l_suppkey#79#2012 = l_suppkey#79)))
    // Left Table: ListBuffer(l_orderkey#77, l_suppkey#79)
    // Right Table: ListBuffer(l_orderkey#77#2010, l_suppkey#79#2012)
    // Output Table: ListBuffer(l_orderkey#77, l_suppkey#79)
    // Node Depth: 5
    int num_rows = tbl_JOIN_LEFTANTI_TD_5733_output.getNumRow();
        for (int j = 0; j < num_rows; j++) {
          int32_t rowid_1 = tbl_JOIN_LEFTANTI_TD_5733_output.getInt32(j, 0);
          int32_t rowid_2 = tbl_JOIN_LEFTANTI_TD_5733_output.getInt32(j, 1);
          int64_t key = tbl_JOIN_LEFTANTI_TD_5733_output.getInt32(j, 2);
          tbl_JOIN_LEFTANTI_TD_5733_consolidated_output.setInt32(j, 0, in1.getInt32(rowid_2 - 1, 0));
          tbl_JOIN_LEFTANTI_TD_5733_consolidated_output.setInt32(j, 1, in1.getInt32(rowid_2 - 1, 1));
        }
        tbl_JOIN_LEFTANTI_TD_5733_consolidated_output.setNumRow(num_rows);
    std::cout << "tbl_JOIN_LEFTANTI_TD_5733_consolidated_output #Row: " << tbl_JOIN_LEFTANTI_TD_5733_consolidated_output.getNumRow() << std::endl;
}

void SW_Consolidated_JOIN_INNER_TD_3653_output(Table& in1, Table&in2, Table& tbl_JOIN_INNER_TD_3653_output, Table& tbl_JOIN_INNER_TD_3653_consolidated_output) {
    // StringRowIDSubstitution: true StringRowIDBackSubstitution: false
    // Supported operation: JOIN_INNER
    // Operation: ListBuffer((s_nationkey#345 = n_nationkey#144))
    // Left Table: ListBuffer(s_suppkey#342, s_name#343, s_nationkey#345)
    // Right Table: ListBuffer(n_nationkey#144)
    // Output Table: ListBuffer(s_suppkey#342, s_name#343)
    // Node Depth: 3
    int num_rows = tbl_JOIN_INNER_TD_3653_output.getNumRow();
        for (int j = 0; j < num_rows; j++) {
          int32_t rowid_1 = tbl_JOIN_INNER_TD_3653_output.getInt32(j, 0);
          int32_t rowid_2 = tbl_JOIN_INNER_TD_3653_output.getInt32(j, 1);
          int64_t key = tbl_JOIN_INNER_TD_3653_output.getInt32(j, 2);
          tbl_JOIN_INNER_TD_3653_consolidated_output.setInt32(j, 0, in1.getInt32(rowid_2 - 1, 0));
          tbl_JOIN_INNER_TD_3653_consolidated_output.setInt32(j, 1, in1.getInt32(rowid_2 - 1, 1));
        }
        tbl_JOIN_INNER_TD_3653_consolidated_output.setNumRow(num_rows);
    std::cout << "tbl_JOIN_INNER_TD_3653_consolidated_output #Row: " << tbl_JOIN_INNER_TD_3653_consolidated_output.getNumRow() << std::endl;
}

void SW_Consolidated_JOIN_INNER_TD_4616_output(Table& in1, Table&in2, Table& tbl_JOIN_INNER_TD_4616_output, Table& tbl_JOIN_INNER_TD_4616_consolidated_output) {
    // StringRowIDSubstitution: true StringRowIDBackSubstitution: false
    // Supported operation: JOIN_INNER
    // Operation: ListBuffer((s_suppkey#342 = l_suppkey#79))
    // Left Table: ListBuffer(s_suppkey#342, s_name#343)
    // Right Table: ListBuffer(l_orderkey#77, l_suppkey#79)
    // Output Table: ListBuffer(s_name#343, l_orderkey#77)
    // Node Depth: 4
    int num_rows = tbl_JOIN_INNER_TD_4616_output.getNumRow();
        for (int j = 0; j < num_rows; j++) {
          int32_t rowid_1 = tbl_JOIN_INNER_TD_4616_output.getInt32(j, 0);
          int32_t rowid_2 = tbl_JOIN_INNER_TD_4616_output.getInt32(j, 1);
          int64_t key = tbl_JOIN_INNER_TD_4616_output.getInt32(j, 2);
          tbl_JOIN_INNER_TD_4616_consolidated_output.setInt32(j, 0, in1.getInt32(rowid_2 - 1, 1));
          tbl_JOIN_INNER_TD_4616_consolidated_output.setInt32(j, 1, in2.getInt32(rowid_1 - 1, 0));
        }
        tbl_JOIN_INNER_TD_4616_consolidated_output.setNumRow(num_rows);
    std::cout << "tbl_JOIN_INNER_TD_4616_consolidated_output #Row: " << tbl_JOIN_INNER_TD_4616_consolidated_output.getNumRow() << std::endl;
}

void SW_Filter_TD_3321(Table &tbl_SerializeFromObject_TD_5180_input, Table &tbl_Filter_TD_3321_output) {
    // StringRowIDSubstitution: true StringRowIDBackSubstitution: false
    // Supported operation: Filter
    // Operation: ListBuffer((o_orderstatus#209 = 70))
    // Input: ListBuffer(o_orderkey#207, o_orderstatus#209)
    // Output: ListBuffer(o_orderkey#207)
    int r = 0;
    int nrow1 = tbl_SerializeFromObject_TD_5180_input.getNumRow();
    for (int i = 0; i < nrow1; i++) {
        int32_t _o_orderstatus209 = tbl_SerializeFromObject_TD_5180_input.getInt32(i, 1);
        if (_o_orderstatus209 == 70) {
            int32_t _o_orderkey207_t = tbl_SerializeFromObject_TD_5180_input.getInt32(i, 0);
            tbl_Filter_TD_3321_output.setInt32(r, 0, _o_orderkey207_t);
            r++;
        }
    }
    tbl_Filter_TD_3321_output.setNumRow(r);
    std::cout << "tbl_Filter_TD_3321_output #Row: " << tbl_Filter_TD_3321_output.getNumRow() << std::endl;
}

void SW_Consolidated_JOIN_INNER_TD_2684_output(Table& in1, Table&in2, Table& tbl_JOIN_INNER_TD_2684_output, Table& tbl_JOIN_INNER_TD_2684_consolidated_output) {
    // StringRowIDSubstitution: true StringRowIDBackSubstitution: false
    // Supported operation: JOIN_INNER
    // Operation: ListBuffer((o_orderkey#207 = l_orderkey#77))
    // Left Table: ListBuffer(s_name#343, l_orderkey#77)
    // Right Table: ListBuffer(o_orderkey#207)
    // Output Table: ListBuffer(s_name#343)
    // Node Depth: 2
    int num_rows = tbl_JOIN_INNER_TD_2684_output.getNumRow();
        for (int j = 0; j < num_rows; j++) {
          int32_t rowid_1 = tbl_JOIN_INNER_TD_2684_output.getInt32(j, 0);
          int32_t rowid_2 = tbl_JOIN_INNER_TD_2684_output.getInt32(j, 1);
          int64_t key = tbl_JOIN_INNER_TD_2684_output.getInt32(j, 2);
          tbl_JOIN_INNER_TD_2684_consolidated_output.setInt32(j, 0, in1.getInt32(rowid_2 - 1, 0));
        }
        tbl_JOIN_INNER_TD_2684_consolidated_output.setNumRow(num_rows);
    std::cout << "tbl_JOIN_INNER_TD_2684_consolidated_output #Row: " << tbl_JOIN_INNER_TD_2684_consolidated_output.getNumRow() << std::endl;
}

typedef std::string SW_Aggregate_TD_1648_key;
struct SW_Aggregate_TD_1648_payload {
    int64_t _numwait1897L_count_0;
};
void SW_Aggregate_TD_1648(Table *tbl_JOIN_INNER_TD_2684_consolidated_output, Table &tbl_SerializeFromObject_TD_5180_input, Table &tbl_SerializeFromObject_TD_5188_input, Table &tbl_SerializeFromObject_TD_4935_input, Table &tbl_Aggregate_TD_1648_output, int hpTimes) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: true
    // Supported operation: Aggregate
    // Operation: ListBuffer(s_name#343, count(1) AS numwait#1897L)
    // Input: ListBuffer(s_name#343)
    // Output: ListBuffer(s_name#343, numwait#1897L)
    std::unordered_map<SW_Aggregate_TD_1648_key, SW_Aggregate_TD_1648_payload> ht1;
for (int p_idx = 0; p_idx < hpTimes; p_idx++) {
    int nrow1 = tbl_JOIN_INNER_TD_2684_consolidated_output[p_idx].getNumRow();
    for (int i = 0; i < nrow1; i++) {
        std::array<char, TPCH_READ_S_NAME_LEN + 1> _s_name343 = tbl_SerializeFromObject_TD_5188_input.getcharN<char, TPCH_READ_S_NAME_LEN + 1>(tbl_JOIN_INNER_TD_2684_consolidated_output[p_idx].getInt32(i, 0), 1);
        SW_Aggregate_TD_1648_key k = std::string(_s_name343.data());
        int64_t _numwait1897L_count_0 = 1 != 0 ? 1 : 0;
        SW_Aggregate_TD_1648_payload p{_numwait1897L_count_0};
        auto it = ht1.find(k);
        if (it != ht1.end()) {
            int64_t count_0 = (it->second)._numwait1897L_count_0 + _numwait1897L_count_0;
            p._numwait1897L_count_0 = count_0;
            ht1[k] = p;
        } else { 
            ht1.insert(std::make_pair(k, p));
        }
    }
}
    int r = 0;
    for (auto& it : ht1) {
        std::array<char, TPCH_READ_S_NAME_LEN + 1> _s_name343{};
        memcpy(_s_name343.data(), (it.first).data(), (it.first).length());
        tbl_Aggregate_TD_1648_output.setcharN<char, TPCH_READ_S_NAME_LEN + 1>(r, 0, _s_name343);
        int64_t _numwait1897L = (it.second)._numwait1897L_count_0;
        tbl_Aggregate_TD_1648_output.setInt64(r, 1, _numwait1897L);
        ++r;
    }
    tbl_Aggregate_TD_1648_output.setNumRow(r);
    std::cout << "tbl_Aggregate_TD_1648_output #Row: " << tbl_Aggregate_TD_1648_output.getNumRow() << std::endl;
}

void SW_Sort_TD_0482(Table &tbl_Aggregate_TD_1648_output, Table &tbl_Sort_TD_0482_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: Sort
    // Operation: ListBuffer(numwait#1897L DESC NULLS LAST, s_name#343 ASC NULLS FIRST)
    // Input: ListBuffer(s_name#343, numwait#1897L)
    // Output: ListBuffer(s_name#343, numwait#1897L)
    struct SW_Sort_TD_0482Row {
        std::string _s_name343;
        int64_t _numwait1897L;
    }; 

    struct {
        bool operator()(const SW_Sort_TD_0482Row& a, const SW_Sort_TD_0482Row& b) const { return 
 (a._numwait1897L > b._numwait1897L) || 
 ((a._numwait1897L == b._numwait1897L) && (a._s_name343 < b._s_name343)); 
}
    }SW_Sort_TD_0482_order; 

    int nrow1 = tbl_Aggregate_TD_1648_output.getNumRow();
    std::vector<SW_Sort_TD_0482Row> rows;
    for (int i = 0; i < nrow1; i++) {
        std::array<char, TPCH_READ_S_NAME_LEN + 1> _s_name343 = tbl_Aggregate_TD_1648_output.getcharN<char, TPCH_READ_S_NAME_LEN +1>(i, 0);
        int64_t _numwait1897L = tbl_Aggregate_TD_1648_output.getInt64(i, 1);
        SW_Sort_TD_0482Row t = {std::string(_s_name343.data()),_numwait1897L};
        rows.push_back(t);
    }
    std::sort(rows.begin(), rows.end(), SW_Sort_TD_0482_order);
    int r = 0;
    for (auto& it : rows) {
        std::array<char, TPCH_READ_S_NAME_LEN + 1> _s_name343{};
        memcpy(_s_name343.data(), (it._s_name343).data(), (it._s_name343).length());
        tbl_Sort_TD_0482_output.setcharN<char, TPCH_READ_S_NAME_LEN +1>(r, 0, _s_name343);
        tbl_Sort_TD_0482_output.setInt64(r, 1, it._numwait1897L);
        if (r < 10) {
            std::cout << (it._s_name343).data() << " " << it._numwait1897L << " " << std::endl;
        }
        ++r;
    }
    tbl_Sort_TD_0482_output.setNumRow(r);
    std::cout << "tbl_Sort_TD_0482_output #Row: " << tbl_Sort_TD_0482_output.getNumRow() << std::endl;
}

