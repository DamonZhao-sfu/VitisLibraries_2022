#include <regex> 
#include <stdint.h> 

void SW_Filter_TD_4209(Table &tbl_SerializeFromObject_TD_5828_input, Table &tbl_Filter_TD_4209_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: Filter
    // Operation: ListBuffer((isnotnull(o_comment#215) AND NOT o_comment#215 LIKE %unusual%deposits%))
    // Input: ListBuffer(o_orderkey#207, o_custkey#208, o_comment#215)
    // Output: ListBuffer(o_orderkey#207, o_custkey#208)
    int r = 0;
    int nrow1 = tbl_SerializeFromObject_TD_5828_input.getNumRow();
    for (int i = 0; i < nrow1; i++) {
        std::array<char, TPCH_READ_O_CMNT_MAX + 1> _o_comment215 = tbl_SerializeFromObject_TD_5828_input.getcharN<char, TPCH_READ_O_CMNT_MAX + 1>(i, 2);
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
            int32_t _o_orderkey207_t = tbl_SerializeFromObject_TD_5828_input.getInt32(i, 0);
            tbl_Filter_TD_4209_output.setInt32(r, 0, _o_orderkey207_t);
            int32_t _o_custkey208_t = tbl_SerializeFromObject_TD_5828_input.getInt32(i, 1);
            tbl_Filter_TD_4209_output.setInt32(r, 1, _o_custkey208_t);
            r++;
        }
    }
    tbl_Filter_TD_4209_output.setNumRow(r);
    std::cout << "tbl_Filter_TD_4209_output #Row: " << tbl_Filter_TD_4209_output.getNumRow() << std::endl;
}


void SW_JOIN_LEFTANTI_TD_3852_concatenate(Table &tbl_JOIN_LEFTANTI_TD_3852_output, Table &tbl_JOIN_INNER_TD_0301_output) {
    int start_idx = tbl_JOIN_LEFTANTI_TD_3852_output.getNumRow();
    int nrow = tbl_JOIN_INNER_TD_0301_output.getNumRow();
    int i = 0;
    for (int r(start_idx); r<start_idx+nrow; ++r) {
        int32_t _c_custkey22 = tbl_JOIN_INNER_TD_0301_output.getInt32(i, 0);
        tbl_JOIN_LEFTANTI_TD_3852_output.setInt32(r, 0, _c_custkey22);
        int32_t _o_orderkey207 = tbl_JOIN_INNER_TD_0301_output.getInt32(i, 1);
        tbl_JOIN_LEFTANTI_TD_3852_output.setInt32(r, 1, _o_orderkey207);
        i++;
    }
    tbl_JOIN_LEFTANTI_TD_3852_output.setNumRow(start_idx + nrow);
    std::cout << "tbl_JOIN_LEFTANTI_TD_3852_output #Row: " << tbl_JOIN_LEFTANTI_TD_3852_output.getNumRow() << std::endl;
}

typedef int32_t SW_Aggregate_TD_2425_key;
struct SW_Aggregate_TD_2425_payload {
    int64_t _c_count1367L_count_0;
};
void SW_Aggregate_TD_2425(Table *tbl_JOIN_LEFTANTI_TD_3852_output, Table &tbl_Aggregate_TD_2425_output, int hpTimes) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: Aggregate
    // Operation: ListBuffer(c_custkey#22, count(o_orderkey#207) AS c_count#1367L)
    // Input: ListBuffer(c_custkey#22, o_orderkey#207)
    // Output: ListBuffer(c_count#1367L)
    std::unordered_map<SW_Aggregate_TD_2425_key, SW_Aggregate_TD_2425_payload> ht1;
for (int p_idx = 0; p_idx < hpTimes; p_idx++) {
    int nrow1 = tbl_JOIN_LEFTANTI_TD_3852_output[p_idx].getNumRow();
    for (int i = 0; i < nrow1; i++) {
        int32_t _c_custkey22 = tbl_JOIN_LEFTANTI_TD_3852_output[p_idx].getInt32(i, 0);
        int32_t _o_orderkey207 = tbl_JOIN_LEFTANTI_TD_3852_output[p_idx].getInt32(i, 1);
        SW_Aggregate_TD_2425_key k = _c_custkey22;
        int64_t _c_count1367L_count_0 = _o_orderkey207 != 0 ? 1 : 0;
        SW_Aggregate_TD_2425_payload p{_c_count1367L_count_0};
        auto it = ht1.find(k);
        if (it != ht1.end()) {
            int64_t count_0 = (it->second)._c_count1367L_count_0 + _c_count1367L_count_0;
            p._c_count1367L_count_0 = count_0;
            ht1[k] = p;
        } else { 
            ht1.insert(std::make_pair(k, p));
        }
    }
}
    int r = 0;
    for (auto& it : ht1) {
        // _c_custkey22 not required in the output table
        int64_t _c_count1367L = (it.second)._c_count1367L_count_0;
        tbl_Aggregate_TD_2425_output.setInt64(r, 0, _c_count1367L);
        ++r;
    }
    tbl_Aggregate_TD_2425_output.setNumRow(r);
    std::cout << "tbl_Aggregate_TD_2425_output #Row: " << tbl_Aggregate_TD_2425_output.getNumRow() << std::endl;
}

