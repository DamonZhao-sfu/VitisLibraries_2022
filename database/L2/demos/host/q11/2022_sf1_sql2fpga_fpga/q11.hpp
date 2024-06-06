#include <regex> 
#include <stdint.h> 

void SW_Filter_TD_4676(Table &tbl_SerializeFromObject_TD_5998_input, Table &tbl_Filter_TD_4676_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: Filter
    // Operation: ListBuffer((isnotnull(n_name#145) AND (n_name#145 = IRAN)))
    // Input: ListBuffer(n_nationkey#144, n_name#145)
    // Output: ListBuffer(n_nationkey#144)
    int r = 0;
    int nrow1 = tbl_SerializeFromObject_TD_5998_input.getNumRow();
    for (int i = 0; i < nrow1; i++) {
        std::array<char, TPCH_READ_NATION_LEN + 1> _n_name145 = tbl_SerializeFromObject_TD_5998_input.getcharN<char, TPCH_READ_NATION_LEN + 1>(i, 1);
        if ((1) && (std::string(_n_name145.data()) == "IRAN")) {
            int32_t _n_nationkey144_t = tbl_SerializeFromObject_TD_5998_input.getInt32(i, 0);
            tbl_Filter_TD_4676_output.setInt32(r, 0, _n_nationkey144_t);
            r++;
        }
    }
    tbl_Filter_TD_4676_output.setNumRow(r);
    std::cout << "tbl_Filter_TD_4676_output #Row: " << tbl_Filter_TD_4676_output.getNumRow() << std::endl;
}

void SW_Consolidated_JOIN_INNER_TD_3883_output(Table& in1, Table&in2, Table& tbl_JOIN_INNER_TD_3883_output, Table& tbl_JOIN_INNER_TD_3883_consolidated_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: JOIN_INNER
    // Operation: ListBuffer((s_nationkey#345 = n_nationkey#144))
    // Left Table: ListBuffer(n_nationkey#144)
    // Right Table: ListBuffer(s_suppkey#342, s_nationkey#345)
    // Output Table: ListBuffer(s_suppkey#342)
    // Node Depth: 3
    int num_rows = tbl_JOIN_INNER_TD_3883_output.getNumRow();
        for (int j = 0; j < num_rows; j++) {
          int32_t rowid_1 = tbl_JOIN_INNER_TD_3883_output.getInt32(j, 0);
          int32_t rowid_2 = tbl_JOIN_INNER_TD_3883_output.getInt32(j, 1);
          int64_t key = tbl_JOIN_INNER_TD_3883_output.getInt32(j, 2);
          tbl_JOIN_INNER_TD_3883_consolidated_output.setInt32(j, 0, in2.getInt32(rowid_1 - 1, 0));
        }
        tbl_JOIN_INNER_TD_3883_consolidated_output.setNumRow(num_rows);
    std::cout << "tbl_JOIN_INNER_TD_3883_consolidated_output #Row: " << tbl_JOIN_INNER_TD_3883_consolidated_output.getNumRow() << std::endl;
}

void SW_Consolidated_JOIN_INNER_TD_4820_output(Table& in1, Table&in2, Table& tbl_JOIN_INNER_TD_4820_output, Table& tbl_JOIN_INNER_TD_4820_consolidated_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: JOIN_INNER
    // Operation: ListBuffer((ps_suppkey#306 = s_suppkey#342))
    // Left Table: ListBuffer(s_suppkey#342)
    // Right Table: ListBuffer(ps_partkey#305, ps_suppkey#306, ps_availqty#307, ps_supplycost#308)
    // Output Table: ListBuffer(ps_partkey#305, ps_availqty#307, ps_supplycost#308)
    // Node Depth: 4
    int num_rows = tbl_JOIN_INNER_TD_4820_output.getNumRow();
        for (int j = 0; j < num_rows; j++) {
          int32_t rowid_1 = tbl_JOIN_INNER_TD_4820_output.getInt32(j, 0);
          int32_t rowid_2 = tbl_JOIN_INNER_TD_4820_output.getInt32(j, 1);
          int64_t key = tbl_JOIN_INNER_TD_4820_output.getInt32(j, 2);
          tbl_JOIN_INNER_TD_4820_consolidated_output.setInt32(j, 0, in2.getInt32(rowid_1 - 1, 0));
          tbl_JOIN_INNER_TD_4820_consolidated_output.setInt32(j, 1, in2.getInt32(rowid_1 - 1, 2));
          tbl_JOIN_INNER_TD_4820_consolidated_output.setInt32(j, 2, in2.getInt32(rowid_1 - 1, 3));
        }
        tbl_JOIN_INNER_TD_4820_consolidated_output.setNumRow(num_rows);
    std::cout << "tbl_JOIN_INNER_TD_4820_consolidated_output #Row: " << tbl_JOIN_INNER_TD_4820_consolidated_output.getNumRow() << std::endl;
}

