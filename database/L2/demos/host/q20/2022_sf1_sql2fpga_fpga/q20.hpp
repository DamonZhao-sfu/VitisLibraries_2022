#include <regex> 
#include <stdint.h> 

void SW_Filter_TD_772(Table &tbl_SerializeFromObject_TD_8228_input, Table &tbl_Filter_TD_772_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: Filter
    // Operation: ListBuffer((isnotnull(p_name#259) AND StartsWith(p_name#259, blush)))
    // Input: ListBuffer(p_partkey#258, p_name#259)
    // Output: ListBuffer(p_partkey#258)
    int r = 0;
    int nrow1 = tbl_SerializeFromObject_TD_8228_input.getNumRow();
    for (int i = 0; i < nrow1; i++) {
        std::array<char, TPCH_READ_P_NAME_LEN + 1> _p_name259 = tbl_SerializeFromObject_TD_8228_input.getcharN<char, TPCH_READ_P_NAME_LEN + 1>(i, 1);
        if ((1) && (std::string(_p_name259.data()).find("blush")==0)) {
            int32_t _p_partkey258_t = tbl_SerializeFromObject_TD_8228_input.getInt32(i, 0);
            tbl_Filter_TD_772_output.setInt32(r, 0, _p_partkey258_t);
            r++;
        }
    }
    tbl_Filter_TD_772_output.setNumRow(r);
    std::cout << "tbl_Filter_TD_772_output #Row: " << tbl_Filter_TD_772_output.getNumRow() << std::endl;
}

void SW_Filter_TD_7786(Table &tbl_SerializeFromObject_TD_8350_input, Table &tbl_Filter_TD_7786_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: Filter
    // Operation: ListBuffer(((l_shipdate#87 >= 19950101) AND (l_shipdate#87 < 19960101)))
    // Input: ListBuffer(l_partkey#78, l_suppkey#79, l_quantity#81, l_shipdate#87)
    // Output: ListBuffer(l_partkey#78, l_suppkey#79, l_quantity#81)
    int r = 0;
    int nrow1 = tbl_SerializeFromObject_TD_8350_input.getNumRow();
    for (int i = 0; i < nrow1; i++) {
        int32_t _l_shipdate87 = tbl_SerializeFromObject_TD_8350_input.getInt32(i, 3);
        if ((_l_shipdate87 >= 19950101) && (_l_shipdate87 < 19960101)) {
            int32_t _l_partkey78_t = tbl_SerializeFromObject_TD_8350_input.getInt32(i, 0);
            tbl_Filter_TD_7786_output.setInt32(r, 0, _l_partkey78_t);
            int32_t _l_suppkey79_t = tbl_SerializeFromObject_TD_8350_input.getInt32(i, 1);
            tbl_Filter_TD_7786_output.setInt32(r, 1, _l_suppkey79_t);
            int32_t _l_quantity81_t = tbl_SerializeFromObject_TD_8350_input.getInt32(i, 2);
            tbl_Filter_TD_7786_output.setInt32(r, 2, _l_quantity81_t);
            r++;
        }
    }
    tbl_Filter_TD_7786_output.setNumRow(r);
    std::cout << "tbl_Filter_TD_7786_output #Row: " << tbl_Filter_TD_7786_output.getNumRow() << std::endl;
}

