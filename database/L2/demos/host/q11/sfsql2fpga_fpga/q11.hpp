#include <regex> 
#include <stdint.h> 

void SW_Filter_TD_4366(Table &tbl_SerializeFromObject_TD_5693_input, Table &tbl_Filter_TD_4366_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: Filter
    // Operation: ListBuffer((isnotnull(n_name#145) AND (n_name#145 = IRAN)))
    // Input: ListBuffer(n_nationkey#144, n_name#145)
    // Output: ListBuffer(n_nationkey#144)
    int r = 0;
    int nrow1 = tbl_SerializeFromObject_TD_5693_input.getNumRow();
    for (int i = 0; i < nrow1; i++) {
        std::array<char, TPCH_READ_NATION_LEN + 1> _n_name145 = tbl_SerializeFromObject_TD_5693_input.getcharN<char, TPCH_READ_NATION_LEN + 1>(i, 1);
        if ((1) && (std::string(_n_name145.data()) == "IRAN")) {
            int32_t _n_nationkey144_t = tbl_SerializeFromObject_TD_5693_input.getInt32(i, 0);
            tbl_Filter_TD_4366_output.setInt32(r, 0, _n_nationkey144_t);
            r++;
        }
    }
    tbl_Filter_TD_4366_output.setNumRow(r);
    std::cout << "tbl_Filter_TD_4366_output #Row: " << tbl_Filter_TD_4366_output.getNumRow() << std::endl;
}



void SW_Aggregate_TD_2214(Table *tbl_JOIN_INNER_TD_4851_output, Table &tbl_Aggregate_TD_2214_output, int hpTimes) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: Aggregate
    // Operation: ListBuffer(CheckOverflow((promote_precision(cast(cast(sum((ps_supplycost#308 * ps_availqty#307)) as decimal(20,0)) as decimal(30,10))) * 0.0001000000), DecimalType(31,10), true) AS (sum((ps_supplycost * ps_availqty)) * 0.0001000000)#1206)
    // Input: ListBuffer(ps_partkey#305, ps_availqty#307, ps_supplycost#308)
    // Output: ListBuffer((sum((ps_supplycost * ps_availqty)) * 0.0001000000)#1206)
    int64_t sum_0 = 0;
for (int p_idx = 0; p_idx < hpTimes; p_idx++) {
    int nrow1 = tbl_JOIN_INNER_TD_4851_output[p_idx].getNumRow();
    for (int i = 0; i < nrow1; i++) {
        int32_t _ps_partkey305 = tbl_JOIN_INNER_TD_4851_output[p_idx].getInt32(i, 0);
        int32_t _ps_availqty307 = tbl_JOIN_INNER_TD_4851_output[p_idx].getInt32(i, 1);
        int32_t _ps_supplycost308 = tbl_JOIN_INNER_TD_4851_output[p_idx].getInt32(i, 2);
        int64_t _000010000001206_sum_0 = (_ps_supplycost308 * _ps_availqty307);
        sum_0 += _000010000001206_sum_0;
    }
}
    int r = 0;
    int64_t _000010000001206 = (sum_0 * 0.0001000000);
    tbl_Aggregate_TD_2214_output.setInt64(r++, 0, _000010000001206);
    tbl_Aggregate_TD_2214_output.setNumRow(r);
    std::cout << "tbl_Aggregate_TD_2214_output #Row: " << tbl_Aggregate_TD_2214_output.getNumRow() << std::endl;
}

typedef int32_t SW_Aggregate_TD_2685_key;
struct SW_Aggregate_TD_2685_payload {
    int64_t _value1202L_sum_0;
};
void SW_Aggregate_TD_2685(Table *tbl_JOIN_INNER_TD_4851_output, Table &tbl_Aggregate_TD_2685_output, int hpTimes) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: Aggregate
    // Operation: ListBuffer(ps_partkey#305, sum((ps_supplycost#308 * ps_availqty#307)) AS value#1202L)
    // Input: ListBuffer(ps_partkey#305, ps_availqty#307, ps_supplycost#308)
    // Output: ListBuffer(ps_partkey#305, value#1202L)
    std::unordered_map<SW_Aggregate_TD_2685_key, SW_Aggregate_TD_2685_payload> ht1;
