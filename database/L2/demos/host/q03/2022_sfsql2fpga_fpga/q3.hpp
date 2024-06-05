#include <regex> 
#include <stdint.h> 

void SW_Filter_TD_426(Table &tbl_SerializeFromObject_TD_526_input, Table &tbl_Filter_TD_426_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: Filter
    // Operation: ListBuffer((o_orderdate#211 < 19950307))
    // Input: ListBuffer(o_orderkey#207, o_custkey#208, o_orderdate#211, o_shippriority#214)
    // Output: ListBuffer(o_orderkey#207, o_custkey#208, o_orderdate#211, o_shippriority#214)
    int r = 0;
    int nrow1 = tbl_SerializeFromObject_TD_526_input.getNumRow();
    for (int i = 0; i < nrow1; i++) {
        int32_t _o_orderdate211 = tbl_SerializeFromObject_TD_526_input.getInt32(i, 2);
        if (_o_orderdate211 < 19950307) {
            int32_t _o_orderkey207_t = tbl_SerializeFromObject_TD_526_input.getInt32(i, 0);
            tbl_Filter_TD_426_output.setInt32(r, 0, _o_orderkey207_t);
            int32_t _o_custkey208_t = tbl_SerializeFromObject_TD_526_input.getInt32(i, 1);
            tbl_Filter_TD_426_output.setInt32(r, 1, _o_custkey208_t);
            int32_t _o_orderdate211_t = tbl_SerializeFromObject_TD_526_input.getInt32(i, 2);
            tbl_Filter_TD_426_output.setInt32(r, 2, _o_orderdate211_t);
            int32_t _o_shippriority214_t = tbl_SerializeFromObject_TD_526_input.getInt32(i, 3);
            tbl_Filter_TD_426_output.setInt32(r, 3, _o_shippriority214_t);
            r++;
        }
    }
    tbl_Filter_TD_426_output.setNumRow(r);
    std::cout << "tbl_Filter_TD_426_output #Row: " << tbl_Filter_TD_426_output.getNumRow() << std::endl;
}

void SW_Filter_TD_4727(Table &tbl_SerializeFromObject_TD_5928_input, Table &tbl_Filter_TD_4727_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: Filter
    // Operation: ListBuffer((isnotnull(c_mktsegment#28) AND (c_mktsegment#28 = MACHINERY)))
    // Input: ListBuffer(c_custkey#22, c_mktsegment#28)
    // Output: ListBuffer(c_custkey#22)
    int r = 0;
    int nrow1 = tbl_SerializeFromObject_TD_5928_input.getNumRow();
    for (int i = 0; i < nrow1; i++) {
        std::array<char, TPCH_READ_MAXAGG_LEN + 1> _c_mktsegment28 = tbl_SerializeFromObject_TD_5928_input.getcharN<char, TPCH_READ_MAXAGG_LEN + 1>(i, 1);
        if ((1) && (std::string(_c_mktsegment28.data()) == "MACHINERY")) {
            int32_t _c_custkey22_t = tbl_SerializeFromObject_TD_5928_input.getInt32(i, 0);
            tbl_Filter_TD_4727_output.setInt32(r, 0, _c_custkey22_t);
            r++;
        }
    }
    tbl_Filter_TD_4727_output.setNumRow(r);
    std::cout << "tbl_Filter_TD_4727_output #Row: " << tbl_Filter_TD_4727_output.getNumRow() << std::endl;
}

void SW_Consolidated_JOIN_INNER_TD_314_output(Table& in1, Table&in2, Table& tbl_JOIN_INNER_TD_314_output, Table& tbl_JOIN_INNER_TD_314_consolidated_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: JOIN_INNER
    // Operation: ListBuffer((c_custkey#22 = o_custkey#208))
    // Left Table: ListBuffer(c_custkey#22)
    // Right Table: ListBuffer(o_orderkey#207, o_custkey#208, o_orderdate#211, o_shippriority#214)
    // Output Table: ListBuffer(o_orderkey#207, o_orderdate#211, o_shippriority#214)
    // Node Depth: 3
    int num_rows = tbl_JOIN_INNER_TD_314_output.getNumRow();
        for (int j = 0; j < num_rows; j++) {
          int32_t rowid_1 = tbl_JOIN_INNER_TD_314_output.getInt32(j, 0);
          int32_t rowid_2 = tbl_JOIN_INNER_TD_314_output.getInt32(j, 1);
          int64_t key = tbl_JOIN_INNER_TD_314_output.getInt32(j, 2);
          tbl_JOIN_INNER_TD_314_consolidated_output.setInt32(j, 0, in2.getInt32(rowid_1 - 1, 0));
          tbl_JOIN_INNER_TD_314_consolidated_output.setInt32(j, 1, in2.getInt32(rowid_1 - 1, 2));
          tbl_JOIN_INNER_TD_314_consolidated_output.setInt32(j, 2, in2.getInt32(rowid_1 - 1, 3));
        }
        tbl_JOIN_INNER_TD_314_consolidated_output.setNumRow(num_rows);
    std::cout << "tbl_JOIN_INNER_TD_314_consolidated_output #Row: " << tbl_JOIN_INNER_TD_314_consolidated_output.getNumRow() << std::endl;
}