void SW_Aggregate_TD_2811(Table &tbl_JOIN_INNER_TD_4820_consolidated_output, Table &tbl_Aggregate_TD_2811_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: Aggregate
    // Operation: ListBuffer(CheckOverflow((promote_precision(cast(cast(sum((ps_supplycost#308 * ps_availqty#307)) as decimal(20,0)) as decimal(30,10))) * 0.0001000000), DecimalType(31,10), true) AS (sum((ps_supplycost * ps_availqty)) * 0.0001000000)#1204)
    // Input: ListBuffer(ps_partkey#305, ps_availqty#307, ps_supplycost#308)
    // Output: ListBuffer((sum((ps_supplycost * ps_availqty)) * 0.0001000000)#1204)
    int64_t sum_0 = 0;
    int nrow1 = tbl_JOIN_INNER_TD_4820_consolidated_output.getNumRow();
    for (int i = 0; i < nrow1; i++) {
        int32_t _ps_partkey305 = tbl_JOIN_INNER_TD_4820_consolidated_output.getInt32(i, 0);
        int32_t _ps_availqty307 = tbl_JOIN_INNER_TD_4820_consolidated_output.getInt32(i, 1);
        int32_t _ps_supplycost308 = tbl_JOIN_INNER_TD_4820_consolidated_output.getInt32(i, 2);
        int64_t _000010000001204_sum_0 = (_ps_supplycost308 * _ps_availqty307);
        sum_0 += _000010000001204_sum_0;
    }
    int r = 0;
    int64_t _000010000001204 = (sum_0 * 0.0001000000);
    tbl_Aggregate_TD_2811_output.setInt64(r++, 0, _000010000001204);
    tbl_Aggregate_TD_2811_output.setNumRow(r);
    std::cout << "tbl_Aggregate_TD_2811_output #Row: " << tbl_Aggregate_TD_2811_output.getNumRow() << std::endl;
}

typedef int32_t SW_Aggregate_TD_2700_key;
struct SW_Aggregate_TD_2700_payload {
    int64_t _value1200L_sum_0;
};
void SW_Aggregate_TD_2700(Table &tbl_JOIN_INNER_TD_4820_consolidated_output, Table &tbl_Aggregate_TD_2700_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: Aggregate
    // Operation: ListBuffer(ps_partkey#305, sum((ps_supplycost#308 * ps_availqty#307)) AS value#1200L)
    // Input: ListBuffer(ps_partkey#305, ps_availqty#307, ps_supplycost#308)
    // Output: ListBuffer(ps_partkey#305, value#1200L)
    std::unordered_map<SW_Aggregate_TD_2700_key, SW_Aggregate_TD_2700_payload> ht1;
    int nrow1 = tbl_JOIN_INNER_TD_4820_consolidated_output.getNumRow();
    for (int i = 0; i < nrow1; i++) {
        int32_t _ps_partkey305 = tbl_JOIN_INNER_TD_4820_consolidated_output.getInt32(i, 0);
        int32_t _ps_availqty307 = tbl_JOIN_INNER_TD_4820_consolidated_output.getInt32(i, 1);
        int32_t _ps_supplycost308 = tbl_JOIN_INNER_TD_4820_consolidated_output.getInt32(i, 2);
        SW_Aggregate_TD_2700_key k = _ps_partkey305;
        int64_t _value1200L_sum_0 = (_ps_supplycost308 * _ps_availqty307);
        SW_Aggregate_TD_2700_payload p{_value1200L_sum_0};
        auto it = ht1.find(k);
        if (it != ht1.end()) {
            int64_t sum_0 = (it->second)._value1200L_sum_0 + _value1200L_sum_0;
            p._value1200L_sum_0 = sum_0;
            ht1[k] = p;
        } else { 
            ht1.insert(std::make_pair(k, p));
        }
    }
    int r = 0;
    for (auto& it : ht1) {
        tbl_Aggregate_TD_2700_output.setInt32(r, 0, (it.first));
        int64_t _value1200L = (it.second)._value1200L_sum_0;
        tbl_Aggregate_TD_2700_output.setInt64(r, 1, _value1200L);
        ++r;
    }
    tbl_Aggregate_TD_2700_output.setNumRow(r);
    std::cout << "tbl_Aggregate_TD_2700_output #Row: " << tbl_Aggregate_TD_2700_output.getNumRow() << std::endl;
}