for (int p_idx = 0; p_idx < hpTimes; p_idx++) {
    int nrow1 = tbl_JOIN_INNER_TD_4851_output[p_idx].getNumRow();
    for (int i = 0; i < nrow1; i++) {
        int32_t _ps_partkey305 = tbl_JOIN_INNER_TD_4851_output[p_idx].getInt32(i, 0);
        int32_t _ps_availqty307 = tbl_JOIN_INNER_TD_4851_output[p_idx].getInt32(i, 1);
        int32_t _ps_supplycost308 = tbl_JOIN_INNER_TD_4851_output[p_idx].getInt32(i, 2);
        SW_Aggregate_TD_2685_key k = _ps_partkey305;
        int64_t _value1202L_sum_0 = (_ps_supplycost308 * _ps_availqty307);
        SW_Aggregate_TD_2685_payload p{_value1202L_sum_0};
        auto it = ht1.find(k);
        if (it != ht1.end()) {
            int64_t sum_0 = (it->second)._value1202L_sum_0 + _value1202L_sum_0;
            p._value1202L_sum_0 = sum_0;
            ht1[k] = p;
        } else { 
            ht1.insert(std::make_pair(k, p));
        }
    }
}
    int r = 0;
    for (auto& it : ht1) {
        tbl_Aggregate_TD_2685_output.setInt32(r, 0, (it.first));
        int64_t _value1202L = (it.second)._value1202L_sum_0;
        tbl_Aggregate_TD_2685_output.setInt64(r, 1, _value1202L);
        ++r;
    }
    tbl_Aggregate_TD_2685_output.setNumRow(r);
    std::cout << "tbl_Aggregate_TD_2685_output #Row: " << tbl_Aggregate_TD_2685_output.getNumRow() << std::endl;
}

void SW_Filter_TD_1648(Table &tbl_Aggregate_TD_2685_output, Table &tbl_Aggregate_TD_2214_output, Table &tbl_Filter_TD_1648_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: Filter
    // Operation: ListBuffer((isnotnull(value#1202L) AND (cast(cast(value#1202L as decimal(20,0)) as decimal(31,10)) > scalar-subquery#1203 [])))
    // Input: ListBuffer(ps_partkey#305, value#1202L)
    // Output: ListBuffer(ps_partkey#305, value#1202L)
    int r = 0;
    int nrow1 = tbl_Aggregate_TD_2685_output.getNumRow();
    for (int i = 0; i < nrow1; i++) {
        int64_t _value1202L = tbl_Aggregate_TD_2685_output.getInt64(i, 1);
        if ((1) && (_value1202L > tbl_Aggregate_TD_2214_output.getInt64(0, 0))) {
            int32_t _ps_partkey305_t = tbl_Aggregate_TD_2685_output.getInt32(i, 0);
            tbl_Filter_TD_1648_output.setInt32(r, 0, _ps_partkey305_t);
            int64_t _value1202L_t = tbl_Aggregate_TD_2685_output.getInt64(i, 1);
            tbl_Filter_TD_1648_output.setInt64(r, 1, _value1202L_t);
            r++;
        }
    }
    tbl_Filter_TD_1648_output.setNumRow(r);
    std::cout << "tbl_Filter_TD_1648_output #Row: " << tbl_Filter_TD_1648_output.getNumRow() << std::endl;
}

void SW_Sort_TD_0318(Table &tbl_Filter_TD_1648_output, Table &tbl_Sort_TD_0318_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: Sort
    // Operation: ListBuffer(value#1202L DESC NULLS LAST)
    // Input: ListBuffer(ps_partkey#305, value#1202L)
    // Output: ListBuffer(ps_partkey#305, value#1202L)
    struct SW_Sort_TD_0318Row {
        int32_t _ps_partkey305;
        int64_t _value1202L;
    }; 

    struct {
        bool operator()(const SW_Sort_TD_0318Row& a, const SW_Sort_TD_0318Row& b) const { return 
 (a._value1202L > b._value1202L); 
}
    }SW_Sort_TD_0318_order; 

    int nrow1 = tbl_Filter_TD_1648_output.getNumRow();
    std::vector<SW_Sort_TD_0318Row> rows;
    for (int i = 0; i < nrow1; i++) {
        int32_t _ps_partkey305 = tbl_Filter_TD_1648_output.getInt32(i, 0);
        int64_t _value1202L = tbl_Filter_TD_1648_output.getInt64(i, 1);
        SW_Sort_TD_0318Row t = {_ps_partkey305,_value1202L};
        rows.push_back(t);
    }
    std::sort(rows.begin(), rows.end(), SW_Sort_TD_0318_order);
    int r = 0;
    for (auto& it : rows) {
        tbl_Sort_TD_0318_output.setInt32(r, 0, it._ps_partkey305);
        tbl_Sort_TD_0318_output.setInt64(r, 1, it._value1202L);
        if (r < 10) {
            std::cout << it._ps_partkey305 << " " << it._value1202L << " " << std::endl;
        }
        ++r;
    }
    tbl_Sort_TD_0318_output.setNumRow(r);
    std::cout << "tbl_Sort_TD_0318_output #Row: " << tbl_Sort_TD_0318_output.getNumRow() << std::endl;
}

