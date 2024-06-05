#include <regex> 
#include <stdint.h> 

void SW_Filter_TD_4606(Table &tbl_SerializeFromObject_TD_5994_input, Table &tbl_Filter_TD_4606_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: Filter
    // Operation: ListBuffer((isnotnull(s_comment#348) AND s_comment#348 LIKE %Customer%Complaints%))
    // Input: ListBuffer(s_suppkey#342, s_comment#348)
    // Output: ListBuffer(s_suppkey#342)
    int r = 0;
    int nrow1 = tbl_SerializeFromObject_TD_5994_input.getNumRow();
    for (int i = 0; i < nrow1; i++) {
        std::array<char, TPCH_READ_S_CMNT_MAX + 1> _s_comment348 = tbl_SerializeFromObject_TD_5994_input.getcharN<char, TPCH_READ_S_CMNT_MAX + 1>(i, 1);
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
        if ((1) && custom_func_obj.strm_pattern("Customer", "Complaints", _s_comment348.data())) {
            int32_t _s_suppkey342_t = tbl_SerializeFromObject_TD_5994_input.getInt32(i, 0);
            tbl_Filter_TD_4606_output.setInt32(r, 0, _s_suppkey342_t);
            r++;
        }
    }
    tbl_Filter_TD_4606_output.setNumRow(r);
    std::cout << "tbl_Filter_TD_4606_output #Row: " << tbl_Filter_TD_4606_output.getNumRow() << std::endl;
}

void SW_Filter_TD_3677(Table &tbl_SerializeFromObject_TD_437_input, Table &tbl_Filter_TD_3677_output) {
    // StringRowIDSubstitution: true StringRowIDBackSubstitution: false
    // Supported operation: Filter
    // Operation: ListBuffer(((isnotnull(p_brand#261) AND isnotnull(p_type#262)) AND ((NOT (p_brand#261 = Brand#32) AND NOT StartsWith(p_type#262, SMALL ANODIZED)) AND p_size#263 IN (43,7,27,21,5,15,36,11))))
    // Input: ListBuffer(p_partkey#258, p_brand#261, p_type#262, p_size#263)
    // Output: ListBuffer(p_partkey#258, p_brand#261, p_type#262, p_size#263)
    int r = 0;
    int nrow1 = tbl_SerializeFromObject_TD_437_input.getNumRow();
    for (int i = 0; i < nrow1; i++) {
        std::array<char, TPCH_READ_P_BRND_LEN + 1> _p_brand261 = tbl_SerializeFromObject_TD_437_input.getcharN<char, TPCH_READ_P_BRND_LEN + 1>(i, 1);
        std::array<char, TPCH_READ_P_TYPE_LEN + 1> _p_type262 = tbl_SerializeFromObject_TD_437_input.getcharN<char, TPCH_READ_P_TYPE_LEN + 1>(i, 2);
        int32_t _p_size263 = tbl_SerializeFromObject_TD_437_input.getInt32(i, 3);
        auto reuse_col_str_899 = _p_size263;
        if (((1) && (1)) && ((!((std::string(_p_brand261.data()) == "Brand#32")) && !((std::string(_p_type262.data()).find("SMALL ANODIZED")==0))) && ((reuse_col_str_899 == 43) || (reuse_col_str_899 == 7) || (reuse_col_str_899 == 27) || (reuse_col_str_899 == 21) || (reuse_col_str_899 == 5) || (reuse_col_str_899 == 15) || (reuse_col_str_899 == 36) || (reuse_col_str_899 == 11) || (0)))) {
            int32_t _p_partkey258_t = tbl_SerializeFromObject_TD_437_input.getInt32(i, 0);
            tbl_Filter_TD_3677_output.setInt32(r, 0, _p_partkey258_t);
            tbl_Filter_TD_3677_output.setInt32(r, 1, i);
            tbl_Filter_TD_3677_output.setInt32(r, 2, i);
            int32_t _p_size263_t = tbl_SerializeFromObject_TD_437_input.getInt32(i, 3);
            tbl_Filter_TD_3677_output.setInt32(r, 3, _p_size263_t);
            r++;
        }
    }
    tbl_Filter_TD_3677_output.setNumRow(r);
    std::cout << "tbl_Filter_TD_3677_output #Row: " << tbl_Filter_TD_3677_output.getNumRow() << std::endl;
}

