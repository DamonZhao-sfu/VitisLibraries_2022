#include <regex> 
#include <stdint.h> 

void SW_Filter_TD_4458(Table &tbl_SerializeFromObject_TD_531_input, Table &tbl_Filter_TD_4458_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: Filter
    // Operation: ListBuffer((isnotnull(o_comment#215) AND NOT o_comment#215 LIKE %unusual%deposits%))
    // Input: ListBuffer(o_orderkey#207, o_custkey#208, o_comment#215)
    // Output: ListBuffer(o_orderkey#207, o_custkey#208)
    int r = 0;
    int nrow1 = tbl_SerializeFromObject_TD_531_input.getNumRow();
    for (int i = 0; i < nrow1; i++) {
        std::array<char, TPCH_READ_O_CMNT_MAX + 1> _o_comment215 = tbl_SerializeFromObject_TD_531_input.getcharN<char, TPCH_READ_O_CMNT_MAX + 1>(i, 2);
        struct custom_func {
                bool strm_pattern(std::string sub1, std::string sub2, std::string s, int len = 7) {
                    std::string::size_type spe_f = s.find(sub1);
                    std::string::size_type c_f = 0;
                    while (spe_f != std::string::npos) {
                        c_f += (spe_f + len);
                        std::string sub_s = s.substr(c_f);
                        if (sub_s.find(sub2) != std::string::npos) return true;
                        spe_f = sub_s.find(sub1);
                    }
                    return false;
                }
        }custom_func_obj;
        if ((1) && !(custom_func_obj.strm_pattern("unusual", "deposits", _o_comment215.data()))) {
            int32_t _o_orderkey207_t = tbl_SerializeFromObject_TD_531_input.getInt32(i, 0);
            tbl_Filter_TD_4458_output.setInt32(r, 0, _o_orderkey207_t);
            int32_t _o_custkey208_t = tbl_SerializeFromObject_TD_531_input.getInt32(i, 1);
            tbl_Filter_TD_4458_output.setInt32(r, 1, _o_custkey208_t);
            r++;
        }
    }
    tbl_Filter_TD_4458_output.setNumRow(r);
    std::cout << "tbl_Filter_TD_4458_output #Row: " << tbl_Filter_TD_4458_output.getNumRow() << std::endl;
}

void SW_Consolidated_JOIN_INNER_TD_0821_output(Table& in1, Table&in2, Table& tbl_JOIN_INNER_TD_0821_output, Table& tbl_JOIN_INNER_TD_0821_consolidated_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: JOIN_INNER
    // Operation: ListBuffer((c_custkey#22 = o_custkey#208))
    // Left Table: ListBuffer(c_custkey#22)
    // Right Table: ListBuffer(o_orderkey#207, o_custkey#208)
    // Output Table: ListBuffer(c_custkey#22, o_orderkey#207, o_custkey#208)
    // Node Depth: 0
    int num_rows = tbl_JOIN_INNER_TD_0821_output.getNumRow();
        for (int j = 0; j < num_rows; j++) {
          int32_t rowid_1 = tbl_JOIN_INNER_TD_0821_output.getInt32(j, 0);
          int32_t rowid_2 = tbl_JOIN_INNER_TD_0821_output.getInt32(j, 1);
          int64_t key = tbl_JOIN_INNER_TD_0821_output.getInt32(j, 2);
          tbl_JOIN_INNER_TD_0821_consolidated_output.setInt32(j, 0, in1.getInt32(rowid_2 - 1, 0));
          tbl_JOIN_INNER_TD_0821_consolidated_output.setInt32(j, 1, in2.getInt32(rowid_1 - 1, 0));
          tbl_JOIN_INNER_TD_0821_consolidated_output.setInt32(j, 2, in2.getInt32(rowid_1 - 1, 1));
        }
        tbl_JOIN_INNER_TD_0821_consolidated_output.setNumRow(num_rows);
    std::cout << "tbl_JOIN_INNER_TD_0821_consolidated_output #Row: " << tbl_JOIN_INNER_TD_0821_consolidated_output.getNumRow() << std::endl;
}

