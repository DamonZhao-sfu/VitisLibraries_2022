#include <regex> 
#include <stdint.h> 

void SW_Filter_TD_3355(Table &tbl_SerializeFromObject_TD_4866_input, Table &tbl_Filter_TD_3355_output) {
    // StringRowIDSubstitution: true StringRowIDBackSubstitution: false
    // Supported operation: Filter
    // Operation: ListBuffer((l_commitdate#88 < l_receiptdate#89))
    // Input: ListBuffer(l_orderkey#77, l_commitdate#88, l_receiptdate#89)
    // Output: ListBuffer(l_orderkey#77)
    int r = 0;
    int nrow1 = tbl_SerializeFromObject_TD_4866_input.getNumRow();
    for (int i = 0; i < nrow1; i++) {
        int32_t _l_commitdate88 = tbl_SerializeFromObject_TD_4866_input.getInt32(i, 1);
        int32_t _l_receiptdate89 = tbl_SerializeFromObject_TD_4866_input.getInt32(i, 2);
        if (_l_commitdate88 < _l_receiptdate89) {
            int32_t _l_orderkey77_t = tbl_SerializeFromObject_TD_4866_input.getInt32(i, 0);
            tbl_Filter_TD_3355_output.setInt32(r, 0, _l_orderkey77_t);
            r++;
        }
    }
    tbl_Filter_TD_3355_output.setNumRow(r);
    std::cout << "tbl_Filter_TD_3355_output #Row: " << tbl_Filter_TD_3355_output.getNumRow() << std::endl;
}

void SW_Filter_TD_3480(Table &tbl_SerializeFromObject_TD_4535_input_stringRowIDSubstitute, Table &tbl_Filter_TD_3480_output) {
    // StringRowIDSubstitution: true StringRowIDBackSubstitution: false
    // Supported operation: Filter
    // Operation: ListBuffer(((o_orderdate#211 >= 19930701) AND (o_orderdate#211 < 19931001)))
    // Input: ListBuffer(o_orderkey#207, o_orderpriority#212, o_orderdate#211)
    // Output: ListBuffer(o_orderkey#207, o_orderpriority#212)
    int r = 0;
    int nrow1 = tbl_SerializeFromObject_TD_4535_input_stringRowIDSubstitute.getNumRow();
    for (int i = 0; i < nrow1; i++) {
        int32_t _o_orderdate211 = tbl_SerializeFromObject_TD_4535_input_stringRowIDSubstitute.getInt32(i, 2);
        if ((_o_orderdate211 >= 19930701) && (_o_orderdate211 < 19931001)) {
            int32_t _o_orderkey207_t = tbl_SerializeFromObject_TD_4535_input_stringRowIDSubstitute.getInt32(i, 0);
            tbl_Filter_TD_3480_output.setInt32(r, 0, _o_orderkey207_t);
            tbl_Filter_TD_3480_output.setInt32(r, 1, i);
            r++;
        }
    }
    tbl_Filter_TD_3480_output.setNumRow(r);
    std::cout << "tbl_Filter_TD_3480_output #Row: " << tbl_Filter_TD_3480_output.getNumRow() << std::endl;
}

void SW_Consolidated_JOIN_LEFTSEMI_TD_287_output(Table& in1, Table&in2, Table& tbl_JOIN_LEFTSEMI_TD_287_output, Table& tbl_JOIN_LEFTSEMI_TD_287_consolidated_output) {
    // StringRowIDSubstitution: true StringRowIDBackSubstitution: false
    // Supported operation: JOIN_LEFTSEMI
    // Operation: ListBuffer((l_orderkey#77 = o_orderkey#207))
    // Left Table: ListBuffer(l_orderkey#77)
    // Right Table: ListBuffer(o_orderkey#207, o_orderpriority#212)
    // Output Table: ListBuffer(o_orderpriority#212)
    // Node Depth: 2
    int num_rows = tbl_JOIN_LEFTSEMI_TD_287_output.getNumRow();
        for (int j = 0; j < num_rows; j++) {
          int32_t rowid_1 = tbl_JOIN_LEFTSEMI_TD_287_output.getInt32(j, 0);
          int32_t rowid_2 = tbl_JOIN_LEFTSEMI_TD_287_output.getInt32(j, 1);
          int64_t key = tbl_JOIN_LEFTSEMI_TD_287_output.getInt32(j, 2);
          tbl_JOIN_LEFTSEMI_TD_287_consolidated_output.setInt32(j, 0, in2.getInt32(rowid_1 - 1, 1));
        }
        tbl_JOIN_LEFTSEMI_TD_287_consolidated_output.setNumRow(num_rows);
    std::cout << "tbl_JOIN_LEFTSEMI_TD_287_consolidated_output #Row: " << tbl_JOIN_LEFTSEMI_TD_287_consolidated_output.getNumRow() << std::endl;
}