void SW_Consolidated_JOIN_LEFTANTI_TD_3403_output(Table& in1, Table&in2, Table& tbl_JOIN_LEFTANTI_TD_3403_output, Table& tbl_JOIN_LEFTANTI_TD_3403_consolidated_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: JOIN_LEFTANTI
    // Operation: ListBuffer(((ps_suppkey#306 = s_suppkey#342) OR isnull((ps_suppkey#306 = s_suppkey#342))))
    // Left Table: ListBuffer(s_suppkey#342)
    // Right Table: ListBuffer(ps_partkey#305, ps_suppkey#306)
    // Output Table: ListBuffer(ps_partkey#305, ps_suppkey#306)
    // Node Depth: 3
    int num_rows = tbl_JOIN_LEFTANTI_TD_3403_output.getNumRow();
        for (int j = 0; j < num_rows; j++) {
          int32_t rowid_1 = tbl_JOIN_LEFTANTI_TD_3403_output.getInt32(j, 0);
          int32_t rowid_2 = tbl_JOIN_LEFTANTI_TD_3403_output.getInt32(j, 1);
          int64_t key = tbl_JOIN_LEFTANTI_TD_3403_output.getInt32(j, 2);
          tbl_JOIN_LEFTANTI_TD_3403_consolidated_output.setInt32(j, 0, in2.getInt32(rowid_1 - 1, 0));
          tbl_JOIN_LEFTANTI_TD_3403_consolidated_output.setInt32(j, 1, in2.getInt32(rowid_1 - 1, 1));
        }
        tbl_JOIN_LEFTANTI_TD_3403_consolidated_output.setNumRow(num_rows);
    std::cout << "tbl_JOIN_LEFTANTI_TD_3403_consolidated_output #Row: " << tbl_JOIN_LEFTANTI_TD_3403_consolidated_output.getNumRow() << std::endl;
}

void SW_Consolidated_JOIN_INNER_TD_2163_output(Table& in1, Table&in2, Table& tbl_JOIN_INNER_TD_2163_output, Table& tbl_JOIN_INNER_TD_2163_consolidated_output) {
    // StringRowIDSubstitution: true StringRowIDBackSubstitution: false
    // Supported operation: JOIN_INNER
    // Operation: ListBuffer((p_partkey#258 = ps_partkey#305))
    // Left Table: ListBuffer(p_partkey#258, p_brand#261, p_type#262, p_size#263)
    // Right Table: ListBuffer(ps_partkey#305, ps_suppkey#306)
    // Output Table: ListBuffer(ps_suppkey#306, p_brand#261, p_type#262, p_size#263)
    // Node Depth: 2
    int num_rows = tbl_JOIN_INNER_TD_2163_output.getNumRow();
        for (int j = 0; j < num_rows; j++) {
          int32_t rowid_1 = tbl_JOIN_INNER_TD_2163_output.getInt32(j, 0);
          int32_t rowid_2 = tbl_JOIN_INNER_TD_2163_output.getInt32(j, 1);
          int64_t key = tbl_JOIN_INNER_TD_2163_output.getInt32(j, 2);
          tbl_JOIN_INNER_TD_2163_consolidated_output.setInt32(j, 0, in2.getInt32(rowid_1 - 1, 1));
          tbl_JOIN_INNER_TD_2163_consolidated_output.setInt32(j, 1, in1.getInt32(rowid_2 - 1, 1));
          tbl_JOIN_INNER_TD_2163_consolidated_output.setInt32(j, 2, in1.getInt32(rowid_2 - 1, 2));
          tbl_JOIN_INNER_TD_2163_consolidated_output.setInt32(j, 3, in1.getInt32(rowid_2 - 1, 3));
        }
        tbl_JOIN_INNER_TD_2163_consolidated_output.setNumRow(num_rows);
    std::cout << "tbl_JOIN_INNER_TD_2163_consolidated_output #Row: " << tbl_JOIN_INNER_TD_2163_consolidated_output.getNumRow() << std::endl;
}