void SW_Consolidated_JOIN_LEFTSEMI_TD_6393_output(Table& in1, Table&in2, Table& tbl_JOIN_LEFTSEMI_TD_6393_output, Table& tbl_JOIN_LEFTSEMI_TD_6393_consolidated_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: JOIN_LEFTSEMI
    // Operation: ListBuffer((l_partkey#78 = p_partkey#258))
    // Left Table: ListBuffer(p_partkey#258)
    // Right Table: ListBuffer(l_partkey#78, l_suppkey#79, l_quantity#81)
    // Output Table: ListBuffer(l_partkey#78, l_suppkey#79, l_quantity#81)
    // Node Depth: 6
    int num_rows = tbl_JOIN_LEFTSEMI_TD_6393_output.getNumRow();
        for (int j = 0; j < num_rows; j++) {
          int32_t rowid_1 = tbl_JOIN_LEFTSEMI_TD_6393_output.getInt32(j, 0);
          int32_t rowid_2 = tbl_JOIN_LEFTSEMI_TD_6393_output.getInt32(j, 1);
          int64_t key = tbl_JOIN_LEFTSEMI_TD_6393_output.getInt32(j, 2);
          tbl_JOIN_LEFTSEMI_TD_6393_consolidated_output.setInt32(j, 0, in2.getInt32(rowid_1 - 1, 0));
          tbl_JOIN_LEFTSEMI_TD_6393_consolidated_output.setInt32(j, 1, in2.getInt32(rowid_1 - 1, 1));
          tbl_JOIN_LEFTSEMI_TD_6393_consolidated_output.setInt32(j, 2, in2.getInt32(rowid_1 - 1, 2));
        }
        tbl_JOIN_LEFTSEMI_TD_6393_consolidated_output.setNumRow(num_rows);
    std::cout << "tbl_JOIN_LEFTSEMI_TD_6393_consolidated_output #Row: " << tbl_JOIN_LEFTSEMI_TD_6393_consolidated_output.getNumRow() << std::endl;
}

struct SW_Aggregate_TD_5596_key {
    int32_t _l_partkey78;
    int32_t _l_suppkey79;
    bool operator==(const SW_Aggregate_TD_5596_key& other) const { return (_l_partkey78 == other._l_partkey78) && (_l_suppkey79 == other._l_suppkey79); }
};
namespace std {
template <>
struct hash<SW_Aggregate_TD_5596_key> {
    std::size_t operator() (const SW_Aggregate_TD_5596_key& k) const {
        using std::size_t;
        using std::hash;
        using std::string;
        return (hash<int32_t>()(k._l_partkey78)) + (hash<int32_t>()(k._l_suppkey79));
    }
};
}
struct SW_Aggregate_TD_5596_payload {
    int64_t _suml_quantity1851L_sum_0;
};
void SW_Aggregate_TD_5596(Table &tbl_JOIN_LEFTSEMI_TD_6393_consolidated_output, Table &tbl_Aggregate_TD_5596_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: Aggregate
    // Operation: ListBuffer(l_partkey#78, l_suppkey#79, sum(l_quantity#81) AS sum(l_quantity)#1851L)
    // Input: ListBuffer(l_partkey#78, l_suppkey#79, l_quantity#81)
    // Output: ListBuffer(sum(l_quantity)#1851L, l_partkey#78, l_suppkey#79)
    std::unordered_map<SW_Aggregate_TD_5596_key, SW_Aggregate_TD_5596_payload> ht1;
    int nrow1 = tbl_JOIN_LEFTSEMI_TD_6393_consolidated_output.getNumRow();
    for (int i = 0; i < nrow1; i++) {
        int32_t _l_partkey78 = tbl_JOIN_LEFTSEMI_TD_6393_consolidated_output.getInt32(i, 0);
        int32_t _l_suppkey79 = tbl_JOIN_LEFTSEMI_TD_6393_consolidated_output.getInt32(i, 1);
        int32_t _l_quantity81 = tbl_JOIN_LEFTSEMI_TD_6393_consolidated_output.getInt32(i, 2);
        SW_Aggregate_TD_5596_key k{_l_partkey78, _l_suppkey79};
        int64_t _suml_quantity1851L_sum_0 = _l_quantity81;
        SW_Aggregate_TD_5596_payload p{_suml_quantity1851L_sum_0};
        auto it = ht1.find(k);
        if (it != ht1.end()) {
            int64_t sum_0 = (it->second)._suml_quantity1851L_sum_0 + _suml_quantity1851L_sum_0;
            p._suml_quantity1851L_sum_0 = sum_0;
            ht1[k] = p;
        } else { 
            ht1.insert(std::make_pair(k, p));
        }
    }
    int r = 0;
    for (auto& it : ht1) {
        tbl_Aggregate_TD_5596_output.setInt32(r, 1, (it.first)._l_partkey78);
        tbl_Aggregate_TD_5596_output.setInt32(r, 2, (it.first)._l_suppkey79);
        int64_t _suml_quantity1851L = (it.second)._suml_quantity1851L_sum_0;
        tbl_Aggregate_TD_5596_output.setInt64(r, 0, _suml_quantity1851L);
        ++r;
    }
    tbl_Aggregate_TD_5596_output.setNumRow(r);
    std::cout << "tbl_Aggregate_TD_5596_output #Row: " << tbl_Aggregate_TD_5596_output.getNumRow() << std::endl;
}

