#include <regex> 
#include <stdint.h> 

void SW_Filter_TD_6381(Table &tbl_SerializeFromObject_TD_5322_input, Table &tbl_Filter_TD_6381_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: Filter
    // Operation: ListBuffer(((c_acctbal#27 > 0) AND substring(c_phone#26, 1, 2) IN (19,11,16,27,15,22,12)))
    // Input: ListBuffer(c_custkey#22, c_phone#26, c_acctbal#27)
    // Output: ListBuffer(c_acctbal#27)
    int r = 0;
    int nrow1 = tbl_SerializeFromObject_TD_5322_input.getNumRow();
    for (int i = 0; i < nrow1; i++) {
        int32_t _c_acctbal27 = tbl_SerializeFromObject_TD_5322_input.getInt32(i, 2);
        std::array<char, TPCH_READ_PHONE_LEN + 1> _c_phone26 = tbl_SerializeFromObject_TD_5322_input.getcharN<char, TPCH_READ_PHONE_LEN + 1>(i, 1);
        auto reuse_col_str_301 = std::string(_c_phone26.data()).substr(0, 2);
        if ((_c_acctbal27 > 0) && ((reuse_col_str_301 == "19") || (reuse_col_str_301 == "11") || (reuse_col_str_301 == "16") || (reuse_col_str_301 == "27") || (reuse_col_str_301 == "15") || (reuse_col_str_301 == "22") || (reuse_col_str_301 == "12") || (0))) {
            int32_t _c_acctbal27_t = tbl_SerializeFromObject_TD_5322_input.getInt32(i, 2);
            tbl_Filter_TD_6381_output.setInt32(r, 0, _c_acctbal27_t);
            r++;
        }
    }
    tbl_Filter_TD_6381_output.setNumRow(r);
    std::cout << "tbl_Filter_TD_6381_output #Row: " << tbl_Filter_TD_6381_output.getNumRow() << std::endl;
}

void SW_Aggregate_TD_5490(Table &tbl_Filter_TD_6381_output, Table &tbl_Aggregate_TD_5490_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: Aggregate
    // Operation: ListBuffer(avg(c_acctbal#27) AS avg(c_acctbal)#2042)
    // Input: ListBuffer(c_acctbal#27)
    // Output: ListBuffer(avg(c_acctbal)#2042)
    int64_t avg_0 = 0;
    int nrow1 = tbl_Filter_TD_6381_output.getNumRow();
    for (int i = 0; i < nrow1; i++) {
        int32_t _c_acctbal27 = tbl_Filter_TD_6381_output.getInt32(i, 0);
        int64_t _avgc_acctbal2042_avg_0 = _c_acctbal27;
        avg_0 = (avg_0 + _avgc_acctbal2042_avg_0);
    }
    int r = 0;
    int64_t _avgc_acctbal2042 = avg_0 / nrow1;
    tbl_Aggregate_TD_5490_output.setInt64(r++, 0, _avgc_acctbal2042);
    tbl_Aggregate_TD_5490_output.setNumRow(r);
    std::cout << "tbl_Aggregate_TD_5490_output #Row: " << tbl_Aggregate_TD_5490_output.getNumRow() << std::endl;
}

void SW_Filter_TD_4168(Table &tbl_SerializeFromObject_TD_5322_input, Table &tbl_Aggregate_TD_5490_output, Table &tbl_Filter_TD_4168_output) {
    // StringRowIDSubstitution: true StringRowIDBackSubstitution: false
    // Supported operation: Filter
    // Operation: ListBuffer((substring(c_phone#26, 1, 2) IN (19,11,16,27,15,22,12) AND (cast(c_acctbal#27 as double) > scalar-subquery#2036 [])))
    // Input: ListBuffer(c_custkey#22, c_phone#26, c_acctbal#27)
    // Output: ListBuffer(c_custkey#22, c_phone#26, c_acctbal#27)
    int r = 0;
    int nrow1 = tbl_SerializeFromObject_TD_5322_input.getNumRow();
    for (int i = 0; i < nrow1; i++) {
        std::array<char, TPCH_READ_PHONE_LEN + 1> _c_phone26 = tbl_SerializeFromObject_TD_5322_input.getcharN<char, TPCH_READ_PHONE_LEN + 1>(i, 1);
        int32_t _c_acctbal27 = tbl_SerializeFromObject_TD_5322_input.getInt32(i, 2);
        auto reuse_col_str_110 = std::string(_c_phone26.data()).substr(0, 2);
        if (((reuse_col_str_110 == "19") || (reuse_col_str_110 == "11") || (reuse_col_str_110 == "16") || (reuse_col_str_110 == "27") || (reuse_col_str_110 == "15") || (reuse_col_str_110 == "22") || (reuse_col_str_110 == "12") || (0)) && (_c_acctbal27 > tbl_Aggregate_TD_5490_output.getInt64(0, 0))) {
            int32_t _c_custkey22_t = tbl_SerializeFromObject_TD_5322_input.getInt32(i, 0);
            tbl_Filter_TD_4168_output.setInt32(r, 0, _c_custkey22_t);
            tbl_Filter_TD_4168_output.setInt32(r, 1, i);
            int32_t _c_acctbal27_t = tbl_SerializeFromObject_TD_5322_input.getInt32(i, 2);
            tbl_Filter_TD_4168_output.setInt32(r, 2, _c_acctbal27_t);
            r++;
        }
    }
    tbl_Filter_TD_4168_output.setNumRow(r);
    std::cout << "tbl_Filter_TD_4168_output #Row: " << tbl_Filter_TD_4168_output.getNumRow() << std::endl;
}

