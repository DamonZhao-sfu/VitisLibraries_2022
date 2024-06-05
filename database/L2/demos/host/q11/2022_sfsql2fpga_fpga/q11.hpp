#include <regex> 
#include <stdint.h> 

void SW_Filter_TD_4760(Table &tbl_SerializeFromObject_TD_5284_input, Table &tbl_Filter_TD_4760_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: Filter
    // Operation: ListBuffer((isnotnull(n_name#145) AND (n_name#145 = IRAN)))
    // Input: ListBuffer(n_nationkey#144, n_name#145)
    // Output: ListBuffer(n_nationkey#144)
    int r = 0;
    int nrow1 = tbl_SerializeFromObject_TD_5284_input.getNumRow();
    for (int i = 0; i < nrow1; i++) {
        std::array<char, TPCH_READ_NATION_LEN + 1> _n_name145 = tbl_SerializeFromObject_TD_5284_input.getcharN<char, TPCH_READ_NATION_LEN + 1>(i, 1);
        if ((1) && (std::string(_n_name145.data()) == "IRAN")) {
            int32_t _n_nationkey144_t = tbl_SerializeFromObject_TD_5284_input.getInt32(i, 0);
            tbl_Filter_TD_4760_output.setInt32(r, 0, _n_nationkey144_t);
            r++;
        }
    }
    tbl_Filter_TD_4760_output.setNumRow(r);
    std::cout << "tbl_Filter_TD_4760_output #Row: " << tbl_Filter_TD_4760_output.getNumRow() << std::endl;
}

void SW_Consolidated_JOIN_INNER_TD_3524_output(Table& in1, Table&in2, Table& tbl_JOIN_INNER_TD_3524_output, Table& tbl_JOIN_INNER_TD_3524_consolidated_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: JOIN_INNER
    // Operation: ListBuffer((s_nationkey#345 = n_nationkey#144))
    // Left Table: ListBuffer(n_nationkey#144)
    // Right Table: ListBuffer(s_suppkey#342, s_nationkey#345)
    // Output Table: ListBuffer(s_suppkey#342)
    // Node Depth: 3
    int num_rows = tbl_JOIN_INNER_TD_3524_output.getNumRow();
        for (int j = 0; j < num_rows; j++) {
          int32_t rowid_1 = tbl_JOIN_INNER_TD_3524_output.getInt32(j, 0);
          int32_t rowid_2 = tbl_JOIN_INNER_TD_3524_output.getInt32(j, 1);
          int64_t key = tbl_JOIN_INNER_TD_3524_output.getInt32(j, 2);
          tbl_JOIN_INNER_TD_3524_consolidated_output.setInt32(j, 0, in2.getInt32(rowid_1 - 1, 0));
        }
        tbl_JOIN_INNER_TD_3524_consolidated_output.setNumRow(num_rows);
    std::cout << "tbl_JOIN_INNER_TD_3524_consolidated_output #Row: " << tbl_JOIN_INNER_TD_3524_consolidated_output.getNumRow() << std::endl;
}

void SW_Consolidated_JOIN_INNER_TD_4140_output(Table& in1, Table&in2, Table& tbl_JOIN_INNER_TD_4140_output, Table& tbl_JOIN_INNER_TD_4140_consolidated_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: JOIN_INNER
    // Operation: ListBuffer((ps_suppkey#306 = s_suppkey#342))
    // Left Table: ListBuffer(s_suppkey#342)
    // Right Table: ListBuffer(ps_partkey#305, ps_suppkey#306, ps_availqty#307, ps_supplycost#308)
    // Output Table: ListBuffer(ps_partkey#305, ps_availqty#307, ps_supplycost#308)
    // Node Depth: 4
    int num_rows = tbl_JOIN_INNER_TD_4140_output.getNumRow();
        for (int j = 0; j < num_rows; j++) {
          int32_t rowid_1 = tbl_JOIN_INNER_TD_4140_output.getInt32(j, 0);
          int32_t rowid_2 = tbl_JOIN_INNER_TD_4140_output.getInt32(j, 1);
          int64_t key = tbl_JOIN_INNER_TD_4140_output.getInt32(j, 2);
          tbl_JOIN_INNER_TD_4140_consolidated_output.setInt32(j, 0, in2.getInt32(rowid_1 - 1, 0));
          tbl_JOIN_INNER_TD_4140_consolidated_output.setInt32(j, 1, in2.getInt32(rowid_1 - 1, 2));
          tbl_JOIN_INNER_TD_4140_consolidated_output.setInt32(j, 2, in2.getInt32(rowid_1 - 1, 3));
        }
        tbl_JOIN_INNER_TD_4140_consolidated_output.setNumRow(num_rows);
    std::cout << "tbl_JOIN_INNER_TD_4140_consolidated_output #Row: " << tbl_JOIN_INNER_TD_4140_consolidated_output.getNumRow() << std::endl;
}

