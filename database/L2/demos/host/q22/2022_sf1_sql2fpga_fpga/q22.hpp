#include <regex> 
#include <stdint.h> 

void SW_Filter_TD_6918(Table &tbl_SerializeFromObject_TD_5916_input, Table &tbl_Filter_TD_6918_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: Filter
    // Operation: ListBuffer(((c_acctbal#27 > 0) AND substring(c_phone#26, 1, 2) IN (19,11,16,27,15,22,12)))
    // Input: ListBuffer(c_custkey#22, c_phone#26, c_acctbal#27)
    // Output: ListBuffer(c_acctbal#27)
    int r = 0;
    int nrow1 = tbl_SerializeFromObject_TD_5916_input.getNumRow();
    for (int i = 0; i < nrow1; i++) {
        int32_t _c_acctbal27 = tbl_SerializeFromObject_TD_5916_input.getInt32(i, 2);
        std::array<char, TPCH_READ_PHONE_LEN + 1> _c_phone26 = tbl_SerializeFromObject_TD_5916_input.getcharN<char, TPCH_READ_PHONE_LEN + 1>(i, 1);
        auto reuse_col_str_402 = std::string(_c_phone26.data()).substr(0, 2);
        if ((_c_acctbal27 > 0) && ((reuse_col_str_402 == "19") || (reuse_col_str_402 == "11") || (reuse_col_str_402 == "16") || (reuse_col_str_402 == "27") || (reuse_col_str_402 == "15") || (reuse_col_str_402 == "22") || (reuse_col_str_402 == "12") || (0))) {
            int32_t _c_acctbal27_t = tbl_SerializeFromObject_TD_5916_input.getInt32(i, 2);
            tbl_Filter_TD_6918_output.setInt32(r, 0, _c_acctbal27_t);
            r++;
        }
    }
    tbl_Filter_TD_6918_output.setNumRow(r);
    std::cout << "tbl_Filter_TD_6918_output #Row: " << tbl_Filter_TD_6918_output.getNumRow() << std::endl;
}

void SW_Aggregate_TD_5899(Table &tbl_Filter_TD_6918_output, Table &tbl_Aggregate_TD_5899_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: Aggregate
    // Operation: ListBuffer(avg(c_acctbal#27) AS avg(c_acctbal)#2045)
    // Input: ListBuffer(c_acctbal#27)
    // Output: ListBuffer(avg(c_acctbal)#2045)
    int64_t avg_0 = 0;
    int nrow1 = tbl_Filter_TD_6918_output.getNumRow();
    for (int i = 0; i < nrow1; i++) {
        int32_t _c_acctbal27 = tbl_Filter_TD_6918_output.getInt32(i, 0);
        int64_t _avgc_acctbal2045_avg_0 = _c_acctbal27;
        avg_0 = (avg_0 + _avgc_acctbal2045_avg_0);
    }
    int r = 0;
    int64_t _avgc_acctbal2045 = avg_0 / nrow1;
    tbl_Aggregate_TD_5899_output.setInt64(r++, 0, _avgc_acctbal2045);
    tbl_Aggregate_TD_5899_output.setNumRow(r);
    std::cout << "tbl_Aggregate_TD_5899_output #Row: " << tbl_Aggregate_TD_5899_output.getNumRow() << std::endl;
}

void SW_Filter_TD_4176(Table &tbl_SerializeFromObject_TD_5916_input, Table &tbl_Aggregate_TD_5899_output, Table &tbl_Filter_TD_4176_output) {
    // StringRowIDSubstitution: true StringRowIDBackSubstitution: false
    // Supported operation: Filter
    // Operation: ListBuffer((substring(c_phone#26, 1, 2) IN (19,11,16,27,15,22,12) AND (cast(c_acctbal#27 as double) > scalar-subquery#2039 [])))
    // Input: ListBuffer(c_custkey#22, c_phone#26, c_acctbal#27)
    // Output: ListBuffer(c_custkey#22, c_phone#26, c_acctbal#27)
    int r = 0;
    int nrow1 = tbl_SerializeFromObject_TD_5916_input.getNumRow();
    for (int i = 0; i < nrow1; i++) {
        std::array<char, TPCH_READ_PHONE_LEN + 1> _c_phone26 = tbl_SerializeFromObject_TD_5916_input.getcharN<char, TPCH_READ_PHONE_LEN + 1>(i, 1);
        int32_t _c_acctbal27 = tbl_SerializeFromObject_TD_5916_input.getInt32(i, 2);
        auto reuse_col_str_647 = std::string(_c_phone26.data()).substr(0, 2);
        if (((reuse_col_str_647 == "19") || (reuse_col_str_647 == "11") || (reuse_col_str_647 == "16") || (reuse_col_str_647 == "27") || (reuse_col_str_647 == "15") || (reuse_col_str_647 == "22") || (reuse_col_str_647 == "12") || (0)) && (_c_acctbal27 > tbl_Aggregate_TD_5899_output.getInt64(0, 0))) {
            int32_t _c_custkey22_t = tbl_SerializeFromObject_TD_5916_input.getInt32(i, 0);
            tbl_Filter_TD_4176_output.setInt32(r, 0, _c_custkey22_t);
            tbl_Filter_TD_4176_output.setInt32(r, 1, i);
            int32_t _c_acctbal27_t = tbl_SerializeFromObject_TD_5916_input.getInt32(i, 2);
            tbl_Filter_TD_4176_output.setInt32(r, 2, _c_acctbal27_t);
            r++;
        }
    }
    tbl_Filter_TD_4176_output.setNumRow(r);
    std::cout << "tbl_Filter_TD_4176_output #Row: " << tbl_Filter_TD_4176_output.getNumRow() << std::endl;
}