void SW_Filter_TD_57(Table &tbl_SerializeFromObject_TD_6549_input, Table &tbl_Filter_TD_57_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: Filter
    // Operation: ListBuffer((isnotnull(p_name#259) AND StartsWith(p_name#259, blush)))
    // Input: ListBuffer(p_partkey#258, p_name#259)
    // Output: ListBuffer(p_partkey#258)
    int r = 0;
    int nrow1 = tbl_SerializeFromObject_TD_6549_input.getNumRow();
    for (int i = 0; i < nrow1; i++) {
        std::array<char, TPCH_READ_P_NAME_LEN + 1> _p_name259 = tbl_SerializeFromObject_TD_6549_input.getcharN<char, TPCH_READ_P_NAME_LEN + 1>(i, 1);
        if ((1) && (std::string(_p_name259.data()).find("blush")==0)) {
            int32_t _p_partkey258_t = tbl_SerializeFromObject_TD_6549_input.getInt32(i, 0);
            tbl_Filter_TD_57_output.setInt32(r, 0, _p_partkey258_t);
            r++;
        }
    }
    tbl_Filter_TD_57_output.setNumRow(r);
    std::cout << "tbl_Filter_TD_57_output #Row: " << tbl_Filter_TD_57_output.getNumRow() << std::endl;
}

void SW_Filter_TD_4596(Table &tbl_Aggregate_TD_5596_output, Table &tbl_Filter_TD_4596_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: Filter
    // Operation: ListBuffer(isnotnull(sum(l_quantity)#1851L))
    // Input: ListBuffer(sum(l_quantity)#1851L, l_partkey#78, l_suppkey#79)
    // Output: ListBuffer(sum(l_quantity)#1851L, l_partkey#78, l_suppkey#79)
    int r = 0;
    int nrow1 = tbl_Aggregate_TD_5596_output.getNumRow();
    for (int i = 0; i < nrow1; i++) {
        int64_t _suml_quantity1851L = tbl_Aggregate_TD_5596_output.getInt64(i, 0);
        if (1) {
            int64_t _suml_quantity1851L_t = tbl_Aggregate_TD_5596_output.getInt64(i, 0);
            tbl_Filter_TD_4596_output.setInt64(r, 0, _suml_quantity1851L_t);
            int32_t _l_partkey78_t = tbl_Aggregate_TD_5596_output.getInt32(i, 1);
            tbl_Filter_TD_4596_output.setInt32(r, 1, _l_partkey78_t);
            int32_t _l_suppkey79_t = tbl_Aggregate_TD_5596_output.getInt32(i, 2);
            tbl_Filter_TD_4596_output.setInt32(r, 2, _l_suppkey79_t);
            r++;
        }
    }
    tbl_Filter_TD_4596_output.setNumRow(r);
    std::cout << "tbl_Filter_TD_4596_output #Row: " << tbl_Filter_TD_4596_output.getNumRow() << std::endl;
}

