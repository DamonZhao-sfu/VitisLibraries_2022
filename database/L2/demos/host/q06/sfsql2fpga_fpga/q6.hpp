#include <regex> 
#include <stdint.h> 

void SW_Filter_TD_1373(Table &tbl_SerializeFromObject_TD_2754_input, Table &tbl_Filter_TD_1373_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: Filter
    // Operation: ListBuffer((((l_shipdate#87 >= 19940101) AND (l_shipdate#87 < 19950101)) AND (((l_discount#83 >= 5) AND (l_discount#83 <= 7)) AND (l_quantity#81 < 24))))
    // Input: ListBuffer(l_extendedprice#82, l_discount#83, l_shipdate#87, l_quantity#81)
    // Output: ListBuffer(l_extendedprice#82, l_discount#83)
    int r = 0;
    int nrow1 = tbl_SerializeFromObject_TD_2754_input.getNumRow();
    for (int i = 0; i < nrow1; i++) {
        int32_t _l_shipdate87 = tbl_SerializeFromObject_TD_2754_input.getInt32(i, 2);
        int32_t _l_discount83 = tbl_SerializeFromObject_TD_2754_input.getInt32(i, 1);
        int32_t _l_quantity81 = tbl_SerializeFromObject_TD_2754_input.getInt32(i, 3);
        if (((_l_shipdate87 >= 19940101) && (_l_shipdate87 < 19950101)) && (((_l_discount83 >= 5) && (_l_discount83 <= 7)) && (_l_quantity81 < 24))) {
            int32_t _l_extendedprice82_t = tbl_SerializeFromObject_TD_2754_input.getInt32(i, 0);
            tbl_Filter_TD_1373_output.setInt32(r, 0, _l_extendedprice82_t);
            int32_t _l_discount83_t = tbl_SerializeFromObject_TD_2754_input.getInt32(i, 1);
            tbl_Filter_TD_1373_output.setInt32(r, 1, _l_discount83_t);
            r++;
        }
    }
    tbl_Filter_TD_1373_output.setNumRow(r);
    std::cout << "tbl_Filter_TD_1373_output #Row: " << tbl_Filter_TD_1373_output.getNumRow() << std::endl;
}

void SW_Aggregate_TD_0596(Table &tbl_Filter_TD_1373_output, Table &tbl_Aggregate_TD_0596_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: Aggregate
    // Operation: ListBuffer(sum((l_extendedprice#82 * l_discount#83)) AS revenue#842L)
    // Input: ListBuffer(l_extendedprice#82, l_discount#83)
    // Output: ListBuffer(revenue#842L)
    int64_t sum_0 = 0;
    int nrow1 = tbl_Filter_TD_1373_output.getNumRow();
    for (int i = 0; i < nrow1; i++) {
        int32_t _l_extendedprice82 = tbl_Filter_TD_1373_output.getInt32(i, 0);
        int32_t _l_discount83 = tbl_Filter_TD_1373_output.getInt32(i, 1);
        int64_t _revenue842L_sum_0 = (_l_extendedprice82 * _l_discount83);
        sum_0 += _revenue842L_sum_0;
    }
    int r = 0;
    int64_t _revenue842L = sum_0;
    tbl_Aggregate_TD_0596_output.setInt64(r++, 0, _revenue842L);
    tbl_Aggregate_TD_0596_output.setNumRow(r);
    std::cout << "tbl_Aggregate_TD_0596_output #Row: " << tbl_Aggregate_TD_0596_output.getNumRow() << std::endl;
}

