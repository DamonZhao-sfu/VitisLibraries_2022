#include <regex> 
#include <stdint.h> 

void SW_Filter_TD_5576(Table &tbl_SerializeFromObject_TD_6366_input, Table &tbl_Filter_TD_5576_output) {
    // StringRowIDSubstitution: true StringRowIDBackSubstitution: false
    // Supported operation: Filter
    // Operation: ListBuffer(((o_orderdate#211 >= 19940801) AND (o_orderdate#211 < 19941101)))
    // Input: ListBuffer(o_orderkey#207, o_custkey#208, o_orderdate#211)
    // Output: ListBuffer(o_orderkey#207, o_custkey#208)
    int r = 0;
    int nrow1 = tbl_SerializeFromObject_TD_6366_input.getNumRow();
    for (int i = 0; i < nrow1; i++) {
        int32_t _o_orderdate211 = tbl_SerializeFromObject_TD_6366_input.getInt32(i, 2);
        if ((_o_orderdate211 >= 19940801) && (_o_orderdate211 < 19941101)) {
            int32_t _o_orderkey207_t = tbl_SerializeFromObject_TD_6366_input.getInt32(i, 0);
            tbl_Filter_TD_5576_output.setInt32(r, 0, _o_orderkey207_t);
            int32_t _o_custkey208_t = tbl_SerializeFromObject_TD_6366_input.getInt32(i, 1);
            tbl_Filter_TD_5576_output.setInt32(r, 1, _o_custkey208_t);
            r++;
        }
    }
    tbl_Filter_TD_5576_output.setNumRow(r);
    std::cout << "tbl_Filter_TD_5576_output #Row: " << tbl_Filter_TD_5576_output.getNumRow() << std::endl;
}

void SW_Filter_TD_4349(Table &tbl_SerializeFromObject_TD_5277_input, Table &tbl_Filter_TD_4349_output) {
    // StringRowIDSubstitution: true StringRowIDBackSubstitution: false
    // Supported operation: Filter
    // Operation: ListBuffer((l_returnflag#85 = 82))
    // Input: ListBuffer(l_orderkey#77, l_extendedprice#82, l_discount#83, l_returnflag#85)
    // Output: ListBuffer(l_orderkey#77, l_extendedprice#82, l_discount#83)
    int r = 0;
    int nrow1 = tbl_SerializeFromObject_TD_5277_input.getNumRow();
    for (int i = 0; i < nrow1; i++) {
        int32_t _l_returnflag85 = tbl_SerializeFromObject_TD_5277_input.getInt32(i, 3);
        if (_l_returnflag85 == 82) {
            int32_t _l_orderkey77_t = tbl_SerializeFromObject_TD_5277_input.getInt32(i, 0);
            tbl_Filter_TD_4349_output.setInt32(r, 0, _l_orderkey77_t);
            int32_t _l_extendedprice82_t = tbl_SerializeFromObject_TD_5277_input.getInt32(i, 1);
            tbl_Filter_TD_4349_output.setInt32(r, 1, _l_extendedprice82_t);
            int32_t _l_discount83_t = tbl_SerializeFromObject_TD_5277_input.getInt32(i, 2);
            tbl_Filter_TD_4349_output.setInt32(r, 2, _l_discount83_t);
            r++;
        }
    }
    tbl_Filter_TD_4349_output.setNumRow(r);
    std::cout << "tbl_Filter_TD_4349_output #Row: " << tbl_Filter_TD_4349_output.getNumRow() << std::endl;
}