struct SW_Aggregate_TD_1520_key {
    std::string _p_brand261;
    std::string _p_type262;
    int32_t _p_size263;
    bool operator==(const SW_Aggregate_TD_1520_key& other) const { return (_p_brand261 == other._p_brand261) && (_p_type262 == other._p_type262) && (_p_size263 == other._p_size263); }
};
namespace std {
template <>
struct hash<SW_Aggregate_TD_1520_key> {
    std::size_t operator() (const SW_Aggregate_TD_1520_key& k) const {
        using std::size_t;
        using std::hash;
        using std::string;
        return (hash<string>()(k._p_brand261)) + (hash<string>()(k._p_type262)) + (hash<int32_t>()(k._p_size263));
    }
};
}
struct SW_Aggregate_TD_1520_payload {
    int64_t _supplier_cnt1548L_count_0;
};
void SW_Aggregate_TD_1520(Table *tbl_JOIN_INNER_TD_2163_consolidated_output, Table &tbl_SerializeFromObject_TD_437_input, Table &tbl_Aggregate_TD_1520_output, int hpTimes) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: true
    // Supported operation: Aggregate
    // Operation: ListBuffer(p_brand#261, p_type#262, p_size#263, count(distinct ps_suppkey#306) AS supplier_cnt#1548L)
    // Input: ListBuffer(ps_suppkey#306, p_brand#261, p_type#262, p_size#263)
    // Output: ListBuffer(p_brand#261, p_type#262, p_size#263, supplier_cnt#1548L)
    std::unordered_map<SW_Aggregate_TD_1520_key, SW_Aggregate_TD_1520_payload> ht1;
for (int p_idx = 0; p_idx < hpTimes; p_idx++) {
    int nrow1 = tbl_JOIN_INNER_TD_2163_consolidated_output[p_idx].getNumRow();
    for (int i = 0; i < nrow1; i++) {
        int32_t _ps_suppkey306 = tbl_JOIN_INNER_TD_2163_consolidated_output[p_idx].getInt32(i, 0);
        std::array<char, TPCH_READ_P_BRND_LEN + 1> _p_brand261 = tbl_SerializeFromObject_TD_437_input.getcharN<char, TPCH_READ_P_BRND_LEN + 1>(tbl_JOIN_INNER_TD_2163_consolidated_output[p_idx].getInt32(i, 1), 1);
        std::array<char, TPCH_READ_P_TYPE_LEN + 1> _p_type262 = tbl_SerializeFromObject_TD_437_input.getcharN<char, TPCH_READ_P_TYPE_LEN + 1>(tbl_JOIN_INNER_TD_2163_consolidated_output[p_idx].getInt32(i, 2), 2);
        int32_t _p_size263 = tbl_JOIN_INNER_TD_2163_consolidated_output[p_idx].getInt32(i, 3);
        SW_Aggregate_TD_1520_key k{std::string(_p_brand261.data()), std::string(_p_type262.data()), _p_size263};
        int64_t _supplier_cnt1548L_count_0 = _ps_suppkey306 != 0 ? 1 : 0;
        SW_Aggregate_TD_1520_payload p{_supplier_cnt1548L_count_0};
        auto it = ht1.find(k);
        if (it != ht1.end()) {
            int64_t count_0 = (it->second)._supplier_cnt1548L_count_0 + _supplier_cnt1548L_count_0;
            p._supplier_cnt1548L_count_0 = count_0;
            ht1[k] = p;
        } else { 
            ht1.insert(std::make_pair(k, p));
        }
    }
}
    int r = 0;
    for (auto& it : ht1) {
        std::array<char, TPCH_READ_P_BRND_LEN + 1> _p_brand261{};
        memcpy(_p_brand261.data(), ((it.first)._p_brand261).data(), ((it.first)._p_brand261).length());
        tbl_Aggregate_TD_1520_output.setcharN<char, TPCH_READ_P_BRND_LEN + 1>(r, 0, _p_brand261);
        std::array<char, TPCH_READ_P_TYPE_LEN + 1> _p_type262{};
        memcpy(_p_type262.data(), ((it.first)._p_type262).data(), ((it.first)._p_type262).length());
        tbl_Aggregate_TD_1520_output.setcharN<char, TPCH_READ_P_TYPE_LEN + 1>(r, 1, _p_type262);
        tbl_Aggregate_TD_1520_output.setInt32(r, 2, (it.first)._p_size263);
        int64_t _supplier_cnt1548L = (it.second)._supplier_cnt1548L_count_0;
        tbl_Aggregate_TD_1520_output.setInt64(r, 3, _supplier_cnt1548L);
        ++r;
    }
    tbl_Aggregate_TD_1520_output.setNumRow(r);
    std::cout << "tbl_Aggregate_TD_1520_output #Row: " << tbl_Aggregate_TD_1520_output.getNumRow() << std::endl;
}