void SW_Aggregate_TD_2675(Table *tbl_JOIN_INNER_TD_4140_consolidated_output, Table &tbl_Aggregate_TD_2675_output, int hpTimes) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: Aggregate
    // Operation: ListBuffer(CheckOverflow((promote_precision(cast(cast(sum((ps_supplycost#308 * ps_availqty#307)) as decimal(20,0)) as decimal(30,10))) * 0.0001000000), DecimalType(31,10), true) AS (sum((ps_supplycost * ps_availqty)) * 0.0001000000)#1201)
    // Input: ListBuffer(ps_partkey#305, ps_availqty#307, ps_supplycost#308)
    // Output: ListBuffer((sum((ps_supplycost * ps_availqty)) * 0.0001000000)#1201)
    int64_t sum_0 = 0;
for (int p_idx = 0; p_idx < hpTimes; p_idx++) {
    int nrow1 = tbl_JOIN_INNER_TD_4140_consolidated_output[p_idx].getNumRow();
    for (int i = 0; i < nrow1; i++) {
        int32_t _ps_partkey305 = tbl_JOIN_INNER_TD_4140_consolidated_output[p_idx].getInt32(i, 0);
        int32_t _ps_availqty307 = tbl_JOIN_INNER_TD_4140_consolidated_output[p_idx].getInt32(i, 1);
        int32_t _ps_supplycost308 = tbl_JOIN_INNER_TD_4140_consolidated_output[p_idx].getInt32(i, 2);
        int64_t _000010000001201_sum_0 = (_ps_supplycost308 * _ps_availqty307);
        sum_0 += _000010000001201_sum_0;
    }
}
    int r = 0;
    int64_t _000010000001201 = (sum_0 * 0.0001000000);
    tbl_Aggregate_TD_2675_output.setInt64(r++, 0, _000010000001201);
    tbl_Aggregate_TD_2675_output.setNumRow(r);
    std::cout << "tbl_Aggregate_TD_2675_output #Row: " << tbl_Aggregate_TD_2675_output.getNumRow() << std::endl;
}

typedef int32_t SW_Aggregate_TD_2261_key;
struct SW_Aggregate_TD_2261_payload {
    int64_t _value1197L_sum_0;
};
void SW_Aggregate_TD_2261(Table *tbl_JOIN_INNER_TD_4140_consolidated_output, Table &tbl_Aggregate_TD_2261_output, int hpTimes) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: Aggregate
    // Operation: ListBuffer(ps_partkey#305, sum((ps_supplycost#308 * ps_availqty#307)) AS value#1197L)
    // Input: ListBuffer(ps_partkey#305, ps_availqty#307, ps_supplycost#308)
    // Output: ListBuffer(ps_partkey#305, value#1197L)
    std::unordered_map<SW_Aggregate_TD_2261_key, SW_Aggregate_TD_2261_payload> ht1;
for (int p_idx = 0; p_idx < hpTimes; p_idx++) {
    int nrow1 = tbl_JOIN_INNER_TD_4140_consolidated_output[p_idx].getNumRow();
    for (int i = 0; i < nrow1; i++) {
        int32_t _ps_partkey305 = tbl_JOIN_INNER_TD_4140_consolidated_output[p_idx].getInt32(i, 0);
        int32_t _ps_availqty307 = tbl_JOIN_INNER_TD_4140_consolidated_output[p_idx].getInt32(i, 1);
        int32_t _ps_supplycost308 = tbl_JOIN_INNER_TD_4140_consolidated_output[p_idx].getInt32(i, 2);
        SW_Aggregate_TD_2261_key k = _ps_partkey305;
        int64_t _value1197L_sum_0 = (_ps_supplycost308 * _ps_availqty307);
        SW_Aggregate_TD_2261_payload p{_value1197L_sum_0};
        auto it = ht1.find(k);
        if (it != ht1.end()) {
            int64_t sum_0 = (it->second)._value1197L_sum_0 + _value1197L_sum_0;
            p._value1197L_sum_0 = sum_0;
            ht1[k] = p;
        } else { 
            ht1.insert(std::make_pair(k, p));
        }
    }
}
    int r = 0;
    for (auto& it : ht1) {
        tbl_Aggregate_TD_2261_output.setInt32(r, 0, (it.first));
        int64_t _value1197L = (it.second)._value1197L_sum_0;
        tbl_Aggregate_TD_2261_output.setInt64(r, 1, _value1197L);
        ++r;
    }
    tbl_Aggregate_TD_2261_output.setNumRow(r);
    std::cout << "tbl_Aggregate_TD_2261_output #Row: " << tbl_Aggregate_TD_2261_output.getNumRow() << std::endl;
}