void SW_Consolidated_JOIN_LEFTSEMI_TD_4559_output(Table& in1, Table&in2, Table& tbl_JOIN_LEFTSEMI_TD_4559_output, Table& tbl_JOIN_LEFTSEMI_TD_4559_consolidated_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: JOIN_LEFTSEMI
    // Operation: ListBuffer((ps_partkey#305 = p_partkey#258))
    // Left Table: ListBuffer(p_partkey#258)
    // Right Table: ListBuffer(ps_partkey#305, ps_suppkey#306, ps_availqty#307)
    // Output Table: ListBuffer(ps_partkey#305, ps_suppkey#306, ps_availqty#307)
    // Node Depth: 4
    int num_rows = tbl_JOIN_LEFTSEMI_TD_4559_output.getNumRow();
        for (int j = 0; j < num_rows; j++) {
          int32_t rowid_1 = tbl_JOIN_LEFTSEMI_TD_4559_output.getInt32(j, 0);
          int32_t rowid_2 = tbl_JOIN_LEFTSEMI_TD_4559_output.getInt32(j, 1);
          int64_t key = tbl_JOIN_LEFTSEMI_TD_4559_output.getInt32(j, 2);
          tbl_JOIN_LEFTSEMI_TD_4559_consolidated_output.setInt32(j, 0, in2.getInt32(rowid_1 - 1, 0));
          tbl_JOIN_LEFTSEMI_TD_4559_consolidated_output.setInt32(j, 1, in2.getInt32(rowid_1 - 1, 1));
          tbl_JOIN_LEFTSEMI_TD_4559_consolidated_output.setInt32(j, 2, in2.getInt32(rowid_1 - 1, 2));
        }
        tbl_JOIN_LEFTSEMI_TD_4559_consolidated_output.setNumRow(num_rows);
    std::cout << "tbl_JOIN_LEFTSEMI_TD_4559_consolidated_output #Row: " << tbl_JOIN_LEFTSEMI_TD_4559_consolidated_output.getNumRow() << std::endl;
}

void SW_Consolidated_JOIN_INNER_TD_3497_output(Table& in1, Table&in2, Table& tbl_JOIN_INNER_TD_3497_output, Table& tbl_JOIN_INNER_TD_3497_consolidated_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: JOIN_INNER
    // Operation: ListBuffer((((cast(ps_availqty#307 as bigint) > sum(l_quantity)#1851L) AND (l_partkey#78 = ps_partkey#305)) AND (l_suppkey#79 = ps_suppkey#306)))
    // Left Table: ListBuffer(ps_partkey#305, ps_suppkey#306, ps_availqty#307)
    // Right Table: ListBuffer(sum(l_quantity)#1851L, l_partkey#78, l_suppkey#79)
    // Output Table: ListBuffer(ps_suppkey#306)
    // Node Depth: 3
    int num_rows = tbl_JOIN_INNER_TD_3497_output.getNumRow();
        for (int j = 0; j < num_rows; j++) {
          int32_t rowid_1 = tbl_JOIN_INNER_TD_3497_output.getInt32(j, 0);
          int32_t rowid_2 = tbl_JOIN_INNER_TD_3497_output.getInt32(j, 1);
          int64_t key = tbl_JOIN_INNER_TD_3497_output.getInt32(j, 2);
          tbl_JOIN_INNER_TD_3497_consolidated_output.setInt32(j, 0, in1.getInt32(rowid_2 - 1, 1));
        }
        tbl_JOIN_INNER_TD_3497_consolidated_output.setNumRow(num_rows);
    std::cout << "tbl_JOIN_INNER_TD_3497_consolidated_output #Row: " << tbl_JOIN_INNER_TD_3497_consolidated_output.getNumRow() << std::endl;
}