void SW_Sort_TD_074(Table &tbl_Aggregate_TD_1520_output, Table &tbl_Sort_TD_074_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: Sort
    // Operation: ListBuffer(supplier_cnt#1548L DESC NULLS LAST, p_brand#261 ASC NULLS FIRST, p_type#262 ASC NULLS FIRST, p_size#263 ASC NULLS FIRST)
    // Input: ListBuffer(p_brand#261, p_type#262, p_size#263, supplier_cnt#1548L)
    // Output: ListBuffer(p_brand#261, p_type#262, p_size#263, supplier_cnt#1548L)
    struct SW_Sort_TD_074Row {
        std::string _p_brand261;
        std::string _p_type262;
        int32_t _p_size263;
        int64_t _supplier_cnt1548L;
    }; 

    struct {
        bool operator()(const SW_Sort_TD_074Row& a, const SW_Sort_TD_074Row& b) const { return 
 (a._supplier_cnt1548L > b._supplier_cnt1548L) || 
 ((a._supplier_cnt1548L == b._supplier_cnt1548L) && (a._p_brand261 < b._p_brand261)) || 
 ((a._supplier_cnt1548L == b._supplier_cnt1548L) && (a._p_brand261 == b._p_brand261) && (a._p_type262 < b._p_type262)) || 
 ((a._supplier_cnt1548L == b._supplier_cnt1548L) && (a._p_brand261 == b._p_brand261) && (a._p_type262 == b._p_type262) && (a._p_size263 < b._p_size263)); 
}
    }SW_Sort_TD_074_order; 

    int nrow1 = tbl_Aggregate_TD_1520_output.getNumRow();
    std::vector<SW_Sort_TD_074Row> rows;
    for (int i = 0; i < nrow1; i++) {
        std::array<char, TPCH_READ_P_BRND_LEN + 1> _p_brand261 = tbl_Aggregate_TD_1520_output.getcharN<char, TPCH_READ_P_BRND_LEN +1>(i, 0);
        std::array<char, TPCH_READ_P_TYPE_LEN + 1> _p_type262 = tbl_Aggregate_TD_1520_output.getcharN<char, TPCH_READ_P_TYPE_LEN +1>(i, 1);
        int32_t _p_size263 = tbl_Aggregate_TD_1520_output.getInt32(i, 2);
        int64_t _supplier_cnt1548L = tbl_Aggregate_TD_1520_output.getInt64(i, 3);
        SW_Sort_TD_074Row t = {std::string(_p_brand261.data()),std::string(_p_type262.data()),_p_size263,_supplier_cnt1548L};
        rows.push_back(t);
    }
    std::sort(rows.begin(), rows.end(), SW_Sort_TD_074_order);
    int r = 0;
    for (auto& it : rows) {
        std::array<char, TPCH_READ_P_BRND_LEN + 1> _p_brand261{};
        memcpy(_p_brand261.data(), (it._p_brand261).data(), (it._p_brand261).length());
        tbl_Sort_TD_074_output.setcharN<char, TPCH_READ_P_BRND_LEN +1>(r, 0, _p_brand261);
        std::array<char, TPCH_READ_P_TYPE_LEN + 1> _p_type262{};
        memcpy(_p_type262.data(), (it._p_type262).data(), (it._p_type262).length());
        tbl_Sort_TD_074_output.setcharN<char, TPCH_READ_P_TYPE_LEN +1>(r, 1, _p_type262);
        tbl_Sort_TD_074_output.setInt32(r, 2, it._p_size263);
        tbl_Sort_TD_074_output.setInt64(r, 3, it._supplier_cnt1548L);
        if (r < 10) {
            std::cout << (it._p_brand261).data() << " " << (it._p_type262).data() << " " << it._p_size263 << " " << it._supplier_cnt1548L << " " << std::endl;
        }
        ++r;
    }
    tbl_Sort_TD_074_output.setNumRow(r);
    std::cout << "tbl_Sort_TD_074_output #Row: " << tbl_Sort_TD_074_output.getNumRow() << std::endl;
}