void SW_Filter_TD_1217(Table &tbl_Aggregate_TD_2261_output, Table &tbl_Aggregate_TD_2675_output, Table &tbl_Filter_TD_1217_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: Filter
    // Operation: ListBuffer((isnotnull(value#1197L) AND (cast(cast(value#1197L as decimal(20,0)) as decimal(31,10)) > scalar-subquery#1198 [])))
    // Input: ListBuffer(ps_partkey#305, value#1197L)
    // Output: ListBuffer(ps_partkey#305, value#1197L)
    int r = 0;
    int nrow1 = tbl_Aggregate_TD_2261_output.getNumRow();
    for (int i = 0; i < nrow1; i++) {
        int64_t _value1197L = tbl_Aggregate_TD_2261_output.getInt64(i, 1);
        if ((1) && (_value1197L > tbl_Aggregate_TD_2675_output.getInt64(0, 0))) {
            int32_t _ps_partkey305_t = tbl_Aggregate_TD_2261_output.getInt32(i, 0);
            tbl_Filter_TD_1217_output.setInt32(r, 0, _ps_partkey305_t);
            int64_t _value1197L_t = tbl_Aggregate_TD_2261_output.getInt64(i, 1);
            tbl_Filter_TD_1217_output.setInt64(r, 1, _value1197L_t);
            r++;
        }
    }
    tbl_Filter_TD_1217_output.setNumRow(r);
    std::cout << "tbl_Filter_TD_1217_output #Row: " << tbl_Filter_TD_1217_output.getNumRow() << std::endl;
}

void SW_Sort_TD_088(Table &tbl_Filter_TD_1217_output, Table &tbl_Sort_TD_088_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: Sort
    // Operation: ListBuffer(value#1197L DESC NULLS LAST)
    // Input: ListBuffer(ps_partkey#305, value#1197L)
    // Output: ListBuffer(ps_partkey#305, value#1197L)
    struct SW_Sort_TD_088Row {
        int32_t _ps_partkey305;
        int64_t _value1197L;
    }; 

    struct {
        bool operator()(const SW_Sort_TD_088Row& a, const SW_Sort_TD_088Row& b) const { return 
 (a._value1197L > b._value1197L); 
}
    }SW_Sort_TD_088_order; 

    int nrow1 = tbl_Filter_TD_1217_output.getNumRow();
    std::vector<SW_Sort_TD_088Row> rows;
    for (int i = 0; i < nrow1; i++) {
        int32_t _ps_partkey305 = tbl_Filter_TD_1217_output.getInt32(i, 0);
        int64_t _value1197L = tbl_Filter_TD_1217_output.getInt64(i, 1);
        SW_Sort_TD_088Row t = {_ps_partkey305,_value1197L};
        rows.push_back(t);
    }
    std::sort(rows.begin(), rows.end(), SW_Sort_TD_088_order);
    int r = 0;
    for (auto& it : rows) {
        tbl_Sort_TD_088_output.setInt32(r, 0, it._ps_partkey305);
        tbl_Sort_TD_088_output.setInt64(r, 1, it._value1197L);
        if (r < 10) {
            std::cout << it._ps_partkey305 << " " << it._value1197L << " " << std::endl;
        }
        ++r;
    }
    tbl_Sort_TD_088_output.setNumRow(r);
    std::cout << "tbl_Sort_TD_088_output #Row: " << tbl_Sort_TD_088_output.getNumRow() << std::endl;
}