void SW_Filter_TD_215(Table &tbl_SerializeFromObject_TD_36_input, Table &tbl_Filter_TD_215_output) {
    // StringRowIDSubstitution: true StringRowIDBackSubstitution: false
    // Supported operation: Filter
    // Operation: ListBuffer((isnotnull(n_name#145) AND (n_name#145 = INDONESIA)))
    // Input: ListBuffer(n_nationkey#144, n_name#145)
    // Output: ListBuffer(n_nationkey#144)
    int r = 0;
    int nrow1 = tbl_SerializeFromObject_TD_36_input.getNumRow();
    for (int i = 0; i < nrow1; i++) {
        std::array<char, TPCH_READ_NATION_LEN + 1> _n_name145 = tbl_SerializeFromObject_TD_36_input.getcharN<char, TPCH_READ_NATION_LEN + 1>(i, 1);
        if ((1) && (std::string(_n_name145.data()) == "INDONESIA")) {
            int32_t _n_nationkey144_t = tbl_SerializeFromObject_TD_36_input.getInt32(i, 0);
            tbl_Filter_TD_215_output.setInt32(r, 0, _n_nationkey144_t);
            r++;
        }
    }
    tbl_Filter_TD_215_output.setNumRow(r);
    std::cout << "tbl_Filter_TD_215_output #Row: " << tbl_Filter_TD_215_output.getNumRow() << std::endl;
}

void SW_Consolidated_JOIN_LEFTSEMI_TD_2260_output(Table& in1, Table&in2, Table& tbl_JOIN_LEFTSEMI_TD_2260_output, Table& tbl_JOIN_LEFTSEMI_TD_2260_consolidated_output) {
    // StringRowIDSubstitution: true StringRowIDBackSubstitution: false
    // Supported operation: JOIN_LEFTSEMI
    // Operation: ListBuffer((s_suppkey#342 = ps_suppkey#306))
    // Left Table: ListBuffer(ps_suppkey#306)
    // Right Table: ListBuffer(s_suppkey#342, s_name#343, s_address#344, s_nationkey#345)
    // Output Table: ListBuffer(s_name#343, s_address#344, s_nationkey#345)
    // Node Depth: 2
    int num_rows = tbl_JOIN_LEFTSEMI_TD_2260_output.getNumRow();
        for (int j = 0; j < num_rows; j++) {
          int32_t rowid_1 = tbl_JOIN_LEFTSEMI_TD_2260_output.getInt32(j, 0);
          int32_t rowid_2 = tbl_JOIN_LEFTSEMI_TD_2260_output.getInt32(j, 1);
          int64_t key = tbl_JOIN_LEFTSEMI_TD_2260_output.getInt32(j, 2);
          tbl_JOIN_LEFTSEMI_TD_2260_consolidated_output.setInt32(j, 0, in2.getInt32(rowid_1 - 1, 1));
          tbl_JOIN_LEFTSEMI_TD_2260_consolidated_output.setInt32(j, 1, in2.getInt32(rowid_1 - 1, 2));
          tbl_JOIN_LEFTSEMI_TD_2260_consolidated_output.setInt32(j, 2, in2.getInt32(rowid_1 - 1, 3));
        }
        tbl_JOIN_LEFTSEMI_TD_2260_consolidated_output.setNumRow(num_rows);
    std::cout << "tbl_JOIN_LEFTSEMI_TD_2260_consolidated_output #Row: " << tbl_JOIN_LEFTSEMI_TD_2260_consolidated_output.getNumRow() << std::endl;
}