void SW_Consolidated_JOIN_LEFTANTI_TD_333_output(Table& in1, Table&in2, Table& tbl_JOIN_LEFTANTI_TD_333_output, Table& tbl_JOIN_LEFTANTI_TD_333_consolidated_output) {
    // StringRowIDSubstitution: true StringRowIDBackSubstitution: false
    // Supported operation: JOIN_LEFTANTI
    // Operation: ListBuffer((o_custkey#208 = c_custkey#22))
    // Left Table: ListBuffer(o_custkey#208)
    // Right Table: ListBuffer(c_custkey#22, c_phone#26, c_acctbal#27)
    // Output Table: ListBuffer(c_phone#26, c_acctbal#27)
    // Node Depth: 3
    int num_rows = tbl_JOIN_LEFTANTI_TD_333_output.getNumRow();
        for (int j = 0; j < num_rows; j++) {
          int32_t rowid_1 = tbl_JOIN_LEFTANTI_TD_333_output.getInt32(j, 0);
          int32_t rowid_2 = tbl_JOIN_LEFTANTI_TD_333_output.getInt32(j, 1);
          int64_t key = tbl_JOIN_LEFTANTI_TD_333_output.getInt32(j, 2);
          tbl_JOIN_LEFTANTI_TD_333_consolidated_output.setInt32(j, 0, in2.getInt32(rowid_1 - 1, 1));
          tbl_JOIN_LEFTANTI_TD_333_consolidated_output.setInt32(j, 1, in2.getInt32(rowid_1 - 1, 2));
        }
        tbl_JOIN_LEFTANTI_TD_333_consolidated_output.setNumRow(num_rows);
    std::cout << "tbl_JOIN_LEFTANTI_TD_333_consolidated_output #Row: " << tbl_JOIN_LEFTANTI_TD_333_consolidated_output.getNumRow() << std::endl;
}

void SW_Project_TD_2641(Table &tbl_JOIN_LEFTANTI_TD_333_consolidated_output, Table &tbl_SerializeFromObject_TD_5322_input, Table &tbl_Project_TD_2641_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: true
    // Supported operation: Project
    // Operation: ListBuffer(substring(c_phone#26, 1, 2) AS cntrycode#2035)
    // Input: ListBuffer(c_phone#26, c_acctbal#27)
    // Output: ListBuffer(cntrycode#2035, c_acctbal#27)
    int nrow1 = tbl_JOIN_LEFTANTI_TD_333_consolidated_output.getNumRow();
    for (int i = 0; i < nrow1; i++) {
        std::array<char, TPCH_READ_PHONE_LEN + 1> _c_phone26 = tbl_SerializeFromObject_TD_5322_input.getcharN<char, TPCH_READ_PHONE_LEN + 1>(tbl_JOIN_LEFTANTI_TD_333_consolidated_output.getInt32(i, 0), 1);
        int32_t _c_acctbal27 = tbl_JOIN_LEFTANTI_TD_333_consolidated_output.getInt32(i, 1);
        std::string _cntrycode2035_str = std::string(_c_phone26.data()).substr(0, 2);
        std::array<char, TPCH_READ_REGION_LEN + 1> _cntrycode2035{};
        memcpy(_cntrycode2035.data(), _cntrycode2035_str.data(), (_cntrycode2035_str).length());
        tbl_Project_TD_2641_output.setcharN<char, TPCH_READ_REGION_LEN + 1>(i, 0, _cntrycode2035);
        tbl_Project_TD_2641_output.setInt32(i, 1, _c_acctbal27);
    }
    tbl_Project_TD_2641_output.setNumRow(nrow1);
    std::cout << "tbl_Project_TD_2641_output #Row: " << tbl_Project_TD_2641_output.getNumRow() << std::endl;
}