void SW_Consolidated_JOIN_INNER_TD_4407_output(Table& in1, Table&in2, Table& tbl_JOIN_INNER_TD_4407_output, Table& tbl_JOIN_INNER_TD_4407_consolidated_output) {
    // StringRowIDSubstitution: true StringRowIDBackSubstitution: false
    // Supported operation: JOIN_INNER
    // Operation: ListBuffer((c_custkey#22 = o_custkey#208))
    // Left Table: ListBuffer(o_orderkey#207, o_custkey#208)
    // Right Table: ListBuffer(c_custkey#22, c_name#23, c_nationkey#25, c_acctbal#27)
    // Output Table: ListBuffer(c_custkey#22, c_name#23, c_nationkey#25, c_acctbal#27, o_orderkey#207)
    // Node Depth: 4
    int num_rows = tbl_JOIN_INNER_TD_4407_output.getNumRow();
        for (int j = 0; j < num_rows; j++) {
          int32_t rowid_1 = tbl_JOIN_INNER_TD_4407_output.getInt32(j, 0);
          int32_t rowid_2 = tbl_JOIN_INNER_TD_4407_output.getInt32(j, 1);
          int64_t key = tbl_JOIN_INNER_TD_4407_output.getInt32(j, 2);
          tbl_JOIN_INNER_TD_4407_consolidated_output.setInt32(j, 0, in2.getInt32(rowid_1 - 1, 0));
          tbl_JOIN_INNER_TD_4407_consolidated_output.setInt32(j, 1, in2.getInt32(rowid_1 - 1, 1));
          tbl_JOIN_INNER_TD_4407_consolidated_output.setInt32(j, 2, in2.getInt32(rowid_1 - 1, 2));
          tbl_JOIN_INNER_TD_4407_consolidated_output.setInt32(j, 3, in2.getInt32(rowid_1 - 1, 3));
          tbl_JOIN_INNER_TD_4407_consolidated_output.setInt32(j, 4, in1.getInt32(rowid_2 - 1, 0));
        }
        tbl_JOIN_INNER_TD_4407_consolidated_output.setNumRow(num_rows);
    std::cout << "tbl_JOIN_INNER_TD_4407_consolidated_output #Row: " << tbl_JOIN_INNER_TD_4407_consolidated_output.getNumRow() << std::endl;
}

void SW_Consolidated_JOIN_INNER_TD_3705_output(Table& in1, Table&in2, Table& tbl_JOIN_INNER_TD_3705_output, Table& tbl_JOIN_INNER_TD_3705_consolidated_output) {
    // StringRowIDSubstitution: true StringRowIDBackSubstitution: false
    // Supported operation: JOIN_INNER
    // Operation: ListBuffer((l_orderkey#77 = o_orderkey#207))
    // Left Table: ListBuffer(l_orderkey#77, l_extendedprice#82, l_discount#83)
    // Right Table: ListBuffer(c_custkey#22, c_name#23, c_nationkey#25, c_acctbal#27, o_orderkey#207)
    // Output Table: ListBuffer(c_custkey#22, c_name#23, c_nationkey#25, c_acctbal#27, l_extendedprice#82, l_discount#83)
    // Node Depth: 3
    int num_rows = tbl_JOIN_INNER_TD_3705_output.getNumRow();
        for (int j = 0; j < num_rows; j++) {
          int32_t rowid_1 = tbl_JOIN_INNER_TD_3705_output.getInt32(j, 0);
          int32_t rowid_2 = tbl_JOIN_INNER_TD_3705_output.getInt32(j, 1);
          int64_t key = tbl_JOIN_INNER_TD_3705_output.getInt32(j, 2);
          tbl_JOIN_INNER_TD_3705_consolidated_output.setInt32(j, 0, in2.getInt32(rowid_1 - 1, 0));
          tbl_JOIN_INNER_TD_3705_consolidated_output.setInt32(j, 1, in2.getInt32(rowid_1 - 1, 1));
          tbl_JOIN_INNER_TD_3705_consolidated_output.setInt32(j, 2, in2.getInt32(rowid_1 - 1, 2));
          tbl_JOIN_INNER_TD_3705_consolidated_output.setInt32(j, 3, in2.getInt32(rowid_1 - 1, 3));
          tbl_JOIN_INNER_TD_3705_consolidated_output.setInt32(j, 4, in1.getInt32(rowid_2 - 1, 1));
          tbl_JOIN_INNER_TD_3705_consolidated_output.setInt32(j, 5, in1.getInt32(rowid_2 - 1, 2));
        }
        tbl_JOIN_INNER_TD_3705_consolidated_output.setNumRow(num_rows);
    std::cout << "tbl_JOIN_INNER_TD_3705_consolidated_output #Row: " << tbl_JOIN_INNER_TD_3705_consolidated_output.getNumRow() << std::endl;
}

