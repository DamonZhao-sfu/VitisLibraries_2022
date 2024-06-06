#include <regex> 
#include <stdint.h> 

void SW_Filter_TD_8737(Table &tbl_SerializeFromObject_TD_9761_input, Table &tbl_Filter_TD_8737_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: Filter
    // Operation: ListBuffer((l_receiptdate#89 > l_commitdate#88))
    // Input: ListBuffer(l_orderkey#77, l_suppkey#79, l_receiptdate#89, l_commitdate#88)
    // Output: ListBuffer(l_orderkey#77, l_suppkey#79)
    int r = 0;
    int nrow1 = tbl_SerializeFromObject_TD_9761_input.getNumRow();
    for (int i = 0; i < nrow1; i++) {
        int32_t _l_receiptdate89 = tbl_SerializeFromObject_TD_9761_input.getInt32(i, 2);
        int32_t _l_commitdate88 = tbl_SerializeFromObject_TD_9761_input.getInt32(i, 3);
        if (_l_receiptdate89 > _l_commitdate88) {
            int32_t _l_orderkey77_t = tbl_SerializeFromObject_TD_9761_input.getInt32(i, 0);
            tbl_Filter_TD_8737_output.setInt32(r, 0, _l_orderkey77_t);
            int32_t _l_suppkey79_t = tbl_SerializeFromObject_TD_9761_input.getInt32(i, 1);
            tbl_Filter_TD_8737_output.setInt32(r, 1, _l_suppkey79_t);
            r++;
        }
    }
    tbl_Filter_TD_8737_output.setNumRow(r);
    std::cout << "tbl_Filter_TD_8737_output #Row: " << tbl_Filter_TD_8737_output.getNumRow() << std::endl;
}

void SW_Filter_TD_7740(Table &tbl_SerializeFromObject_TD_9761_input, Table &tbl_Filter_TD_7740_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: Filter
    // Operation: ListBuffer((l_receiptdate#89 > l_commitdate#88))
    // Input: ListBuffer(l_orderkey#77, l_suppkey#79, l_receiptdate#89, l_commitdate#88)
    // Output: ListBuffer(l_orderkey#77#2013, l_suppkey#79#2015)
    int r = 0;
    int nrow1 = tbl_SerializeFromObject_TD_9761_input.getNumRow();
    for (int i = 0; i < nrow1; i++) {
        int32_t _l_receiptdate89 = tbl_SerializeFromObject_TD_9761_input.getInt32(i, 2);
        int32_t _l_commitdate88 = tbl_SerializeFromObject_TD_9761_input.getInt32(i, 3);
        if (_l_receiptdate89 > _l_commitdate88) {
            int32_t _l_orderkey772013_t = tbl_SerializeFromObject_TD_9761_input.getInt32(i, 0);
            tbl_Filter_TD_7740_output.setInt32(r, 0, _l_orderkey772013_t);
            int32_t _l_suppkey792015_t = tbl_SerializeFromObject_TD_9761_input.getInt32(i, 1);
            tbl_Filter_TD_7740_output.setInt32(r, 1, _l_suppkey792015_t);
            r++;
        }
    }
    tbl_Filter_TD_7740_output.setNumRow(r);
    std::cout << "tbl_Filter_TD_7740_output #Row: " << tbl_Filter_TD_7740_output.getNumRow() << std::endl;
}

void SW_Consolidated_JOIN_LEFTSEMI_TD_7524_output(Table& in1, Table&in2, Table& tbl_JOIN_LEFTSEMI_TD_7524_output, Table& tbl_JOIN_LEFTSEMI_TD_7524_consolidated_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: JOIN_LEFTSEMI
    // Operation: ListBuffer(((l_orderkey#77#1997 = l_orderkey#77) AND NOT (l_suppkey#79#1999 = l_suppkey#79)))
    // Left Table: ListBuffer(l_orderkey#77#1997, l_suppkey#79#1999)
    // Right Table: ListBuffer(l_orderkey#77, l_suppkey#79)
    // Output Table: ListBuffer(l_orderkey#77, l_suppkey#79)
    // Node Depth: 7
    int num_rows = tbl_JOIN_LEFTSEMI_TD_7524_output.getNumRow();
        for (int j = 0; j < num_rows; j++) {
          int32_t rowid_1 = tbl_JOIN_LEFTSEMI_TD_7524_output.getInt32(j, 0);
          int32_t rowid_2 = tbl_JOIN_LEFTSEMI_TD_7524_output.getInt32(j, 1);
          int64_t key = tbl_JOIN_LEFTSEMI_TD_7524_output.getInt32(j, 2);
          tbl_JOIN_LEFTSEMI_TD_7524_consolidated_output.setInt32(j, 0, in2.getInt32(rowid_1 - 1, 0));
          tbl_JOIN_LEFTSEMI_TD_7524_consolidated_output.setInt32(j, 1, in2.getInt32(rowid_1 - 1, 1));
        }
        tbl_JOIN_LEFTSEMI_TD_7524_consolidated_output.setNumRow(num_rows);
    std::cout << "tbl_JOIN_LEFTSEMI_TD_7524_consolidated_output #Row: " << tbl_JOIN_LEFTSEMI_TD_7524_consolidated_output.getNumRow() << std::endl;
}