void SW_Consolidated_JOIN_LEFTANTI_TD_3623_output(Table& in1, Table&in2, Table& tbl_JOIN_LEFTANTI_TD_3623_output, Table& tbl_JOIN_LEFTANTI_TD_3623_consolidated_output) {
    // StringRowIDSubstitution: true StringRowIDBackSubstitution: false
    // Supported operation: JOIN_LEFTANTI
    // Operation: ListBuffer((o_custkey#208 = c_custkey#22))
    // Left Table: ListBuffer(o_custkey#208)
    // Right Table: ListBuffer(c_custkey#22, c_phone#26, c_acctbal#27)
    // Output Table: ListBuffer(c_phone#26, c_acctbal#27)
    // Node Depth: 3
    int num_rows = tbl_JOIN_LEFTANTI_TD_3623_output.getNumRow();
        for (int j = 0; j < num_rows; j++) {
          int32_t rowid_1 = tbl_JOIN_LEFTANTI_TD_3623_output.getInt32(j, 0);
          int32_t rowid_2 = tbl_JOIN_LEFTANTI_TD_3623_output.getInt32(j, 1);
          int64_t key = tbl_JOIN_LEFTANTI_TD_3623_output.getInt32(j, 2);
          tbl_JOIN_LEFTANTI_TD_3623_consolidated_output.setInt32(j, 0, in2.getInt32(rowid_1 - 1, 1));
          tbl_JOIN_LEFTANTI_TD_3623_consolidated_output.setInt32(j, 1, in2.getInt32(rowid_1 - 1, 2));
        }
        tbl_JOIN_LEFTANTI_TD_3623_consolidated_output.setNumRow(num_rows);
    std::cout << "tbl_JOIN_LEFTANTI_TD_3623_consolidated_output #Row: " << tbl_JOIN_LEFTANTI_TD_3623_consolidated_output.getNumRow() << std::endl;
}

void SW_Project_TD_2418(Table &tbl_JOIN_LEFTANTI_TD_3623_consolidated_output, Table &tbl_SerializeFromObject_TD_5916_input, Table &tbl_Project_TD_2418_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: true
    // Supported operation: Project
    // Operation: ListBuffer(substring(c_phone#26, 1, 2) AS cntrycode#2038)
    // Input: ListBuffer(c_phone#26, c_acctbal#27)
    // Output: ListBuffer(cntrycode#2038, c_acctbal#27)
    int nrow1 = tbl_JOIN_LEFTANTI_TD_3623_consolidated_output.getNumRow();
    for (int i = 0; i < nrow1; i++) {
        std::array<char, TPCH_READ_PHONE_LEN + 1> _c_phone26 = tbl_SerializeFromObject_TD_5916_input.getcharN<char, TPCH_READ_PHONE_LEN + 1>(tbl_JOIN_LEFTANTI_TD_3623_consolidated_output.getInt32(i, 0), 1);
        int32_t _c_acctbal27 = tbl_JOIN_LEFTANTI_TD_3623_consolidated_output.getInt32(i, 1);
        std::string _cntrycode2038_str = std::string(_c_phone26.data()).substr(0, 2);
        std::array<char, TPCH_READ_REGION_LEN + 1> _cntrycode2038{};
        memcpy(_cntrycode2038.data(), _cntrycode2038_str.data(), (_cntrycode2038_str).length());
        tbl_Project_TD_2418_output.setcharN<char, TPCH_READ_REGION_LEN + 1>(i, 0, _cntrycode2038);
        tbl_Project_TD_2418_output.setInt32(i, 1, _c_acctbal27);
    }
    tbl_Project_TD_2418_output.setNumRow(nrow1);
    std::cout << "tbl_Project_TD_2418_output #Row: " << tbl_Project_TD_2418_output.getNumRow() << std::endl;
}