void SW_Consolidated_JOIN_INNER_TD_2823_output(Table& in1, Table&in2, Table& tbl_JOIN_INNER_TD_2823_output, Table& tbl_JOIN_INNER_TD_2823_consolidated_output) {
    // StringRowIDSubstitution: true StringRowIDBackSubstitution: false
    // Supported operation: JOIN_INNER
    // Operation: ListBuffer((c_nationkey#25 = n_nationkey#144))
    // Left Table: ListBuffer(n_nationkey#144, n_name#145)
    // Right Table: ListBuffer(c_custkey#22, c_name#23, c_nationkey#25, c_acctbal#27, l_extendedprice#82, l_discount#83)
    // Output Table: ListBuffer(c_custkey#22, c_name#23, c_acctbal#27, l_extendedprice#82, l_discount#83, n_name#145)
    // Node Depth: 2
    int num_rows = tbl_JOIN_INNER_TD_2823_output.getNumRow();
        for (int j = 0; j < num_rows; j++) {
          int32_t rowid_1 = tbl_JOIN_INNER_TD_2823_output.getInt32(j, 0);
          int32_t rowid_2 = tbl_JOIN_INNER_TD_2823_output.getInt32(j, 1);
          int64_t key = tbl_JOIN_INNER_TD_2823_output.getInt32(j, 2);
          tbl_JOIN_INNER_TD_2823_consolidated_output.setInt32(j, 0, in2.getInt32(rowid_1 - 1, 0));
          tbl_JOIN_INNER_TD_2823_consolidated_output.setInt32(j, 1, in2.getInt32(rowid_1 - 1, 1));
          tbl_JOIN_INNER_TD_2823_consolidated_output.setInt32(j, 2, in2.getInt32(rowid_1 - 1, 3));
          tbl_JOIN_INNER_TD_2823_consolidated_output.setInt32(j, 3, in2.getInt32(rowid_1 - 1, 4));
          tbl_JOIN_INNER_TD_2823_consolidated_output.setInt32(j, 4, in2.getInt32(rowid_1 - 1, 5));
          tbl_JOIN_INNER_TD_2823_consolidated_output.setInt32(j, 5, in1.getInt32(rowid_2 - 1, 1));
        }
        tbl_JOIN_INNER_TD_2823_consolidated_output.setNumRow(num_rows);
    std::cout << "tbl_JOIN_INNER_TD_2823_consolidated_output #Row: " << tbl_JOIN_INNER_TD_2823_consolidated_output.getNumRow() << std::endl;
}