void SW_Consolidated_JOIN_LEFTSEMI_TD_6755_output(Table& in1, Table&in2, Table& tbl_JOIN_LEFTSEMI_TD_6755_output, Table& tbl_JOIN_LEFTSEMI_TD_6755_consolidated_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: JOIN_LEFTSEMI
    // Operation: ListBuffer(((l_orderkey#77#2013 = l_orderkey#77) AND NOT (l_suppkey#79#2015 = l_suppkey#79)))
    // Left Table: ListBuffer(l_orderkey#77#2013, l_suppkey#79#2015)
    // Right Table: ListBuffer(l_orderkey#77, l_suppkey#79)
    // Output Table: ListBuffer(l_orderkey#77, l_suppkey#79)
    // Node Depth: 6
    int num_rows = tbl_JOIN_LEFTSEMI_TD_6755_output.getNumRow();
        for (int j = 0; j < num_rows; j++) {
          int32_t rowid_1 = tbl_JOIN_LEFTSEMI_TD_6755_output.getInt32(j, 0);
          int32_t rowid_2 = tbl_JOIN_LEFTSEMI_TD_6755_output.getInt32(j, 1);
          int64_t key = tbl_JOIN_LEFTSEMI_TD_6755_output.getInt32(j, 2);
          tbl_JOIN_LEFTSEMI_TD_6755_consolidated_output.setInt32(j, 0, in2.getInt32(rowid_1 - 1, 0));
          tbl_JOIN_LEFTSEMI_TD_6755_consolidated_output.setInt32(j, 1, in2.getInt32(rowid_1 - 1, 1));
        }
        tbl_JOIN_LEFTSEMI_TD_6755_consolidated_output.setNumRow(num_rows);
    std::cout << "tbl_JOIN_LEFTSEMI_TD_6755_consolidated_output #Row: " << tbl_JOIN_LEFTSEMI_TD_6755_consolidated_output.getNumRow() << std::endl;
}

void SW_Filter_TD_3765(Table &tbl_SerializeFromObject_TD_4885_input, Table &tbl_Filter_TD_3765_output) {
    // StringRowIDSubstitution: true StringRowIDBackSubstitution: false
    // Supported operation: Filter
    // Operation: ListBuffer((isnotnull(n_name#145) AND (n_name#145 = JAPAN)))
    // Input: ListBuffer(n_nationkey#144, n_name#145)
    // Output: ListBuffer(n_nationkey#144)
    int r = 0;
    int nrow1 = tbl_SerializeFromObject_TD_4885_input.getNumRow();
    for (int i = 0; i < nrow1; i++) {
        std::array<char, TPCH_READ_NATION_LEN + 1> _n_name145 = tbl_SerializeFromObject_TD_4885_input.getcharN<char, TPCH_READ_NATION_LEN + 1>(i, 1);
        if ((1) && (std::string(_n_name145.data()) == "JAPAN")) {
            int32_t _n_nationkey144_t = tbl_SerializeFromObject_TD_4885_input.getInt32(i, 0);
            tbl_Filter_TD_3765_output.setInt32(r, 0, _n_nationkey144_t);
            r++;
        }
    }
    tbl_Filter_TD_3765_output.setNumRow(r);
    std::cout << "tbl_Filter_TD_3765_output #Row: " << tbl_Filter_TD_3765_output.getNumRow() << std::endl;
}