typedef std::string SW_Aggregate_TD_1120_key;
struct SW_Aggregate_TD_1120_payload {
    int64_t _order_count747L_count_0;
};
void SW_Aggregate_TD_1120(Table &tbl_JOIN_LEFTSEMI_TD_287_consolidated_output, Table &tbl_SerializeFromObject_TD_4535_input, Table &tbl_SerializeFromObject_TD_4866_input, Table &tbl_Aggregate_TD_1120_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: true
    // Supported operation: Aggregate
    // Operation: ListBuffer(o_orderpriority#212, count(1) AS order_count#747L)
    // Input: ListBuffer(o_orderpriority#212)
    // Output: ListBuffer(o_orderpriority#212, order_count#747L)
    std::unordered_map<SW_Aggregate_TD_1120_key, SW_Aggregate_TD_1120_payload> ht1;
    int nrow1 = tbl_JOIN_LEFTSEMI_TD_287_consolidated_output.getNumRow();
    for (int i = 0; i < nrow1; i++) {
        std::array<char, TPCH_READ_MAXAGG_LEN + 1> _o_orderpriority212 = tbl_SerializeFromObject_TD_4535_input.getcharN<char, TPCH_READ_MAXAGG_LEN + 1>(tbl_JOIN_LEFTSEMI_TD_287_consolidated_output.getInt32(i, 0), 1);
        SW_Aggregate_TD_1120_key k = std::string(_o_orderpriority212.data());
        int64_t _order_count747L_count_0 = 1 != 0 ? 1 : 0;
        SW_Aggregate_TD_1120_payload p{_order_count747L_count_0};
        auto it = ht1.find(k);
        if (it != ht1.end()) {
            int64_t count_0 = (it->second)._order_count747L_count_0 + _order_count747L_count_0;
            p._order_count747L_count_0 = count_0;
            ht1[k] = p;
        } else { 
            ht1.insert(std::make_pair(k, p));
        }
    }
    int r = 0;
    for (auto& it : ht1) {
        std::array<char, TPCH_READ_MAXAGG_LEN + 1> _o_orderpriority212{};
        memcpy(_o_orderpriority212.data(), (it.first).data(), (it.first).length());
        tbl_Aggregate_TD_1120_output.setcharN<char, TPCH_READ_MAXAGG_LEN + 1>(r, 0, _o_orderpriority212);
        int64_t _order_count747L = (it.second)._order_count747L_count_0;
        tbl_Aggregate_TD_1120_output.setInt64(r, 1, _order_count747L);
        ++r;
    }
    tbl_Aggregate_TD_1120_output.setNumRow(r);
    std::cout << "tbl_Aggregate_TD_1120_output #Row: " << tbl_Aggregate_TD_1120_output.getNumRow() << std::endl;
}

void SW_Sort_TD_0231(Table &tbl_Aggregate_TD_1120_output, Table &tbl_Sort_TD_0231_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: Sort
    // Operation: ListBuffer(o_orderpriority#212 ASC NULLS FIRST)
    // Input: ListBuffer(o_orderpriority#212, order_count#747L)
    // Output: ListBuffer(o_orderpriority#212, order_count#747L)
    struct SW_Sort_TD_0231Row {
        std::string _o_orderpriority212;
        int64_t _order_count747L;
    }; 

    struct {
        bool operator()(const SW_Sort_TD_0231Row& a, const SW_Sort_TD_0231Row& b) const { return 
 (a._o_orderpriority212 < b._o_orderpriority212); 
}
    }SW_Sort_TD_0231_order; 

    int nrow1 = tbl_Aggregate_TD_1120_output.getNumRow();
    std::vector<SW_Sort_TD_0231Row> rows;
    for (int i = 0; i < nrow1; i++) {
        std::array<char, TPCH_READ_MAXAGG_LEN + 1> _o_orderpriority212 = tbl_Aggregate_TD_1120_output.getcharN<char, TPCH_READ_MAXAGG_LEN +1>(i, 0);
        int64_t _order_count747L = tbl_Aggregate_TD_1120_output.getInt64(i, 1);
        SW_Sort_TD_0231Row t = {std::string(_o_orderpriority212.data()),_order_count747L};
        rows.push_back(t);
    }
    std::sort(rows.begin(), rows.end(), SW_Sort_TD_0231_order);
    int r = 0;
    for (auto& it : rows) {
        std::array<char, TPCH_READ_MAXAGG_LEN + 1> _o_orderpriority212{};
        memcpy(_o_orderpriority212.data(), (it._o_orderpriority212).data(), (it._o_orderpriority212).length());
        tbl_Sort_TD_0231_output.setcharN<char, TPCH_READ_MAXAGG_LEN +1>(r, 0, _o_orderpriority212);
        tbl_Sort_TD_0231_output.setInt64(r, 1, it._order_count747L);
        if (r < 10) {
            std::cout << (it._o_orderpriority212).data() << " " << it._order_count747L << " " << std::endl;
        }
        ++r;
    }
    tbl_Sort_TD_0231_output.setNumRow(r);
    std::cout << "tbl_Sort_TD_0231_output #Row: " << tbl_Sort_TD_0231_output.getNumRow() << std::endl;
}