struct SW_Aggregate_TD_155_key {
    int32_t _c_custkey22;
    std::string _c_name23;
    int32_t _c_acctbal27;
    std::string _n_name145;
    bool operator==(const SW_Aggregate_TD_155_key& other) const { return (_c_custkey22 == other._c_custkey22) && (_c_name23 == other._c_name23) && (_c_acctbal27 == other._c_acctbal27) && (_n_name145 == other._n_name145); }
};
namespace std {
template <>
struct hash<SW_Aggregate_TD_155_key> {
    std::size_t operator() (const SW_Aggregate_TD_155_key& k) const {
        using std::size_t;
        using std::hash;
        using std::string;
        return (hash<int32_t>()(k._c_custkey22)) + (hash<string>()(k._c_name23)) + (hash<int32_t>()(k._c_acctbal27)) + (hash<string>()(k._n_name145));
    }
};
}
struct SW_Aggregate_TD_155_payload {
    int64_t _revenue1130L_sum_0;
};
void SW_Aggregate_TD_155(Table &tbl_JOIN_INNER_TD_2823_consolidated_output, Table &tbl_SerializeFromObject_TD_5309_input, Table &tbl_SerializeFromObject_TD_6366_input, Table &tbl_SerializeFromObject_TD_5277_input, Table &tbl_SerializeFromObject_TD_3662_input, Table &tbl_Aggregate_TD_155_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: true
    // Supported operation: Aggregate
    // Operation: ListBuffer(c_custkey#22, c_name#23, c_acctbal#27, n_name#145, sum((l_extendedprice#82 * (100 - l_discount#83))) AS revenue#1130L)
    // Input: ListBuffer(c_custkey#22, c_name#23, c_acctbal#27, l_extendedprice#82, l_discount#83, n_name#145)
    // Output: ListBuffer(c_custkey#22, c_name#23, revenue#1130L, c_acctbal#27, n_name#145)
    std::unordered_map<SW_Aggregate_TD_155_key, SW_Aggregate_TD_155_payload> ht1;
    int nrow1 = tbl_JOIN_INNER_TD_2823_consolidated_output.getNumRow();
    for (int i = 0; i < nrow1; i++) {
        int32_t _c_custkey22 = tbl_JOIN_INNER_TD_2823_consolidated_output.getInt32(i, 0);
        std::array<char, TPCH_READ_C_NAME_LEN + 1> _c_name23 = tbl_SerializeFromObject_TD_5309_input.getcharN<char, TPCH_READ_C_NAME_LEN + 1>(tbl_JOIN_INNER_TD_2823_consolidated_output.getInt32(i, 1), 1);
        int32_t _c_acctbal27 = tbl_JOIN_INNER_TD_2823_consolidated_output.getInt32(i, 2);
        int32_t _l_extendedprice82 = tbl_JOIN_INNER_TD_2823_consolidated_output.getInt32(i, 3);
        int32_t _l_discount83 = tbl_JOIN_INNER_TD_2823_consolidated_output.getInt32(i, 4);
        std::array<char, TPCH_READ_NATION_LEN + 1> _n_name145 = tbl_SerializeFromObject_TD_3662_input.getcharN<char, TPCH_READ_NATION_LEN + 1>(tbl_JOIN_INNER_TD_2823_consolidated_output.getInt32(i, 5), 1);
        SW_Aggregate_TD_155_key k{_c_custkey22, std::string(_c_name23.data()), _c_acctbal27, std::string(_n_name145.data())};
        int64_t _revenue1130L_sum_0 = (_l_extendedprice82 * (100 - _l_discount83));
        SW_Aggregate_TD_155_payload p{_revenue1130L_sum_0};
        auto it = ht1.find(k);
        if (it != ht1.end()) {
            int64_t sum_0 = (it->second)._revenue1130L_sum_0 + _revenue1130L_sum_0;
            p._revenue1130L_sum_0 = sum_0;
            ht1[k] = p;
        } else { 
            ht1.insert(std::make_pair(k, p));
        }
    }
    int r = 0;
    for (auto& it : ht1) {
        tbl_Aggregate_TD_155_output.setInt32(r, 0, (it.first)._c_custkey22);
        std::array<char, TPCH_READ_C_NAME_LEN + 1> _c_name23{};
        memcpy(_c_name23.data(), ((it.first)._c_name23).data(), ((it.first)._c_name23).length());
        tbl_Aggregate_TD_155_output.setcharN<char, TPCH_READ_C_NAME_LEN + 1>(r, 1, _c_name23);
        tbl_Aggregate_TD_155_output.setInt32(r, 3, (it.first)._c_acctbal27);
        std::array<char, TPCH_READ_NATION_LEN + 1> _n_name145{};
        memcpy(_n_name145.data(), ((it.first)._n_name145).data(), ((it.first)._n_name145).length());
        tbl_Aggregate_TD_155_output.setcharN<char, TPCH_READ_NATION_LEN + 1>(r, 4, _n_name145);
        int64_t _revenue1130L = (it.second)._revenue1130L_sum_0;
        tbl_Aggregate_TD_155_output.setInt64(r, 2, _revenue1130L);
        ++r;
    }
    tbl_Aggregate_TD_155_output.setNumRow(r);
    std::cout << "tbl_Aggregate_TD_155_output #Row: " << tbl_Aggregate_TD_155_output.getNumRow() << std::endl;
}