void SW_Consolidated_JOIN_LEFTANTI_TD_5488_output(Table& in1, Table&in2, Table& tbl_JOIN_LEFTANTI_TD_5488_output, Table& tbl_JOIN_LEFTANTI_TD_5488_consolidated_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: JOIN_LEFTANTI
    // Operation: ListBuffer(((l_orderkey#77#2013 = l_orderkey#77) AND NOT (l_suppkey#79#2015 = l_suppkey#79)))
    // Left Table: ListBuffer(l_orderkey#77, l_suppkey#79)
    // Right Table: ListBuffer(l_orderkey#77#2013, l_suppkey#79#2015)
    // Output Table: ListBuffer(l_orderkey#77, l_suppkey#79)
    // Node Depth: 5
    int num_rows = tbl_JOIN_LEFTANTI_TD_5488_output.getNumRow();
        for (int j = 0; j < num_rows; j++) {
          int32_t rowid_1 = tbl_JOIN_LEFTANTI_TD_5488_output.getInt32(j, 0);
          int32_t rowid_2 = tbl_JOIN_LEFTANTI_TD_5488_output.getInt32(j, 1);
          int64_t key = tbl_JOIN_LEFTANTI_TD_5488_output.getInt32(j, 2);
          tbl_JOIN_LEFTANTI_TD_5488_consolidated_output.setInt32(j, 0, in1.getInt32(rowid_2 - 1, 0));
          tbl_JOIN_LEFTANTI_TD_5488_consolidated_output.setInt32(j, 1, in1.getInt32(rowid_2 - 1, 1));
        }
        tbl_JOIN_LEFTANTI_TD_5488_consolidated_output.setNumRow(num_rows);
    std::cout << "tbl_JOIN_LEFTANTI_TD_5488_consolidated_output #Row: " << tbl_JOIN_LEFTANTI_TD_5488_consolidated_output.getNumRow() << std::endl;
}

void SW_Consolidated_JOIN_INNER_TD_3454_output(Table& in1, Table&in2, Table& tbl_JOIN_INNER_TD_3454_output, Table& tbl_JOIN_INNER_TD_3454_consolidated_output) {
    // StringRowIDSubstitution: true StringRowIDBackSubstitution: false
    // Supported operation: JOIN_INNER
    // Operation: ListBuffer((s_nationkey#345 = n_nationkey#144))
    // Left Table: ListBuffer(s_suppkey#342, s_name#343, s_nationkey#345)
    // Right Table: ListBuffer(n_nationkey#144)
    // Output Table: ListBuffer(s_suppkey#342, s_name#343)
    // Node Depth: 3
    int num_rows = tbl_JOIN_INNER_TD_3454_output.getNumRow();
        for (int j = 0; j < num_rows; j++) {
          int32_t rowid_1 = tbl_JOIN_INNER_TD_3454_output.getInt32(j, 0);
          int32_t rowid_2 = tbl_JOIN_INNER_TD_3454_output.getInt32(j, 1);
          int64_t key = tbl_JOIN_INNER_TD_3454_output.getInt32(j, 2);
          tbl_JOIN_INNER_TD_3454_consolidated_output.setInt32(j, 0, in1.getInt32(rowid_2 - 1, 0));
          tbl_JOIN_INNER_TD_3454_consolidated_output.setInt32(j, 1, in1.getInt32(rowid_2 - 1, 1));
        }
        tbl_JOIN_INNER_TD_3454_consolidated_output.setNumRow(num_rows);
    std::cout << "tbl_JOIN_INNER_TD_3454_consolidated_output #Row: " << tbl_JOIN_INNER_TD_3454_consolidated_output.getNumRow() << std::endl;
}

void SW_Consolidated_JOIN_INNER_TD_4586_output(Table& in1, Table&in2, Table& tbl_JOIN_INNER_TD_4586_output, Table& tbl_JOIN_INNER_TD_4586_consolidated_output) {
    // StringRowIDSubstitution: true StringRowIDBackSubstitution: false
    // Supported operation: JOIN_INNER
    // Operation: ListBuffer((s_suppkey#342 = l_suppkey#79))
    // Left Table: ListBuffer(s_suppkey#342, s_name#343)
    // Right Table: ListBuffer(l_orderkey#77, l_suppkey#79)
    // Output Table: ListBuffer(s_name#343, l_orderkey#77)
    // Node Depth: 4
    int num_rows = tbl_JOIN_INNER_TD_4586_output.getNumRow();
        for (int j = 0; j < num_rows; j++) {
          int32_t rowid_1 = tbl_JOIN_INNER_TD_4586_output.getInt32(j, 0);
          int32_t rowid_2 = tbl_JOIN_INNER_TD_4586_output.getInt32(j, 1);
          int64_t key = tbl_JOIN_INNER_TD_4586_output.getInt32(j, 2);
          tbl_JOIN_INNER_TD_4586_consolidated_output.setInt32(j, 0, in1.getInt32(rowid_2 - 1, 1));
          tbl_JOIN_INNER_TD_4586_consolidated_output.setInt32(j, 1, in2.getInt32(rowid_1 - 1, 0));
        }
        tbl_JOIN_INNER_TD_4586_consolidated_output.setNumRow(num_rows);
    std::cout << "tbl_JOIN_INNER_TD_4586_consolidated_output #Row: " << tbl_JOIN_INNER_TD_4586_consolidated_output.getNumRow() << std::endl;
}