typedef int64_t SW_Aggregate_TD_1576_key;
struct SW_Aggregate_TD_1576_payload {
    int64_t _custdist1362L_count_0;
};
void SW_Aggregate_TD_1576(Table &tbl_Aggregate_TD_2425_output, Table &tbl_Aggregate_TD_1576_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: Aggregate
    // Operation: ListBuffer(c_count#1367L, count(1) AS custdist#1362L)
    // Input: ListBuffer(c_count#1367L)
    // Output: ListBuffer(c_count#1367L, custdist#1362L)
    std::unordered_map<SW_Aggregate_TD_1576_key, SW_Aggregate_TD_1576_payload> ht1;
    int nrow1 = tbl_Aggregate_TD_2425_output.getNumRow();
    for (int i = 0; i < nrow1; i++) {
        int64_t _c_count1367L = tbl_Aggregate_TD_2425_output.getInt64(i, 0);
        SW_Aggregate_TD_1576_key k = _c_count1367L;
        int64_t _custdist1362L_count_0 = 1 != 0 ? 1 : 0;
        SW_Aggregate_TD_1576_payload p{_custdist1362L_count_0};
        auto it = ht1.find(k);
        if (it != ht1.end()) {
            int64_t count_0 = (it->second)._custdist1362L_count_0 + _custdist1362L_count_0;
            p._custdist1362L_count_0 = count_0;
            ht1[k] = p;
        } else { 
            ht1.insert(std::make_pair(k, p));
        }
    }
    int r = 0;
    for (auto& it : ht1) {
        tbl_Aggregate_TD_1576_output.setInt64(r, 0, (it.first));
        int64_t _custdist1362L = (it.second)._custdist1362L_count_0;
        tbl_Aggregate_TD_1576_output.setInt64(r, 1, _custdist1362L);
        ++r;
    }
    tbl_Aggregate_TD_1576_output.setNumRow(r);
    std::cout << "tbl_Aggregate_TD_1576_output #Row: " << tbl_Aggregate_TD_1576_output.getNumRow() << std::endl;
}

void SW_Sort_TD_0954(Table &tbl_Aggregate_TD_1576_output, Table &tbl_Sort_TD_0954_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: Sort
    // Operation: ListBuffer(custdist#1362L DESC NULLS LAST, c_count#1367L DESC NULLS LAST)
    // Input: ListBuffer(c_count#1367L, custdist#1362L)
    // Output: ListBuffer(c_count#1367L, custdist#1362L)
    struct SW_Sort_TD_0954Row {
        int64_t _c_count1367L;
        int64_t _custdist1362L;
    }; 

    struct {
        bool operator()(const SW_Sort_TD_0954Row& a, const SW_Sort_TD_0954Row& b) const { return 
 (a._custdist1362L > b._custdist1362L) || 
 ((a._custdist1362L == b._custdist1362L) && (a._c_count1367L > b._c_count1367L)); 
}
    }SW_Sort_TD_0954_order; 

    int nrow1 = tbl_Aggregate_TD_1576_output.getNumRow();
    std::vector<SW_Sort_TD_0954Row> rows;
    for (int i = 0; i < nrow1; i++) {
        int64_t _c_count1367L = tbl_Aggregate_TD_1576_output.getInt64(i, 0);
        int64_t _custdist1362L = tbl_Aggregate_TD_1576_output.getInt64(i, 1);
        SW_Sort_TD_0954Row t = {_c_count1367L,_custdist1362L};
        rows.push_back(t);
    }
    std::sort(rows.begin(), rows.end(), SW_Sort_TD_0954_order);
    int r = 0;
    for (auto& it : rows) {
        tbl_Sort_TD_0954_output.setInt64(r, 0, it._c_count1367L);
        tbl_Sort_TD_0954_output.setInt64(r, 1, it._custdist1362L);
        if (r < 10) {
            std::cout << it._c_count1367L << " " << it._custdist1362L << " " << std::endl;
        }
        ++r;
    }
    tbl_Sort_TD_0954_output.setNumRow(r);
    std::cout << "tbl_Sort_TD_0954_output #Row: " << tbl_Sort_TD_0954_output.getNumRow() << std::endl;
}