void SW_Sort_TD_0999(Table &tbl_Aggregate_TD_155_output, Table &tbl_Sort_TD_0999_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: Sort
    // Operation: ListBuffer(revenue#1130L DESC NULLS LAST)
    // Input: ListBuffer(c_custkey#22, c_name#23, revenue#1130L, c_acctbal#27, n_name#145)
    // Output: ListBuffer(c_custkey#22, c_name#23, revenue#1130L, c_acctbal#27, n_name#145)
    struct SW_Sort_TD_0999Row {
        int32_t _c_custkey22;
        std::string _c_name23;
        int64_t _revenue1130L;
        int32_t _c_acctbal27;
        std::string _n_name145;
    }; 

    struct {
        bool operator()(const SW_Sort_TD_0999Row& a, const SW_Sort_TD_0999Row& b) const { return 
 (a._revenue1130L > b._revenue1130L); 
}
    }SW_Sort_TD_0999_order; 

    int nrow1 = tbl_Aggregate_TD_155_output.getNumRow();
    std::vector<SW_Sort_TD_0999Row> rows;
    for (int i = 0; i < nrow1; i++) {
        int32_t _c_custkey22 = tbl_Aggregate_TD_155_output.getInt32(i, 0);
        std::array<char, TPCH_READ_C_NAME_LEN + 1> _c_name23 = tbl_Aggregate_TD_155_output.getcharN<char, TPCH_READ_C_NAME_LEN +1>(i, 1);
        int64_t _revenue1130L = tbl_Aggregate_TD_155_output.getInt64(i, 2);
        int32_t _c_acctbal27 = tbl_Aggregate_TD_155_output.getInt32(i, 3);
        std::array<char, TPCH_READ_NATION_LEN + 1> _n_name145 = tbl_Aggregate_TD_155_output.getcharN<char, TPCH_READ_NATION_LEN +1>(i, 4);
        SW_Sort_TD_0999Row t = {_c_custkey22,std::string(_c_name23.data()),_revenue1130L,_c_acctbal27,std::string(_n_name145.data())};
        rows.push_back(t);
    }
    std::sort(rows.begin(), rows.end(), SW_Sort_TD_0999_order);
    int r = 0;
    for (auto& it : rows) {
        tbl_Sort_TD_0999_output.setInt32(r, 0, it._c_custkey22);
        std::array<char, TPCH_READ_C_NAME_LEN + 1> _c_name23{};
        memcpy(_c_name23.data(), (it._c_name23).data(), (it._c_name23).length());
        tbl_Sort_TD_0999_output.setcharN<char, TPCH_READ_C_NAME_LEN +1>(r, 1, _c_name23);
        tbl_Sort_TD_0999_output.setInt64(r, 2, it._revenue1130L);
        tbl_Sort_TD_0999_output.setInt32(r, 3, it._c_acctbal27);
        std::array<char, TPCH_READ_NATION_LEN + 1> _n_name145{};
        memcpy(_n_name145.data(), (it._n_name145).data(), (it._n_name145).length());
        tbl_Sort_TD_0999_output.setcharN<char, TPCH_READ_NATION_LEN +1>(r, 4, _n_name145);
        if (r < 10) {
            std::cout << it._c_custkey22 << " " << (it._c_name23).data() << " " << it._revenue1130L << " " << it._c_acctbal27 << " " << (it._n_name145).data() << " " << std::endl;
        }
        ++r;
    }
    tbl_Sort_TD_0999_output.setNumRow(r);
    std::cout << "tbl_Sort_TD_0999_output #Row: " << tbl_Sort_TD_0999_output.getNumRow() << std::endl;
}