void SW_Filter_TD_3451(Table &tbl_SerializeFromObject_TD_5445_input, Table &tbl_Filter_TD_3451_output) {
    // StringRowIDSubstitution: true StringRowIDBackSubstitution: false
    // Supported operation: Filter
    // Operation: ListBuffer((o_orderstatus#209 = 70))
    // Input: ListBuffer(o_orderkey#207, o_orderstatus#209)
    // Output: ListBuffer(o_orderkey#207)
    int r = 0;
    int nrow1 = tbl_SerializeFromObject_TD_5445_input.getNumRow();
    for (int i = 0; i < nrow1; i++) {
        int32_t _o_orderstatus209 = tbl_SerializeFromObject_TD_5445_input.getInt32(i, 1);
        if (_o_orderstatus209 == 70) {
            int32_t _o_orderkey207_t = tbl_SerializeFromObject_TD_5445_input.getInt32(i, 0);
            tbl_Filter_TD_3451_output.setInt32(r, 0, _o_orderkey207_t);
            r++;
        }
    }
    tbl_Filter_TD_3451_output.setNumRow(r);
    std::cout << "tbl_Filter_TD_3451_output #Row: " << tbl_Filter_TD_3451_output.getNumRow() << std::endl;
}

void SW_Consolidated_JOIN_INNER_TD_2757_output(Table& in1, Table&in2, Table& tbl_JOIN_INNER_TD_2757_output, Table& tbl_JOIN_INNER_TD_2757_consolidated_output) {
    // StringRowIDSubstitution: true StringRowIDBackSubstitution: false
    // Supported operation: JOIN_INNER
    // Operation: ListBuffer((o_orderkey#207 = l_orderkey#77))
    // Left Table: ListBuffer(s_name#343, l_orderkey#77)
    // Right Table: ListBuffer(o_orderkey#207)
    // Output Table: ListBuffer(s_name#343)
    // Node Depth: 2
    int num_rows = tbl_JOIN_INNER_TD_2757_output.getNumRow();
        for (int j = 0; j < num_rows; j++) {
          int32_t rowid_1 = tbl_JOIN_INNER_TD_2757_output.getInt32(j, 0);
          int32_t rowid_2 = tbl_JOIN_INNER_TD_2757_output.getInt32(j, 1);
          int64_t key = tbl_JOIN_INNER_TD_2757_output.getInt32(j, 2);
          tbl_JOIN_INNER_TD_2757_consolidated_output.setInt32(j, 0, in1.getInt32(rowid_2 - 1, 0));
        }
        tbl_JOIN_INNER_TD_2757_consolidated_output.setNumRow(num_rows);
    std::cout << "tbl_JOIN_INNER_TD_2757_consolidated_output #Row: " << tbl_JOIN_INNER_TD_2757_consolidated_output.getNumRow() << std::endl;
}