typedef std::string SW_Aggregate_TD_117_key;
struct SW_Aggregate_TD_117_payload {
    int64_t _numcust2038L_count_0;
    int64_t _totacctbal2039L_sum_1;
};
void SW_Aggregate_TD_117(Table &tbl_Project_TD_2641_output, Table &tbl_Aggregate_TD_117_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: Aggregate
    // Operation: ListBuffer(cntrycode#2035, count(1) AS numcust#2038L, sum(c_acctbal#27) AS totacctbal#2039L)
    // Input: ListBuffer(cntrycode#2035, c_acctbal#27)
    // Output: ListBuffer(cntrycode#2035, numcust#2038L, totacctbal#2039L)
    std::unordered_map<SW_Aggregate_TD_117_key, SW_Aggregate_TD_117_payload> ht1;
    int nrow1 = tbl_Project_TD_2641_output.getNumRow();
    for (int i = 0; i < nrow1; i++) {
        std::array<char, TPCH_READ_REGION_LEN + 1> _cntrycode2035 = tbl_Project_TD_2641_output.getcharN<char, TPCH_READ_REGION_LEN + 1>(i, 0);
        int32_t _c_acctbal27 = tbl_Project_TD_2641_output.getInt32(i, 1);
        SW_Aggregate_TD_117_key k = std::string(_cntrycode2035.data());
        int64_t _numcust2038L_count_0 = 1 != 0 ? 1 : 0;
        int64_t _totacctbal2039L_sum_1 = _c_acctbal27;
        SW_Aggregate_TD_117_payload p{_numcust2038L_count_0, _totacctbal2039L_sum_1};
        auto it = ht1.find(k);
        if (it != ht1.end()) {
            int64_t count_0 = (it->second)._numcust2038L_count_0 + _numcust2038L_count_0;
            p._numcust2038L_count_0 = count_0;
            int64_t sum_1 = (it->second)._totacctbal2039L_sum_1 + _totacctbal2039L_sum_1;
            p._totacctbal2039L_sum_1 = sum_1;
            ht1[k] = p;
        } else { 
            ht1.insert(std::make_pair(k, p));
        }
    }
    int r = 0;
    for (auto& it : ht1) {
        std::array<char, TPCH_READ_REGION_LEN + 1> _cntrycode2035{};
        memcpy(_cntrycode2035.data(), (it.first).data(), (it.first).length());
        tbl_Aggregate_TD_117_output.setcharN<char, TPCH_READ_REGION_LEN + 1>(r, 0, _cntrycode2035);
        int64_t _numcust2038L = (it.second)._numcust2038L_count_0;
        tbl_Aggregate_TD_117_output.setInt64(r, 1, _numcust2038L);
        int64_t _totacctbal2039L = (it.second)._totacctbal2039L_sum_1;
        tbl_Aggregate_TD_117_output.setInt64(r, 2, _totacctbal2039L);
        ++r;
    }
    tbl_Aggregate_TD_117_output.setNumRow(r);
    std::cout << "tbl_Aggregate_TD_117_output #Row: " << tbl_Aggregate_TD_117_output.getNumRow() << std::endl;
}

void SW_Sort_TD_0370(Table &tbl_Aggregate_TD_117_output, Table &tbl_Sort_TD_0370_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: Sort
    // Operation: ListBuffer(cntrycode#2035 ASC NULLS FIRST)
    // Input: ListBuffer(cntrycode#2035, numcust#2038L, totacctbal#2039L)
    // Output: ListBuffer(cntrycode#2035, numcust#2038L, totacctbal#2039L)
    struct SW_Sort_TD_0370Row {
        std::string _cntrycode2035;
        int64_t _numcust2038L;
        int64_t _totacctbal2039L;
    }; 

    struct {
        bool operator()(const SW_Sort_TD_0370Row& a, const SW_Sort_TD_0370Row& b) const { return 
 (a._cntrycode2035 < b._cntrycode2035); 
}
    }SW_Sort_TD_0370_order; 

    int nrow1 = tbl_Aggregate_TD_117_output.getNumRow();
    std::vector<SW_Sort_TD_0370Row> rows;
    for (int i = 0; i < nrow1; i++) {
        std::array<char, TPCH_READ_REGION_LEN + 1> _cntrycode2035 = tbl_Aggregate_TD_117_output.getcharN<char, TPCH_READ_REGION_LEN +1>(i, 0);
        int64_t _numcust2038L = tbl_Aggregate_TD_117_output.getInt64(i, 1);
        int64_t _totacctbal2039L = tbl_Aggregate_TD_117_output.getInt64(i, 2);
        SW_Sort_TD_0370Row t = {std::string(_cntrycode2035.data()),_numcust2038L,_totacctbal2039L};
        rows.push_back(t);
    }
    std::sort(rows.begin(), rows.end(), SW_Sort_TD_0370_order);
    int r = 0;
    for (auto& it : rows) {
        std::array<char, TPCH_READ_REGION_LEN + 1> _cntrycode2035{};
        memcpy(_cntrycode2035.data(), (it._cntrycode2035).data(), (it._cntrycode2035).length());
        tbl_Sort_TD_0370_output.setcharN<char, TPCH_READ_REGION_LEN +1>(r, 0, _cntrycode2035);
        tbl_Sort_TD_0370_output.setInt64(r, 1, it._numcust2038L);
        tbl_Sort_TD_0370_output.setInt64(r, 2, it._totacctbal2039L);
        if (r < 10) {
            std::cout << (it._cntrycode2035).data() << " " << it._numcust2038L << " " << it._totacctbal2039L << " " << std::endl;
        }
        ++r;
    }
    tbl_Sort_TD_0370_output.setNumRow(r);
    std::cout << "tbl_Sort_TD_0370_output #Row: " << tbl_Sort_TD_0370_output.getNumRow() << std::endl;
}