typedef std::string SW_Aggregate_TD_1201_key;
struct SW_Aggregate_TD_1201_payload {
    int64_t _numcust2041L_count_0;
    int64_t _totacctbal2042L_sum_1;
};
void SW_Aggregate_TD_1201(Table &tbl_Project_TD_2418_output, Table &tbl_Aggregate_TD_1201_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: Aggregate
    // Operation: ListBuffer(cntrycode#2038, count(1) AS numcust#2041L, sum(c_acctbal#27) AS totacctbal#2042L)
    // Input: ListBuffer(cntrycode#2038, c_acctbal#27)
    // Output: ListBuffer(cntrycode#2038, numcust#2041L, totacctbal#2042L)
    std::unordered_map<SW_Aggregate_TD_1201_key, SW_Aggregate_TD_1201_payload> ht1;
    int nrow1 = tbl_Project_TD_2418_output.getNumRow();
    for (int i = 0; i < nrow1; i++) {
        std::array<char, TPCH_READ_REGION_LEN + 1> _cntrycode2038 = tbl_Project_TD_2418_output.getcharN<char, TPCH_READ_REGION_LEN + 1>(i, 0);
        int32_t _c_acctbal27 = tbl_Project_TD_2418_output.getInt32(i, 1);
        SW_Aggregate_TD_1201_key k = std::string(_cntrycode2038.data());
        int64_t _numcust2041L_count_0 = 1 != 0 ? 1 : 0;
        int64_t _totacctbal2042L_sum_1 = _c_acctbal27;
        SW_Aggregate_TD_1201_payload p{_numcust2041L_count_0, _totacctbal2042L_sum_1};
        auto it = ht1.find(k);
        if (it != ht1.end()) {
            int64_t count_0 = (it->second)._numcust2041L_count_0 + _numcust2041L_count_0;
            p._numcust2041L_count_0 = count_0;
            int64_t sum_1 = (it->second)._totacctbal2042L_sum_1 + _totacctbal2042L_sum_1;
            p._totacctbal2042L_sum_1 = sum_1;
            ht1[k] = p;
        } else { 
            ht1.insert(std::make_pair(k, p));
        }
    }
    int r = 0;
    for (auto& it : ht1) {
        std::array<char, TPCH_READ_REGION_LEN + 1> _cntrycode2038{};
        memcpy(_cntrycode2038.data(), (it.first).data(), (it.first).length());
        tbl_Aggregate_TD_1201_output.setcharN<char, TPCH_READ_REGION_LEN + 1>(r, 0, _cntrycode2038);
        int64_t _numcust2041L = (it.second)._numcust2041L_count_0;
        tbl_Aggregate_TD_1201_output.setInt64(r, 1, _numcust2041L);
        int64_t _totacctbal2042L = (it.second)._totacctbal2042L_sum_1;
        tbl_Aggregate_TD_1201_output.setInt64(r, 2, _totacctbal2042L);
        ++r;
    }
    tbl_Aggregate_TD_1201_output.setNumRow(r);
    std::cout << "tbl_Aggregate_TD_1201_output #Row: " << tbl_Aggregate_TD_1201_output.getNumRow() << std::endl;
}

void SW_Sort_TD_088(Table &tbl_Aggregate_TD_1201_output, Table &tbl_Sort_TD_088_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: Sort
    // Operation: ListBuffer(cntrycode#2038 ASC NULLS FIRST)
    // Input: ListBuffer(cntrycode#2038, numcust#2041L, totacctbal#2042L)
    // Output: ListBuffer(cntrycode#2038, numcust#2041L, totacctbal#2042L)
    struct SW_Sort_TD_088Row {
        std::string _cntrycode2038;
        int64_t _numcust2041L;
        int64_t _totacctbal2042L;
    }; 

    struct {
        bool operator()(const SW_Sort_TD_088Row& a, const SW_Sort_TD_088Row& b) const { return 
 (a._cntrycode2038 < b._cntrycode2038); 
}
    }SW_Sort_TD_088_order; 

    int nrow1 = tbl_Aggregate_TD_1201_output.getNumRow();
    std::vector<SW_Sort_TD_088Row> rows;
    for (int i = 0; i < nrow1; i++) {
        std::array<char, TPCH_READ_REGION_LEN + 1> _cntrycode2038 = tbl_Aggregate_TD_1201_output.getcharN<char, TPCH_READ_REGION_LEN +1>(i, 0);
        int64_t _numcust2041L = tbl_Aggregate_TD_1201_output.getInt64(i, 1);
        int64_t _totacctbal2042L = tbl_Aggregate_TD_1201_output.getInt64(i, 2);
        SW_Sort_TD_088Row t = {std::string(_cntrycode2038.data()),_numcust2041L,_totacctbal2042L};
        rows.push_back(t);
    }
    std::sort(rows.begin(), rows.end(), SW_Sort_TD_088_order);
    int r = 0;
    for (auto& it : rows) {
        std::array<char, TPCH_READ_REGION_LEN + 1> _cntrycode2038{};
        memcpy(_cntrycode2038.data(), (it._cntrycode2038).data(), (it._cntrycode2038).length());
        tbl_Sort_TD_088_output.setcharN<char, TPCH_READ_REGION_LEN +1>(r, 0, _cntrycode2038);
        tbl_Sort_TD_088_output.setInt64(r, 1, it._numcust2041L);
        tbl_Sort_TD_088_output.setInt64(r, 2, it._totacctbal2042L);
        if (r < 10) {
            std::cout << (it._cntrycode2038).data() << " " << it._numcust2041L << " " << it._totacctbal2042L << " " << std::endl;
        }
        ++r;
    }
    tbl_Sort_TD_088_output.setNumRow(r);
    std::cout << "tbl_Sort_TD_088_output #Row: " << tbl_Sort_TD_088_output.getNumRow() << std::endl;
}