typedef std::string SW_Aggregate_TD_1429_key;
struct SW_Aggregate_TD_1429_payload {
    int64_t _numwait1900L_count_0;
};
void SW_Aggregate_TD_1429(Table &tbl_JOIN_INNER_TD_2757_consolidated_output, Table &tbl_SerializeFromObject_TD_5445_input, Table &tbl_SerializeFromObject_TD_525_input, Table &tbl_SerializeFromObject_TD_4885_input, Table &tbl_Aggregate_TD_1429_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: true
    // Supported operation: Aggregate
    // Operation: ListBuffer(s_name#343, count(1) AS numwait#1900L)
    // Input: ListBuffer(s_name#343)
    // Output: ListBuffer(s_name#343, numwait#1900L)
    std::unordered_map<SW_Aggregate_TD_1429_key, SW_Aggregate_TD_1429_payload> ht1;
    int nrow1 = tbl_JOIN_INNER_TD_2757_consolidated_output.getNumRow();
    for (int i = 0; i < nrow1; i++) {
        std::array<char, TPCH_READ_S_NAME_LEN + 1> _s_name343 = tbl_SerializeFromObject_TD_525_input.getcharN<char, TPCH_READ_S_NAME_LEN + 1>(tbl_JOIN_INNER_TD_2757_consolidated_output.getInt32(i, 0), 1);
        SW_Aggregate_TD_1429_key k = std::string(_s_name343.data());
        int64_t _numwait1900L_count_0 = 1 != 0 ? 1 : 0;
        SW_Aggregate_TD_1429_payload p{_numwait1900L_count_0};
        auto it = ht1.find(k);
        if (it != ht1.end()) {
            int64_t count_0 = (it->second)._numwait1900L_count_0 + _numwait1900L_count_0;
            p._numwait1900L_count_0 = count_0;
            ht1[k] = p;
        } else { 
            ht1.insert(std::make_pair(k, p));
        }
    }
    int r = 0;
    for (auto& it : ht1) {
        std::array<char, TPCH_READ_S_NAME_LEN + 1> _s_name343{};
        memcpy(_s_name343.data(), (it.first).data(), (it.first).length());
        tbl_Aggregate_TD_1429_output.setcharN<char, TPCH_READ_S_NAME_LEN + 1>(r, 0, _s_name343);
        int64_t _numwait1900L = (it.second)._numwait1900L_count_0;
        tbl_Aggregate_TD_1429_output.setInt64(r, 1, _numwait1900L);
        ++r;
    }
    tbl_Aggregate_TD_1429_output.setNumRow(r);
    std::cout << "tbl_Aggregate_TD_1429_output #Row: " << tbl_Aggregate_TD_1429_output.getNumRow() << std::endl;
}

void SW_Sort_TD_0156(Table &tbl_Aggregate_TD_1429_output, Table &tbl_Sort_TD_0156_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: Sort
    // Operation: ListBuffer(numwait#1900L DESC NULLS LAST, s_name#343 ASC NULLS FIRST)
    // Input: ListBuffer(s_name#343, numwait#1900L)
    // Output: ListBuffer(s_name#343, numwait#1900L)
    struct SW_Sort_TD_0156Row {
        std::string _s_name343;
        int64_t _numwait1900L;
    }; 

    struct {
        bool operator()(const SW_Sort_TD_0156Row& a, const SW_Sort_TD_0156Row& b) const { return 
 (a._numwait1900L > b._numwait1900L) || 
 ((a._numwait1900L == b._numwait1900L) && (a._s_name343 < b._s_name343)); 
}
    }SW_Sort_TD_0156_order; 

    int nrow1 = tbl_Aggregate_TD_1429_output.getNumRow();
    std::vector<SW_Sort_TD_0156Row> rows;
    for (int i = 0; i < nrow1; i++) {
        std::array<char, TPCH_READ_S_NAME_LEN + 1> _s_name343 = tbl_Aggregate_TD_1429_output.getcharN<char, TPCH_READ_S_NAME_LEN +1>(i, 0);
        int64_t _numwait1900L = tbl_Aggregate_TD_1429_output.getInt64(i, 1);
        SW_Sort_TD_0156Row t = {std::string(_s_name343.data()),_numwait1900L};
        rows.push_back(t);
    }
    std::sort(rows.begin(), rows.end(), SW_Sort_TD_0156_order);
    int r = 0;
    for (auto& it : rows) {
        std::array<char, TPCH_READ_S_NAME_LEN + 1> _s_name343{};
        memcpy(_s_name343.data(), (it._s_name343).data(), (it._s_name343).length());
        tbl_Sort_TD_0156_output.setcharN<char, TPCH_READ_S_NAME_LEN +1>(r, 0, _s_name343);
        tbl_Sort_TD_0156_output.setInt64(r, 1, it._numwait1900L);
        if (r < 10) {
            std::cout << (it._s_name343).data() << " " << it._numwait1900L << " " << std::endl;
        }
        ++r;
    }
    tbl_Sort_TD_0156_output.setNumRow(r);
    std::cout << "tbl_Sort_TD_0156_output #Row: " << tbl_Sort_TD_0156_output.getNumRow() << std::endl;
}