void SW_Filter_TD_3612(Table &tbl_SerializeFromObject_TD_4597_input, Table &tbl_Filter_TD_3612_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: Filter
    // Operation: ListBuffer((l_shipdate#87 > 19950307))
    // Input: ListBuffer(l_orderkey#77, l_extendedprice#82, l_discount#83, l_shipdate#87)
    // Output: ListBuffer(l_orderkey#77, l_extendedprice#82, l_discount#83)
    int r = 0;
    int nrow1 = tbl_SerializeFromObject_TD_4597_input.getNumRow();
    for (int i = 0; i < nrow1; i++) {
        int32_t _l_shipdate87 = tbl_SerializeFromObject_TD_4597_input.getInt32(i, 3);
        if (_l_shipdate87 > 19950307) {
            int32_t _l_orderkey77_t = tbl_SerializeFromObject_TD_4597_input.getInt32(i, 0);
            tbl_Filter_TD_3612_output.setInt32(r, 0, _l_orderkey77_t);
            int32_t _l_extendedprice82_t = tbl_SerializeFromObject_TD_4597_input.getInt32(i, 1);
            tbl_Filter_TD_3612_output.setInt32(r, 1, _l_extendedprice82_t);
            int32_t _l_discount83_t = tbl_SerializeFromObject_TD_4597_input.getInt32(i, 2);
            tbl_Filter_TD_3612_output.setInt32(r, 2, _l_discount83_t);
            r++;
        }
    }
    tbl_Filter_TD_3612_output.setNumRow(r);
    std::cout << "tbl_Filter_TD_3612_output #Row: " << tbl_Filter_TD_3612_output.getNumRow() << std::endl;
}


void SW_Aggregate_TD_133_consolidate(Table *tbl_Aggregate_TD_133_output_preprocess, Table &tbl_Aggregate_TD_133_output, int hpTimes) {
    int nrow = 0;
for (int p_idx = 0; p_idx < hpTimes; p_idx++) {
    int nrow_p = tbl_Aggregate_TD_133_output_preprocess[p_idx].getNumRow();
    for (int r(0); r<nrow_p; ++r) {
        int32_t _l_orderkey77 = tbl_Aggregate_TD_133_output_preprocess[p_idx].getInt32(r, 7);
        tbl_Aggregate_TD_133_output.setInt32(r, 0, _l_orderkey77);
        int64_t _revenue686L = tbl_Aggregate_TD_133_output_preprocess[p_idx].combineInt64(r, 8, 0);
        tbl_Aggregate_TD_133_output.setInt64(r, 1, _revenue686L);
        int32_t _o_orderdate211 = tbl_Aggregate_TD_133_output_preprocess[p_idx].getInt32(r, 6);
        tbl_Aggregate_TD_133_output.setInt32(r, 2, _o_orderdate211);
        int32_t _o_shippriority214 = tbl_Aggregate_TD_133_output_preprocess[p_idx].getInt32(r, 5);
        tbl_Aggregate_TD_133_output.setInt32(r, 3, _o_shippriority214);
    }
    nrow += nrow_p;
}
    tbl_Aggregate_TD_133_output.setNumRow(nrow);
    std::cout << "tbl_Aggregate_TD_133_output #Row: " << tbl_Aggregate_TD_133_output.getNumRow() << std::endl;
}

void SW_Sort_TD_0483(Table &tbl_Aggregate_TD_133_output, Table &tbl_Sort_TD_0483_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: Sort
    // Operation: ListBuffer(revenue#686L DESC NULLS LAST, o_orderdate#211 ASC NULLS FIRST)
    // Input: ListBuffer(l_orderkey#77, revenue#686L, o_orderdate#211, o_shippriority#214)
    // Output: ListBuffer(l_orderkey#77, revenue#686L, o_orderdate#211, o_shippriority#214)
    struct SW_Sort_TD_0483Row {
        int32_t _l_orderkey77;
        int64_t _revenue686L;
        int32_t _o_orderdate211;
        int32_t _o_shippriority214;
    }; 

    struct {
        bool operator()(const SW_Sort_TD_0483Row& a, const SW_Sort_TD_0483Row& b) const { return 
 (a._revenue686L > b._revenue686L) || 
 ((a._revenue686L == b._revenue686L) && (a._o_orderdate211 < b._o_orderdate211)); 
}
    }SW_Sort_TD_0483_order; 

    int nrow1 = tbl_Aggregate_TD_133_output.getNumRow();
    std::vector<SW_Sort_TD_0483Row> rows;
    for (int i = 0; i < nrow1; i++) {
        int32_t _l_orderkey77 = tbl_Aggregate_TD_133_output.getInt32(i, 0);
        int64_t _revenue686L = tbl_Aggregate_TD_133_output.getInt64(i, 1);
        int32_t _o_orderdate211 = tbl_Aggregate_TD_133_output.getInt32(i, 2);
        int32_t _o_shippriority214 = tbl_Aggregate_TD_133_output.getInt32(i, 3);
        SW_Sort_TD_0483Row t = {_l_orderkey77,_revenue686L,_o_orderdate211,_o_shippriority214};
        rows.push_back(t);
    }
    std::sort(rows.begin(), rows.end(), SW_Sort_TD_0483_order);
    int r = 0;
    for (auto& it : rows) {
        tbl_Sort_TD_0483_output.setInt32(r, 0, it._l_orderkey77);
        tbl_Sort_TD_0483_output.setInt64(r, 1, it._revenue686L);
        tbl_Sort_TD_0483_output.setInt32(r, 2, it._o_orderdate211);
        tbl_Sort_TD_0483_output.setInt32(r, 3, it._o_shippriority214);
        if (r < 10) {
            std::cout << it._l_orderkey77 << " " << it._revenue686L << " " << it._o_orderdate211 << " " << it._o_shippriority214 << " " << std::endl;
        }
        ++r;
    }
    tbl_Sort_TD_0483_output.setNumRow(r);
    std::cout << "tbl_Sort_TD_0483_output #Row: " << tbl_Sort_TD_0483_output.getNumRow() << std::endl;
}