void SW_JOIN_LEFTANTI_TD_3422_concatenate(Table &tbl_JOIN_LEFTANTI_TD_3422_output, Table &tbl_JOIN_INNER_TD_0821_consolidated_output) {
    int start_idx = tbl_JOIN_LEFTANTI_TD_3422_output.getNumRow();
    int nrow = tbl_JOIN_INNER_TD_0821_consolidated_output.getNumRow();
    int i = 0;
    for (int r(start_idx); r<start_idx+nrow; ++r) {
        int32_t _c_custkey22 = tbl_JOIN_INNER_TD_0821_consolidated_output.getInt32(i, 0);
        tbl_JOIN_LEFTANTI_TD_3422_output.setInt32(r, 0, _c_custkey22);
        int32_t _o_orderkey207 = tbl_JOIN_INNER_TD_0821_consolidated_output.getInt32(i, 1);
        tbl_JOIN_LEFTANTI_TD_3422_output.setInt32(r, 1, _o_orderkey207);
        i++;
    }
    tbl_JOIN_LEFTANTI_TD_3422_output.setNumRow(start_idx + nrow);
    std::cout << "tbl_JOIN_LEFTANTI_TD_3422_output #Row: " << tbl_JOIN_LEFTANTI_TD_3422_output.getNumRow() << std::endl;
}
void SW_Consolidated_JOIN_LEFTANTI_TD_3422_output(Table& in1, Table&in2, Table& tbl_JOIN_LEFTANTI_TD_3422_output, Table& tbl_JOIN_LEFTANTI_TD_3422_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: JOIN_LEFTANTI
    // Operation: ListBuffer((c_custkey#22 = o_custkey#208))
    // Left Table: ListBuffer(c_custkey#22)
    // Right Table: ListBuffer(c_custkey#22, o_orderkey#207, o_custkey#208)
    // Output Table: ListBuffer(c_custkey#22, o_orderkey#207)
    // Node Depth: 3
    int num_rows = tbl_JOIN_LEFTANTI_TD_3422_output.getNumRow();
        for (int j = 0; j < num_rows; j++) {
          int32_t rowid_1 = tbl_JOIN_LEFTANTI_TD_3422_output.getInt32(j, 0);
          int32_t rowid_2 = tbl_JOIN_LEFTANTI_TD_3422_output.getInt32(j, 1);
          int64_t key = tbl_JOIN_LEFTANTI_TD_3422_output.getInt32(j, 2);
          tbl_JOIN_LEFTANTI_TD_3422_output.setInt32(j, 0, in1.getInt32(rowid_2 - 1, 0));
          tbl_JOIN_LEFTANTI_TD_3422_output.setInt32(j, 0, in2.getInt32(rowid_1 - 1, 0));
          tbl_JOIN_LEFTANTI_TD_3422_output.setInt32(j, 1, in2.getInt32(rowid_1 - 1, 1));
        }
        tbl_JOIN_LEFTANTI_TD_3422_output.setNumRow(num_rows);
    std::cout << "tbl_JOIN_LEFTANTI_TD_3422_output #Row: " << tbl_JOIN_LEFTANTI_TD_3422_output.getNumRow() << std::endl;
}

typedef int32_t SW_Aggregate_TD_2851_key;
struct SW_Aggregate_TD_2851_payload {
    int64_t _c_count1365L_count_0;
};
void SW_Aggregate_TD_2851(Table &tbl_JOIN_LEFTANTI_TD_3422_output, Table &tbl_Aggregate_TD_2851_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: Aggregate
    // Operation: ListBuffer(c_custkey#22, count(o_orderkey#207) AS c_count#1365L)
    // Input: ListBuffer(c_custkey#22, o_orderkey#207)
    // Output: ListBuffer(c_count#1365L)
    std::unordered_map<SW_Aggregate_TD_2851_key, SW_Aggregate_TD_2851_payload> ht1;
    int nrow1 = tbl_JOIN_LEFTANTI_TD_3422_output.getNumRow();
    for (int i = 0; i < nrow1; i++) {
        int32_t _c_custkey22 = tbl_JOIN_LEFTANTI_TD_3422_output.getInt32(i, 0);
        int32_t _o_orderkey207 = tbl_JOIN_LEFTANTI_TD_3422_output.getInt32(i, 1);
        SW_Aggregate_TD_2851_key k = _c_custkey22;
        int64_t _c_count1365L_count_0 = _o_orderkey207 != 0 ? 1 : 0;
        SW_Aggregate_TD_2851_payload p{_c_count1365L_count_0};
        auto it = ht1.find(k);
        if (it != ht1.end()) {
            int64_t count_0 = (it->second)._c_count1365L_count_0 + _c_count1365L_count_0;
            p._c_count1365L_count_0 = count_0;
            ht1[k] = p;
        } else { 
            ht1.insert(std::make_pair(k, p));
        }
    }
    int r = 0;
    for (auto& it : ht1) {
        // _c_custkey22 not required in the output table
        int64_t _c_count1365L = (it.second)._c_count1365L_count_0;
        tbl_Aggregate_TD_2851_output.setInt64(r, 0, _c_count1365L);
        ++r;
    }
    tbl_Aggregate_TD_2851_output.setNumRow(r);
    std::cout << "tbl_Aggregate_TD_2851_output #Row: " << tbl_Aggregate_TD_2851_output.getNumRow() << std::endl;
}