void SW_Filter_TD_159(Table &tbl_Aggregate_TD_2700_output, Table &tbl_Aggregate_TD_2811_output, Table &tbl_Filter_TD_159_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: Filter
    // Operation: ListBuffer((isnotnull(value#1200L) AND (cast(cast(value#1200L as decimal(20,0)) as decimal(31,10)) > scalar-subquery#1201 [])))
    // Input: ListBuffer(ps_partkey#305, value#1200L)
    // Output: ListBuffer(ps_partkey#305, value#1200L)
    int r = 0;
    int nrow1 = tbl_Aggregate_TD_2700_output.getNumRow();
    for (int i = 0; i < nrow1; i++) {
        int64_t _value1200L = tbl_Aggregate_TD_2700_output.getInt64(i, 1);
        if ((1) && (_value1200L > tbl_Aggregate_TD_2811_output.getInt64(0, 0))) {
            int32_t _ps_partkey305_t = tbl_Aggregate_TD_2700_output.getInt32(i, 0);
            tbl_Filter_TD_159_output.setInt32(r, 0, _ps_partkey305_t);
            int64_t _value1200L_t = tbl_Aggregate_TD_2700_output.getInt64(i, 1);
            tbl_Filter_TD_159_output.setInt64(r, 1, _value1200L_t);
            r++;
        }
    }
    tbl_Filter_TD_159_output.setNumRow(r);
    std::cout << "tbl_Filter_TD_159_output #Row: " << tbl_Filter_TD_159_output.getNumRow() << std::endl;
}

void SW_Sort_TD_0476(Table &tbl_Filter_TD_159_output, Table &tbl_Sort_TD_0476_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: Sort
    // Operation: ListBuffer(value#1200L DESC NULLS LAST)
    // Input: ListBuffer(ps_partkey#305, value#1200L)
    // Output: ListBuffer(ps_partkey#305, value#1200L)
    struct SW_Sort_TD_0476Row {
        int32_t _ps_partkey305;
        int64_t _value1200L;
    }; 

    struct {
        bool operator()(const SW_Sort_TD_0476Row& a, const SW_Sort_TD_0476Row& b) const { return 
 (a._value1200L > b._value1200L); 
}
    }SW_Sort_TD_0476_order; 

    int nrow1 = tbl_Filter_TD_159_output.getNumRow();
    std::vector<SW_Sort_TD_0476Row> rows;
    for (int i = 0; i < nrow1; i++) {
        int32_t _ps_partkey305 = tbl_Filter_TD_159_output.getInt32(i, 0);
        int64_t _value1200L = tbl_Filter_TD_159_output.getInt64(i, 1);
        SW_Sort_TD_0476Row t = {_ps_partkey305,_value1200L};
        rows.push_back(t);
    }
    std::sort(rows.begin(), rows.end(), SW_Sort_TD_0476_order);
    int r = 0;
    for (auto& it : rows) {
        tbl_Sort_TD_0476_output.setInt32(r, 0, it._ps_partkey305);
        tbl_Sort_TD_0476_output.setInt64(r, 1, it._value1200L);
        if (r < 10) {
            std::cout << it._ps_partkey305 << " " << it._value1200L << " " << std::endl;
        }
        ++r;
    }
    tbl_Sort_TD_0476_output.setNumRow(r);
    std::cout << "tbl_Sort_TD_0476_output #Row: " << tbl_Sort_TD_0476_output.getNumRow() << std::endl;
}