void SW_Consolidated_JOIN_INNER_TD_1225_output(Table& in1, Table&in2, Table& tbl_JOIN_INNER_TD_1225_output, Table& tbl_JOIN_INNER_TD_1225_consolidated_output) {
    // StringRowIDSubstitution: true StringRowIDBackSubstitution: false
    // Supported operation: JOIN_INNER
    // Operation: ListBuffer((s_nationkey#345 = n_nationkey#144))
    // Left Table: ListBuffer(s_name#343, s_address#344, s_nationkey#345)
    // Right Table: ListBuffer(n_nationkey#144)
    // Output Table: ListBuffer(s_name#343, s_address#344)
    // Node Depth: 1
    int num_rows = tbl_JOIN_INNER_TD_1225_output.getNumRow();
        for (int j = 0; j < num_rows; j++) {
          int32_t rowid_1 = tbl_JOIN_INNER_TD_1225_output.getInt32(j, 0);
          int32_t rowid_2 = tbl_JOIN_INNER_TD_1225_output.getInt32(j, 1);
          int64_t key = tbl_JOIN_INNER_TD_1225_output.getInt32(j, 2);
          tbl_JOIN_INNER_TD_1225_consolidated_output.setInt32(j, 0, in1.getInt32(rowid_2 - 1, 0));
          tbl_JOIN_INNER_TD_1225_consolidated_output.setInt32(j, 1, in1.getInt32(rowid_2 - 1, 1));
        }
        tbl_JOIN_INNER_TD_1225_consolidated_output.setNumRow(num_rows);
    std::cout << "tbl_JOIN_INNER_TD_1225_consolidated_output #Row: " << tbl_JOIN_INNER_TD_1225_consolidated_output.getNumRow() << std::endl;
}

void SW_Sort_TD_0177(Table &tbl_JOIN_INNER_TD_1225_consolidated_output, Table &tbl_SerializeFromObject_TD_3353_input, Table &tbl_SerializeFromObject_TD_36_input, Table &tbl_Sort_TD_0177_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: true
    // Supported operation: Sort
    // Operation: ListBuffer(s_name#343 ASC NULLS FIRST)
    // Input: ListBuffer(s_name#343, s_address#344)
    // Output: ListBuffer(s_name#343, s_address#344)
    struct SW_Sort_TD_0177Row {
        std::string _s_name343;
        std::string _s_address344;
    }; 

    struct {
        bool operator()(const SW_Sort_TD_0177Row& a, const SW_Sort_TD_0177Row& b) const { return 
 (a._s_name343 < b._s_name343); 
}
    }SW_Sort_TD_0177_order; 

    int nrow1 = tbl_JOIN_INNER_TD_1225_consolidated_output.getNumRow();
    std::vector<SW_Sort_TD_0177Row> rows;
    for (int i = 0; i < nrow1; i++) {
        std::array<char, TPCH_READ_S_NAME_LEN + 1> _s_name343 = tbl_SerializeFromObject_TD_3353_input.getcharN<char, TPCH_READ_S_NAME_LEN + 1>(tbl_JOIN_INNER_TD_1225_consolidated_output.getInt32(i, 0), 1);
        std::array<char, TPCH_READ_S_ADDR_MAX + 1> _s_address344 = tbl_SerializeFromObject_TD_3353_input.getcharN<char, TPCH_READ_S_ADDR_MAX + 1>(tbl_JOIN_INNER_TD_1225_consolidated_output.getInt32(i, 1), 2);
        SW_Sort_TD_0177Row t = {std::string(_s_name343.data()),std::string(_s_address344.data())};
        rows.push_back(t);
    }
    std::sort(rows.begin(), rows.end(), SW_Sort_TD_0177_order);
    int r = 0;
    for (auto& it : rows) {
        std::array<char, TPCH_READ_S_NAME_LEN + 1> _s_name343{};
        memcpy(_s_name343.data(), (it._s_name343).data(), (it._s_name343).length());
        tbl_Sort_TD_0177_output.setcharN<char, TPCH_READ_S_NAME_LEN +1>(r, 0, _s_name343);
        std::array<char, TPCH_READ_S_ADDR_MAX + 1> _s_address344{};
        memcpy(_s_address344.data(), (it._s_address344).data(), (it._s_address344).length());
        tbl_Sort_TD_0177_output.setcharN<char, TPCH_READ_S_ADDR_MAX +1>(r, 1, _s_address344);
        if (r < 10) {
            std::cout << (it._s_name343).data() << " " << (it._s_address344).data() << " " << std::endl;
        }
        ++r;
    }
    tbl_Sort_TD_0177_output.setNumRow(r);
    std::cout << "tbl_Sort_TD_0177_output #Row: " << tbl_Sort_TD_0177_output.getNumRow() << std::endl;
}