typedef int64_t SW_Aggregate_TD_1692_key;
struct SW_Aggregate_TD_1692_payload {
    int64_t _custdist1360L_count_0;
};
void SW_Aggregate_TD_1692(Table &tbl_Aggregate_TD_2851_output, Table &tbl_Aggregate_TD_1692_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: Aggregate
    // Operation: ListBuffer(c_count#1365L, count(1) AS custdist#1360L)
    // Input: ListBuffer(c_count#1365L)
    // Output: ListBuffer(c_count#1365L, custdist#1360L)
    std::unordered_map<SW_Aggregate_TD_1692_key, SW_Aggregate_TD_1692_payload> ht1;
    int nrow1 = tbl_Aggregate_TD_2851_output.getNumRow();
    for (int i = 0; i < nrow1; i++) {
        int64_t _c_count1365L = tbl_Aggregate_TD_2851_output.getInt64(i, 0);
        SW_Aggregate_TD_1692_key k = _c_count1365L;
        int64_t _custdist1360L_count_0 = 1 != 0 ? 1 : 0;
        SW_Aggregate_TD_1692_payload p{_custdist1360L_count_0};
        auto it = ht1.find(k);
        if (it != ht1.end()) {
            int64_t count_0 = (it->second)._custdist1360L_count_0 + _custdist1360L_count_0;
            p._custdist1360L_count_0 = count_0;
            ht1[k] = p;
        } else { 
            ht1.insert(std::make_pair(k, p));
        }
    }
    int r = 0;
    for (auto& it : ht1) {
        tbl_Aggregate_TD_1692_output.setInt64(r, 0, (it.first));
        int64_t _custdist1360L = (it.second)._custdist1360L_count_0;
        tbl_Aggregate_TD_1692_output.setInt64(r, 1, _custdist1360L);
        ++r;
    }
    tbl_Aggregate_TD_1692_output.setNumRow(r);
    std::cout << "tbl_Aggregate_TD_1692_output #Row: " << tbl_Aggregate_TD_1692_output.getNumRow() << std::endl;
}

void SW_Sort_TD_0688(Table &tbl_Aggregate_TD_1692_output, Table &tbl_Sort_TD_0688_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: Sort
    // Operation: ListBuffer(custdist#1360L DESC NULLS LAST, c_count#1365L DESC NULLS LAST)
    // Input: ListBuffer(c_count#1365L, custdist#1360L)
    // Output: ListBuffer(c_count#1365L, custdist#1360L)
    struct SW_Sort_TD_0688Row {
        int64_t _c_count1365L;
        int64_t _custdist1360L;
    }; 

    struct {
        bool operator()(const SW_Sort_TD_0688Row& a, const SW_Sort_TD_0688Row& b) const { return 
 (a._custdist1360L > b._custdist1360L) || 
 ((a._custdist1360L == b._custdist1360L) && (a._c_count1365L > b._c_count1365L)); 
}
    }SW_Sort_TD_0688_order; 

    int nrow1 = tbl_Aggregate_TD_1692_output.getNumRow();
    std::vector<SW_Sort_TD_0688Row> rows;
    for (int i = 0; i < nrow1; i++) {
        int64_t _c_count1365L = tbl_Aggregate_TD_1692_output.getInt64(i, 0);
        int64_t _custdist1360L = tbl_Aggregate_TD_1692_output.getInt64(i, 1);
        SW_Sort_TD_0688Row t = {_c_count1365L,_custdist1360L};
        rows.push_back(t);
    }
    std::sort(rows.begin(), rows.end(), SW_Sort_TD_0688_order);
    int r = 0;
    for (auto& it : rows) {
        tbl_Sort_TD_0688_output.setInt64(r, 0, it._c_count1365L);
        tbl_Sort_TD_0688_output.setInt64(r, 1, it._custdist1360L);
        if (r < 10) {
            std::cout << it._c_count1365L << " " << it._custdist1360L << " " << std::endl;
        }
        ++r;
    }
    tbl_Sort_TD_0688_output.setNumRow(r);
    std::cout << "tbl_Sort_TD_0688_output #Row: " << tbl_Sort_TD_0688_output.getNumRow() << std::endl;
}

