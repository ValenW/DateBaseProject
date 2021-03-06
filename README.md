# DateBaseProject
中山大学软件学院2013数据库课程设计

主要涉及可扩展哈希在数据库中的应用。

### 实现算法
可扩展哈希，算法实现分为两大部分，第一部分是建立索引，第二部分是查询。<br>
建立索引是将输入的每一条记录根据指定的键值放入合适的哈希桶内，当哈希桶已满时，需要进行分裂。<br>
查询是根据输入的键值返回具有相同键值的记录，返回的记录可能有不止一条。<br>

### 实现过程
1.读入由 tpc-h 生成的 lineitem.tbl，以 L_ORDERKEY 属性作为键值将记录放入合适的哈希桶内。<br>
2.读入测试文件 testinput.in 内的数据，数据中包含多个需要查询的键值。<br>
3.将通过键值查询得到的所有记录都输出到 testoutput.out 文件中。<br>

#### 详细内容可参看
[项目要求](https://github.com/ValenW/DateBaseProject/blob/master/课程设计.pdf)和[实验报告](https://github.com/ValenW/DateBaseProject/blob/master/report.pdf)

###TODOS:
- [x] **基本数据结构**
    - [x] 基本内容页
    - [x] 缓冲池
    - [x] 目录页
- [x] **算法实现**
    - [x] 低位哈希
    - [x] 高位哈希
    - [x] 时钟轮换
- [ ] **其他优化**
    - [x] 位运算优化
    - [ ] Win API优化
